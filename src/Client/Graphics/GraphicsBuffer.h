/*
 *  Client/Display/OpenGL20/GraphicsBuffer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 19/10/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_GRAPHICSBUFFER_H
#define _DREAM_CLIENT_GRAPHICS_GRAPHICSBUFFER_H

#include "Graphics.h"
#include <vector>

namespace Dream {
	namespace Client {
		namespace Graphics {

			//For further information:
			//http://oss.sgi.com/projects/ogl-sample/registry/ARB/vertex_buffer_object.txt
			
			class GraphicsBuffer : public ObjectHandle {
			protected:
				GLenum m_target;
				
			public:
				class Class : public ObjectHandle::Class {
				public:
					EXPOSE_CLASSTYPE
				};
				
				EXPOSE_CLASS(GraphicsBuffer)
				
				enum Target {
					Array = GL_ARRAY_BUFFER,
					ElementArray = GL_ELEMENT_ARRAY_BUFFER
					// Not supported on OpenGL ES
					//PixelPackBuffer = GL_PIXEL_PACK_BUFFER_ARB,
					//PixelUnpackBuffer = GL_PIXEL_UNPACK_BUFFER_ARB,
				};
				
#ifndef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
				enum Access {
					Read = GL_READ_ONLY,
					Write = GL_WRITE_ONLY,
					ReadWrite = GL_READ_WRITE
				};
#endif
				
				enum Mode {
#ifndef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
					StreamDraw = GL_STREAM_DRAW,
#endif
					//StreamRead = GL_STREAM_READ,
					//StreamCopy = GL_STREAM_COPY,
					StaticDraw = GL_STATIC_DRAW,
					//StaticRead = GL_STATIC_READ,
					//StaticCopy = GL_STATIC_COPY,
					DynamicDraw = GL_DYNAMIC_DRAW,
					//DynamicRead = GL_DYNAMIC_READ,
					//DynamicCopy = GL_DYNAMIC_COPY
				};
				
				GraphicsBuffer (const Target & t = Array);
				~GraphicsBuffer ();
				
				void bind () const;
				void unbind () const;
				
				void setData (IndexT length, const ByteT *data, const Mode &usage);
				void setData (IndexT offset, IndexT length, const ByteT *data);
				
				//ByteT* getData (IndexT offset, IndexT length

#ifndef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
				ByteT* mapBuffer (const Access &access);
				void unmapBuffer ();
#endif
			};
			
			template <unsigned E, typename t = RealT, GraphicsBuffer::Target TARGET = GraphicsBuffer::Array>
			class BufferedArray {
			public:
				typedef Vector<E, t> ElementT;
			
			protected:
				REF(GraphicsBuffer) m_buffer;
				bool m_compacted;
				std::vector<ElementT> m_vector;
			
			public:
				BufferedArray () : m_compacted(false) {
					
				}
			
				const std::vector<ElementT> & vector () const { return m_vector; }
				std::vector<ElementT> & vector () { return m_vector; }
			
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
				
				void upload (const GraphicsBuffer::Mode &mode = GraphicsBuffer::StaticDraw) {
					ensure(!m_compacted);
					
					m_buffer = new GraphicsBuffer(TARGET);
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
						return (void*)(offset * sizeof(ElementT));
					} else {
						ensure(!m_compacted);
						
						return &(m_vector[offset]);
					}
				}
				
			};
			
			template <unsigned E, typename t = RealT>
			class VectorArray : public BufferedArray<E, t, GraphicsBuffer::Array> {
			public:
				void vertexPointer () const {
					::glVertexPointer(E, GLTypeTraits<t>::TYPE, 0, this->base());
				}
		
				void texCoordPointer () const {
					::glTexCoordPointer(E, GLTypeTraits<t>::TYPE, 0, this->base());
				}
		
				void colorPointer () const {
					::glColorPointer(E, GLTypeTraits<t>::TYPE, 0, this->base());
				}
				
				void normalPointer () const {
					::glNormalPointer(GLTypeTraits<t>::TYPE, 0, this->base());
				}
			};

			template <typename _ElementIndexT = GLuint>
			struct IndexArray : public BufferedArray<1, _ElementIndexT, GraphicsBuffer::ElementArray> {
			public:
				typedef _ElementIndexT ElementIndexT;	

				void drawElements(GLenum mode) const {
					::glDrawElements(mode, this->m_vector.size(), GLTypeTraits<ElementIndexT>::TYPE, this->base());
				}
			};
		}
	}
}

#endif
