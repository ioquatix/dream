/*
 *  ParticleRenderer.h
 *  Dream
 *
 *  Created by Samuel Williams on 13/08/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_PARTICLERENDERER_H
#define _DREAM_CLIENT_GRAPHICS_PARTICLERENDERER_H

#include "Graphics.h"

// Dream::Core::TimedSystem
#include "../../Core/Timer.h"
#include "../../Core/STLAdditions.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{

			template <typename IndexT>
			void setupQuadIndices (unsigned numberOfQuads, std::vector<IndexT> & indices)
			{
				IndexT INDICES[] = {0, 1, 3, 1, 3, 2};
				
				unsigned base = indices.size() / 6;
				int diff = numberOfQuads - base;
				
				while (diff > 0) {
					for (unsigned j = 0; j < 6; j += 1) {
						indices.push_back((base * 4) + INDICES[j]);
					}
					
					base += 1;
					diff -= 1;
				}
			}
			
			inline RealT realRandom ()
			{
				return (RealT)rand() / (RealT)RAND_MAX;
			}
			
			inline unsigned integralRandom (unsigned max)
			{
				return unsigned(realRandom() * max) % max;
			}

			typedef std::vector<GLushort> IndicesT;

			void drawQuads (unsigned count, const IndicesT& indices, const std::vector<Vec3>& vertexArray, 
				const std::vector<Vec2>& texCoordArray, const std::vector<Vec4>& colorArray);

			template <typename ParticlesT>
			class QuadParticleRenderer : public TimedSystem<ParticlesT>
			{
				protected:
					struct Physics : public ParticlesT::Particle
					{
						Vec3 velocity;
						Vec3 position;
						
						Vec3 points[4];
						Vec2 texCoords[4];
						
						Vec3 color;
						RealT colorModulator;
						
						TimeT life;
						
						void setTextureQuadrant (const Vec2u & count, const Vec2u index) {
							Vec2 size = Vec2(IDENTITY) / count;
							Vec2 offset = size * index;
							
							texCoords[0] = offset; offset[X] += size[X];
							texCoords[1] = offset; offset[Y] += size[Y];
							texCoords[2] = offset; offset[X] -= size[X];
							texCoords[3] = offset;
						}
						
						void setRandomQuadrant (const Vec2u & count) {
							setTextureQuadrant(count, Vec2u(integralRandom(count[X]), integralRandom(count[Y])));
						}
						
						void setQuad (Vec3 pos, Vec3 up, Vec3 forward, RealT rotation) {
							using Dream::Numerics::Mat44;
							
							position = pos;
							
							if (rotation != 0.0) {
								up = Mat44::rotatingMatrix(rotation, forward) * up;
							}
							
							Mat44 rot = Mat44::rotatingMatrix(R90, forward);
							for (unsigned i = 0; i < 4; i += 1) {
								points[i] = up;
								up = rot * up;
							}
						}
						
						void applyForce (RealT dt, const Vec3 & force) {

						}
						
						void updateTime (RealT dt, const Vec3 & force) {
							life -= dt;
							colorModulator += dt;
							
							position = position + (velocity * dt) + (force * dt * dt * 0.5);
							velocity += force * dt;
						}
						
						RealT calculateAlpha (RealT timeout) {
							if (life < timeout)
								return life / timeout;
							else 
								return 1.0;
						}
					};
					
					void resetArrays () {
						m_vertexArray.clear();
						m_texCoordArray.clear();
						m_colorArray.clear();
					}
					
					void addParticle (const Physics & p) {
						for (unsigned j = 0; j < 4; j += 1) {
							m_vertexArray.push_back(p.position + p.points[j]);
							m_texCoordArray.push_back(p.texCoords[j]);
						}
					}
					
					void addParticle (const Physics & p, const Vec4 & color) {
						addParticle(p);
						
						for (unsigned j = 0; j < 4; j += 1) {
							m_colorArray.push_back(color);
						}	
					}
					
					struct Particle
					{
					};
					
				public:
					void draw () {
						drawQuads(m_physics.size(), m_indices, m_vertexArray, m_texCoordArray, m_colorArray);
					}
					
					std::vector<GLushort> m_indices;
					std::vector<Physics> m_physics;
					std::vector<Vec3> m_vertexArray;
					std::vector<Vec2> m_texCoordArray;
					std::vector<Vec4> m_colorArray;
			};
		}
	}
}

#endif
