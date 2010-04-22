/*
 *  Imaging/Text/Font.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 8/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_TEXT_FONT_H
#define _DREAM_IMAGING_TEXT_FONT_H

#include "../Image.h"

namespace Dream
{
	namespace Imaging
	{
		namespace Text
		{
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
				REF(Data) m_fontData;
				
				Vector<2, unsigned> computeBoundingBox (const std::wstring & text) const;
				
			public:		
				EXPOSE_CLASS(Font)
				
				class Class : public Object::Class, IMPLEMENTS(Loadable::Class) {
				public:
					EXPOSE_CLASSTYPE
					
					virtual void registerLoaderTypes (REF(ILoader) loader);
					
					virtual REF(Object) initWithPath(const Path & p);
					virtual REF(Object) initFromData(const REF(Data) data, const ILoader * loader);
				};
				
				Font (const Path &);
				Font (const REF(Data) data);
				
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