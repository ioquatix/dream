//
//  Text/Font.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 8/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_TEXT_FONT_H
#define _DREAM_IMAGING_TEXT_FONT_H

#include "../Imaging/Image.h"

namespace Dream
{
	namespace Text
	{
		using namespace Dream::Imaging;

		// Could be worth checking out http://branan.github.com/gltext/

		class TypographyException : public std::exception {
		protected:
			int _error;

		public:
			TypographyException (int error) : _error(error) {
			}

			const int & error_state () { return _error; }
		};

		namespace Detail {
			class FontFace;
		}

		class Font : public Object {
		protected:
			Detail::FontFace *_face;
			Ref<IData> _font_data;

			Vec2u compute_bounding_box (const std::string & text) const;

		public:
			class Loader : implements ILoadable {
			public:
				virtual void register_loader_types (ILoader * loader);

				virtual Ref<Object> init_with_path(const Path & p);
				virtual Ref<Object> load_from_data(const Ptr<IData> data, const ILoader * loader);
			};

			Font (const Path &);
			Font (const Ptr<IData> data);

			virtual ~Font ();

			Detail::FontFace * font_face ();
			const Detail::FontFace * font_face () const;

			void set_pixel_size (unsigned sz);
			std::size_t single_line_offset ();

			// utf8 encoding is assumed.
			Ref<Image> render_text (const std::string & text);
			Ref<Image> render_text (const std::string & text, unsigned line_width);
		};
	}
}

#endif
