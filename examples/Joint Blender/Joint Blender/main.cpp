//
//  main.cpp
//  Joint Blender
//
//  Created by Samuel Williams on 9/06/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
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

#include <Dream/Geometry/Generate/Planar.h>
#include <Dream/Geometry/Generate/Cylinder.h>

#include <functional>

namespace Dream {	
	namespace Geometry {
		namespace Generate {
			
			template <typename MeshT>
			void cylinder(MeshT & mesh, const ISpline<3> * spline, RealT radius, std::size_t slices, bool cap_start, bool cap_end) {
				std::vector<RealT> times = spline->times_at_resolution(20, 2);
				std::vector<std::vector<std::size_t>> indices(slices);
				
				for (std::size_t c = 0; c < slices; ++c) {
					RealT rotation = R360 * ((RealT)c / (RealT)slices);
					
					for (auto time : times) {
						Vec3 v = spline->point_at_time(time);
						Vec3 tangent = spline->tangent_at_time(time);
						
						Vec3 pt = Mat44::rotating_matrix(-rotation, tangent) * spline->normal_at_time(time) * radius;
						
						std::size_t index = mesh->addVertex(v + pt);
						indices[c].push_back(index);
					}
				}
				
				int rotation = 0;
				///iterate over each stack idx
				for (std::size_t i = 0; i < (indices[0].size() - 1); i += 1) {
					std::vector<std::size_t> left, right;
					//indices[rotation around][stack idx]
					
					for (std::size_t j = 0; j < indices.size(); j++) {
						left.push_back(indices.at(j).at(i));
						right.push_back(indices.at(j).at((i+1) % indices[0].size()));
					}
					
					rotation = calculateRotationOffset(mesh->vertices(left), mesh->vertices(right));
					
					for (std::size_t c = 0; c < indices.size(); c += 1) {
						std::size_t c2 = (c+1) % indices.size(); //rotation bottom
						std::size_t i2 = (i+1) % indices[0].size();
						
						std::size_t idxs[2][2];
						idxs[0][0] = indices.at((c2) % indices.size()).at(i);
						idxs[0][1] = indices.at((c) % indices.size()).at(i);
						idxs[1][0] = indices.at((c2+rotation) % indices.size()).at(i2); 
						idxs[1][1] = indices.at((c+rotation) % indices.size()).at(i2);
						
						mesh->addTriangleSurface(idxs[0][0], idxs[1][0], idxs[0][1]);
						mesh->addTriangleSurface(idxs[0][1], idxs[1][0], idxs[1][1]);
						
						mesh->vertex(idxs[0][1])->setColor(vec(0.0, 1.0, 0.0, 1.0));
					}
				}
				
				for (std::size_t k = 0; k < indices[0].size(); k++) {
					mesh->vertex(indices[0][k])->setColor(vec(1.0, 0.0, 0.0, 1.0));
				}
			}
		}
	}
}

namespace JointBlender {
	using namespace Dream;
	using namespace Dream::Events;
	using namespace Dream::Resources;
	using namespace Dream::Renderer;
	using namespace Dream::Client::Display;
	using namespace Dream::Client::Graphics;
	
	template <unsigned D>
	class DynamicMuStretch {
		// This class combines both linear and exponential MU functions, so you can interpolate between
		// them easily from the user interface.
	public:
		RealT k; // in [0.0, 1.0]
		
		DynamicMuStretch() : k(1.0) {}
		
		Vector<D> mu(const Spline<D>* s, unsigned n) {
			return HermiteSpline<D>::four_point_linear_mu(s, n) * k
			+ HermiteSpline<D>::four_point_exponential_mu(s, n) * (1.0 - k);
		}
		
		RealT length(RealT m) {
			// Really terrible helper function
			return m * k + ((m * m) / 30.0) * (1.0 - k);
		}
		
		void more() {
			k = Math::clamp(k + 0.1);
			std::cout << "K: " << k << std::endl;
		}
		
		void less() {
			k = Math::clamp(k - 0.1);
			std::cout << "K: " << k << std::endl;
		}
	};
	
	class JointBlenderScene : public Scene {
	protected:
		typedef Geometry::Mesh<> MeshT;
		Ref<MeshBuffer<MeshT>> _upper, _lower, _joint, _grid;
		
		bool _centers;
		Ref<RendererState> _renderer_state;
		Ref<Program> _wireframe_program;
		
		DynamicMuStretch<3> _dynamic_mu;
		
		virtual void will_become_current (ISceneManager *);
		//virtual void will_revoke_current (ISceneManager *);
		
		virtual bool button(const ButtonInput & input);
		virtual bool resize (const ResizeInput & input);
		virtual bool motion (const MotionInput & input);
		
	public:
		virtual ~JointBlenderScene ();
		
		virtual void render_frame_for_time (TimeT time);
		
		//virtual void update ();
	};
	
	void JointBlenderScene::will_become_current(ISceneManager * manager) {
		Scene::will_become_current(manager);
		
		_renderer_state = new RendererState;
		_renderer_state->resource_loader = manager->resource_loader();
		_renderer_state->texture_manager = new TextureManager;
		_renderer_state->shader_manager = new ShaderManager;
		
		Ref<BirdsEyeCamera> camera = new BirdsEyeCamera;
		camera->set_distance(100);
		camera->set_multiplier(Vec3(0.1, 0.1, 0.01));
		
		Ref<IProjection> projection = new PerspectiveProjection(R90 * 0.7, 1, 1024 * 12);
		
		_renderer_state->viewport = new Viewport(camera, projection);
		_renderer_state->viewport->set_bounds(AlignedBox<2>(ZERO, manager->display_context()->size()));

		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);

		{
			Shared<MeshT> mesh = new MeshT;
			Generate::cylinder(*mesh, 10, 10, 50, 16, 3, false, false);
			mesh->layout = LINES;
			
			mesh->apply(Mat44::translating_matrix(Vec3(40, 0, 0)));
			mesh->apply(Mat44::rotating_matrix(R180, Vec3(0, 0, 1)));
			
			_lower = new MeshBuffer<MeshT>(mesh);
			auto binding = _lower->vertex_array().binding();
			binding.attach(_lower->index_buffer());
			auto attributes = binding.attach(_lower->vertex_buffer());
			attributes[0] = &MeshT::VertexT::position;
		}
		
		{
			_upper = new MeshBuffer<MeshT>;
			auto binding = _upper->vertex_array().binding();
			binding.attach(_upper->index_buffer());
			auto attributes = binding.attach(_upper->vertex_buffer());
			attributes[0] = &MeshT::VertexT::position;
		}
		
		{
			_joint = new MeshBuffer<MeshT>;
			auto binding = _joint->vertex_array().binding();
			binding.attach(_joint->index_buffer());
			auto attributes = binding.attach(_joint->vertex_buffer());
			attributes[0] = &MeshT::VertexT::position;
		}
		
		{
			Shared<MeshT> mesh = new MeshT;
			mesh->layout = LINES;
			Generate::grid(*mesh, 32, 2.0);
			
			_grid = new MeshBuffer<MeshT>(mesh);
			
			auto binding = _grid->vertex_array().binding();
			
			// Attach indices
			binding.attach(_grid->index_buffer());
			
			// Attach attributes
			auto attributes = binding.attach(_grid->vertex_buffer());
			attributes[0] = &MeshT::VertexT::position;
			
		}
		
		_centers = false;
		
		{
			_wireframe_program = _renderer_state->load_program("wireframe");
			_wireframe_program->set_attribute_location("position", 0);
			_wireframe_program->link();
			
			auto binding = _wireframe_program->binding();
			binding.set_uniform("major_color", Vec4(1.0, 1.0, 1.0, 1.0));
		}
	}
	
	JointBlenderScene::~JointBlenderScene () {
		
	}
	
	bool JointBlenderScene::resize (const ResizeInput & input)
	{
		logger()->log(LOG_INFO, LogBuffer() << "Resizing to " << input.new_size());
		
		_renderer_state->viewport->set_bounds(AlignedBox<2>(ZERO, input.new_size()));
		glViewport(0, 0, input.new_size()[WIDTH], input.new_size()[HEIGHT]);
		
		check_graphics_error();
		
		return Scene::resize(input);
	}
	
	bool JointBlenderScene::motion (const MotionInput & input)
	{
		_renderer_state->viewport->camera()->process(input);
				
		return true;
	}
	
	bool JointBlenderScene::button(const ButtonInput & input) {
		if (input.state() == Pressed) {
			switch (input.key().button()) {
				case ',':
					_dynamic_mu.less();
					return true;
				case '.':
					_dynamic_mu.more();
					return true;
			}
		}
		
		return Scene::button(input);
	}
	
	void JointBlenderScene::render_frame_for_time(TimeT time) {
		Scene::render_frame_for_time(time);
		
		{
			Shared<MeshT> mesh = new MeshT;
			Generate::cylinder(*mesh, 10, 10, 50, 16, 3, false, false);
			//mesh->layout = LINES;
			
			mesh->apply(Mat44::translating_matrix(Vec3(40, 0, 0)));
			
			RealT rotation = -(0.5 * R180 * Number<TimeT>::sin(Number<TimeT>::wrap(time, R360)));
			mesh->apply(Mat44::rotating_matrix(rotation, Vec3(0, 0, 1)));
			
			_upper->set_mesh(mesh);
		}
		
		/// Dequeue any button/motion/resize events and process them now (in this thread).
		manager()->process_pending_events(this);
		
		check_graphics_error();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		{
			typedef HermiteSpline<3> SplineT;
			
			std::vector<SplineT> splines(16);
			
			for (unsigned i = 0; i < 16; ++i) {
				unsigned k = (-i) % 16;
				unsigned j = (i - 8) % 16;
				
				splines[i] = SplineT(std::bind(&DynamicMuStretch<3>::mu, _dynamic_mu, std::placeholders::_1, std::placeholders::_2));
				
				splines[i].set_closed(false);
				
				splines[i].points().push_back(_lower->mesh()->vertices[16*2+k].position);
				splines[i].points().push_back(_lower->mesh()->vertices[k].position);
				splines[i].points().push_back(_upper->mesh()->vertices[j].position);
				splines[i].points().push_back(_upper->mesh()->vertices[16*2+j].position);
			}
			
			Shared<MeshT> mesh = new MeshT;
			Generate::cylinder(*mesh, splines, 5.0);
			mesh->layout = LINES;
			_joint->set_mesh(mesh);
		}
		
		{
			auto binding = _wireframe_program->binding();
			
			binding.set_uniform("display_matrix", _renderer_state->viewport->display_matrix());
			
			binding.set_uniform("major_color", Vec4(0.6, 0.62, 0.94, 1.0));
			_lower->draw();
			_upper->draw();
			binding.set_uniform("major_color", Vec4(0.0, 0.0, 1.0, 1.0));
			_joint->draw();
			//_grid->draw();
		}
	}
}

int main (int argc, const char * argv[])
{
	using namespace Dream;
	using namespace Dream::Core;
	using namespace Dream::Client::Display;
	
	Ref<Dictionary> config = new Dictionary;
	IApplication::run_scene(new JointBlender::JointBlenderScene, config);
	
	return 0;
}
