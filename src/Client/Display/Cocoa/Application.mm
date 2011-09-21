//
//  Application.mm
//  Dream
//
//  Created by Samuel Williams on 14/09/11.
//  Copyright 2011 Orion Transfer Ltd. All rights reserved.
//

#include "Application.h"
#include "Context.h"
#include "DApplicationDelegate.h"

namespace Dream {
	namespace Client {
		namespace Display {
		
			void IApplication::start (PTR(IApplicationDelegate) delegate)
			{
				REF(Cocoa::Application) application = new Cocoa::Application(delegate);
				
				application->run();
			}
		
			namespace Cocoa {
				Application::Application(PTR(IApplicationDelegate) applicationDelegate)
					: m_applicationDelegate(applicationDelegate)
				{
				
				}
				
				Application::~Application ()
				{
				
				}
				
				NSString * applicationName()
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

				void Application::createApplicationMenus()
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

				void Application::transformToForegroundApplication()
				{
					ProcessSerialNumber psn;
					
					if (!GetCurrentProcess(&psn)) {
						TransformProcessType(&psn, kProcessTransformToForegroundApplication);
						SetFrontProcess(&psn);
					}
				}
				
				void Application::run()
				{
					transformToForegroundApplication();

					[NSApplication sharedApplication];
					
					// Setup the application delegate wrapper:
					DApplicationDelegate * wrapper = [[DApplicationDelegate alloc] init];
					
					[wrapper setApplication:this];
					[wrapper setApplicationDelegate:m_applicationDelegate.get()];
					
					[NSApp setDelegate:wrapper];

					createApplicationMenus();
										
					[NSApp run];
				}
				
				REF(IContext) Application::createContext (REF(Dictionary) config)
				{
					return new WindowContext(config);
				}
				
				IApplicationDelegate * Application::delegate () const
				{
					return m_applicationDelegate.get();
				}
			}
		}
	}
}
