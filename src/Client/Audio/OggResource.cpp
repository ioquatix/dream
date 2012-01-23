//
//  Client/Audio/OggResource.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 7/10/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#include "OggResource.h"

// We don't need the static callbacks, since we define our own callbacks to hook into IData::input_stream().
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

			class OggReader : private NonCopyable
			{
				protected:
					Shared<std::istream> _input_stream;
					OggVorbis_File _file;
					
				public:
					OggReader (Shared<std::istream> input_stream);
					virtual ~OggReader ();
					
					OggVorbis_File * file ()
					{
						return &_file;
					}
					
				protected:
					static std::size_t read_callback (void * buffer, std::size_t size, std::size_t count, void * source) {
						OggReader * ogg_reader = (OggReader *)source;
						
						return ogg_reader->_input_stream->rdbuf()->sgetn((char*)buffer, size * count);
					}
					
					static int seek_callback (void * source, ogg_int64_t offset, int whence) {
						OggReader * ogg_reader = (OggReader *)source;
						
						ogg_reader->_input_stream->seekg(offset, (std::ios::seekdir)whence);
						
						if (ogg_reader->_input_stream->bad())
							return -1;
						else
							return 0;
					}
					
					static long tell_callback (void * source) {
						OggReader * ogg_reader = (OggReader *)source;
						
						return ogg_reader->_input_stream->tellg();
					}
			};
			
			OggReader::OggReader (Shared<std::istream> input_stream)
				: _input_stream(input_stream)
			{
				ov_callbacks callbacks;
				
				callbacks.read_func = read_callback;
				callbacks.seek_func = seek_callback;
				callbacks.tell_func = tell_callback;
				callbacks.close_func = NULL;
				
				int result = ov_open_callbacks(this, &_file, NULL, 0, callbacks);
				
				if (result != 0)
					throw std::runtime_error(OggResource::error_string(result));
			}
			
			OggReader::~OggReader ()
			{
				ov_clear(&_file);
			}
			
#pragma mark -
#pragma mark OggStream
			
			class OggStream : public Stream {
			protected:
				virtual bool load_next_buffer (Ptr<Source> source, ALuint buffer);
				
				OggReader _reader;
				const Ref<IData> _data;
				
				bool _loop;
				
			public:
				OggStream (Ptr<Source> source, ALenum format, ALsizei frequency, const Ptr<IData> data);
				virtual ~OggStream ();
				
				virtual void stop ();
			};
			
			
			
			OggStream::OggStream (Ptr<Source> source, ALenum format, ALsizei frequency, const Ptr<IData> data)
				: Stream(source, format, frequency), _reader(data->input_stream()), _data(data), _loop(true)
			{
				
			}
			
			OggStream::~OggStream ()
			{
			
			}
			
			void OggStream::stop ()
			{
				Stream::stop();
				
				// Reset playhead.
				int result = ov_pcm_seek(_reader.file(), 0);
				
				if (result != 0)
					throw std::runtime_error(OggResource::error_string(result));
			}
			
			// Load a chunk of ogg data into the given buffer
			bool OggStream::load_next_buffer (Ptr<Source> source, ALuint buffer)
			{
				ByteT chunk[ChunkSize];
				IndexT size = 0;
				int section, result = 0;
				
				while (size < ChunkSize)
				{
					result = ov_read(_reader.file(), (char*)(chunk + size), ChunkSize - size, 0, 2, 1, &section);
					
					if (result > 0)
						size += result;
					else if (result == 0 && _loop)
						ov_pcm_seek(_reader.file(), 0);
					else if (result == 0)
						break;
					else
						throw std::runtime_error(OggResource::error_string(result));
				}
				
				if (size == 0) {
					stop_buffer_callbacks();
					return false;
				}
				
				buffer_data(source, buffer, _format, chunk, size, _frequency);
				
				return true;
			}

#pragma mark -
#pragma mark OggResource

			void OggResource::Loader::register_loader_types (ILoader * loader)
			{
				loader->set_loader_for_extension(this, "ogg");
				loader->set_loader_for_extension(this, "oga");
			}
			
			Ref<Object> OggResource::Loader::load_from_data (const Ptr<IData> data, const ILoader * loader)
			{
				return new OggResource(data);
			}
			
			OggResource::OggResource (const Ptr<IData> data)
				: _data(data)
			{
				OggReader ogg_reader(data->input_stream());
				
				vorbis_info * info = ov_info(ogg_reader.file(), -1);
				//_comment = ov_comment(ogg_reader.file(), -1);
				
				// Determine audio chunk format
				if (info->channels == 1)
					_format = AL_FORMAT_MONO16;
				else
					_format = AL_FORMAT_STEREO16;
				
				_frequency = info->rate;
			}
			
			OggResource::~OggResource ()
			{
			
			}
			
			Ref<Stream> OggResource::create_stream (Ptr<Source> source)
			{
				return new OggStream(source, _format, _frequency, _data);
			}
			
			Ref<Sound> OggResource::create_sound (Ptr<Source> source)
			{
				return NULL;
			}
			
			StringT OggResource::error_string (int code) {
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
					<< "version         " << _info->version         << "\n"
					<< "channels        " << _info->channels        << "\n"
					<< "rate (hz)       " << _info->rate            << "\n"
					<< "bitrate upper   " << _info->bitrate_upper   << "\n"
					<< "bitrate nominal " << _info->bitrate_nominal << "\n"
					<< "bitrate lower   " << _info->bitrate_lower   << "\n"
					<< "bitrate window  " << _info->bitrate_window  << "\n"
					<< "\n"
					<< "vendor " << _comment->vendor << "\n";
					
				for(int i = 0; i < _comment->comments; i++)
					std::cerr << "   " << _comment->user_comments[i] << "\n";
					
				std::cerr << std::endl;
			}
			*/
		}
	}
}
