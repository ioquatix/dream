//
//  Events/Loop.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_EVENTS_LOOP_H
#define _DREAM_EVENTS_LOOP_H

#include "Events.h"
#include "Source.h"

#include <set>
#include <queue>

#include <functional>
#include <thread>
#include <mutex>

#ifdef BSD
#define DREAM_USE_KQUEUE
#else
#define DREAM_USE_POLL
#endif

namespace Dream
{
	namespace Events
	{

#pragma mark -
		
		class Loop;
		
		class IFileDescriptorMonitor : implements IObject
		{
		public:			
			/// Remove a source to be monitored
			virtual void add_source (PTR(IFileDescriptorSource) source) abstract;
			
			/// Add a source to be monitored
			virtual void remove_source (PTR(IFileDescriptorSource) source) abstract;
			
			/// Count of active file descriptors
			virtual int source_count () const abstract;
			
			/// Monitor sources for duration and handle any events that occur.
			/// If timeout >= 0, this call will return at least before this timeout
			/// If timeout == 0, this call does not block
			/// If timeout <= 0, this call blocks indefinitely
			virtual int wait_for_events (TimeT timeout, Loop * loop) abstract;
		};
		
		class FileDescriptorClosed
		{
		};
		
#pragma mark -
		
		/**
		 A run-loop to provide timed and io based event handling.
		 
		 Several different modes of operation:
		 - Wait until some event occurs (t == -1)
		 - Return as soon as possible (t == 0)
		 - Wait until a timeout is reached (t > 0)
		 
		 Some information is also generated
		 - Time until next timer event (dt)
		 */
		class Loop : public Object
		{
		private:
			REF(IFileDescriptorMonitor) _file_descriptor_monitor;
						
			friend class NotificationPipeSource;
			REF(NotificationPipeSource) _urgent_notification_pipe;
			
			void process_notifications ();
			
			struct Notifications
			{				
				typedef std::queue<REF(INotificationSource)> QueueT;
				
				Notifications ();
				void swap ();
				
				std::mutex lock;
				
				/// A queue of notifications that need to be processed
				QueueT sources;
				QueueT processing;
			};			
			
			Notifications _notifications;
			std::thread::id _current_thread;
			bool _running;
			
			struct TimerHandle
			{
				TimeT timeout;
				REF(ITimerSource) source;
				
				bool operator< (const TimerHandle & other) const
				{
					return timeout > other.timeout;
				}
			};
			
			typedef std::priority_queue<TimerHandle> TimerHandlesT;
			TimerHandlesT _timerHandles;
			
			bool next_timeout (TimeT &);
			
			/// Process any timer events that may be pending
			/// @returns the time until the next timeout if it exists
			/// @returns -1 if there are no further timeouts
			TimeT process_timers ();
			
			/// Process any file descriptors and their events.
			/// Timeout supplied as per IFileDescriptorMonitor::wait_for_events()
			void process_file_descriptors (TimeT timeout);
			
			Stopwatch _stopwatch;
		
		public:
			Loop ();
			~Loop ();
		
		protected:
			bool _stop_when_idle;
			unsigned _rate_limit;

			/// Runs one iteration of the loop.
			/// If user_timer_timeout is true, then the timeout is set to the value returned by process_timers() otherwise, the timeout supplied is used.
			/// @sa run_until_timeout()
			/// @sa IFileDescriptorMonitor::wait_for_events
			void run_one_iteration (bool use_timer_timeout, TimeT timeout = 0);
		public:
			/// Set whether once there are no longer IO or Timer sources, the runloop will stop automatically.
			void set_stop_when_idle (bool stop_when_idle = true);
			
			/// Sets whether or not timers and notifications can stall the loop. If a timer constantly schedules itself in the past, it will be called 
			/// repeatedly and possibly stall other timers and event notification sources. The same goes for notifications - if a notification reschedules 
			/// itself repeatedly. If you rate limit (which is the default) these types of situations will not cause the loop to stall. This function should
			/// generally be used to ensure a robust loop i.e. events are processed promptly.
			void set_rate_limit (unsigned rate = 10);
			
			/// This stopwatch is not thread-safe.
			const Stopwatch & stopwatch () const;
			
			/// Schedule a timer for periodic events. This function is thread-safe. If called from a spearate thread, the timer is added by sending
			/// an asynchronous notification. The timer will be run on the same thread as the loop, not the calling thread.
			void schedule_timer (REF(ITimerSource) source);
			
			/// This function performs a notification as soon as possible. This function is thread-safe. If called from a separate thread, it may block while it
			/// locks the notification queue. Also, it is okay for a notification to schedule another notification, but it possibly won't run until the next
			/// execution of the loop (with the current implementation, this is true in about 50% of cases as notifications are processed twice each run through
			/// the loop).
			void post_notification (REF(INotificationSource) note, bool urgent = false);
			
			/// Monitor a file descriptor and process any read/write events when it is possible to do so.
			/// This function is NOT thread-safe.
			void monitor (PTR(IFileDescriptorSource) source);
			
			/// Stop monitoring a file descriptor.
			/// This function is NOT thread-safe.
			void stop_monitoring_file_descriptor (PTR(IFileDescriptorSource) source);
			
			/// Stops the event loop. This function is thread-safe. If called from a separate thread, sends an urgent stop notification.
			void stop ();
			
			/// Run through the event loop once.
			void run_once (bool block);
			
			/// Run through the event loop until it is stopped.
			void run_forever ();
			
			/// Run the loop until a specific deadline. This function is fairly strict, and in the general case should return within the timeout specified.
			/// This function is designed to be used within other run-loops.
			/// This function is only valid when timeout is greater than 0. For timeouts less than or equal to 0, see run_once() or run_forever(). If you supply 
			/// a timeout <= 0, an exception will be thrown.
			/// The function will process the loop until the specified timeout has been reached. If the loop stops, it will return prematurely, and the result
			/// will be the remaining time.
			TimeT run_until_timeout (TimeT timeout);
		};
	}
}

#endif
