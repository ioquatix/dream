/*
 *  RandomPointSequence.cpp
 *  Dream
 *
 *  Created by Administrator on 17/02/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "RandomSequence.h"

namespace Dream {
	namespace Numerics {

		RandomSequence::RandomSequence (unsigned seed, RealT min, RealT max)
		: m_generator(seed), m_distribution(min, max) {
		
		}

		const RealT RandomSequence::nextReal () {
			return m_distribution(m_generator);
		}
		
	}
}