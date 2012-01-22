/*
 *  Client/Text/TextBuffer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 26/09/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "TextBuffer.h"
#include "TextBlock.h"

namespace Dream
{
	namespace Text
	{
	
#pragma mark -
		
		TextBuffer::TextBuffer (REF(Font) font) : m_font(font), m_textUpdated(true), m_useStaticSize(false)
		{
		
		}
		
		TextBuffer::~TextBuffer ()
		{
		
		}
		
		void TextBuffer::setStaticSize (Vector<2, unsigned> size) {
			m_size = size;
			m_useStaticSize = true;
		}
		
		void TextBuffer::setDynamicSize () {
			m_size.zero();
			m_useStaticSize = false;
		}
		
		void TextBuffer::setText (const std::string & text)
		{
			setText(convertStringToUTF16(text));
		}
		
		void TextBuffer::setText (const std::wstring & text)
		{
			if (m_text != text) {
				m_text = text;
				m_textUpdated = true;
			}
		}
		
		void TextBuffer::insertCharacterAtOffset (unsigned offset, unsigned character) {
			m_textUpdated = true;
			m_text.insert(m_text.begin()+offset, character);
		}
		
		unsigned TextBuffer::offsetForPoint (const Vec2u offset) {
			return m_text.size();
		}
		
		REF(IPixelBuffer) TextBuffer::renderText (bool & regenerated)
		{
			regenerated = false;
			
			if (m_textUpdated) {
				if (m_imageText != m_text) {
					TextBlock textBlock(m_font->fontFace());

					if (m_useStaticSize) {
						textBlock.setLineWidth(m_size[X]);
					}
					
					textBlock.setText(m_text);
					Vec2u textBlockSize = textBlock.calculateSize();
					
					if (m_image)
						m_image->allocate(textBlockSize << 1, LUMINANCE, UBYTE);
					else
						m_image = new Image(textBlockSize << 1, LUMINANCE, UBYTE);
					
					m_image->clear();
					
					textBlock.render(m_image);
					regenerated = true;
				}
				
				m_imageText = m_text;
				m_textUpdated = false;
			}
			
			return m_image;
		}
		
	}
}
