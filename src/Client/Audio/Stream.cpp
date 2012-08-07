//
//  Client/Audio/Stream.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/10/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Stream.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			ALenum bytes_per_sample (ALenum format)
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

// MARK: -

			IStreamDelegate::~IStreamDelegate ()
			{
			}

			void IStreamDelegate::stream_will_play (Ptr<Stream> stream)
			{
			}

			void IStreamDelegate::stream_did_pause (Ptr<Stream> stream)
			{
			}

			void IStreamDelegate::stream_did_stop (Ptr<Stream> stream)
			{
			}

			void IStreamDelegate::stream_did_queue_buffer (Ptr<Stream> stream, ALenum format, const ALvoid * data, ALsizei size)
			{
			}

// MARK: -

			Stream::Stream (Ptr<Source> source, ALenum format, ALsizei frequency) : _source(source), _format(format), _frequency(frequency)
			{
				_buffers.resize(BufferCount);
				alGenBuffers(_buffers.size(), &_buffers[0]);
			}

			Stream::~Stream () {
				alDeleteBuffers(_buffers.size(), &_buffers[0]);

				stop_buffer_callbacks();
			}

			void Stream::buffer_data(Ptr<Source> source, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
			{
				if (_delegate)
					_delegate->stream_did_queue_buffer(this, format, data, size);

				IStreamable::buffer_data(source, buffer, format, data, size, freq);
			}

			void Stream::buffer_callback ()
			{
				_source->stream_buffers(this);
			}

			void Stream::start_buffer_callbacks (Ptr<Events::Loop> loop)
			{
				stop_buffer_callbacks();

				_timer = new TimerSource(std::bind(&Stream::buffer_callback, this), seconds_per_buffer(), true, true);

				loop->schedule_timer(_timer);
			}

			void Stream::stop_buffer_callbacks ()
			{
				if (_timer) {
					_timer->cancel();
					_timer = NULL;
				}
			}

			void Stream::play (Ptr<Events::Loop> loop)
			{
				if (_fader) _fader->cancel();

				if (!_source->is_playing()) {
					if (_delegate)
						_delegate->stream_will_play(this);

					AudioError::reset();

					// If buffers are currently being processed,
					// queued + processed = total buffers
					ALint processed = _source->processed_buffer_count();
					std::vector<ALuint> free_buffers;

					if (processed > 0) {
						free_buffers.resize(processed);
						_source->unqueue_buffers(&free_buffers[0], processed);
					} else {
						// If there are no processed buffers and no queued buffers,
						// that means that the buffers have not been loaded yet.
						if (_source->queued_buffer_count() == 0)
							free_buffers = _buffers;
					}

					AudioError::check("Checking Buffers");

					// Setup the initial buffers
					for (std::size_t i = 0; i < free_buffers.size(); i++) {
						load_next_buffer(_source, free_buffers[i]);
					}

					AudioError::check("Loading Buffers");

					_source->queue_buffers(&free_buffers[0], free_buffers.size());

					start_buffer_callbacks(loop);

					_source->play();
				}
			}

			void Stream::pause ()
			{
				stop_buffer_callbacks();
				_source->pause();

				if (_delegate)
					_delegate->stream_did_pause(this);
			}

			void Stream::stop ()
			{
				stop_buffer_callbacks();
				_source->stop();

				// Remove any queued buffers.
				_source->set_sound(0);

				if (_delegate)
					_delegate->stream_did_stop(this);
			}

			void Stream::fade_out (Ptr<Events::Loop> loop, TimeT duration, RealT gain)
			{
				if (_fader)
					_fader->cancel();

				Shared<IKnob> decrease_gain = new LinearKnob<float>(_source, AL_GAIN, _source->gain(), gain);
				_fader = new Fader(decrease_gain, 100, duration / 100);

				_fader->set_finish_callback(std::bind(&Stream::pause, this));

				loop->schedule_timer(_fader);
			}

			void Stream::fade_in (Ptr<Events::Loop> loop, TimeT duration, RealT gain)
			{
				// This will cancel any existing fader
				play(loop);

				Shared<IKnob> increase_gain = new LinearKnob<float>(_source, AL_GAIN, _source->gain(), gain);
				_fader = new Fader(increase_gain, 100, duration / 100);

				loop->schedule_timer(_fader);
			}

			TimeT Stream::seconds_per_buffer () const {
				// Frequency is the number of samples per second.
				TimeT bytes_per_second = TimeT(_frequency) * bytes_per_sample(_format);
				return TimeT(ChunkSize) / bytes_per_second;
			}

			void Stream::set_delegate(Ptr<IStreamDelegate> delegate)
			{
				_delegate = delegate;
			}

			Ptr<IStreamDelegate> Stream::delegate ()
			{
				return _delegate;
			}
		}
	}
}
