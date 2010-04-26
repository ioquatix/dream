/*
 *  Reference.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 3/10/09.
 *  Copyright 2009 Samuel Williams. All rights reserved.
 *
 */

#include "Reference.h"

#include <libkern/OSAtomic.h>
#include <set>

// For testing
#include "Core/CodeTest.h"

namespace Dream {
	
	// #define TRACK_ALLOCATIONS 1
	
	#ifdef TRACK_ALLOCATIONS
	enum AllocationStatus {
		ALLOCATED,
		DELETED,
		STATIC
	};
	
	typedef std::map<const void *, AllocationStatus> AllocationsT;
	AllocationsT s_allocations;
	double s_refCountBumps = 0;
	
	void markStaticAllocation (void * object) {
		s_allocations[object] = STATIC;
	}
	#endif

	SharedObject::SharedObject () : m_count(0) {
		#ifdef TRACK_ALLOCATIONS
		s_allocations[this] = ALLOCATED;
		#endif
	}

	SharedObject::SharedObject (const SharedObject & other) : m_count(0) {
		
	}
			
	void SharedObject::operator= (const SharedObject & other) {
		// Don't touch reference count.
	}
	
	SharedObject::~SharedObject () {
	
	}
	
	void SharedObject::retain () const {
		// std::cout << "Reference Increment @ " << this << " -> " << (m_count+1) << std::endl;

		#ifdef TRACK_ALLOCATIONS
		s_refCountBumps += 1;
		#endif

		OSAtomicIncrement32(&m_count);
	}
	
	void SharedObject::release () const {
		// std::cout << "Reference Decrement @ " << this << " -> " << (m_count-1) << std::endl;
		
		#ifdef TRACK_ALLOCATIONS
		s_refCountBumps += 1;
		#endif
		
		int32_t count = OSAtomicDecrement32(&m_count);

		if (count == 0)
			deallocate();
	}
	
	void SharedObject::deallocate () const {
		//std::cout << "Deleting Object @ " << this << std::endl;
		
		#ifdef TRACK_ALLOCATIONS
		s_allocations[this] = DELETED;
		#endif
		
		delete this;
	}
	
	int32_t SharedObject::referenceCount () const {
		return m_count;
	}
	
	void debugAllocations () {
		#ifdef TRACK_ALLOCATIONS
		int totalAllocations = 0;
		
		for (AllocationsT::iterator i = s_allocations.begin(); i != s_allocations.end(); ++i) {
			if (i->second == ALLOCATED)
				std::cerr << "\t" << i->first << " has not been freed!" << std::endl;
			
			totalAllocations += 1;
		}
		
		std::cerr << "Total Allocations: " << totalAllocations << std::endl;
		std::cerr << "Ref Count Bumps: " << s_refCountBumps << std::endl;
		#endif
	}
	
#pragma mark -
#pragma mark Unit Tests

#ifdef ENABLE_TESTING

	UNIT_TEST(Reference)
	{
		BEGIN_TEST
		
		testing("Shared Object");
		
		REF(SharedObject) s1, s2, s3;
		
		assertEqual(s1, s2, "Null objects are equal");
		
		s1 = new SharedObject;
		
		assertFalse(equal(s1, s2, "Null and non-null objects are not equal"));
		
		s2 = new SharedObject;
		s3 = new SharedObject;
		
		assertEqual(s1, s1, "Objects are equal");
		assertFalse(equal(s1, s2, "Objects are not equal"));
		
		std::set<REF(SharedObject)> objects;
		objects.insert(s1);
		objects.insert(s2);
		objects.insert(s3);
		
		assertEqual(objects.size(), 3, "Set contains correct number of objects");
	}
#endif
	
}
