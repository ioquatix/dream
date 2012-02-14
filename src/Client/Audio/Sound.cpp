//
//  Client/Audio/Sound.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 2/02/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "Sound.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			void Sound::Loader::register_loader_types (ILoader * loader)
			{
				loader->set_loader_for_extension(this, "wav");
			}
			
			ALenum data_format (ALint channel_count, ALint bits_per_sample)
			{
				if (channel_count == 1 && bits_per_sample == 8)
					return AL_FORMAT_MONO8;
				else if (channel_count == 1 && bits_per_sample == 16)
					return AL_FORMAT_MONO16;
				else if (channel_count == 2 && bits_per_sample == 8)
					return AL_FORMAT_STEREO8;
				else if (channel_count == 2 && bits_per_sample == 16)
					return AL_FORMAT_STEREO16;
				else
					return 0;
			}
			
			typedef Ref<Sound>(*DecoderT)(const Buffer *, ALint, ALint, ALfloat);
			
			Ref<Sound> decode_linear_codec (const Buffer * buf, ALint channel_count, ALint bits_per_sample, ALfloat sample_frequency)
			{
				return new Sound(data_format(channel_count, bits_per_sample), sample_frequency, buf);
			}
			
			Ref<Sound> decode_pcm8SCodec (const Buffer * buf, ALint channel_count, ALint bits_per_sample, ALfloat sample_frequency)
			{
				DynamicBuffer copy;
				copy.assign(*buf);
				
				int8_t *d = (int8_t *)copy.begin();
				IndexT i;
				
				for (i = 0; i < copy.size(); i++)
					d[i] += (int8_t) 128;
				
				return new Sound(data_format(channel_count, bits_per_sample), sample_frequency, &copy);
			}
			
			const Endian PCM16Endian = LITTLE;
			Ref<Sound> decode_pcm16Codec (const Buffer * buf, ALint channel_count, ALint bits_per_sample, ALfloat sample_frequency)
			{
				if (host_endian() == PCM16Endian) {
					return new Sound(data_format(channel_count, bits_per_sample), sample_frequency, buf);
				} else {
					DynamicBuffer copy(buf->size(), true);
					
					for (unsigned i = 0; i < buf->size(); i += 2)
					{
						int16_t sample;
						buf->read(i, sample, PCM16Endian);
						copy.append(sample);
					}
					
					return new Sound(data_format(channel_count, bits_per_sample), sample_frequency, &copy);
				}
			}
						
			Ref<Sound> load_wave_data (const Ptr<IData> data)
			{				
				DecoderT decoder = NULL;
				Shared<Buffer> buffer = data->buffer();
				
				IndexT i = 4;
				
				uint32_t chunk_length;
				int32_t magic;
				i += buffer->read(i, chunk_length, LITTLE);
				i += buffer->read(i, magic, BIG);
				
				if (magic != 'WAVE')
					throw LoadError("Could not load WAV data");
				
				bool found_header;
				uint16_t audio_format, channel_count, block_align, bits_per_sample;
				uint32_t sample_frequency, byte_rate;
				
				while (i < buffer->size())
				{
					i += buffer->read(i, magic, BIG);
					i += buffer->read(i, chunk_length, LITTLE);
					
					if (magic == 'fmt ')
					{
						// Decode header
						found_header = true;
						
						i += buffer->read(i, audio_format, LITTLE);
						i += buffer->read(i, channel_count, LITTLE);
						i += buffer->read(i, sample_frequency, LITTLE);
						i += buffer->read(i, byte_rate, LITTLE);
						i += buffer->read(i, block_align, LITTLE);
						i += buffer->read(i, bits_per_sample, LITTLE);
												
						i += chunk_length - 16;
						
						if (audio_format == 1) {
							if (bits_per_sample == 8)
								// Copy samples verbatim.
								decoder = decode_linear_codec;
							else
								// Use PCM16 decoder - will pass through if endian doesn't need to be converted.
								decoder = decode_pcm16Codec;
						} else if (audio_format == 7) {
							//bits_per_sample *= 2;
							//decoder = decode_ulaw_codec;
							throw LoadError("Unsupported WAV encoding (ULaw)");
						} else {
							throw LoadError("Unsupported WAV encoding (Unknown)");
						}
					} else if (magic == 'data') {						
						if (!found_header)
							throw LoadError("Corrupt or truncated data");
						
						StaticBuffer sample_data(&(*buffer)[i], chunk_length);
						
						DREAM_ASSERT(decoder != NULL);
						return decoder(&sample_data, channel_count, bits_per_sample, sample_frequency);
					} else {
						// Unknown header
						i += chunk_length;
					}
				}
				
				throw LoadError("Corrupt or truncated data");
			}
						
			Ref<Object> Sound::Loader::load_from_data (const Ptr<IData> data, const ILoader * loader)
			{
				Shared<Buffer> buffer = data->buffer();
				
				Mimetype mt = buffer->mimetype();
				
				if (mt == AUDIO_XWAV) {
					return load_wave_data(data);
				} else {
					throw LoadError("Could not load audio data");
				}
			}
			
			Sound::Sound (ALenum format, ALsizei frequency, const Buffer * samples) : _format(format), _frequency(frequency)
			{
				alGenBuffers(1, &_buffer_id);
			    alBufferData(_buffer_id, _format, samples->begin(), samples->size(), _frequency);
			}
			
			Sound::~Sound ()
			{
				alDeleteBuffers(1, &_buffer_id);
			}
		}
	}
}
