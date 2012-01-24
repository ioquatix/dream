//
//  Text/TextBuffer.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/09/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_TEXT_TEXTBUFFER_H
#define _DREAM_IMAGING_TEXT_TEXTBUFFER_H

#include "Font.h"
#include "../Core/Strings.h"

namespace Dream
{
	namespace Text
	{
		
		class TextBuffer : public Object {
		protected:
			Ref<Font> _font;
			Ref<Image> _image;
			std::string _image_text;
			
			std::string _text;
			bool _text_updated;
			
			bool _use_static_size;
			Vector<2, unsigned> _size;
			
		public:
			TextBuffer (Ref<Font> font);
			virtual ~TextBuffer ();
			
			void set_text (const std::string & text);

			template <typename StringT>
			void append_text (const StringT & text)
			{
				set_text(_text + convert_string_to_utf16(text));
			}
			
			template <typename StringT>
			void append_line (const StringT & str)
			{
				append_text(str);
				append_text("\n");
			}
			
			void insert_character_at_offset (unsigned offset, unsigned character);
			unsigned offset_for_point (const Vec2u offset);
			
			void set_static_size (Vector<2, unsigned> size);
			void set_dynamic_size ();
			
			Ref<IPixelBuffer> render_text (bool & regenerated);
		};
		
	}
}

#endif
