/*
 *  Client/Graphics/View.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 23/07/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_VIEW_H
#define _DREAM_CLIENT_GRAPHICS_VIEW_H

#include "Graphics.h"
#include "../../Numerics/Vector.h"
#include "../../Numerics/Matrix.h"
#include "../../Geometry/Line.h"
#include "../../Geometry/AlignedBox.h"
#include "../Display/Input.h"
#include "../Display/Layer.h"
#include "../Text/Font.h"
#include "../Text/TextBuffer.h"

#include "TexturedQuadRenderer.h"
#include <boost/function.hpp>

namespace Dream {
	namespace Client {
		namespace Graphics {			
			
			/** Implements the basic structure of user interface layout and display.
			*/
			class View : public Object, IMPLEMENTS(Layer) {	
				EXPOSE_CLASS(View)
			
				class Class : public Object::Class, IMPLEMENTS(Layer::Class) {
				public:
					EXPOSE_CLASSTYPE
				};
			
			public:
				/** The top level user interface controller.
				*/
				class Controller : public Object, IMPLEMENTS(Layer) {
					EXPOSE_CLASS(Controller)
					
					class Class : public Object::Class, IMPLEMENTS(Layer::Class) {
					public:
						EXPOSE_CLASSTYPE
					};
					
				protected:
					bool m_debug;
					
					REF(View) m_principal;
					View *m_staticFocusedView, *m_dynamicFocusedView;
					
					void viewsToPoint (const Vec2 &point, std::vector<View*> &views);
					void setPrincipalView (View *p);
					
					friend class View;
				public:
					Controller ();
					virtual ~Controller ();
					
					virtual bool resize (const ResizeInput &);
					virtual bool button (const ButtonInput &);
					virtual bool motion (const MotionInput &);
						
					/// Returns the top level view.
					REF(View) principalView () { return m_principal; }
					const REF(View) principalView () const { return m_principal; }
					
					/// Set the current dynamically focused view.
					/// This is normally determined by the location of the mouse or pointer device.
					void setDynamicFocusedView(View* view);
					
					/// Get the currently dynamically focused view.
					View* dynamicFocusedView() const { return m_dynamicFocusedView; }
					
					// Set the current statically focused view.
					/// This is normally determined by a click of the mouse or keyboard.
					void setStaticFocusedView(View* view);
					
					/// Get the current statically focused view.
					View* staticFocusedView() const { return m_staticFocusedView; }
					
					virtual void renderFrameForTime (IScene * scene, TimeT time);
					
					virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
					virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
					
					virtual void dumpStructure (std::ostream & outp);
					
					/// Enable debugging of user interface layout.
					/// Borders are drawn around all user interface elements.
					void setDebugMode (bool enabled);
					
					/// Get the current debug mode.
					/// @returns true if debug mode enabled.
					bool debugMode ();
				};
				
			private:
				// This is used for setting up the parent <=> child relationship
				void registerChildView (REF(View) child);
				void viewsToPoint (const Vec2 &point, std::vector<View*> &views, bool first = true);
				
			protected:
				bool m_enabled;
				PTR(Controller) m_controller;
				PTR(View) m_parent;
				
				std::vector<REF(View)> m_subviews;
				
				Vec2 m_size, m_scale;
				Vec2 m_offset, m_orientation;
				StringT m_tag;
				
				AlignedBox<2> m_bounds;
				RealT m_rotation;
				
				//RealT m_boundsRotation, m_frameRotation;
				
				virtual bool intersectsWith (const Vec2 &point);
			
				virtual void renderView (IScene * scene, TimeT time);
			
			private:
				void init ();
				
			public:
				/// Initialize the view inside a parent view.
				View (PTR(View) parent);
				
				/// Initialize the view as the top level view in a controller.
				View (PTR(Controller) controller);
				
				/// Initialize the view as a top level view and set its bounding box.
				View (PTR(Controller) controller, const AlignedBox<2> &bounds);
				
				virtual ~View();
				
				/// Return the top level controller.
				virtual PTR(Controller) controller () { return m_controller; }
				virtual const PTR(Controller) controller () const { return m_controller; }
				
				/// @returns true if the view is the top level view in the heirarchy.
				bool isPrincipalView () const;
				
				/// Set the size and scale of the view.
				/// The size is measured in units (typically pixels), while scale is specified using
				/// a percentage of the containing view.
				void setSizeAndScale (const Vec2 &size, const Vec2 &scale);
				
				/// Set the offset and orientation of the view.
				/// The offset is measured in units (typically pixels), while the orientation is
				/// specified as a percentage location in the containing view.
				/// @sa AlignedBox::alignWithinSuperBox
				void setOffsetAndOrientation (const Vec2 &offset, const Vec2 &orientation);
				
				/// Set the rotation of the view box relative to the containing view.
				void setRotation (const RealT rotation);
				
				virtual const PTR(View) parentView () const { return m_parent; }
				virtual PTR(View) parentView () { return m_parent; }
				
				const std::vector<REF(View)> & childrenViews () const { return m_subviews; }
				std::vector<REF(View)> & childrenViews () { return m_subviews; }
				
				// Viewport in normalized coordinates 0.0 ... 1.0
				virtual const AlignedBox<2> & bounds () const { return m_bounds; }
				virtual RealT rotation () const { return m_rotation; }
				
				// Location with reference to superframe in normalized coordinates
				//virtual const AlignedBox<2> & frame () const { return m_frame; }
				//virtual RealT frameRotation () const { return m_frameRotation; }
				
				// Input Processing
				virtual bool resize (const ResizeInput &);
				virtual bool button (const ButtonInput &);
				virtual bool motion (const MotionInput &);
				
				virtual void renderFrameForTime (IScene * scene, TimeT time);
				virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
				virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
				
				bool hasDynamicFocus () const;
				bool hasStaticFocus () const;
				
				// Enabled / Disabled - whether it will respond to events and propagate events
				bool isEnabled () const;
				virtual void enable ();
				virtual void disable ();
				
				void setTag (const StringT & tag) { m_tag = tag; }
				StringT tag () const { return m_tag; }
				
				virtual void dumpStructure (std::ostream & outp, unsigned indent = 0);
			};
			
			class ImageView : public View {
				EXPOSE_CLASS(ImageView)
			
				class Class : public View::Class {
					EXPOSE_CLASSTYPE
				};
			
			protected:
				REF(IPixelBuffer) m_defaultImage;
				REF(IPixelBuffer) m_dynamicFocusImage;
				REF(IPixelBuffer) m_staticFocusImage;
				
				REF(IPixelBuffer) m_lastImage;
				
				PixelBufferRenderer m_imageRenderer;
				
				virtual void renderView (IScene * scene, TimeT time);
				
			public:
				ImageView (PTR(View) parent);
				virtual ~ImageView ();
				
				void setDefaultImage (REF(IPixelBuffer) image);
				void setDynamicFocusImage (REF(IPixelBuffer) image);
				void setStaticFocusImage (REF(IPixelBuffer) image);
								
				virtual bool motion (const MotionInput &);
				
				typedef boost::function<bool (const Input &)> OnActivateT;
				OnActivateT onActivate;
			};
			
			using Text::Font;
			using Text::TextBuffer;
			
			class TextView : public View {
				EXPOSE_CLASS(TextView)
				
				class Class : public View::Class {
					EXPOSE_CLASSTYPE
				};
			
			protected:
				REF(Text::TextBuffer) m_textBuffer;
				
				bool m_editable;
				unsigned int m_offset;
				
			public:
				TextView (PTR(View) parent, PTR(Font) font);
				virtual ~TextView ();
				
				REF(TextBuffer) textBuffer ();
				
				void setEditable (bool flag = true);
				bool editable ();
				
				virtual bool button (const ButtonInput &);
				virtual bool motion (const MotionInput &);
			};
		}
	}
}

#endif
