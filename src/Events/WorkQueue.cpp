//
//  WorkQueue.cpp
//  Dream
//
//  Created by Samuel Williams on 14/02/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "WorkQueue.h"

namespace Dream {
	
	namespace Events {
		
		WorkQueue::WorkQueue() {
			
		}
		
		WorkQueue::~WorkQueue() {
			
		}
		
		/*
		 std::vector<Range> ranges;
		 const std::size_t THREADS = 8;
		 std::size_t unit_size = _physics.size() / THREADS;
		 std::size_t current = 0, remaining = _physics.size();
		 const std::size_t MINIMUM = 1024;
		 
		 while (remaining > 0) {
		 // We don't allocate less than MINIMUM units of work to a single range.
		 Range range = {
		 current,
		 std::min(std::max(unit_size, MINIMUM), remaining_size),
		 0
		 };
		 
		 range.end = range.offset + range.length;
		 
		 current += range.size;
		 remaining -= range.size;
		 
		 ranges.push_back(range);
		 }
		 
		 clear();
		 
		 std::vector<std::thread> threads;
		 
		 for (Range & range : ranges) {
		 threads.push_back(std::thread([=](){
		 std::size_t i = range.offset;
		 while (i < range.end) {
		 Physics & physics = _physics[i];
		 
		 if (physics.update_time(dt, Vec3(0.0, 0.0, -9.8))) {
		 RealT alpha = physics.calculate_alpha(0.7);
		 
		 physics.update_vertex_color(Vec3(0.2 * physics.color_modulation(2.0)) << alpha);
		 
		 // Add the particle to be drawn:
		 queue(physics);
		 
		 i += 1;
		 } else {
		 // "Remove" the current element in this range.
		 _physics[i] = _physics[range.end];
		 range.end -= 1;
		 }
		 }
		 }));
		 }
		 
		 // Wait for all work to be complete.
		 for (std::thread & thread : threads) {
		 thread.join();
		 }
		 
		 for (Range & range : ranges) {
		 std::size_t removed = range.end - (range.start + range.offset);
		 
		 if (removed) {
		 }
		 */
	}
}
