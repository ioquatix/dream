/*
 *  Client/Audio/OggResource.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 7/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "OggResource.h"

// We don't need the static callbacks, since we define our own callbacks to hook into IData::inputStream().
#define OV_EXCLUDE_STATIC_CALLBACKS

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace Dream
{
	namespace Client
	{
		namespace Audio
		{

#pragma mark -
#pragma mark OggReader

			class OggReader : public boost::noncopyable
			{
				protected:
					Shared<std::istream> m_inputStream;
					OggVorbis_File m_file;
					
				public:
					OggReader (Shared<std::istream> inputStream);
					virtual ~OggReader ();
					
					OggVorbis_File * file ()
					{
						return &m_file;
					}
					
				protected:
					static std::size_t readCallback (void * buffer, std::size_t size, std::size_t count, void * source) {
						OggReader * oggReader = (OggReader *)source;
						
						return oggReader->m_inputStream->rdbuf()->sgetn((char*)buffer, size * count);
					}
					
					static int seekCallback (void * source, ogg_int64_t offset, int whence) {
						OggReader * oggReader = (OggReader *)source;
						
						oggReader->m_inputStream->seekg(offset, (std::ios::seekdir)whence);
						
						if (oggReader->m_inputStream->bad())
							return -1;
						else
							return 0;
					}
					
					static long tellCallback (void * source) {
						OggReader * oggReader = (OggReader *)source;
						
						return oggReader->m_inputStream->tellg();
					}
			};
			
			OggReader::OggReader (Shared<std::istream> inputStream)
				: m_inputStream(inputStream)
			{
				ov_callbacks callbacks;
				
				callbacks.read_func = readCallback;
				callbacks.seek_func = seekCallback;
				callbacks.tell_func = tellCallback;
				callbacks.close_func = NULL;
				
				int result = ov_open_callbacks(this, &m_file, NULL, 0, callbacks);
				
				if (result != 0)
					throw std::runtime_error(OggResource::errorString(result));
			}
			
			OggReader::~OggReader ()
			{
				ov_clear(&m_file);
			}
			
#pragma mark -
#pragma mark OggStream
			
			class OggStream : public Stream
			{
				EXPOSE_CLASS(OggStream)
				
				class Class : public Stream::Class
				{
					EXPOSE_CLASSTYPE
				};
				
				protected:
					virtual bool loadNextBuffer (PTR(Source) source, ALuint buffer);
					
					OggReader m_reader;
					const REF(IData) m_data;
					
					bool m_loop;
					
				public:
					OggStream (PTR(Source) source, ALenum format, ALsizei frequency, const PTR(IData) data);
					virtual ~OggStream ();
					
					virtual void stop ();
			};
			
			IMPLEMENT_CLASS(OggStream)
			
			OggStream::OggStream (PTR(Source) source, ALenum format, ALsizei frequency, const PTR(IData) data)
				: Stream(source, format, frequency), m_reader(data->inputStream()), m_data(data), m_loop(true)
			{
				
			}
			
			OggStream::~OggStream ()
			{
			
			}
			
			void OggStream::stop ()
			{
				Stream::stop();
				
				// Reset playhead.
				int result = ov_pcm_seek(m_reader.file(), 0);
				
				if (result != 0)
					throw std::runtime_error(OggResource::errorString(result));
			}
			
			// Load a chunk of ogg data into the given buffer
			bool OggStream::loadNextBuffer (PTR(Source) source, ALuint buffer)
			{
				ByteT chunk[ChunkSize];
				IndexT size = 0;
				int section, result = 0;
				
				while (size < ChunkSize)
				{
					result = ov_read(m_reader.file(), (char*)(chunk + size), ChunkSize - size, 0, 2, 1, &section);
					
					if (result > 0)
						size += result;
					else if (result == 0 && m_loop)
						ov_pcm_seek(m_reader.file(), 0);
					else if (result == 0)
						break;
					else
						throw std::runtime_error(OggResource::errorString(result));
				}
				
				if (size == 0) {
					stopBufferCallbacks();
					return false;
				}
				
				AudioError::reset();
				alBufferData(buffer, m_format, chunk, size, m_frequency);
				AudioError::check("Buffering Data");
				
				return true;
			}

#pragma mark -
#pragma mark OggResource

			IMPLEMENT_CLASS(OggResource)

			void OggResource::Class::registerLoaderTypes (REF(ILoader) loader)
			{
				loader->setLoaderForExtension(this, "ogg");
				loader->setLoaderForExtension(this, "oga");
			}
			
			REF(Object) OggResource::Class::initFromData (const PTR(IData) data, const ILoader * loader)
			{
				return new OggResource(data);
			}
			
			OggResource::OggResource (const PTR(IData) data)
				: m_data(data)
			{
				OggReader oggReader(data->inputStream());
				
				vorbis_info * info = ov_info(oggReader.file(), -1);
				//m_comment = ov_comment(oggReader.file(), -1);
				
				// Determine audio chunk format
				if (info->channels == 1)
					m_format = AL_FORMAT_MONO16;
				else
					m_format = AL_FORMAT_STEREO16;
				
				m_frequency = info->rate;
			}
			
			OggResource::~OggResource ()
			{
			
			}
			
			REF(Stream) OggResource::createStream (PTR(Source) source)
			{
				return new OggStream(source, m_format, m_frequency, m_data);
			}
			
			REF(Sound) OggResource::createSound (PTR(Source) source)
			{
				return NULL;
			}
			
			StringT OggResource::errorString (int code) {
				switch(code) {
					case OV_EREAD:
						return "Could not read audio stream.";
					case OV_ENOTVORBIS:
						return "Stream does not contain Vorbis data.";
					case OV_EVERSION:
						return "Vorbis version mismatch.";
					case OV_EBADHEADER:
						return "Invalid Vorbis stream header.";
					case OV_EFAULT:
						return "Internal logic fault (bug or heap/stack corruption).";
					default:
						return "Unknown OGG/Vorbis error.";
				}
			}
			
			/*
			void OggResource::debug() {
				std::cerr
					<< "version         " << m_info->version         << "\n"
					<< "channels        " << m_info->channels        << "\n"
					<< "rate (hz)       " << m_info->rate            << "\n"
					<< "bitrate upper   " << m_info->bitrate_upper   << "\n"
					<< "bitrate nominal " << m_info->bitrate_nominal << "\n"
					<< "bitrate lower   " << m_info->bitrate_lower   << "\n"
					<< "bitrate window  " << m_info->bitrate_window  << "\n"
					<< "\n"
					<< "vendor " << m_comment->vendor << "\n";
					
				for(int i = 0; i < m_comment->comments; i++)
					std::cerr << "   " << m_comment->user_comments[i] << "\n";
					
				std::cerr << std::endl;
			}
			*/
		}
	}
}
