//
//  Logger.cpp
//  Dream
//
//  Created by Samuel Williams on 24/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Logger.h"
#include "Thread.h"

#include <thread>
#include <sstream>
#include <map>
#include <iomanip>
#include <time.h>

namespace Dream
{
	namespace Events
	{
		
		const char * Logger::level_name(LogLevel level)
		{
			switch (level) {
				case LOG_ERROR:
					return "ERROR";
					
				case LOG_WARN:
					return "WARN";
					
				case LOG_INFO:
					return "INFO";
					
				case LOG_DEBUG:
					return "DEBUG";
					
				default:
					return "UNKNOWN";
			}
		}
		
		void Logger::start_session()
		{
			// Print out the current time
			time_t clock_time;
			time(&clock_time);
			
			struct tm current_local_time;
			localtime_r(&clock_time, &current_local_time);
			
			const std::size_t MAX_LENGTH = 1024;
			char date_buffer[MAX_LENGTH];
			strftime(date_buffer, MAX_LENGTH, "%c", &current_local_time);
			
			LogBuffer log_buffer;
			log_buffer << "Logging session started at " << date_buffer;
			log(LOG_INFO, log_buffer);
		}
		
		Logger::Logger() : _output(std::cerr), _log_level(LOG_ALL)
		{
			_log_time.reset();
			
			start_session();
		}
		
		Logger::~Logger()
		{
		}
		
		void Logger::header(LogLevel level) {
			_output << "[" 
				<< std::setfill(' ') << std::setw(8) << _log_time.time() << "; " 
				/* << std::setw(18) */ << thread_name() << " " 
				/* << std::setw(5) */ << level_name(level) << "] ";
		}
				
		void Logger::log(LogLevel level, std::string message)
		{
			if (level & _log_level) {
				std::lock_guard<std::mutex> lock(_lock);
				
				header(level);
				
				_output << message << std::endl;
			}
		}
		
		void Logger::log(LogLevel level, std::ostream & buffer)
		{
			if (level & _log_level) {
				std::lock_guard<std::mutex> lock(_lock);
				
				header(level);
				
				_output << buffer.rdbuf() << std::endl;
			}
		}
		
		void Logger::system_error(std::string message)
		{
			std::string error_description = system_error_description(errno);
			
			log(LOG_ERROR, LogBuffer() << "System error: " << message << ": " << error_description);
		}
		
		void Logger::enable(LogLevel level)
		{
			_log_level = LogLevel(_log_level | level);
		}
		
		void Logger::disable(LogLevel level)
		{
			_log_level = LogLevel(_log_level & ~level);
		}
		
		typedef std::map<std::thread::id, std::string> ThreadNamesT;
		ThreadNamesT _thread_names;
		
		void Logger::set_thread_name(std::string name)
		{
			_thread_names[std::this_thread::get_id()] = name;
		}
		
		std::string Logger::thread_name() const
		{
			ThreadNamesT::iterator i = _thread_names.find(std::this_thread::get_id());
			
			if (i != _thread_names.end()) {
				return i->second;
			} else {
				return (std::stringstream() << std::this_thread::get_id()).str();
			}
		}
		
		namespace Logging {
			Logger * logger() {
				static Logger * logger = NULL;
				
				if (!logger) {
					logger = new Logger;
				}
				
				return logger;
			}
		}
	}
}



