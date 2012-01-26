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
	
	/*
		MeshT _mesh = ...;
		
		VertexArray _grid_array;
		VertexBuffer _grid_vertices(GL_ARRAY_BUFFER, _mesh.vertices);
		VertexBuffer _grid_indices(GL_ELEMENT_ARRAY_BUFFER, _mesh.indices);
	 
		_grid_array->draw();
	 */
	
	class VertexArray : private NonCopyable {
	protected:
		GLuint _handle;
		
	public:
		GLuint handle() const { return _handle; }
		
		VertexArray() {
			glGenVertexArrays(1, &_handle);
		}
		
		~VertexArray() {
			glDeleteVertexArrays(1, &_handle);
		}
		
		void bind() {
			glBindVertexArray(_handle);
		}
		
		void unbind() {
			glBindVertexArray(0);
		}
		
		// These functions facilitate canonical usage where data is stored in vertex buffers.
		void draw(GLenum mode, GLsizei count, GLenum type) {
			glDrawElements(mode, count, type, 0);
		}
		
		void set_attribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, std::ptrdiff_t offset) {
			glVertexAttribPointer(index, size, type, normalized, stride, (const GLvoid *)offset);
		}
	};
	
	class VertexBuffer : private NonCopyable {
	protected:
		GLuint _handle;
		GLenum _target;
		
	public:
		GLuint handle() const { return _handle; }
		GLenum target() const { return _target; }
		
		template <typename AnyT>
		void buffer_data(Array<AnyT> array, GLenum usage) {
			glBufferData(_target, array.length(), array.data(), usage);
		}
		
		VertexBuffer(GLenum target) : _target(target) {
			glGenBuffers(1, &_handle);
		}
		
		~VertexBuffer() {
			glDeleteBuffers(1, &_handle);
		}
		
		void attach(VertexArray & vertex_array) {
			glBindBuffer(_target, _handle);
		}
	};
	
	template <typename MeshT>
	class MeshBuffer : private NonCopyable {
	protected:
		Shared<MeshT> _mesh;
		
		VertexArray _vertex_array;
		
		GLenum _index_buffer_usage, _vertex_buffer_usage;
		
		VertexBuffer _index_buffer;
		VertexBuffer _vertex_buffer;
		
		std::size_t _count;
		
		bool _invalid;
				
	public:
		MeshBuffer(Shared<MeshT> mesh) : _mesh(mesh), _index_buffer_usage(GL_STATIC_DRAW), _vertex_buffer_usage(GL_STATIC_DRAW), _index_buffer(GL_ELEMENT_ARRAY_BUFFER), _vertex_buffer(GL_ARRAY_BUFFER), _invalid(true) {
			
		}
		
		virtual ~MeshBuffer() {
			
		}
		
		/// This function is used as follows:
		/// _mesh_buffer->associate(_position_attribute, MeshT::VertexT::position);
		template<class T, typename U>
		void associate(GLuint index, U T::* member, bool normalized = false) {
			_vertex_array.set_attribute(index, U::ELEMENTS, GLTypeTraits<typename U::ElementT>::TYPE, normalized, sizeof(T), member_offset(member));
		}
		
		void set_usage(GLenum usage) {
			_index_buffer_usage = _vertex_buffer_usage = usage;
		}
		
		void set_index_buffer_usage(GLenum usage) {
			_index_buffer_usage = usage;
		}
		
		void set_vertex_buffer_usage(GLenum usage) {
			_vertex_buffer_usage = usage;
		}
		
		void set_mesh(Shared<MeshT> mesh) {
			if (_mesh != mesh) {
				_mesh = mesh;				
			}
			
			_invalid = true;
		}
		
		void invalidate() {
			_invalid = true;
		}
		
		void upload() {
			if (_invalid) {				
				_vertex_array.bind();
				
				_index_buffer.attach(_vertex_array);
				_index_buffer.buffer_data(_mesh->indices, _index_buffer_usage);
				
				_vertex_buffer.attach(_vertex_array);
				_vertex_buffer.buffer_data(_mesh->vertices, _vertex_buffer_usage);
				
				_vertex_array.unbind();
				
				// Keep track of the number of indices uploaded for drawing:
				_count = _mesh->indices.size();
				
				// The mesh buffer is now okay for drawing:
				_invalid = false;
			}
		}
		
		void draw(GLenum mode) {
			// Ensure the geometry is available for rendering:
			upload();
			
			_vertex_array.draw(mode, _count, GLTypeTraits<typename MeshT::IndexT>::TYPE);
		}
	};
	
	typedef Geometry::Mesh<> MeshT;
	
	class DemoScene : public Scene
	{
	protected:
		// These are the locations of default attributes in the shader programs used:
		enum ProgramAttributes {
			POSITION = 0,
			COLOR = 1,
			NORMAL = 2,
			MAPPING = 3
		};
		
		Ref<BirdsEyeCamera> _camera;
		Ref<IProjection> _projection;
		Ref<Viewport> _viewport;
		
		Ref<Program> _shader_program;
		GLuint _position_uniform, _light_positions_uniform, _color_attribute, _position_attribute, _normal_attribute, _mapping_attribute;
		
		GLuint _diffuse_uniform;
		IndexT _crate_texture;
		
		GLuint _object_vertex_array, _grid_vertex_array;
		Vec2 _point;
		Vec4 _light_positions[2];
		
		MeshT _object_mesh, _grid_mesh;
		
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
			
			_color_attribute = _shader_program->attribute_location("color");
			_position_attribute = _shader_program->attribute_location("position");
			_normal_attribute = _shader_program->attribute_location("normal");
			_mapping_attribute = _shader_program->attribute_location("mapping");
			check_error();
		}
				
		{
			using namespace Geometry;
			
			//Generate::cube(_mesh, Geometry::AlignedBox<3>::from_center_and_size(ZERO, 1.0));
			//Generate::shade_square_cube(_mesh);
			
			Generate::sphere(_object_mesh, 0.5, 12, 12);
			Generate::solid_color(_object_mesh, Vec4(0.5, 0.5, 0.5, 1.0));
		}
		
		{
			glGenVertexArrays(1, &_object_vertex_array);
			glBindVertexArray(_object_vertex_array);
			
			GLuint vertex_buffer, index_buffer;
			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, _object_mesh.vertices.length(), _object_mesh.vertices.data(), GL_STATIC_DRAW);
			
			check_error();
			
			glEnableVertexAttribArray(_position_attribute);
			glEnableVertexAttribArray(_color_attribute);
			glEnableVertexAttribArray(_normal_attribute);
			glEnableVertexAttribArray(_mapping_attribute);
			
			check_error();
			
			glVertexAttribPointer(_position_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::position));
			glVertexAttribPointer(_color_attribute, 4, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::color));
			glVertexAttribPointer(_normal_attribute, 3, GL_FLOAT, GL_TRUE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::normal));
			glVertexAttribPointer(_mapping_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::mapping));
			
			check_error();
			
			glGenBuffers(1, &index_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _object_mesh.indices.length(), _object_mesh.indices.data(), GL_STATIC_DRAW);
			
			check_error();
		}
		
		{
			glGenVertexArrays(1, &_grid_vertex_array);
			glBindVertexArray(_grid_vertex_array);
			
			{
				Generate::grid(_grid_mesh, 32, 2.0);
			}
			
			GLuint vertex_buffer, index_buffer;
			glGenBuffers(1, &vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, _grid_mesh.vertices.length(), _grid_mesh.vertices.data(), GL_STATIC_DRAW);
			
			check_error();
			
			glEnableVertexAttribArray(_position_attribute);
			glEnableVertexAttribArray(_normal_attribute);
			glEnableVertexAttribArray(_color_attribute);
			glEnableVertexAttribArray(_mapping_attribute);
			
			glVertexAttribPointer(_position_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::position));
			glVertexAttribPointer(_color_attribute, 4, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::color));
			glVertexAttribPointer(_normal_attribute, 3, GL_FLOAT, GL_TRUE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::normal));
			glVertexAttribPointer(_mapping_attribute, 2, GL_FLOAT, GL_FALSE, sizeof(MeshT::VertexT), (const GLvoid *)member_offset(&MeshT::VertexT::mapping));
			
			check_error();
			
			glGenBuffers(1, &index_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _grid_mesh.indices.length(), _grid_mesh.indices.data(), GL_STATIC_DRAW);
			
			check_error();
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
		
		glBindVertexArray(_grid_vertex_array);
		glDrawElements(GL_LINES, _grid_mesh.indices.size(), GLTypeTraits<MeshT::IndexT>::TYPE, 0);
		
		glBindVertexArray(_object_vertex_array);
		glDrawElements(GL_TRIANGLE_STRIP, _object_mesh.indices.size(), GLTypeTraits<MeshT::IndexT>::TYPE, 0);

		glPolygonOffset(0.0, -0.1);
		glDrawElements(GL_LINE_STRIP, _object_mesh.indices.size(), GLTypeTraits<MeshT::IndexT>::TYPE, 0);
		glPolygonOffset(0.0, 0.0);
		
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
