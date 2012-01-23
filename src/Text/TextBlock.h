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
			Vector<2,unsigned> m_extents;
			unsigned m_lineWidth;
			std::vector<TextLine*> m_lines;
			
			Vector<2,unsigned> m_horizontalPadding;
			Vector<2,unsigned> m_verticalPadding;
			
			TextDirection m_charDir, m_lineDir;
			
			// Freetype
			bool m_kerning;
			Detail::FontFace * m_face;
			
			friend class TextLine;
			
			void compositeCharacters(REF(IMutablePixelBuffer) pbuf, CharacterBoxes * boxes);
			
		public:
			TextBlock (Detail::FontFace * font);
			virtual ~TextBlock ();
			
			//Vector<2,unsigned> pixelSize (wchar_t c) const;
			
			void setTextDirection (TextDirection charDir, TextDirection lineDir);
			
			bool isHorizontal () const;
			bool isVertical () const;
			
			Vector<2, unsigned> textOrigin ();
			
			void setKerning (bool enabled);
			bool kerningEnabled ();
			
			void setLineWidth (const unsigned &w);
			bool isLineWidthFixed () const;
			unsigned lineWidth () const;
			
			void compositeToImage (REF(IMutablePixelBuffer) img);
			
			void clear ();
			TextLine * lastLine ();
			
			// We must normalize input to have \n line endings
			void addText (const std::wstring &str);
			void setText (const std::wstring &str);
			std::wstring text () const;
			
			Vector<2,unsigned> calculateSize () const;
			
			void render (REF(IMutablePixelBuffer) pbuf, CharacterBoxes * boxes = NULL);
		};
		
		class TextLineRenderer;
		
		class TextLine {
			TextBlock * m_block;

			unsigned m_width;			
			std::wstring m_chars;
			
			TextLineRenderer * m_renderer;
			
		public:
			TextLine (TextBlock * b);
			virtual ~TextLine ();
			
			const std::wstring & text () const { return m_chars; }
			unsigned width () const { return m_width; }
			
			bool canAddCharacter (wchar_t c) const;
			
			bool addCharacter (wchar_t c);
			void compositeToImage (REF(IMutablePixelBuffer) img, Vector<2,unsigned> pen, CharacterBoxes * boxes = NULL);
		};
		
	}
}

#endif
