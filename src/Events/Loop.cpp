//
//  Events/Loop.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Loop.h"
#include "Console.h"
#include "Logger.h"
#include "Thread.h"

#include "../Core/Timer.h"

#include <iostream>
#include <limits>
#include <fcntl.h>
#include <unistd.h>

// PollFileDescriptorMonitor
#include <poll.h>

// KQueueFileDescriptorMonitor
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

namespace Dream
{
	namespace Events
	{

// MARK: mark -
// MARK: mark Helper Functions
		
		/// Returns the READ/WRITE events that may occur for a file descriptor.
		/// @returns READ_EVENT if file is open READ ONLY.
		/// @returns WRITE_EVENT if file is open WRITE ONLY.
		/// @returns READ_EVENT|WRITE_EVENT if file is READ/WRITE.
		int events_for_file_descriptor (int fd)
		{
			if (fd == STDIN_FILENO) return READ_READY;
			if (fd == STDOUT_FILENO || fd == STDERR_FILENO) return WRITE_READY;
			
			int file_mode = fcntl(fd, F_GETFL) & O_ACCMODE;
			int events;
			
			DREAM_ASSERT(file_mode != -1);
			
			switch(file_mode)
			{
				case O_RDONLY:
					events = READ_READY;
					break;
				case O_WRONLY:
					events = WRITE_READY;
					break;
				case O_RDWR:
					events = READ_READY|WRITE_READY;
					break;
				default:
					events = 0;
			}
			
			return events;
		}
		
// MARK: mark File Descriptor Monitor Implementations		
// MARK: mark -
		
		typedef std::set<Ref<IFileDescriptorSource>> FileDescriptorHandlesT;
		
		class KQueueFileDescriptorMonitor : public Object, implements IFileDescriptorMonitor
		{
		protected:
			FileDescriptorT _kqueue;
			std::set<FileDescriptorT> _removed_file_descriptors;
			
			FileDescriptorHandlesT _file_descriptor_handles;
			
		public:
			KQueueFileDescriptorMonitor ();
			virtual ~KQueueFileDescriptorMonitor ();
			
			virtual void add_source (Ptr<IFileDescriptorSource> source);
			virtual void remove_source (Ptr<IFileDescriptorSource> source);
			
			virtual int source_count () const;
			
			virtual int wait_for_events (TimeT timeout, Loop * loop);
		};
				
		KQueueFileDescriptorMonitor::KQueueFileDescriptorMonitor ()
		{
			_kqueue = kqueue();
		}

		KQueueFileDescriptorMonitor::~KQueueFileDescriptorMonitor ()
		{
			close(_kqueue);
		}
		
		void KQueueFileDescriptorMonitor::add_source (Ptr<IFileDescriptorSource> source)
		{
			FileDescriptorT fd = source->file_descriptor();
			_file_descriptor_handles.insert(source);
			
			int mode = events_for_file_descriptor(fd);
			
			struct kevent change[2];
			int c = 0;
			
			if (mode & READ_READY)
				EV_SET(&change[c++], fd, EVFILT_READ, EV_ADD, 0, 0, (void*)source.get());
				
			if (mode & WRITE_READY)
				EV_SET(&change[c++], fd, EVFILT_WRITE, EV_ADD, 0, 0, (void*)source.get());
					
			int result = kevent(_kqueue, change, c, NULL, 0, NULL);
					
			if (result == -1)
				perror(__PRETTY_FUNCTION__);
		}
		
		int KQueueFileDescriptorMonitor::source_count () const
		{
			return _file_descriptor_handles.size();
		}
		
		void KQueueFileDescriptorMonitor::remove_source (Ptr<IFileDescriptorSource> source)
		{
			FileDescriptorT fd = source->file_descriptor();
			
			struct kevent change[2];
			int c = 0;
			
			int mode = events_for_file_descriptor(fd);
			
			if (mode & READ_READY)
				EV_SET(&change[c++], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
				
			if (mode & WRITE_READY)
				EV_SET(&change[c++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
				
			int result = kevent(_kqueue, change, c, NULL, 0, NULL);
				
			if (result == -1)
				perror(__PRETTY_FUNCTION__);
					
			_removed_file_descriptors.insert(fd);
			_file_descriptor_handles.erase(source);
		}
		
		int KQueueFileDescriptorMonitor::wait_for_events (TimeT timeout, Loop * loop)
		{
			const unsigned KQUEUE_SIZE = 32;
			int count;
			
			struct kevent events[KQUEUE_SIZE];
			timespec kevent_timeout;
			
			_removed_file_descriptors.clear();
			
			if (timeout > 0.0) {
				kevent_timeout.tv_sec = timeout;
				kevent_timeout.tv_nsec = (timeout - kevent_timeout.tv_sec) * 1000000000;
			} else {
				kevent_timeout.tv_sec = 0;
				kevent_timeout.tv_nsec = 0;
			}

			if (timeout < 0)
				count = kevent(_kqueue, NULL, 0, events, KQUEUE_SIZE, NULL);
			else
				count = kevent(_kqueue, NULL, 0, events, KQUEUE_SIZE, &kevent_timeout);
			
			if (count == -1) {
				perror(__PRETTY_FUNCTION__);
			} else {
				for(unsigned i = 0; i < count; i += 1)
				{
					//std::cerr << this << " event[" << i << "] for fd: " << events[i].ident << " filter: " << events[i].filter << std::endl;
					
					// Discard events for descriptors which have already been removed
					if (_removed_file_descriptors.find(events[i].ident) != _removed_file_descriptors.end())
						continue;
					
					IFileDescriptorSource * s = (IFileDescriptorSource *)events[i].udata;
					
					if(events[i].flags & EV_ERROR) {
						logger()->log(LOG_ERROR, LogBuffer() << "Error processing fd: " << s->file_descriptor());
					}
					
					try {
						if (events[i].filter == EVFILT_READ)
							s->process_events(loop, READ_READY);
						
						if (events[i].filter == EVFILT_WRITE)
							s->process_events(loop, WRITE_READY);
					} catch (FileDescriptorClosed & ex) {
						remove_source(s);
					} catch (std::runtime_error & ex) {
						LogBuffer buffer;
						buffer << "Exception thrown by runloop " << this << ": " << ex.what() << std::endl;
						buffer << "Removing file descriptor " << s->file_descriptor() << "..." << std::endl;
						logger()->log(LOG_ERROR, buffer);
						
						remove_source(s);
					}
				}
			}
			
			return count;
		}
		
// MARK: mark -
		
		class PollFileDescriptorMonitor : public Object, implements IFileDescriptorMonitor
		{
		protected:
			// Used to provide O(1) delete time within process_events handler
			bool _delete_current_file_descriptor_handle;
			Ref<IFileDescriptorSource> _current_file_descriptor_source;
			
			FileDescriptorHandlesT _file_descriptor_handles;
			
		public:
			PollFileDescriptorMonitor ();
			virtual ~PollFileDescriptorMonitor ();
			
			virtual void add_source (Ptr<IFileDescriptorSource> source);
			virtual void remove_source (Ptr<IFileDescriptorSource> source);
			
			virtual int source_count () const;
			
			virtual int wait_for_events (TimeT timeout, Loop * loop);
		};
		
		PollFileDescriptorMonitor::PollFileDescriptorMonitor ()
		{
		}
		
		PollFileDescriptorMonitor::~PollFileDescriptorMonitor ()
		{	
		}
		
		void PollFileDescriptorMonitor::add_source (Ptr<IFileDescriptorSource> source)
		{
			_file_descriptor_handles.insert(source);
		}
		
		void PollFileDescriptorMonitor::remove_source (Ptr<IFileDescriptorSource> source)
		{
			if (_current_file_descriptor_source == source) {
				_delete_current_file_descriptor_handle = true;
			} else {
				_file_descriptor_handles.erase(source);
			}
		}
		
		int PollFileDescriptorMonitor::source_count () const
		{
			return _file_descriptor_handles.size();
		}
		
		int PollFileDescriptorMonitor::wait_for_events (TimeT timeout, Loop * loop)
		{
			// Number of events which have been processed
			int count;
			
			std::vector<FileDescriptorHandlesT::iterator> handles;
			std::vector<struct pollfd> pollfds;
			
			handles.reserve(_file_descriptor_handles.size());
			pollfds.reserve(_file_descriptor_handles.size());
			
			for(FileDescriptorHandlesT::iterator i = _file_descriptor_handles.begin(); i != _file_descriptor_handles.end(); i++) {
				struct pollfd pfd;
				
				pfd.fd = (*i)->file_descriptor();
				pfd.events = POLLIN|POLLOUT;
				
				handles.push_back(i);
				pollfds.push_back(pfd);
				
				//std::cerr << "Monitoring " << (*i)->class_name() << " fd: " << (*i)->file_descriptor() << std::endl;
			}
			
			int result = 0;
			
			if (timeout > 0.0) {
				// Convert timeout to milliseconds
				timeout = std::min(timeout, (TimeT)(std::numeric_limits<int>::max() / 1000));
				// Granularity of poll is not very good, so it might run through the loop multiple times
				// in order to reach a timeout. When timeout is less than 1ms, timeout * 1000 = 0, i.e.
				// non-blocking poll.
				result = poll(&pollfds[0], pollfds.size(), (timeout * 1000) + 1);
			} else if (timeout == 0) {
				result = poll(&pollfds[0], pollfds.size(), 0);
			} else {
				result = poll(&pollfds[0], pollfds.size(), -1);				
			}
			
			if (result < 0) {
				logger()->system_error("poll()");
			}
			
			_delete_current_file_descriptor_handle = false;
			
			if (result > 0) {
				for (unsigned i = 0; i < pollfds.size(); i += 1) {					
					int e = 0;
					
					if (pollfds[i].revents & POLLIN)
						e |= READ_READY;
					
					if (pollfds[i].revents & POLLOUT)
						e |= WRITE_READY;
					
					if (pollfds[i].revents & POLLNVAL) {
						logger()->log(LOG_ERROR, LogBuffer() << "Invalid file descriptor: " << pollfds[i].fd);
					}
					
					if (e == 0) continue;
					
					count += 1;
					_current_file_descriptor_source = *(handles[i]);
					
					try {
						_current_file_descriptor_source->process_events(loop, Event(e));
					} catch (std::runtime_error & ex) {
						//std::cerr << "Exception thrown by runloop " << this << ": " << ex.what() << std::endl;
						//std::cerr << "Removing file descriptor " << _current_file_descriptor_source->file_descriptor() << " ..." << std::endl;
						
						_delete_current_file_descriptor_handle = true;
					}
					
					if (_delete_current_file_descriptor_handle) {
						// O(1) delete time if deleting current handle
						_file_descriptor_handles.erase(handles[i]);
						_delete_current_file_descriptor_handle = false;
					}	
				}
			}
			
			return count;
		}
						
// MARK: mark -
// MARK: mark class Loop
				
		Loop::Loop () : _stop_when_idle(true), _rate_limit(20)
		{
			// Setup file descriptor monitor
			_file_descriptor_monitor = new KQueueFileDescriptorMonitor;
			
			// Setup timers
			_stopwatch.start();
			
			// Make sure stdin is a pipe
			reopen_standard_file_descriptors_as_pipes();
			
			// Create and open an urgent notification pipe
			_urgent_notification_pipe = new NotificationPipeSource;
			monitor(_urgent_notification_pipe);
		}
		
		Loop::~Loop ()
		{
			// Remove the internal urgent notification pipe
			//std::cerr << "Stop monitoring urgent notification pipe..." << std::endl;
			//stop_monitoring_file_descriptor(_urgent_notification_pipe);
			
			//foreach(Ref<IFileDescriptorSource> source, _file_descriptor_handles)
			//{
			//	std::cerr << "FD Still Scheduled: " << source->class_name() << source->file_descriptor() << std::endl;
			//}
		}
		
		void Loop::set_stop_when_idle (bool stop_when_idle)
		{
			_stop_when_idle = stop_when_idle;
		}
		
		const Stopwatch & Loop::stopwatch () const
		{
			return _stopwatch;
		}
		
// MARK: mark -
		
		/// Used to schedule a timer to the loop via a notification.
		class ScheduleTimerNotificationSource : public Object, implements INotificationSource
		{
			protected:
				Ref<ITimerSource> _timer_source;
			
			public:
				ScheduleTimerNotificationSource(Ref<ITimerSource> timer_source);
				virtual ~ScheduleTimerNotificationSource ();
				
				virtual void process_events (Loop * event_loop, Event event);
		};
		
		ScheduleTimerNotificationSource::ScheduleTimerNotificationSource(Ref<ITimerSource> timer_source)
			: _timer_source(timer_source)
		{
			
		}
		
		ScheduleTimerNotificationSource::~ScheduleTimerNotificationSource ()
		{
		
		}
		
		void ScheduleTimerNotificationSource::process_events (Loop * event_loop, Event event)
		{
			// Add the timer into the run-loop.
			if (event == NOTIFICATION)
				event_loop->schedule_timer(_timer_source);
		}
		
		void Loop::schedule_timer (Ref<ITimerSource> source)
		{			
			if (std::this_thread::get_id() == _current_thread) {
				TimerHandle th;
				
				TimeT current_time = _stopwatch.time();
				th.timeout = source->next_timeout(current_time, current_time);
				
				th.source = source;
				
				_timer_handles.push(th);
			} else {
				// Add the timer via a notification which is passed across the thread.
				Ref<ScheduleTimerNotificationSource> note = new ScheduleTimerNotificationSource(source);
				this->post_notification(note);
			}
		}
		
// MARK: mark -
		
		void Loop::post_notification (Ref<INotificationSource> note, bool urgent)
		{
			// Lock the event loop notification queue
			// Add note to the end of the queue
			// Interrupt event loop thread if urgent
			
			if (std::this_thread::get_id() == _current_thread) {
				note->process_events(this, NOTIFICATION);
			} else {
				{
					// Enqueue the notification to be processed
					std::lock_guard<std::mutex> lock(_notifications.lock);
					_notifications.sources.push(note);
				}
				
				if (urgent) {
					// Interrupt event loop thread so that it processes notifications more quickly
					_urgent_notification_pipe->notify_event_loop();
				}	
			}
		}
			
		void Loop::monitor (Ptr<IFileDescriptorSource> source)
		{
			DREAM_ASSERT(source->file_descriptor() != -1);
			//std::cerr << this << " monitoring fd: " << fd << std::endl;
			//IFileDescriptorSource::debug_file_descriptor_flags(fd);
			
			_file_descriptor_monitor->add_source(source);
		}
		
		void Loop::stop_monitoring_file_descriptor (Ptr<IFileDescriptorSource> source)
		{
			DREAM_ASSERT(source->file_descriptor() != -1);
			
			//std::cerr << this << " removing fd: " << fd << std::endl;
			//IFileDescriptorSource::debug_file_descriptor_flags(fd);
		
			_file_descriptor_monitor->remove_source(source);
		}
		
		/// If there is a timeout, returns true and the timeout in s.
		/// If there isn't a timeout, returns false and -1 in s.
		bool Loop::next_timeout (TimeT & s)
		{
			if (_timer_handles.empty()) {
				s = -1;
				return false;
			} else {
				s = _timer_handles.top().timeout - _stopwatch.time();
				return true;
			}
		}
		
		void Loop::stop ()
		{
			if (std::this_thread::get_id() != _current_thread)
			{
				post_notification(NotificationSource::stop_loop_notification(), true);	
			}
			else
			{
				_running = false;	
			}
		}
		
		Loop::Notifications::Notifications ()
		{
		
		}
		
		void Loop::Notifications::swap ()
		{
			// This function assumes that the structure has been locked..
			std::swap(sources, processing);
		}
		
		void Loop::process_notifications ()
		{		
			// Escape quickly - if this is not thread-safe, we still shouldn't have a problem unless
			// the data-structure itself gets corrupt, but this shouldn't be possible because empty() is const.
			// If we get a false positive, we still check below by locking the structure properly.
			if (_notifications.sources.empty())
				return;
						
			{
				std::lock_guard<std::mutex> lock(_notifications.lock);
				
				// Grab all pending notifications
				_notifications.swap();
			}
			
			unsigned rate = _rate_limit;
			
			while (!_notifications.processing.empty() && (rate-- || _rate_limit == 0))
			{
				Ref<INotificationSource> note = _notifications.processing.front();
				_notifications.processing.pop();
				
				note->process_events(this, NOTIFICATION);
			}
			
			if (rate == 0 && _rate_limit != 0) {
				logger()->log(LOG_WARN, LogBuffer() << "Warning: Notifications were rate limited! " << _notifications.processing.size() << " rescheduled notification(s)!");
				
				while (!_notifications.processing.empty())
				{
					Ref<INotificationSource> note = _notifications.processing.front();
					_notifications.processing.pop();
					
					post_notification(note, false);
				}
				
			}
		}
		
		TimeT Loop::process_timers ()
		{
			TimeT timeout;
			unsigned rate = _rate_limit;
			
			// It is the case that (we have a timeout) AND (we are within the rate limit OR we are not rate limiting)
			while(next_timeout(timeout) && timeout <= 0.0 && (rate-- || _rate_limit == 0))
			{
				// Check if the timeout is late.
				//if (timeout < -0.1)
				//	std::cerr << "Timeout was late: " << timeout << std::endl;
				
				TimerHandle th = _timer_handles.top();
				_timer_handles.pop();
				
				th.source->process_events(this, TIMEOUT);
				
				if (th.source->repeats()) {
					// Calculate the next time to schedule.
					th.timeout = th.source->next_timeout(th.timeout, _stopwatch.time());
					_timer_handles.push(th);
				}
			}
			
			if (rate == 0 && _rate_limit != 0) {
				logger()->log(LOG_WARN, "Warning: Timers were rate limited");
			}
			
			// There are timeouts that should have run, but didn't.
			if (timeout < 0)
				timeout = 0;
			
			return timeout;
		}
		
		void Loop::process_file_descriptors (TimeT timeout)
		{
			// Timeout is now the amount of time we have to process other events until another timeout will need to fire.
			if (_file_descriptor_monitor->source_count())
				_file_descriptor_monitor->wait_for_events(timeout, this);
			else if (timeout > 0.0)
				Core::sleep(timeout);
		}
		
		void Loop::run_one_iteration (bool use_timer_timeout, TimeT timeout)
		{
			TimeT time_until_next_timer_event = process_timers();
			
			// Process notifications before waiting for IO... [optional - reduce notification latency]
			process_notifications();
			
			// We have 1 "hidden" source: _urgent_notification_pipe..
			if (_stop_when_idle && _file_descriptor_monitor->source_count() == 1 && _timer_handles.size() == 0)
				stop();
			
			// A timer may have stopped the runloop. We should check here before we possibly block indefinitely.
			if (_running == false)
				return;
			
			// If the timeout specified was too big, we set it till the time the next event will occur, so that this function (will/should) be called again
			// shortly and process the timeout as appropriate.
			if (use_timer_timeout || timeout > time_until_next_timer_event)
				timeout = time_until_next_timer_event;
			
			process_file_descriptors(timeout);
			
			// Process any outstanding notifications after IO... [required]
			process_notifications();
		}
		
		void Loop::run_once (bool block)
		{
			_running = true;
			_current_thread = std::this_thread::get_id();
			
			run_one_iteration(false, block ? -1 : 0);
			
			_running = false;
		}
		
		void Loop::run_forever ()
		{			
			//std::cerr << "Entering runloop " << std::flush;
			_running = true;
			_current_thread = std::this_thread::get_id();
			//std::cerr << "..." << std::endl;
			
			while (_running) {
				run_one_iteration(true);
			}
		}
		
		TimeT Loop::run_until_timeout (TimeT timeout)
		{
			DREAM_ASSERT(timeout > 0);
			
			using Core::EggTimer;
			
			_running = true;
			_current_thread = std::this_thread::get_id();
			
			EggTimer timer(timeout);
			
			timer.start();
			while (_running && (timeout = timer.remaining_time()) > 0) {
				run_one_iteration(false, timeout);
			}
			
			return timer.remaining_time();
		}
		
// MARK: mark -
// MARK: mark Unit Tests
		
#ifdef ENABLE_TESTING
		int ticks;
		void ticker_callback (Loop * event_loop, TimerSource * ts, Event event)
		{
			if (ticks < 100)
				ticks += 1;
		}
		
		void timeout_callback (Loop * rl, TimerSource *, Event event)
		{
			std::cout << "Timeout callback @ " << rl->stopwatch().time() << std::endl;
		}
		
		static void stop_callback (Loop * event_loop, TimerSource *, Event event)
		{
			event_loop->stop();
		}
		
		void stdin_callback (Loop * rl, FileDescriptorSource *, Event event)
		{
			std::cout << "Stdin events: " << event << std::endl;
			
			if (event & READ_READY) {
				std::string s;
				std::cin >> s;
				
				std::cout << "Data read: " << s << std::endl;
			}
		}
		
		UNIT_TEST(TimerSource)
		{
			testing("Timer Sources");
			
			Ref<Loop> event_loop = new Loop;
			
			ticks = 0;
			
			event_loop->schedule_timer(new TimerSource(stop_callback, 1.1));
			event_loop->schedule_timer(new TimerSource(ticker_callback, 0.01, true));
			
			event_loop->monitor(FileDescriptorSource::for_standard_in(stdin_callback));
			
			event_loop->run_forever ();
			
			check(ticks == 100) << "Ticker callback called correctly";
			
			event_loop = new Loop;
			
			ticks = 0;
			
			event_loop->schedule_timer(new TimerSource(ticker_callback, 0.1, true));
			
			event_loop->run_until_timeout(1.01);
			
			check(ticks == 10) << "Ticker callback called correctly within specified timeout";
		}
		
		int notified;
		static void send_notification_after_delay (Ref<Loop> event_loop, Ref<INotificationSource> note)
		{
			int count = 0;
			while (count < 10)
			{
				Core::sleep(0.1);
				event_loop->post_notification(note);
				
				count += 1;
			}
			
			//event_loop->post_notification(note);
		}
		
		
		static void notification_received (Loop * rl, NotificationSource * note, Event event)
		{
			notified += 1;
			
			if (notified >= 10)
				rl->stop();
		}
		
		UNIT_TEST(Notification)
		{
			testing("Notification Sources");
			
			Ref<Loop> event_loop = new Loop;
			Ref<NotificationSource> note = new NotificationSource(notification_received);
			
			// Fail the test after 5 seconds if we are not notified.
			event_loop->schedule_timer(new TimerSource(stop_callback, 2));
			
			notified = 0;
			
			std::thread notification_thread(std::bind(send_notification_after_delay, event_loop, note));
			
			event_loop->run_forever();
			
			notification_thread.join();
			
			check(notified == 10) << "Notification occurred";
		}
		
		static void send_stop_after_delay (Ref<Loop> event_loop)
		{
			Core::sleep(0.5);
			event_loop->stop();
		}
		
		static bool timer_stopped;
		static void mark_and_stop_callback (Loop * event_loop, TimerSource *, Event event)
		{
			timer_stopped = true;
			event_loop->stop();
		}
		
		UNIT_TEST(EventLoopStop)
		{
			testing("Stopping from another thread");
			
			timer_stopped = false;
			
			Ref<Loop> event_loop = new Loop;
			event_loop->set_stop_when_idle(false);
			
			event_loop->schedule_timer(new TimerSource(mark_and_stop_callback, 1.0));
			
			std::thread stop_thread(std::bind(send_stop_after_delay, event_loop));
			
			// Will be stopped after 2 seconds from the above thread
			event_loop->run_forever();
			
			stop_thread.join();
			
			check(!timer_stopped) << "Thread stopped runloop";
		}
		
#endif
	}
}
