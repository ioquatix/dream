/*
 *  Imaging/PerlinNoise.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 13/05/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "PerlinNoise.h"

#include <boost/random.hpp>
#include "Interpolate.h"

namespace Dream {
	namespace Numerics {
		PerlinNoise::PerlinNoise(unsigned seed) {
			typedef boost::uniform_real<> real_distribution_t;
			typedef boost::uniform_int<> int_distribution_t;
			
			typedef boost::minstd_rand base_generator_t;
			typedef boost::variate_generator<base_generator_t, int_distribution_t> i_gen_t;
			typedef boost::variate_generator<base_generator_t, real_distribution_t> r_gen_t;
			
			i_gen_t r256(base_generator_t (seed), int_distribution_t (0, 256)); // glues randomness with mapping
			
			for (IndexT i = 0; i < 256; ++i) m_indicies[i] = i;
			
			/* Mix it up! */
			for (IndexT i = 0; i < 256; ++i) {
				IndexT w = r256() % 256;
				/* Swap */
				register unsigned char c;
				c = m_indicies[w];
				m_indicies[w] = m_indicies[i];
				m_indicies[i] = c;
			}
			
			r_gen_t r01(base_generator_t (seed+1), real_distribution_t (0.0, 1.0)); // glues randomness with mapping
			
			for (IndexT i = 0; i < 256; ++i) m_table[i] = (RealT)r01();
		}

		RealT PerlinNoise::sample(const Vec3 &v) const {	
			Vec3 t (v);
			Vec3 o (v);
			
			RealT d[8];
			o.floor();
			
			/* Get noise at 8 lattice points */
			for (IndexT lz = 0; lz < 2; ++ lz) {
				d[0 + lz*4] = latticeNoise((IndexT)o[X], (IndexT)o[Y], (IndexT)o[Z] + lz);
				d[1 + lz*4] = latticeNoise((IndexT)o[X], (IndexT)o[Y] + 1, (IndexT)o[Z] + lz);
				d[2 + lz*4] = latticeNoise((IndexT)o[X] + 1, (IndexT)o[Y] + 1, (IndexT)o[Z] + lz);
				d[3 + lz*4] = latticeNoise((IndexT)o[X] + 1, (IndexT)o[Y], (IndexT)o[Z] + lz);
			}

			RealT x0, x1, x2, x3, y0, y1;
			t.frac();
			
			x0 = linearInterpolate(t[X], d[0], d[3]);
			x1 = linearInterpolate(t[X], d[1], d[2]);
			x2 = linearInterpolate(t[X], d[4], d[7]);
			x3 = linearInterpolate(t[X], d[5], d[6]);
			
			y0 = linearInterpolate(t[Y], x0, x1);
			y1 = linearInterpolate(t[Y], x2, x3);
			
			RealT result = linearInterpolate(t[Z], y0, y1);
			
			return result;
		}

		RealT PerlinNoise::latticeNoise(IndexT i, IndexT j, IndexT k) const {
		#define P(x) m_indicies[(x) & 255]
			//std::cout << i << " " << j << " " << k << " = " << m_table[P(i + P(j + P(k)))] << std::endl;
			return m_table[P(i + P(j + P(k)))];
			
		#undef P
		}

		/* Noise should scale between 0.0...1,0 */
		RealT PerlinNoise::turbulence (const Vec3 &at) const {
			RealT v = 0;
			v += sample(at, 1.0/32.0, 1.0/2.0);
			v += sample(at, 1.0/16.0, 1.0/4.0);
			v += sample(at, 1.0/8.0,  1.0/8.0);
			v += sample(at, 1.0/4.0,  1.0/16.0);
			v += sample(at, 1.0/2.0,  (1.0/16.0) * 0.75);
			v += sample(at, 1.0/1.0,  (1.0/16.0) * 0.25);
			
			return v;
		}
		
		//RealT PerlinNoise::clouds (const Vec3 &v) const {
		//	return (noise(v) * noise(v*.5) * noise(v*.25) * noise(v*.5*.25));
		//}

		RealT PerlinNoise::marble (const RealT &strength, const Vec3 &v) const {
			RealT t = turbulence(v);
			
			return t;
		}
	}
}