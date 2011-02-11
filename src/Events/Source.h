/*
 *  Events/Source.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_EVENTS_SOURCE_H
#define _DREAM_EVENTS_SOURCE_H

#include "Events.h"

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace Dream
{
	namespace Events
	{
		using boost::function;
		
		class Loop;
		class ISource;
		
		class ISource : IMPLEMENTS(Object)
		{
			EXPOSE_INTERFACE(Source)
			
			class Class : IMPLEMENTS(Object::Class)
			{
			};
			
			virtual void processEvents (Loop *, Event) abstract;
		};
		
		class INotificationSource : IMPLEMENTS(Source)
		{
			EXPOSE_INTERFACE(NotificationSource)
			
			class Class : IMPLEMENTS(Source::Class)
			{
			};
			
		public:
			virtual void processEvents (Loop *, Event) abstract;
		};
		
		class NotificationSource : public Object, IMPLEMENTS(NotificationSource)
		{
			EXPOSE_CLASS(NotificationSource)
			
			typedef boost::function<void (Loop *, NotificationSource *, Event)> CallbackT;
			
			class Class : public Object::Class, IMPLEMENTS(NotificationSource::Class)
			{
				EXPOSE_CLASSTYPE
				
				virtual REF(NotificationSource) init (CallbackT callback) const;
			};
			
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
		
		class ITimerSource : IMPLEMENTS(Source)
		{
			EXPOSE_INTERFACE(TimerSource)
			
			class Class : IMPLEMENTS(Source::Class)
			{
			};
			
		public:
			virtual bool repeats () const abstract;
			virtual TimeT nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const abstract;
		};
		
		class TimerSource : public Object, IMPLEMENTS(TimerSource)
		{
			EXPOSE_CLASS(TimerSource)

			typedef boost::function<void (Loop *, TimerSource *, Event)> CallbackT;
			
			class Class : public Object::Class, IMPLEMENTS(TimerSource::Class)
			{
				EXPOSE_CLASSTYPE
				
				virtual REF(TimerSource) init (CallbackT callback, TimeT duration, bool repeats = false) const;
			};
						
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
		
		class IFileDescriptorSource : IMPLEMENTS(Source)
		{
			EXPOSE_INTERFACE(FileDescriptorSource)
			
			class Class : IMPLEMENTS(Source::Class)
			{
			};
			
		public:
			virtual FileDescriptorT fileDescriptor () const abstract;
			
			/// Helper functions
			void setWillBlock (bool value);
			bool willBlock ();
			
			static void debugFileDescriptorFlags (int fd);
		};
		
		class FileDescriptorSource : public Object, IMPLEMENTS(FileDescriptorSource) {
			EXPOSE_CLASS(FileDescriptorSource)
			
			typedef boost::function<void (Loop *, FileDescriptorSource *, Event)> CallbackT;
			
			class Class : public Object::Class, IMPLEMENTS(FileDescriptorSource::Class) {
				EXPOSE_CLASSTYPE
			};
			
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
		class NotificationPipeSource : public Object, IMPLEMENTS(FileDescriptorSource)
		{
			EXPOSE_CLASS(NotificationPipeSource)
			
			class Class : public Object::Class, IMPLEMENTS(FileDescriptorSource::Class)
			{
				EXPOSE_CLASSTYPE
			};
			
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