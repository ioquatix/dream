//
//  Client/Audio/Mixer.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/02/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Mixer.h"
#include "Sound.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
// MARK: -
// MARK: AudioError

			AudioError::AudioError(ErrorNumberT error_number, StringT error_description, StringT error_target) : _error_number(error_number) {
				StringStreamT buffer;
				buffer << "Audio Error #" << error_number << ": " << error_description << "(" << error_target << ")";
				_message = buffer.str();
			}

			StringT AudioError::what () const
			{
				return _message;
			}

			void AudioError::check (StringT what)
			{
				ALint error = alGetError();

				if (error != AL_NO_ERROR) {
					throw AudioError(error, alGetString(error), what);
				}
			}

			void AudioError::reset ()
			{
				ALint error = AL_NO_ERROR;

				while ((error = alGetError()) != AL_NO_ERROR) {
					logger()->log(LOG_ERROR, LogBuffer() << "Unhandled audio error #" << error << ": " << alGetString(error));
				}
			}

// MARK: -
// MARK: Source

			Source::Source ()
			{
				AudioError::reset();

				alGenSources(1, &_source_id);
				//set_pitch(1.0);
				//set_gain(1.0);
				//set_position(Vec3(ZERO));
				//set_velocity(Vec3(ZERO));
				//set_reference_distance(100);

				AudioError::check("Allocating Source");
			}

			Source::~Source ()
			{
				AudioError::reset();

				logger()->log(LOG_INFO, LogBuffer() << "Deleting audio source: " << _source_id);

				alDeleteSources(1, &_source_id);

				AudioError::check("Deallocating Source");
			}

			void Source::set_parameter(ALenum parameter, float value)
			{
				alSourcef(_source_id, parameter, value);
			}

			void Source::set_pitch (float pitch)
			{
				alSourcef(_source_id, AL_PITCH, pitch);
			}

			void Source::set_gain (float gain)
			{
				alSourcef(_source_id, AL_GAIN, gain);
			}

			void Source::set_position (const Vec3 & position)
			{
				alSourcefv(_source_id, AL_POSITION, position.value());
			}

			void Source::set_velocity (const Vec3 & velocity)
			{
				alSourcefv(_source_id, AL_VELOCITY, velocity.value());
			}

			float Source::pitch ()
			{
				float value;

				alGetSourcef(_source_id, AL_PITCH, &value);

				return value;
			}

			float Source::gain ()
			{
				float value;

				alGetSourcef(_source_id, AL_GAIN, &value);

				return value;
			}

			Vec3 Source::position ()
			{
				Vec3 value;

				alGetSourcefv(_source_id, AL_POSITION, (ALfloat *)value.value());

				return value;
			}

			Vec3 Source::velocity ()
			{
				Vec3 value;

				alGetSourcefv(_source_id, AL_VELOCITY, (ALfloat *)value.value());

				return value;
			}

			ALint Source::sample_offset ()
			{
				ALint offset = 0;

				alGetSourcei(_source_id, AL_SAMPLE_OFFSET, &offset);

				return offset;
			}

			TimeT Source::time_offset ()
			{
				ALfloat offset = 0;

				alGetSourcef(_source_id, AL_SEC_OFFSET, &offset);

				return offset;
			}

			IndexT Source::byte_offset ()
			{
				ALint offset = 0;

				alGetSourcei(_source_id, AL_BYTE_OFFSET, &offset);

				return offset;
			}

			void Source::set_local ()
			{
				alSource3f(_source_id, AL_POSITION, 0.0, 0.0, 0.0);
				alSource3f(_source_id, AL_VELOCITY, 0.0, 0.0, 0.0);
				alSource3f(_source_id, AL_DIRECTION, 0.0, 0.0, 0.0);
				alSourcef(_source_id, AL_ROLLOFF_FACTOR, 0.0);
				alSourcei(_source_id, AL_SOURCE_RELATIVE, AL_TRUE);
			}

			void Source::set_reference_distance (float dist)
			{
				alSourcef(_source_id, AL_REFERENCE_DISTANCE, dist);
			}

			void Source::set_sound (ALuint buffer_id)
			{
				alSourcei(_source_id, AL_BUFFER, buffer_id);
			}

			void Source::set_sound (Ptr<Sound> sound)
			{
				_sound = sound;

				alSourcei(_source_id, AL_BUFFER, sound->_buffer_id);
			}

			void Source::queue_buffers (ALuint * buffers, std::size_t count)
			{
				AudioError::reset();
				alSourceQueueBuffers(_source_id, count, buffers);
				AudioError::check("Source Queue Buffers");
			}

			void Source::unqueue_buffers (ALuint * buffers, std::size_t count)
			{
				AudioError::reset();
				alSourceUnqueueBuffers(_source_id, count, buffers);
				AudioError::check("Source Unqueue Buffers");
			}

			ALint Source::processed_buffer_count ()
			{
				ALint value = 0;

				alGetSourcei(_source_id, AL_BUFFERS_PROCESSED, &value);

				AudioError::reset();

				return value;
			}

			ALint Source::queued_buffer_count ()
			{
				ALint value = 0;

				alGetSourcei(_source_id, AL_BUFFERS_QUEUED, &value);

				AudioError::reset();

				return value;
			}

			IStreamable::~IStreamable ()
			{
			}

			void IStreamable::buffer_data(Ptr<Source> source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
			{
				AudioError::reset();
				alBufferData(buffer, format, data, size, freq);
				AudioError::check("Buffering Data");
			}

			bool Source::stream_buffers (IStreamable * stream) {
				ALint processed = processed_buffer_count();
				bool complete = true;

				// std::cerr << "Processed buffers = " << processed << std::endl;

				AudioError::reset();

				while (processed--) {
					ALuint buffer;
					unqueue_buffers(&buffer, 1);

					bool result = stream->load_next_buffer(this, buffer);

					if (result) {
						queue_buffers(&buffer, 1);
					} else {
						complete = false;
					}
				}

				AudioError::check("Streaming Buffers");

				return complete;
			}

			void Source::set_looping (bool mode)
			{
				alSourcei(_source_id, AL_LOOPING, mode);
			}

			void Source::play ()
			{
				/*
				float pitch, gain, min_gain, max_gain, max_distance, rolloff_factor, cone_outer_gain, cone_inner_angle, cone_outer_angle, Ref_distance;
				al_get_sourcef(_source_id, AL_PITCH, &pitch);
				al_get_sourcef(_source_id, AL_GAIN, &gain);
				al_get_sourcef(_source_id, AL_MIN_GAIN, &min_gain);
				al_get_sourcef(_source_id, AL_MAX_GAIN, &max_gain);
				al_get_sourcef(_source_id, AL_MAX_DISTANCE, &max_distance);
				al_get_sourcef(_source_id, AL_ROLLOFF_FACTOR, &rolloff_factor);
				al_get_sourcef(_source_id, AL_CONE_OUTER_GAIN, &cone_outer_gain);
				al_get_sourcef(_source_id, AL_CONE_INNER_ANGLE, &cone_inner_angle);
				al_get_sourcef(_source_id, AL_CONE_OUTER_ANGLE, &cone_outer_angle);
				al_get_sourcef(_source_id, AL_Ref_DISTANCE, &Ref_distance);

				std::cout << " ==== " << this << " ==== " << std::endl;
				std::cout << "Audio properties: " << pitch << " " << gain << " " << min_gain << " " << max_gain << " " << max_distance << " " << rolloff_factor
				<< " " << cone_outer_gain << " " << cone_inner_angle << " " << cone_outer_angle << " " << Ref_distance << std::endl;

				ALint source_relative, buffer, source_state, buffers_queued, buffers_processed;
				al_get_sourcei(_source_id, AL_SOURCE_RELATIVE, &source_relative);
				al_get_sourcei(_source_id, AL_BUFFER, &buffer);
				al_get_sourcei(_source_id, AL_SOURCE_STATE, &source_state);
				al_get_sourcei(_source_id, AL_BUFFERS_QUEUED, &buffers_queued);
				al_get_sourcei(_source_id, AL_BUFFERS_PROCESSED, &buffers_processed);

				std::cout << "Source State: " << source_relative << " " << buffer << " " << source_state << " " << buffers_queued
				<< " " << buffers_processed << std::endl;

				Vec3 position, velocity, direction;
				al_get_sourcefv(_source_id, AL_POSITION, (ALfloat *)position.value());
				al_get_sourcefv(_source_id, AL_VELOCITY, (ALfloat *)velocity.value());
				al_get_sourcefv(_source_id, AL_DIRECTION, (ALfloat *)direction.value());

				std::cout << "Pos: " << position << " Vel: " << velocity << " Dir: " << direction << std::endl;

				al_get_listenerf(AL_GAIN, &gain);
				al_get_listenerfv(AL_POSITION, (ALfloat *)position.value());
				al_get_listenerfv(AL_VELOCITY, (ALfloat *)velocity.value());
				Vector<6,float> orientation;
				al_get_listenerfv(AL_ORIENTATION, (ALfloat *)orientation.value());

				std::cout << "Listener State: " << gain << " " << position << " " << velocity << " " << orientation << std::endl;
				*/

				AudioError::reset();
				alSourcePlay(_source_id);
				AudioError::check("Source Playback");
			}

			void Source::pause ()
			{
				AudioError::reset();
				alSourcePause(_source_id);
				AudioError::check("Source Pause");
			}

			void Source::stop ()
			{
				AudioError::reset();
				alSourceStop(_source_id);
				AudioError::check("Source Stop");
			}

			bool Source::is_playing () const
			{
				ALenum state;

				alGetSourcei(_source_id, AL_SOURCE_STATE, &state);

				return state == AL_PLAYING;
			}

			Ref<Mixer> Mixer::shared_mixer ()
			{
				static Ref<Mixer> global_mixer;

				if (!global_mixer) {
					global_mixer = new Mixer();
				}

				return global_mixer;
			}

			ALCdevice * _default_audio_device () {
				static ALCdevice * device = NULL;

				if (!device) {
					const ALCchar * device_name = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
					device = alcOpenDevice(device_name);
				}

				return device;
			}

			Mixer::Mixer ()
			{
				const ALCchar * device_name = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

				AudioError::reset();
				_audio_device = _default_audio_device();
				_audio_context = alcCreateContext(_audio_device, NULL);
				AudioError::check("Initializing Audio Context");

				bool result = alcMakeContextCurrent(_audio_context);

				LogBuffer buffer;
				buffer << "OpenAL Context Initialized..." << std::endl;
				buffer << "OpenAL Vendor: " << alGetString(AL_VENDOR) << " " << alGetString(AL_VERSION) << std::endl;
				buffer << "OpenAL Device: '" << device_name << "'" << std::endl;
				logger()->log(LOG_INFO, buffer);

				//al_distance_model(AL_LINEAR_DISTANCE);
				set_listener_position(Vec3(ZERO));
				set_listener_velocity(Vec3(ZERO));
				set_listener_orientation(Vec3(0.0, 0.0, -1.0), Vec3(0.0, 1.0, 0.0));

				DREAM_ASSERT(result && "Failed to initialize audio hardware!?");
			}

			Mixer::~Mixer ()
			{
				if (_audio_context)
					alcDestroyContext(_audio_context);

				//if (_audio_device)
				//	alc_close_device(_audio_device);
			}

			void Mixer::suspend_processing ()
			{
				alcSuspendContext(_audio_context);
			}

			void Mixer::resume_processing ()
			{
				alcProcessContext(_audio_context);
			}

			void Mixer::set_listener_position (const Vec3 & position)
			{
				alListenerfv(AL_POSITION, position.value());
			}

			void Mixer::set_listener_velocity (const Vec3 & velocity)
			{
				alListenerfv(AL_VELOCITY, velocity.value());
			}

			Ref<Source> Mixer::create_source ()
			{
				return new Source;
			}

			void Mixer::set_listener_orientation (const Vec3 & looking_at, const Vec3 & up)
			{
				Vector<6, float> o;

				o.set(looking_at.value(), 3);
				o.set(up.value(), 3, 3);

				alListenerfv(AL_ORIENTATION, o.value());
			}
		}
	}
}
