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

@synthesize displayContext = _display_context;

- (BOOL)acceptsFirstResponder
{
	return YES;
}

- (BOOL)becomeFirstResponder {
	EventInput event_input(EventInput::RESUME);
	
	_display_context->process(event_input);
	
	return YES;
}

- (BOOL)resignFirstResponder {
	EventInput event_input(EventInput::PAUSE);
	
	_display_context->process(event_input);
	
	_skip_next_motion_event = NO;
	
	return YES;
}

- (void) warpCursorToCenter {
	// Warp the mouse cursor to the center of the view.
	NSRect bounds = self.bounds;
	NSPoint view_center = NSMakePoint(bounds.origin.x + bounds.size.width / 2.0, bounds.origin.y + bounds.size.height / 2.0);
	NSPoint window_center = [self convertPoint:view_center toView:nil];
	
	NSRect window_center_rect = {window_center, {0, 0}};
	NSRect screen_offset_rect = [self.window convertRectToScreen:window_center_rect];
	NSPoint screen_offset = screen_offset_rect.origin;
	//NSLog(@"Screen offset: %@", NSStringFromPoint(screen_offset));
	
	NSScreen * screen = self.window.screen;
	//NSLog(@"Screen frame: %@", NSStringFromRect(screen.frame));
	
	CGFloat top = screen.frame.origin.y + screen.frame.size.height;
	CGPoint new_cursor_position = CGPointMake(screen_offset.x, top - screen_offset.y);
	//NSLog(@"Cursor position: %@", NSStringFromPoint((NSPoint){new_cursor_position.x, new_cursor_position.y}));
	
	// This is a horrible hack because CGWarpMouseCursorPosition causes a spurious motion event delta:
	_skip_next_motion_event = YES;
	
	CGWarpMouseCursorPosition(new_cursor_position);
}

- (void)reshape
{	
	[super reshape];
	
	if (!_display_context) return;
	
	logger()->log(LOG_DEBUG, LogBuffer() << "Reshape for context: " << self.openGLContext.CGLContextObj);
	
	using namespace Dream::Client::Display;
	
	NSSize frame_size = [self frame].size;
	Vec2u new_size(frame_size.width, frame_size.height);
	ResizeInput resize_input(new_size);
	
	_display_context->process(resize_input);
	
	// The renderer needs to render a frame since the view size has changed.
	_display_context->wait_for_refresh();
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

- (BOOL) handleMouseEvent:(NSEvent *)event withButton:(Dream::Events::ButtonT)button
{
	NSEventMask mouse_motion_mask = NSMouseMovedMask | NSLeftMouseDraggedMask | NSRightMouseDraggedMask | NSOtherMouseDraggedMask;
	//NSLog(@"Handling mouse event: %@", event);
	
	Vec3 position, movement;
	AlignedBox<2> bounds(ZERO, ZERO);
	
	NSPoint window_point = [event locationInWindow];
	
	// Convert the point from window base coordinates to view coordinates
	NSPoint current_point = [self convertPoint:window_point fromView:nil];
	
	// This is due to a bug in CGWarpMouseCursorPosition - what a horrible hack.
	if (_skip_next_motion_event && (NSEventMaskFromType([event type]) & mouse_motion_mask)) {		
		_skip_next_motion_event = NO;
		return YES;
	}
	
	position[X] = current_point.x;
	position[Y] = current_point.y;
	position[Z] = 0;
		
	if (button == MouseScroll && [event hasPreciseScrollingDeltas]) {
		movement[X] = [event scrollingDeltaX];
		movement[Y] = [event scrollingDeltaY];
		movement[Z] = [event deltaZ];
	} else {
		movement[X] = [event deltaX];
		// Strictly speaking, this isn't completely correct. A change in position in the view's coordinates would be more accurate, but this isn't so easy
		// to implement with a disassociated cursor. So, we assume that the mouse coordinates and view coordinates have inverse-y and reverse the delta appropriately.
		movement[Y] = -[event deltaY];
		movement[Z] = [event deltaZ];
	}
	
	StateT state;
	NSEventType event_type = [event type];
	
	if (button == MouseScroll) {
		// http://developer.apple.com/library/mac/#releasenotes/Cocoa/AppKit.html
		switch ([event momentumPhase]) {
			case NSEventPhaseNone:
			case NSEventPhaseChanged:
				state = Pressed;
				break;
				
			default:
				state = Released;
				break;
		}
	} else {
		if (event_type == NSLeftMouseDown || event_type == NSRightMouseDown || event_type == NSOtherMouseDown)
			state = Pressed;
		else if (event_type == NSLeftMouseDragged || event_type == NSRightMouseDragged || event_type == NSOtherMouseDragged)
			state = Dragged;
		else
			state = Released;
	}
	
	// The mouse point is relative to the frame of the view.
	bounds.set_origin(Vec2(self.frame.origin.x, self.frame.origin.y));
	bounds.set_size_from_origin(Vec2(self.frame.size.width, self.frame.size.height));
	
	Key key(DefaultMouse, button);
	MotionInput motion_input(key, state, position, movement, bounds);
	
	return _display_context->process(motion_input);
}

- (BOOL) handleEvent:(NSEvent *)event
{
	//NSLog(@"Handling event: %@", event);

	if (!_display_context) return false;
	
	if ([event type] == NSKeyDown) {
		Key key(DefaultKeyboard, [[event characters] characterAtIndex:0]);
		ButtonInput buttonInput(key, Pressed);
		
		// If the key is processed, don't pass it on.
		return _display_context->process(buttonInput);
	} else if ([event type] == NSKeyUp) {
		Key key(DefaultKeyboard, [[event characters] characterAtIndex:0]);
		ButtonInput buttonInput(key, Released);
		
		// If the key is processed, don't pass it on.
		return _display_context->process(buttonInput);
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
			case NSMouseMoved:
				return [self handleMouseEvent:event withButton:[self buttonForEvent:event]];
				
			case NSScrollWheel:
				return [self handleMouseEvent:event withButton:MouseScroll];
				
			case NSMouseEntered:
				return [self handleMouseEvent:event withButton:MouseEntered];
			case NSMouseExited:
				return [self handleMouseEvent:event withButton:MouseExited];
		}
	}
	
	return false;
}

- (void) scrollWheel:(NSEvent *)event
{
	[self handleEvent:event];
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

/*
- (IBAction)toggleFullScreen:(id)sender {
	if ([self isInFullScreenMode]) {
		[self exitFullScreenModeWithOptions:nil];
		
		// When the window returns to normal size, the view doesn't seem to have first responder status. We ensure that it is the first responder so that mouse events are tracked properly.
		[[self window] makeFirstResponder:self];
	} else {
		[self enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
	}
	
	[self reshape];
}
 */

@end
