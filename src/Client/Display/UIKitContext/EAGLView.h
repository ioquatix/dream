/*
 *  Client/Display/UIKitContext/EAGLView.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 18/04/09.
 *  Copyright Orion Transfer Ltd 2009. All rights reserved.
 *
 */

// This is a private header, and should not be used as public API.

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

enum EAGLViewOpenGLVersion {
	OPENGLES_11 = 1,
	OPENGLES_20 = 2
};

/*
This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
The view content is basically an EAGL surface you render your OpenGL scene into.
Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
*/
@interface EAGLView : UIView {
    /* The pixel dimensions of the backbuffer */
    GLint backingWidth;
    GLint backingHeight;
    
    EAGLContext *context;
    
    /* OpenGL names for the renderbuffer and framebuffers used to render to this view */
	GLuint defaultFramebuffer, colorRenderbuffer;
    
    /* OpenGL name for the depth buffer that is attached to viewFramebuffer, if it exists (0 if it does not exist) */
    GLuint depthRenderbuffer;
}

- (id)initWithFrame:(CGRect)frame version:(EAGLViewOpenGLVersion)version;

@property (nonatomic, retain) EAGLContext *context;

- (void) flipBuffers;

@end
