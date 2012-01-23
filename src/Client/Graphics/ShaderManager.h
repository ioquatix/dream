//
//  Client/Graphics/ShaderManager.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/12/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_CLIENT_GRAPHICS_SHADERMANAGER_H
#define _DREAM_CLIENT_GRAPHICS_SHADERMANAGER_H

#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			/*
				REF(ShaderManager) shader_manager = new ShaderManager;
				vertex_shader = shader_manager->compile(GL_VERTEX_SHADER, resource_loader()->...);
				fragment_shader = shader_manager->compile(GL_VERTEX_SHADER, resource_loader()->...);
				REF(Program) program = new Program();
				program->attach(vertex_shader);
				program->attach(fragment_shader);
				program->link();
			 */

			class ShaderError : public std::exception
			{
			protected:
				StringT _message;
			public:
				ShaderError(StringT message);
				
				const char * what () const noexcept;
			};
			
			class Program : public Object {
			protected:
				// This is actually a program handle.
				GLenum _handle;
				
			public:
				Program();
				~Program();
				
				void attach(GLenum shader);
				bool link();
				
				GLuint attribute_location(const char * name);
				GLuint uniform_location(const char * name);
				
				void bind_fragment_location(const char * name, GLuint output = 0);
				
				void property(GLenum name, GLint * value) {
					glGetProgramiv(_handle, name, value);
				}
				
				Shared<Buffer> info_log();
				
				void enable();
				void disable();
			};
			
			/*
			 Once the ShaderManager is deallocated, associated Program objects that are not linked will become invalid.
			 */
			class ShaderManager : public Object {
			protected:
				std::vector<GLenum> _shaders;
				
			public:
				ShaderManager();
				~ShaderManager();
				
				GLenum compile(GLenum type, const Buffer * buffer);
			};
			
		}
	}
}

#endif
