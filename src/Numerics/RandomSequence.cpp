//
//  Numerics/RandomSequence.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/02/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "RandomSequence.h"

namespace Dream {
	namespace Numerics {

		RandomSequence::RandomSequence (unsigned seed, RealT min, RealT max)
		: m_distribution(min, max), m_generator(seed) {
		
		}

		const RealT RandomSequence::nextReal () {
			return m_distribution(m_generator);
		}
		
	}
}
