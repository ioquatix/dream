/*
 *  Client/Display/UIKitContext/DreamView.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 29/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

// This is a private header, and should not be used as public API.

#import "EAGLView.h"

#include "../MultiFingerInput.h"

@interface DreamView : EAGLView <UITextFieldDelegate> {
	Dream::Client::Display::MultiFingerInput * m_multiFingerInput;
	
	UITextField * m_textField;
	BOOL m_keyboardVisible;
}

- (BOOL) isKeyboardVisible;
- (void) showKeyboard;
- (void) hideKeyboard;

@end
