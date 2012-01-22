/*
 *  Client/Text/TextBuffer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/09/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_TEXT_TEXTBUFFER_H
#define _DREAM_IMAGING_TEXT_TEXTBUFFER_H

#include "Font.h"
#include "../Core/Strings.h"

namespace Dream
{
	namespace Text
	{
		
		class TextBuffer : public Object {
		protected:
			REF(Font) m_font;
			REF(Image) m_image;
			std::wstring m_imageText;
			
			std::wstring m_text;
			bool m_textUpdated;
			
			bool m_useStaticSize;
			Vector<2, unsigned> m_size;
			
		public:
			TextBuffer (REF(Font) font);
			virtual ~TextBuffer ();
			
			void setText (const std::string & text);
			void setText (const std::wstring & text);

			template <typename StringT>
			void appendText (const StringT & text)
			{
				setText(m_text + convertStringToUTF16(text));
			}
			
			template <typename StringT>
			void appendLine (const StringT & str)
			{
				appendText(str);
				appendText("\n");
			}
			
			void insertCharacterAtOffset (unsigned offset, unsigned character);
			unsigned offsetForPoint (const Vec2u offset);
			
			void setStaticSize (Vector<2, unsigned> size);
			void setDynamicSize ();
			
			REF(IPixelBuffer) renderText (bool & regenerated);
		};
		
	}
}

#endif
