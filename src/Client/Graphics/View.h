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
#include "../../Imaging/Text/Font.h"
#include "../../Imaging/Text/TextBuffer.h"

#include "TexturedQuadRenderer.h"
#include <boost/function.hpp>

namespace Dream {
	namespace Client {
		namespace Graphics {			
			using namespace Dream::Client::Display;
			
			class View : public Object, IMPLEMENTS(Layer) {	
				EXPOSE_CLASS(View)
			
				class Class : public Object::Class, IMPLEMENTS(Layer::Class) {
				public:
					EXPOSE_CLASSTYPE
				};
			
			public:
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
								
					REF(View) principalView () { return m_principal; }
					const REF(View) principalView () const { return m_principal; }
					
					// Mouse focus
					void setDynamicFocusedView(View* view);
					View* dynamicFocusedView() const { return m_dynamicFocusedView; }
					
					// Text focus
					void setStaticFocusedView(View* view);
					View* staticFocusedView() const { return m_staticFocusedView; }
					
					virtual void renderFrameForTime (IScene * scene, TimeT time);
					
					virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
					virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
					
					virtual void dumpStructure (std::ostream & outp);
					
					void setDebugMode (bool enabled);
					bool debugMode ();
				};
				
			private:
				// This is used for setting up the parent <=> child relationship
				void registerChildView (REF(View) child);
				void viewsToPoint (const Vec2 &point, std::vector<View*> &views, bool first = true);
				
			protected:
				bool m_enabled;
				Controller *m_controller;
				View *m_parent;
				
				std::vector<REF(View)> m_subviews;
				
				Vec2 m_size, m_scale;
				Vec2 m_offset, m_orientation;
				
				AlignedBox<2> m_bounds;
				RealT m_rotation;
				
				  //, m_frame;
				//real_t m_boundsRotation, m_frameRotation;
				
				virtual bool intersectsWith (const Vec2 &point);
			
				virtual void renderView (IScene * scene, TimeT time);
			
			private:
				void init ();
				
			public:
				View (View *parent);
				View (Controller *controller);
				View (Controller *controller, const AlignedBox<2> &bounds);
				
				virtual ~View();
				
				virtual Controller* controller () { return m_controller; }
				virtual const Controller* controller () const { return m_controller; }
				
				bool isPrincipalView () const;
				
				void setSizeAndScale (const Vec2 &size, const Vec2 &scale);
				void setOffsetAndOrientation (const Vec2 &offset, const Vec2 &orientation);
				void setRotation (const RealT rotation);
				
				virtual const View* parentView () const { return m_parent; }
				virtual View* parentView () { return m_parent; }
				
				const std::vector<REF(View)> & childrenViews () const { return m_subviews; }
				std::vector<REF(View)> & childrenViews () { return m_subviews; }
				
				// Viewport in normalized coordinates 0.0 ... 1.0
				virtual const AlignedBox<2> & bounds () const { return m_bounds; }
				virtual RealT rotation () const { return m_rotation; }
				
				// Location with reference to superframe in normalized coordinates
				//virtual const AlignedBox<2> & frame () const { return m_frame; }
				//virtual real_t frameRotation () const { return m_frameRotation; }
				
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
				ImageView (View *parent);
				virtual ~ImageView ();
				
				void setDefaultImage (REF(IPixelBuffer) image);
				void setDynamicFocusImage (REF(IPixelBuffer) image);
				void setStaticFocusImage (REF(IPixelBuffer) image);
								
				virtual bool motion (const MotionInput &);
				
				typedef boost::function<bool (const Input &)> OnActivateT;
				OnActivateT onActivate;
			};
			
			using Imaging::Text::Font;
			using Imaging::Text::TextBuffer;
			
			class TextView : public View {
				EXPOSE_CLASS(TextView)
				
				class Class : public View::Class {
					EXPOSE_CLASSTYPE
				};
			
			protected:
				REF(Imaging::Text::TextBuffer) m_textBuffer;
				
				bool m_editable;
				unsigned int m_offset;
				
			public:
				TextView (View * parent, REF(Font) font);
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
