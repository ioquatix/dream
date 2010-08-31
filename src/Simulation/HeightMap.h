/*
 *  HeightMap.h
 *  Dream
 *
 *  Created by Samuel Williams on 30/10/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_HEIGHTMAP_H
#define _DREAM_HEIGHTMAP_H

#include "Framework.h"
#include "Class.h"

#include "Vector.h"

namespace Dream {
	namespace Simulation {
	
		class Mesh;
		class IPixelBuffer;
		class IMutablePixelBuffer;
		
		class HeightMap {
	public:
			HeightMap ();
			virtual ~HeightMap ();
			
			virtual RealT height (const Vec2 &at) abstract;
			
			/* Bivalent - 0 = black, 1 = white */
			void toImage (REF(IMutablePixelBuffer) image);
			void toMesh (Mesh & mesh, Vector<2,unsigned> size, Vec3 scale, RealT offset);
		};
		
		class ImageHeightMap : public HeightMap {
	protected:
			REF(IPixelBuffer) m_image;
	public:
			ImageHeightMap (REF(IPixelBuffer) image);
			
			virtual RealT height (const Vec2 &at);
		};
		
		class BilinearHeightMap : public HeightMap {
	protected:
			HeightMap *m_input;
			
	public:
			BilinearHeightMap (HeightMap *input);
			
			virtual RealT height (const Vec2 &at);
		};
		
		class ScaleHeightMap : public HeightMap {
	protected:
			HeightMap *m_input;
			Vec2 m_scale;
			
	public:
			ScaleHeightMap (HeightMap *input, Vec2 scale);
			
			virtual RealT height (const Vec2 &at);
		};
		
		class OffsetHeightMap : public HeightMap {
	protected:
			HeightMap *m_input;
			Vec2 m_offset;
			
	public:
			OffsetHeightMap (HeightMap *input, Vec2 offset);
			
			virtual RealT height (const Vec2 &at);
		};
	
	}
}

#endif
