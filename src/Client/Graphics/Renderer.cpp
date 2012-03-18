//
//  Client/Graphics/Renderer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/01/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "Renderer.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			INode::~INode() {
				
			}
			
			void INode::accept(IRenderer * renderer) {
				renderer->render(this);
			}
			
			void INode::traverse(IRenderer * renderer) {
				// An implementation would look something like this:
				//for (auto child : this->children()) {
				//	child->accept(renderer);
				//}
			}
			
#pragma mark -
			
			IRenderer::~IRenderer() {
				
			}
				
			void IRenderer::render(INode * node) {
				node->traverse(this);
			}
			
			void IRenderer::traverse(INode * node) {
				node->accept(this);
			}
			
#pragma mark -
			
			RendererState::~RendererState() {
				
			}
			
			GLuint RendererState::compile_shader_of_type (GLenum type, StringT name) {
				Ref<IData> data = resource_loader->data_for_resource(name);
				
				if (data) {
					logger()->log(LOG_DEBUG, LogBuffer() << "Loading " << name);
					return shader_manager->compile(type, data->buffer().get());
				} else {
					return 0;
				}
			}
			
			Ref<Program> RendererState::load_program(StringT name) {
				GLuint vertex_shader = compile_shader_of_type(GL_VERTEX_SHADER, name + ".vertex-shader");
				GLuint geometry_shader = compile_shader_of_type(GL_GEOMETRY_SHADER, name + ".geometry-shader");
				GLuint fragment_shader = compile_shader_of_type(GL_FRAGMENT_SHADER, name + ".fragment-shader");
				
				Ref<Program> program = new Program;
				
				if (vertex_shader)
					program->attach(vertex_shader);
				
				if (geometry_shader)
					program->attach(geometry_shader);
				
				if (fragment_shader)
					program->attach(fragment_shader);
				
				program->link();
				program->bind_fragment_location("fragment_color");
				
				return program;
			}	
			
			Ref<Texture> RendererState::load_texture(const TextureParameters & parameters, StringT name) {
				Ref<IPixelBuffer> image = resource_loader->load<IPixelBuffer>(name);
				
				if (image) {
					return texture_manager->allocate(parameters, image);
				} else {
					return nullptr;
				}
			}
			
#pragma mark -
			
			GLuint BasicRenderer::compile_shader_of_type (GLenum type, StringT name) {
				Ref<IData> data = _resource_loader->data_for_resource(name);
				
				if (data) {
					logger()->log(LOG_DEBUG, LogBuffer() << "Loading " << name);
					return _shader_manager->compile(type, data->buffer().get());
				} else {
					return 0;
				}
			}
			
			Ref<Program> BasicRenderer::load_program(StringT name) {
				GLuint vertex_shader = compile_shader_of_type(GL_VERTEX_SHADER, name + ".vertex-shader");
				GLuint geometry_shader = compile_shader_of_type(GL_GEOMETRY_SHADER, name + ".geometry-shader");
				GLuint fragment_shader = compile_shader_of_type(GL_FRAGMENT_SHADER, name + ".fragment-shader");
				
				Ref<Program> program = new Program;
				
				if (vertex_shader)
					program->attach(vertex_shader);
				
				if (geometry_shader)
					program->attach(geometry_shader);
				
				if (fragment_shader)
					program->attach(fragment_shader);
				
				program->link();
				program->bind_fragment_location("fragment_color");
				
				return program;
			}
			
			BasicRenderer::BasicRenderer(State & state) : _resource_loader(state.resource_loader), _texture_manager(state.texture_manager), _shader_manager(state.shader_manager), _viewport(state.viewport) {
				
			}
			
			BasicRenderer::BasicRenderer(Ptr<Resources::Loader> resource_loader, Ptr<TextureManager> texture_manager, Ptr<ShaderManager> shader_manager, Ptr<Renderer::Viewport> viewport) : _resource_loader(resource_loader), _texture_manager(texture_manager), _shader_manager(shader_manager), _viewport(viewport) {
				
			}
			
			BasicRenderer::~BasicRenderer() {
				
			}
		}
	}
}
