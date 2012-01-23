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
				virtual void processEvents (Loop *, Event) abstract;
		};
		
		class INotificationSource : implements ISource
		{
			public:
				virtual void processEvents (Loop *, Event) abstract;
		};
		
		class NotificationSource : public Object, implements INotificationSource
		{
			typedef std::function<void (Loop *, NotificationSource *, Event)> CallbackT;
			
			protected:
				CallbackT m_callback;
				
			public:
				/// This function is called from the main thread as soon as possible after notify() has been called.
				virtual void processEvents (Loop *, Event);
				
				/// @todo loop should really be weak?
				NotificationSource (CallbackT callback);
				virtual ~NotificationSource ();
							
				static REF(NotificationSource) stopLoopNotification ();
		};
		
		class ITimerSource : implements ISource
		{
			public:
				virtual bool repeats () const abstract;
				virtual TimeT nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const abstract;
		};
		
		class TimerSource : public Object, implements ITimerSource
		{
			typedef std::function<void (Loop *, TimerSource *, Event)> CallbackT;

			protected:
				bool m_cancelled, m_repeats, m_strict;
				TimeT m_duration;
				CallbackT m_callback;
				
			public:
				/// A strict timer attempts to fire callbacks even if they are in the past.
				/// A non-strict timer might drop events.
				TimerSource (CallbackT callback, TimeT duration, bool repeats = false, bool strict = false);
				~TimerSource ();
				
				virtual void processEvents (Loop *, Event);
				
				virtual bool repeats () const;
				virtual TimeT nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const;
				
				void cancel ();
		};
		
		class IFileDescriptorSource : implements ISource
		{
			public:
				virtual FileDescriptorT fileDescriptor () const abstract;
				
				/// Helper functions
				void setWillBlock (bool value);
				bool willBlock ();
				
				static void debugFileDescriptorFlags (int fd);
		};
		
		class FileDescriptorSource : public Object, implements IFileDescriptorSource
		{
			typedef std::function<void (Loop *, FileDescriptorSource *, Event)> CallbackT;
			
			protected:
				int m_fd;
				CallbackT m_callback;
				
			public:
				FileDescriptorSource(CallbackT callback, FileDescriptorT fd);
				virtual ~FileDescriptorSource ();
				
				virtual FileDescriptorT fileDescriptor () const;
				
				virtual void processEvents (Loop *, Event);
				
				static REF(FileDescriptorSource) forStandardIn (CallbackT);
				static REF(FileDescriptorSource) forStandardOut (CallbackT);
				static REF(FileDescriptorSource) forStandardError (CallbackT);
		};
		
		/* Internal class used for processing urgent notifications
		 
		 */
		class NotificationPipeSource : public Object, implements IFileDescriptorSource
		{
			protected:
				int m_filedes[2];
				
			public:
				NotificationPipeSource ();
				virtual ~NotificationPipeSource	();
				
				void notifyEventLoop () const;
				
				virtual FileDescriptorT fileDescriptor () const;
				virtual void processEvents (Loop *, Event);
		};
		
	}
}

#endif
