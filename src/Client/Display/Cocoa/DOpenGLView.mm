//
//  Client/Display/Cocoa/DOpenGLView.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#import "DOpenGLView.h"

using namespace Dream::Client::Display;

@implementation DOpenGLView

@synthesize displayContext;

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (void)reshape
{
	if (!displayContext) return;
	
	using namespace Dream::Client::Display;
	
	NSSize frameSize = [self frame].size;
	Vec2u newSize(frameSize.width, frameSize.height);
	ResizeInput resizeInput(newSize);
	
	displayContext->process(resizeInput);
}

- (unsigned) buttonForEvent:(NSEvent *)event
{
	NSEventType t = [event type];
	
	if (t == NSLeftMouseDown || t == NSLeftMouseUp || t == NSLeftMouseDragged) {
		if ([event modifierFlags] & NSControlKeyMask)
			return MouseRightButton;
		
		return MouseLeftButton;
	}
	
	if (t == NSRightMouseDown || t == NSRightMouseUp || t == NSRightMouseDragged)
		return MouseRightButton;
	
	return [event buttonNumber];
}

- (BOOL) handleMouseEvent:(NSEvent *)event withButton:(unsigned)button
{
	Vec3 position, movement;
	AlignedBox<2> bounds(ZERO, ZERO);
	
	NSPoint curPoint = [event locationInWindow];
	curPoint = [self convertPoint:curPoint fromView:nil];
	
	position[X] = curPoint.x;
	position[Y] = curPoint.y;
	position[Z] = 0;
		
	movement[X] = [event deltaX];
	movement[Y] = [event deltaY];
	movement[Z] = [event deltaZ];
	
	StateT state;
	NSEventType t = [event type];
	
	if (t == NSLeftMouseDown || t == NSRightMouseDown || t == NSOtherMouseDown)
		state = Pressed;
	else if (t == NSLeftMouseDragged || t == NSRightMouseDragged || t == NSOtherMouseDragged)
		state = Dragged;
	else
		state = Released;
	
	bounds.setOrigin(Vec2(self.frame.origin.x, self.frame.origin.y));
	bounds.setSizeFromOrigin(Vec2(self.frame.size.width, self.frame.size.height));
	
	Key key(DefaultMouse, button);
	MotionInput motionInput(key, state, position, movement, bounds);
	
	return displayContext->process(motionInput);
}

- (BOOL) handleEvent:(NSEvent *)event
{
	//NSLog(@"Handling event: %@", event);

	if (!displayContext) return false;
	
	if ([event type] == NSKeyDown) {
		Key key(DefaultKeyboard, [[event characters] characterAtIndex:0]);
		ButtonInput buttonInput(key, Pressed);
		
		// If the key is processed, don't pass it on.
		return displayContext->process(buttonInput);
	} else if ([event type] == NSKeyUp) {
		Key key(DefaultKeyboard, [[event characters] characterAtIndex:0]);
		ButtonInput buttonInput(key, Released);
		
		// If the key is processed, don't pass it on.
		return displayContext->process(buttonInput);
	} else {
		switch ([event type]) {
			case NSLeftMouseDown:
			case NSLeftMouseUp:
			case NSLeftMouseDragged:
			case NSRightMouseDown:
			case NSRightMouseUp:
			case NSRightMouseDragged:
			case NSOtherMouseDown:
			case NSOtherMouseUp:
			case NSOtherMouseDragged:
				return [self handleMouseEvent:event withButton:[self buttonForEvent:event]];
			case NSScrollWheel:
				return [self handleMouseEvent:event withButton:MouseScroll];
			case NSMouseMoved:
				return [self handleMouseEvent:event withButton:NullButton];
			case NSMouseEntered:
				return [self handleMouseEvent:event withButton:MouseEntered];
			case NSMouseExited:
				return [self handleMouseEvent:event withButton:MouseExited];
		}
	}
	
	return false;
}

- (void) mouseDown: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) mouseDragged: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) mouseUp: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) mouseMoved: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) mouseEntered: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) mouseExited: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) rightMouseDown: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) rightMouseDragged: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) rightMouseUp: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) otherMouseDown: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) otherMouseDragged: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void) otherMouseUp: (NSEvent*)event
{
	[self handleEvent:event];
}

- (void)keyDown:(NSEvent *)event
{
	[self handleEvent:event];
}

- (void)keyUp:(NSEvent *)event
{
	[self handleEvent:event];
}

@end
