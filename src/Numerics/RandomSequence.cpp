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
		: m_sequence(BaseGeneratorT(seed), DistributionT (min, max)) {
		
		}

		const RealT RandomSequence::nextReal () {
			return m_sequence();
		}
		
	}
}