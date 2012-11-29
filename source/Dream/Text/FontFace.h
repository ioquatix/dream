//
//  Text/FontFace.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 30/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_TEXT_FONTFACE_H
#define _DREAM_IMAGING_TEXT_FONTFACE_H

// This is a private header, and should not be used as public API.

#include "Font.h"

#include <map>

// Should we hide these details?
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

namespace Dream
{
	namespace Text
	{
		namespace Detail
		{
			struct FontGlyph {
				FT_Glyph glyph;

				FT_Pos lsb_delta;
				FT_Pos rsb_delta;

				FT_Vector advance;

				~FontGlyph ();

				void composite_to_buffer(Vec2u origin, Ref<IMutablePixelBuffer> img) const;

				Vec2u calculate_character_origin (Vec2u pen) const;
				void get_cbox(FT_UInt bbox_mode, FT_BBox * acbox) const;

				bool is_bitmap () const;
				bool is_outline () const;

				unsigned hinting_adjustment(unsigned prev_rsbdelta) const;
			};

			class FontFace {
			protected:
				typedef std::map<FT_UInt, FontGlyph*> GlyphMapT;
				GlyphMapT _glyph_cache;

				FT_Face _face;
				PixelFormat _pixel_format; //ALPHA or INTENSITY

			public:
				FontFace (FT_Face _face, PixelFormat _fmt);
				virtual ~FontFace ();

				bool has_kerning ();

				// These three functions need >> 6 to convert to pixels
				unsigned line_offset () const;
				unsigned ascender_offset () const;
				unsigned descender_offset() const;

				FT_UInt get_char_index (FT_UInt c);
				FT_Face face ();

				PixelFormat pixel_format ();

				FontGlyph * load_glyph_for_index (FT_UInt c);

				Vec2u process_text(const std::string & text, Ref<Image> dst);
			};
		}
	}
}

#endif
