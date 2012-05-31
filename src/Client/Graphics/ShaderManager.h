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
			void test() {
				Ref<Program> program = ...;
				Shared<UniformBuffer> lighting_buffer;
			 
				UniformBuffer::Binding uniform_binding(0);
				
				program->set_uniform_block(uniform_binding);
				buffer->bind_range(uniform_binding);
			}
			 */
			
			class UniformBuffer : private NonCopyable {
			protected:
				GLuint _handle;
				
			public:
				UniformBuffer(std::size_t size = 0, GLenum mode = GL_STREAM_DRAW) {
					glGenBuffers(1, &_handle);
					
					if (size) {
						bind();
						resize(size, mode);
						unbind();
					}
				}
				
				~UniformBuffer() {
					glDeleteBuffers(1, &_handle);
				}
				
				void bind() {
					glBindBuffer(GL_UNIFORM_BUFFER, _handle);
				}
				
				void unbind() {
					glBindBuffer(GL_UNIFORM_BUFFER, 0);
				}
				
				void resize(std::size_t size, GLenum mode = GL_STREAM_DRAW) {
					glBufferData(GL_UNIFORM_BUFFER, (GLsizeiptr)size, NULL, mode);
				}
				
				void bind_range(GLuint bindingIndex, GLintptr size, GLintptr offset = 0) {
					bind();
					glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, _handle, offset, size);
					unbind();
				}
				
				class Binding {
				protected:
					GLuint _index;
					
				public:
					Binding(GLuint index) : _index(index) {
					}
					
					GLuint index() const { return _index; }
				};
			};
			
			template<unsigned E>
			class GLUniformTraits {
			};

			template<unsigned R, unsigned C>
			class GLUniformMatrixTraits {	
			};
			
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
				
				void enable();
				void disable();
				
			public:
				Program();
				~Program();
				
				void attach(GLenum shader);
				bool link();
				
				GLint attribute_location(const char * name);
				GLint uniform_location(const char * name);				
				GLint uniform_block_index(const char * name);
				
				void bind_fragment_location(const char * name, GLuint output = 0);
				
			public:
				void set_attribute_location(const char * name, GLuint location) {
					glBindAttribLocation(_handle, location, name);
				}
				
				class Binding {
				protected:
					Program * _program;
				
					GLuint location_of(GLuint location) {
						return location;
					}
					
					GLuint location_of(const char * name) {
						return _program->uniform_location(name);
					}
					
				public:
					Binding(Program * program) : _program(program) {
						_program->enable();
					}
					
					Binding(Binding && other) : _program(other._program) {
						other._program = NULL;
					}
					
					~Binding() {
						if (_program)
							_program->disable();
					}
					
					template <typename LocationT>
					void set_texture_unit(LocationT name, GLuint unit) {
						glUniform1i(location_of(name), unit);
					}
					
					template <typename LocationT, unsigned E, typename T>
					void set_uniform(LocationT name, const Vector<E, T> & vector) {
						GLUniformTraits<E>::set(location_of(name), 1, vector.value());
					}
					
					template <typename LocationT, unsigned E, typename T, unsigned N>
					void set_uniform(LocationT name, const Vector<E, T>(& vector)[N]) {
						GLUniformTraits<E>::set(location_of(name), N, vector[0].value());
					}
					
					template <typename LocationT, unsigned R, unsigned C, typename T>
					void set_uniform(LocationT name, const Matrix<R, C, T> & matrix, bool transpose = false) {
						GLUniformMatrixTraits<R, C>::set(location_of(name), 1, transpose, matrix.value());
					}
				};
				
				Binding binding() {
					Binding binding(this);
					
					// Is this approach really efficient?
					return std::move(binding);
				}
				
				void property(GLenum name, GLint * value) {
					glGetProgramiv(_handle, name, value);
				}
				
				Shared<Buffer> info_log();
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
			
// MARK: mark -
// MARK: mark Uniform Specialisations
			
			template<>
			class GLUniformTraits<1> {
			public:
				static void set(GLint location, const GLsizei count, const GLuint * value) {
					glUniform1uiv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLint * value) {
					glUniform1iv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLfloat * value) {
					glUniform1fv(location, count, value);
				}
			};
			
			template<>
			class GLUniformTraits<2> {
			public:
				static void set(GLint location, const GLsizei count, const GLuint * value) {
					glUniform2uiv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLint * value) {
					glUniform2iv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLfloat * value) {
					glUniform2fv(location, count, value);
				}
			};
			
			template<>
			class GLUniformTraits<3> {
			public:
				static void set(GLint location, const GLsizei count, const GLuint * value) {
					glUniform3uiv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLint * value) {
					glUniform3iv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLfloat * value) {
					glUniform3fv(location, count, value);
				}
			};
			
			template<>
			class GLUniformTraits<4> {
			public:
				static void set(GLint location, const GLsizei count, const GLuint * value) {
					glUniform4uiv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLint * value) {
					glUniform4iv(location, count, value);
				}
				
				static void set(GLint location, const GLsizei count, const GLfloat * value) {
					glUniform4fv(location, count, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<2, 2> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix2fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<2, 3> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix2x3fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<2, 4> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix2x4fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<3, 2> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix3x2fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<3, 3> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix3fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<3, 4> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix3x4fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<4, 2> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix4x2fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<4, 3> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix4x3fv(location, count, transpose, value);
				}
			};
			
			template<>
			class GLUniformMatrixTraits<4, 4> {
			public:
				static void set(GLint location, const GLsizei count, GLboolean transpose, const GLfloat * value) {
					glUniformMatrix4fv(location, count, transpose, value);
				}
			};
		}
	}
}

#endif
