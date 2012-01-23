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
		
#pragma mark -
#pragma mark AudioError

			AudioError::AudioError (ErrorNumberT errorNumber, StringT errorDescription, StringT errorTarget)
			{
				StringStreamT buffer;
				buffer << "Audio Error #" << errorNumber << ": " << errorDescription << "(" << errorTarget << ")";
				m_message = buffer.str();
			}
			
			StringT AudioError::what () const
			{
				return m_message;
			}
			
			void AudioError::check (StringT what)
			{
				ALint error = alGetError();
				
				if (error != AL_NO_ERROR)
				{
					throw AudioError(error, alGetString(error), what);
				}
			}
			
			void AudioError::reset ()
			{
				ALint error = AL_NO_ERROR;
				
				while ((error = alGetError()) != AL_NO_ERROR)
				{
					std::cerr << "Unhandled audio error #" << error << ": " << alGetString(error) << std::endl;
				}
			}

#pragma mark -
#pragma mark Source

			Source::Source ()
			{
				AudioError::reset();
				
				alGenSources(1, &m_sourceID);
				//setPitch(1.0);
				//setGain(1.0);
				//setPosition(Vec3(ZERO));
				//setVelocity(Vec3(ZERO));
				//setReferenceDistance(100);
				
				AudioError::check("Allocating Source");
			}
			
			Source::~Source ()
			{
				AudioError::reset();
				
				std::cerr << "Deleting audio source: " << m_sourceID << std::endl;
				alDeleteSources(1, &m_sourceID);
				
				AudioError::check("Deallocating Source");
			}
			
			void Source::setParameter(ALenum parameter, float value)
			{
				alSourcef(m_sourceID, parameter, value);
			}
			
			void Source::setPitch (float pitch)
			{
				alSourcef(m_sourceID, AL_PITCH, pitch);
			}
			
			void Source::setGain (float gain)
			{
				alSourcef(m_sourceID, AL_GAIN, gain);
			}
			
			void Source::setPosition (const Vec3 & position)
			{
				alSourcefv(m_sourceID, AL_POSITION, position.value());
			}
			
			void Source::setVelocity (const Vec3 & velocity)
			{
				alSourcefv(m_sourceID, AL_VELOCITY, velocity.value());
			}
			
			float Source::pitch ()
			{
				float value;
				
				alGetSourcef(m_sourceID, AL_PITCH, &value);
				
				return value;
			}
			
			float Source::gain ()
			{
				float value;
				
				alGetSourcef(m_sourceID, AL_GAIN, &value);
				
				return value;
			}
			
			Vec3 Source::position ()
			{
				Vec3 value;
				
				alGetSourcefv(m_sourceID, AL_POSITION, (ALfloat *)value.value());
				
				return value;
			}
			
			Vec3 Source::velocity ()
			{
				Vec3 value;
				
				alGetSourcefv(m_sourceID, AL_VELOCITY, (ALfloat *)value.value());
				
				return value;
			}
			
			ALint Source::sampleOffset ()
			{
				ALint offset = 0;
				
				alGetSourcei(m_sourceID, AL_SAMPLE_OFFSET, &offset);
				
				return offset;
			}
			
			TimeT Source::timeOffset ()
			{
				ALfloat offset = 0;
				
				alGetSourcef(m_sourceID, AL_SEC_OFFSET, &offset);
				
				return offset;
			}
			
			IndexT Source::byteOffset ()
			{
				ALint offset = 0;
				
				alGetSourcei(m_sourceID, AL_BYTE_OFFSET, &offset);
				
				return offset;
			}
			
			void Source::setLocal ()
			{
			    alSource3f(m_sourceID, AL_POSITION, 0.0, 0.0, 0.0);
				alSource3f(m_sourceID, AL_VELOCITY, 0.0, 0.0, 0.0);
				alSource3f(m_sourceID, AL_DIRECTION, 0.0, 0.0, 0.0);
				alSourcef(m_sourceID, AL_ROLLOFF_FACTOR, 0.0);
				alSourcei(m_sourceID, AL_SOURCE_RELATIVE, AL_TRUE);
			}
			
			void Source::setReferenceDistance (float dist)
			{
				alSourcef(m_sourceID, AL_REFERENCE_DISTANCE, dist);
			}

			void Source::setSound (ALuint bufferID)
			{
				alSourcei(m_sourceID, AL_BUFFER, bufferID);
			}
			
			void Source::setSound (PTR(Sound) sound)
			{
				m_sound = sound;
				
				alSourcei(m_sourceID, AL_BUFFER, sound->m_bufferID);
			}
			
			void Source::queueBuffers (ALuint * buffers, std::size_t count)
			{
				AudioError::reset();
				alSourceQueueBuffers(m_sourceID, count, buffers);
				AudioError::check("Source Queue Buffers");
			}
			
			void Source::unqueueBuffers (ALuint * buffers, std::size_t count)
			{
				AudioError::reset();
				alSourceUnqueueBuffers(m_sourceID, count, buffers);
				AudioError::check("Source Unqueue Buffers");
			}
			
			ALint Source::processedBufferCount ()
			{
				ALint value = 0;
				
				alGetSourcei(m_sourceID, AL_BUFFERS_PROCESSED, &value);
				
				AudioError::reset();
				
				return value;
			}
			
			ALint Source::queuedBufferCount ()
			{
				ALint value = 0;
				
				alGetSourcei(m_sourceID, AL_BUFFERS_QUEUED, &value);
				
				AudioError::reset();
				
				return value;
			}
			
			IStreamable::~IStreamable ()
			{
			
			}
			
			void IStreamable::bufferData(PTR(Source) source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
			{
				AudioError::reset();
				alBufferData(buffer, format, data, size, freq);
				AudioError::check("Buffering Data");				
			}
			
			bool Source::streamBuffers (IStreamable * stream) {
				ALint processed = processedBufferCount();
				bool complete = true;
				
				// std::cerr << "Processed buffers = " << processed << std::endl;
				
				AudioError::reset();
				
				while (processed--) {
					ALuint buffer;
					unqueueBuffers(&buffer, 1);
					
					bool result = stream->loadNextBuffer(this, buffer);
					
					if (result) {
						queueBuffers(&buffer, 1);
					} else {
						complete = false;
					}
				}
				
				AudioError::check("Streaming Buffers");
				
				return complete;
			}
			
			void Source::setLooping (bool mode)
			{
				alSourcei(m_sourceID, AL_LOOPING, mode);
			}
			
			void Source::play ()
			{
				/*
				float pitch, gain, minGain, maxGain, maxDistance, rolloffFactor, coneOuterGain, coneInnerAngle, coneOuterAngle, referenceDistance;
				alGetSourcef(m_sourceID, AL_PITCH, &pitch);
				alGetSourcef(m_sourceID, AL_GAIN, &gain);
				alGetSourcef(m_sourceID, AL_MIN_GAIN, &minGain);
				alGetSourcef(m_sourceID, AL_MAX_GAIN, &maxGain);
				alGetSourcef(m_sourceID, AL_MAX_DISTANCE, &maxDistance);
				alGetSourcef(m_sourceID, AL_ROLLOFF_FACTOR, &rolloffFactor);
				alGetSourcef(m_sourceID, AL_CONE_OUTER_GAIN, &coneOuterGain);
				alGetSourcef(m_sourceID, AL_CONE_INNER_ANGLE, &coneInnerAngle);
				alGetSourcef(m_sourceID, AL_CONE_OUTER_ANGLE, &coneOuterAngle);
				alGetSourcef(m_sourceID, AL_REFERENCE_DISTANCE, &referenceDistance);
				
				std::cout << " ==== " << this << " ==== " << std::endl;
				std::cout << "Audio properties: " << pitch << " " << gain << " " << minGain << " " << maxGain << " " << maxDistance << " " << rolloffFactor
				<< " " << coneOuterGain << " " << coneInnerAngle << " " << coneOuterAngle << " " << referenceDistance << std::endl;
				
				ALint sourceRelative, buffer, sourceState, buffersQueued, buffersProcessed;
				alGetSourcei(m_sourceID, AL_SOURCE_RELATIVE, &sourceRelative);
				alGetSourcei(m_sourceID, AL_BUFFER, &buffer);
				alGetSourcei(m_sourceID, AL_SOURCE_STATE, &sourceState);
				alGetSourcei(m_sourceID, AL_BUFFERS_QUEUED, &buffersQueued);
				alGetSourcei(m_sourceID, AL_BUFFERS_PROCESSED, &buffersProcessed);
				
				std::cout << "Source State: " << sourceRelative << " " << buffer << " " << sourceState << " " << buffersQueued 
				<< " " << buffersProcessed << std::endl;
								
				Vec3 position, velocity, direction;
				alGetSourcefv(m_sourceID, AL_POSITION, (ALfloat *)position.value());
				alGetSourcefv(m_sourceID, AL_VELOCITY, (ALfloat *)velocity.value());
				alGetSourcefv(m_sourceID, AL_DIRECTION, (ALfloat *)direction.value());
				
				std::cout << "Pos: " << position << " Vel: " << velocity << " Dir: " << direction << std::endl;
				
				alGetListenerf(AL_GAIN, &gain);
				alGetListenerfv(AL_POSITION, (ALfloat *)position.value());
				alGetListenerfv(AL_VELOCITY, (ALfloat *)velocity.value());
				Vector<6,float> orientation;
				alGetListenerfv(AL_ORIENTATION, (ALfloat *)orientation.value());
				
				std::cout << "Listener State: " << gain << " " << position << " " << velocity << " " << orientation << std::endl;
				*/
				
				AudioError::reset();
				alSourcePlay(m_sourceID);
				AudioError::check("Source Playback");
			}
			
			void Source::pause ()
			{
				AudioError::reset();
				alSourcePause(m_sourceID);
				AudioError::check("Source Pause");
			}
			
			void Source::stop ()
			{
				AudioError::reset();
				alSourceStop(m_sourceID);
				AudioError::check("Source Stop");
			}
			
			bool Source::isPlaying () const
			{
				ALenum state;
    
				alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
    
				return state == AL_PLAYING;
			}
			
			REF(Mixer) Mixer::sharedMixer ()
			{
				static REF(Mixer) g_mixer;
				
				if (!g_mixer) {
					g_mixer = new Mixer();
				}
				
				return g_mixer;
			}
			
			ALCdevice * _defaultAudioDevice () {
				static ALCdevice * device = NULL;
				
				if (!device) {
					const ALCchar * deviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
					device = alcOpenDevice(deviceName);
				}
				
				return device;
			}
			
			Mixer::Mixer ()
			{
				const ALCchar * deviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
				
				AudioError::reset();
				m_audioDevice = _defaultAudioDevice();
				m_audioContext = alcCreateContext(m_audioDevice, NULL);
				AudioError::check("Initializing Audio Context");
				
				bool result = alcMakeContextCurrent(m_audioContext);
				std::cerr << "OpenAL Context Initialized..." << std::endl;
				std::cerr << "OpenAL Vendor: " << alGetString(AL_VENDOR) << " " << alGetString(AL_VERSION) << std::endl;
				std::cerr << "OpenAL Device: '" << deviceName << "'" << std::endl;
				
				//alDistanceModel(AL_LINEAR_DISTANCE);
				setListenerPosition(Vec3(ZERO));
				setListenerVelocity(Vec3(ZERO));
				setListenerOrientation(Vec3(0.0, 0.0, -1.0), Vec3(0.0, 1.0, 0.0));
				
				ensure(result && "Failed to initialize audio hardware!?");
			}
			
			Mixer::~Mixer ()
			{
				if (m_audioContext)
					alcDestroyContext(m_audioContext);
				
				//if (m_audioDevice)
				//	alcCloseDevice(m_audioDevice);
			}
			
			void Mixer::suspendProcessing ()
			{
				alcSuspendContext(m_audioContext);
			}
			
			void Mixer::resumeProcessing ()
			{
				alcProcessContext(m_audioContext);
			}
			
			void Mixer::setListenerPosition (const Vec3 & position)
			{
				alListenerfv(AL_POSITION, position.value());
			}
			
			void Mixer::setListenerVelocity (const Vec3 & velocity)
			{
				alListenerfv(AL_VELOCITY, velocity.value());				
			}
			
			REF(Source) Mixer::createSource ()
			{
				return new Source;
			}
			
			void Mixer::setListenerOrientation (const Vec3 & lookingAt, const Vec3 & up)
			{
				Vector<6, float> o;
				
				o.set(lookingAt.value(), 3);
				o.set(up.value(), 3, 3);
								
				alListenerfv(AL_ORIENTATION, o.value());
			}
			
		}
	}
}
