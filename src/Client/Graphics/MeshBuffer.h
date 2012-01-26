//
//  Client/Graphics/MeshBuffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_MESHBUFFER_H
#define _DREAM_CLIENT_GRAPHICS_MESHBUFFER_H

#include "Graphics.h"
#include "../../Geometry/Mesh.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			class VertexArray : private NonCopyable {
			protected:
				GLuint _handle;
				
			public:
				GLuint handle() const { return _handle; }
				
				VertexArray() {
					glGenVertexArrays(1, &_handle);
					
					check_error();
				}
				
				~VertexArray() {
					glDeleteVertexArrays(1, &_handle);
					
					check_error();
				}
				
				void bind() {
					glBindVertexArray(_handle);
					
					check_error();
				}
				
				void unbind() {
					glBindVertexArray(0);
					
					check_error();
				}
				
				// These functions facilitate canonical usage where data is stored in vertex buffers.
				void draw(GLenum mode, GLsizei count, GLenum type) {
					glDrawElements(mode, count, type, 0);
					
					check_error();
				}
				
				void set_attribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, std::ptrdiff_t offset) {
					glVertexAttribPointer(index, size, type, normalized, stride, (const GLvoid *)offset);
					
					check_error();
				}
			};
			
			class VertexBuffer : private NonCopyable {
			protected:
				GLuint _handle;
				GLenum _target;
				
			public:
				GLuint handle() const { return _handle; }
				GLenum target() const { return _target; }
				
				template <typename ArrayT>
				void buffer_data(const ArrayT & array, GLenum usage) {
					glBufferData(_target, array.length(), array.data(), usage);
					
					check_error();
				}
				
				VertexBuffer(GLenum target) : _target(target) {
					glGenBuffers(1, &_handle);
					
					check_error();
				}
				
				~VertexBuffer() {
					glDeleteBuffers(1, &_handle);
					
					check_error();
				}
				
				void attach(VertexArray & vertex_array) {
					glBindBuffer(_target, _handle);
					
					check_error();
				}
			};
			
			template <typename MeshBufferT>
			class Associations;
			
			template <typename MeshT>
			class MeshBuffer : public Object {
			protected:
				friend class Associations<MeshBuffer>;
				
				Shared<MeshT> _mesh;
				
				VertexArray _vertex_array;
				
				GLenum _index_buffer_usage, _vertex_buffer_usage, _default_draw_mode;
				
				VertexBuffer _index_buffer;
				VertexBuffer _vertex_buffer;
				
				std::size_t _count;
				
				bool _invalid;
				
				void start_associations() {
					_vertex_array.bind();
					_vertex_buffer.attach(_vertex_array);
				}
				
				template<class T, typename U>
				void associate(GLuint index, U T::* member, bool normalized = false) {
					_vertex_array.set_attribute(index, U::ELEMENTS, GLTypeTraits<typename U::ElementT>::TYPE, normalized, sizeof(T), member_offset(member));
					
					// We assume that the attributes are enabled by default:
					enable(index);
				}
				
				
				void finish_associations() {
					_vertex_array.unbind();
				}
				
				void upload_buffers() {
					if (_mesh) {
						_index_buffer.attach(_vertex_array);
						_index_buffer.buffer_data(_mesh->indices, _index_buffer_usage);
						
						_vertex_buffer.attach(_vertex_array);
						_vertex_buffer.buffer_data(_mesh->vertices, _vertex_buffer_usage);
						
						_vertex_array.unbind();
						
						// Keep track of the number of indices uploaded for drawing:
						_count = _mesh->indices.size();
						
						// The mesh buffer is now okay for drawing:
						_invalid = false;
					}
				}
				
			public:
				MeshBuffer(Shared<MeshT> mesh = NULL) : _mesh(mesh), _index_buffer_usage(GL_STATIC_DRAW), _vertex_buffer_usage(GL_STATIC_DRAW), _index_buffer(GL_ELEMENT_ARRAY_BUFFER), _default_draw_mode(GL_TRIANGLE_STRIP), _vertex_buffer(GL_ARRAY_BUFFER), _invalid(true) {
					
				}
				
				virtual ~MeshBuffer() {
					
				}
				
				Associations<MeshBuffer> associations() {
					return Associations<MeshBuffer>(*this);
				}
				
				void prepare() {
					// Doing this here multiple times for multiple associations seems inefficient...
					_vertex_array.bind();
					_vertex_buffer.attach(_vertex_array);
				}
				
				void enable(GLuint index) {			
					glEnableVertexAttribArray(index);
					
					check_error();
				}
				
				void disable(GLuint index) {
					glDisableVertexAttribArray(index);
					
					check_error();
				}
				
				void set_default_draw_mode(GLenum mode) {
					_default_draw_mode = mode;
				}
				
				void set_usage(GLenum usage) {
					_index_buffer_usage = _vertex_buffer_usage = usage;
				}
				
				void set_index_buffer_usage(GLenum usage) {
					_index_buffer_usage = usage;
				}
				
				void set_vertex_buffer_usage(GLenum usage) {
					_vertex_buffer_usage = usage;
				}
				
				void set_mesh(Shared<MeshT> & mesh) {
					if (_mesh != mesh) {
						_mesh = mesh;				
					}
					
					_invalid = true;
				}
				
				void set_mesh(Shared<MeshT> & mesh, GLenum default_draw_mode) {
					set_mesh(mesh);
					
					set_default_draw_mode(default_draw_mode);
				}
				
				void invalidate() {
					_invalid = true;
				}
				
				bool valid() {
					return !_invalid;
				}
				
				void upload() {
					if (_invalid && _mesh) {				
						_vertex_array.bind();
						
						upload_buffers();
						
						_vertex_array.unbind();
					}
				}
				
				void draw() {
					draw(_default_draw_mode);
				}
				
				void draw(GLenum mode) {
					_vertex_array.bind();
					
					if (_invalid) {
						upload_buffers();
					}
					
					if (!_invalid) {
						_vertex_array.bind();
						_vertex_array.draw(mode, (GLsizei)_count, GLTypeTraits<typename MeshT::IndexT>::TYPE);
						_vertex_array.unbind();
					}
					
					_vertex_array.unbind();
				}
			};
			
			template <typename MeshBufferT>
			class Associations {
			protected:
				MeshBufferT & _mesh_buffer;
				
			public:
				Associations(MeshBufferT & mesh_buffer) : _mesh_buffer(mesh_buffer) {
					_mesh_buffer.start_associations();
				}
				
				~Associations() {
					_mesh_buffer.finish_associations();
				}
				
				struct Location {
					Associations & associations;
					GLuint index;
					
					template<class T, typename U>
					void operator=(U T::* member) {
						associations._mesh_buffer.associate(index, member);
					}
				};
				
				Location operator[](GLuint index) {
					return (Location){*this, index};
				}
				
				template<class T, typename U>
				void associate(GLuint index, U T::* member, bool normalized = false) {
					_mesh_buffer.associate(index, member, normalized);
				}
			};
			
		}
	}
}

#endif
