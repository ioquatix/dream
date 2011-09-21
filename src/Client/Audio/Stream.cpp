/*
 *  Client/Audio/Stream.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Stream.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			ALenum bytesPerSample (ALenum format)
			{
				switch (format) {
					case AL_FORMAT_MONO8:
						return 1;
					case AL_FORMAT_MONO16:
						return 2;
					case AL_FORMAT_STEREO8:
						return 2;
					case AL_FORMAT_STEREO16:
						return 4;
					default:
						break;
				}
				
				return 1;
			}
			
#pragma mark -

			IStreamDelegate::~IStreamDelegate ()
			{
			
			}

			void IStreamDelegate::streamWillPlay (PTR(Stream) stream)
			{
			
			}
			
			void IStreamDelegate::streamDidPause (PTR(Stream) stream)
			{
			
			}
			
			void IStreamDelegate::streamDidStop (PTR(Stream) stream)
			{
			
			}
			
			void IStreamDelegate::streamDidQueueBuffer (PTR(Stream) stream, ALenum format, const ALvoid * data, ALsizei size)
			{
			
			}
			
#pragma mark -
			
			Stream::Stream (PTR(Source) source, ALenum format, ALsizei frequency) 
				: m_source(source), m_format(format), m_frequency(frequency)
			{
				m_buffers.resize(BufferCount);
				alGenBuffers(m_buffers.size(), &m_buffers[0]);
			}
			
			Stream::~Stream () {
				alDeleteBuffers(m_buffers.size(), &m_buffers[0]);
			}
			
			void Stream::bufferData(PTR(Source) source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
			{
				if (m_delegate)
					m_delegate->streamDidQueueBuffer(this, format, data, size);
				
				IStreamable::bufferData(source, buffer, format, data, size, freq);
			}
			
			void Stream::bufferCallback ()
			{
				m_source->streamBuffers(this);
			}
			
			void Stream::startBufferCallbacks (PTR(Events::Loop) loop)
			{
				stopBufferCallbacks();
				
				m_timer = new TimerSource(boost::bind(&Stream::bufferCallback, this), secondsPerBuffer(), true, true);
				
				loop->scheduleTimer(m_timer);
			}
			
			void Stream::stopBufferCallbacks ()
			{
				if (m_timer) {
					m_timer->cancel();
					m_timer = NULL;
				}
			}
			
			void Stream::play (PTR(Events::Loop) loop)
			{
				if (m_fader) m_fader->cancel();
			
				if (!m_source->isPlaying()) {
					if (m_delegate)
						m_delegate->streamWillPlay(this);
				
					AudioError::reset();
					
					// If buffers are currently being processed,
					// queued + processed = total buffers
					ALint processed = m_source->processedBufferCount();
					std::vector<ALuint> freeBuffers;
					
					if (processed > 0) {
						freeBuffers.resize(processed);
						m_source->unqueueBuffers(&freeBuffers[0], processed);
					} else {
						// If there are no processed buffers and no queued buffers,
						// that means that the buffers have not been loaded yet.
						if (m_source->queuedBufferCount() == 0)
							freeBuffers = m_buffers;
					}
					
					AudioError::check("Checking Buffers");
					
					// Setup the initial buffers
					for (std::size_t i = 0; i < freeBuffers.size(); i++) {
						loadNextBuffer(m_source, freeBuffers[i]);
					}
					
					AudioError::check("Loading Buffers");
					
					m_source->queueBuffers(&freeBuffers[0], freeBuffers.size());
					
					startBufferCallbacks(loop);
					
					m_source->play();
				}
			}
			
			void Stream::pause ()
			{
				stopBufferCallbacks();
				m_source->pause();
				
				if (m_delegate)
					m_delegate->streamDidPause(this);
			}
			
			void Stream::stop ()
			{
				stopBufferCallbacks();
				m_source->stop();
				
				// Remove any queued buffers.
				m_source->setSound(0);
				
				if (m_delegate)
					m_delegate->streamDidStop(this);
			}
			
			void Stream::fadeOut (PTR(Events::Loop) loop, TimeT duration, RealT gain)
			{
				if (m_fader)
					m_fader->cancel();
				
				Shared<IKnob> decreaseGain = new LinearKnob<float>(m_source, AL_GAIN, m_source->gain(), gain);
				m_fader = new Fader(decreaseGain, 100, duration / 100);
				
				m_fader->setFinishCallback(boost::bind(&Stream::pause, this));
				
				loop->scheduleTimer(m_fader);
			}
			
			void Stream::fadeIn (PTR(Events::Loop) loop, TimeT duration, RealT gain)
			{
				// This will cancel any existing fader
				play(loop);
				
				Shared<IKnob> increaseGain = new LinearKnob<float>(m_source, AL_GAIN, m_source->gain(), gain);
				m_fader = new Fader(increaseGain, 100, duration / 100);
				
				loop->scheduleTimer(m_fader);
			}
			
			TimeT Stream::secondsPerBuffer () const {
				// Frequency is the number of samples per second.
				TimeT bytesPerSecond = TimeT(m_frequency) * bytesPerSample(m_format);
				return TimeT(ChunkSize) / bytesPerSecond;
			}
			
			void Stream::setDelegate(PTR(IStreamDelegate) delegate)
			{
				m_delegate = delegate;
			}
			
			PTR(IStreamDelegate) Stream::delegate ()
			{
				return m_delegate;
			}
		}
	}
}
