/*
 *  Client/Audio/Mixer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 2/02/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_AUDIO_MIXER_H
#define _DREAM_CLIENT_AUDIO_MIXER_H

#include "../../Core/System.h"
#include "../../Numerics/Numerics.h"
#include "../../Numerics/Vector.h"
#include "../../Numerics/Quaternion.h"

#include "../../Events/Fader.h"

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			using namespace Dream::Core;
			using namespace Dream::Numerics;
			using namespace Dream::Events;
			
			class Sound;
			class IStreamable;
			
			class AudioError {
			protected:
				ALint m_errorNumber;
				StringT m_message;
			
			public:
				AudioError(ErrorNumberT errorNumber, StringT errorDescription, StringT errorTarget);
				
				StringT what () const;
				
				static void check (StringT what);
				static void reset ();
			};
			
			class Source : public Object
			{
			protected:
				ALuint m_sourceID;
				REF(Sound) m_sound;
				
			public:
				Source ();
				virtual ~Source ();
				
				void setParameter(ALenum parameter, float value);
				
				void setPitch (float pitch);
				void setGain (float gain);
				void setPosition (const Vec3 &);
				void setVelocity (const Vec3 &);
				
				float pitch ();
				float gain ();
				Vec3 position ();
				Vec3 velocity ();
				
				ALint sampleOffset ();
				TimeT timeOffset ();
				IndexT byteOffset ();
				
				void setLocal ();
				
				void setReferenceDistance (float dist);
				
				void setSound (ALuint bufferID);
				void setSound (PTR(Sound) sound);
				
				// Streaming buffers
				void queueBuffers (ALuint * buffers, std::size_t count);
				void unqueueBuffers (ALuint * buffers, std::size_t count);
				
				bool streamBuffers (IStreamable * stream);
				
				ALint processedBufferCount ();
				ALint queuedBufferCount ();
				
				void setLooping (bool);
				
				void play ();
				void pause ();
				void stop ();
				
				bool isPlaying () const;
			};
			
			template <typename ValueT>
			class LinearKnob : implements IKnob
			{
				protected:
					REF(Source) m_source;
					ALenum m_parameter;
					ValueT m_begin, m_end;
								
				public:
					LinearKnob (PTR(Source) source, ALenum parameter, ValueT begin, ValueT end)
						: m_source(source), m_parameter(parameter), m_begin(begin), m_end(end)
					{
					
					}
					
					virtual ~LinearKnob ()
					{
					
					}
					
					virtual void update (RealT time)
					{
						ValueT value = linearInterpolate(time, m_begin, m_end);
						m_source->setParameter(m_parameter, value);
					}
			};
			
			class IStreamable : implements IObject
			{
			public:
				virtual ~IStreamable ();
			
				// Return false if there are no more buffers.
				virtual bool loadNextBuffer (PTR(Source) source, ALuint buffer) abstract;
				
				virtual void bufferData(PTR(Source) source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
			};
			
			class Mixer : public Object
			{
			protected:
				ALCdevice * m_audioDevice;
				ALCcontext * m_audioContext;
				
			public:
				static REF(Mixer) sharedMixer ();
				
				Mixer ();
				virtual ~Mixer ();
				
				void suspendProcessing ();
				void resumeProcessing ();
				
				REF(Source) createSource ();
				
				void setListenerPosition (const Vec3 &);
				void setListenerVelocity (const Vec3 &);
				void setListenerOrientation (const Vec3 & lookingAt, const Vec3 & up);
			};
			
		}
	}
}

#endif
