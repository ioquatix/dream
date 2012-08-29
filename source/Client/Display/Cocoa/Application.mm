//
//  Client/Display/Cocoa/Application.mm
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright (c) 2011 Samuel Williams. All rights reserved.
//

#include "Application.h"
#include "Context.h"
#include "DApplicationDelegate.h"

namespace Dream {
	namespace Client {
		namespace Display {
		
			void IApplication::start (Ptr<IApplicationDelegate> delegate)
			{
				Ref<Cocoa::Application> application = new Cocoa::Application(delegate);
				
				application->run();
			}
		
			namespace Cocoa {
				Application::Application(Ptr<IApplicationDelegate> applicationDelegate) : _application_delegate(applicationDelegate)
				{
				
				}
				
				Application::~Application ()
				{
				
				}
				
				NSString * application_name()
				{
					NSDictionary * dict;
					NSString * appName = 0;

					// Determine the application name
					dict = (NSDictionary *)CFBundleGetInfoDictionary(CFBundleGetMainBundle());
					if (dict)
						appName = [dict objectForKey:@"CFBundleName"];
					
					if (![appName length])
						appName = [[NSProcessInfo processInfo] processName];

					return appName;
				}

				void Application::create_application_menus()
				{					
					/* Create the main menu bar */
					[NSApp setMainMenu:[[NSMenu alloc] init]];

					/* Create the application menu */
					NSString * app_name = application_name();
					NSMenu * apple_menu = [[NSMenu alloc] initWithTitle:@""];
					
					NSString * title;
					NSMenuItem * menu_item;
					
					/* Add menu items */
					title = [@"About " stringByAppendingString:app_name];
					[apple_menu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];

					[apple_menu addItem:[NSMenuItem separatorItem]];

					title = [@"Hide " stringByAppendingString:app_name];
					[apple_menu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];

					menu_item = (NSMenuItem *)[apple_menu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
					[menu_item setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];

					[apple_menu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

					[apple_menu addItem:[NSMenuItem separatorItem]];

					title = [@"Quit " stringByAppendingString:app_name];
					[apple_menu addItemWithTitle:title action:@selector(terminate:) keyEquivalent:@"q"];
					
					/* Put menu into the menubar */
					menu_item = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
					[menu_item setSubmenu:apple_menu];
					[[NSApp mainMenu] addItem:menu_item];
					[menu_item release];

					/* Tell the application object that this is now the application menu */
					[NSApp performSelector:@selector(setAppleMenu:) withObject:apple_menu];
					// [NSApp setAppleMenu:apple_menu];
					[apple_menu release];

					/* Create the window menu */
					NSMenu * window_menu = [[NSMenu alloc] initWithTitle:@"Window"];
					
					// FullScreen item
					menu_item = [[NSMenuItem alloc] initWithTitle:@"Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"];
					[window_menu addItem:menu_item];
					[menu_item release];
					
					/* "Minimize" item */
					menu_item = [[NSMenuItem alloc] initWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
					[window_menu addItem:menu_item];
					[menu_item release];
					
					/* Put menu into the menubar */
					menu_item = [[NSMenuItem alloc] initWithTitle:@"Window" action:nil keyEquivalent:@""];
					[menu_item setSubmenu:window_menu];
					[[NSApp mainMenu] addItem:menu_item];
					[menu_item release];
					
					/* Tell the application object that this is now the window menu */
					[NSApp setWindowsMenu:window_menu];
					[window_menu release];
				}

				void Application::transform_to_foreground_application()
				{
					ProcessSerialNumber psn;
					
					if (!GetCurrentProcess(&psn)) {
						TransformProcessType(&psn, kProcessTransformToForegroundApplication);
						SetFrontProcess(&psn);
					}
				}
				
				void Application::run()
				{
					transform_to_foreground_application();

					[NSApplication sharedApplication];
					
					// Setup the application delegate wrapper:
					DApplicationDelegate * wrapper = [[DApplicationDelegate alloc] init];					
					[wrapper setApplication:this];
					
					[NSApp setDelegate:wrapper];

					create_application_menus();
										
					[NSApp run];
				}
				
				Ref<IContext> Application::create_context(Ref<Dictionary> config)
				{
					return new WindowContext(config);
				}
				
				IApplicationDelegate * Application::delegate () const
				{
					return _application_delegate.get();
				}
			}
		}
	}
}
