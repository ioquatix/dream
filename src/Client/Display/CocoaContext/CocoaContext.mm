/*
 *  CocoaContext.mm
 *  Dream
 *
 *  Created by Samuel Williams on 27/03/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

// Exclude quickdraw.
#define __INCLUDE_QD__

#include "Application.h"
#include "Context.h"

#include "CocoaContext.h"

@interface NSAppleMenuController : NSObject
- (void)controlMenu:(NSMenu *)aMenu;
@end

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			
#pragma mark -
#pragma mark Modes
			
			class CocoaContextMode : public Object, implements IContextMode {
			public:
				CocoaContextMode();
				virtual ~CocoaContextMode();
				
				virtual StringT descriptiveName () const;
				
				virtual REF(IContext) setup (PTR(Dictionary) config) const;
			};
			
			CocoaContextMode::CocoaContextMode() {
			
			}
			
			CocoaContextMode::~CocoaContextMode() {
				
			}
			
			StringT CocoaContextMode::descriptiveName () const {
				return "Cocoa Window";
			}
			
			REF(IContext) CocoaContextMode::setup (PTR(Dictionary) config) const {
				return new CocoaContext(config);
			}
			
			CocoaContext::Modes CocoaContext::s_modes;

			CocoaContext::Modes::Modes () {
				ContextManager * manager = ContextManager::sharedManager();
				
				// NSArray * screens = [NSScreen screens];
				manager->registerContextMode(new CocoaContextMode());
			}

#pragma mark -
#pragma mark Application
			
			void IApplication::start (PTR(Dictionary) config)
			{
				setup();
				run();
			}
			
#pragma mark -
#pragma mark Renderer Subclass
						
#pragma mark -
						
			// This is the renderer output callback function
			CVReturn CocoaContext::displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
			{
				CocoaContext * ctx = (CocoaContext*)displayLinkContext;
				
				return ctx->displayLinkCallback(displayLink, now, outputTime, flagsIn, flagsOut);
			}
			
			// This is the renderer output callback function
			CVReturn CocoaContext::displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut)
			{
				if (m_displayWillRefresh == true) {
					m_displayRefreshStallCount += 1;
					
					if (m_displayRefreshStallCount % 500 == 100)
						std::cerr << __func__ << ": Pipeline stalled (" << m_displayRefreshStallCount << ")..." << std::endl;
				} else {
					// Only warn when stall is large
					if (m_displayRefreshStallCount > 5)
						std::cerr << __func__ << ": Display stalled for " << m_displayRefreshStallCount << " frame(s)" << std::endl;
					
					m_displayRefreshStallCount = 0;
					m_displayWillRefresh = true;
				}
				
				TimeT time = (TimeT)(outputTime->hostTime) / (TimeT)CVGetHostClockFrequency();			
				ensure(m_notificationSource);
				m_notificationSource->frameCallback(m_loop.get(), time);
				
				return 0;
			}
			
			NSString * CocoaContext::applicationName()
			{
				NSDictionary * dict;
				NSString * appName = 0;

				/* Determine the application name */
				dict = (NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
				if (dict)
					appName = [dict objectForKey:@"CFBundleName"];
				
				if (![appName length])
					appName = [[NSProcessInfo processInfo] processName];

				return appName;
			}

			void CocoaContext::createApplicationMenus()
			{
				NSString *appName;
				NSString *title;
				NSMenu *appleMenu;
				NSMenu *windowMenu;
				NSMenuItem *menuItem;
				
				/* Create the main menu bar */
				[NSApp setMainMenu:[[NSMenu alloc] init]];

				/* Create the application menu */
				appName = applicationName();
				appleMenu = [[NSMenu alloc] initWithTitle:@""];
				
				/* Add menu items */
				title = [@"About " stringByAppendingString:appName];
				[appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

				[appleMenu addItem:[NSMenuItem separatorItem]];

				title = [@"Hide " stringByAppendingString:appName];
				[appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];

				menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
				[menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

				[appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

				[appleMenu addItem:[NSMenuItem separatorItem]];

				title = [@"Quit " stringByAppendingString:appName];
				[appleMenu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
				
				/* Put menu into the menubar */
				menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
				[menuItem setSubmenu:appleMenu];
				[[NSApp mainMenu] addItem:menuItem];
				[menuItem release];

				/* Tell the application object that this is now the application menu */
				[NSApp performSelector:@selector(setAppleMenu:) withObject:appleMenu];
				// [NSApp setAppleMenu:appleMenu];
				[appleMenu release];

				/* Create the window menu */
				windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
				
				// FullScreen item
				menuItem = [[NSMenuItem alloc] initWithTitle:@"Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"];
				[windowMenu addItem:menuItem];
				[menuItem release];
				
				/* "Minimize" item */
				menuItem = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
				[windowMenu addItem:menuItem];
				[menuItem release];
				
				/* Put menu into the menubar */
				menuItem = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
				[menuItem setSubmenu:windowMenu];
				[[NSApp mainMenu] addItem:menuItem];
				[menuItem release];
				
				/* Tell the application object that this is now the window menu */
				[NSApp setWindowsMenu:windowMenu];
				[windowMenu release];
			}

			void CocoaContext::transformToForegroundApplication()
			{
				ProcessSerialNumber psn;
				
				if (!GetCurrentProcess(&psn)) {
					TransformProcessType(&psn, kProcessTransformToForegroundApplication);
					SetFrontProcess(&psn);
				}
			}
			
			void CocoaContext::setTitle(String title) {
				NSString * windowTitle = [[NSString alloc] initWithBytes:title.data() length:title.length() encoding:NSUTF8StringEncoding];
				
				[m_window setTitle:windowTitle];

				[windowTitle release];
			}
			
			void CocoaContext::show() {
				[m_window makeKeyAndOrderFront:nil];
			}
			
			void CocoaContext::hide() {
				//[m_window close];
			}
			
			void CocoaContext::setFrameSync(bool vsync) {
				GLint swapInt = vsync ? 1 : 0;
				
				std::cerr << "VSync = " << vsync << std::endl;
				
				[[m_view openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
			}
			
			CocoaContext::CocoaContext(PTR(Dictionary) config) 
				: m_displayWillRefresh(false), m_displayRefreshStallCount(0) {
				m_pool = [[NSAutoreleasePool alloc] init];

				[NSApplication sharedApplication];
				
				NSWindow * configWindow = NULL;
				if (config->get("CocoaContext.Window", configWindow)) {
					m_window = configWindow;
				} else {
					transformToForegroundApplication();
										
					if ([NSApp mainMenu] == nil)
						createApplicationMenus();
				
					NSRect windowSize = NSMakeRect(50, 50, 1024, 768);
					unsigned windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

					m_window = [[NSWindow alloc] initWithContentRect:windowSize styleMask:windowStyle backing:NSBackingStoreBuffered defer:YES];
					[m_window setAcceptsMouseMovedEvents:YES];
					
					m_delegate = [CocoaContextDelegate new];
					[[m_delegate screenManager] setPartialScreenWindow:m_window];
					
					if ([NSApp delegate] == nil) {
						[NSApp setDelegate:m_delegate];
					}
					
					[m_window setDelegate:m_delegate];
					
					[NSApp finishLaunching];
				}
				
				NSOpenGLView * graphicsView = NULL;
				if (config->get("CocoaContext.View", graphicsView)) {
					m_view = graphicsView;
				} else {
					NSOpenGLPixelFormatAttribute attribs[] = {
						NSOpenGLPFAWindow,
						NSOpenGLPFADoubleBuffer,	// double buffered
						NSOpenGLPFAAccelerated,
						NSOpenGLPFANoRecovery,
						/* Anti-aliasing */
						NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)3, /* 3 looks awsome */
						NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)4,
						/* --- --- --- */
						NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)24,
						NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)32,
						(NSOpenGLPixelFormatAttribute)nil
					};

					NSOpenGLPixelFormat * pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
					
					graphicsView = [[[NSOpenGLView alloc] initWithFrame:[[m_window contentView] frame] pixelFormat:pf] autorelease];
					m_view = graphicsView;
					
					[graphicsView setAutoresizingMask:NSViewHeightSizable | NSViewWidthSizable];				
					
					[m_window setContentView:graphicsView];
				}
				
				// ***** SETUP CVDisplayLink *****
				// Synchronize buffer swaps with vertical refresh rate
				setFrameSync(true);
				
				// Create a display link capable of being used with all active displays
				CVDisplayLinkCreateWithActiveCGDisplays(&m_displayLink);
				
				// Set the renderer output callback function
				CVDisplayLinkSetOutputCallback(m_displayLink, &CocoaContext::displayLinkCallback, this);
				
				// Set the display link for the current renderer
				CGLContextObj cglContext = (CGLContextObj)[[graphicsView openGLContext] CGLContextObj];
				CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[graphicsView pixelFormat] CGLPixelFormatObj];
				CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(m_displayLink, cglContext, cglPixelFormat);
				
				// OpenGL Defaults:
				glEnable(GL_DEPTH_TEST);
				
				// Tight alignment
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				
				//glPolygonOffset (1.0f, 1.0f);
				
				m_renderer = new MacOSXOpenGLRenderer();
				
				std::cerr << "OpenGL Context Initialized..." << std::endl;
				std::cerr << "OpenGL Vendor: " << glGetString(GL_VENDOR) << std::endl;
				std::cerr << "OpenGL Renderer: " << glGetString(GL_RENDERER) << " " << glGetString(GL_VERSION) << std::endl;
			}
			
			CocoaContext::~CocoaContext () {
				CVDisplayLinkStop(m_displayLink);
				CVDisplayLinkRelease(m_displayLink);
				
				[m_window release];
				[m_pool release];				
			}
			
			unsigned CocoaContext::buttonFromEvent(NSEvent * theEvent)
			{
				NSEventType t = [theEvent type];
								
				if (t == NSLeftMouseDown || t == NSLeftMouseUp || t == NSLeftMouseDragged) {
					if ([theEvent modifierFlags] & NSControlKeyMask)
						return MouseRightButton;
					
					return MouseLeftButton;
				}
				
				if (t == NSRightMouseDown || t == NSRightMouseUp || t == NSRightMouseDragged)
					return MouseRightButton;
				
				return [theEvent buttonNumber];
			}
			
			bool CocoaContext::handleMouseEvent(NSEvent * theEvent, unsigned button, IInputHandler * handler)
			{
				Vec3 position, movement;
				AlignedBox<2> bounds(ZERO, ZERO);
				
				NSPoint curPoint = [theEvent locationInWindow];
				curPoint = [m_view convertPoint:curPoint fromView:nil];
				
				position[X] = curPoint.x;
				position[Y] = curPoint.y;
				position[Z] = 0;
								
				movement[X] = [theEvent deltaX];
				movement[Y] = [theEvent deltaY];
				movement[Z] = [theEvent deltaZ];
				
				StateT state;
				NSEventType t = [theEvent type];
				
				if (t == NSLeftMouseDown || t == NSRightMouseDown || t == NSOtherMouseDown)
					state = Pressed;
				else if (t == NSLeftMouseDragged || t == NSRightMouseDragged || t == NSOtherMouseDragged)
					state = Dragged;
				else
					state = Released;
				
				// Determine if curPoint is based in the window coordinates or screen coordinates.
				// http://www.cocoabuilder.com/archive/cocoa/104529-current-mouse-screen.html
				NSWindow * sourceWindow = [theEvent window];
				if (sourceWindow != nil) {
					// In this case we assume that the point is in the view's coordinate system by now.
					// It might be useful to augment the event handling system in the future to handle multiple
					// viewports, but for now lets just assument that events are handled via a single view.
					bounds.setOrigin(Vec2(m_view.frame.origin.x, m_view.frame.origin.y));
					bounds.setSizeFromOrigin(Vec2(m_view.frame.size.width, m_view.frame.size.height));
										
					// Get the window frame and set the input bounds appropriately.
					//NSRect windowFrame = [sourceWindow frame];
					
					//bounds.setOrigin(Vec2(windowFrame.origin.x, windowFrame.origin.y));
					//bounds.setSizeFromOrigin(Vec2(windowFrame.size.width, windowFrame.size.height));
				} else {
					// Get the screen frame and set the input bounds appropriately.
					
					std::cerr << "Mouse motion captured outside window: " << position << " (event ignored)." << std::endl;
					
					/*
					for (NSScreen * screen in [NSScreen screens]) {
						if (NSMouseInRect(curPoint, [screen frame], NO)) {
							NSRect screenFrame = [screen frame];
							
							bounds.setOrigin(Vec2(screenFrame.origin.x, screenFrame.origin.y));
							bounds.setSizeFromOrigin(Vec2(screenFrame.size.width, screenFrame.size.height));
							
							break;
						}
					}
					*/
					
					return false;
				}
				
				Key key(DefaultMouse, button);
				MotionInput motionInput(key, state, position, movement, bounds);
				
				return handler->process(motionInput);
			}
			
			ResolutionT CocoaContext::resolution ()
			{
				NSSize size = [m_view frame].size;
				
				return ResolutionT(size.width, size.height);
			}
			
			void CocoaContext::flipBuffers ()
			{
				//if (m_displayWillRefresh != true)
				//	std::cerr << __func__ << ": Frame will not be displayed!" << std::endl;
				
				m_displayWillRefresh = false;
				
				[[m_view openGLContext] flushBuffer];
			}
			
			void CocoaContext::processPendingEvents (IInputHandler * handler)
			{
				CocoaContextDelegate * delegate = m_delegate;
				[delegate setInputHandler:handler];
			
				while (true) {
					NSEvent * e = [[NSApplication sharedApplication] nextEventMatchingMask:NSAnyEventMask untilDate:nil inMode:NSDefaultRunLoopMode dequeue:YES];					
					if (e == NULL) return;
					
					bool consumed = false;
				
					NSView * v = [m_view hitTest:[e locationInWindow]];
				
					if ([e type] == NSKeyDown) {
						Key key(DefaultKeyboard, [[e characters] characterAtIndex:0]);
						ButtonInput ipt(key, Pressed);
						
						// If the key is processed, don't pass it on.
						if (handler->process(ipt))
							continue;
						
						consumed = true;
					} else if ([e type] == NSKeyUp) {
						Key key(DefaultKeyboard, [[e characters] characterAtIndex:0]);
						ButtonInput ipt(key, Released);
						
						// If the key is processed, don't pass it on.
						if (handler->process(ipt))
							continue;
						
						consumed = true;
					} else if (v) {
						switch ([e type]) {
							case NSLeftMouseDown:
							case NSLeftMouseUp:
							case NSLeftMouseDragged:
							case NSRightMouseDown:
							case NSRightMouseUp:
							case NSRightMouseDragged:
							case NSOtherMouseDown:
							case NSOtherMouseUp:
							case NSOtherMouseDragged:
								if (handleMouseEvent(e, buttonFromEvent(e), handler)) continue;
								consumed = true;
								break;
							case NSScrollWheel:
								if (handleMouseEvent(e, MouseScroll, handler)) continue;
								consumed = true;
								break;
							case NSMouseMoved:
								if (handleMouseEvent(e, NullButton, handler)) continue;
								consumed = true;
								break;
							case NSMouseEntered:
								if (handleMouseEvent(e, MouseEntered, handler)) continue;
								consumed = true;
								break;
							case NSMouseExited:
								if (handleMouseEvent(e, MouseExited, handler)) continue;
								consumed = true;
								break;
						}
					}
					
					// Event Debugging
					//if (!consumed)
					//	NSLog(@"Event Not Consumed: %@", e);
					
					[NSApp sendEvent:e];
				}
				
				[delegate setInputHandler:nil];
			}
		
			void CocoaContext::scheduleFrameNotificationCallback (REF(Events::Loop) loop, FrameCallbackT callback)
			{
				// We stop the display link initially,
				CVDisplayLinkStop(m_displayLink);
				
				if (callback) {
					// Setup the notification source
					if (!m_notificationSource)
						m_notificationSource = new FrameNotificationSource(callback);
					else
						m_notificationSource->setCallback(callback);
				
					m_loop = loop;
					m_displayWillRefresh = false;
					
					// Start the display link
					CVDisplayLinkStart(m_displayLink);
				}					
			}
			
			REF(CocoaContext::MacOSXOpenGLRenderer) CocoaContext::renderer ()
			{
				return m_renderer;
			}

		}
	}
}
