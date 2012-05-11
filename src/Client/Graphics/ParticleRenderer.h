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
#include "../../Core/Algorithm.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			
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
				
				struct Physics : public ParticlesT::Particle {
				protected:					
					Vertex _vertices[4];
					
				public:
					Vec3 velocity;
					Vec3 position;
					
					Vec3 color;
					RealT color_modulator;
					
					RealT life, age;

					
					Physics() : velocity(ZERO), color(1.0), life(0), age(0) {
						color_modulator = real_random();
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
					
					void set_position(Vec3 center, Vec3 up, Vec3 forward, RealT rotation) {
						using Dream::Numerics::Mat44;
						
						position = center;
						
						if (rotation != 0.0) {
							up = Mat44::rotating_matrix(rotation, forward) * up;
						}
						
						Mat44 transform = Mat44::rotating_matrix(R90, forward);
						for (std::size_t i = 0; i < 4; i += 1) {
							_vertices[i].position = position;
							_vertices[i].offset = up;
							up = transform * up;
						}
					}
					
					template <typename TransformT>
					void transform_offsets(const TransformT & transform) {
						for (std::size_t i = 0; i < 4; i += 1) {
							_vertices[i].offset = transform * _vertices[i].offset;
						}
					}
					
					void update_vertex_color(const Vec4 & offset_color) {
						Vec4 sum = (color << 0.0) + offset_color;
						
						for (std::size_t i = 0; i < 4; i += 1)
							_vertices[i].color = sum;
					}
					
					void add_life(const RealT & amount) {
						life += amount;
					}
					
					RealT color_modulation(RealT factor = 1.0) const {
						return Math::sin(color_modulator * R360 * factor);
					}
					
					inline bool update_time (RealT dt, const Vec3 & force = ZERO) {
						age += dt;
						
						if (age < life) {
							color_modulator += dt;
							
							position += (velocity * dt) + (force * dt * dt * 0.5);
							
							// Update vertex position:
							for (std::size_t i = 0; i < 4; i += 1) {
								_vertices[i].position = position;
							}
							
							velocity += force * dt;
							
							return true;
						}
						
						return false;
					}
					
					RealT calculate_alpha (RealT timeout) {
						RealT remaining = life - age;
						
						if (remaining < timeout)
							return remaining / timeout;
						else 
							return 1.0;
					}
					
					RealT calculate_alpha (RealT timeout, RealT ramp) {
						if (age < ramp) {
							return age / ramp;
						}
						
						return calculate_alpha(timeout);
					}
					
					void queue(Vertex * destination) {
						memcpy(destination, _vertices, sizeof(_vertices));
					}
				};
				
				std::vector<Physics> _physics;
				std::vector<GLushort> _indices;
				
				std::size_t _count;
				VertexArray _vertex_array;
				IndexBuffer<GLushort> _indices_buffer;
				VertexBuffer<Vertex> _vertex_buffer;
				
				std::size_t required_size() {
					return _physics.size() * 4 * sizeof(Vertex);
				}
				
			public:
				enum Attributes {
					POSITION = 0,
					OFFSET = 1,
					MAPPING = 2,
					COLOR = 3
				};
				
				ParticleRenderer() : _count(0) {
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
				
				// To use this function, make sure you implement
				// bool update_physics(Physics & physics, TimeT last_time, TimeT current_time, TimeT dt)
				void update_for_duration (TimeT last_time, TimeT current_time, TimeT dt)
				{
					if (_physics.size() == 0)
						return;
					
					auto binding = _vertex_buffer.binding();
					if (_vertex_buffer.size() < required_size())
						binding.resize(required_size());
					
					auto buffer = binding.array();
					
					std::size_t i = 0;
					while (i < _physics.size()) {
						Physics & physics = _physics[i];
						
						bool alive = static_cast<ParticlesT*>(this)->update_physics(physics, last_time, current_time, dt);
						
						if (alive) {
							// Add the particle to be drawn:
							physics.queue(&buffer[i*4]);
							
							i += 1;
						} else {
							erase_element_at_index(i, _physics);
						}
					}
					
					binding.unmap();
					
					_count = i;
				}
				
				void draw() {
					// If there is nothing to draw, bail out quickly.
					if (_count == 0)
						return;
					
					// Setup indices for drawing quadrilaterals as triangles:
					std::size_t additions = setup_triangle_indicies(_count, _indices);
					
					if (additions) {
						auto binding = _indices_buffer.binding();
						binding.set_data(_indices);
					}
					
					{
						auto binding = _vertex_array.binding();
						binding.draw_elements(GL_TRIANGLES, _count * 6, GLTypeTraits<GLushort>::TYPE);
					}
				}
				
				struct Particle {
				};
			};
			
		}
	}
}

#endif
