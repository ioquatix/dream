//
//  Events/Source.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_EVENTS_SOURCE_H
#define _DREAM_EVENTS_SOURCE_H

#include "Events.h"

namespace Dream
{
	namespace Events
	{
		class Loop;
		class ISource;
		
		class ISource : implements IObject
		{
			public:
				virtual void process_events (Loop *, Event) abstract;
		};
		
		class INotificationSource : implements ISource
		{
			public:
				virtual void process_events (Loop *, Event) abstract;
		};
		
		class NotificationSource : public Object, implements INotificationSource
		{
			typedef std::function<void (Loop *, NotificationSource *, Event)> CallbackT;
			
			protected:
				CallbackT _callback;
				
			public:
				/// This function is called from the main thread as soon as possible after notify() has been called.
				virtual void process_events (Loop *, Event);
				
				/// @todo loop should really be weak?
				NotificationSource (CallbackT callback);
				virtual ~NotificationSource ();
							
				static REF(NotificationSource) stop_loop_notification ();
		};
		
		class ITimerSource : implements ISource
		{
			public:
				virtual bool repeats () const abstract;
				virtual TimeT next_timeout (const TimeT & last_timeout, const TimeT & current_time) const abstract;
		};
		
		class TimerSource : public Object, implements ITimerSource
		{
			typedef std::function<void (Loop *, TimerSource *, Event)> CallbackT;

			protected:
				bool _cancelled, _repeats, _strict;
				TimeT _duration;
				CallbackT _callback;
				
			public:
				/// A strict timer attempts to fire callbacks even if they are in the past.
				/// A non-strict timer might drop events.
				TimerSource (CallbackT callback, TimeT duration, bool repeats = false, bool strict = false);
				~TimerSource ();
				
				virtual void process_events (Loop *, Event);
				
				virtual bool repeats () const;
				virtual TimeT next_timeout (const TimeT & last_timeout, const TimeT & current_time) const;
				
				void cancel ();
		};
		
		class IFileDescriptorSource : implements ISource
		{
			public:
				virtual FileDescriptorT file_descriptor () const abstract;
				
				/// Helper functions
				void set_will_block (bool value);
				bool will_block ();
				
				static void debug_file_descriptor_flags (int fd);
		};
		
		class FileDescriptorSource : public Object, implements IFileDescriptorSource
		{
			typedef std::function<void (Loop *, FileDescriptorSource *, Event)> CallbackT;
			
			protected:
				int _fd;
				CallbackT _callback;
				
			public:
				FileDescriptorSource(CallbackT callback, FileDescriptorT fd);
				virtual ~FileDescriptorSource ();
				
				virtual FileDescriptorT file_descriptor () const;
				
				virtual void process_events (Loop *, Event);
				
				static REF(FileDescriptorSource) for_standard_in (CallbackT);
				static REF(FileDescriptorSource) for_standard_out (CallbackT);
				static REF(FileDescriptorSource) for_standard_error (CallbackT);
		};
		
		/* Internal class used for processing urgent notifications
		 
		 */
		class NotificationPipeSource : public Object, implements IFileDescriptorSource
		{
			protected:
				int _filedes[2];
				
			public:
				NotificationPipeSource ();
				virtual ~NotificationPipeSource	();
				
				void notify_event_loop () const;
				
				virtual FileDescriptorT file_descriptor () const;
				virtual void process_events (Loop *, Event);
		};
		
	}
}

#endif
