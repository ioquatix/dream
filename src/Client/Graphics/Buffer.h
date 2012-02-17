//
//  BufferHandle.h
//  Dream
//
//  Created by Samuel Williams on 15/02/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_BUFFER_H
#define _DREAM_CLIENT_GRAPHICS_BUFFER_H

#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
	
			template <typename ElementT>
			class BufferedArray {
			protected:
				ElementT * _base;
				std::size_t _size;
				
			public:
				BufferedArray(ElementT * base, std::size_t size) : _base(base), _size(size) {
				}
				
				ElementT & operator[](std::size_t index) {
					return _base[index];
				}
				
				ElementT * begin() {
					return _base;
				}
				
				ElementT * end() {
					return _base + _size;
				}
			};
			
			class VertexArray;
			
			template <GLenum TARGET>
			class BufferHandle : private NonCopyable {
			protected:
				GLenum _handle;
				GLenum _usage;
				std::size_t _size;
				
				void bind() {
					glBindBuffer(TARGET, _handle);
				}
				
				void unbind() {
					glBindBuffer(TARGET, 0);
				}
				
				friend class VertexArray;
				
			public:		
				BufferHandle(GLenum usage) : _usage(usage), _size(0) {
					glGenBuffers(1, &_handle);
				}
				
				~BufferHandle() {
					glDeleteBuffers(1, &_handle);
				}
				
				std::size_t size() const {
					return _size;
				}
				
				GLenum usage() const {
					return _usage;
				}
				
				class Binding : private NonCopyable {
				protected:
					BufferHandle * _buffer_handle;
					
				public:
					Binding(BufferHandle * buffer_handle) : _buffer_handle(buffer_handle) {
						_buffer_handle->bind();
					}
					
					Binding(Binding && other) : _buffer_handle(other._buffer_handle) {
						other._buffer_handle = NULL;
					}
					
					~Binding() {
						if (_buffer_handle)
							_buffer_handle->unbind();
					}
					
					void resize(std::size_t size) {
						_buffer_handle->_size = size;
						
						glBufferData(TARGET, (GLsizeiptr)size, NULL, _buffer_handle->usage());
					}
					
					void resize(std::size_t size, GLenum usage) {
						_buffer_handle->_usage = usage;
						resize(size);
					}
					
					void set_data(void * data, std::size_t size) {
						_buffer_handle->_size = size;
						
						glBufferData(TARGET, (GLsizeiptr)size, data, _buffer_handle->usage());
					}
					
					template <typename ArrayT> 
					void set_data (const ArrayT & array) {
						set_data((void *)array.data(), array.size() * sizeof(typename ArrayT::value_type));
					}
					
					void set_partial_data(void * data, std::size_t offset, std::size_t size) {
						glBufferSubData(TARGET, (GLsizeiptr)offset, (GLsizeiptr)size, data);
					}
					
					void * map_data(GLenum access = GL_WRITE_ONLY) {
						return glMapBuffer(TARGET, access);
					}
					
					void unmap_data() {
						glUnmapBuffer(TARGET);
					}
					
					template <typename ElementT>
					BufferedArray<ElementT> map_array(GLenum access = GL_WRITE_ONLY) {
						return BufferedArray<ElementT>(map_data(access), _buffer_handle->size());
					}
				};
				
				Binding binding() {
					Binding binding(this);
					
					return std::move(binding);
				}
			};
			
			template <GLenum TARGET, typename ElementT>
			class GraphicsBuffer : public BufferHandle<TARGET> {
			public:
				GraphicsBuffer(GLenum usage) : BufferHandle<TARGET>(usage) {
				}
				
				BufferedArray<ElementT> map(GLenum access = GL_WRITE_ONLY) {
					auto binding = this->binding();
					return binding.template map_array<ElementT>(access);
				}
				
				void commit() {
					auto binding = this->binding();
					binding.unmap_data();
				}
			};
			
			template <typename VertexT>
			class VertexBuffer : public GraphicsBuffer<GL_ARRAY_BUFFER, VertexT> {
			public:
				VertexBuffer(GLenum usage = GL_STREAM_DRAW) : GraphicsBuffer<GL_ARRAY_BUFFER, VertexT>(usage) {
				}
			};
			
			template <typename IndexT>
			class IndexBuffer : public GraphicsBuffer<GL_ELEMENT_ARRAY_BUFFER, IndexT> {
			public:
				IndexBuffer(GLenum usage = GL_STREAM_DRAW) : GraphicsBuffer<GL_ELEMENT_ARRAY_BUFFER, IndexT>(usage) {
				}
			};
		}
	}
}

#endif
