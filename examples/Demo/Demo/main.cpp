//
//  main.cpp
//  Demo
//
//  Created by Samuel Williams on 16/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#include <Dream/Client/Client.h>
#include <Dream/Client/Display/Application.h>

#include <Dream/Client/Graphics/ShaderManager.h>
#include <Dream/Client/Graphics/TextureManager.h>

#include <Dream/Renderer/Viewport.h>
#include <Dream/Renderer/BirdsEyeCamera.h>
#include <Dream/Renderer/PointCamera.h>
#include <Dream/Renderer/Projection.h>

#include <Dream/Events/Logger.h>

#include <Dream/Geometry/Mesh.h>

namespace Demo {
	using namespace Dream;
	using namespace Dream::Events;
	using namespace Dream::Resources;
	using namespace Dream::Renderer;
	using namespace Dream::Client::Display;
	using namespace Dream::Client::Graphics;
	
	typedef Geometry::Mesh<> MeshT;
	
	class DemoScene : public Scene
	{
	protected:
		Ref<BirdsEyeCamera> _camera;
		Ref<IProjection> _projection;
		Ref<Viewport> _viewport;
		
		Ref<Program> _shader_program;
		GLuint _position_uniform, _color_attribute, _position_attribute;
			
		GLuint _vertex_array;
		Vec2 _point;
		
		Ref<ShaderManager> _shader_manager;
		Ref<TextureManager> _texture_manager;

	public:
		virtual ~DemoScene ();
		
		virtual void will_become_current (ISceneManager *);
		virtual void will_revoke_current (ISceneManager *);
		
		virtual bool button (const ButtonInput & input);
		virtual bool motion (const MotionInput & input);
		virtual bool resize (const ResizeInput & input);
				
		GLuint compile_shader_of_type (GLenum type, StringT path);
		
		virtual void render_frame_for_time (TimeT time);
	};
	
	DemoScene::~DemoScene ()
	{
	}
	
	GLuint DemoScene::compile_shader_of_type (GLenum type, StringT name)
	{
		Ref<IData> data = resource_loader()->data_for_resource(name);
		
		return _shader_manager->compile(type, data->buffer().get());
	}
	
	void DemoScene::will_become_current(ISceneManager * manager)
	{
		Scene::will_become_current(manager);
		
		_camera = new BirdsEyeCamera;
		_camera->set_distance(5);
		_projection = new PerspectiveProjection(R90 * 0.7, 1, 1024 * 12);
		_viewport = new Viewport(_camera, _projection);
		_viewport->set_bounds(AlignedBox<2>(ZERO, manager->display_context()->size()));
		
		_point.zero();
		
		_shader_manager = new ShaderManager;
		_texture_manager = new TextureManager;
		
		GLuint vertex_shader = compile_shader_of_type(GL_VERTEX_SHADER, "shader.vsh");
		GLuint fragment_shader = compile_shader_of_type(GL_FRAGMENT_SHADER, "shader.fsh");

		check_error();

		ensure(vertex_shader != 0);
		ensure(fragment_shader != 0);
		
		if (vertex_shader && fragment_shader) {
			_shader_program = new Program;
			_shader_program->attach(vertex_shader);
			_shader_program->attach(fragment_shader);
			check_error();
			
			_shader_program->link();
			
			_shader_program->bind_fragment_location("fragment_colour");
			
			_position_uniform = _shader_program->uniform_location("p");
			
			_color_attribute = _shader_program->attribute_location("colour");
			_position_attribute = _shader_program->attribute_location("position");
			check_error();
		}
		
		MeshT box;
		
		{
			using namespace Geometry;
			
			Generate::cube(box, Geometry::AlignedBox<3>::from_center_and_size(ZERO, 0.5));
			Generate::shade_square_cube(box);
		}
		
		glGenVertexArrays(1, &_vertex_array);
		glBindVertexArray(_vertex_array);
		
		GLuint vertex_buffer, index_buffer;
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, box.indices.length(), box.indices.data(), GL_STATIC_DRAW);
		
		check_error();
		
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, box.vertices.length(), box.vertices.data(), GL_STATIC_DRAW);
		
		check_error();
		
		glEnableVertexAttribArray(_position_attribute);
		glEnableVertexAttribArray(_color_attribute);
		
		check_error();
		
		glVertexAttribPointer(_position_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::position));
		glVertexAttribPointer(_color_attribute, 4, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::normal));
		
		check_error();
		
		glClearColor(0.0, 0.0, 0.0, 1.0);
		
		logger()->log(LOG_INFO, "Will become current.");
	}
	
	void DemoScene::will_revoke_current (ISceneManager * manager)
	{
		Scene::will_revoke_current(manager);
		
		logger()->log(LOG_INFO, "Will revoke current.");
		
		_shader_manager = NULL;
		_texture_manager = NULL;
		_shader_program = NULL;
	}
	
	bool DemoScene::resize (const ResizeInput & input)
	{
		logger()->log(LOG_INFO, LogBuffer() << "Resizing to " << input.new_size());
		
		_viewport->set_bounds(AlignedBox<2>(ZERO, input.new_size()));
		glViewport(0, 0, input.new_size()[WIDTH], input.new_size()[HEIGHT]);
		
		return Scene::resize(input);
	}
	
	bool DemoScene::motion (const MotionInput & input)
	{
		AlignedBox<2> bounds = input.bounds();
		
		if (this->manager()->display_context()->cursor_mode() == CURSOR_NORMAL) {
			// This assumes the mouse position is changing:
			Vec2 relative_position = input.current_position().reduce() - bounds.origin();
		
			_point = (relative_position / bounds.size()) * 2 - 1;
		} else {
			// We are receiving relative changes to cursor position:
			_point += (input.motion().reduce() / 500.0);
		}
		
		_camera->process(input);
		
		//logger()->log(LOG_INFO, LogBuffer() << "Location: " << input.current_position() << " Motion: " << input.motion() << " Point: " << _point);
		
		return true;
	}
	
	bool DemoScene::button (const ButtonInput & input)
	{
		// Control the cursor mode with key 't'.
		if (input.button_pressed('t')) {
			switch (this->manager()->display_context()->cursor_mode()) {
				case CURSOR_NORMAL:
					this->manager()->display_context()->set_cursor_mode(CURSOR_GRAB);
					break;
					
				case CURSOR_GRAB:
					this->manager()->display_context()->set_cursor_mode(CURSOR_NORMAL);
					break;
				
				default:
					break;
			}
			
			return true;
		}
		
		logger()->log(LOG_INFO, LogBuffer() << "Key pressed: " << input.key().button());
		
		return false;
	}
	
	void DemoScene::render_frame_for_time (TimeT time)
	{
		Scene::render_frame_for_time(time);
		
		/// Dequeue any button/motion/resize events and process them now (in this thread).
		manager()->process_pending_events(this);
		
		check_error();
		
		glClear(GL_COLOR_BUFFER_BIT);
		_shader_program->enable();
		
		GLuint display_matrix_uniform = _shader_program->uniform_location("display_matrix");
		glUniformMatrix4fv(display_matrix_uniform, 1, GL_FALSE, _viewport->display_matrix().value());
		
		check_error();
		
		//Vec2 p(0.5 * Math::sin(time), 0.5 * Math::cos(time));
		glUniform2fv(_position_uniform, 1, (const GLfloat *)_point.value());
		
		check_error();
		
		glDrawElements(GL_TRIANGLE_STRIP, 28, GLTypeTraits<MeshT::IndexT>::TYPE, 0);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 28);
		
		check_error();
	}
	
	class DemoApplicationDelegate : public Object, implements IApplicationDelegate
	{
		protected:
			Ref<Context> _context;
		
			virtual ~DemoApplicationDelegate ();
			virtual void application_did_finish_launching (IApplication * application);
			
			virtual void application_will_enter_background (IApplication * application);
			virtual void application_did_enter_foreground (IApplication * application);
	};
	
	DemoApplicationDelegate::~DemoApplicationDelegate ()
	{
	}
	
	void DemoApplicationDelegate::application_did_finish_launching (IApplication * application)
	{		
		Ref<Dictionary> config = new Dictionary;
		_context = application->create_context(config);
		
		Ref<Thread> thread = new Events::Thread;
		Ref<ILoader> loader = SceneManager::default_resource_loader();
		
		Ref<SceneManager> sceneManager = new SceneManager(_context, thread->loop(), loader);
		
		sceneManager->push_scene(new DemoScene);
		
		_context->start();
	}
	
	void DemoApplicationDelegate::application_will_enter_background (IApplication * application)
	{
		logger()->log(LOG_INFO, "Entering background...");

		//m_context->stop();
	}
	
	void DemoApplicationDelegate::application_did_enter_foreground (IApplication * application)
	{
		logger()->log(LOG_INFO, "Entering foreground...");
		
		//m_context->start();
		
		// Grab the cursor
		_context->set_cursor_mode(CURSOR_GRAB);
		logger()->log(LOG_INFO, LogBuffer() << "Cursor Mode: " << _context->cursor_mode());
	}
}

int main (int argc, const char * argv[])
{
	using namespace Demo;
	using namespace Dream::Client::Display;
	
	Ref<DemoApplicationDelegate> delegate = new DemoApplicationDelegate;
	IApplication::start(delegate);
	
    return 0;
}
