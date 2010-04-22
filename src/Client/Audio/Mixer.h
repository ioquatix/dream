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

#include "../../Numerics/Numerics.h"
#include "../../Numerics/Vector.h"
#include "../../Numerics/Quaternion.h"

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
			
			class Sound;
			
			class Source : public Object
			{
				EXPOSE_CLASS(Source)
				
				class Class : public Object::Class
				{
					EXPOSE_CLASSTYPE
				};
			protected:
				ALuint m_sourceID;
				
			public:
				Source ();
				virtual ~Source ();
				
				void setPitch (float pitch);
				void setGain (float gain);
				void setPosition (const Vec3 &);
				void setVelocity (const Vec3 &);
				
				void setReferenceDistance (float dist);
				
				void setSound (ALuint bufferID);
				void setSound (REF(Sound));
												
				void setLooping (bool);
				
				void play () const;
			};
			
			class Mixer : public Object
			{
				EXPOSE_CLASS(Mixer)
				
				class Class : public Object::Class
				{
					EXPOSE_CLASSTYPE
					
					virtual REF(Mixer) init ();
				};
				
			protected:
				ALCdevice * m_audioDevice;
				ALCcontext * m_audioContext;
				
				Mixer ();
				
			public:
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
