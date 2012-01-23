//
//  Events/Thread.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 13/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Thread.h"

namespace Dream {
	namespace Events {
		
		Thread::Thread ()
			: _thread(NULL)
		{
			_loop = new Loop;
			_loop->set_stop_when_idle(false);
		}
		
		Thread::~Thread ()
		{
			stop();
		}
		
		REF(Loop) Thread::loop()
		{
			return _loop;
		}
		
		void Thread::start ()
		{			
			if (!_thread)
				_thread = new std::thread(std::bind(&Thread::run, this));
		}
		
		void Thread::run ()
		{
			std::cerr << "Starting thread event loop..." << std::endl;
		
			// Lock the loop to ensure it isn't released by another thread.
			REF(Loop) loop = _loop;
			
			loop->run_forever();
			
			std::cerr << "Exiting thread event loop..." << std::endl;
		}
		
		void Thread::stop ()
		{
			if (_thread) {
				_loop->stop();
				_thread->join();
				
				_thread = NULL;
			}
		}

#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		
		class TTLNote : public Object, implements INotificationSource {			
			public:
				std::vector<REF(Loop)> loops;
				std::size_t count;
				
				TTLNote ();
				virtual void process_events (Loop *, Event);
		};
		
		TTLNote::TTLNote ()
			: count(0)
		{
		
		}
		
		void TTLNote::process_events(Loop * loop, Event event)
		{			
			count += 1;
			
			if (loops.size() > 0) {
				REF(Loop) next = loops.back();
				loops.pop_back();
				
				next->post_notification(this, true);
			}
		}
		
		void add1(REF(Queue<int>) queue)
		{
			queue->add(1);
		}
		
		UNIT_TEST(Thread) {
			testing("Threads");
			
			REF(Thread) t1, t2, t3;
			
			t1 = new Thread;
			t2 = new Thread;
			t3 = new Thread;
			
			REF(TTLNote) note = new TTLNote;
			note->loops.push_back(t1->loop());
			note->loops.push_back(t2->loop());
			note->loops.push_back(t3->loop());
			
			t1->start();
			t2->start();
			t3->start();
			
			t1->loop()->post_notification(note, true);
			
			sleep(1);
			
			check(note->count == 4) << "Note count " << note->count << " indicates failure in notification system";
			
			testing("Queues");
			
			// Three threads will generate semi-random integers.
			REF(Queue<int>) queue = new Queue<int>();
			
			REF(TimerSource) e1 = new TimerSource(std::bind(add1, queue), 0.001, true);
			REF(TimerSource) e2 = new TimerSource(std::bind(add1, queue), 0.001, true);
			REF(TimerSource) e3 = new TimerSource(std::bind(add1, queue), 0.001, true);
			
			t1->loop()->schedule_timer(e1);
			t2->loop()->schedule_timer(e2);
			t3->loop()->schedule_timer(e3);
			
			queue->add(10);
			
			/// They will be bulk processed by this loop which will fetch several items at a time.
			int total = 0;
			while (total < 1000) {
				sleep(0.1);
			
				// Fetch all items currently available.
				std::vector<int> * items = queue->fetch();
				
				if (items->size() == 0)
					continue;
				
				std::cerr << "Fetched " << items->size() << " items" << std::endl;
				
				foreach(i, *items) {
					total += *i;
				}				
			}
			
			check(total > 1000) << "Total was not incremented correctly";
			
			t1->stop();
			t2->stop();
			t3->stop();
			
			std::cerr << std::flush;
		}
	
#endif

	}
}
