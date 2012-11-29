//
//  Client/Graphics/FrameBuffer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/05/08.
//  Copyright (c) 2008 Orion Transfer Ltd. All rights reserved.
//

#include "FrameBuffer.h"
#include <exception>
#include <stdexcept>

namespace Dream {
	namespace Client {
		namespace Graphics {
			GLenum FrameBuffer::check_status () const {
				GLenum status = glCheckFramebufferStatus(_target);

				if (status != GL_FRAMEBUFFER_COMPLETE) {
					switch (status) {
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
						throw std::runtime_error("Framebuffer Error: Attachment invalid");
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
						throw std::runtime_error("Framebuffer Error: Missing attachment");
#ifdef DREAM_OPENGL32
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
						throw std::runtime_error("Framebuffer Error: Missing draw buffer");
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
						throw std::runtime_error("Framebuffer Error: Missing read buffer");
#endif
					case GL_FRAMEBUFFER_UNSUPPORTED:
						throw std::runtime_error("Framebuffer Error: Configuration Unsupported");
					default:
						throw std::runtime_error("Framebuffer Error: Unknown problem");
					}
				}

				return status;
			}
		}
	}
}
