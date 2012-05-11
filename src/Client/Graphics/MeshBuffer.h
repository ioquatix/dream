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
								
				typedef IndexBuffer<typename MeshT::IndexT> IndexBufferT;
				typedef VertexBuffer<typename MeshT::VertexT> VertexBufferT;
				
				IndexBufferT _index_buffer;
				VertexBufferT _vertex_buffer;
				
				std::size_t _count;
				
				bool _invalid;
				
				void upload_buffers() {
					DREAM_ASSERT(_mesh);
					
					{
						auto binding = _index_buffer.binding();
						binding.set_data(_mesh->indices);
						
						check_graphics_error();
					}
					
					{
						auto binding = _vertex_buffer.binding();
						binding.set_data(_mesh->vertices);
						
						check_graphics_error();
					}
					
					// Keep track of the number of indices uploaded for drawing:
					_count = _mesh->indices.size();
					
					// The mesh buffer is now okay for drawing:
					_invalid = false;
					
					check_graphics_error();
				}
				
			public:
				MeshBuffer(Shared<MeshT> mesh = NULL) : _mesh(mesh), _invalid(true) {
				}
				
				virtual ~MeshBuffer() {
					
				}
				
				void set_usage(GLenum usage) {
					_index_buffer.set_usage(usage);
					_vertex_buffer.set_usage(usage);
				}
				
				void set_mesh(Shared<MeshT> & mesh) {
					if (_mesh != mesh) {
						_mesh = mesh;				
					}
					
					_invalid = true;
				}
				
				Shared<MeshT> mesh() {
					return _mesh;
				}
				
				void invalidate() {
					_invalid = true;
				}
				
				bool valid() {
					return !_invalid;
				}
				
				void upload() {
					if (_invalid && _mesh) {
						upload_buffers();
					}
				}
				
				void draw() {
					if (_invalid) {
						upload_buffers();
					}
					
					if (!_invalid) {
						auto binding = _vertex_array.binding();
						binding.draw_elements(_mesh->layout, (GLsizei)_count, GLTypeTraits<typename MeshT::IndexT>::TYPE);
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
