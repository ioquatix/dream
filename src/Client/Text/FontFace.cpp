/*
 *  Client/Text/FontFace.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 30/11/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "FontFace.h"
#include "TextBlock.h"

namespace Dream
{
	namespace Client
	{
		namespace Text
		{
			namespace Detail
			{
#pragma mark -
				FontGlyph::~FontGlyph ()
				{
					FT_Done_Glyph(glyph);
				}
				
				void FontGlyph::compositeToBuffer(Vector<2,unsigned> origin, REF(IMutablePixelBuffer) img) const
				{
					ensure(isBitmap());
					FT_BitmapGlyph bm = (FT_BitmapGlyph)glyph;
					
					if (bm->bitmap.buffer != NULL) {
						UnbufferedImage charImg(ALPHA, UBYTE);
						
						// Retrieve the bitmap
						charImg.setData(bm->bitmap.buffer, vec<unsigned>(bm->bitmap.width, bm->bitmap.rows, 1));
						
						// Composite the character bitmap into the destination image
						img->copyPixelsFrom(charImg, origin << 0, CopyFlip);
					}
				}
				
				Vector<2,unsigned> FontGlyph::calculateCharacterOrigin (Vector<2,unsigned> pen) const
				{
					ensure(isBitmap());
					FT_BitmapGlyph bm = (FT_BitmapGlyph)glyph;
					
					Vector<2,unsigned> origin;
					origin[X] = (pen[X] >> 6) + bm->left;
					origin[Y] = (pen[Y] >> 6) - (bm->bitmap.rows - bm->top);
					
					return origin;
				}
				
				void FontGlyph::getCBox(FT_UInt bboxMode, FT_BBox * acbox) const
				{
					FT_Glyph_Get_CBox(glyph, bboxMode, acbox);
				}
				
				bool FontGlyph::isBitmap () const
				{
					return glyph->format == FT_GLYPH_FORMAT_BITMAP;
				}
				
				bool FontGlyph::isOutline () const
				{
					return glyph->format == FT_GLYPH_FORMAT_OUTLINE;
				}
				
				unsigned FontGlyph::hintingAdjustment(unsigned prevRSBDelta) const
				{
					if (prevRSBDelta - lsbDelta >= 32)
						return 64;
					else if (prevRSBDelta - lsbDelta < -32)
						return 64;
					
					return 0;
				}
					
#pragma mark -
			
				FontFace::FontFace (FT_Face _face, ImagePixelFormat _fmt) : m_face(_face), m_pixelFormat(_fmt)
				{
				
				}
			
				FontFace::~FontFace ()
				{
					int count = 0;
					
					foreach(glyph, m_glyphCache) {
						delete glyph->second;
						count += 1;
					}
					
					FT_Done_Face(m_face);
					
					std::cerr << "Freed " << count << " cached glyphs." << std::endl;
				}
			
				bool FontFace::hasKerning ()
				{
					return FT_HAS_KERNING(m_face);
				}
				
				// These three functions need >> 6 to convert to pixels
				unsigned FontFace::lineOffset () const
				{
					return m_face->size->metrics.height >> 6;
				}
				
				unsigned FontFace::ascenderOffset () const
				{
					return m_face->size->metrics.ascender >> 6;
				}
				
				unsigned FontFace::descenderOffset() const
				{
					return std::abs(m_face->size->metrics.descender >> 6);
				}
				
				FT_UInt FontFace::getCharIndex (FT_UInt c)
				{
					return FT_Get_Char_Index(m_face, c);
				}
				
				FT_Face FontFace::face ()
				{
					return m_face;
				}
				
				ImagePixelFormat FontFace::pixelFormat ()
				{
					return m_pixelFormat;
				}
				
				FontGlyph * FontFace::loadGlyphForIndex (FT_UInt idx)
				{
					//FT_UInt idx = FT_Get_Char_Index(m_face, c);
					GlyphMapT::iterator itr = m_glyphCache.find(idx);
					
					if (itr != m_glyphCache.end()) {
						return itr->second;
					}
					
					FT_Error err = FT_Load_Glyph(m_face, idx, FT_LOAD_RENDER);
					if (err) throw TypographyException(err);
					
					FontGlyph * cache = new FontGlyph;
					
					// Copy glyph into cache
					FT_Get_Glyph(m_face->glyph, &cache->glyph);
					cache->advance = m_face->glyph->advance;
					cache->lsbDelta = m_face->glyph->lsb_delta;
					cache->rsbDelta = m_face->glyph->rsb_delta;
					
					m_glyphCache[idx] = cache;
					
					return cache;
				}
				
				Vector<2, unsigned> FontFace::processText(const std::wstring& text, REF(Image) dst)
				{
					TextBlock block(this);
					
					block.setText(text);
					
					if (dst) {
						block.render(dst);
					}
					
					Vector<2,unsigned> size = block.calculateSize();
					
					return size;
				}				
			}
		}
	}
}
