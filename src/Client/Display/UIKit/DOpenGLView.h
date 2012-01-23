//
//  Client/Display/UIKit/DOpenGLView.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

// This is a private header, and should not be used as public API.

#import "EAGLView.h"

#include "../Context.h"
#include "../../../Events/MultiFingerInput.h"

@interface DOpenGLView : EAGLView <UITextFieldDelegate> {
	Dream::Client::Display::MultiFingerInput * m_multiFingerInput;
	
	UITextField * m_textField;
	BOOL m_keyboardVisible;

	Dream::Client::Display::Context * displayContext;
}

@property(nonatomic,assign) Dream::Client::Display::Context * displayContext;

- (BOOL) isKeyboardVisible;
- (void) showKeyboard;
- (void) hideKeyboard;

@end
