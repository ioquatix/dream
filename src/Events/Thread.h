//
//  Events/Thread.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 13/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_EVENTS_THREAD_H
#define _DREAM_EVENTS_THREAD_H

#include "Loop.h"

#include <thread>
#include <mutex>

namespace Dream
{
	namespace Events
	{
	
		/// Manage an event-loop on a separate thread.
		class Thread : public Object
		{
			protected:
				REF(Loop) _loop;
				Shared<std::thread> _thread;
				
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
				std::mutex _lock;
				std::vector<ItemT> * _waiting, * _processing;
			
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
			std::lock_guard<std::mutex> lock(_lock);
			
			_waiting = new std::vector<ItemT>;
			_processing = new std::vector<ItemT>;
		}
		
		template <typename ItemT>
		Queue<ItemT>::~Queue ()
		{
			std::lock_guard<std::mutex> lock(_lock);
			
			delete _waiting;
			delete _processing;
		}

		// This function can be called by any number of threads.
		template <typename ItemT>
		void Queue<ItemT>::add (ItemT item)
		{
			std::lock_guard<std::mutex> lock(_lock);

			_waiting->push_back(item);
		}

		//
		template <typename ItemT>
		void Queue<ItemT>::flush ()
		{
			std::lock_guard<std::mutex> lock(_lock);
			
			_waiting->resize(0);
		}

		// This function is not re-entrant and must be called by only one thread or protected with a lock.
		// It is designed this way because this queue is most useful and efficient in this configuration.
		template <typename ItemT>
		std::vector<ItemT> * Queue<ItemT>::fetch ()
		{
			{
				std::lock_guard<std::mutex> lock(_lock);

				std::swap(_processing, _waiting);
				_waiting->resize(0);
			}
			
			return _processing;
		}
	
	}
}

#endif
