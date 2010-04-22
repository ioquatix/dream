/*
 *  Events/Console.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/12/08.
 *  Copyright 2008 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Console.h"

#include "../Core/Singleton.h"
#include "Source.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>


namespace Dream
{
	namespace Events
	{
		using namespace boost;
		using namespace Core;
		
		class ConsolePipeRedirector {
			thread_group m_threads;
			mutex m_setupMutex;
			bool m_done;
			
		public:
			ConsolePipeRedirector () : m_done(false)
			{
				
			}
			
			static void copyDataBetweenFileDescriptors (int input, int output)
			{
				char buf[512];
				
				while(1) {
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
			
			void reopenStandardFileDescriptorsAsPipes ()
			{	
				{
					mutex::scoped_lock lock(m_setupMutex);
				
					if (m_done) return;
					m_done = true;
				}
				
				int result;
				int stdinPipe[2];//, stdoutPipe[2], stderrPipe[2];
				
				// Create a new pipe for stdin data
				result = pipe(stdinPipe);
				if (result) perror("pipe(stdinPipe)");
				// Copy the stdin device to a new fd
				int stdinDevice = dup(STDIN_FILENO);
				
				// Same again for stdout
				//result = pipe(stdoutPipe);
				//if (result) perror("pipe(stdoutPipe)");
				//int stdoutDevice = dup(STDOUT_FILENO);
				
				// Same again for stderr
				//result = pipe(stderrPipe);
				//if (result) perror("pipe(stderrPipe)");
				//int stderrDevice = dup(STDERR_FILENO);
				
				// Copy the pipe endpoint into the standard place for in, out and err.
				dup2(stdinPipe[0], STDIN_FILENO);
				//dup2(stdoutPipe[1], STDOUT_FILENO);
				//dup2(stderrPipe[1], STDERR_FILENO);
								
				// Spawn threads to handle reading and writing in a blocking fashion.
				m_threads.create_thread(bind(copyDataBetweenFileDescriptors, stdinDevice, stdinPipe[1]));
				//m_threads.create_thread(bind(copyDataBetweenFileDescriptors, stdoutPipe[0], stdoutDevice));
				//m_threads.create_thread(bind(copyDataBetweenFileDescriptors, stderrPipe[0], stderrDevice));
			}
		};
		
		Singleton<ConsolePipeRedirector> g_consolePipeRedirector;
		
		void reopenStandardFileDescriptorsAsPipes ()
		{
			g_consolePipeRedirector->reopenStandardFileDescriptorsAsPipes();
		}
	}
}