//
//  DOpenGLView.h
//  Dream
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#import <AppKit/AppKit.h>
#include "../Context.h"

@interface DOpenGLView : NSOpenGLView {
	Dream::Client::Display::Context * displayContext;
}

@property(nonatomic,assign) Dream::Client::Display::Context * displayContext;

- (BOOL) handleEvent:(NSEvent *)event;

@end
