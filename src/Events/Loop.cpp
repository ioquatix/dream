/*
 *  Events/Loop.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Loop.h"
#include "Console.h"
#include "../Core/Timer.h"
#include "../Core/STLAdditions.h"

#include <iostream>
#include <limits>
#include <fcntl.h>

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

#pragma mark -
#pragma mark Helper Functions
		
		/// Returns the READ/WRITE events that may occur for a file descriptor.
		/// @returns READ_EVENT if file is open READ ONLY.
		/// @returns WRITE_EVENT if file is open WRITE ONLY.
		/// @returns READ_EVENT|WRITE_EVENT if file is READ/WRITE.
		int eventsForFileDescriptor (int fd)
		{
			if (fd == STDIN_FILENO) return READ_READY;
			if (fd == STDOUT_FILENO || fd == STDERR_FILENO) return WRITE_READY;
			
			int fileMode = fcntl(fd, F_GETFL) & O_ACCMODE;
			int events;
			
			ensure(fileMode != -1);
			
			switch(fileMode)
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
		
#pragma mark File Descriptor Monitor Implementations		
#pragma mark -
		
		IMPLEMENT_INTERFACE(FileDescriptorMonitor)
		
		typedef std::set<REF(IFileDescriptorSource)> FileDescriptorHandlesT;
		
		class KQueueFileDescriptorMonitor : public Object, IMPLEMENTS(FileDescriptorMonitor)
		{
			EXPOSE_CLASS(KQueueFileDescriptorMonitor)
			
			class Class : public Object::Class, IMPLEMENTS(FileDescriptorMonitor::Class)
			{
				EXPOSE_CLASSTYPE
			};
			
		protected:
			FileDescriptorT m_kqueue;
			std::set<FileDescriptorT> m_removedFileDescriptors;
			
			FileDescriptorHandlesT m_fileDescriptorHandles;
			
		public:
			KQueueFileDescriptorMonitor ();
			virtual ~KQueueFileDescriptorMonitor ();
			
			virtual void addSource (REF(IFileDescriptorSource) source);
			virtual void removeSource (REF(IFileDescriptorSource) source);
			
			virtual int sourceCount () const;
			
			virtual int waitForEvents (TimeT timeout, Loop * loop);
		};
		
		IMPLEMENT_CLASS(KQueueFileDescriptorMonitor)
		
		KQueueFileDescriptorMonitor::KQueueFileDescriptorMonitor ()
		{
			m_kqueue = kqueue();
		}

		KQueueFileDescriptorMonitor::~KQueueFileDescriptorMonitor ()
		{
			close(m_kqueue);
		}
		
		void KQueueFileDescriptorMonitor::addSource (REF(IFileDescriptorSource) source)
		{
			FileDescriptorT fd = source->fileDescriptor();
			m_fileDescriptorHandles.insert(source);
			
			int mode = eventsForFileDescriptor(fd);
			
			struct kevent change[2];
			int c = 0;
			
			if (mode & READ_READY)
				EV_SET(&change[c++], fd, EVFILT_READ, EV_ADD, 0, 0, (void*)source.get());
				
			if (mode & WRITE_READY)
				EV_SET(&change[c++], fd, EVFILT_WRITE, EV_ADD, 0, 0, (void*)source.get());
					
			int result = kevent(m_kqueue, change, c, NULL, 0, NULL);
					
			if (result == -1)
				perror(__PRETTY_FUNCTION__);
		}
		
		int KQueueFileDescriptorMonitor::sourceCount () const
		{
			return m_fileDescriptorHandles.size();
		}
		
		void KQueueFileDescriptorMonitor::removeSource (REF(IFileDescriptorSource) source)
		{
			FileDescriptorT fd = source->fileDescriptor();
			
			struct kevent change[2];
			int c = 0;
			
			int mode = eventsForFileDescriptor(fd);
			
			if (mode & READ_READY)
				EV_SET(&change[c++], fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
				
			if (mode & WRITE_READY)
				EV_SET(&change[c++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
				
			int result = kevent(m_kqueue, change, c, NULL, 0, NULL);
				
			if (result == -1)
				perror(__PRETTY_FUNCTION__);
					
			m_removedFileDescriptors.insert(fd);
			m_fileDescriptorHandles.erase(source);
		}		
		
		int KQueueFileDescriptorMonitor::waitForEvents (TimeT timeout, Loop * loop)
		{
			const unsigned KQUEUE_SIZE = 32;
			int count;
			
			struct kevent events[KQUEUE_SIZE];
			timespec keventTimeout;
			
			m_removedFileDescriptors.clear();
			
			if (timeout > 0.0) {
				keventTimeout.tv_sec = timeout;
				keventTimeout.tv_nsec = (timeout - keventTimeout.tv_sec) * 1000000000;
			} else {
				keventTimeout.tv_sec = 0;
				keventTimeout.tv_nsec = 0;
			}

			if (timeout < 0)
				count = kevent(m_kqueue, NULL, 0, events, KQUEUE_SIZE, NULL);
			else
				count = kevent(m_kqueue, NULL, 0, events, KQUEUE_SIZE, &keventTimeout);
			
			if (count == -1) {
				perror(__PRETTY_FUNCTION__);
			} else {
				for(unsigned i = 0; i < count; i += 1)
				{
					//std::cerr << this << " event[" << i << "] for fd: " << events[i].ident << " filter: " << events[i].filter << std::endl;
					
					// Discard events for descriptors which have already been removed
					if (m_removedFileDescriptors.find(events[i].ident) != m_removedFileDescriptors.end())
						continue;
					
					IFileDescriptorSource * s = (IFileDescriptorSource *)events[i].udata;
					
					if(events[i].flags & EV_ERROR)
					{
						std::cerr << "Error processing fd: " << s->fileDescriptor() << std::endl;
					}
					
					try {
						if (events[i].filter == EVFILT_READ)
							s->processEvents(loop, READ_READY);
						
						if (events[i].filter == EVFILT_WRITE)
							s->processEvents(loop, WRITE_READY);
					} catch (std::runtime_error & ex) {
						//std::cerr << "Exception thrown by runloop " << this << ": " << ex.what() << std::endl;
						//std::cerr << "Removing file descriptor " << s->fileDescriptor() << " ..." << std::endl;
						
						removeSource(s);
					}
				}
			}
			
			return count;
		}
		
#pragma mark -
		
		class PollFileDescriptorMonitor : public Object, IMPLEMENTS(FileDescriptorMonitor)
		{
			EXPOSE_CLASS(PollFileDescriptorMonitor)
			
			class Class : public Object::Class, IMPLEMENTS(FileDescriptorMonitor::Class)
			{
				EXPOSE_CLASSTYPE
			};
			
		protected:
			// Used to provide O(1) delete time within processEvents handler
			bool m_deleteCurrentFileDescriptorHandle;
			REF(IFileDescriptorSource) m_currentFileDescriptorSource;
			
			FileDescriptorHandlesT m_fileDescriptorHandles;
			
		public:
			PollFileDescriptorMonitor ();
			virtual ~PollFileDescriptorMonitor ();
			
			virtual void addSource (REF(IFileDescriptorSource) source);
			virtual void removeSource (REF(IFileDescriptorSource) source);
			
			virtual int sourceCount () const;
			
			virtual int waitForEvents (TimeT timeout, Loop * loop);
		};
		
		IMPLEMENT_CLASS(PollFileDescriptorMonitor)
		
		PollFileDescriptorMonitor::PollFileDescriptorMonitor ()
		{
		}
		
		PollFileDescriptorMonitor::~PollFileDescriptorMonitor ()
		{	
		}
		
		void PollFileDescriptorMonitor::addSource (REF(IFileDescriptorSource) source)
		{
			m_fileDescriptorHandles.insert(source);
		}
		
		void PollFileDescriptorMonitor::removeSource (REF(IFileDescriptorSource) source)
		{
			if (m_currentFileDescriptorSource == source) {
				m_deleteCurrentFileDescriptorHandle = true;
			} else {
				m_fileDescriptorHandles.erase(source);
			}
		}
		
		int PollFileDescriptorMonitor::sourceCount () const
		{
			return m_fileDescriptorHandles.size();
		}
		
		int PollFileDescriptorMonitor::waitForEvents (TimeT timeout, Loop * loop)
		{
			// Number of events which have been processed
			int count;
			
			std::vector<FileDescriptorHandlesT::iterator> handles;
			std::vector<struct pollfd> pollfds;
			
			handles.reserve(m_fileDescriptorHandles.size());
			pollfds.reserve(m_fileDescriptorHandles.size());
			
			for(FileDescriptorHandlesT::iterator i = m_fileDescriptorHandles.begin(); i != m_fileDescriptorHandles.end(); i++) {
				struct pollfd pfd;
				
				pfd.fd = (*i)->fileDescriptor();
				pfd.events = POLLIN|POLLOUT;
				
				handles.push_back(i);
				pollfds.push_back(pfd);
				
				//std::cerr << "Monitoring " << (*i)->className() << " fd: " << (*i)->fileDescriptor() << std::endl;
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
				std::cerr << "poll failed: " << result << std::endl;
				perror("poll");
			}
			
			m_deleteCurrentFileDescriptorHandle = false;
			
			if (result > 0) {
				for (unsigned i = 0; i < pollfds.size(); i += 1) {					
					int e = 0;
					
					if (pollfds[i].revents & POLLIN)
						e |= READ_READY;
					
					if (pollfds[i].revents & POLLOUT)
						e |= WRITE_READY;
					
					if (pollfds[i].revents & POLLNVAL)
						std::cerr << "Invalid file descriptor: " << pollfds[i].fd << std::endl;
					
					if (e == 0) continue;
					
					count += 1;
					m_currentFileDescriptorSource = *(handles[i]);
					
					try {
						m_currentFileDescriptorSource->processEvents(loop, Event(e));
					} catch (std::runtime_error & ex) {
						//std::cerr << "Exception thrown by runloop " << this << ": " << ex.what() << std::endl;
						//std::cerr << "Removing file descriptor " << m_currentFileDescriptorSource->fileDescriptor() << " ..." << std::endl;
						
						m_deleteCurrentFileDescriptorHandle = true;
					}
					
					if (m_deleteCurrentFileDescriptorHandle) {
						// O(1) delete time if deleting current handle
						m_fileDescriptorHandles.erase(handles[i]);
						m_deleteCurrentFileDescriptorHandle = false;
					}	
				}
			}
			
			return count;
		}
						
#pragma mark -
#pragma mark class Loop

		IMPLEMENT_CLASS(Loop)
		
		REF(Loop) Loop::Class::init ()
		{
			return ptr(new Loop);
		}
				
		Loop::Loop () : m_stopWhenIdle(true), m_rateLimit(20)
		{
			// Setup file descriptor monitor
			m_fileDescriptorMonitor = ptr(new KQueueFileDescriptorMonitor);
			
			// Setup timers
			m_stopwatch.start();
			
			// Make sure stdin is a pipe
			reopenStandardFileDescriptorsAsPipes();
			
			// Create and open an urgent notification pipe
			m_urgentNotificationPipe = ptr(new NotificationPipeSource);
			monitorFileDescriptor(m_urgentNotificationPipe);
		}
		
		Loop::~Loop ()
		{
			// Remove the internal urgent notification pipe
			//std::cerr << "Stop monitoring urgent notification pipe..." << std::endl;
			//stopMonitoringFileDescriptor(m_urgentNotificationPipe);
			
			//foreach(REF(IFileDescriptorSource) source, m_fileDescriptorHandles)
			//{
			//	std::cerr << "FD Still Scheduled: " << source->className() << source->fileDescriptor() << std::endl;
			//}
		}
		
		void Loop::setStopWhenIdle (bool stopWhenIdle)
		{
			m_stopWhenIdle = stopWhenIdle;
		}
		
		const Stopwatch & Loop::stopwatch () const
		{
			return m_stopwatch;
		}
		
		void Loop::scheduleTimer (REF(ITimerSource) source)
		{
			TimerHandle th;
			
			TimeT currentTime = m_stopwatch.time();
			th.timeout = source->nextTimeout(currentTime, currentTime);
			
			th.source = source;
			
			m_timerHandles.push(th);
		}
		
		void Loop::postNotification (REF(INotificationSource) note, bool urgent)
		{
			using namespace boost;
			typedef boost::mutex::scoped_lock scoped_lock;
		
			// Lock the event loop notification queue
			// Add note to the end of the queue
			// Interrupt event loop thread if urgent
			
			if (this_thread::get_id() == m_currentThread) {
				note->processEvents(this, NOTIFICATION);
			} else {
				{
					// Enqueue the notification to be processed
					scoped_lock lock(m_notifications.lock);
					m_notifications.sources->push(note);
				}
				
				if (urgent) {
					// Interrupt event loop thread so that it processes notifications more quickly
					m_urgentNotificationPipe->notifyEventLoop();
				}	
			}
		}
			
		void Loop::monitorFileDescriptor (REF(IFileDescriptorSource) source)
		{
			ensure(source->fileDescriptor() != -1);
			//std::cerr << this << " monitoring fd: " << fd << std::endl;
			//IFileDescriptorSource::debugFileDescriptorFlags(fd);
			
			m_fileDescriptorMonitor->addSource(source);
		}
		
		void Loop::stopMonitoringFileDescriptor (REF(IFileDescriptorSource) source)
		{
			ensure(source->fileDescriptor() != -1);
			
			//std::cerr << this << " removing fd: " << fd << std::endl;
			//IFileDescriptorSource::debugFileDescriptorFlags(fd);
		
			m_fileDescriptorMonitor->removeSource(source);
		}
		
		/// If there is a timeout, returns true and the timeout in s.
		/// If there isn't a timeout, returns false and -1 in s.
		bool Loop::nextTimeout (TimeT & s)
		{
			if (m_timerHandles.empty()) {
				s = -1;
				return false;
			} else {
				s = m_timerHandles.top().timeout - m_stopwatch.time();
				return true;
			}
		}
		
		void Loop::stop ()
		{
			if (boost::this_thread::get_id() != m_currentThread)
			{
				postNotification(NotificationSource::stopLoopNotification(), true);	
			}
			else
			{
				m_running = false;	
			}
		}
		
		Loop::Notifications::Notifications ()
		{
			sources.reset(new QueueT);
			processing.reset(new QueueT);
		}
		
		void Loop::Notifications::swap ()
		{
			// This function assumes that the structure has been locked..
			sources.swap(processing);
		}
		
		void Loop::processNotifications ()
		{
			typedef boost::mutex::scoped_lock scoped_lock;		
		
			// Escape quickly - if this is not thread-safe, we still shouldn't have a problem unless
			// the data-structure itself gets corrupt, but this shouldn't be possible because empty() is const.
			// If we get a false positive, we still check below by locking the structure properly.
			if (m_notifications.sources->empty())
				return;
						
			{
				scoped_lock lock(m_notifications.lock);
				
				// Grab all pending notifications
				m_notifications.swap();
			}
			
			unsigned rate = m_rateLimit;
			
			while (!m_notifications.processing->empty()  && (rate-- || m_rateLimit == 0))
			{
				REF(INotificationSource) note = m_notifications.processing->front();
				m_notifications.processing->pop();
				
				note->processEvents(this, NOTIFICATION);
			}
			
			if (rate == 0 && m_rateLimit != 0) {
				std::cerr << "Warning: Notifications were rate limited! " << m_notifications.processing->size() << " rescheduled notification(s)!" << std::endl;
				
				while (!m_notifications.processing->empty())
				{
					REF(INotificationSource) note = m_notifications.processing->front();
					m_notifications.processing->pop();
					
					postNotification(note, false);
				}
				
			}
		}
		
		TimeT Loop::processTimers ()
		{
			TimeT timeout;
			unsigned rate = m_rateLimit;
			
			// It is the case that (we have a timeout) AND (we are within the rate limit OR we are not rate limiting)
			while(nextTimeout(timeout) && timeout <= 0.0 && (rate-- || m_rateLimit == 0))
			{
				// Check if the timeout is late.
				if (timeout < -0.1)
					std::cerr << "Timeout was late: " << timeout << std::endl;
				
				TimerHandle th = m_timerHandles.top();
				m_timerHandles.pop();
				
				th.source->processEvents(this, TIMEOUT);
				
				if (th.source->repeats()) {
					// Calculate the next time to schedule.
					th.timeout = th.source->nextTimeout(th.timeout, m_stopwatch.time());
					m_timerHandles.push(th);
				}
			}
			
			if (rate == 0 && m_rateLimit != 0)
				std::cerr << "Warning: Timers were rate limited" << std::endl;
			
			return timeout;
		}
		
		void Loop::processFileDescriptors (TimeT timeout)
		{
			// Timeout is now the amount of time we have to process other events until another timeout will need to fire.
			if (m_fileDescriptorMonitor->sourceCount())
				m_fileDescriptorMonitor->waitForEvents(timeout, this);
			else if (timeout > 0.0)
				Core::sleep(timeout);
		}
		
		void Loop::runOneIteration (bool useTimerTimeout, TimeT timeout)
		{
			TimeT timeUntilNextTimerEvent = processTimers();
			
			// Process notifications before waiting for IO... [optional - reduce notification latency]
			processNotifications();
			
			// We have 1 "hidden" source: m_urgentNotificationPipe..
			if (m_stopWhenIdle && m_fileDescriptorMonitor->sourceCount() == 1 && m_timerHandles.size() == 0)
				stop();
			
			// A timer may have stopped the runloop. We should check here before we possibly block indefinitely.
			if (m_running == false)
				return;
			
			// If the timeout specified was too big, we set it till the time the next event will occur, so that this function (will/should) be called again
			// shortly and process the timeout as appropriate.
			if (useTimerTimeout || timeout > timeUntilNextTimerEvent)
				timeout = timeUntilNextTimerEvent;
			
			processFileDescriptors(timeout);
			
			// Process any outstanding notifications after IO... [required]
			processNotifications();
		}
		
		void Loop::runOnce (bool block)
		{
			using namespace boost;
			
			m_running = true;
			m_currentThread = this_thread::get_id();
			
			runOneIteration(false, block ? -1 : 0);
			
			m_running = false;
		}
		
		void Loop::runForever ()
		{
			using namespace boost;
				
			m_running = true;
			m_currentThread = this_thread::get_id();
				
			while(m_running)
			{
				runOneIteration(true);
			}
		}
		
		TimeT Loop::runUntilTimeout (TimeT timeout)
		{
			ensure(timeout > 0);
			
			using namespace boost;
			using Core::EggTimer;
			
			m_running = true;
			m_currentThread = this_thread::get_id();
			
			EggTimer timer(timeout);
			
			timer.start();
			while(m_running && (timeout = timer.remainingTime()) > 0)
			{
				runOneIteration(false, timeout);
			}
			
			return timer.remainingTime();
		}
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		int ticks;
		void tickerCallback (Loop * eventLoop, TimerSource * ts, Event event)
		{
			if (ticks < 100)
				ticks += 1;
		}
		
		void timeoutCallback (Loop * rl, TimerSource *, Event event)
		{
			std::cout << "Timeout callback @ " << rl->stopwatch().time() << std::endl;
		}
		
		static void stopCallback (Loop * eventLoop, TimerSource *, Event event)
		{
			eventLoop->stop();
		}
		
		void stdinCallback (Loop * rl, FileDescriptorSource *, Event event)
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
			
			REF(Loop) eventLoop = Loop::klass.init();
			
			ticks = 0;
			
			eventLoop->scheduleTimer(TimerSource::klass.init(stopCallback, 1.1));
			eventLoop->scheduleTimer(TimerSource::klass.init(tickerCallback, 0.01, true));
			
			eventLoop->monitorFileDescriptor(FileDescriptorSource::forStandardIn(stdinCallback));
			
			eventLoop->runForever ();
			
			assertEqual(ticks, 100, "Ticker callback called correctly");
			
			eventLoop = Loop::klass.init();
			
			ticks = 0;
			
			eventLoop->scheduleTimer(TimerSource::klass.init(tickerCallback, 0.1, true));
			
			eventLoop->runUntilTimeout(1.01);
			
			assertEqual(ticks, 10, "Ticker callback called correctly within specified timeout");
		}
		
		int notified;
		static void sendNotificationAfterDelay (REF(Loop) eventLoop, REF(NotificationSource) note)
		{
			int count = 0;
			while (count < 10)
			{
				Core::sleep(0.1);
				eventLoop->postNotification(note);
				
				count += 1;
			}
			
			//eventLoop->postNotification(note);
		}
		
		
		static void notificationReceived (Loop * rl, NotificationSource * note, Event event)
		{
			notified += 1;
			
			if (notified >= 10)
				rl->stop();
		}
		
		UNIT_TEST(NotificationSource)
		{
			using namespace boost;
			
			testing("Notification Sources");
			
			REF(Loop) eventLoop = Loop::klass.init();
			REF(NotificationSource) note = NotificationSource::klass.init(notificationReceived);
			
			// Fail the test after 5 seconds if we are not notified.
			eventLoop->scheduleTimer(TimerSource::klass.init(stopCallback, 2));
			
			notified = 0;
			
			thread_group children;
			children.create_thread(bind(sendNotificationAfterDelay, eventLoop, note));
			
			eventLoop->runForever();
			
			assertEqual(notified, 10, "Notification occurred");
		}
		
		static void sendStopAfterDelay (REF(Loop) eventLoop)
		{
			Core::sleep(0.5);
			eventLoop->stop();
		}
		
		static bool timerStopped;
		static void markAndStopCallback (Loop * eventLoop, TimerSource *, Event event)
		{
			timerStopped = true;
			eventLoop->stop();
		}
		
		UNIT_TEST(EventLoopStop)
		{
			typedef boost::thread_group ThreadGroup;
			testing("Stopping from another thread");
			
			timerStopped = false;
			
			REF(Loop) eventLoop = Loop::klass.init();
			eventLoop->setStopWhenIdle(false);
			
			eventLoop->scheduleTimer(TimerSource::klass.init(markAndStopCallback, 1.0));
			
			ThreadGroup children;
			children.create_thread(bind(sendStopAfterDelay, eventLoop));
			
			// Will be stopped after 2 seconds from the above thread
			eventLoop->runForever();
			
			assertFalse(timerStopped, "Thread stopped runloop");
		}
		
#endif
	}
}