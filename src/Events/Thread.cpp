//
//  Thread.cpp
//  Dream
//
//  Created by Samuel Williams on 13/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#include "Thread.h"

namespace Dream {
	namespace Events {
		
		Thread::Thread ()
			: m_thread(NULL)
		{
			m_loop = new Loop;
			m_loop->setStopWhenIdle(false);
		}
		
		Thread::~Thread ()
		{
			stop();
		}
		
		REF(Loop) Thread::loop()
		{
			return m_loop;
		}
		
		void Thread::start ()
		{			
			if (!m_thread)
				m_thread = new std::thread(std::bind(&Thread::run, this));
		}
		
		void Thread::run ()
		{
			std::cerr << "Starting thread event loop..." << std::endl;
		
			// Lock the loop to ensure it isn't released by another thread.
			REF(Loop) loop = m_loop;
			
			loop->runForever();
			
			std::cerr << "Exiting thread event loop..." << std::endl;
		}
		
		void Thread::stop ()
		{
			if (m_thread) {
				m_loop->stop();
				m_thread->join();
				
				m_thread = NULL;
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
				virtual void processEvents (Loop *, Event);
		};
		
		TTLNote::TTLNote ()
			: count(0)
		{
		
		}
		
		void TTLNote::processEvents(Loop * loop, Event event)
		{			
			count += 1;
			
			if (loops.size() > 0) {
				REF(Loop) next = loops.back();
				loops.pop_back();
				
				next->postNotification(this, true);
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
			
			t1->loop()->postNotification(note, true);
			
			sleep(1);
			
			check(note->count == 4) << "Note count " << note->count << " indicates failure in notification system";
			
			testing("Queues");
			
			// Three threads will generate semi-random integers.
			REF(Queue<int>) queue = new Queue<int>();
			
			REF(TimerSource) e1 = new TimerSource(std::bind(add1, queue), 0.001, true);
			REF(TimerSource) e2 = new TimerSource(std::bind(add1, queue), 0.001, true);
			REF(TimerSource) e3 = new TimerSource(std::bind(add1, queue), 0.001, true);
			
			t1->loop()->scheduleTimer(e1);
			t2->loop()->scheduleTimer(e2);
			t3->loop()->scheduleTimer(e3);
			
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
