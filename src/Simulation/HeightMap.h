//
//  Simulation/HeightMap.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/10/06.
//  Copyright 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_SIMULATION_HEIGHTMAP_H
#define _DREAM_SIMULATION_HEIGHTMAP_H

#include "../Framework.h"
#include "../Numerics/Vector.h"
#include "../Numerics/Interpolate.h"
#include "../Imaging/PixelBuffer.h"

namespace Dream {
	namespace Simulation {
		using namespace Numerics;
		using namespace Imaging;
		
		class HeightMap {
		public:
			HeightMap ();
			virtual ~HeightMap ();
			
			virtual RealT height (const Vec2 &at) abstract;
			
			/* Bivalent - 0 = black, 1 = white */
			//void to_image(REF(IMutablePixelBuffer) image);
			//void to_mesh(Mesh & mesh, Vec2u size, Vec3 scale, RealT offset);
		};
		
		class ImageHeightMap : public HeightMap {
		protected:
			Ref<IPixelBuffer> _image;
		public:
			ImageHeightMap (Ref<IPixelBuffer> image);
			virtual ~ImageHeightMap();
			
			virtual RealT height (const Vec2 &at);
		};
		
		class BilinearHeightMap : public HeightMap {
		protected:
			HeightMap * _input;
			
		public:
			BilinearHeightMap (HeightMap *input);
			virtual ~BilinearHeightMap();
			
			virtual RealT height (const Vec2 &at);
		};
		
		class ScaleHeightMap : public HeightMap {
		protected:
			HeightMap * _input;
			Vec2 _scale;
			
		public:
			ScaleHeightMap (HeightMap * input, Vec2 scale);
			virtual ~ScaleHeightMap();
			
			virtual RealT height (const Vec2 &at);
		};
		
		class OffsetHeightMap : public HeightMap {
		protected:
			HeightMap * _input;
			Vec2 _offset;
			
		public:
			OffsetHeightMap (HeightMap * input, Vec2 offset);
			virtual ~OffsetHeightMap();
			
			virtual RealT height (const Vec2 &at);
		};
		
	}
}

#endif
