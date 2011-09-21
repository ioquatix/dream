//
//  EAGLView.m
//  Death Ball Touch
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Orion Transfer Ltd 2009. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "EAGLView.h"

@interface EAGLView (Private)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

const int RENDER_THREAD_FINISHED = 1;

@implementation EAGLView

@synthesize context;

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
    [context release];
    
    [super dealloc];
}

- (void)setContext:(EAGLContext *)newContext
{
    if (context != newContext) {
        [self deleteFramebuffer];
        
        [context release];
        context = [newContext retain];
        
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)createFramebuffer
{
    if (context && !defaultFramebuffer) {
        [EAGLContext setCurrentContext:context];
        
        // Create default framebuffer object.
        glGenFramebuffers(1, &defaultFramebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        // Create color render buffer and allocate backing store.
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		
        [context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
		
		// Depth buffer setup
		GLenum depthFormat = GL_DEPTH_COMPONENT16;
		
		glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, backingWidth, backingHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
}

- (void)deleteFramebuffer
{
    if (context) {
        [EAGLContext setCurrentContext:context];
        
        if (defaultFramebuffer) {
            glDeleteFramebuffers(1, &defaultFramebuffer);
            defaultFramebuffer = 0;
        }
        
        if (colorRenderbuffer) {
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            colorRenderbuffer = 0;
        }
		
		if (depthRenderbuffer) {
			glDeleteRenderbuffers(1, &depthRenderbuffer);
			depthRenderbuffer = 0;
		}
    }
}

- (void)makeCurrentContext
{
    if (context) {
        [EAGLContext setCurrentContext:context];
        
		if (resizeBuffers) {
			[self deleteFramebuffer];
			resizeBuffers = NO;
		}
		
        if (!defaultFramebuffer)
            [self createFramebuffer];
        
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
        
        glViewport(0, 0, backingWidth, backingHeight);
    }
}

- (BOOL)flushBuffers
{
    BOOL success = FALSE;
    
    if (context) {
        [EAGLContext setCurrentContext:context];
        
        glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
        
        success = [context presentRenderbuffer:GL_RENDERBUFFER];
    }
    
    return success;
}

- (void)layoutSubviews
{
    // The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
    resizeBuffers = YES;
}

- (void) render:(CADisplayLink*)displayLink
{

}

- (void) _exit
{

}

- (void) _start
{
	// Lock the render process
	[renderThreadLock lock];
	
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
	[renderThreadLock unlockWithCondition:RENDER_THREAD_FINISHED];
}

- (void) start
{
	if (!renderThreadLock) {
		renderThreadLock = [NSConditionLock new];
	}

	if (!renderThread) {
		NSLog(@"Starting render thread...");
		renderThread = [[NSThread alloc] initWithTarget:self selector:@selector(_start) object:nil];
		
		[renderThread start];
	} else {
		NSLog(@"Render thread already running: %@", renderThread);
	}
}

- (void) stop
{
	NSLog(@"Stopping render thread...");
	
	if (renderThread) {
		[renderThread cancel];
		
		// Send a notification via a mach port to the other thread's run loop. This causes the
		// runMode:beforeDate: method to return immediately, with the end result that the run-loop
		// is stopped.
		[self performSelector:@selector(_exit) onThread:renderThread withObject:nil waitUntilDone:NO];
		
		// Wait for the render thread to finish.
		[renderThreadLock lockWhenCondition:RENDER_THREAD_FINISHED];
		
		[renderThread release];
		renderThread = nil;
		
		[renderThreadLock unlock];
	}
}

@end
