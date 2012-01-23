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
							virtual Ref<Object> load_from_data (const Ptr<IData> data, const ILoader * loader);
					};
				
				protected:
					Ref<IData> _data;
					
					ALenum _format;
					ALsizei _frequency;
					
				public:
					OggResource (const Ptr<IData> data);
					virtual ~OggResource ();
					
					Ref<Stream> create_stream (Ptr<Source> source);
					
					// Current implementation returns NULL. Use create_stream or WAV files.
					Ref<Sound> create_sound (Ptr<Source> source);
					
					//void debug();
					static StringT error_string(int code);
			};
			
		}
	}
}

#endif
