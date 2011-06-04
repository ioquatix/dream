/*
 *  Client/Audio/OggResource.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

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
							virtual void registerLoaderTypes (ILoader * loader);
							virtual REF(Object) loadFromData (const PTR(IData) data, const ILoader * loader);
					};
				
				protected:
					REF(IData) m_data;
					
					ALenum m_format;
					ALsizei m_frequency;
					
				public:
					OggResource (const PTR(IData) data);
					virtual ~OggResource ();
					
					REF(Stream) createStream (PTR(Source) source);
					REF(Sound) createSound (PTR(Source) source);
					
					//void debug();
					static StringT errorString(int code);
			};
			
		}
	}
}

#endif
