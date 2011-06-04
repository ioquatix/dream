/*
 *  Core/Timer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/09/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_TIMER_H
#define _DREAM_CORE_TIMER_H

#include "Core.h"

namespace Dream {
	namespace Core {
		/// Measured in seconds
		typedef double TimeT;
		
		TimeT systemTime ();
		void sleep (const TimeT & s);
		
		class Stopwatch;
		
		class Timer {
		protected:
			mutable TimeT m_last, m_total;
			
		public:			
			Timer ();
			virtual ~Timer ();
			
			virtual void reset ();
			virtual TimeT time () const;
		};
		
		class Stopwatch {
		protected:		
			mutable TimeT m_total;
			mutable Timer m_timer;
			
			bool m_running;
			
		public:
			Stopwatch ();
			virtual ~Stopwatch ();
			
			virtual void reset ();
			virtual TimeT time () const;
			
			bool running () const 
			{
				return m_running;
			}
			
			void start ();
			void pause ();
		};
		
		class EggTimer : public Stopwatch
		{			
		protected:
			TimeT m_duration;
			
		public:
			EggTimer (TimeT duration);
			virtual ~EggTimer ();
			
			/// Returns whether the duration has passed.
			bool finished () const;
			
			/// This function returns the time remaining, which may be negative if the Stopwatch has gone past the timeout.
			TimeT remainingTime () const;
		};
		
#pragma mark -
#pragma mark Timer Statistics
		
		class TimerStatistics {
		private:
			bool m_performReset;
			TimeT m_lastTime;
			
			TimeT m_min;
			TimeT m_max;
			
			TimeT m_duration;
			unsigned long m_count;
			
		public:
			TimerStatistics ();
			
			TimeT averageDuration () const;
			TimeT updatesPerSecond () const;
			
			const TimeT & minimumDuration () const { return m_min; }
			const TimeT & maximumDuration () const { return m_max; }
			
			const TimeT & totalDuration () const { return m_duration; }
			const unsigned long & updateCount () const { return m_count; }
			
			/// Reset current statistics, but maintains overall average statistics, such as min and max.
			void reset ();
			
			/// Optionally call this method to only take into consideration time between now and when update() is called.
			void beginTimer (const TimeT & startTime);
			
			/// Update a single iteration of a timer, and collect statistics about how long it took to execute since the last call to update().
			void update (const TimeT & currentTime);
		};
		
#pragma mark -
#pragma mark Timed System

		template <typename DerivedT>
		class TimedSystem
		{
		protected:
			TimeT m_lastTime;
			
		public:
			TimedSystem () : m_lastTime(-1)
			{
				
			}
			
			void increment (TimeT dt)
			{
				if (m_lastTime == -1) m_lastTime = 0;
				update(m_lastTime + dt);
			}
			
			void update (TimeT time)
			{
				if (m_lastTime == -1) m_lastTime = time;
				TimeT dt = time - m_lastTime;
				m_lastTime = time;

				static_cast<DerivedT*>(this)->updateForDuration(m_lastTime, time, dt);				
			}
			
			const TimeT & currentTime () {
				return m_lastTime;
			}
		};
		
	}
}

#endif