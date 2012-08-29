//
//  Events/Console.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 13/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Console.h"

#include "Source.h"
#include "../Reference.h"

#include <thread>
#include <mutex>
#include <algorithm>
#include <unistd.h>

namespace Dream
{
	namespace Events
	{
		using namespace Core;

		class ConsolePipeRedirector : private NonCopyable {
			Shared<std::thread> _thread;
			std::mutex _setup_mutex;
			bool _done;

		public:
			ConsolePipeRedirector () : _done(false)
			{
			}

			static void copy_data_between_file_descriptors (int input, int output)
			{
				char buf[512];

				while (1) {
					ssize_t count = read(input, buf, 512);

					if (count < 0) {
						perror(__PRETTY_FUNCTION__);
						return;
					} else if (count == 0) {
						break;
					}

					write(output, buf, count);
				}
			}

			void reopen_standard_file_descriptors_as_pipes ()
			{
				{
					std::lock_guard<std::mutex> lock(_setup_mutex);

					if (_done) return;

					_done = true;
				}

				int result;
				int stdin_pipe[2]; //, stdout_pipe[2], stderr_pipe[2];

				// Create a new pipe for stdin data
				result = pipe(stdin_pipe);
				if (result) perror("pipe(stdin_pipe)");

				// Copy the stdin device to a new fd
				int stdin_device = dup(STDIN_FILENO);

				// Same again for stdout
				//result = pipe(stdout_pipe);
				//if (result) perror("pipe(stdout_pipe)");
				//int stdout_device = dup(STDOUT_FILENO);

				// Same again for stderr
				//result = pipe(stderr_pipe);
				//if (result) perror("pipe(stderr_pipe)");
				//int stderr_device = dup(STDERR_FILENO);

				// Copy the pipe endpoint into the standard place for in, out and err.
				dup2(stdin_pipe[0], STDIN_FILENO);
				//dup2(stdout_pipe[1], STDOUT_FILENO);
				//dup2(stderr_pipe[1], STDERR_FILENO);

				// Spawn threads to handle reading and writing in a blocking fashion.
				_thread = new std::thread(copy_data_between_file_descriptors, stdin_device, stdin_pipe[1]);
				//_threads.create_thread(bind(copy_data_between_file_descriptors, stdout_pipe[0], stdout_device));
				//_threads.create_thread(bind(copy_data_between_file_descriptors, stderr_pipe[0], stderr_device));
			}
		};

		void reopen_standard_file_descriptors_as_pipes ()
		{
			static ConsolePipeRedirector * s_redirector = NULL;

			if (s_redirector)
				return;

			s_redirector = new ConsolePipeRedirector;
			s_redirector->reopen_standard_file_descriptors_as_pipes();
		}
	}
}
