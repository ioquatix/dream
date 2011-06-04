/*
 *  Client/Display/OpenGL20/Shader.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 24/04/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_SHADER_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_SHADER_H

#include "../../../Resources/Loadable.h"
#include "../../../Numerics/Vector.h"
#include "../../../Core/Strings.h"
#include "ObjectHandle.h"

#include <map>

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGL20 {
				
				using namespace Dream::Core;
				using namespace Display;				
				
				//class IUniformValue : implements IObject {
				//
				
				//public:
				//	virtual ~IUniformValue ();
				//};
				
				class IShader : implements IObject
				{
				public:
					//virtual void setUniform(String name, const IUniformValue &value) abstract;
					
					virtual void bindTextureUnit(unsigned texUnit, String name) abstract;
					
					virtual void enable () abstract;
					virtual void disable () abstract;
				};
				
				enum ShaderProperty {
#ifdef GL_VERSION_2_0
					FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
					VERTEX_SHADER = GL_VERTEX_SHADER,
					COMPILE_STATUS = GL_COMPILE_STATUS,
					LINK_STATUS = GL_LINK_STATUS,
					LOG_LENGTH = GL_INFO_LOG_LENGTH
#else
					FRAGMENT_SHADER = GL_FRAGMENT_SHADER_ARB,
					VERTEX_SHADER = GL_VERTEX_SHADER_ARB,
					COMPILE_STATUS = GL_OBJECT_COMPILE_STATUS_ARB,
					LINK_STATUS = GL_OBJECT_LINK_STATUS_ARB,
					LOG_LENGTH = GL_OBJECT_INFO_LOG_LENGTH_ARB
#endif
				};
				
				class ShaderHandle : public ObjectHandle {
				public:
					ShaderHandle (GLenum type) {
#ifdef GL_VERSION_2_0
						m_handle = glCreateShader(type);
#else
						m_handle = glCreateShaderObjectARB(type);
#endif
					}
					
					virtual ~ShaderHandle () {
#ifdef GL_VERSION_2_0
						glDeleteShader(handle());
#else
						glDeleteObjectARB(handle());
#endif
					}
					
					void property (ShaderProperty pname, GLint* dst) {
#ifdef GL_VERSION_2_0
						glGetShaderiv(handle(), pname, dst);
#else
						glGetObjectParameterivARB(handle(), pname, dst);
#endif
					}
					
					bool compile (const String &code) {
						GLint compiled = 0;
						
						const char *buf = code.c_str();
						
#ifdef GL_VERSION_2_0	
						glShaderSource(handle(), 1, &buf, NULL);
						glCompileShader(handle());
#else
						glShaderSourceARB(handle(), 1, &buf, NULL);
						glCompileShaderARB(handle());
#endif
						property(COMPILE_STATUS, &compiled);
						
						if (!compiled) return false; // Failure
						else return true;
					}
					
					String infoLog () {
						String buf;
						GLint len = 0, chars = 0;
						char *log;
						
						property(LOG_LENGTH, &len);
						
						if (len > 0) {
							log = (char *)malloc(len);
							
#ifdef GL_VERSION_2_0
							glGetShaderInfoLog(handle(), len, &chars, log);
#else
							glGetInfoLogARB(handle(), len, &chars, log);
#endif
							buf = log;
							free(log);
						}
						
						return buf;
					}
				};
				
				class ShaderProgramHandle : public ObjectHandle {
				public:
					ShaderProgramHandle () {
#ifdef GL_VERSION_2_0
						m_handle = glCreateProgram();
#else
						m_handle = glCreateProgramObjectARB();
#endif
					}
					
					virtual ~ShaderProgramHandle () {
#ifdef GL_VERSION_2_0
						glDeleteProgram(handle());
#else
						glDeleteObjectARB(handle());
#endif
					}
					
					void attach (REF(ObjectHandle) object) {
#ifdef GL_VERSION_2_0
						glAttachShader(handle(), object->handle());
#else
						glAttachObjectARB(handle(), object->handle());
#endif
					}
					
					void enable () const {
#ifdef GL_VERSION_2_0
						glUseProgram(handle());
#else
						glUseProgramObjectARB(handle());
#endif
					}
					
					void disable () const {
#ifdef GL_VERSION_2_0
						glUseProgram(0);
#else
						glUseProgramObjectARB(NULL);
#endif		
					}
					
					GLuint uniformLocation (const char* name) const {
#ifdef GL_VERSION_2_0
						return glGetUniformLocation(handle(), name);
#else
						return glGetUniformLocationARB(handle(), name);
#endif		
					}
					
					bool link ();
					
					void property (ShaderProperty pname, GLint* dst) {
#ifdef GL_VERSION_2_0
						glGetProgramiv(handle(), pname, dst);
#else
						glGetObjectParameterivARB(handle(), pname, dst);
#endif
					}
					
					String infoLog () {
						String buf;
						GLint len = 0, chars = 0;
						char *log;
						
						property(LOG_LENGTH, &len);
						
						if (len > 0) {
							log = (char *)malloc(len);
							
#ifdef GL_VERSION_2_0
							glGetProgramInfoLog(handle(), len, &chars, log);
#else
							glGetInfoLogARB(handle(), len, &chars, log);
#endif
							buf = log;
							free(log);
						}
						
						return buf;
					}
				};
				
				class Shader : public Object, implements IShader {
				protected:
					enum {
						HEADER_CODE = 0,
						VERTEX_CODE = 1,
						FRAGMENT_CODE = 2,
					};
					
					REF(ShaderHandle) m_vertexShader, m_fragmentShader;
					REF(ShaderProgramHandle) m_shaderProgram;
					
					void initFromSource(String vertexSource, String fragmentSource);
					void initFromStream(std::istream &input, const std::map<String, String> & definitions); 
				public:		
					Shader(std::istream &input, const std::map<String, String> & definitions);
					virtual ~Shader();
					
					virtual void bindTextureUnit(unsigned texUnit, String name) {
						//enable();
						//setUniform(name.c_str(), Vector<1,GLint> (texUnit));
						//disable();
					}
					/*
					template <unsigned D, typename NumericT>
					void setUniform (const char *pname, const Vector<D, NumericT> &v) {
						GLint loc = m_shaderProgram->uniformLocation(pname);
						typedef typename GL<NumericT>::template V<D> gl;
						
						gl::uniform(loc, 1, v.value());
					}
					*/
					// Activate / Deactivate better words?
					void enable () {
						m_shaderProgram->enable();
					}
					
					void disable () {
						m_shaderProgram->disable();
					}
				};
				
				class IShaderFactory : implements IObject
				{
				public:					
					virtual REF(Shader) createShader (const std::map<String, String> & definitions) abstract;
				};
			}			
		}
	}
}

#endif
