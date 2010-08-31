/*
 *  FrameBuffer.h
 *  Dream
 *
 *  Created by Samuel Williams on 1/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_FRAMEBUFFER_H
#define _DREAM_CLIENT_GRAPHICS_FRAMEBUFFER_H

#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
		
#ifdef DREAM_USE_OPENGLES11 || DREAM_USE_OPENGLES20
			enum {
				FRAMEBUFFER = GL_FRAMEBUFFER_OES,
				RENDERBUFFER = GL_RENDERBUFFER_OES
			};
#else
			enum {
				FRAMEBUFFER = GL_FRAMEBUFFER_EXT,
				RENDERBUFFER = GL_RENDERBUFFER_EXT
			};
#endif
		
			class RenderBuffer;
			
			class FrameBuffer : public ObjectHandle {
				EXPOSE_CLASS(FrameBuffer)
				
				class Class : public ObjectHandle::Class {
					EXPOSE_CLASSTYPE
				};
			protected:
				inline GLenum target () const {
					return FRAMEBUFFER;
				}
				
			public:		
				FrameBuffer ();
				
				void setTexture (REF(Texture) tex, GLenum attachment, GLint level = 0);
				
				GLenum checkStatus () const;
				static void useMainFrameBuffer ();
				
				void addRenderBuffer(REF(RenderBuffer) renderBuffer, GLenum attachment);
				
				void beginRender ();
				void endRender ();
				
				virtual ~FrameBuffer ();
			};
			
			class RenderBuffer : public ObjectHandle {
				EXPOSE_CLASS(RenderBuffer)
				
				class Class : public ObjectHandle::Class {
					EXPOSE_CLASSTYPE
				};
			protected:
				friend class FrameBuffer;
				
				inline GLenum target () const {
					return RENDERBUFFER;
				}
			
			public:
				RenderBuffer ();
				virtual ~RenderBuffer ();
				
				inline void setupStorage (GLenum internalFormat, const Vector<2, unsigned> & size) { setupStorage(internalFormat, size[0], size[1]); }
				void setupStorage (GLenum internalFormat, GLsizei width, GLsizei height);
			};
		}
	}
}

#endif
