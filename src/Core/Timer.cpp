/*
 *  Core/Timer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/09/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

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
		
#pragma mark -
#pragma mark Timer Implementation
		
		IMPLEMENT_CLASS(Timer)
		
		Timer::Timer () {
			this->reset();
		}
		
		Timer::~Timer () {
			
		}
		
		void Timer::reset () {
			this->m_last = systemTime();
			this->m_total = 0.0;
		}
		
		TimeT Timer::time () const {
			TimeT current = systemTime();
			this->m_total += current - this->m_last;
			this->m_last = current;
			return this->m_total;
		}
		
#pragma mark -
#pragma mark Stopwatch Implementation
		
		IMPLEMENT_CLASS(Stopwatch)
		
		Stopwatch::Stopwatch () : m_total(0), m_running(false) {
			
		}
		
		Stopwatch::~Stopwatch () {
			
		}
		
		void Stopwatch::start () {
			if (!m_running) {
				m_running = true;
				
				m_timer.reset();
			}
		}
		
		void Stopwatch::pause () {
			if (m_running) {
				m_running = false;
				
				m_total += m_timer.time();
				/* Must call start to restart timer */
			}
		}
		
		void Stopwatch::reset () {
			m_running = false;
			m_total = 0;
		}
		
		TimeT Stopwatch::time () const {
			if (m_running)
			{
				m_total += m_timer.time();
				m_timer.reset();
			}
			
			return m_total;
		}
		
#pragma mark -
#pragma mark EggTimer Implementation
		
		IMPLEMENT_CLASS(EggTimer)
		
		EggTimer::EggTimer (TimeT duration) : m_duration(duration)
		{
			
		}
		
		EggTimer::~EggTimer ()
		{
			
		}
		
		bool EggTimer::finished () const
		{
			return time() >= m_duration;
		}
		
		TimeT EggTimer::remainingTime () const
		{
			return m_duration - time();
		}
		
#pragma mark -
#pragma mark class TimerStatistics
		
		using namespace Dream::Numerics;
		
		TimerStatistics::TimerStatistics () : m_duration(0), m_min(std::numeric_limits<RealT>::max()), m_max(0), m_count(0) 
		{
			
		}	
		
		TimeT TimerStatistics::averageDuration () const
		{
			if (m_count == 0) return 0;
			
			return m_duration / m_count;
		}
		
		TimeT TimerStatistics::updatesPerSecond () const
		{
			if (m_duration == 0) return 0;
			
			return m_count / m_duration;
		}
		
		void TimerStatistics::reset ()
		{
			m_performReset = true;
		}
		
		void TimerStatistics::beginTimer (const TimeT & startTime)
		{
			m_lastTime = startTime;
		}
		
		void TimerStatistics::update (const TimeT & currentTime)
		{
			m_min = std::min(m_min, currentTime - m_lastTime);
			m_max = std::max(m_max, currentTime - m_lastTime);
			
			if (m_performReset) {
				m_performReset = false;
				
				if (m_count > 0) {
					m_duration = m_duration / m_count;
					m_count = 1; // We count last frame average plus this frame
				} else {
					m_duration = 0;
				}
			}
			
			m_duration += currentTime - m_lastTime;
			m_count += 1;
			
			m_lastTime = currentTime;
		}		
		
	}
}