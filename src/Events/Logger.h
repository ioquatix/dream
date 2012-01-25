//
//  Logger.h
//  Dream
//
//  Created by Samuel Williams on 24/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_EVENTS_LOGGER_H
#define _DREAM_EVENTS_LOGGER_H

#include "Loop.h"

#include <thread>
#include <mutex>
#include <sstream>

namespace Dream
{
	namespace Events
	{
		/// Use this namespace for global logging facilities.
		namespace Logging {
			enum LogLevel {
				LOG_ERROR = 1,
				LOG_WARN = 2,
				LOG_INFO = 4,
				LOG_DEBUG = 8,
				
				LOG_ALL = (1 | 2 | 4 | 8)
			};
			
			typedef std::stringstream LogBuffer;
		}
		
		using namespace Logging;
		
		class Log;
		
		class Logger : public Object
		{
		protected:
			Timer _log_time;
			
			std::mutex _lock;
			
			LogLevel _log_level;
			FileDescriptorT _output;
			
			static const char * level_name(LogLevel level);
			
			void header(LogLevel level);
			
			/// Print out a logging header
			void start_session();
			
		public:
			Logger();
			virtual ~Logger();
			
			void log(LogLevel level, const std::string & message);
			void log(LogLevel level, const std::ostream & buffer);
			
			/// Provides perror like functionality
			void system_error(std::string message);
			
			void enable(LogLevel level);
			void disable(LogLevel level);
			
			void set_thread_name(std::string name);
			std::string thread_name() const;
		};
		
		namespace Logging {
			Logger * logger();
		};
	}
}

#endif
