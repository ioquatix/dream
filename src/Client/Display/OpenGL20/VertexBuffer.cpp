/*
 *  Client/Display/OpenGL20/VertexBuffer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 19/10/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "VertexBuffer.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			IMPLEMENT_CLASS(VertexBuffer)
			
			VertexBuffer::VertexBuffer(const Target &t) {
				m_target = t;
				glGenBuffersARB(1, &m_id);
			}
			
			VertexBuffer::~VertexBuffer () {
				glDeleteBuffersARB(1, &m_id);
			}
			
			void VertexBuffer::bind () const {
				glBindBufferARB(m_target, m_id);
			}
			
			void VertexBuffer::unbind () const {
				glBindBufferARB(m_target, 0);
			}
			
			void VertexBuffer::setData (IndexT length, const ByteT *data, const Mode &usage) {
				glBufferDataARB(m_target, length, data, usage);
			}
			
			void VertexBuffer::setData(IndexT offset, IndexT length, const ByteT *data) {
				glBufferSubDataARB(m_target, offset, length, data);
			}
			
			ByteT* VertexBuffer::mapBuffer (const Access &access) {
				return static_cast<ByteT*> (glMapBufferARB(m_target, access));
			}
			
			void VertexBuffer::unmapBuffer () {
				glUnmapBuffer(m_target);
			}
			
		}
	}
}
