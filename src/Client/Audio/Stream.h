/*
 *  Client/Audio/Stream.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_AUDIO_STREAM_H
#define _DREAM_CLIENT_AUDIO_STREAM_H

#include "Sound.h"
#include "../../Events/Source.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			using namespace Dream::Resources;
			
			class Stream : IMPLEMENTS(Events::ITimerSource)
			{
				EXPOSE_CLASS(Sound)
				
				class Class : public Object::Class, IMPLEMENTS(Loadable::Class)
				{
					EXPOSE_CLASSTYPE
					
					virtual void registerLoaderTypes (REF(ILoader) loader);
					virtual REF(Object) initFromData (const REF(Data) data, const ILoader * loader);
				};
				
			protected:
				ALuint m_buffers[2];
				unsigned m_frontBuffer;
				
				
				
			public:
				Stream (ALenum format, ALsizei frequency, const Buffer *);
				virtual ~Stream ();
				
				/* For scheduling in the run-loop */
				virtual bool repeats () const;
				virtual TimeT nextTimeout (const TimeT & lastTimeout, const TimeT & currentTime) const;
			};
		}
	}
}

#endif
