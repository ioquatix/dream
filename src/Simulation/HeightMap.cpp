//
//  Simulation/HeightMap.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/10/06.
//  Copyright 2006 Samuel Williams. All rights reserved.
//
//

#include "HeightMap.h"

namespace Dream {
	namespace Simulation {
		
		HeightMap::HeightMap () {
		
		}
			
		HeightMap::~HeightMap () {
		
		}
		
		/*
		void HeightMap::to_mesh (Mesh & mesh, Vec2u size, Vec3 scale, RealT offset) {
			std::size_t base = mesh.vertices().size();
			
			// Add points in the mesh
			for (std::size_t y = 0; y < size[HEIGHT]; ++y) {
				for (std::size_t x = 0; x < size[WIDTH]; ++x) {				
					//std::cout << "(" << x << "," << y << ") = " << height(Vec2(x,y)) << std::endl;
					Vec3 v(x * scale[X], y * scale[Y], height(Vec2(x,y)) * scale[Z] + offset);
					mesh.addVertex(v);
				}
			}
			
			// Add triangles using points
			std::size_t w = size[WIDTH];
			for (std::size_t y = 0; (y+1) < size[HEIGHT]; ++y) {
				for (std::size_t x = 0; (x+1) < size[WIDTH]; ++x) {
					std::size_t m = base + x + y*w;
					
					mesh.addTriangleSurface(m, m+w+1, m+w);
					mesh.addTriangleSurface(m, m+1, m+w+1);
				}
			}
		}

		// Bivalent - 0 = black, 1 = white
		void HeightMap::to_image (REF(IMutablePixelBuffer) image) {
			//std::size_t byteOffset = sizeof(pixel_t) - image->bytesPerPixel();
			
			for (std::size_t y = 0; y < (std::size_t)image->size()[HEIGHT]; y += 1) {
				for (std::size_t x = 0; x < (std::size_t)image->size()[WIDTH]; x += 1) {
					// Get the height of the pixel in normalized pixel space
					RealT z = height(Vec2 (x, y));
					pixel_t pxl = (pixel_t)(z * image->maxPixelSize());
					
					image->writePixel(Vector<3,std::size_t> (x,y,0), pxl);				
				}
			}
		}
		*/

		ImageHeightMap::ImageHeightMap (Ref<IPixelBuffer> image) : _image(image) {
		
		}
			
		RealT ImageHeightMap::height (const Vec2 &at) {
			//std::size_t byteOffset = sizeof(pixel_t) - _image->bytesPerPixel();
			PixelT max = _image->max_pixel_size();
			PixelT pixel = _image->read_pixel(Vec3u(at[X], at[Y], 0));
			
			ByteT * pixel_data = (ByteT *)&pixel;
			pixel_data[3] = 0;
			ByteT * max_data = (ByteT *)&max;
			max_data[3] = 0;
			
			return RealT(pixel) / RealT(max);			
		}
		
		BilinearHeightMap::BilinearHeightMap (HeightMap *input) : _input(input) {
		
		}
		
		RealT BilinearHeightMap::height (const Vec2 &_at) {
			// Offset _at so our sample space is not misaligned because of filtering:
			Vec2 at(_at);
			at -= 0.5;
			
			//return _input->height(at);
			RealT s[4];
			
			Vec2 t (at), j;
			t.floor();
			
			for (std::size_t i = 0; i < 4; ++i) {
				s[i] = _input->height((t + j));
				j.increment(Vec2(2, 2));
			}
			
			Vec2 f (at.copy().frac());
			//std::cout << "Frac: " << f << std::endl;
			RealT a = linear_interpolate(f[X], s[0], s[1]);
			RealT b = linear_interpolate(f[X], s[2], s[3]);
			RealT c = linear_interpolate(f[Y], a, b);
			
			return c;
		}
		
		ScaleHeightMap::ScaleHeightMap (HeightMap *input, Vec2 scale) : _input(input), _scale(scale) {
		
		}
			
		RealT ScaleHeightMap::height (const Vec2 &at) {
			return _input->height(at * _scale);
		}

		OffsetHeightMap::OffsetHeightMap (HeightMap *input, Vec2 offset) : _input(input), _offset(offset) {
		
		}
			
		RealT OffsetHeightMap::height (const Vec2 &at) {
			return _input->height(at + _offset);
		}
	}
}
