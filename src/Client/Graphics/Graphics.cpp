//
//  Client/Graphics/Graphics.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/12/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Graphics.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			void check_graphics_error()
			{
				GLenum error = GL_NO_ERROR;
				
				while ((error = glGetError()) != GL_NO_ERROR) {
					logger()->log(LOG_ERROR, LogBuffer() << "OpenGL Error #" << error);
					
					// Abort due to error
					ensure(error == GL_NO_ERROR);
				}
			}
		}
	}
}
