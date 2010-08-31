/*
 *  FrameBuffer.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 1/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "FrameBuffer.h"
#include <exception>
#include <stdexcept>

namespace Dream {
	namespace Client {
		namespace Graphics {
	
#pragma mark -

#ifdef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
#define glGenFramebuffersEXT glGenFramebuffersOES
#define glBindFramebufferEXT glBindFramebufferOES
#define glDeleteFramebuffersEXT glDeleteFramebuffersOES
#define glFramebufferTexture2DEXT glFramebufferTexture2DOES
#define glCheckFramebufferStatusEXT glCheckFramebufferStatusOES
#define glFramebufferRenderbufferEXT glFramebufferRenderbufferOES
#define glGenRenderbuffersEXT glGenRenderbuffersOES
#define glBindRenderbufferEXT glBindRenderbufferOES
#define glDeleteRenderbuffersEXT glDeleteRenderbuffersOES
#define glRenderbufferStorageEXT glRenderbufferStorageOES

#define GL_FRAMEBUFFER_COMPLETE_EXT GL_FRAMEBUFFER_COMPLETE_OES
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_OES
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_OES
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT GL_FRAMEBUFFER_UNSUPPORTED_OES
#endif

			IMPLEMENT_CLASS(FrameBuffer)

			FrameBuffer::FrameBuffer () {
				glGenFramebuffersEXT(1, &m_handle);
				glBindFramebufferEXT(target(), m_handle);
			}
			
			FrameBuffer::~FrameBuffer () {
				glDeleteFramebuffersEXT(1, &m_handle);
			}
			
			void FrameBuffer::beginRender () {
				glBindFramebufferEXT(target(), m_handle);
			}
			
			void FrameBuffer::endRender () {
				glBindFramebufferEXT(target(), 0);
			}
			
			void FrameBuffer::setTexture (REF(Texture) tex, GLenum attachment, GLint level) {
				glFramebufferTexture2DEXT(target(), attachment, tex->target(), tex->id(), level);
			}
			
			GLenum FrameBuffer::checkStatus () const {
				GLenum status = glCheckFramebufferStatusEXT(target());
				
				if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
					switch (status) {
						case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
							throw std::runtime_error("Framebuffer Error: Attachment invalid");
						case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
							throw std::runtime_error("Framebuffer Error: Missing attachment");
						case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
							throw std::runtime_error("Framebuffer Error: Missing dimensions");
						case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
							throw std::runtime_error("Framebuffer Error: Missing formats");
#ifndef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
// These are not available in OpenGL ES
						case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
							throw std::runtime_error("Framebuffer Error: Missing draw buffer");
						case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
							throw std::runtime_error("Framebuffer Error: Missing read buffer");
#endif
						case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
							throw std::runtime_error("Framebuffer Error: Configuration Unsupported");
					}
				}
				
				return status;
			}
			
			void FrameBuffer::addRenderBuffer(REF(RenderBuffer) renderBuffer, GLenum attachment) {
				glFramebufferRenderbufferEXT(target(), attachment, renderBuffer->target(), renderBuffer->handle());
			}
			
#pragma mark -
			
			IMPLEMENT_CLASS(RenderBuffer)
				
			RenderBuffer::RenderBuffer () {
				glGenRenderbuffersEXT(1, &m_handle);
				glBindRenderbufferEXT(target(), m_handle);
			}
			
			RenderBuffer::~RenderBuffer () {
				glDeleteRenderbuffersEXT(1, &m_handle);
			}
					
			void RenderBuffer::setupStorage (GLenum internalFormat, GLsizei width, GLsizei height) {
				glRenderbufferStorageEXT(target(), internalFormat, width, height);
			}
		}
	}
}