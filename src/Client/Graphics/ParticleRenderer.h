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
#include "MeshBuffer.h"
#include "ShaderManager.h"
#include "../../Core/Timer.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			
			/// Provides an efficient way to erase elements from an unsorted std::vector
			template <typename t>
			bool erase_element_at_index (std::size_t index, std::vector<t> & array)
			{
				if (array.size() == (index+1)) {
					array.pop_back();
					return false;
				} else {
					array[index] = array.back();
					array.pop_back();
					return true;
				}
			}
			
			/// Setup an array of indices for rendering quadrilaterals as triangles
			template <typename IndexT>
			std::size_t setup_triangle_indicies(std::size_t count, std::vector<IndexT> & indices) {
				IndexT INDICES[] = {0, 1, 3, 1, 3, 2};
				
				std::size_t base = indices.size() / 6;
				
				if (count > base) {
					count += 256;
					
					logger()->log(LOG_DEBUG, LogBuffer() << "Generating " << (count - base) << " indices");
					
					std::size_t added = 0;
					
					while (base < count) {
						for (unsigned j = 0; j < 6; j += 1) {
							indices.push_back((base * 4) + INDICES[j]);
						}
						
						added += 6;
						base += 1;
					}
					
					return added;
				}
				
				return 0;
			}
			
			inline RealT real_random () {
				return (RealT)rand() / (RealT)RAND_MAX;
			}
			
			inline RealT real_random(RealT min, RealT max) {
				return min + (real_random() * (max - min));
			}
			
			inline unsigned integral_random (unsigned max) {
				return unsigned(real_random() * max) % max;
			}
			
			template <typename ParticlesT>
			class ParticleRenderer : public Object, public TimedSystem<ParticlesT> {
			protected:				
				struct Vertex {
					Vec3 position;
					Vec3 offset;
					Vec2 mapping;
					Vec4 color;
				};
				
				class Physics : public ParticlesT::Particle {
				protected:
					Vec3 _velocity;
					Vec3 _position;
					
					Vertex _vertices[4];
					
					Vec3 _color;
					RealT _color_modulator;
					
					RealT _life;
					
				public:
					Physics() : _velocity(ZERO), _color(1.0), _life(0) {
						_color_modulator = real_random();
					}
					
					void set_mapping(const Vec2u & count, const Vec2u index) {
						Vec2 size = Vec2(IDENTITY) / count;
						Vec2 offset = size * index;
						
						_vertices[0].mapping = offset; offset[X] += size[X];
						_vertices[1].mapping = offset; offset[Y] += size[Y];
						_vertices[2].mapping = offset; offset[X] -= size[X];
						_vertices[3].mapping = offset;
					}
					
					void set_random_mapping(const Vec2u & count) {
						set_mapping(count, Vec2u(integral_random(count[X]), integral_random(count[Y])));
					}
					
					void set_velocity(const Vec3 & velocity) {
						_velocity = velocity;
					}
					
					void set_position(Vec3 position, Vec3 up, Vec3 forward, RealT rotation) {
						using Dream::Numerics::Mat44;
						
						_position = position;
						
						if (rotation != 0.0) {
							up = Mat44::rotating_matrix(rotation, forward) * up;
						}
						
						Mat44 transform = Mat44::rotating_matrix(R90, forward);
						for (std::size_t i = 0; i < 4; i += 1) {
							_vertices[i].position = _position;
							_vertices[i].offset = up;
							up = transform * up;
						}
					}
					
					void set_color(const Vec3 & color) {
						_color = color;
					}
					
					void update_vertex_color(const Vec4 & color) {
						Vec4 sum = (_color << 0.0) + color;
						
						for (std::size_t i = 0; i < 4; i += 1)
							_vertices[i].color = sum;
					}
					
					void add_life(const RealT & amount) {
						_life += amount;
					}
					
					RealT color_modulation(RealT factor = 1.0) const {
						return Math::sin(_color_modulator * R360 * factor);
					}
					
					inline bool update_time (RealT dt, const Vec3 & force = ZERO) {
						_life -= dt;
						
						if (_life > 0) {
							_color_modulator += dt;
							
							_position += (_velocity * dt) + (force * dt * dt * 0.5);
							
							// Update vertex position:
							for (std::size_t i = 0; i < 4; i += 1) {
								_vertices[i].position = _position;
							}
							
							_velocity += force * dt;
							
							return true;
						}
						
						return false;
					}
					
					RealT calculate_alpha (RealT timeout) {
						if (_life < timeout)
							return _life / timeout;
						else 
							return 1.0;
					}
					
					void queue(std::vector<Vertex> & vertices) {
						std::copy(begin(_vertices), end(_vertices), std::back_inserter(vertices));
					}
				};
				
				std::vector<Physics> _physics;
				std::vector<Vertex> _vertices;
				std::vector<GLushort> _indices;
				
				VertexArray _vertex_array;
				IndexBuffer<GLushort> _indices_buffer;
				VertexBuffer<Vertex> _vertex_buffer;
				
			public:
				enum Attributes {
					POSITION = 0,
					OFFSET = 1,
					MAPPING = 2,
					COLOR = 3
				};
				
				ParticleRenderer() {
					auto binding = _vertex_array.binding();
					
					// Attach vertices buffer:
					auto attributes = binding.attach(_vertex_buffer);
					attributes[POSITION] = &Vertex::position;
					attributes[OFFSET] = &Vertex::offset;
					attributes[MAPPING] = &Vertex::mapping;
					attributes[COLOR] = &Vertex::color;
					
					// Attach indices buffer:
					binding.attach(_indices_buffer);
				}
				
				virtual ~ParticleRenderer() {	
				}
				
				void clear() {
					_vertices.clear();
				}
				
				void queue(Physics & physics) {
					physics.queue(_vertices);
				}
				
				void draw() {										
					// Number of particles to draw:
					std::size_t count = _vertices.size() / 4;
					
					// Setup indices for drawing quadrilaterals as triangles:
					std::size_t additions = setup_triangle_indicies(count, _indices);
					
					if (additions) {
						auto binding = _indices_buffer.binding();
						binding.set_data(_indices);
					}
					
					{
						auto binding = _vertex_buffer.binding();
						binding.set_data(_vertices);
					}
					
					{
						auto binding = _vertex_array.binding();
						binding.draw_elements(GL_TRIANGLES, count * 6, GLTypeTraits<GLushort>::TYPE);
					}
				}
				
				struct Particle
				{
				};
			};
		}
	}
}

#endif
