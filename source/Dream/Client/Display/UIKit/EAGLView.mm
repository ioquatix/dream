//
//  Client/Display/UIKit/EAGLView.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Samuel Williams 2009. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#include "EAGLView.h"
#include "../../../Events/Logger.h"

@interface EAGLView (Private)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

const int RENDER_THREAD_FINISHED = 1;

@implementation EAGLView

@synthesize context = _context;

// You must implement this method
+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (void)setup
{
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
	
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
									kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
									nil];
	
	// Check the resolution of the main screen to support high resolution devices.
	if([[UIScreen mainScreen] respondsToSelector:@selector(scale)]) {
		CGFloat scale = [[UIScreen mainScreen] scale];

		[self setContentScaleFactor:scale];
	}
}

-(id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];
	
	if (self) {
		[self setup];
	}
	
	return self;
}

//The EAGL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:.
- (id)initWithCoder:(NSCoder*)coder
{
    self = [super initWithCoder:coder];
	
	if (self) {
		[self setup];
    }
    
    return self;
}

- (void)dealloc
{
	[self stop];

    [self deleteFramebuffer];    
    [_context release];
    
    [super dealloc];
}

- (void)setContext:(EAGLContext *)newContext
{
    if (_context != newContext) {
        [self deleteFramebuffer];
        
        [_context release];
        _context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)createFramebuffer
{
    if (_context && !_default_framebuffer) {
        [EAGLContext setCurrentContext:_context];
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &_default_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _default_framebuffer);
        
        // Create color render buffer and allocate backing store.
        glGenRenderbuffers(1, &_color_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, _color_renderbuffer);
		
        [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &_backing_width);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &_backing_height);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _color_renderbuffer);
		
		// Depth buffer setup
		GLenum depth_format = GL_DEPTH_COMPONENT16;
		
		glGenRenderbuffers(1, &_depth_renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, _depth_renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, depth_format, _backing_width, _backing_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_renderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

- (void)deleteFramebuffer
{
    if (_context) {
        [EAGLContext setCurrentContext:_context];
        
        if (_default_framebuffer) {
            glDeleteFramebuffers(1, &_default_framebuffer);
            _default_framebuffer = 0;
        }
        
        if (_color_renderbuffer) {
            glDeleteRenderbuffers(1, &_color_renderbuffer);
            _color_renderbuffer = 0;
        }
		
		if (_depth_renderbuffer) {
			glDeleteRenderbuffers(1, &_depth_renderbuffer);
			_depth_renderbuffer = 0;
		}
    }
}

- (void)makeCurrentContext
{
    if (_context) {
        [EAGLContext setCurrentContext:_context];
        
		if (_resize_buffers) {
			[self deleteFramebuffer];
			_resize_buffers = NO;
		}
		
        if (!_default_framebuffer)
            [self createFramebuffer];
        
        glBindFramebuffer(GL_FRAMEBUFFER, _default_framebuffer);
        
        glViewport(0, 0, _backing_width, _backing_height);
    }
}

- (BOOL)flushBuffers
{
    BOOL success = FALSE;
    
    if (_context) {
        [EAGLContext setCurrentContext:_context];
        
        glBindRenderbuffer(GL_RENDERBUFFER, _color_renderbuffer);
        
        success = [_context presentRenderbuffer:GL_RENDERBUFFER];
    }
    
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    _resize_buffers = YES;
}

- (void) render:(CADisplayLink*)displayLink
{

}

- (void) _exit
{

}

- (void) _start
{
	// Lock the render process:
	[_render_thread_lock lock];
	
	Dream::Events::Logging::logger()->set_thread_name("Renderer");
	
	// Set the context for rendering operations:
	[self makeCurrentContext];
	
	// We shouldn't begin the rendering thread more than once.
	NSAutoreleasePool * pool = [NSAutoreleasePool new];	

	NSRunLoop * currentRunLoop = [NSRunLoop currentRunLoop];

	// Add display link
	CADisplayLink * displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render:)];
	[displayLink addToRunLoop:currentRunLoop forMode:NSRunLoopCommonModes];
	
	NSLog(@"Entering rendering loop.");
	
	NSThread * currentThread = [[NSThread currentThread] retain];
	while (![currentThread isCancelled]) {
		[currentRunLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
	}
	
	NSLog(@"Exiting rendering loop.");
	
	[displayLink invalidate];
	[currentThread release];
	[pool release];
	
	// Signal that we are done.
	[_render_thread_lock unlockWithCondition:RENDER_THREAD_FINISHED];
}

- (void) start
{
	if (!_render_thread_lock) {
		_render_thread_lock = [NSConditionLock new];
	}

	if (!_render_thread) {
		NSLog(@"Starting render thread...");
		_render_thread = [[NSThread alloc] initWithTarget:self selector:@selector(_start) object:nil];
		
		[_render_thread start];
	} else {
		NSLog(@"Render thread already running: %@", _render_thread);
	}
}

- (void) stop
{
	NSLog(@"Stopping render thread...");
	
	if (_render_thread) {
		[_render_thread cancel];
		
		// Send a notification via a mach port to the other thread's run loop. This causes the
		// runMode:beforeDate: method to return immediately, with the end result that the run-loop
		// is stopped.
		[self performSelector:@selector(_exit) onThread:_render_thread withObject:nil waitUntilDone:NO];
		
		// Wait for the render thread to finish.
		[_render_thread_lock lockWhenCondition:RENDER_THREAD_FINISHED];
		
		[_render_thread release];
		_render_thread = nil;
		
		[_render_thread_lock unlock];
	}
}

@end
