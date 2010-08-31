/*
 *  HeightMap.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 30/10/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "HeightMap.h"

#include "Interpolate.h"
#include "Mesh.h"
#include "PixelBuffer.h"

namespace Dream {
	
	HeightMap::HeightMap () {
	
	}
		
	HeightMap::~HeightMap () {
	
	}
	
	void HeightMap::toMesh (Mesh & mesh, Vector<2,unsigned> size, Vec3 scale, RealT offset) {
		unsigned base = mesh.vertices().size();
		
		/* Add points in the mesh */
		for (unsigned y = 0; y < size[HEIGHT]; ++y) {
			for (unsigned x = 0; x < size[WIDTH]; ++x) {				
				//std::cout << "(" << x << "," << y << ") = " << height(Vec2(x,y)) << std::endl;
				Vec3 v(x * scale[X], y * scale[Y], height(Vec2(x,y)) * scale[Z] + offset);
				mesh.addVertex(v);
			}
		}
		
		/* Add triangles using points */
		unsigned w = size[WIDTH];
		for (unsigned y = 0; (y+1) < size[HEIGHT]; ++y) {
			for (unsigned x = 0; (x+1) < size[WIDTH]; ++x) {
				unsigned m = base + x + y*w;
				
				mesh.addTriangleSurface(m, m+w+1, m+w);
				mesh.addTriangleSurface(m, m+1, m+w+1);
			}
		}
	}

	/* Bivalent - 0 = black, 1 = white */
	void HeightMap::toImage (REF(IMutablePixelBuffer) image) {
		//unsigned byteOffset = sizeof(pixel_t) - image->bytesPerPixel();
		
		for (unsigned y = 0; y < (unsigned)image->size()[HEIGHT]; y += 1) {
			for (unsigned x = 0; x < (unsigned)image->size()[WIDTH]; x += 1) {
				// Get the height of the pixel in normalized pixel space
				RealT z = height(Vec2 (x, y));
				pixel_t pxl = (pixel_t)(z * image->maxPixelSize());
				
				image->writePixel(Vector<3,unsigned> (x,y,0), pxl);				
			}
		}
	}
	
	ImageHeightMap::ImageHeightMap (REF(IPixelBuffer) image) : m_image(image) {
	
	}
		
	RealT ImageHeightMap::height (const Vec2 &at) {
		//unsigned byteOffset = sizeof(pixel_t) - m_image->bytesPerPixel();
		pixel_t max = m_image->maxPixelSize();		
		pixel_t pxl = m_image->readPixel(Vector<3, unsigned> (at[X], at[Y], 0));
		
		byte_t * pxlp = (byte_t *)&pxl;
		pxlp[3] = 0;
		byte_t * maxp = (byte_t *)&max;
		maxp[3] = 0;
		
		double d = (double)pxl / (double)max;
		
		return (RealT)d;
	}
	
	BilinearHeightMap::BilinearHeightMap (HeightMap *input) : m_input(input) {
	
	}
	
	RealT BilinearHeightMap::height (const Vec2 &_at) {
		// Offset _at so our sample space is not misaligned because of filtering:
		Vec2 at(_at);
		at -= 0.5;
		
		//return m_input->height(at);
		RealT s[4];
		
		Vec2 t (at), j;
		t.floor();
		
		for (unsigned i = 0; i < 4; ++i) {
			s[i] = m_input->height((t + j));
			j.increment(Vec2(2, 2));
		}
		
		Vec2 f (at.copy().frac());
		//std::cout << "Frac: " << f << std::endl;
		RealT a = linearInterpolate(f[X], s[0], s[1]);
		RealT b = linearInterpolate(f[X], s[2], s[3]);
		RealT c = linearInterpolate(f[Y], a, b);
		
		return c;
	}
	
	ScaleHeightMap::ScaleHeightMap (HeightMap *input, Vec2 scale) : m_input(input), m_scale(scale) {
	
	}
		
	RealT ScaleHeightMap::height (const Vec2 &at) {
		return m_input->height(at * m_scale);
	}

	OffsetHeightMap::OffsetHeightMap (HeightMap *input, Vec2 offset) : m_input(input), m_offset(offset) {
	
	}
		
	RealT OffsetHeightMap::height (const Vec2 &at) {
		return m_input->height(at + m_offset);
	}
	
};
