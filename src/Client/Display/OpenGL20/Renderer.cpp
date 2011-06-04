/*
 *  Client/Display/OpenGL20/Renderer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Renderer.h"

#include "RenderState.h"
#include "Texture.h"

#include "../../../Core/Strings.h"
#include <iterator>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{
			namespace OpenGL20
			{
			
				

				Renderer::Renderer () : m_textureController(new TextureController)
				{
					ensure(softwareVersion() >= 2.0 && "OpenGL 2.0 support required");
					
					const char * exts = (const char *)glGetString(GL_EXTENSIONS);
					split(exts, ' ', std::inserter(m_extensions, m_extensions.begin()));
				}

				Renderer::~Renderer ()
				{
					delete m_textureController;
				}
				
				float Renderer::softwareVersion () const
				{
					return atof((const char *)glGetString(GL_VERSION));
				}
				
				const std::set<String> & Renderer::softwareExtensions () const
				{
					return m_extensions;
				}
				
				void Renderer::updateCurrentRenderState (REF(RenderState) renderState)
				{
					if (m_currentRenderState != renderState)
					{
						if (m_currentRenderState)
							m_currentRenderState->revokeState(this);
						
						if (renderState)
							renderState->applyState(this);
						
						m_currentRenderState = renderState;
					}
				}

				void Renderer::setViewport (const Vector<2, int> & origin, const Vector<2, int> & size)
				{
					//std::cerr << "*** Setting viewport: origin=" << origin << " size=" << size << std::endl;
					
					glViewport(origin[X], origin[Y], size[WIDTH], size[HEIGHT]);
				}

				AlignedBox<2, int> Renderer::viewport () const
				{
					GLint vp[4];
					glGetIntegerv(GL_VIEWPORT, vp);
					
					AlignedBox<2, int> box (vec(vp[0], vp[1]), vec(vp[2], vp[3]));
					
					return box;
				}

				void Renderer::setPerspectiveView (RealT ratio, RealT fov, RealT near, RealT far)
				{
					// fov is in radians, but internally we are using degrees
					fov = fov * R2D;
					
					//std::cerr << "*** Setting perspective view: ratio=" << ratio << " fov=" << fov << " near=" << near << " far=" << far << std::endl;
					
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					gluPerspective(fov, ratio, near, far);
					
					glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
					
					glMatrixMode(GL_MODELVIEW);
				}

				void Renderer::setOrthographicView (const AlignedBox<2> & box, RealT near, RealT far)
				{
					RealT left, right, top, bottom;
					
					left = box.min()[X];
					right = box.max()[X];
					top = box.max()[Y];
					bottom = box.min()[Y];
					
					//std::cerr << "*** Setting orthographic view: left=" << left << " right=" << right << " bottom=" << bottom << " top=" << top << std::endl;
					
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					
					glOrtho(left, right, bottom, top, near, far);
					
					glMatrixMode(GL_MODELVIEW);
				}

				void Renderer::setBackgroundColor (const Vec4 & color)
				{
					glClearColor(color[X], color[Y], color[Z], color[W]);
				}

				void Renderer::clearBuffers (int mask)
				{
					glClear(mask);
				}

				Mat44 Renderer::projectionMatrix () const
				{
					Mat44 m;
					glGetFloatv(GL_PROJECTION_MATRIX, m.value());
					
					return m;
				}

				Mat44 Renderer::modelViewMatrix () const
				{
					Mat44 m;
					glGetFloatv(GL_MODELVIEW_MATRIX, m.value());
					
					return m;
				}

				void Renderer::setupOrthographicDisplay (const Vector<2, unsigned> & viewportSize) {
					glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					
					glDisable (GL_DEPTH_TEST); // ensure text is not remove by death buffer test.
					// for text alpha blending
					glEnable (GL_BLEND);
					glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					
					glColor4f(1.0, 1.0, 1.0, 1.0);
					
					// set orthograhic 1:1  pixel transform in local view coords
					glMatrixMode (GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity ();
					glMatrixMode (GL_MODELVIEW);
					glPushMatrix();
					glLoadIdentity ();
					
					glScalef (2.0f / viewportSize[WIDTH], 2.0f / viewportSize[HEIGHT], 1.0f);
					glTranslatef ((viewportSize[WIDTH] / -2.0f), (viewportSize[HEIGHT] / -2.0f), 0.0f);
				}

				void Renderer::finishOrthographicDisplay () {
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
					glMatrixMode (GL_PROJECTION);
					glPopMatrix();
					
					glPopAttrib();
				}
				
				void Renderer::setupView (const Mat44 & viewMatrix)
				{
					glMatrixMode(GL_MODELVIEW);
					glLoadIdentity();
				
					glMultMatrixf(viewMatrix.value());
				}
				
				TextureController * Renderer::textureController()
				{
					return m_textureController;
				}
				
			}
		}
	}
}