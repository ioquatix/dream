//
//  Events/Source.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Source.h"

#include "Loop.h"

#include <fcntl.h>
#include <iostream>

#include <unistd.h>

#include "Logger.h"

namespace Dream
{
	namespace Events
	{
// MARK: -
// MARK: class NotificationSource

		NotificationSource::NotificationSource (CallbackT callback) : _callback(callback)
		{
		}

		void NotificationSource::process_events (Loop * event_loop, Event event)
		{
			if (event == NOTIFICATION)
				_callback(event_loop, this, event);
		}

		NotificationSource::~NotificationSource ()
		{
		}

		void stop_run_loop_callback (Loop * event_loop, NotificationSource * note, Event enent)
		{
			event_loop->stop();
		}

		Ref<NotificationSource> NotificationSource::stop_loop_notification ()
		{
			return new NotificationSource(stop_run_loop_callback);
		}

// MARK: -
// MARK: class TimerSource

		TimerSource::TimerSource (CallbackT callback, TimeT duration, bool repeats, bool strict) : _cancelled(false), _repeats(repeats), _strict(strict), _duration(duration), _callback(callback)
		{
		}

		TimerSource::~TimerSource ()
		{
		}

		void TimerSource::process_events (Loop * rl, Event events)
		{
			if (!_cancelled)
				_callback(rl, this, events);
		}

		bool TimerSource::repeats () const
		{
			if (_cancelled)
				return false;

			return _repeats;
		}

		TimeT TimerSource::next_timeout (const TimeT & last_timeout, const TimeT & current_time) const
		{
			// This means that TimerSource will attempt to "catch-up"
			//return last_timeout + _duration;

			// This means that TimerSource will process updates as is possible, and might drop
			// updates if they are in the past
			if (!_strict && last_timeout + _duration < current_time)
				return current_time;
			else
				return last_timeout + _duration;
		}

		void TimerSource::cancel ()
		{
			_cancelled = true;
		}

// MARK: -
// MARK: class IFileDescriptorSource

		void IFileDescriptorSource::debug_file_descriptor_flags(int fd)
		{
			LogBuffer log_buffer;

			int flags = fcntl(fd, F_GETFL);

			log_buffer << "Flags for #" << fd << ":";

			if (flags & O_NONBLOCK)
				log_buffer << " NONBLOCK";

			int access_mode = flags & O_ACCMODE;

			if (access_mode == O_RDONLY)
				log_buffer << " RDONLY";
			else if (access_mode == O_WRONLY)
				log_buffer << " WRONLY";
			else
				log_buffer << " RDWR";

			if (flags & O_APPEND)
				log_buffer << " APPEND";

			if (flags & O_CREAT)
				log_buffer << " CREATE";

			if (flags & O_TRUNC)
				log_buffer << " TRUNCATE";

			logger()->log(LOG_DEBUG, log_buffer);
		}

		void IFileDescriptorSource::set_will_block (bool value)
		{
			FileDescriptorT curfd = file_descriptor();

			if (value == false) {
				fcntl(curfd, F_SETFL, fcntl(curfd, F_GETFL) | O_NONBLOCK);
			} else {
				fcntl(curfd, F_SETFL, fcntl(curfd, F_GETFL) & ~O_NONBLOCK);
			}
		}

		bool IFileDescriptorSource::will_block ()
		{
			return !(fcntl(file_descriptor(), F_GETFL) & O_NONBLOCK);
		}

// MARK: -
// MARK: class FileDescriptorSource

		FileDescriptorSource::FileDescriptorSource (CallbackT callback, int fd) : _fd(fd), _callback(callback)
		{
		}

		FileDescriptorSource::~FileDescriptorSource ()
		{
		}

		void FileDescriptorSource::process_events (Loop * event_loop, Event events)
		{
			_callback(event_loop, this, events);
		}

		FileDescriptorT FileDescriptorSource::file_descriptor () const
		{
			return _fd;
		}

		Ref<FileDescriptorSource> FileDescriptorSource::for_standard_in (CallbackT callback)
		{
			return new FileDescriptorSource(callback, STDIN_FILENO);
		}

		Ref<FileDescriptorSource> FileDescriptorSource::for_standard_out (CallbackT callback)
		{
			return new FileDescriptorSource(callback, STDOUT_FILENO);
		}

		Ref<FileDescriptorSource> FileDescriptorSource::for_standard_error (CallbackT callback)
		{
			return new FileDescriptorSource(callback, STDERR_FILENO);
		}

// MARK: -
// MARK: class NotificationPipeSource

		NotificationPipeSource::NotificationPipeSource ()
		{
			int result = pipe(_filedes);
			DREAM_ASSERT(result == 0);
		}

		NotificationPipeSource::~NotificationPipeSource ()
		{
			close(_filedes[0]);
			close(_filedes[1]);
		}

		FileDescriptorT NotificationPipeSource::file_descriptor () const
		{
			// Read end
			return _filedes[0];
		}

		void NotificationPipeSource::notify_event_loop () const
		{
			// Send a byte down the pipe
			write(_filedes[1], "\0", 1);
		}

		void NotificationPipeSource::process_events (Loop * loop, Event event)
		{
			char buf[32];

			// Discard all notification bytes
			read(_filedes[0], &buf, 32);

			// Process urgent notifications
			loop->process_notifications();
		}
	}
}
