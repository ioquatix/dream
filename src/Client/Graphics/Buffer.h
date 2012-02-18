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
				
				template <typename ElementT>
				class Binding : private NonCopyable {
				protected:
					BufferHandle * _buffer_handle;
					
					GLsizeiptr byte_offset(std::size_t offset) {
						return (GLsizeiptr)(sizeof(ElementT) * offset);
					}
					
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
					
					void size() {
						return _buffer_handle->_size / sizeof(ElementT);
					}
					
					void resize(std::size_t size) {
						std::size_t data_size = byte_offset(size);
						
						_buffer_handle->_size = data_size;
						
						glBufferData(TARGET, data_size, NULL, _buffer_handle->usage());
					}
					
					void resize(std::size_t size, GLenum usage) {
						_buffer_handle->_usage = usage;
						
						resize(size);
					}
					
					void set_data(const ElementT * data, std::size_t size) {
						_buffer_handle->_size = size;
						
						glBufferData(TARGET, byte_offset(size), data, _buffer_handle->usage());
					}
					
					template <typename ArrayT> 
					void set_data (const ArrayT & array) {
						set_data(array.data(), array.size());
					}
					
					void set_partial_data(const void * data, std::size_t offset, std::size_t size) {
						glBufferSubData(TARGET, byte_offset(offset), byte_offset(size), data);
					}
					
					ElementT * map(GLenum access = GL_WRITE_ONLY) {
						return (ElementT *)glMapBuffer(TARGET, access);
					}
					
					ElementT * map(std::size_t offset, std::size_t size, GLenum access = GL_WRITE_ONLY) {
						DREAM_ASSERT(offset + size < _buffer_handle->size());
						
						return (ElementT *)glMapBufferRange(TARGET, byte_offset(offset), byte_offset(size), access);
					}
					
					void unmap() {
						glUnmapBuffer(TARGET);
					}
					
					BufferedArray<ElementT> array(GLenum access = GL_WRITE_ONLY) {
						return BufferedArray<ElementT>((ElementT *)map(access), _buffer_handle->size());
					}
					
					BufferedArray<ElementT> array(std::size_t offset, std::size_t size, GLenum acccess = GL_WRITE_ONLY) {
						return BufferedArray<ElementT>(map(offset, size), size);
					}
				};
				
				template <typename ElementT>
				Binding<ElementT> binding() {
					Binding<ElementT> binding(this);
					
					return std::move(binding);
				}
			};
			
			template <GLenum TARGET, typename ElementT>
			class GraphicsBuffer : public BufferHandle<TARGET> {
			public:
				// Wow...
				typedef typename BufferHandle<TARGET>::template Binding<ElementT> BindingT;
				
				GraphicsBuffer(GLenum usage) : BufferHandle<TARGET>(usage) {
				}
				
				using BufferHandle<TARGET>::binding;
				
				BindingT binding() {
					BindingT binding(this);
					
					return std::move(binding);
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
