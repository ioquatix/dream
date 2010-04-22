/*
 *  Client/Display/OpenGL20/Shader.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 24/04/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Shader.h"
#include "OpenGL20.h"
#include "../../../Resources/Loader.h"
#include "../../../Core/Strings.h"
#include "../../../Core/Buffer.h"
#include <exception>


namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGL20 {
			
				IMPLEMENT_INTERFACE(Shader)
				IMPLEMENT_INTERFACE(ShaderFactory)
				
#pragma mark -
				
				using namespace Dream::Resources;
				using namespace Dream::Core;
				
#pragma mark -
#pragma mark class ShaderFactory
				
				class ShaderFactory : public Object, IMPLEMENTS(ShaderFactory) {
					EXPOSE_CLASS(ShaderFactory)
					
					class Class : public Object::Class, IMPLEMENTS(ShaderFactory::Class), IMPLEMENTS(Loadable::Class) {
						EXPOSE_CLASSTYPE
						
						virtual void registerLoaderTypes (REF(ILoader) loader);
						virtual REF(Object) initFromData (const REF(Data) data, const ILoader * loader);
					};
				protected:
					const REF(Data) m_data;
					REF(const ILoader) m_loader;
					
				public:
					ShaderFactory (const REF(Data) data, const ILoader * loader);
					virtual ~ShaderFactory ();
					
					virtual REF(Shader) createShader (const std::map<String, String> & definitions);
				};
				
				IMPLEMENT_CLASS(ShaderFactory)
				
				void ShaderFactory::Class::registerLoaderTypes (REF(ILoader) loader) {
					loader->setLoaderForExtension(this, "shader");
				}
				
				REF(Object) ShaderFactory::Class::initFromData (const REF(Data) data, const ILoader * loader) {
					return ptr(new ShaderFactory(data, loader));
				}
				
				ShaderFactory::ShaderFactory (const REF(Data) data, const ILoader * loader) : m_data(data), m_loader(ptr(loader)) {
				}
				
				ShaderFactory::~ShaderFactory () {
				}
				
				REF(Shader) ShaderFactory::createShader (const std::map<String, String> & definitions) {
					StaticBuffer buf(m_data->start(), m_data->size());
					BufferStream ds(buf);
					
					REF(Shader) shader = ptr(new Shader(ds, definitions));
					
					return shader;
				}
				
#pragma mark -
				
				IMPLEMENT_CLASS(ShaderHandle)
				IMPLEMENT_CLASS(ShaderProgramHandle)
				
				IMPLEMENT_CLASS(Shader)
								
				bool ShaderProgramHandle::link () {
#ifdef GL_VERSION_2_0
					glLinkProgram(get());
#else
					glLinkProgramARB(get());
#endif
					
					GLint linked = 0;
					property(LINK_STATUS, &linked);
					
#if defined(__APPLE__)
					if (linked) {
						enable();
						
						GLint hardwareAccelerated;
						
						CGLGetParameter(CGLGetCurrentContext(), kCGLCPGPUVertexProcessing, &hardwareAccelerated);
						if (!hardwareAccelerated) {
							std::cerr << "Warning: Vertex shader is NOT being hardware-accelerated" << std::endl;
						}
						
						CGLGetParameter(CGLGetCurrentContext(), kCGLCPGPUFragmentProcessing, &hardwareAccelerated);
						if (!hardwareAccelerated) {
							std::cerr << "Warning: Fragment shader is NOT being hardware-accelerated" << std::endl;
						}
						
						disable();
					}
#endif
					
					if (!linked) return false; // Failure
					else return true;
				}
				
				Shader::Shader(std::istream &input, const std::map<String, String> & definitions) {
					initFromStream(input, definitions);
				}
				
				void Shader::initFromStream (std::istream &ifs, const std::map<String, String> & definitions) {
					using namespace std;
					
					String t, buf;
					
					ifs >> t;
					
					assert(t == "@shader");
					
					ifs >> t;
					
					assert(t == "version=120" || t == "version=1.0");
					
					stringstream vertexSource(""), fragmentSource("");
					
					int mode = HEADER_CODE;
					int line = 0;
					
					vertexSource << "#version 120" << std::endl;
					fragmentSource << "#version 120" << std::endl;
					
					for (iterateEach(definitions, d)) {
						vertexSource << "#define " << (*d).first << " " << (*d).second << std::endl;
						fragmentSource << "#define " << (*d).first << " " << (*d).second << std::endl;
					}
					
					vertexSource << "#line " << line << std::endl;
					fragmentSource << "#line " << line << std::endl;
					
					while (ifs.good()) {
						getline(ifs, buf); line += 1;
						
						buf = trimmed(buf, "\r\n");
						
						if (buf == "@vertex") {
							mode = VERTEX_CODE;
							vertexSource << "#line " << line << std::endl;
						} else if (buf == "@fragment") {
							mode = FRAGMENT_CODE;
							fragmentSource << "#line " << line << std::endl;
						} else {
							switch (mode) {
								case VERTEX_CODE:
									vertexSource << buf << std::endl;
									break;
								case FRAGMENT_CODE:
									fragmentSource << buf << std::endl;
									break;
								case HEADER_CODE:
								default:
									vertexSource << buf << std::endl;
									fragmentSource << buf << std::endl;
							}
						}
					}
					
					initFromSource(vertexSource.str(), fragmentSource.str());
				}
				
				void Shader::initFromSource (String vertexSource, String fragmentSource) {		
					m_vertexShader = ptr(new ShaderHandle(VERTEX_SHADER));
					m_fragmentShader = ptr(new ShaderHandle(FRAGMENT_SHADER));
					
					if (!m_vertexShader->compile(vertexSource))
						std::cerr << m_vertexShader->infoLog() << std::endl;
					
					if (!m_fragmentShader->compile(fragmentSource))
						std::cerr << m_fragmentShader->infoLog() << std::endl;
					
					m_shaderProgram = ptr(new ShaderProgramHandle);
					
					m_shaderProgram->attach(m_vertexShader);
					m_shaderProgram->attach(m_fragmentShader);
					
					if (!m_shaderProgram->link()) {
						std::cerr << m_shaderProgram->infoLog() << std::endl;
						throw std::runtime_error("Could not process shader!");
					}
				}
				
				Shader::~Shader () {
					
				}
				
			}
		}
	}
}
