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

			class Sound : public Object {
			public:
				class Loader : implements ILoadable {
				public:
					virtual void register_loader_types (ILoader * loader);
					virtual Ref<Object> load_from_data (const Ptr<IData> data, const ILoader * loader);
				};

			protected:
				ALuint _buffer_id;
				ALenum _format;
				ALfloat _frequency;

			public:
				Sound (ALenum format, ALsizei frequency, const Buffer *);
				virtual ~Sound ();

				friend class Source;
			};
		}
	}
}

#endif
