//
//  Core/Timer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_TIMER_H
#define _DREAM_CORE_TIMER_H

#include "Core.h"

namespace Dream {
	namespace Core {
		/// Measured in seconds
		typedef double TimeT;
		
		TimeT system_time ();
		void sleep (const TimeT & s);
		
		class Timer {
		protected:
			mutable TimeT _last, _total;
			
		public:			
			Timer ();
			virtual ~Timer ();
			
			virtual void reset ();
			virtual TimeT time () const;
		};
		
		// Counts up
		class Stopwatch {
		protected:		
			mutable TimeT _total;
			mutable Timer _timer;
			
			bool _running;
			
		public:
			Stopwatch ();
			virtual ~Stopwatch ();
			
			virtual void reset ();
			virtual TimeT time () const;
			
			bool running () const 
			{
				return _running;
			}
			
			void start ();
			void pause ();
		};
		
		// Counts down
		class EggTimer : public Stopwatch
		{			
		protected:
			TimeT _duration;
			
		public:
			EggTimer (TimeT duration);
			virtual ~EggTimer ();
			
			/// Returns whether the duration has passed.
			bool finished () const;
			
			/// This function returns the time remaining, which may be negative if the Stopwatch has gone past the timeout.
			TimeT remaining_time () const;
		};
		
// MARK: mark -
// MARK: mark Timer Statistics
		
		class TimerStatistics {
		private:
			bool _perform_reset;
			TimeT _last_time;
			
			TimeT _duration;
						
			TimeT _min, _max;
			
			unsigned long _count;
			
		public:
			TimerStatistics ();
			
			TimeT average_duration () const;
			TimeT updates_per_second () const;
			
			const TimeT & minimum_duration () const { return _min; }
			const TimeT & maximum_duration () const { return _max; }
			
			const TimeT & total_duration () const { return _duration; }
			const unsigned long & update_count () const { return _count; }
			
			/// Reset current statistics, but maintains overall average statistics, such as min and max.
			void reset ();
			
			/// Optionally call this method to only take into consideration time between now and when update() is called.
			void begin_timer (const TimeT & start_time);
			
			/// Update a single iteration of a timer, and collect statistics about how long it took to execute since the last call to update().
			void update (const TimeT & current_time);
		};
		
// MARK: mark -
// MARK: mark Timed System

		template <typename DerivedT>
		class TimedSystem
		{
		protected:
			TimeT _last_time;
			
		public:
			TimedSystem () : _last_time(-1)
			{
				
			}
			
			void increment (TimeT dt)
			{
				if (_last_time == -1) _last_time = 0;
				update(_last_time + dt);
			}
			
			void update (TimeT time)
			{
				if (_last_time == -1) _last_time = time;
				TimeT dt = time - _last_time;
				_last_time = time;

				static_cast<DerivedT*>(this)->update_for_duration(_last_time, time, dt);				
			}
			
			const TimeT & current_time () {
				return _last_time;
			}
		};
		
	}
}

#endif
