//
//  Client/Display/UIKit/DOpenGLView.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 29/04/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//

#import "DOpenGLView.h"
#include "Context.h"

using namespace Dream::Client::Display;

@interface DOpenGLView(Private)
- (void) initializeKeyboard;

- (BOOL)textField: (UITextField *)_textField shouldChangeCharactersInRange: (NSRange)range replacementString: (NSString *)string;
- (BOOL)textFieldShouldReturn: (UITextField*)_textField;
@end

static Vec3 positionInView (UIView * view, UITouch * touch)
{
	CGPoint locationInView = [touch locationInView:view];
	CGRect bounds = [view bounds];
	
	return Vec3(locationInView.x, bounds.size.height - locationInView.y, 0);
}

AlignedBox<2> boundsFromFrame (CGRect frame)
{
	Vec2 origin(frame.origin.x, frame.origin.y);
	Vec2 size(frame.size.width, frame.size.height);
	
	AlignedBox<2> bounds(origin, origin + size);
	
	return bounds;
}

@implementation DOpenGLView

@synthesize displayContext;

- (void)setup
{
	[super setup];

	m_multiFingerInput = new MultiFingerInput;
		
	[self initializeKeyboard];
}

- (void) dealloc
{
	delete m_multiFingerInput;
	[super dealloc];
}

- (void) render: (CADisplayLink*)displayLink
{
	if (!displayContext)
		return;

	TimeT nextTime = [displayLink timestamp] + [displayLink duration];
	
	displayContext->renderFrameForTime(nextTime);
}

- (void)touchesBegan: (NSSet *)touches withEvent: (UIEvent *)event
{
	if (!displayContext)
		return;

	for (UITouch * touch in touches) {
		const FingerTracking & ft = m_multiFingerInput->beginMotion((FingerID)touch, positionInView(self, touch));
		
		Key touchKey(DefaultTouchPad, ft.button);
		MotionInput motionInput(touchKey, Pressed, ft.position, ft.motion, boundsFromFrame([self frame]));
		displayContext->process(motionInput);
	}
}

- (void)touchesMoved: (NSSet *)touches withEvent: (UIEvent *)event
{
	if (!displayContext)
		return;

	for (UITouch * touch in touches) {
		const FingerTracking & ft = m_multiFingerInput->updateMotion((FingerID)touch, positionInView(self, touch));
		
		Key touchKey(DefaultTouchPad, ft.button);
		MotionInput motionInput(touchKey, Dragged, ft.position, ft.motion, boundsFromFrame([self frame]));
		displayContext->process(motionInput);
	}
}

- (void)touchesEnded: (NSSet *)touches withEvent: (UIEvent *)event
{
	if (!displayContext)
		return;

	for (UITouch * touch in touches) {
		FingerTracking ft = m_multiFingerInput->finishMotion((FingerID)touch, positionInView(self, touch));
		
		Key touchKey(DefaultTouchPad, ft.button);
		MotionInput motionInput(touchKey, Released, ft.position, ft.motion, boundsFromFrame([self frame]));
		displayContext->process(motionInput);
	}
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	/* This can happen if the user puts more than 5 touches on the screen at once, or perhaps in other circumstances.  Usually (it seems) all active touches are
	 canceled. We forward this on to touchesEnded, which will hopefully provide adequate behaviour. */
	[self touchesEnded:touches withEvent:event];
}

- (BOOL) isKeyboardVisible
{
	return m_keyboardVisible;
}

- (void)initializeKeyboard
{	
	m_textField = [[[UITextField alloc] initWithFrame: CGRectZero] autorelease];
	m_textField.delegate = self;
	/* placeholder so there is something to delete! */
	m_textField.text = @" ";	
	
	/* set UITextInputTrait properties, mostly to defaults */
	m_textField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	m_textField.autocorrectionType = UITextAutocorrectionTypeNo;
	m_textField.enablesReturnKeyAutomatically = NO;
	m_textField.keyboardAppearance = UIKeyboardAppearanceDefault;
	m_textField.keyboardType = UIKeyboardTypeDefault;
	m_textField.returnKeyType = UIReturnKeyDefault;
	m_textField.secureTextEntry = NO;	
	
	m_textField.hidden = YES;
	m_keyboardVisible = NO;
	/* add the UITextField (hidden) to our view */
	[self addSubview: m_textField];
}

/* reveal onscreen virtual keyboard */
- (void) showKeyboard
{
	[m_textField becomeFirstResponder];
	m_keyboardVisible = YES;
}

/* hide onscreen virtual keyboard */
- (void) hideKeyboard
{
	[m_textField resignFirstResponder];
	m_keyboardVisible = NO;
}

/* UITextFieldDelegate method.  Invoked when user types something. */
- (BOOL)textField: (UITextField *)_textField shouldChangeCharactersInRange: (NSRange)range replacementString: (NSString *)string
{	
	return NO; /* don't allow the edit! (keep placeholder text there) */
}

/* Terminates the editing session */
- (BOOL)textFieldShouldReturn: (UITextField*)_textField
{
	[self hideKeyboard];

	return YES;
}

@end
