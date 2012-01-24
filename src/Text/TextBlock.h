//
//  Text/TextBlock.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 19/08/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_TEXT_TEXTBLOCK_H
#define _DREAM_IMAGING_TEXT_TEXTBLOCK_H

#include "Font.h"
#include "FontFace.h"
#include "../Geometry/AlignedBox.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace Dream
{
	namespace Text
	{
		using Geometry::AlignedBox2u;
	
		enum TextDirection {
			LR = 1,
			RL = 2,
			TB = 4,
			BT = 8
		};
		
		class TextLine;
		
		typedef std::vector<AlignedBox2u> CharacterBoxes;
		
		class TextBlock {
		protected:
			Vector<2,unsigned> _extents;
			unsigned _lineWidth;
			std::vector<TextLine*> _lines;
			
			Vector<2,unsigned> _horizontal_padding;
			Vector<2,unsigned> _vertical_padding;
			
			TextDirection _char_dir, _lineDir;
			
			// Freetype
			bool _kerning;
			Detail::FontFace * _face;
			
			friend class TextLine;
			
			void composite_characters(Ref<IMutablePixelBuffer> pbuf, CharacterBoxes * boxes);
			
		public:
			TextBlock (Detail::FontFace * font);
			virtual ~TextBlock ();
			
			//Vector<2,unsigned> pixel_size (CodePointT c) const;
			
			void set_text_direction (TextDirection char_dir, TextDirection line_dir);
			
			bool is_horizontal () const;
			bool is_vertical () const;
			
			Vector<2, unsigned> text_origin ();
			
			void set_kerning (bool enabled);
			bool kerning_enabled ();
			
			void set_line_width (const unsigned &w);
			bool is_line_width_fixed () const;
			unsigned line_width () const;
			
			void composite_to_image (Ref<IMutablePixelBuffer> img);
			
			void clear ();
			TextLine * last_line ();
			
			// We must normalize input to have \n line endings
			void add_text (const std::string &str);
			void set_text (const std::string &str);
			
			std::string text () const;
			
			Vector<2,unsigned> calculate_size () const;
			
			void render (Ref<IMutablePixelBuffer> pbuf, CharacterBoxes * boxes = NULL);
		};
		
		class TextLineRenderer;
		
		class TextLine {
			TextBlock * _block;

			unsigned _width;			
			std::string _chars;
			
			TextLineRenderer * _renderer;
			
		public:
			TextLine (TextBlock * b);
			virtual ~TextLine ();
			
			const std::string & text () const { return _chars; }
			unsigned width () const { return _width; }
			
			bool can_add_character (CodePointT c) const;
			
			bool add_character (CodePointT c);
			void composite_to_image (Ref<IMutablePixelBuffer> img, Vector<2,unsigned> pen, CharacterBoxes * boxes = NULL);
		};
		
	}
}

#endif
