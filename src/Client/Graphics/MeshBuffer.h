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
						
			/// Manages a set of vertices.
			class VertexArray : private NonCopyable {
			protected:
				GLuint _handle;
				
			public:
				GLuint handle() const { return _handle; }
				
				VertexArray();
				~VertexArray();
				
				void bind();
				void unbind();
				
				void enable(GLuint index) {			
					glEnableVertexAttribArray(index);
					
					check_graphics_error();
				}
				
				void disable(GLuint index) {
					glDisableVertexAttribArray(index);
					
					check_graphics_error();
				}
				
				// These functions facilitate canonical usage where data is stored in vertex buffers.
				void draw_elements(GLenum mode, GLsizei count, GLenum type);
				void draw_arrays(GLenum mode, GLint first, GLsizei count);
				
				void set_attribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, std::ptrdiff_t offset);
				
				class Attributes;
			};
			
			template <typename ElementT>
			struct BasicVertex {
				ElementT element;
			};
			
			class VertexBuffer : private NonCopyable {
			protected:
				GLuint _handle;
				GLenum _target;
				
			public:
				GLuint handle() const { return _handle; }
				GLenum target() const { return _target; }
				
				VertexBuffer(GLenum target = GL_ARRAY_BUFFER) : _target(target) {
					glGenBuffers(1, &_handle);
					
					check_graphics_error();
				}
				
				~VertexBuffer() {
					glDeleteBuffers(1, &_handle);
					
					check_graphics_error();
				}
				
				void resize(std::size_t size, GLenum usage) {
					glBufferData(_target, size, NULL, usage);
					
					check_graphics_error();
				}
				
				void assign(void * data, std::size_t size, GLenum usage) {
					glBufferData(_target, size, data, usage);
					
					check_graphics_error();
				}
				
				void assign_partial(void * data, std::size_t offset, std::size_t size) {
					glBufferSubData(_target, offset, size, data);
					
					check_graphics_error();
				}
				
				template <typename ArrayT>
				void assign(const ArrayT & array, GLenum usage = GL_STREAM_DRAW) {
					assign((void *)array.data(), array.size() * sizeof(typename ArrayT::value_type), usage);
				}
				
				void * map(GLenum access = GL_WRITE_ONLY) {
					return glMapBuffer(_target, access);
				}
				
				void * map_partial(std::size_t offset, std::size_t size, GLenum access = GL_WRITE_ONLY) {
					return glMapBufferRange(_target, offset, size, access);
				}
				
				void unmap() {
					glUnmapBuffer(_target);
					
					check_graphics_error();
				}
				
				void attach(VertexArray & vertex_array) {
					glBindBuffer(_target, _handle);
					
					check_graphics_error();
				}
			};
			
			class IndexBuffer : public VertexBuffer {
			public:
				IndexBuffer() : VertexBuffer(GL_ELEMENT_ARRAY_BUFFER) {
				}
			};
			
			// http://www.opengl.org/wiki/Buffer_Object_Streaming
			class VertexArray::Attributes : private NonCopyable {
			protected:
				VertexArray & _vertex_array;
				VertexBuffer & _vertex_buffer;
				
				void bind() {
					_vertex_array.bind();
					_vertex_buffer.attach(_vertex_array);
				}
				
			public:
				Attributes(VertexArray & vertex_array, VertexBuffer & vertex_buffer) : _vertex_array(vertex_array), _vertex_buffer(vertex_buffer) {
					bind();
				}
				
				template <typename MeshBufferT>
				Attributes(MeshBufferT mesh_buffer) : _vertex_array(mesh_buffer->vertex_array()), _vertex_buffer(mesh_buffer->vertex_buffer()) {
					bind();
				}
				
				~Attributes() {
					_vertex_array.unbind();
				}
				
				struct Location {
					Attributes & attributes;
					GLuint index;
					
					template <class T, typename U>
					void operator=(U T::* member) {
						attributes.associate(index, member);
					}
				};
				
				Location operator[](GLuint index) {
					return (Location){*this, index};
				}
				
				template<class T, typename U>
				void associate(GLuint index, U T::* member, bool normalized = false) {
					_vertex_array.set_attribute(index, U::ELEMENTS, GLTypeTraits<typename U::ElementT>::TYPE, normalized, sizeof(T), member_offset(member));
					
					// We assume that the attributes are enabled by default:
					_vertex_array.enable(index);
				}
			};
			
			template <typename MeshT>
			class MeshBuffer : public Object {
			protected:
				Shared<MeshT> _mesh;
				
				VertexArray _vertex_array;
				
				GLenum _index_buffer_usage, _vertex_buffer_usage, _default_draw_mode;
				
				VertexBuffer _index_buffer;
				VertexBuffer _vertex_buffer;
				
				std::size_t _count;
				
				bool _invalid;
				
				void upload_buffers() {
					DREAM_ASSERT(_mesh != NULL);
					
					_index_buffer.attach(_vertex_array);
					_index_buffer.assign(_mesh->indices, _index_buffer_usage);
					
					_vertex_buffer.attach(_vertex_array);
					_vertex_buffer.assign(_mesh->vertices, _vertex_buffer_usage);
											
					// Keep track of the number of indices uploaded for drawing:
					_count = _mesh->indices.size();
					
					// The mesh buffer is now okay for drawing:
					_invalid = false;
				}
				
			public:
				MeshBuffer(Shared<MeshT> mesh = NULL) : _mesh(mesh), _index_buffer_usage(GL_STATIC_DRAW), _vertex_buffer_usage(GL_STATIC_DRAW), _index_buffer(GL_ELEMENT_ARRAY_BUFFER), _default_draw_mode(GL_TRIANGLE_STRIP), _vertex_buffer(GL_ARRAY_BUFFER), _invalid(true) {
					
				}
				
				virtual ~MeshBuffer() {
					
				}
				
				VertexArray & vertex_array() { return _vertex_array; }
				VertexBuffer & index_buffer() { return _index_buffer; }
				VertexBuffer & vertex_buffer() { return _vertex_buffer; }
				
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
						_vertex_array.draw_elements(mode, (GLsizei)_count, GLTypeTraits<typename MeshT::IndexT>::TYPE);
					}
					
					_vertex_array.unbind();
				}
			};
			
		}
	}
}

#endif
