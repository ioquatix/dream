/*
 *  ParticleRenderer.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 13/08/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "ParticleRenderer.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
		
			void drawQuads (unsigned count, const IndicesT& indices, const std::vector<Vec3>& vertexArray, 
				const std::vector<Vec2>& texCoordArray, const std::vector<Vec4>& colorArray)
			{
				if (count == 0) return;
				
				ensure((indices.size() * 6) >= count);
				
				glVertexPointer(3, GLTypeTraits<RealT>::TYPE, 0, &vertexArray[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glTexCoordPointer(2, GLTypeTraits<RealT>::TYPE, 0, &texCoordArray[0]);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				glColorPointer(4, GLTypeTraits<RealT>::TYPE, 0, &colorArray[0]);
				glEnableClientState(GL_COLOR_ARRAY);
				
				glDepthMask(GL_FALSE);
				glDisable(GL_LIGHTING);
				//glDisable(GL_DEPTH_TEST);
				glEnable(GL_COLOR_MATERIAL);	
				
				glDrawElements(GL_TRIANGLES, count * 6, GL_UNSIGNED_SHORT, &indices[0]);
				
				glDisable(GL_COLOR_MATERIAL);
				//glEnable(GL_DEPTH_TEST);
				glEnable(GL_LIGHTING);
				glDepthMask(GL_TRUE);
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
			}
		
		}
		
		/*
		// Example of using point sprites
		void ShadedParticleSystem::render(Renderer *r) const {
			glPushAttrib(GL_ENABLE_BIT);
			if (m_shader) m_shader->enable();
			glDisable(GL_TEXTURE_3D);
			glDisable(GL_LIGHTING);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			//if (m_textures.size()) m_textures[0]->enable();
			
			// This is how will our point sprite's size will be modified by 
			// distance from the viewer.
			RealT size = 14;
			//float quadratic[] =  { 60.0f, 0.0f, 0.0f };
			//glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic);

			// The alpha of a point is calculated to allow the fading of points 
			// instead of shrinking them past a defined threshold size. The threshold 
			// is defined by GL_POINT_FADE_THRESHOLD_SIZE_ARB and is not clamped to 
			// the minimum and maximum point sizes.
			glPointParameterfARB(GL_POINT_FADE_THRESHOLD_SIZE_ARB, 60.0f);

			float fAdjustedSize = size / 1.0f;

			glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, 1.0f);
			glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, fAdjustedSize);

			// Specify point sprite texture coordinate replacement mode for each texture unit
			glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

			//
			// Render point sprites...
			//
			
			unsigned i = 0;
			if (m_textures.size()) m_textures[i % m_textures.size()]->use();
			
			glEnable(GL_POINT_SPRITE_ARB);
			glPointSize(size);

			glBegin(GL_POINTS);
			for (iterateEach(m_particles, p)) {
				
				if ((*p)->active()) {
					(*p)->render(r);
				}
				
			}
			glEnd();
			
			if (m_shader) m_shader->disable();
			glPopAttrib();
		}
		
		*/
	}
}
