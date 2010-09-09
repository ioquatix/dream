/*
 *  Client/Graphics/View.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 19/06/07.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "View.h"
#include "WireframeRenderer.h"
#include "../Display/Scene.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
		
			IMPLEMENT_CLASS(View::Controller)
	
			View::Controller::Controller () : m_debug(false) {
				m_staticFocusedView = NULL;
				m_dynamicFocusedView = NULL;
			}

			View::Controller::~Controller () {
				
			}

			void View::Controller::setPrincipalView (View *p) {
				m_staticFocusedView = NULL;
				m_dynamicFocusedView = NULL;
				
				m_principal = p;
			}

			bool View::Controller::resize(const ResizeInput &ipt) {
				m_principal->resize(ipt);
				return true;
			}

			bool View::Controller::button(const ButtonInput &ipt) {
				if (m_staticFocusedView)
					return m_staticFocusedView->button(ipt);
				
				return false;
			}

			bool View::Controller::motion(const MotionInput &ipt) {		
				bool result = false;
				
				View* current = dynamicFocusedView();
				std::vector<View*> views;
				
				viewsToPoint(ipt.currentPosition().reduce(), views);
				
				View* next = NULL;
						
				if (views.size()) {
					// The mouse is over some views, grab the last child
					next = views.back();
					
					if (current != next) {
						// The dynamic focus needs to be updated
						setDynamicFocusedView(next);				
					}
				} else {
					// Set null view
					setDynamicFocusedView(NULL);
				}
				
				if (ipt.state() == Pressed) {
					// next is NULL if clicked outside a bounding view
					setStaticFocusedView(next);			
				}
				
				if (next) {
					views.clear();
					viewsToPoint(ipt.previousPosition().reduce(), views);
					View *prev = NULL;
					
					if (views.size()) {
						prev = views.back();
						
						if (prev->isEnabled())
							prev->motion(ipt);
					}
					
					if (next->isEnabled())
						result = next->motion(ipt);
				}
				
				return result;
			}

			void View::Controller::setDynamicFocusedView(View* view) {
				m_dynamicFocusedView = view;
			}

			void View::Controller::setStaticFocusedView(View* view) {
				m_staticFocusedView = view;
			}

			void View::Controller::viewsToPoint (const Vec2 &point, std::vector<View*> &views) {
				m_principal->viewsToPoint(point, views);
			}

			void View::Controller::renderFrameForTime (IScene * scene, TimeT time) {
				scene->renderer()->setupOrthographicDisplay(m_principal->bounds().size());
				m_principal->renderFrameForTime(scene, time);
				scene->renderer()->finishOrthographicDisplay();
			}
			
			void View::Controller::didBecomeCurrent (ISceneManager * manager, IScene * scene) {
				ensure(m_principal);
				
				m_principal->didBecomeCurrent(manager, scene);
			}
			
			void View::Controller::willRevokeCurrent (ISceneManager * manager, IScene * scene) {
				m_principal->willRevokeCurrent(manager, scene);
			}
			
			void View::Controller::dumpStructure (std::ostream & outp) {
				outp << "View Controller " << this << std::endl;
				m_principal->dumpStructure(outp, 1);
			}

			void View::Controller::setDebugMode (bool enabled) {
				m_debug = enabled;
			}
			
			bool View::Controller::debugMode () {
				return m_debug;
			}

			IMPLEMENT_CLASS(View)

			void View::init () {
				m_bounds = AlignedBox<2>(ZERO);
				m_rotation = 0;
				m_offset.zero();
				m_orientation.zero();
				m_scale.zero();
				m_size.zero();
			}
			
			void View::dumpStructure (std::ostream & outp, unsigned indent) {
				using namespace std;
				
				string space(indent, '\t');
				
				outp << space << "View: " << className() << endl;
				outp << space << "Enabled = " << m_enabled << " Bounds = " << m_bounds << " Rotation = " << m_rotation << " Subviews = " << m_subviews.size() << endl;
				outp << space << "Size = " << m_size << " Scale = " << m_scale << " Offset = " << m_offset << " Orientation = " << m_orientation << endl;
				
				for (iterateEach(m_subviews, sv)) {
					(*sv)->dumpStructure(outp, indent + 1);
				}
			}

			View::View (PTR(View) parent) 
				: m_enabled(true), m_parent(parent)
			{
				// This is a subview of m_parent
				m_parent->registerChildView(this);
				m_controller = m_parent->controller();
				m_enabled = m_parent->isEnabled();
				
				init();
			}

			View::View (PTR(Controller) controller) 
				: m_enabled(true), m_controller(controller) 
			{
				m_controller->setPrincipalView(this);
				init();
			}

			View::View (PTR(Controller) controller, const AlignedBox<2> &bounds)
				: m_enabled(true), m_controller(controller), m_bounds(bounds)
			{
				m_controller->setPrincipalView(this);
				init();
			}

			View::~View() {
				
			}

			void View::setSizeAndScale (const Vec2 &size, const Vec2 &scale) {
				m_size = size;
				m_scale = scale;
			}

			void View::setOffsetAndOrientation (const Vec2 &offset, const Vec2 &orientation) {
				m_offset = offset;
				m_orientation = orientation;
			}
			
			void View::setRotation (const RealT rotation) {
				m_rotation = rotation;
			}

			void View::registerChildView (REF(View) child) {
				m_subviews.push_back(child);
			}

			bool View::isPrincipalView () const {
				return !m_parent;
			}

			bool View::intersectsWith (const Vec2 &point) {
				return m_bounds.containsPoint(point);
			}

			void View::viewsToPoint (const Vec2 &point, std::vector<View*> &views, bool first) {
				if (first && intersectsWith(point))
					views.push_back(this);
				
				for (iterateEach(m_subviews, sv)) {
					if ((*sv)->intersectsWith(point)) {
						views.push_back((*sv).get());			
						
						// Descend into child view
						(*sv)->viewsToPoint(point, views, false);
						
						break;
					}
				}
			}

			void View::renderView (IScene * scene, TimeT time) {
				WireframeRenderer boundingBoxRenderer;
				
				if (hasStaticFocus())
					boundingBoxRenderer.setPrimaryColor(Vec4(1.0, 0.0, 0.0, 0.2));
				else if (hasDynamicFocus())
					boundingBoxRenderer.setPrimaryColor(Vec4(1.0, 1.0, 0.0, 0.2));
				else
					boundingBoxRenderer.setPrimaryColor(Vec4(0.0, 1.0, 0.0, 0.2));
				
				
				if (m_controller->debugMode())
					boundingBoxRenderer.render(scene->renderer(), m_bounds);
			}

			void View::renderFrameForTime(IScene * scene, TimeT time) {
				if (m_rotation != 0) {
					Vec2 t = -m_bounds.center();
				
					glPushMatrix();
					glTranslatef(t[X], t[Y], 0);
					glRotatef(m_rotation, 0.0, 0.0, 1.0);
					glTranslatef(-t[X], -t[Y], 0);
				}
				
				renderView(scene, time);
				
				for (iterateEach(m_subviews, sv)) {
					(*sv)->renderFrameForTime(scene, time);
				}
				
				if (m_rotation != 0) {
					glPopMatrix();
				}
			}
			
			void View::didBecomeCurrent (ISceneManager * manager, IScene * scene) {
				for (iterateEach(m_subviews, sv)) {
					(*sv)->didBecomeCurrent(manager, scene);
				}
			}
			
			void View::willRevokeCurrent (ISceneManager * manager, IScene * scene) {
				for (iterateEach(m_subviews, sv)) {
					(*sv)->willRevokeCurrent(manager, scene);
				}			
			}

			bool View::resize(const ResizeInput &ipt) {
				if (isPrincipalView()) {
					// We need to resize the entire size
					m_bounds.min().zero();
					std::cout << "Resizing principal from " << m_bounds.origin() << "; " << m_bounds.size();
					m_bounds.setSizeFromOrigin(ipt.newSize());
					std::cout << " to " << m_bounds.origin() << "; " << m_bounds.size() << "(" << ipt.newSize() << ")" << std::endl;
				} else {
					//std::cout << "Resizing view from " << m_bounds.origin() << "; " << m_bounds.size();
					m_bounds.setSizeFromOrigin(m_parent->bounds().size() * m_scale + m_size);
					m_bounds.alignWithinSuperBox(m_parent->bounds(), m_orientation);
					m_bounds.translateBy(m_offset);
					//std::cout << " to " << m_bounds.origin() << "; " << m_bounds.size() << std::endl;
				}

				bool result = false;
				
				// Propagate resize input down
				for (iterateEach(m_subviews, sv)) {
					result = result || (*sv)->resize(ipt);
				}
				
				return result;
			}

			bool View::button(const ButtonInput &ipt) {
				if (!m_enabled) return false;
				
				bool handled = false;
				
				for (iterateEach(m_subviews, sv)) {
					handled = handled || (*sv)->button(ipt);
				}
				
				return handled;
			}

			bool View::motion(const MotionInput &ipt) {
				return false;
			}

			bool View::hasDynamicFocus () const {
				return controller()->dynamicFocusedView() == this;
			}

			bool View::hasStaticFocus () const {
				return controller()->staticFocusedView() == this;
			}  

			bool View::isEnabled () const {
				return m_enabled;
			}

			void View::enable () {
				m_enabled = true;
				
				for (iterateEach(m_subviews, sv)) {
					(*sv)->enable();
				}
			}

			void View::disable () {
				m_enabled = false;
				
				for (iterateEach(m_subviews, sv)) {
					(*sv)->disable();
				}
			}
			
#pragma mark -

			IMPLEMENT_CLASS(ImageView)

			ImageView::ImageView (PTR(View) parent) : View(parent) {
			
			}
			
			ImageView::~ImageView () {
			
			}
			
			void ImageView::setDefaultImage (REF(IPixelBuffer) image) {
				m_defaultImage = image;
			}
			
			void ImageView::setDynamicFocusImage (REF(IPixelBuffer) image) {
				m_dynamicFocusImage = image;
			}
			
			void ImageView::setStaticFocusImage (REF(IPixelBuffer) image) {
				m_staticFocusImage = image;
			}
			
			void ImageView::renderView (IScene * scene, TimeT time) {
				REF(IPixelBuffer) image = m_defaultImage;
				
				if (hasStaticFocus() && m_staticFocusImage) {
					image = m_staticFocusImage;
				} else if (hasDynamicFocus() && m_dynamicFocusImage) {
					image = m_dynamicFocusImage;
				}
				
				if (image) {
					if (m_lastImage != image) {
						m_imageRenderer.change(scene->renderer(), image, m_bounds, vec(false, true));
						m_lastImage = image;
					}
					
					m_imageRenderer.render(scene->renderer());
				}
				
				View::renderView(scene, time);
			}
			
			bool ImageView::motion (const MotionInput & input) {
				if (onActivate && hasStaticFocus()) {
					return onActivate(input);
				}
				
				return false;
			}
		
#pragma mark -

			IMPLEMENT_CLASS(TextView)
			
			TextView::TextView (PTR(View) parent, PTR(Font) font) : View(parent), m_offset(0) {
				m_textBuffer = new Text::TextBuffer(font);
			}
			
			TextView::~TextView () {
			
			}
			
			REF(TextBuffer) TextView::textBuffer () {
				return m_textBuffer;
			}
			
			void TextView::setEditable (bool flag) {
				m_editable = flag;
			}
			
			bool TextView::editable () {
				return m_editable;
			}
			
			bool TextView::button (const ButtonInput & input) {
				if (m_editable && hasStaticFocus()) {
					m_textBuffer->insertCharacterAtOffset(m_offset, input.key().button());
					m_offset += 1;
					
					return true;
				}
				
				return false;
			}
			
			bool TextView::motion (const MotionInput & input) {
				return View::motion(input);
			}
		
		}
	}
}