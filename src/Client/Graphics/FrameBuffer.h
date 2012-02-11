//
//  Client/Graphics/FrameBuffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/05/08.
//  Copyright (c) 2008 Orion Transfer Ltd. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_FRAMEBUFFER_H
#define _DREAM_CLIENT_GRAPHICS_FRAMEBUFFER_H

#include "Graphics.h"
#include "TextureManager.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			/** A renderbuffer object is newly introduced for offscreen rendering. It allows to render a scene directly to a renderbuffer object, instead of rendering to a texture object. Renderbuffer is simply a data storage object containing a single image of a renderable internal format. It is used to store OpenGL logical buffers that do not have corresponding texture format, such as stencil or depth buffer.
			 */
			class RenderBuffer : private NonCopyable {
			protected:
				friend class FrameBuffer;
				
				GLuint _handle;
				GLenum _target;
				
			public:				
				RenderBuffer(GLenum target = GL_RENDERBUFFER) {
					glGenRenderbuffers(1, &_handle);
				}
				
				~RenderBuffer() {
					glDeleteRenderbuffers(1, &_handle);
				}
				
				const GLuint handle() const {
					return _handle;
				}
				
				const GLenum target() const {
					return _target;
				}
				
				void bind() {
					glBindRenderbuffer(_target, _handle);
				}
				
				void unbind() {
					glBindRenderbuffer(_target, 0);
				}
				
				void setup_storage(GLenum internal_format, Vec2u size) {
					glRenderbufferStorage(target(), internal_format, size[WIDTH], size[HEIGHT]);
				}	
			};
			
			// The final rendering destination of the OpenGL pipeline is called the framebuffer.
			class FrameBuffer : private NonCopyable {
			protected:
				GLuint _handle;
				GLenum _target;
												
			public:				
				FrameBuffer(GLenum target = GL_FRAMEBUFFER) {
					glGenFramebuffers(1, &_target);
				}
				
				~FrameBuffer() {
					glDeleteFramebuffers(1, &_target);
				}
				
				const GLuint handle() const {
					return _handle;
				}
				
				const GLenum target() const {
					return _target;
				}
				
				void bind() {
					glBindFramebuffer(_target, _handle);
				}
				
				void unbind() {
					glBindFramebuffer(_target, _handle);
				}
				
				void set_texture(Ptr<Texture> texture, GLenum attachment, GLint level = 0) {
					glFramebufferTexture2D(_target, attachment, texture->target(), texture->handle(), level);
				}
								
				GLenum check_status () const;
				
				void add(const RenderBuffer & render_buffer, GLenum attachment) {
					glFramebufferRenderbuffer(_target, attachment, render_buffer.target(), render_buffer.handle());
				}
			};
			
		}
	}
}

#endif
