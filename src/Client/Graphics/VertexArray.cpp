//
//  VertexArray.cpp
//  Dream
//
//  Created by Samuel Williams on 16/02/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#include "VertexArray.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
	
#ifdef DREAM_OPENGLES2
			decltype(&glGenVertexArraysOES) glGenVertexArrays = glGenVertexArraysOES;
			decltype(&glDeleteVertexArraysOES) glDeleteVertexArrays = glDeleteVertexArraysOES;
			decltype(&glBindVertexArrayOES) glBindVertexArray = glBindVertexArrayOES;
#endif
			
			// The implementation is here to hide the various overides defined above - e.g. not in public headers.
			VertexArray::VertexArray() {
				glGenVertexArrays(1, &_handle);
				
				check_graphics_error();
			}
			
			VertexArray::~VertexArray() {
				glDeleteVertexArrays(1, &_handle);
				
				check_graphics_error();
			}
			
			void VertexArray::bind() {
				//logger()->log(LOG_DEBUG, LogBuffer() << "Binding array " << _handle);
				
				glBindVertexArray(_handle);
				
				check_graphics_error();
			}
			
			void VertexArray::unbind() {
				//logger()->log(LOG_DEBUG, LogBuffer() << "Unbinding array " << _handle);
				
				glBindVertexArray(0);
				
				check_graphics_error();
			}
			
			void VertexArray::Binding::enable(GLuint index) {			
				glEnableVertexAttribArray(index);
				
				check_graphics_error();
			}
			
			void VertexArray::Binding::disable(GLuint index) {
				glDisableVertexAttribArray(index);
				
				check_graphics_error();
			}
			
			// These functions facilitate canonical usage where data is stored in vertex buffers.
			void VertexArray::Binding::draw_elements(GLenum mode, GLsizei count, GLenum type) {
				glDrawElements(mode, count, type, 0);
				
				check_graphics_error();
			}
			
			void VertexArray::Binding::draw_arrays(GLenum mode, GLint first, GLsizei count) {
				glDrawArrays(mode, first, count);
				
				check_graphics_error();
			}
			
			void VertexArray::Binding::set_attribute(GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, std::ptrdiff_t offset) {
				glVertexAttribPointer(index, size, type, normalized, stride, (const GLvoid *)offset);
				
				check_graphics_error();
			}

			
		}
	}
}
