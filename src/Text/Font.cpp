//
//  Text/Font.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 8/05/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Font.h"

#include "../Resources/Loader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FontFace.h"
#include "TextBlock.h"

namespace Dream
{
	namespace Text
	{
		
		const char * ft2_error_message(int code);
		
		FT_Library freetype_library ()
		{
			using namespace std;
			static FT_Library library = NULL;
			
			if (library == NULL) {
				FT_Error err = FT_Init_FreeType(&library);
				
				if (err) {
					cerr << "Error loading freetype: " << err << endl;
				}
			}
			
			return library;
		}
		
		using namespace std;
		
		void Font::Loader::register_loader_types (ILoader * loader) {
			loader->set_loader_for_extension(this, "ttf");
			loader->set_loader_for_extension(this, "dfont");
		}
		
		Ref<Object> Font::Loader::init_with_path(const Path & p) {
			return new Font(p);
		}
		
		Ref<Object> Font::Loader::load_from_data(const Ptr<IData> data, const ILoader * loader) {
			return new Font(data);
		}
		
		Font::Font (const Path & p) : _face(NULL)
		{
			FT_Face face;
			FT_Error err = FT_New_Face(freetype_library(), p.to_local_path().c_str(), 0, &face);
			
			if (err) {
				throw LoadError(StringT("Error loading freetype font (") + p.to_local_path() + "): " + ft2_error_message(err));
			} else {
				_face = new Detail::FontFace(face, ALPHA);
			}
			
			set_pixel_size(12);
		}
		
		Font::Font (const Ptr<IData> data) : _face(NULL)
		{
			Shared<Buffer> buffer = data->buffer();
			
			FT_Face face;
			FT_Error err = FT_New_Memory_Face(freetype_library(), buffer->begin(), buffer->size(), 0, &face);
			_font_data = data;
			
			if (err) {
				throw LoadError(StringT("Error loading freetype font:") + ft2_error_message(err));
			} else {
				_face = new Detail::FontFace(face, ALPHA);
			}
			
			set_pixel_size(12);
		}
		
		Font::~Font ()
		{
			if (_face) {
				delete _face;
				_face = NULL;
			}
		}
		
		Detail::FontFace * Font::font_face ()
		{
			return _face;
		}
		
		const Detail::FontFace * Font::font_face () const 
		{
			return _face;
		}
		
		void Font::set_pixel_size(unsigned size)
		{
			ensure(_face != NULL);
			
			FT_Error err = FT_Set_Pixel_Sizes(_face->face(), size, size);
			
			ensure(err == 0);
		}
		
		Vector<2, unsigned> Font::compute_bounding_box (const std::string &text) const
		{
			ensure(_face != NULL);
			
			return _face->process_text(text, Ref<Image>());
		}
		
		Ref<Image> Font::render_text (const std::string & text)
		{
			ensure(_face != NULL);
			
			Ref<Image> img = new Image(compute_bounding_box(text) << 1U, _face->pixel_format(), UBYTE);
			img->zero();
			
			_face->process_text(text, img);
			
			return img;
		}
		
		IndexT Font::single_line_offset ()
		{
			return _face->line_offset();
		}
		
		// uses a method described in fterrors.h to build an error translation function
#undef __FTERRORS_H__
#define FT_ERRORDEF(e, v, s)			case v: return s;
#define FT_ERROR_START_LIST
#define FT_ERROR_END_LIST
		
		const char * ft2_error_message(int code) {
			switch (code) {
#include FT_ERRORS_H
			}
			
			
			std::cerr << __PRETTY_FUNCTION__ << "unknown error code: " << code << std::endl;
			return "unknown error";
		}
		
	}
}
