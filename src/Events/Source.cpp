/*
 *  Events/Source.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Source.h"

#include "Loop.h"

#include <fcntl.h>
#include <iostream>

#include <unistd.h>

namespace Dream
{
	namespace Events
	{
		
#pragma mark -
#pragma mark class NotificationSource
		
		NotificationSource::NotificationSource (CallbackT callback) : m_callback(callback)
		{
			
		}
		
		void NotificationSource::processEvents (Loop * eventLoop, Event event)
		{
			if (event == NOTIFICATION)
				m_callback(eventLoop, this, event);
		}
		
		NotificationSource::~NotificationSource ()
		{
			
		}
		
		void stopRunLoopCallback (Loop * eventLoop, NotificationSource * note, Event enent)
		{
			eventLoop->stop();
		}
		
		REF(NotificationSource) NotificationSource::stopLoopNotification ()
		{
			return new NotificationSource(stopRunLoopCallback);
		}
		
#pragma mark -
#pragma mark class TimerSource
				
		TimerSource::TimerSource (CallbackT callback, TimeT duration, bool repeats, bool strict) 
			: m_repeats(repeats), m_strict(strict), m_duration(duration), m_callback(callback), m_cancelled(false)
		{
			
		}
		
		TimerSource::~TimerSource ()
		{
			
		}
		
		void TimerSource::processEvents (Loop * rl, Event events)
		{
			if (!m_cancelled)
				m_callback(rl, this, events);
		}
		
		bool TimerSource::repeats () const
		{
			if (m_cancelled)
				return false;
			
			return m_repeats;
		}
		
		TimeT TimerSource::nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const
		{
			// This means that TimerSource will attempt to "catch-up"
			//return lastTimeout + m_duration;
			
			// This means that TimerSource will process updates as is possible, and might drop
			// updates if they are in the past
			if (!m_strict && lastTimeout + m_duration < currentTime)
				return currentTime;
			else
				return lastTimeout + m_duration;
		}
		
		void TimerSource::cancel ()
		{
			m_cancelled = true;
		}
		
#pragma mark -
#pragma mark class IFileDescriptorSource
		
		void IFileDescriptorSource::debugFileDescriptorFlags (int fd)
		{
			using namespace std;
			
			int flags = fcntl(fd, F_GETFL);
			
			cout << "Flags for #" << fd << ":";
			
			if (flags & O_NONBLOCK)
				cout << " NONBLOCK";
			
			int accessMode = flags & O_ACCMODE;
			
			if (accessMode == O_RDONLY)
				cout << " RDONLY";
			else if (accessMode == O_WRONLY)
				cout << " WRONLY";
			else
				cout << " RDWR";

			if (flags & O_APPEND)
				cout << " APPEND";
			
			if (flags & O_CREAT)
				cout << " CREATE";
			
			if (flags & O_TRUNC)
				cout << " TRUNCATE";
			
			cout << endl;
		}
		
		void IFileDescriptorSource::setWillBlock (bool value)
		{
			FileDescriptorT curfd = fileDescriptor();
			
			if (value == false) {
				fcntl(curfd, F_SETFL, fcntl(curfd, F_GETFL) | O_NONBLOCK);
			} else {
				fcntl(curfd, F_SETFL, fcntl(curfd, F_GETFL) & ~O_NONBLOCK);
			}
		}
		
		bool IFileDescriptorSource::willBlock ()
		{
			return !(fcntl(fileDescriptor(), F_GETFL) & O_NONBLOCK);
		}
		
#pragma mark -
#pragma mark class FileDescriptorSource
		
		FileDescriptorSource::FileDescriptorSource (CallbackT callback, int fd) : m_callback(callback), m_fd(fd) 
		{
			
		}
		
		FileDescriptorSource::~FileDescriptorSource ()
		{
			
		}
		
		void FileDescriptorSource::processEvents (Loop * eventLoop, Event events)
		{
			m_callback(eventLoop, this, events);
		}
		
		FileDescriptorT FileDescriptorSource::fileDescriptor () const
		{
			return m_fd;
		}
		
		REF(FileDescriptorSource) FileDescriptorSource::forStandardIn (CallbackT callback)
		{
			return new FileDescriptorSource(callback, STDIN_FILENO);
		}
		
		REF(FileDescriptorSource) FileDescriptorSource::forStandardOut (CallbackT callback)
		{
			return new FileDescriptorSource(callback, STDOUT_FILENO);
		}
		
		REF(FileDescriptorSource) FileDescriptorSource::forStandardError (CallbackT callback)
		{
			return new FileDescriptorSource(callback, STDERR_FILENO);			
		}
		
#pragma mark -
#pragma mark class NotificationPipeSource
		
		NotificationPipeSource::NotificationPipeSource ()
		{
			int result = pipe(m_filedes);
			ensure(result == 0);
		}
		
		NotificationPipeSource::~NotificationPipeSource ()
		{
			close(m_filedes[0]);
			close(m_filedes[1]);
		}
		
		FileDescriptorT NotificationPipeSource::fileDescriptor () const
		{
			// Read end
			return m_filedes[0];
		}
		
		void NotificationPipeSource::notifyEventLoop () const
		{
			// Send a byte down the pipe
			write(m_filedes[1], "\0", 1);
		}
		
		void NotificationPipeSource::processEvents (Loop * loop, Event event)
		{
			char buf[32];
			
			// Discard all notification bytes
			read(m_filedes[0], &buf, 32);
			
			// Process urgent notifications
			loop->processNotifications();
		}
		
		
	}
}