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
#include <Dream/Client/Graphics/WireframeRenderer.h>

#include <Dream/Renderer/Viewport.h>
#include <Dream/Renderer/BirdsEyeCamera.h>
#include <Dream/Renderer/PointCamera.h>
#include <Dream/Renderer/Projection.h>

#include <Dream/Geometry/Generate/Sphere.h>
#include <Dream/Geometry/Generate/Planar.h>

#include <Dream/Events/Logger.h>

#include <Dream/Client/Graphics/MeshBuffer.h>
#include <Dream/Client/Graphics/PixelBufferRenderer.h>

#include <Dream/Numerics/Quaternion.h>
#include <Dream/Client/Graphics/ParticleRenderer.h>

namespace Demo {
	using namespace Dream;
	using namespace Dream::Events;
	using namespace Dream::Resources;
	using namespace Dream::Renderer;
	using namespace Dream::Client::Display;
	using namespace Dream::Client::Graphics;
	
	class TrailParticles : public ParticleRenderer<TrailParticles> {
	public:
		bool update_particle (Particle & particle, TimeT last_time, TimeT current_time, TimeT dt);
		void add(LineSegment<3> line_segment, const Vec3 & color);
	};
	
	bool TrailParticles::update_particle (Particle & particle, TimeT last_time, TimeT current_time, TimeT dt) {
		if (particle.update_time(dt, Vec3(0.0, 0.0, -9.8))) {
			RealT alpha = particle.calculate_alpha(0.7);
			particle.update_vertex_color(Vec3(0.2 * particle.color_modulation(2.0)) << alpha);
			
			return true;
		}
		
		return false;
	}
	
	void TrailParticles::add(LineSegment<3> line_segment, const Vec3 & color) {
		const RealT DENSITY = 2.0;
		
		RealT density = DENSITY;
		
		while ((density -= real_random()) > 0) {
			Vec3 point = line_segment.point_at_time(density / DENSITY);
			point[X] += real_random(-0.2, 0.2);
			point[Y] += real_random(-0.2, 0.2);
			point[Z] += real_random(-0.2, 0.2);
			
			Particle particle;
			
			particle.velocity = Vec3(0.0, 0.0, 0.0);
			particle.set_position(point, Vec3(-0.1, -0.1, 0.0), Vec3(0.0, 0.0, -1.0), 0.0);
			particle.add_life(4.0 + real_random());
			particle.color = color;
			particle.set_random_mapping(4);
			
			_particles.push_back(particle);
		}
	}
	
	struct FlowingLight {
		Vec3 position;
		std::vector<Vec3> history;
		Quat rotations[3];
		
		void update() {
			rotations[1].set_to_angle_axis_rotation(rotations[1].rotation_angle(), rotations[2] * rotations[1].rotation_axis());
			rotations[0].set_to_angle_axis_rotation(rotations[0].rotation_angle(), rotations[1] * rotations[0].rotation_axis());
			position = rotations[0] * position;
			
			history.push_back(position);
			for (std::size_t i = 1; i < history.size(); i++) {
				history[i-1] = history[i];
			}
			
			history.resize(std::min<std::size_t>(history.size(), 50));
		}
		
		LineSegment<3> end_segment() const {
			if (history.size() >= 2) {
				return LineSegment<3>(history[history.size() - 1], history[history.size() - 2]);
			} else {
				return LineSegment<3>(position, position);
			}
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
		
		Ref<Program> _textured_program, _flat_program, _solid_program, _particle_program, _wireframe_program;
		GLuint _position_uniform, _light_positions_uniform;
		
		GLuint _diffuse_uniform;
		Ref<Texture> _crate_texture, _particle_texture, _sakura_texture;
		
		Vec2 _point;
		Vec4 _light_positions[3];
		Vec4 _light_colors[3];
		FlowingLight _flowing_lights[3];
		
		Shared<VertexArray> _flowing_array;
		Shared<VertexBuffer<BasicVertex<Vec3>>> _flowing_buffer;
		
		typedef Geometry::Mesh<> MeshT;
		Ref<MeshBuffer<MeshT>> _object_mesh_buffer, _grid_mesh_buffer;
		
		Ref<ShaderManager> _shader_manager;
		Ref<TextureManager> _texture_manager;
		Ref<WireframeRenderer> _wireframe_renderer;
		
		Ref<PixelBufferRenderer> _pixel_buffer_renderer;
		
		Ref<TrailParticles> _trail_particles;
		
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
		_light_positions[0].zero();
		_light_positions[1].zero();
		_light_positions[2].zero();
		
		_rotation = 0.0;
		_light_colors[0] = Vec4(2.0, 0.8, 0.4, 0.0);
		_light_colors[1] = Vec4(0.4, 0.8, 2.0, 0.0);
		_light_colors[2] = Vec4(0.4, 2.0, 0.8, 0.0);
		//_light_colors[0] = Vec4(1.2, 0.2, 0.2, 0.0);
		//_light_colors[1] = Vec4(0.2, 0.2, 1.2, 0.0);
		//_light_colors[2] = Vec4(0.2, 1.2, 0.2, 0.0);
		
		_shader_manager = new ShaderManager;
		_texture_manager = new TextureManager;
		
		TextureParameters parameters;
		parameters.generate_mip_maps = true;
		parameters.min_filter = GL_LINEAR_MIPMAP_LINEAR;
		parameters.mag_filter = GL_LINEAR;
		parameters.target = GL_TEXTURE_2D;
		
		{
			Ref<IPixelBuffer> checkers_image = resource_loader()->load<IPixelBuffer>("Textures/Checkers");		
			_crate_texture = _texture_manager->allocate(parameters, checkers_image);
			
			Ref<IPixelBuffer> particle_image = resource_loader()->load<IPixelBuffer>("Textures/Trail");
			_particle_texture = _texture_manager->allocate(parameters, particle_image);
		}
		
		check_graphics_error();
		
		{
			_pixel_buffer_renderer = new PixelBufferRenderer(_texture_manager);
		}
		
		{
			_solid_program = load_program("Shaders/solid");
		}
		
		{
			_wireframe_program = load_program("Shaders/wireframe");
			_wireframe_program->set_attribute_location("position", WireframeRenderer::POSITION);
			_wireframe_program->link();
			
			_wireframe_renderer = new WireframeRenderer;
		}
		
		{
			_particle_program = load_program("Shaders/particle");
			
			_particle_program->set_attribute_location("position", TrailParticles::POSITION);
			_particle_program->set_attribute_location("offset", TrailParticles::OFFSET);
			_particle_program->set_attribute_location("mapping", TrailParticles::MAPPING);
			_particle_program->set_attribute_location("color", TrailParticles::COLOR);
			_particle_program->link();
			
			auto binding = _particle_program->binding();
			binding.set_texture_unit("diffuse_texture", 0);
			
			_trail_particles = new TrailParticles;
		}
		
		{
			// Setup the buffer for drawing the light trails.
			_flowing_array = new VertexArray;
			_flowing_buffer = new VertexBuffer<BasicVertex<Vec3>>;
			
			auto binding = _flowing_array->binding();
			auto attributes = binding.attach(*_flowing_buffer);
			attributes[0] = &BasicVertex<Vec3>::element;
			
			check_graphics_error();
		}
		
		{
			_flat_program = load_program("Shaders/flat");
			
			_flat_program->set_attribute_location("position", 0);
			_flat_program->set_attribute_location("mapping", 1);
			_flat_program->link();
			
			auto binding = _flat_program->binding();
			binding.set_texture_unit("diffuse_texture", 0);
			
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
			
			check_graphics_error();
		}
		
		{
			using namespace Geometry;
			
			Shared<MeshT> mesh = new MeshT;
			mesh->layout = TRIANGLE_STRIP;
			Generate::sphere(*mesh, 4, 8*2, (16*2)+1);
			//Generate::solid_color(*mesh, Vec4(0.5, 0.5, 0.5, 1.0));
			
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
			_object_mesh_buffer->set_mesh(mesh);
		
			{
				auto binding = _object_mesh_buffer->vertex_array().binding();
				
				// Attach indices
				binding.attach(_object_mesh_buffer->index_buffer());
				
				// Attach attributes
				auto attributes = binding.attach(_object_mesh_buffer->vertex_buffer());
				attributes[POSITION] = &MeshT::VertexT::position;
				attributes[NORMAL] = &MeshT::VertexT::normal;
				//attributes[COLOR] = &MeshT::VertexT::color;
				attributes[MAPPING] = &MeshT::VertexT::mapping;
			}
		}
		
		{
			using namespace Geometry;
			
			Shared<MeshT> mesh = new MeshT;
			mesh->layout = LINES;
			Generate::grid(*mesh, 32, 2.0);
			
			_grid_mesh_buffer = new MeshBuffer<MeshT>();
			_grid_mesh_buffer->set_mesh(mesh);
			
			{
				auto binding = _grid_mesh_buffer->vertex_array().binding();
				
				// Attach indices
				binding.attach(_grid_mesh_buffer->index_buffer());
				
				// Attach attributes
				auto attributes = binding.attach(_grid_mesh_buffer->vertex_buffer());
				attributes[POSITION] = &MeshT::VertexT::position;
				attributes[NORMAL] = &MeshT::VertexT::normal;
				attributes[MAPPING] = &MeshT::VertexT::mapping;
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
			_flowing_lights[1].position = Vec3(0, 10, 0);
			
			_flowing_lights[2].rotations[0].set_to_angle_axis_rotation(R90 / 25.0, Vec3(0.5, 1.0, 0.0).normalize());
			_flowing_lights[2].rotations[1].set_to_angle_axis_rotation(R30 / 10.0, Vec3(-0.7, 0.0, 0.2).normalize());
			_flowing_lights[2].rotations[2].set_to_angle_axis_rotation(R180 / 2, Vec3(0.4, -0.2, 0.3).normalize());
			_flowing_lights[2].position = Vec3(0, 0, 10);
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
	
	void DemoScene::render_frame_for_time (TimeT time) {
		Scene::render_frame_for_time(time);
		
		_trail_particles->update(time);
		
		for (std::size_t i = 0; i < 3; i += 1) {
			_flowing_lights[i].update();
			_light_positions[i] = _flowing_lights[i].position << 1.0;
			
			if (_flowing_lights[i].history.size() > 1) {
				// Add some particle effects
				_trail_particles->add(_flowing_lights[i].end_segment(), _light_colors[i].reduce());
			}
		}
		
		_rotation += 0.01;
		
		/// Dequeue any button/motion/resize events and process them now (in this thread).
		manager()->process_pending_events(this);
		
		check_graphics_error();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		{
			auto binding = _wireframe_program->binding();
			binding.set_uniform("display_matrix", _viewport->display_matrix());
		}
		
		{
			check_graphics_error();
			
			auto binding = _textured_program->binding();
			_texture_manager->bind(0, _crate_texture);
						
			Mat44 modelview_matrix = Mat44::rotating_matrix_around_z(_rotation);
			//modelview_matrix = modelview_matrix * _viewport->display_matrix();
			modelview_matrix = _viewport->display_matrix() * modelview_matrix;
			
			GLuint display_matrix_uniform = _textured_program->uniform_location("display_matrix");
			
			binding.set_uniform("light_colors", _light_colors);
			binding.set_uniform(_light_positions_uniform, _light_positions);
			
			check_graphics_error();
			
			//Vec2 p(0.5 * Math::sin(time), 0.5 * Math::cos(time));
			binding.set_uniform(_position_uniform, _point);
			
			check_graphics_error();
			
			// Draw the various objects to the screen:
			binding.set_uniform(display_matrix_uniform, _viewport->display_matrix());
			_grid_mesh_buffer->draw();
			
			binding.set_uniform(display_matrix_uniform, modelview_matrix);
			_object_mesh_buffer->draw();
			
			check_graphics_error();
			
			//AlignedBox3 sphere_box(-4, 4);
			//_wireframe_renderer->render(sphere_box);
		}

		{
			auto binding = _solid_program->binding();
			binding.set_uniform("display_matrix", _viewport->display_matrix());

			for (std::size_t i = 0; i < 3; i += 1) {
				binding.set_uniform("light_position", _light_positions[i]);
				binding.set_uniform("light_color", _light_colors[i]);
				
				auto array_binding = _flowing_array->binding();
				array_binding.attach(*_flowing_buffer);
				
				auto buffer_binding = _flowing_buffer->binding<Vec3>();
				buffer_binding.set_data(_flowing_lights[i].history);
				
				array_binding.draw_arrays(GL_LINE_STRIP, 0, (GLsizei)_flowing_lights[i].history.size());
				
				check_graphics_error();
			}
		}
		
		{
			glDepthMask(GL_FALSE);
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			auto binding = _particle_program->binding();
			binding.set_uniform("display_matrix", _viewport->display_matrix());

			_texture_manager->bind(0, _particle_texture);
			_trail_particles->draw();
			
			glDisable(GL_BLEND);
			
			glDepthMask(GL_TRUE);
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
	
	DemoApplicationDelegate::~DemoApplicationDelegate() {
	}
	
	void DemoApplicationDelegate::application_did_finish_launching (IApplication * application)
	{		
		Ref<Dictionary> config = new Dictionary;
		_context = application->create_context(config);
		
		Ref<Thread> thread = new Events::Thread;
		Ref<ILoader> loader = SceneManager::default_resource_loader();
		
		Ref<SceneManager> scene_manager = new SceneManager(_context, thread->loop(), loader);
		
#ifdef DREAM_DEBUG
		logger()->log(LOG_INFO, "Debugging mode active");
#else
		logger()->log(LOG_INFO, "Debugging mode inactive");
#endif
		
		scene_manager->push_scene(new DemoScene);
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
