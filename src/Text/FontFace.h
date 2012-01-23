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
			struct FontGlyph
			{
				FT_Glyph glyph;
				
				FT_Pos lsbDelta;
				FT_Pos rsbDelta;
				
				FT_Vector advance;
				
				~FontGlyph ();
				
				void compositeToBuffer(Vector<2,unsigned> origin, REF(IMutablePixelBuffer) img) const;
				
				Vector<2,unsigned> calculateCharacterOrigin (Vector<2,unsigned> pen) const;
				void getCBox(FT_UInt bboxMode, FT_BBox * acbox) const;
				
				bool isBitmap () const;
				bool isOutline () const;
				
				unsigned hintingAdjustment(unsigned prevRSBDelta) const;
			};
			
			class FontFace
			{
			protected:
				typedef std::map<FT_UInt, FontGlyph*> GlyphMapT;
				GlyphMapT m_glyphCache;
				
				FT_Face m_face;
				ImagePixelFormat m_pixelFormat; //ALPHA or INTENSITY
				
			public:
				FontFace (FT_Face _face, ImagePixelFormat _fmt);
				virtual ~FontFace ();
			
				bool hasKerning ();
				
				// These three functions need >> 6 to convert to pixels
				unsigned lineOffset () const;
				unsigned ascenderOffset () const;
				unsigned descenderOffset() const;
				
				FT_UInt getCharIndex (FT_UInt c);
				FT_Face face ();
				
				ImagePixelFormat pixelFormat ();
				
				FontGlyph * loadGlyphForIndex (FT_UInt c);
				
				Vector<2, unsigned> processText(const std::wstring& text, REF(Image) dst);
			};
		}
	}
}

#endif
