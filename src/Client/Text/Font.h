/*
 *  Client/Text/Font.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 8/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_TEXT_FONT_H
#define _DREAM_IMAGING_TEXT_FONT_H

#include "../../Imaging/Image.h"

namespace Dream
{
	namespace Client
	{
		namespace Text
		{
			using namespace Dream::Imaging;
		
			/* Use std::stringstream to easily manage dynamic strings */
			
			class TypographyException : public std::exception {
			protected:
				int m_error;
				
			public:
				TypographyException (int error) : m_error(error) {
					
				}
				
				const int & errorState () { return m_error; }
			};
			
			namespace Detail {
				class FontFace;
			}
			
			class Font : public Object {
			protected:
				Detail::FontFace *m_face;
				REF(IData) m_fontData;
				
				Vector<2, unsigned> computeBoundingBox (const std::wstring & text) const;
				
			public:
				class Loader : implements ILoadable {
				public:
					virtual void registerLoaderTypes (ILoader * loader);
					
					virtual REF(Object) initWithPath(const Path & p);
					virtual REF(Object) loadFromData(const PTR(IData) data, const ILoader * loader);
				};
				
				Font (const Path &);
				Font (const PTR(IData) data);
				
				virtual ~Font ();
				
				Detail::FontFace * fontFace ();
				const Detail::FontFace * fontFace () const;
				
				void setPixelSize (unsigned sz);
				IndexT singleLineOffset ();
				
				REF(Image) renderText (const std::wstring & text);
				REF(Image) renderText (const std::wstring & text, unsigned lineWidth);
			};
			
		}
	}
}

#endif