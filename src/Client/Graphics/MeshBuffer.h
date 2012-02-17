//
//  Client/Graphics/MeshBuffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_MESHBUFFER_H
#define _DREAM_CLIENT_GRAPHICS_MESHBUFFER_H

#include "VertexArray.h"
#include "../../Geometry/Mesh.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			template <typename MeshT>
			class MeshBuffer : public Object {
			protected:
				Shared<MeshT> _mesh;
				
				VertexArray _vertex_array;
				
				GLenum _index_buffer_usage, _vertex_buffer_usage, _default_draw_mode;
				
				typedef IndexBuffer<typename MeshT::IndexT> IndexBufferT;
				typedef VertexBuffer<typename MeshT::VertexT> VertexBufferT;
				
				IndexBufferT _index_buffer;
				VertexBufferT _vertex_buffer;
				
				std::size_t _count;
				
				bool _invalid;
				
				void upload_buffers(VertexArray::Binding & binding) {
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
						auto binding = _vertex_array.binding();
						upload_buffers(binding);
					}
				}
				
				void draw() {
					draw(_default_draw_mode);
				}
				
				void draw(GLenum mode) {
					auto binding = _vertex_array.binding();
					
					if (_invalid) {
						upload_buffers(binding);
					}
					
					if (!_invalid) {
						binding.draw_elements(mode, (GLsizei)_count, GLTypeTraits<typename MeshT::IndexT>::TYPE);
					}
				}
				
				VertexArray & vertex_array() {
					return _vertex_array;
				}
				
				IndexBufferT & index_buffer() {
					return _index_buffer;
				}
				
				VertexBufferT & vertex_buffer() {
					return _vertex_buffer;
				}
			};
			
		}
	}
}

#endif
