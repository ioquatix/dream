//
//  Client/Audio/Mixer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/02/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_AUDIO_MIXER_H
#define _DREAM_CLIENT_AUDIO_MIXER_H

#include "../Client.h"
#include "../../Core/System.h"
#include "../../Events/Fader.h"

#include <Euclid/Numerics/Vector.h>
#include <Euclid/Numerics/Interpolate.h>

#if defined(TARGET_OS_LINUX)
#include <AL/al.h>
#include <AL/alc.h>
#elif defined(TARGET_OS_MAC)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#error Could not find OpenAL headers.
#endif

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			using namespace Dream::Core;
			using Euclid::Numerics::Vec3;
			using namespace Dream::Events;

			class Sound;
			class IStreamable;

			typedef double GainT;

			class AudioError {
			protected:
				ALint _error_number;
				StringT _message;

			public:
				AudioError(ErrorNumberT error_number, StringT error_description, StringT error_target);

				StringT what () const;

				static void check (StringT what);
				static void reset ();
			};

			class Source : public Object {
			protected:
				ALuint _source_id;
				Ref<Sound> _sound;

			public:
				Source ();
				virtual ~Source ();

				void set_parameter(ALenum parameter, float value);

				void set_pitch (float pitch);
				void set_gain (GainT gain);
				void set_position (const Vec3 &);
				void set_velocity (const Vec3 &);

				float pitch ();
				GainT gain ();
				Vec3 position ();
				Vec3 velocity ();

				ALint sample_offset ();
				TimeT time_offset ();
				std::size_t byte_offset ();

				void set_local ();

				void set_reference_distance (float dist);

				void set_sound (ALuint buffer_id);
				void set_sound (Ptr<Sound> sound);

				// Streaming buffers
				void queue_buffers (ALuint * buffers, std::size_t count);
				void unqueue_buffers (ALuint * buffers, std::size_t count);

				bool stream_buffers (IStreamable * stream);

				ALint processed_buffer_count ();
				ALint queued_buffer_count ();

				void set_looping (bool);

				void play ();
				void pause ();
				void stop ();

				bool is_playing () const;
			};

			template <typename ValueT>
			class LinearKnob : implements IKnob {
			protected:
				Ref<Source> _source;
				ALenum _parameter;
				ValueT _begin, _end;

			public:
				LinearKnob (Ptr<Source> source, ALenum parameter, ValueT begin, ValueT end) : _source(source), _parameter(parameter), _begin(begin), _end(end)
				{
				}

				virtual ~LinearKnob ()
				{
				}

				virtual void update (TimeT time)
				{
					ValueT value = Euclid::Numerics::linear_interpolate(time, _begin, _end);
					_source->set_parameter(_parameter, value);
				}
			};

			class IStreamable : implements IObject {
			public:
				virtual ~IStreamable ();

				// Return false if there are no more buffers.
				virtual bool load_next_buffer (Ptr<Source> source, ALuint buffer) abstract;

				virtual void buffer_data(Ptr<Source> source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
			};

			class Mixer : public Object {
			protected:
				ALCdevice * _audio_device;
				ALCcontext * _audio_context;

			public:
				static Ref<Mixer> shared_mixer ();

				Mixer ();
				virtual ~Mixer ();

				void suspend_processing ();
				void resume_processing ();

				Ref<Source> create_source ();

				void set_listener_position (const Vec3 &);
				void set_listener_velocity (const Vec3 &);
				void set_listener_orientation (const Vec3 & looking_at, const Vec3 & up);
			};
		}
	}
}

#endif
