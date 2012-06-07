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

static Vec3 position_in_view (UIView * view, UITouch * touch)
{
	CGPoint locationInView = [touch locationInView:view];
	CGRect bounds = [view bounds];
	
	return Vec3(locationInView.x, bounds.size.height - locationInView.y, 0);
}

static AlignedBox<2> bounds_from_frame (CGRect frame)
{
	Vec2 origin(frame.origin.x, frame.origin.y);
	Vec2 size(frame.size.width, frame.size.height);
	
	AlignedBox<2> bounds(origin, origin + size);
	
	return bounds;
}

@implementation DOpenGLView

@synthesize displayContext = _display_context;

- (void)setup
{
	[super setup];

	_multi_finger_input = new MultiFingerInput;
		
	[self initializeKeyboard];
}

- (void) dealloc
{
	delete _multi_finger_input;
	[super dealloc];
}

- (void) render: (CADisplayLink*)displayLink
{
	if (!_display_context)
		return;

	TimeT nextTime = [displayLink timestamp] + [displayLink duration];
	
	_display_context->render_frame_for_time(nextTime);
}

- (void)touchesBegan: (NSSet *)touches withEvent: (UIEvent *)event
{
	if (!_display_context)
		return;

	for (UITouch * touch in touches) {
		const FingerTracking & ft = _multi_finger_input->begin_motion((FingerID)touch, position_in_view(self, touch));
		
		Key touchKey(DefaultTouchPad, ft.button);
		MotionInput motionInput(touchKey, Pressed, ft.position, ft.motion, bounds_from_frame([self frame]));
		_display_context->process(motionInput);
	}
}

- (void)touchesMoved: (NSSet *)touches withEvent: (UIEvent *)event
{
	if (!_display_context)
		return;

	for (UITouch * touch in touches) {
		const FingerTracking & ft = _multi_finger_input->update_motion((FingerID)touch, position_in_view(self, touch));
		
		Key touchKey(DefaultTouchPad, ft.button);
		MotionInput motionInput(touchKey, Dragged, ft.position, ft.motion, bounds_from_frame([self frame]));
		_display_context->process(motionInput);
	}
}

- (void)touchesEnded: (NSSet *)touches withEvent: (UIEvent *)event
{
	if (!_display_context)
		return;

	for (UITouch * touch in touches) {
		FingerTracking ft = _multi_finger_input->finish_motion((FingerID)touch, position_in_view(self, touch));
		
		Key touchKey(DefaultTouchPad, ft.button);
		MotionInput motionInput(touchKey, Released, ft.position, ft.motion, bounds_from_frame([self frame]));
		_display_context->process(motionInput);
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
	return _keyboard_visible;
}

- (void)initializeKeyboard
{	
	_text_field = [[[UITextField alloc] initWithFrame: CGRectZero] autorelease];
	_text_field.delegate = self;
	/* placeholder so there is something to delete! */
	_text_field.text = @" ";	
	
	/* set UITextInputTrait properties, mostly to defaults */
	_text_field.autocapitalizationType = UITextAutocapitalizationTypeNone;
	_text_field.autocorrectionType = UITextAutocorrectionTypeNo;
	_text_field.enablesReturnKeyAutomatically = NO;
	_text_field.keyboardAppearance = UIKeyboardAppearanceDefault;
	_text_field.keyboardType = UIKeyboardTypeDefault;
	_text_field.returnKeyType = UIReturnKeyDefault;
	_text_field.secureTextEntry = NO;	
	
	_text_field.hidden = YES;
	_keyboard_visible = NO;
	/* add the UITextField (hidden) to our view */
	[self addSubview: _text_field];
}

/* reveal onscreen virtual keyboard */
- (void) showKeyboard
{
	[_text_field becomeFirstResponder];
	_keyboard_visible = YES;
}

/* hide onscreen virtual keyboard */
- (void) hideKeyboard
{
	[_text_field resignFirstResponder];
	_keyboard_visible = NO;
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
