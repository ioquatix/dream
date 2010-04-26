/*
 *  Client/Audio/Mixer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 2/02/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Mixer.h"
#include "Sound.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			IMPLEMENT_CLASS(Source)
			
			Source::Source ()
			{
				alGenSources(1, &m_sourceID);
				setPitch(1.0);
				setGain(1.0);
				setPosition(Vec3(ZERO));
				setVelocity(Vec3(ZERO));
				setReferenceDistance(100);
			}
			
			Source::~Source ()
			{
				alDeleteSources(1, &m_sourceID);
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
			
			void Source::setReferenceDistance (float dist)
			{
				alSourcef(m_sourceID, AL_REFERENCE_DISTANCE, dist);
			}
			
			void Source::setSound (REF(Sound) sound)
			{
				alSourcei(m_sourceID, AL_BUFFER, sound->m_bufferID);
			}
			
			void Source::setLooping (bool mode)
			{
				alSourcei(m_sourceID, AL_LOOPING, mode);
			}
			
			void Source::play () const
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
				
				alSourcePlay(m_sourceID);
			}
			
			IMPLEMENT_CLASS(Mixer)
			
			REF(Mixer) Mixer::Class::init ()
			{
				return new Mixer;
			}
			
			Mixer::Mixer ()
			{
				const ALCchar * deviceName;
				
				deviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
				
				m_audioDevice = alcOpenDevice(NULL);
				m_audioContext = alcCreateContext(m_audioDevice, NULL);
				
				bool result = alcMakeContextCurrent(m_audioContext);
				std::cerr << "OpenAL Context Initialized..." << std::endl;
				std::cerr << "OpenAL Vendor: " << alGetString(AL_VENDOR) << " " << alGetString(AL_VERSION) << std::endl;
				std::cerr << "OpenAL Device: '" << deviceName << "'" << std::endl;
				
				//alDistanceModel(AL_LINEAR_DISTANCE);
				setListenerPosition(Vec3(ZERO));
				setListenerVelocity(Vec3(ZERO));
				setListenerOrientation(Vec3(0.0, 0.0, -1.0), Vec3(0.0, 1.0, 0.0));
				
				ensure(result);
			}
			
			Mixer::~Mixer ()
			{
				if (m_audioContext)
					alcDestroyContext(m_audioContext);
				
				if (m_audioDevice)
					alcCloseDevice(m_audioDevice);
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
