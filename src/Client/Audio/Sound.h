//
//  Client/Audio/Sound.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/02/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_AUDIO_SOUND_H
#define _DREAM_CLIENT_AUDIO_SOUND_H

#include "Mixer.h"

#include "../../Resources/Loader.h"
#include "../../Core/Buffer.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			using namespace Dream::Resources;
			
			class Sound : public Object
			{
				public:
					class Loader : implements ILoadable
					{
					public:
						virtual void registerLoaderTypes (ILoader * loader);
						virtual REF(Object) loadFromData (const PTR(IData) data, const ILoader * loader);
					};
					
				protected:
					ALuint m_bufferID;
					ALenum m_format;
					ALfloat m_frequency;
					
				public:
					Sound (ALenum format, ALsizei frequency, const Buffer *);
					virtual ~Sound ();
									
					friend class Source;
			};
		}
	}
}

#endif
