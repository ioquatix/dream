//
//  Numerics/RandomSequence.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/02/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//
 
#include "Vector.h"

#include <random>

namespace Dream {
	namespace Numerics {
		
		class RandomSequence {
		protected:
			typedef std::uniform_real_distribution<> DistributionT;		
			typedef std::minstd_rand GeneratorT;

			DistributionT m_distribution;
			GeneratorT m_generator;
					
			Vec3 m_currentPoint;
			Vec3 m_scale;
		public:
			RandomSequence (unsigned seed, RealT min = -1.0, RealT max = 1.0);
			
			template <unsigned D>
			const Vector<D> nextVector () {
				Vector<D> result;
				
				for (std::size_t i = 0; i < D; i++) {
					result[i] = nextReal();
				}
				
				return result;
			}
			
			const RealT nextReal ();			
		};
		
	}
}
