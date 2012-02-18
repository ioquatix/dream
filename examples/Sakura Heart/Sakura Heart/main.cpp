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

#include <Dream/Events/Logger.h>

#include <Dream/Client/Graphics/MeshBuffer.h>
#include <Dream/Client/Graphics/PixelBufferRenderer.h>

#include <Dream/Numerics/Quaternion.h>
#include <Dream/Client/Graphics/Renderer.h>
#include <Dream/Client/Graphics/ParticleRenderer.h>

namespace SakuraHeart {
	using namespace Dream;
	using namespace Dream::Events;
	using namespace Dream::Resources;
	using namespace Dream::Renderer;
	using namespace Dream::Client::Display;
	using namespace Dream::Client::Graphics;
	
	// The actual particle simulation/drawing code:
	class HeartParticles : public ParticleRenderer<HeartParticles> {
	public:
		void update_for_duration(TimeT last_time, TimeT current_time, TimeT dt);
		void add();
	};
	
	void HeartParticles::update_for_duration(TimeT last_time, TimeT current_time, TimeT dt) {
		if (_physics.size() == 0)
			return;
		
		std::size_t required_size = _physics.size() * 4 * sizeof(Vertex);
		auto binding = _vertex_buffer.binding();
		if (_vertex_buffer.size() < required_size)
			binding.resize(required_size);
		
		auto buffer = binding.array();
		
		Vec3 gravity(0.0, 0.0, -9.8);
		
		std::size_t i = 0;
		while (i < _physics.size()) {
			Physics & physics = _physics[i];
			
			if (physics.update_time(dt, gravity)) {
				RealT alpha = physics.calculate_alpha(0.7);
				
				physics.update_vertex_color(Vec3(0.2 * physics.color_modulation(2.0)) << alpha);
				
				// Add the particle to be drawn:
				physics.queue(&buffer[i*4]);
				
				i += 1;
			} else {
				erase_element_at_index(i, _physics);
			}
		}
		
		binding.unmap();
		
		_count = i;
	}
	
	void HeartParticles::add() {
		const std::size_t DENSITY = 256 * 2.0;
		
		for (std::size_t i = 0; i < DENSITY; i += 1) {
			RealT t = real_random(0, R360);
			
			Vec3 point;
			
			point[X] = 16.0 * Math::pow(Math::sin(t), 3);
			point[Y] = 0.0;
			point[Z] = 13.0 * Math::cos(t) 
				- 5.0 * Math::cos(2.0*t)
				- 2.0 * Math::cos(3.0*t)
				- Math::cos(4.0*t);
			
			point[X] += real_random(-0.5, 0.5);
			point[Y] += real_random(-0.5, 0.5);
			point[Z] += real_random(-0.5, 0.5);
			
			Physics particle;
			
			particle.set_velocity(Vec3(0.0, 0.0, 0.0));
			particle.set_position(point, Vec3(-0.1, -0.1, 0.0), Vec3(0.0, 0.0, -1.0), 0.0);
			particle.add_life(1 + real_random());
			particle.set_color(Vec3(1.0, 1.0, 1.0));
			particle.set_random_mapping(8);
			
			_physics.push_back(particle);
		}
		
		logger()->log(LOG_DEBUG, LogBuffer() << "Particles: " << _physics.size());
	}
	
	// The high level renderer that manages associated textures/shaders:
	class SakuraHeartRenderer : public BasicRenderer {
	protected:
		Ref<Program> _particle_program;
		Ref<Texture> _sakura_texture;

	public:
		SakuraHeartRenderer(Ptr<Resources::Loader> resource_loader, Ptr<TextureManager> texture_manager, Ptr<ShaderManager> shader_manager, Ptr<Viewport> viewport) : BasicRenderer(resource_loader, texture_manager, shader_manager, viewport) {
			// Load sakura texture:
			TextureParameters parameters;
			parameters.generate_mip_maps = true;
			parameters.min_filter = GL_LINEAR_MIPMAP_LINEAR;
			parameters.mag_filter = GL_LINEAR;
			parameters.target = GL_TEXTURE_2D;
			
			Ref<IPixelBuffer> sakura_image = _resource_loader->load<IPixelBuffer>("Textures/Sakura");
			_sakura_texture = _texture_manager->allocate(parameters, sakura_image);
			
			// Load particle program:
			_particle_program = load_program("Shaders/particle");
			
			_particle_program->set_attribute_location("position", HeartParticles::POSITION);
			_particle_program->set_attribute_location("offset", HeartParticles::OFFSET);
			_particle_program->set_attribute_location("mapping", HeartParticles::MAPPING);
			_particle_program->set_attribute_location("color", HeartParticles::COLOR);
			_particle_program->link();
			
			_particle_program->enable();
			_particle_program->set_texture_unit("diffuse_texture", 0);
			_particle_program->disable();
		}
		
		virtual ~SakuraHeartRenderer() {
			
		}
		
		void render(Ptr<HeartParticles> heart_particles) {
			glDepthMask(GL_FALSE);
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			_particle_program->enable();
			_particle_program->set_uniform("display_matrix", _viewport->display_matrix());
			
			_texture_manager->bind(0, _sakura_texture);
			heart_particles->draw();
			
			_particle_program->disable();
			glDisable(GL_BLEND);
			
			glDepthMask(GL_TRUE);		
		}
	};
	
	// The scene class that manages events:
	class SakuraHeartScene : public Scene
	{
	protected:
		Ref<BirdsEyeCamera> _camera;
		Ref<IProjection> _projection;
		Ref<Viewport> _viewport;
		
		Ref<SakuraHeartRenderer> _sakura_heart_renderer;
		Ref<HeartParticles> _heart_particles;

	public:
		virtual ~SakuraHeartScene();
		
		virtual void will_become_current (ISceneManager *);
		virtual void will_revoke_current (ISceneManager *);
		
		virtual bool event (const EventInput & input);
		virtual bool motion (const MotionInput & input);
		virtual bool resize (const ResizeInput & input);
		
		virtual void render_frame_for_time (TimeT time);
	};
	
	SakuraHeartScene::~SakuraHeartScene() {
	}
	
	void SakuraHeartScene::will_become_current(ISceneManager * manager)
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
		
		auto texture_manager = new TextureManager;
		auto shader_manager = new ShaderManager;
		
		_sakura_heart_renderer = new SakuraHeartRenderer(resource_loader(), texture_manager, shader_manager, _viewport);
		_heart_particles = new HeartParticles;
		
		glClearColor(0.0, 0.0, 0.0, 1.0);
		
		logger()->log(LOG_INFO, "Will become current.");
	}
	
	void SakuraHeartScene::will_revoke_current(ISceneManager * manager)
	{
		Scene::will_revoke_current(manager);
		
		logger()->log(LOG_INFO, "Will revoke current.");
		
		_sakura_heart_renderer = nullptr;
	}
	
	bool SakuraHeartScene::event(const EventInput &input) {
		// Grab the cursor
		Ref<IContext> context = manager()->display_context();
		
		if (input.event() == EventInput::RESUME) {
			context->set_cursor_mode(CURSOR_GRAB);
			logger()->log(LOG_INFO, LogBuffer() << "Cursor Mode: " << context->cursor_mode());
			
			return true;
		} else if (input.event() == EventInput::PAUSE) {
			context->set_cursor_mode(CURSOR_NORMAL);
			logger()->log(LOG_INFO, LogBuffer() << "Cursor Mode: " << context->cursor_mode());
			
			return true;
		}
		
		return false;
	}
	
	bool SakuraHeartScene::resize (const ResizeInput & input)
	{
		logger()->log(LOG_INFO, LogBuffer() << "Resizing to " << input.new_size());
		
		_viewport->set_bounds(AlignedBox<2>(ZERO, input.new_size()));
		glViewport(0, 0, input.new_size()[WIDTH], input.new_size()[HEIGHT]);
		
		check_graphics_error();
		
		return Scene::resize(input);
	}
	
	bool SakuraHeartScene::motion (const MotionInput & input) {
		_camera->process(input);
		
		return true;
	}
	
	void SakuraHeartScene::render_frame_for_time (TimeT time) {
		Scene::render_frame_for_time(time);
		
		_heart_particles->update(time);
		_heart_particles->add();

		/// Dequeue any button/motion/resize events and process them now (in this thread).
		manager()->process_pending_events(this);
				
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		_sakura_heart_renderer->render(_heart_particles);
	}
	
	class SakuraHeartApplicationDelegate : public Object, implements IApplicationDelegate
	{
		protected:
			Ref<Context> _context;
		
			virtual ~SakuraHeartApplicationDelegate ();
			virtual void application_did_finish_launching (IApplication * application);
			
			virtual void application_will_enter_background (IApplication * application);
			virtual void application_did_enter_foreground (IApplication * application);
	};
	
	SakuraHeartApplicationDelegate::~SakuraHeartApplicationDelegate() {
	}
	
	void SakuraHeartApplicationDelegate::application_did_finish_launching (IApplication * application)
	{		
		Ref<Dictionary> config = new Dictionary;
		_context = application->create_context(config);
		
		Ref<Thread> thread = new Events::Thread;
		Ref<ILoader> loader = SceneManager::default_resource_loader();
		
		Ref<SceneManager> scene_manager = new SceneManager(_context, thread->loop(), loader);
		
		scene_manager->push_scene(new SakuraHeartScene);
	}
	
	void SakuraHeartApplicationDelegate::application_will_enter_background (IApplication * application)
	{
		logger()->log(LOG_INFO, "Entering background...");

		_context->stop();
	}
	
	void SakuraHeartApplicationDelegate::application_did_enter_foreground (IApplication * application)
	{
		logger()->log(LOG_INFO, "Entering foreground...");
		
		_context->start();
	}
}

int main (int argc, const char * argv[])
{
	using namespace SakuraHeart;
	using namespace Dream::Client::Display;
	
	Ref<SakuraHeartApplicationDelegate> delegate = new SakuraHeartApplicationDelegate;
	IApplication::start(delegate);
	
    return 0;
}
