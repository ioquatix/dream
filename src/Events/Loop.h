/*
 *  Events/Loop.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_EVENTS_LOOP_H
#define _DREAM_EVENTS_LOOP_H

#include "Events.h"
#include "Source.h"

#include <set>
#include <queue>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

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
			virtual void addSource (PTR(IFileDescriptorSource) source) abstract;
			
			/// Add a source to be monitored
			virtual void removeSource (PTR(IFileDescriptorSource) source) abstract;
			
			/// Count of active file descriptors
			virtual int sourceCount () const abstract;
			
			/// Monitor sources for duration and handle any events that occur.
			/// If timeout >= 0, this call will return at least before this timeout
			/// If timeout == 0, this call does not block
			/// If timeout <= 0, this call blocks indefinitely
			virtual int waitForEvents (TimeT timeout, Loop * loop) abstract;
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
			REF(IFileDescriptorMonitor) m_fileDescriptorMonitor;
						
			friend class NotificationPipeSource;
			REF(NotificationPipeSource) m_urgentNotificationPipe;
			
			void processNotifications ();
			
			struct Notifications
			{				
				typedef std::queue<REF(INotificationSource)> QueueT;
				
				Notifications ();
				void swap ();
				
				boost::mutex lock;
				
				/// A queue of notifications that need to be processed
				QueueT sources;
				QueueT processing;
			};			
			
			Notifications m_notifications;
			boost::thread::id m_currentThread;
			bool m_running;
			
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
			TimerHandlesT m_timerHandles;
			
			bool nextTimeout (TimeT &);
			
			/// Process any timer events that may be pending
			/// @returns the time until the next timeout if it exists
			/// @returns -1 if there are no further timeouts
			TimeT processTimers ();
			
			/// Process any file descriptors and their events.
			/// Timeout supplied as per IFileDescriptorMonitor::waitForEvents()
			void processFileDescriptors (TimeT timeout);
			
			Stopwatch m_stopwatch;
		
		public:
			Loop ();
			~Loop ();
		
		protected:
			bool m_stopWhenIdle;
			unsigned m_rateLimit;

			/// Runs one iteration of the loop.
			/// If userTimerTimeout is true, then the timeout is set to the value returned by processTimers() otherwise, the timeout supplied is used.
			/// @sa runUntilTimeout()
			/// @sa IFileDescriptorMonitor::waitForEvents
			void runOneIteration (bool useTimerTimeout, TimeT timeout = 0);
		public:
			/// Set whether once there are no longer IO or Timer sources, the runloop will stop automatically.
			void setStopWhenIdle (bool stopWhenIdle = true);
			
			/// Sets whether or not timers and notifications can stall the loop. If a timer constantly schedules itself in the past, it will be called 
			/// repeatedly and possibly stall other timers and event notification sources. The same goes for notifications - if a notification reschedules 
			/// itself repeatedly. If you rate limit (which is the default) these types of situations will not cause the loop to stall. This function should
			/// generally be used to ensure a robust loop i.e. events are processed promptly.
			void setRateLimit (unsigned rate = 10);
			
			const Stopwatch & stopwatch () const;
			
			/// Schedule a timer for periodic events.
			void scheduleTimer (REF(ITimerSource) source);
			
			/// This function performs a notification as soon as possible. This function is thread-safe. If called from a separate thread, it may block while it
			/// locks the notification queue. Also, it is okay for a notification to schedule another notification, but it possibly won't run until the next
			/// execution of the loop (with the current implementation, this is true in about 50% of cases as notifications are processed twice each run through
			/// the loop).
			void postNotification (REF(INotificationSource) note, bool urgent = false);
			
			/// Monitor a file descriptor and process any read/write events when it is possible to do so.
			void monitorFileDescriptor (PTR(IFileDescriptorSource) source);
			/// Stop monitoring a file descriptor.
			void stopMonitoringFileDescriptor (PTR(IFileDescriptorSource) source);
			
			/// Stops the event loop. This function is thread-safe. If called from a separate thread, sends an urgent stop notification.
			void stop ();
			
			/// Run through the event loop once
			void runOnce (bool block);
			
			/// Run through the event loop until it is stopped.
			void runForever ();
			
			/// Run the loop until a specific deadline. This function is fairly strict, and in the general case should return within the timeout specified.
			/// This function is designed to be used within other run-loops.
			/// This function is only valid when timeout is greater than 0. For timeouts less than or equal to 0, see runOnce() or runForever(). If you supply 
			/// a timeout <= 0, an exception will be thrown.
			/// The function will process the loop until the specified timeout has been reached. If the loop stops, it will return prematurely, and the result
			/// will be the remaining time.
			TimeT runUntilTimeout (TimeT timeout);
		};
	}
}

#endif