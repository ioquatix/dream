/*
 *  Client/OggStream.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_AUDIO_STREAM_H
#define _DREAM_CLIENT_AUDIO_STREAM_H

#include "Stream.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			using namespace Dream::Resources;

			class OggStream
			{
				EXPOSE_CLASS(Sound)
				
				class Class : public Object::Class, IMPLEMENTS(Loadable::Class)
				{
					EXPOSE_CLASSTYPE
					
					virtual void registerLoaderTypes (REF(ILoader) loader);
					virtual REF(Object) initFromData (const REF(Data) data, const ILoader * loader);
				};
				
			protected:
				
				
			public:
				Stream (ALenum format, ALsizei frequency, const Buffer *);
				virtual ~Stream ();
			};
		}
	}
}

#endif
