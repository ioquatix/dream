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
#include "../../Events/Loop.h"
#include <set>

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			using namespace Dream::Resources;
			using namespace Dream::Events;
			
			const std::size_t ChunkSize = 1024 * 4 * 32;
			const unsigned BufferCount = 3;
			
			class Stream : public Object, implements IStreamable
			{
			protected:
				REF(Source) m_source;
				ALenum m_format;
				ALsizei m_frequency;
				
				std::vector<ALuint> m_buffers;
								
				REF(TimerSource) m_timer;
				REF(Fader) m_fader;
				
				void bufferCallback ();
				void startBufferCallbacks (PTR(Events::Loop) loop);
				void stopBufferCallbacks ();
				
			public:
				Stream (PTR(Source) source, ALenum format, ALsizei frequency);
				virtual ~Stream ();
				
				REF(Source) source () { return m_source; }
				
				virtual void play (PTR(Events::Loop) loop);
				virtual void pause ();
				virtual void stop ();
				
				void fadeOut (PTR(Events::Loop) loop, TimeT duration = 0.1);
				void fadeIn (PTR(Events::Loop) loop, TimeT duration = 0.1);
				
				TimeT secondsPerBuffer () const;
			};
		}
	}
}

#endif
