/*
 *  Client/Audio/Sound.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 2/02/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Sound.h"

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{
			void Sound::Loader::registerLoaderTypes (ILoader * loader)
			{
				loader->setLoaderForExtension(this, "wav");
			}
			
			ALenum dataFormat (ALint channelCount, ALint bitsPerSample)
			{
				if (channelCount == 1 && bitsPerSample == 8)
					return AL_FORMAT_MONO8;
				else if (channelCount == 1 && bitsPerSample == 16)
					return AL_FORMAT_MONO16;
				else if (channelCount == 2 && bitsPerSample == 8)
					return AL_FORMAT_STEREO8;
				else if (channelCount == 2 && bitsPerSample == 16)
					return AL_FORMAT_STEREO16;
				else
					return 0;
			}
			
			typedef REF(Sound)(*DecoderT)(const Buffer *, ALint, ALint, ALfloat);
			
			REF(Sound) decodeLinearCodec (const Buffer * buf, ALint channelCount, ALint bitsPerSample, ALfloat sampleFrequency)
			{
				return new Sound(dataFormat(channelCount, bitsPerSample), sampleFrequency, buf);
			}
			
			REF(Sound) decodePCM8SCodec (const Buffer * buf, ALint channelCount, ALint bitsPerSample, ALfloat sampleFrequency)
			{
				DynamicBuffer copy;
				copy.assign(*buf);
				
				int8_t *d = (int8_t *)copy.begin();
				IndexT i;
				
				for (i = 0; i < copy.size(); i++)
					d[i] += (int8_t) 128;
				
				return new Sound(dataFormat(channelCount, bitsPerSample), sampleFrequency, &copy);
			}
			
			const Endian PCM16Endian = LITTLE;
			REF(Sound) decodePCM16Codec (const Buffer * buf, ALint channelCount, ALint bitsPerSample, ALfloat sampleFrequency)
			{
				if (hostEndian() == PCM16Endian) {
					return new Sound(dataFormat(channelCount, bitsPerSample), sampleFrequency, buf);
				} else {
					DynamicBuffer copy(buf->size(), true);
					
					for (unsigned i = 0; i < buf->size(); i += 2)
					{
						int16_t sample;
						buf->read(i, sample, PCM16Endian);
						copy.append(sample);
					}
					
					return new Sound(dataFormat(channelCount, bitsPerSample), sampleFrequency, &copy);
				}
			}
						
			REF(Sound) loadWaveData (const PTR(IData) data)
			{				
				DecoderT decoder = NULL;
				Shared<Buffer> buffer = data->buffer();
				
				IndexT i = 4;
				
				uint32_t chunkLength;
				int32_t magic;
				i += buffer->read(i, chunkLength, LITTLE);
				i += buffer->read(i, magic, BIG);
				
				if (magic != 'WAVE')
					throw LoadError("Could not load WAV data");
				
				bool foundHeader;
				uint16_t audioFormat, channelCount, blockAlign, bitsPerSample;
				uint32_t sampleFrequency, byteRate;
				
				while (i < buffer->size())
				{
					i += buffer->read(i, magic, BIG);
					i += buffer->read(i, chunkLength, LITTLE);
					
					if (magic == 'fmt ')
					{
						// Decode header
						foundHeader = true;
						
						i += buffer->read(i, audioFormat, LITTLE);
						i += buffer->read(i, channelCount, LITTLE);
						i += buffer->read(i, sampleFrequency, LITTLE);
						i += buffer->read(i, byteRate, LITTLE);
						i += buffer->read(i, blockAlign, LITTLE);
						i += buffer->read(i, bitsPerSample, LITTLE);
												
						i += chunkLength - 16;
						
						if (audioFormat == 1) {
							if (bitsPerSample == 8)
								// Copy samples verbatim.
								decoder = decodeLinearCodec;
							else
								// Use PCM16 decoder - will pass through if endian doesn't need to be converted.
								decoder = decodePCM16Codec;
						} else if (audioFormat == 7) {
							//bitsPerSample *= 2;
							//decoder = decodeULawCodec;
							throw LoadError("Unsupported WAV encoding (ULaw)");
						} else {
							throw LoadError("Unsupported WAV encoding (Unknown)");
						}
					} else if (magic == 'data') {						
						if (!foundHeader)
							throw LoadError("Corrupt or truncated data");
						
						StaticBuffer sampleData(&(*buffer)[i], chunkLength);
						
						ensure(decoder != NULL);
						return decoder(&sampleData, channelCount, bitsPerSample, sampleFrequency);
					} else {
						// Unknown header
						i += chunkLength;
					}
				}
				
				throw LoadError("Corrupt or truncated data");
			}
						
			REF(Object) Sound::Loader::loadFromData (const PTR(IData) data, const ILoader * loader)
			{
				Shared<Buffer> buffer = data->buffer();
				
				Mimetype mt = buffer->mimetype();
				
				if (mt == AUDIO_XWAV) {
					return loadWaveData(data);
				} else {
					throw LoadError("Could not load audio data");
				}
			}
			
			Sound::Sound (ALenum format, ALsizei frequency, const Buffer * samples) : m_format(format), m_frequency(frequency)
			{
				alGenBuffers(1, &m_bufferID);
			    alBufferData(m_bufferID, m_format, samples->begin(), samples->size(), m_frequency);
			}
			
			Sound::~Sound ()
			{
				alDeleteBuffers(1, &m_bufferID);
			}
		}
	}
}
