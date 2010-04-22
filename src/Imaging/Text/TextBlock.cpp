/*
 *  Imaging/Text/TextBlock.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 31/07/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "TextBlock.h"
#include "FontFace.h"

#include "../../Numerics/Matrix.h"
#include "../../Numerics/Vector.h"

namespace Dream
{
	namespace Imaging
	{
		namespace Text
		{
			
			class TextLineRenderer {
			protected:
				Vector<2,unsigned> pen;
				Vector<2,unsigned> m_extents;
				Detail::FontFace * m_face;
				
				FT_Pos prevRSBDelta;
				
				// Glyph Indices
				FT_UInt current;
				FT_UInt previous;
				
				FT_Error err;
				bool kerning;
				
				REF(IMutablePixelBuffer) img;
				
				unsigned m_count;
			public:
				TextLineRenderer (Detail::FontFace * _face, bool _kerning) : m_face(_face), kerning(_kerning), pen(ZERO), m_extents(ZERO)
				{
					prevRSBDelta = 0;
					
					current = 0;
					previous = 0;
					
					err = 0;
					m_count = 0;
				}
				
				TextLineRenderer (const TextLineRenderer & other)
				{
					pen = other.pen;
					m_face = other.m_face;
					prevRSBDelta = other.prevRSBDelta;
					current = other.current;
					previous = other.previous;
					kerning = other.kerning;
					img = other.img;
					m_extents = other.m_extents;
					m_count = other.m_count;
				}
				
				void setImage (REF(IMutablePixelBuffer) _img)
				{
					img = _img;
				}
				
				void setOrigin(Vector<2,unsigned> _pen)
				{
					pen = vec<unsigned>(_pen[X] << 6, _pen[Y] << 6);
				}
				
				Vector<2,unsigned> origin()
				{
					return vec<unsigned>(pen[X] >> 6, pen[Y] >> 6);
				}
				
				const Vector<2,unsigned> & extents()
				{
					return m_extents;
				}
				
				AlignedBox2u compositeCharacter (const Detail::FontGlyph * glyph)
				{
					Vector<2,unsigned> origin = glyph->calculateCharacterOrigin(pen);
					
					// Update box extents
					FT_BBox bbox;
					
					// This incorrectly rounds down the box size.
					glyph->getCBox(FT_GLYPH_BBOX_PIXELS, &bbox);
					
					// We add one here to avoid problems where the box isn't quite wide enough
					// due to anti-aliasing issues.
					m_extents[X] = origin[X] + bbox.xMax + 1;
					m_extents[Y] = origin[Y] + bbox.yMax;
					
					// Copy the bitmap
					if (img)
						glyph->compositeToBuffer(origin, img);
					
					return AlignedBox2u(origin, m_extents);
				}
				
				AlignedBox2u processCharacter (wchar_t c)
				{
					AlignedBox2u box;
					
					current = m_face->getCharIndex(c);
					Detail::FontGlyph * glyph = m_face->loadGlyphForIndex(current);
					
					// Offset the first character a little bit
					int delta = 0; //(m_count == 0 ? 64 : 0);
					
					if (kerning && previous != 0 && current != 0) {
						FT_Vector k;
						FT_Get_Kerning(m_face->face(), previous, current, FT_KERNING_DEFAULT, &k);
						
						// Increase the pen width by the kerning
						delta += k.x;
					}
					
					// Subtract or add a pixel for better position
					delta += glyph->hintingAdjustment(prevRSBDelta);
					
					pen[X] += delta;
					
					box = compositeCharacter(glyph);
					
					pen[X] += glyph->advance.x;
					
					// We need the previous character for calculating spacing
					previous = current;
					prevRSBDelta = glyph->rsbDelta;
					
					m_count += 1;
					
					return box;
				}
				
				unsigned advanceOfCharacter (wchar_t c) {
					TextLineRenderer t(*this);
					t.setImage(NULL);
					
					AlignedBox2u box = t.processCharacter(c);
					
					// Ensure box calculation is correct
					ensure(box.size()[WIDTH] == t.extents()[X] - extents()[X]);
					
					return t.extents()[X] - extents()[X];
				}
			};
			
#pragma mark -
#pragma mark TextLine Implementation
			
			TextLine::TextLine (TextBlock * b) : m_block(b), m_width(0)
			{
				m_renderer = new TextLineRenderer(m_block->m_face, m_block->kerningEnabled());
			}
			
			TextLine::~TextLine ()
			{
				delete m_renderer;
			}
			
			bool TextLine::canAddCharacter (wchar_t c) const
			{
				unsigned w = m_renderer->advanceOfCharacter(c) + m_width;
				
				return !(m_block->isLineWidthFixed() && w > m_block->lineWidth());
			}
			
			bool TextLine::addCharacter (wchar_t c)
			{
				//unsigned w = m_renderer->advanceOfCharacter(c) + m_width;
				
				TextLineRenderer tmp(*m_renderer);
				
				m_renderer->processCharacter(c);
				
				unsigned w = m_renderer->extents()[X];
				
				if (m_block->isLineWidthFixed() && w > m_block->lineWidth()) {
					// Roll back the renderer state
					*m_renderer = tmp;
					return false;
				}
				
				m_chars += c;
				m_width = m_renderer->extents()[X];
				
				return true;
			}
			
			void TextLine::compositeToImage (REF(IMutablePixelBuffer) img, Vector<2,unsigned> pen, CharacterBoxes * boxes)
			{
				TextLineRenderer r(m_block->m_face, m_block->kerningEnabled());
				
				r.setImage(img);
				r.setOrigin(pen);
				
				for (iterateEach(m_chars, c)) {
					AlignedBox2u box = r.processCharacter(*c);
					
					if (boxes) boxes->push_back(box);
				}
			}
			
#pragma mark -
#pragma mark TextBlock Implementation
			
			TextBlock::TextBlock (Detail::FontFace * face) : m_face(face)
			{
				clear();
				
				m_lineWidth = 0;
				m_horizontalPadding.zero();
				m_verticalPadding.zero();
								
				setKerning(true);
				setTextDirection(LR, TB);
			}
			
			TextBlock::~TextBlock ()
			{
				for (iterateEach(m_lines, line)) {
					delete (*line);
				}
			}
			
			/*
			 Vector<2,unsigned> TextBlock::pixelSize (wchar_t c) const {
			 FT_Error err;
			 
			 FT_Glyph glyph;
			 FT_BBox bbox;
			 FT_UInt current = FT_Get_Char_Index(m_face, c);
			 err = FT_Load_Glyph(m_face, current, FT_LOAD_DEFAULT);
			 err = FT_Get_Glyph(m_face->glyph, &glyph);
			 
			 FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);
			 
			 return vec<unsigned> (bbox.xMax - bbox.xMin + 1, bbox.yMax - bbox.yMin);
			 }
			 */
			
			void TextBlock::setTextDirection (TextDirection charDir, TextDirection lineDir)
			{
				ensure(charDir & (LR|RL) && lineDir & (TB|BT)
					   || charDir & (TB|BT) && lineDir & (LR|RL));
				
				//if (charDir & (LR|RL)) ensure(FT_HAS_HORIZONTAL(m_face));
				//if (charDir & (TB|BT)) ensure(FT_HAS_VERTICAL(m_face));
				
				//std::cout << "charDir: " << charDir << " lineDir: " << lineDir << std::endl;
				
				m_charDir = charDir;
				m_lineDir = lineDir;
			}
			
			bool TextBlock::isHorizontal () const
			{
				return m_charDir & (LR|RL);
			}
			
			bool TextBlock::isVertical () const
			{
				return m_charDir & (BT|TB);
			}
			
			Vector<2, unsigned> TextBlock::textOrigin ()
			{
				// {0, 0} is bottom left hand corner
				Vector<2, unsigned> v;
				
				TextDirection c = (TextDirection)(m_charDir|m_lineDir);
				
				if (c & LR) {
					v[0] = 0;
				} else if (c & RL) {
					v[0] = 1;
				}
				
				if (c & BT) {
					v[1] = 0;
				} else if (c & TB) {
					v[1] = 1;
				}
				
				return v;
			}
			
			void TextBlock::setKerning (bool enabled)
			{
				m_kerning = enabled;
			}
			
			bool TextBlock::kerningEnabled ()
			{
				return m_face->hasKerning() && m_kerning;
			}
			
			void TextBlock::setLineWidth (const unsigned &w)
			{
				m_lineWidth = w;
			}
			
			bool TextBlock::isLineWidthFixed () const
			{
				return m_lineWidth != 0;
			}
			
			unsigned TextBlock::lineWidth () const
			{
				return m_lineWidth;
			}
			
			void TextBlock::clear ()
			{
				for (iterateEach(m_lines, line)) {
					delete (*line);
				}
				
				m_lines.clear();
				m_extents.zero();
				
				m_lines.push_back(new TextLine(this));
			}
			
			TextLine * TextBlock::lastLine ()
			{
				return m_lines.back();
			}
			
			// We must normalize input to have \n line endings
			void TextBlock::addText (const std::wstring &str)
			{
				TextLine * line = lastLine();
				
				for (iterateEach(str, c)) {
					if (*c == '\n') {
						line = new TextLine(this);
						m_lines.push_back(line);
						continue;
					}
					
					if (!line->addCharacter(*c)) {
						line = new TextLine(this);
						m_lines.push_back(line);
						
						ensure(line->canAddCharacter(*c));
						line->addCharacter(*c);
					}
				}
			}
			
			void TextBlock::setText (const std::wstring &str)
			{
				clear();
				addText(str);
			}
			
			std::wstring TextBlock::text () const
			{
				std::wstring str;
				
				for (iterateEach(m_lines, line)) {
					str += (*line)->text();
				}
				
				return str;
			}
			
			void TextBlock::render (REF(IMutablePixelBuffer) pbuf, CharacterBoxes * boxes)
			{
				Vector<2,unsigned> pen(ZERO);
				Vector<2,unsigned> origin = pbuf->size().reduce() * textOrigin();
				
				//std::cout << "Ascender: " << ascenderOffset() << std::endl;
				
				 // Removes unnecessary space
				if (m_lineDir == TB) {
					pen[Y] += m_face->lineOffset();
					pen[Y] -= m_face->descenderOffset();
				} else {
					pen[Y] += m_face->descenderOffset();
				}
				
				//std::cout << "Text Origin: " << textOrigin() << std::endl;
				
				for (iterateEach(m_lines, line)) {
					if (m_lineDir == TB) {
						//std::cerr << "Line Origin: " << origin - pen << std::endl;
						(*line)->compositeToImage(pbuf, origin - pen, boxes);
					} else {
						//std::cerr << "Line Origin: " << pen << std::endl;
						(*line)->compositeToImage(pbuf, pen, boxes);
					}
					
					// Set pen to next line
					pen[X] = 0;
					pen[Y] += m_face->lineOffset();
				}
			}
			
			Vector<2,unsigned> TextBlock::calculateSize () const
			{
				Vector<2,unsigned> result(ZERO);
				
				//std::cout << "Calculating text size for " << m_lines.size() << " lines..." << std::endl;
				
				if (isLineWidthFixed()) {
					result[X] = lineWidth();
				} else {
					result[X] = 0;
					for (iterateEach(m_lines, line)) {
						//std::wcout << "'" << (*line)->text() << "' " << (*line)->width() << "(" << result[X] << ")" << std::endl;
						result[X] = std::max(result[X], (*line)->width());
					}
				}
				
				//std::cout << m_lines.size() << std::endl;
				
				result[Y] = m_lines.size() * m_face->lineOffset() + m_face->descenderOffset();
				
				// Removes unnecessary space
				if (m_lines.size())
					result[Y] -= m_face->descenderOffset();
				
				result += vec<unsigned>(m_horizontalPadding.sum(), m_verticalPadding.sum());
				
				return result;
			}
			
		}
	}
}

/*
Bottom Left
LR BT
/  1   0   0 \
|  0   1   0 | * [x, y, 1] = [x, y, 1]
\  0   0   1 /

BT LR
/  0   1   0 \
|  1   0   0 | * [x, y, 1] = [y, x, 1]
\  0   0   1 /

Top Left
LR TB
/  1   0   0 \
|  0  -1  sy | * [x, y, 1] = [x, -y + sy, 1]
\  0   0   1 /

TB LR
/  0  -1  sy \
|  1   0   0 | * [x, y, 1] = [-y + sy, x, 1]
\  0   0   1 /

Bottom Right
RL BT
/ -1   0  sx \
|  0   1   0 | * [x, y, 1] = [-x + sx, y, 1]
\  0   0   1 /

BT RL
/  0   1   0 \
| -1   0  sx | * [x, y, 1] = [y, -x + sx, 1]
\  0   0   1 /

Top Right
RL TB
/ -1   0  sx \
|  0  -1  sy | * [x, y, 1] = [-x + sx, -y + sy, 1]
\  0   0   1 /

TB RL
/  0  -1  sy \
| -1   0  sx | * [x, y, 1] = [-y + sy, -x + sx, 1]
\  0   0   1 /
*/
