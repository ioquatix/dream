//
//  Text/TextBlock.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 31/07/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "TextBlock.h"
#include "FontFace.h"

#include "../Numerics/Matrix.h"
#include "../Numerics/Vector.h"
#include "../Core/Strings.h"

namespace Dream
{
	namespace Text
	{
		class TextLineRenderer {
		protected:
			Detail::FontFace * _face;
			bool kerning;

			Vec2u pen;
			Vec2u _extents;

			FT_Pos prev_rsbdelta;

			// Glyph Indices
			FT_UInt current;
			FT_UInt previous;

			FT_Error err;

			Ref<IMutablePixelBuffer> img;

			unsigned _count;
		public:
			TextLineRenderer (Detail::FontFace * _face, bool _kerning) : _face(_face), kerning(_kerning), pen(ZERO), _extents(ZERO)
			{
				prev_rsbdelta = 0;

				current = 0;
				previous = 0;

				err = 0;
				_count = 0;
			}

			TextLineRenderer (const TextLineRenderer & other)
			{
				pen = other.pen;
				_face = other._face;
				prev_rsbdelta = other.prev_rsbdelta;
				current = other.current;
				previous = other.previous;
				kerning = other.kerning;
				img = other.img;
				_extents = other._extents;
				_count = other._count;
			}

			void set_image (Ref<IMutablePixelBuffer> _img)
			{
				img = _img;
			}

			void set_origin(Vec2u _pen)
			{
				pen = vec<unsigned>(_pen[X] << 6, _pen[Y] << 6);
			}

			Vec2u origin()
			{
				return vec<unsigned>(pen[X] >> 6, pen[Y] >> 6);
			}

			const Vec2u & extents()
			{
				return _extents;
			}

			AlignedBox2u composite_character (const Detail::FontGlyph * glyph)
			{
				Vec2u origin = glyph->calculate_character_origin(pen);

				// Update box extents
				FT_BBox bbox;

				// This incorrectly rounds down the box size.
				glyph->get_cbox(FT_GLYPH_BBOX_PIXELS, &bbox);

				// We add one here to avoid problems where the box isn't quite wide enough
				// due to anti-aliasing issues.
				_extents[X] = origin[X] + bbox.xMax + 1;
				_extents[Y] = origin[Y] + bbox.yMax;

				// Copy the bitmap
				if (img)
					glyph->composite_to_buffer(origin, img);

				return AlignedBox2u(origin, _extents);
			}

			AlignedBox2u process_character (CodePointT c)
			{
				AlignedBox2u box;

				current = _face->get_char_index(c);
				Detail::FontGlyph * glyph = _face->load_glyph_for_index(current);

				// Offset the first character a little bit
				int delta = 0; //(_count == 0 ? 64 : 0);

				if (kerning && previous != 0 && current != 0) {
					FT_Vector k;
					FT_Get_Kerning(_face->face(), previous, current, FT_KERNING_DEFAULT, &k);

					// Increase the pen width by the kerning
					delta += k.x;
				}

				// Subtract or add a pixel for better position
				delta += glyph->hinting_adjustment(prev_rsbdelta);

				pen[X] += delta;

				box = composite_character(glyph);

				pen[X] += glyph->advance.x;

				// We need the previous character for calculating spacing
				previous = current;
				prev_rsbdelta = glyph->rsb_delta;

				_count += 1;

				return box;
			}

			unsigned advance_of_character (CodePointT c) {
				TextLineRenderer t(*this);
				t.set_image(NULL);

				AlignedBox2u box = t.process_character(c);

				// Ensure box calculation is correct
				DREAM_ASSERT(box.size()[WIDTH] == t.extents()[X] - extents()[X]);

				return t.extents()[X] - extents()[X];
			}
		};

// MARK: -
// MARK: TextLine Implementation

		TextLine::TextLine (TextBlock * b) : _block(b), _width(0)
		{
			_renderer = new TextLineRenderer(_block->_face, _block->kerning_enabled());
		}

		TextLine::~TextLine ()
		{
			delete _renderer;
		}

		bool TextLine::can_add_character (CodePointT c) const
		{
			unsigned w = _renderer->advance_of_character(c) + _width;

			return !(_block->is_line_width_fixed() && w > _block->line_width());
		}

		bool TextLine::add_character (CodePointT c)
		{
			//unsigned w = _renderer->advance_of_character(c) + _width;

			TextLineRenderer tmp(*_renderer);

			_renderer->process_character(c);

			unsigned w = _renderer->extents()[X];

			if (_block->is_line_width_fixed() && w > _block->line_width()) {
				// Roll back the renderer state
				*_renderer = tmp;
				return false;
			}

			utf8::append(c, std::back_inserter(_chars));
			_width = _renderer->extents()[X];

			return true;
		}

		void TextLine::composite_to_image (Ref<IMutablePixelBuffer> img, Vec2u pen, CharacterBoxes * boxes)
		{
			TextLineRenderer r(_block->_face, _block->kerning_enabled());

			r.set_image(img);
			r.set_origin(pen);

			foreach (chr, _chars) {
				AlignedBox2u box = r.process_character(*chr);

				if (boxes) boxes->push_back(box);
			}
		}

// MARK: -
// MARK: TextBlock Implementation

		TextBlock::TextBlock (Detail::FontFace * face) : _face(face)
		{
			clear();

			_line_width = 0;
			_horizontal_padding.zero();
			_vertical_padding.zero();

			set_kerning(true);
			set_text_direction(LR, TB);
		}

		TextBlock::~TextBlock ()
		{
			foreach (line, _lines) {
				delete *line;
			}
		}

		/*
		 Vector<2,unsigned> TextBlock::pixel_size (CodePointT c) const {
		 FT_Error err;

		 FT_Glyph glyph;
		 FT_BBox bbox;
		 FT_UInt current = FT_Get_Char_Index(_face, c);
		 err = FT_Load_Glyph(_face, current, FT_LOAD_DEFAULT);
		 err = FT_Get_Glyph(_face->glyph, &glyph);

		 FT_Glyph_Get_CBox(glyph, FT_GLYPH_BBOX_PIXELS, &bbox);

		 return vec<unsigned> (bbox.x_max - bbox.x_min + 1, bbox.y_max - bbox.y_min);
		 }
		 */

		void TextBlock::set_text_direction (TextDirection char_dir, TextDirection line_dir)
		{
			DREAM_ASSERT(((char_dir & (LR|RL)) && (line_dir & (TB|BT)))
			             || ((char_dir & (TB|BT)) && (line_dir & (LR|RL))));

			//if (char_dir & (LR|RL)) DREAM_ASSERT(FT_HAS_HORIZONTAL(_face));
			//if (char_dir & (TB|BT)) DREAM_ASSERT(FT_HAS_VERTICAL(_face));

			//std::cout << "char_dir: " << char_dir << " line_dir: " << line_dir << std::endl;

			_character_direction = char_dir;
			_line_direction = line_dir;
		}

		bool TextBlock::is_horizontal () const
		{
			return _character_direction & (LR|RL);
		}

		bool TextBlock::is_vertical () const
		{
			return _character_direction & (BT|TB);
		}

		Vec2u TextBlock::text_origin ()
		{
			// {0, 0} is bottom left hand corner
			Vec2u v;

			TextDirection c = (TextDirection)(_character_direction|_line_direction);

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

		void TextBlock::set_kerning (bool enabled)
		{
			_kerning = enabled;
		}

		bool TextBlock::kerning_enabled ()
		{
			return _face->has_kerning() && _kerning;
		}

		void TextBlock::set_line_width (const unsigned &w)
		{
			_line_width = w;
		}

		bool TextBlock::is_line_width_fixed () const
		{
			return _line_width != 0;
		}

		unsigned TextBlock::line_width () const
		{
			return _line_width;
		}

		void TextBlock::clear ()
		{
			foreach (line, _lines) {
				delete *line;
			}

			_lines.clear();
			_extents.zero();

			_lines.push_back(new TextLine(this));
		}

		TextLine * TextBlock::last_line ()
		{
			return _lines.back();
		}

		// We must normalize input to have \n line endings
		void TextBlock::add_text (const std::string &str)
		{
			TextLine * line = last_line();

			std::string::const_iterator current = str.begin(), end = str.end();

			while (true) {
				CodePointT codepoint = utf8::next(current, end);

				if (codepoint == '\n') {
					line = new TextLine(this);
					_lines.push_back(line);
					continue;
				}

				if (!line->add_character(*current)) {
					line = new TextLine(this);
					_lines.push_back(line);

					DREAM_ASSERT(line->can_add_character(*current));
					line->add_character(*current);
				}

				if (current == end) {
					break;
				}
			}
		}

		void TextBlock::set_text (const std::string &str)
		{
			clear();
			add_text(str);
		}

		std::string TextBlock::text () const
		{
			std::string str;

			foreach (line, _lines) {
				str += (*line)->text();
			}

			return str;
		}

		void TextBlock::render (Ref<IMutablePixelBuffer> pbuf, CharacterBoxes * boxes)
		{
			Vec2u pen(ZERO);
			Vec2u origin = pbuf->size().reduce() * text_origin();

			//std::cout << "Ascender: " << ascender_offset() << std::endl;

			// Removes unnecessary space
			if (_line_direction == TB) {
				pen[Y] += _face->line_offset();
				pen[Y] -= _face->descender_offset();
			} else {
				pen[Y] += _face->descender_offset();
			}

			//std::cout << "Text Origin: " << text_origin() << std::endl;

			foreach (line, _lines) {
				if (_line_direction == TB) {
					//std::cerr << "Line Origin: " << origin - pen << std::endl;
					(*line)->composite_to_image(pbuf, origin - pen, boxes);
				} else {
					//std::cerr << "Line Origin: " << pen << std::endl;
					(*line)->composite_to_image(pbuf, pen, boxes);
				}

				// Set pen to next line
				pen[X] = 0;
				pen[Y] += _face->line_offset();
			}
		}

		Vec2u TextBlock::calculate_size () const
		{
			Vec2u result(ZERO);

			//std::cout << "Calculating text size for " << _lines.size() << " lines..." << std::endl;

			if (is_line_width_fixed()) {
				result[X] = line_width();
			} else {
				result[X] = 0;
				foreach (line, _lines) {
					//std::wcout << "'" << (*line)->text() << "' " << (*line)->width() << "(" << result[X] << ")" << std::endl;
					result[X] = std::max(result[X], (*line)->width());
				}
			}

			//std::cout << _lines.size() << std::endl;

			result[Y] = _lines.size() * _face->line_offset() + _face->descender_offset();

			// Removes unnecessary space
			if (_lines.size())
				result[Y] -= _face->descender_offset();

			result += vec<unsigned>(_horizontal_padding.sum(), _vertical_padding.sum());

			return result;
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
