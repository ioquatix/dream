//
//  Client/Audio/OggResource.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/10/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CLIENT_AUDIO_OGGRESOURCE_H
#define _DREAM_CLIENT_AUDIO_OGGRESOURCE_H

#include "Stream.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			class OggReader;
			
			class OggResource : public Object
			{
				public:
					class Loader : public Object, implements ILoadable
					{
						public:
							virtual void register_loader_types (ILoader * loader);
							virtual REF(Object) load_from_data (const PTR(IData) data, const ILoader * loader);
					};
				
				protected:
					REF(IData) _data;
					
					ALenum _format;
					ALsizei _frequency;
					
				public:
					OggResource (const PTR(IData) data);
					virtual ~OggResource ();
					
					REF(Stream) create_stream (PTR(Source) source);
					
					// Current implementation returns NULL. Use create_stream or WAV files.
					REF(Sound) create_sound (PTR(Source) source);
					
					//void debug();
					static StringT error_string(int code);
			};
			
		}
	}
}

#endif
