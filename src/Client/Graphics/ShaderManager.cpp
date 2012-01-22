//
//  ShaderManager.cpp
//  Dream
//
//  Created by Samuel Williams on 5/12/11.
//  Copyright (c) 2011 Orion Transfer Ltd. All rights reserved.
//

#include "ShaderManager.h"

#include <exception>

namespace Dream {
	namespace Client {
		namespace Graphics {

			ShaderError::ShaderError(StringT message)
			: m_message(message)
			{
				
			}
			
			const char * ShaderError::what () const noexcept
			{
				return m_message.c_str();
			}
			
#pragma mark -
			
			Program::Program()
			{
				m_handle = glCreateProgram();
			}
			
			Program::~Program()
			{
				glDeleteProgram(m_handle);
			}
			
			void Program::attach(GLenum shader)
			{
				glAttachShader(m_handle, shader);
			}
			
			bool Program::link()
			{
				glLinkProgram(m_handle);
				
				GLint status;
				property(GL_LINK_STATUS, &status);
				glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
				
				if (status == 0) {
					// Something went wrong...
					
					Shared<Buffer> log = info_log();
					std::cerr << "Error linking program:" << std::endl;
					std::cerr << StringT(log->begin(), log->end()) << std::endl;
				}
				
				return status != 0;
			}
			
			GLuint Program::attributeLocation(const char * name)
			{
				return glGetAttribLocation(m_handle, name);
			}
			
			GLuint Program::uniformLocation(const char * name)
			{
				return glGetUniformLocation(m_handle, name);
			}
			
			void Program::bind_fragment_location(const char * name, GLuint output)
			{
				glBindFragDataLocation(m_handle, output, name);
			}
			
			Shared<Buffer> Program::info_log() {
				GLint length;
				property(GL_INFO_LOG_LENGTH, &length);
				
				if (length > 0) {
					Shared<MutableBuffer> buffer = PackedBuffer::newBuffer(length);
					
					glGetProgramInfoLog(m_handle, length, NULL, (GLchar *)buffer->begin());
					
					return buffer;
				} else {
					return NULL;
				}
			}
			
			void Program::enable()
			{
				glUseProgram(m_handle);
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
				for (auto i : m_shaders) {
					glDeleteShader(i);
				}
			}
			
			GLenum ShaderManager::compile(GLenum type, const Buffer * buffer)
			{
				GLuint shader = glCreateShader(type);
				
				const GLchar * source = (GLchar*)buffer->begin();
				GLint length = (GLint)buffer->size();
				glShaderSource(shader, 1, &source, &length);
				checkError();
				
				glCompileShader(shader);
				checkError();
				
				GLint logLength;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
				if (logLength > 0) {
					Shared<MutableBuffer> buffer = PackedBuffer::newBuffer(logLength);
					
					glGetShaderInfoLog(shader, (GLsizei)buffer->size(), (GLsizei*)&logLength, (GLchar*)buffer->begin());
					
					std::cerr << "Error compiling shader:" << std::endl;
					std::cerr << buffer->begin() << std::endl;
				}
				
				GLint status;
				glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
				if (status == 0) {
					std::cerr << "Shader didn't compile!" << std::endl;
					
					glDeleteShader(shader);
					
					return 0;
				}
				
				checkError();
				
				return shader;

			}
		}
	}
}
