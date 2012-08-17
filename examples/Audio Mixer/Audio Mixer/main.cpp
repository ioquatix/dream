
#include <Dream/Client/Client.h>

#include <Dream/Core/Timer.h>
#include <Dream/Client/Display/Application.h>

#include <Dream/Client/Audio/Stream.h>
#include <Dream/Client/Audio/OggResource.h>

#include <Dream/Client/Graphics/Graphics.h>

namespace AudioMixer
{
	using namespace Dream;
	using namespace Dream::Client;
	using namespace Dream::Client::Display;
	
	class AudioVisualisation : public Scene
	{
		protected:
			Ref<Audio::Mixer> _mixer;
			Ref<Audio::Source> _background_music_source;
			Ref<Audio::Stream> _background_music;
						
		public:
			AudioVisualisation ();
			virtual ~AudioVisualisation ();
			
			virtual void will_become_current (ISceneManager *);
			
			virtual bool button(const ButtonInput &);
			virtual bool event(const EventInput &);
			
			virtual void render_frame_for_time (TimeT time);
	};
		
	AudioVisualisation::AudioVisualisation ()
	{
	}
	
	AudioVisualisation::~AudioVisualisation ()
	{
	}
	
	void AudioVisualisation::will_become_current (ISceneManager * manager)
	{
		Scene::will_become_current(manager);
		
		using namespace Dream::Client;
		
		// Get a handle to the system audio mixer.
		_mixer = Audio::Mixer::shared_mixer();
		
		// Create a audio source which will be used for the background music.
		_background_music_source = _mixer->create_source();
		// The source is not positional.
		_background_music_source->set_local();
	
		// Load an ogg vorbis audio loop and play it using the current event loop.
		Ref<Audio::OggResource> ogg_resource = resource_loader()->load<Audio::OggResource>("Music/Collective Is Moving (LQ)");
		_background_music = ogg_resource->create_stream(_background_music_source);
		_background_music->play(manager->event_loop());
	}
	
	bool AudioVisualisation::event (const EventInput & eventInput)
	{
		switch(eventInput.event()) {
			case EventInput::EXIT:
			case EventInput::PAUSE:
				_background_music->pause();
				return true;
				break;
				
			case EventInput::RESUME:
				_background_music->play(manager()->event_loop());
				return true;
				break;
		}
		
		return false;
	}
	
	bool AudioVisualisation::button(const ButtonInput & ipt)
	{
		if (ipt.button_pressed('f')) {
			std::cout << "Fade out..." << std::endl;
			_background_music->fade_out(manager()->event_loop(), 3.0);
			return true;
		} else if (ipt.button_pressed('s')) {
			std::cout << "Stopping..." << std::endl;
			_background_music->stop();
			return true;
		} else if (ipt.button_pressed('a')) {
			std::cout << "Playing..." << std::endl;
			_background_music->source()->set_gain(1.0);
			_background_music->play(manager()->event_loop());
			return true;
		} else if (ipt.button_pressed('g')) {
			std::cout << "Fade in..." << std::endl;
			_background_music->fade_in(manager()->event_loop(), 3.0);
			return true;			
		} else if (ipt.button_pressed('d')) {
			std::cout << "Pausing..." << std::endl;
			_background_music->pause();
			return true;
		} else if (ipt.button_pressed('w')) {
			// May cause popping artifacts
			_background_music->source()->set_gain(0.0);
			return true;
		} else if (ipt.button_pressed('e')) {
			// May cause popping artifacts
			_background_music->source()->set_gain(1.0);
			return true;
		}
		
		return false;
	}
	
	void AudioVisualisation::render_frame_for_time(TimeT time)
	{
		// Clear the window.
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Process incoming user events on the render thread.
		manager()->process_pending_events(this);
	}
}

int main (int argc, char** argv) {
	using namespace Dream;
	using namespace Dream::Client::Display;

	Ref<Dictionary> config = new Dictionary;
	IApplication::run_scene(new AudioMixer::AudioVisualisation, config);
		
	return 0;
}
