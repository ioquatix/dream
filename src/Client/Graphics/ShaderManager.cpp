//
//  Client/Graphics/ShaderManager.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 5/12/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "ShaderManager.h"

#include <exception>

namespace Dream {
	namespace Client {
		namespace Graphics {

			ShaderError::ShaderError(StringT message) : _message(message)
			{
				
			}
			
			const char * ShaderError::what () const noexcept
			{
				return _message.c_str();
			}
			
#pragma mark -
			
			Program::Program()
			{
				_handle = glCreateProgram();
			}
			
			Program::~Program()
			{
				glDeleteProgram(_handle);
			}
			
			void Program::attach(GLenum shader)
			{
				glAttachShader(_handle, shader);
			}
			
			bool Program::link()
			{
				glLinkProgram(_handle);
				
				GLint status;
				property(GL_LINK_STATUS, &status);
				glGetProgramiv(_handle, GL_LINK_STATUS, &status);
				
				if (status == 0) {
					// Something went wrong...
					
					Shared<Buffer> log = info_log();
					
					LogBuffer buffer;
					buffer << "Error linking program:" << std::endl;
					buffer << StringT(log->begin(), log->end()) << std::endl;
					logger()->log(LOG_ERROR, buffer);
				}
				
				return status != 0;
			}
			
			GLint Program::attribute_location(const char * name)
			{
				return glGetAttribLocation(_handle, name);
			}
			
			GLint Program::uniform_location(const char * name)
			{
				return glGetUniformLocation(_handle, name);
			}
			
			GLint Program::uniform_block_index(const char * name) {
				return glGetUniformBlockIndex(_handle, name);
			}
			
			void Program::bind_fragment_location(const char * name, GLuint output)
			{
#ifdef DREAM_OPENGL32
				glBindFragDataLocation(_handle, output, name);
#endif
			}
			
			Shared<Buffer> Program::info_log() {
				GLint length;
				property(GL_INFO_LOG_LENGTH, &length);
				
				if (length > 0) {
					Shared<MutableBuffer> buffer = PackedBuffer::new_buffer(length);
					
					glGetProgramInfoLog(_handle, length, NULL, (GLchar *)buffer->begin());
					
					return buffer;
				} else {
					return NULL;
				}
			}
			
			void Program::enable()
			{
				glUseProgram(_handle);
			}
			
			void Program::disable()
			{
				glUseProgram(0);
			}
			
#pragma mark -
			
			ShaderManager::ShaderManager()
			{
				
			}
			
			ShaderManager::~ShaderManager()
			{
				for (auto i : _shaders) {
					glDeleteShader(i);
				}
			}
			
			GLenum ShaderManager::compile(GLenum type, const Buffer * buffer)
			{
				GLuint shader = glCreateShader(type);
				
				const GLchar * source = (GLchar*)buffer->begin();
				GLint length = (GLint)buffer->size();
				glShaderSource(shader, 1, &source, &length);
				check_graphics_error();
				
				glCompileShader(shader);
				check_graphics_error();
				
				GLint log_length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
				if (log_length > 0) {
					Shared<MutableBuffer> buffer = PackedBuffer::new_buffer(log_length);
					
					glGetShaderInfoLog(shader, (GLsizei)buffer->size(), (GLsizei*)&log_length, (GLchar*)buffer->begin());
					
					LogBuffer log_buffer;
					log_buffer << "Error compiling shader:" << std::endl;
					log_buffer << buffer->begin() << std::endl;
					logger()->log(LOG_ERROR, log_buffer);
				}
				
				GLint status;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
				if (status == 0) {
					logger()->log(LOG_ERROR, "Shader compilation failed!");
					
					glDeleteShader(shader);
					
					return 0;
				}
				
				check_graphics_error();
				
				return shader;
			}
		}
	}
}
