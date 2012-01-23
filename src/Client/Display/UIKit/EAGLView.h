//
//  Client/Display/UIKit/EAGLView.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 18/04/09.
//  Copyright Samuel Williams 2009. All rights reserved.
//
//

// This is a private header, and should not be used as public API.

#import <UIKit/UIKit.h>

#include "Renderer.h"

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView {
    /* The pixel dimensions of the backbuffer */
	BOOL resizeBuffers;
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
	GLuint defaultFramebuffer, colorRenderbuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
	
	// The display link and associated render thread.
	NSThread * renderThread;
	NSConditionLock * renderThreadLock;
}

@property (nonatomic, retain) EAGLContext *context;

// Internally used to setup the view - called during object construction for both initWithFrame: and initWithCoder:
- (void) setup;

- (void) makeCurrentContext;
- (BOOL) flushBuffers;

- (void) start;
- (void) stop;

@end
