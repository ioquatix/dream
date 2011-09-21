//
//  Thread.h
//  Dream
//
//  Created by Samuel Williams on 13/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_EVENTS_THREAD_H
#define _DREAM_EVENTS_THREAD_H

#include "Loop.h"
#include <boost/thread.hpp>

namespace Dream
{
	namespace Events
	{
	
		/// Manage an event-loop on a separate thread.
		class Thread : public Object
		{
			protected:
				REF(Loop) m_loop;
				boost::thread * m_thread;
				
				void run ();
				
			public:
				Thread ();
				
				/// This destructor may block if the event-loop is not responding.
				~Thread ();
				
				/// The remote loop instance.
				REF(Loop) loop();
				
				/// Start the event-loop on a new thread.
				void start();
				
				/// Stop the event-loop.
				void stop();
		};
		
		/// Stream data from multiple writers to a single reader.
		template <typename ItemT>
		class Queue : public Object
		{
			protected:
				boost::mutex m_lock;
				std::vector<ItemT> * m_waiting, * m_processing;
			
			public:
				Queue ();
				virtual ~Queue ();
				
				void add (ItemT item);
				void flush ();
			
				std::vector<ItemT> * fetch ();
		};

		template <typename ItemT>
		Queue<ItemT>::Queue ()
		{
			boost::mutex::scoped_lock lock(m_lock);
			m_waiting = new std::vector<ItemT>;
			m_processing = new std::vector<ItemT>;
		}
		
		template <typename ItemT>
		Queue<ItemT>::~Queue ()
		{
			boost::mutex::scoped_lock lock(m_lock);
			delete m_waiting;
			delete m_processing;
		}

		// This function can be called by any number of threads.
		template <typename ItemT>
		void Queue<ItemT>::add (ItemT item)
		{
			boost::mutex::scoped_lock lock(m_lock);
			m_waiting->push_back(item);
		}

		//
		template <typename ItemT>
		void Queue<ItemT>::flush ()
		{
			boost::mutex::scoped_lock lock(m_lock);
			m_waiting->resize(0);
		}

		// This function is not re-entrant and must be called by only one thread or protected with a lock.
		// It is designed this way because this queue is most useful and efficient in this configuration.
		template <typename ItemT>
		std::vector<ItemT> * Queue<ItemT>::fetch ()
		{
			{
				boost::mutex::scoped_lock lock(m_lock);
				std::swap(m_processing, m_waiting);
				m_waiting->resize(0);
			}
			
			return m_processing;
		}
	
	}
}

#endif