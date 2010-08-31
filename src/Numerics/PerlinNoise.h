/*
 *  Imaging/PerlinNoise.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/05/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_PERLINNOISE_H
#define _DREAM_IMAGING_PERLINNOISE_H

#include "Numerics.h"
#include "Vector.h"

namespace Dream {
	namespace Numerics {
	
		class PerlinNoise {
		protected:
			RealT m_table[256];
			unsigned char m_indicies[256];
			
		public:
			PerlinNoise(unsigned seed);
			
			RealT sample (const Vec3 &v) const;
			
			RealT turbulence (const Vec3 &v) const;
			RealT marble (const RealT &strength, const Vec3 &v) const;
			
			/* roct -> reciprocal octave, ie 1/oct, rscale is 1/scale */
			RealT sample(const Vec3 &at, RealT roct, RealT rscale) const {
				return sample(at * roct) * rscale;
			}
			
		protected:
			RealT latticeNoise(IndexT i, IndexT j, IndexT k) const;
			RealT spline (RealT x) const;
		};

	}
}

#endif