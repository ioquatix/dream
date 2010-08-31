/*
 *  Client/Display/OpenGL20/GraphicsBuffer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 19/10/06.
 *  Copyright 2006 Samuel Williams. All rights reserved.
 *
 */

#include "GraphicsBuffer.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			IMPLEMENT_CLASS(GraphicsBuffer)
			
			GraphicsBuffer::GraphicsBuffer(const Target &t) {
				m_target = t;
				glGenBuffers(1, &m_handle);
			}
			
			GraphicsBuffer::~GraphicsBuffer () {
				glDeleteBuffers(1, &m_handle);
			}
			
			void GraphicsBuffer::bind () const {
				glBindBuffer(m_target, m_handle);
			}
			
			void GraphicsBuffer::unbind () const {
				glBindBuffer(m_target, 0);
			}
			
			void GraphicsBuffer::setData (IndexT length, const ByteT *data, const Mode &usage) {
				glBufferData(m_target, length, data, usage);
			}
			
			void GraphicsBuffer::setData(IndexT offset, IndexT length, const ByteT *data) {
				glBufferSubData(m_target, offset, length, data);
			}
			
#ifndef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
			ByteT* GraphicsBuffer::mapBuffer (const Access &access) {
				return static_cast<ByteT*> (glMapBuffer(m_target, access));
			}
			
			void GraphicsBuffer::unmapBuffer () {
				glUnmapBuffer(m_target);
			}
#endif

		}
	}
}
