//
//  Client/Audio/Stream.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/10/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

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
			
			ALenum bytesPerSample (ALenum format);

			class Stream;
			
			class IStreamDelegate {
				public:
					virtual ~IStreamDelegate();
					
					virtual void streamWillPlay(PTR(Stream) stream);
					virtual void streamDidPause(PTR(Stream) stream);
					virtual void streamDidStop(PTR(Stream) stream);
					
					virtual void streamDidQueueBuffer(PTR(Stream) stream, ALenum format, const ALvoid * data, ALsizei size);
			};
			
			class Stream : public Object, implements IStreamable
			{
			protected:
				PTR(IStreamDelegate) m_delegate;
				
				REF(Source) m_source;
				ALenum m_format;
				ALsizei m_frequency;
				
				std::vector<ALuint> m_buffers;
								
				REF(TimerSource) m_timer;
				REF(Fader) m_fader;
				
				void bufferCallback ();
				void startBufferCallbacks (PTR(Events::Loop) loop);
				void stopBufferCallbacks ();
				
				// We provide additional callbacks for processing data
				virtual void bufferData(PTR(Source) source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
				
			public:
				Stream (PTR(Source) source, ALenum format, ALsizei frequency);
				virtual ~Stream ();
				
				REF(Source) source () { return m_source; }
				
				/// Start the audio stream playing.
				/// If the audio stream has been paused, it may resume from that point.
				/// If the stream is real time, it may resume playing audio at the current time.
				virtual void play (PTR(Events::Loop) loop);
				
				/// Pause the audio stream. The audio stream may be resumed at this point exactly.
				virtual void pause ();
				
				/// Stop the audio stream and discard any buffers that have been queued.
				virtual void stop ();
				
				/// Play the stream and linearly interpolate the current gain to the given.
				/// When the fade is complete, the loop is stopped.
				void fadeOut (PTR(Events::Loop) loop, TimeT duration = 0.1, RealT gain = 0.0);
				
				/// Play the stream and linearly interpolate the current gain to the given gain.
				void fadeIn (PTR(Events::Loop) loop, TimeT duration = 0.1, RealT gain = 1.0);
				
				TimeT secondsPerBuffer () const;
				
				/// Delegate is not retained.
				void setDelegate(PTR(IStreamDelegate) delegate);
				PTR(IStreamDelegate) delegate ();
			};
		}
	}
}

#endif
