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
	
		/* Use std::stringstream to easily manage dynamic strings */
		
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
			
			Vector<2, unsigned> compute_bounding_box (const std::wstring & text) const;
			
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
			IndexT single_line_offset ();
			
			Ref<Image> render_text (const std::wstring & text);
			Ref<Image> render_text (const std::wstring & text, unsigned line_width);
		};
		
	}
}

#endif
