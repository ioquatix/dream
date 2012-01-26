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

#include <Dream/Client/Graphics/MeshBuffer.h>

namespace Demo {
	using namespace Dream;
	using namespace Dream::Events;
	using namespace Dream::Resources;
	using namespace Dream::Renderer;
	using namespace Dream::Client::Display;
	using namespace Dream::Client::Graphics;
		
	class DemoScene : public Scene
	{
	protected:
		// These are the locations of default attributes in the shader programs used:
		enum ProgramAttributes {
			POSITION = 0,
			NORMAL = 1,
			COLOR = 2,
			MAPPING = 3
		};
		
		Ref<BirdsEyeCamera> _camera;
		Ref<IProjection> _projection;
		Ref<Viewport> _viewport;
		
		Ref<Program> _shader_program;
		GLuint _position_uniform, _light_positions_uniform, _color_attribute, _position_attribute, _normal_attribute, _mapping_attribute;
		
		GLuint _diffuse_uniform;
		IndexT _crate_texture;
		
		Vec2 _point;
		Vec4 _light_positions[2];
		
		typedef Geometry::Mesh<> MeshT;
		Ref<MeshBuffer<MeshT>> _object_mesh_buffer, _grid_mesh_buffer;
		
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
		
		glEnable(GL_DEPTH_TEST);
		
		check_error();
		
		_camera = new BirdsEyeCamera;
		_camera->set_distance(10);
		_camera->set_multiplier(Vec3(0.1, 0.1, 0.01));
		
		_projection = new PerspectiveProjection(R90 * 0.7, 1, 1024 * 12);
		_viewport = new Viewport(_camera, _projection);
		_viewport->set_bounds(AlignedBox<2>(ZERO, manager->display_context()->size()));
		
		_point.zero();
		_light_positions[0] = vec(1.0, 1.0, 3.0, 1.0);
		_light_positions[1] = vec(-1.0, -1.0, 3.0, 1.0);
		
		_shader_manager = new ShaderManager;
		_texture_manager = new TextureManager;
		
		TextureParameters parameters;
		parameters.generate_mip_maps = true;
		parameters.min_filter = GL_LINEAR_MIPMAP_LINEAR;
		parameters.mag_filter = GL_LINEAR;
		
		parameters.target = GL_TEXTURE_2D;
		
		//Ref<IPixelBuffer> texture_image = resource_loader()->load<IPixelBuffer>("Materials/9452-diffuse");
		Ref<IPixelBuffer> texture_image = resource_loader()->load<IPixelBuffer>("Materials/DeathBall");
		_crate_texture = _texture_manager->create(parameters, texture_image);
		_texture_manager->bind(_crate_texture, 0);
		
		check_error();
		
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
			
			_shader_program->bind_fragment_location("fragment_color");
			
			_position_uniform = _shader_program->uniform_location("point");
			_diffuse_uniform = _shader_program->uniform_location("diffuse");

			_light_positions_uniform = _shader_program->uniform_location("light_positions");
			
			_shader_program->set_attribute_location("position", POSITION);
			_shader_program->set_attribute_location("normal", NORMAL);
			_shader_program->set_attribute_location("color", COLOR);
			_shader_program->set_attribute_location("mapping", MAPPING);
			
			_color_attribute = _shader_program->attribute_location("color");
			_position_attribute = _shader_program->attribute_location("position");
			_normal_attribute = _shader_program->attribute_location("normal");
			_mapping_attribute = _shader_program->attribute_location("mapping");
			
			check_error();
		}
		
		{
			using namespace Geometry;
			
			Shared<MeshT> mesh = new MeshT;
			
			//Generate::cube(_mesh, Geometry::AlignedBox<3>::from_center_and_size(ZERO, 1.0));
			//Generate::shade_square_cube(_mesh);
			Generate::sphere(*mesh, 0.5, 12, 12);
			Generate::solid_color(*mesh, Vec4(0.5, 0.5, 0.5, 1.0));
			
			_object_mesh_buffer = new MeshBuffer<MeshT>();
			_object_mesh_buffer->set_mesh(mesh);
		
			{
				auto associations = _object_mesh_buffer->associations();
				associations[_position_attribute] = &MeshT::VertexT::position;
				associations[_normal_attribute] = &MeshT::VertexT::normal;
				associations[_color_attribute] = &MeshT::VertexT::color;
				associations[_mapping_attribute] = &MeshT::VertexT::mapping;
			}
		}
		
		{
			using namespace Geometry;
			
			Shared<MeshT> mesh = new MeshT;
			Generate::grid(*mesh, 32, 2.0);
			
			_grid_mesh_buffer = new MeshBuffer<MeshT>();
			_grid_mesh_buffer->set_mesh(mesh, GL_LINES);
			
			{
				auto associations = _grid_mesh_buffer->associations();
				associations[_position_attribute] = &MeshT::VertexT::position;
				associations[_normal_attribute] = &MeshT::VertexT::normal;
				associations[_color_attribute] = &MeshT::VertexT::color;
				associations[_mapping_attribute] = &MeshT::VertexT::mapping;
			}
		}
		
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
		
		check_error();
		
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
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_shader_program->enable();
		
		glUniform1i(_diffuse_uniform, 0);
		
		GLuint display_matrix_uniform = _shader_program->uniform_location("display_matrix");
		glUniformMatrix4fv(display_matrix_uniform, 1, GL_FALSE, _viewport->display_matrix().value());
		
		glUniform4fv(_light_positions_uniform, 2, _light_positions[0].value());
		
		check_error();
		
		//Vec2 p(0.5 * Math::sin(time), 0.5 * Math::cos(time));
		glUniform2fv(_position_uniform, 1, (const GLfloat *)_point.value());
		
		check_error();
		
		// *** Draw the various objects to the screen ***
		_grid_mesh_buffer->draw();
		_object_mesh_buffer->draw();
				
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

		_context->stop();
	}
	
	void DemoApplicationDelegate::application_did_enter_foreground (IApplication * application)
	{
		logger()->log(LOG_INFO, "Entering foreground...");
		
		_context->start();
		
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
