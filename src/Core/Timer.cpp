//
//  Core/Timer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "Timer.h"

#include "../Numerics/Number.h"

#include <ctime>
#include <sys/time.h>

namespace Dream {
	namespace Core {
		void sleep (const TimeT & s)
		{
			struct timespec ts;
			ts.tv_sec = s;
			ts.tv_nsec = (s - ts.tv_sec) * 1000 * 1000 * 1000;
			
			nanosleep(&ts, (struct timespec *)NULL);
		}
		
// MARK: mark -
// MARK: mark Timer Implementation
				
		Timer::Timer () {
			this->reset();
		}
		
		Timer::~Timer () {
			
		}
		
		void Timer::reset () {
			this->_last = system_time();
			this->_total = 0.0;
		}
		
		TimeT Timer::time () const {
			TimeT current = system_time();
			this->_total += current - this->_last;
			this->_last = current;
			return this->_total;
		}
		
// MARK: mark -
// MARK: mark Stopwatch Implementation
				
		Stopwatch::Stopwatch () : _total(0), _running(false) {
			
		}
		
		Stopwatch::~Stopwatch () {
			
		}
		
		void Stopwatch::start () {
			if (!_running) {
				_running = true;
				
				_timer.reset();
			}
		}
		
		void Stopwatch::pause () {
			if (_running) {
				_running = false;
				
				_total += _timer.time();
				/* Must call start to restart timer */
			}
		}
		
		void Stopwatch::reset () {
			_running = false;
			_total = 0;
		}
		
		TimeT Stopwatch::time () const {
			if (_running)
			{
				_total += _timer.time();
				_timer.reset();
			}
			
			return _total;
		}
		
// MARK: mark -
// MARK: mark EggTimer Implementation

		EggTimer::EggTimer (TimeT duration) : _duration(duration)
		{
			
		}
		
		EggTimer::~EggTimer ()
		{
			
		}
		
		bool EggTimer::finished () const
		{
			return time() >= _duration;
		}
		
		TimeT EggTimer::remaining_time () const
		{
			return _duration - time();
		}
		
// MARK: mark -
// MARK: mark class TimerStatistics
		
		using namespace Dream::Numerics;
		
		TimerStatistics::TimerStatistics () : _duration(0), _min(std::numeric_limits<RealT>::max()), _max(0), _count(0) 
		{
			
		}	
		
		TimeT TimerStatistics::average_duration () const
		{
			if (_count == 0) return 0;
			
			return _duration / _count;
		}
		
		TimeT TimerStatistics::updates_per_second () const
		{
			if (_duration == 0) return 0;
			
			return _count / _duration;
		}
		
		void TimerStatistics::reset ()
		{
			_perform_reset = true;
			
			// Reset minimum and maximum durations:
			_min = std::numeric_limits<RealT>::max();
			_max = 0.0;
		}
		
		void TimerStatistics::begin_timer (const TimeT & start_time)
		{
			_last_time = start_time;
		}
		
		void TimerStatistics::update (const TimeT & current_time)
		{
			_min = std::min(_min, current_time - _last_time);
			_max = std::max(_max, current_time - _last_time);
			
			if (_perform_reset) {
				_perform_reset = false;
				
				if (_count > 0) {
					_duration = _duration / _count;
					_count = 1; // We count last frame average plus this frame
				} else {
					_duration = 0;
				}
			}
			
			_duration += current_time - _last_time;
			_count += 1;
			
			_last_time = current_time;
		}		
		
	}
}
