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
#include <Dream/Client/Graphics/PixelBufferRenderer.h>

#include <Dream/Numerics/Quaternion.h>

namespace Demo {
	using namespace Dream;
	using namespace Dream::Events;
	using namespace Dream::Resources;
	using namespace Dream::Renderer;
	using namespace Dream::Client::Display;
	using namespace Dream::Client::Graphics;
	
	struct FlowingLight {
		Vec3 position;
		Array<Vec3> history;
		Quat rotations[3];
		
		void update() {
			rotations[1].set_to_angle_axis_rotation(rotations[1].rotation_angle(), rotations[2] * rotations[1].rotation_axis());
			rotations[0].set_to_angle_axis_rotation(rotations[0].rotation_angle(), rotations[1] * rotations[0].rotation_axis());
			position = rotations[0] * position;
			
			history.push_back(position);
			for (std::size_t i = 1; i < history.size(); i++) {
				history[i-1] = history[i];
			}
			
			history.resize(std::min<std::size_t>(history.size(), 40));
		}
	};
	
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
		
		Ref<Program> _textured_program, _flat_program, _solid_program;
		GLuint _position_uniform, _light_positions_uniform, _color_attribute, _position_attribute, _normal_attribute, _mapping_attribute;
		
		GLuint _diffuse_uniform;
		Ref<Texture> _crate_texture;
		
		Vec2 _point;
		Vec4 _light_positions[2];
		Vec4 _light_colors[2];
		FlowingLight _flowing_lights[2];
		
		Shared<VertexArray> _flowing_array;
		Shared<VertexBuffer> _flowing_buffer;
		
		typedef Geometry::Mesh<> MeshT;
		Ref<MeshBuffer<MeshT>> _object_mesh_buffer, _grid_mesh_buffer;
		
		Ref<ShaderManager> _shader_manager;
		Ref<TextureManager> _texture_manager;
		
		Ref<IPixelBuffer> _image;
		Ref<PixelBufferRenderer> _pixel_buffer_renderer;
		
		RealT _rotation;

	public:
		virtual ~DemoScene ();
		
		virtual void will_become_current (ISceneManager *);
		virtual void will_revoke_current (ISceneManager *);
		
		virtual bool event (const EventInput & input);
		virtual bool button (const ButtonInput & input);
		virtual bool motion (const MotionInput & input);
		virtual bool resize (const ResizeInput & input);
				
		GLuint compile_shader_of_type (GLenum type, StringT path);
		Ref<Program> load_program(StringT name);
		
		virtual void render_frame_for_time (TimeT time);
	};
	
	DemoScene::~DemoScene ()
	{
	}
	
	GLuint DemoScene::compile_shader_of_type (GLenum type, StringT name)
	{
		Ref<IData> data = resource_loader()->data_for_resource(name);
		
		if (data) {
			logger()->log(LOG_DEBUG, LogBuffer() << "Loading " << name);
			return _shader_manager->compile(type, data->buffer().get());
		} else {
			return 0;
		}
	}
	
	Ref<Program> DemoScene::load_program(StringT name) {
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
	
	void DemoScene::will_become_current(ISceneManager * manager)
	{
		Scene::will_become_current(manager);
		
		glEnable(GL_DEPTH_TEST);
		
		check_graphics_error();
		
		_camera = new BirdsEyeCamera;
		_camera->set_distance(10);
		_camera->set_multiplier(Vec3(0.1, 0.1, 0.01));
		
		_projection = new PerspectiveProjection(R90 * 0.7, 1, 1024 * 12);
		_viewport = new Viewport(_camera, _projection);
		_viewport->set_bounds(AlignedBox<2>(ZERO, manager->display_context()->size()));
		
		_point.zero();
		_light_positions[0] = vec(-10.0, 0.0, 1.0, 1.0);
		_light_positions[1] = vec(10.0, 0.0, 1.0, 1.0);
		
		_rotation = 0.0;
		_light_colors[0] = Vec4(2.0, 0.8, 0.4, 0.0);
		_light_colors[1] = Vec4(0.4, 0.8, 2.0, 0.0);
		
		_shader_manager = new ShaderManager;
		_texture_manager = new TextureManager;
		
		TextureParameters parameters;
		parameters.generate_mip_maps = true;
		parameters.min_filter = GL_LINEAR_MIPMAP_LINEAR;
		parameters.mag_filter = GL_LINEAR;
		
		parameters.target = GL_TEXTURE_2D;
		
		_image = resource_loader()->load<IPixelBuffer>("Materials/Checkers");
		Ref<IPixelBuffer> texture_image = resource_loader()->load<IPixelBuffer>("Materials/Checkers");
		
		_crate_texture = _texture_manager->allocate(parameters, texture_image);
		_texture_manager->bind(0, _crate_texture);
		
		check_graphics_error();
		
		{
			_pixel_buffer_renderer = new PixelBufferRenderer(_texture_manager);
		}
		
		{
			_solid_program = load_program("Shaders/solid");
		}
		
		{
			// Setup the buffer for drawing the light trails.
			_flowing_array = new VertexArray;
			_flowing_buffer = new VertexBuffer;
			
			_flowing_array->bind();
			_flowing_buffer->attach(*_flowing_array);
			
			VertexArray::Attributes attributes(*_flowing_array, *_flowing_buffer);
			attributes[0] = &BasicVertex<Vec3>::element;
			
			check_graphics_error();
			
			_flowing_array->unbind();
		}
		
		{
			_flat_program = load_program("Shaders/flat");
			
			_flat_program->set_attribute_location("position", 0);
			_flat_program->set_attribute_location("mapping", 1);
			_flat_program->link();
			
			_flat_program->enable();
			_flat_program->set_texture_unit("diffuse_texture", 0);
			_flat_program->disable();
			
			check_graphics_error();
		}
		
		{
			_textured_program = load_program("Shaders/surface");
			
			_textured_program->set_attribute_location("position", POSITION);
			_textured_program->set_attribute_location("normal", NORMAL);
			_textured_program->set_attribute_location("color", COLOR);
			_textured_program->set_attribute_location("mapping", MAPPING);
			_textured_program->link();
			
			_position_uniform = _textured_program->uniform_location("point");
			_diffuse_uniform = _textured_program->uniform_location("diffuse");

			_light_positions_uniform = _textured_program->uniform_location("light_positions");
			
			_color_attribute = _textured_program->attribute_location("color");
			_position_attribute = _textured_program->attribute_location("position");
			_normal_attribute = _textured_program->attribute_location("normal");
			_mapping_attribute = _textured_program->attribute_location("mapping");
			
			LogBuffer buffer;
			buffer << "Attribute positions" << std::endl;
			buffer << "Position @ " << _position_attribute << std::endl;
			buffer << "Normal @ " << _normal_attribute << std::endl;
			buffer << "Color @ " << _color_attribute << std::endl;
			buffer << "Mapping @ " << _mapping_attribute << std::endl;
			logger()->log(LOG_DEBUG, buffer);
			
			check_graphics_error();
		}
		
		{
			using namespace Geometry;
			
			Shared<MeshT> mesh = new MeshT;
			
			Generate::sphere(*mesh, 4, 8*2, (16*2)+1);
			Generate::solid_color(*mesh, Vec4(0.5, 0.5, 0.5, 1.0));
			
			/*
			{
				LogBuffer buffer;
				buffer << "Mesh vertices: " << std::endl;
				for (auto i : mesh->indices) {
					MeshT::VertexT vertex = mesh->vertices[i];
					
					buffer << "Vertex " << i << ": " << vertex.position << "; " << vertex.mapping << std::endl;
				}
				logger()->log(LOG_DEBUG, buffer);
			}
			*/
			
			_object_mesh_buffer = new MeshBuffer<MeshT>();
			_object_mesh_buffer->set_mesh(mesh, GL_TRIANGLE_STRIP);
		
			{
				VertexArray::Attributes attributes(_object_mesh_buffer);
				attributes[_position_attribute] = &MeshT::VertexT::position;
				attributes[_normal_attribute] = &MeshT::VertexT::normal;
				attributes[_color_attribute] = &MeshT::VertexT::color;
				attributes[_mapping_attribute] = &MeshT::VertexT::mapping;
			}
		}
		
		{
			using namespace Geometry;
			
			Shared<MeshT> mesh = new MeshT;
			Generate::grid(*mesh, 32, 2.0);
			
			_grid_mesh_buffer = new MeshBuffer<MeshT>();
			_grid_mesh_buffer->set_mesh(mesh, GL_LINES);
			
			{
				VertexArray::Attributes attributes(_grid_mesh_buffer);
				attributes[_position_attribute] = &MeshT::VertexT::position;
				attributes[_normal_attribute] = &MeshT::VertexT::normal;
				attributes[_color_attribute] = &MeshT::VertexT::color;
				attributes[_mapping_attribute] = &MeshT::VertexT::mapping;
			}
		}
		
		{
			_flowing_lights[0].rotations[0].set_to_angle_axis_rotation(R90 / 25.0, Vec3(0.0, 0.3, 1.0).normalize());
			_flowing_lights[0].rotations[1].set_to_angle_axis_rotation(R10 / 10.0, Vec3(0.0, 1.0, 1.0).normalize());
			_flowing_lights[0].rotations[2].set_to_angle_axis_rotation(R180 / 5, Vec3(1.0, 0.0, 0.5).normalize());
			_flowing_lights[0].position = Vec3(10, 0, 0);
			
			_flowing_lights[1].rotations[0].set_to_angle_axis_rotation(R90 / 25.0, Vec3(0.0, 1.0, 0.0));
			_flowing_lights[1].rotations[1].set_to_angle_axis_rotation(R30 / 10.0, Vec3(1.0, 1.0, 0.0).normalize());
			_flowing_lights[1].rotations[2].set_to_angle_axis_rotation(R180 / 2, Vec3(1.0, 0.0, 0.7).normalize());
			_flowing_lights[1].position = Vec3(10, 0, 0);
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
		_textured_program = NULL;
	}
	
	bool DemoScene::event(const EventInput &input) {
		// Grab the cursor
		Ref<IContext> context = manager()->display_context();
		
		if (input.event() == EventInput::RESUME) {
			context->set_cursor_mode(CURSOR_GRAB);
			logger()->log(LOG_INFO, LogBuffer() << "Cursor Mode: " << context->cursor_mode());
			
			return true;
		} else {
			context->set_cursor_mode(CURSOR_NORMAL);
			logger()->log(LOG_INFO, LogBuffer() << "Cursor Mode: " << context->cursor_mode());
			
			return true;
		}
		
		return false;
	}
	
	bool DemoScene::resize (const ResizeInput & input)
	{
		logger()->log(LOG_INFO, LogBuffer() << "Resizing to " << input.new_size());
		
		_viewport->set_bounds(AlignedBox<2>(ZERO, input.new_size()));
		glViewport(0, 0, input.new_size()[WIDTH], input.new_size()[HEIGHT]);
		
		check_graphics_error();
		
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
		} else if (input.button_pressed('d')) {
			LogBuffer buffer;
			buffer << "*** Debug ***" << std::endl;
			buffer << "Projection Matrix: " << std::endl << _viewport->projection_matrix() << std::endl;
			buffer << "View Matrix: " << std::endl << _viewport->view_matrix() << std::endl;
			buffer << "Display Matrix: " << std::endl << _viewport->display_matrix() << std::endl;
			logger()->log(LOG_DEBUG, buffer);
		}
		
		logger()->log(LOG_INFO, LogBuffer() << "Key pressed: " << input.key().button());
		
		return false;
	}
	
	void DemoScene::render_frame_for_time (TimeT time)
	{
		Scene::render_frame_for_time(time);
		
		_flowing_lights[0].update();
		_light_positions[0] = _flowing_lights[0].position << 1.0;
		_flowing_lights[1].update();
		_light_positions[1] = _flowing_lights[1].position << 1.0;
		
		_rotation += 0.01;
		
		/// Dequeue any button/motion/resize events and process them now (in this thread).
		manager()->process_pending_events(this);
		
		check_graphics_error();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		{
			_textured_program->enable();
			_texture_manager->bind(0, _crate_texture);
			
			glUniform1i(_diffuse_uniform, 0);
			
			Mat44 modelview_matrix = Mat44::rotating_matrix_around_z(_rotation);
			modelview_matrix = _viewport->display_matrix() * modelview_matrix;
			
			GLuint display_matrix_uniform = _textured_program->uniform_location("display_matrix");
			
			//Vec4 updated_positions[2];
			//updated_positions[0] = _viewport->display_matrix() * _light_positions[0];
			//updated_positions[1] = _viewport->display_matrix() * _light_positions[1];
			
			glUniform4fv(_textured_program->uniform_location("light_colors"), 2, _light_colors[0].value());
			glUniform4fv(_light_positions_uniform, 2, _light_positions[0].value());
			
			check_graphics_error();
			
			//Vec2 p(0.5 * Math::sin(time), 0.5 * Math::cos(time));
			glUniform2fv(_position_uniform, 1, (const GLfloat *)_point.value());
			
			check_graphics_error();
			
			// Draw the various objects to the screen:
			//glUniformMatrix4fv(display_matrix_uniform, 1, GL_FALSE, _viewport->display_matrix().value());
			//_grid_mesh_buffer->draw();
			
			glUniformMatrix4fv(display_matrix_uniform, 1, GL_FALSE, modelview_matrix.value());
			_object_mesh_buffer->draw();
					
			check_graphics_error();
		}
		
		for (std::size_t i = 0; i < 2; i += 1) {
			_solid_program->enable();
			glUniform4f(_solid_program->uniform_location("color"), 0.5, 0.5, 0.5, 0.5);
			glUniformMatrix4fv(_solid_program->uniform_location("display_matrix"), 1, GL_FALSE, _viewport->display_matrix().value());
			glUniform4fv(_solid_program->uniform_location("light_position"), 1, _light_positions[i].value());
			glUniform4fv(_solid_program->uniform_location("light_color"), 1, _light_colors[i].value());
			
			_flowing_array->bind();
			_flowing_buffer->attach(*_flowing_array);
			_flowing_buffer->buffer_data(_flowing_lights[i].history.data_size(), (ByteT *)_flowing_lights[i].history.data(), GL_STREAM_DRAW);
			_flowing_array->draw_arrays(GL_LINE_STRIP, 0, (GLsizei)_flowing_lights[0].history.size());
			_flowing_array->unbind();
			
			check_graphics_error();
		}
		
		// Draw textured rectangle:
		if (0) {
			_flat_program->enable();
			
			GLuint display_matrix_uniform = _flat_program->uniform_location("display_matrix");
			//glUniformMatrix4fv(display_matrix_uniform, 1, GL_FALSE, _viewport->display_matrix().value());
			Mat44 display_matrix = Mat44::rotating_matrix_around_y(R180);
			glUniformMatrix4fv(display_matrix_uniform, 1, GL_FALSE, display_matrix.value());
			
			//AlignedBox2 box = AlignedBox2::from_center_and_size(ZERO, _image->size().reduce());
			AlignedBox2 box = AlignedBox2::from_center_and_size(ZERO, 1.0);
			_pixel_buffer_renderer->render(box, _image);
			
			check_graphics_error();
		}
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
	
	DemoApplicationDelegate::~DemoApplicationDelegate()
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
