/*
 *  Client/Display/OpenGL20/VertexBuffer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 19/10/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_VERTEXBUFFER_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_VERTEXBUFFER_H

#include "Graphics.h"
#include "OpenGL.h"

#include <vector>

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			//For further information:
			//http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_buffer_object.txt
			
			class Buffer : public Object {
			protected:
				GLuint m_id;
				GLenum m_target;
				
			public:
				class Class : public Object::Class {
				public:
					EXPOSE_CLASSTYPE
				};
				
				EXPOSE_CLASS(VertexBuffer)
				
				enum Target {
					Array = GL_ARRAY_BUFFER,
					ElementArray = GL_ELEMENT_ARRAY_BUFFER,
					PixelPackBuffer = GL_PIXEL_PACK_BUFFER_ARB,
					PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER_ARB
				};
				
				enum Access {
					Read = GL_READ_ONLY,
					Write = GL_WRITE_ONLY,
					ReadWrite = GL_READ_WRITE
				};
				
				enum Mode {
					StreamDraw = GL_STREAM_DRAW,
					//StreamRead = GL_STREAM_READ,
					//StreamCopy = GL_STREAM_COPY,
					StaticDraw = GL_STATIC_DRAW,
					//StaticRead = GL_STATIC_READ,
					//StaticCopy = GL_STATIC_COPY,
					DynamicDraw = GL_DYNAMIC_DRAW,
					//DynamicRead = GL_DYNAMIC_READ,
					//DynamicCopy = GL_DYNAMIC_COPY
				};
				
				VertexBuffer (const Target & t = Array);
				~VertexBuffer ();
				
				void bind () const;
				void unbind () const;
				
				void setData (IndexT length, const ByteT *data, const Mode &usage);
				void setData (IndexT offset, IndexT length, const ByteT *data);
				
				//ByteT* getData (IndexT offset, IndexT length
				
				ByteT* mapBuffer (const Access &access);
				void unmapBuffer ();
			};
			
			template <unsigned E, typename t = real_t, VertexBuffer::Target TARGET = VertexBuffer::Array>
			class BufferedArray {
			public:
				typedef Vector<E, t> element_t;
			
			protected:
				REF(VertexBuffer) m_buffer;
				bool m_compacted;
				std::vector<element_t> m_vector;
			
			public:
				BufferedArray () : m_compacted(false) {
					
				}
			
				const std::vector<element_t> & vector () const { return m_vector; }
				std::vector<element_t> & vector () { return m_vector; }
			
				void bind () const {
					if (m_buffer) m_buffer->bind();
				}
				
				void bind (GLenum type) const {
					bind();
					glEnableClientState(type);
				}
				
				void unbind () const {
					if (m_buffer) m_buffer->unbind();
				}
				
				void unbind (GLenum type) const {
					unbind();
					glDisableClientState(type);
				}
				
				void upload (const VertexBuffer::Mode &mode = VertexBuffer::StaticDraw) {
					ensure(!m_compacted);
					
					m_buffer = ptr(new VertexBuffer(TARGET));
					m_buffer->bind();
					m_buffer->setData(m_vector.size() * sizeof(t) * E, (ByteT*)&(m_vector[0]), mode);
				}
				
				void compact () {
					// Release locally allocated memory..
					// But only if we have uploaded to gfx card.
					if (m_buffer && !m_compacted) {
						m_vector.clear(); // Virtual function to clear allocated memory
						m_compacted = true;
					}
				}
				
				const void* base (const IndexT &offset = 0) const {
					if (m_buffer) {
						return (void*)(offset * sizeof(element_t));
					} else {
						ensure(!m_compacted);
						
						return &(m_vector[offset]);
					}
				}
				
			};
			
			template <unsigned E, typename t = real_t>
			class VectorArray : public BufferedArray<E, t, VertexBuffer::Array> {
			public:
				void glVertexPointer () const {
					::glVertexPointer(E, GL<t>::TYPE, 0, this->base());
				}
		
				void glTexCoordPointer () const {
					::glTexCoordPointer(E, GL<t>::TYPE, 0, this->base());
				}
		
				void glColorPointer () const {
					::glColorPointer(E, GL<t>::TYPE, 0, this->base());
				}
				
				void glNormalPointer () const {
					::glNormalPointer(GL<t>::TYPE, 0, this->base());
				}
			};

			template <typename _index_t = GLuint>
			struct IndexArray : public BufferedArray<1, _index_t, VertexBuffer::ElementArray> {
			public:
				typedef _index_t index_t;	

				void glDrawElements(GLenum mode) const {
					::glDrawElements(mode, this->m_vector.size(), GL<index_t>::TYPE, this->base());
				}
			};

		}
	}
}

#endif
