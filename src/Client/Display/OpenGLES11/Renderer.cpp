/*
 *  Client/Display/OpenGLES11/Renderer.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Renderer.h"

#include "RenderState.h"
#include "Texture.h"

namespace Dream {
	namespace Client {
		namespace Display {
			namespace OpenGLES11 {
			
				

				Renderer::Renderer () : m_textureController(new TextureController)
				{
					ensure(softwareVersion() >= 1.1 && "OpenGLES 1.1 support required");
					
					const char * exts = (const char *)glGetString(GL_EXTENSIONS);
					split(exts, ' ', std::inserter(m_extensions, m_extensions.begin()));
				}

				Renderer::~Renderer ()
				{
					delete m_textureController;
				}
				
				float Renderer::softwareVersion () const
				{
					const char * versionString = (const char *)glGetString(GL_VERSION);
					
					// Find the offset in the string where the version number starts
					unsigned i = 0;
					while (versionString[i] != '\0' && (versionString[i] < '0' || versionString[i] > '9')) i += 1;
										
					return atof(versionString + i);
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
					std::cerr << "Setting viewport: origin=" << origin << " size=" << size << std::endl;
					
					glViewport(origin[X], origin[Y], size[WIDTH], size[HEIGHT]);
				}

				AlignedBox<2, int> Renderer::viewport () const
				{
					GLint vp[4];
					glGetIntegerv(GL_VIEWPORT, vp);
					
					AlignedBox<2, int> box (vec(vp[0], vp[1]), vec(vp[2], vp[3]));
					
					return box;
				}
				
				// Borrowed from MESA
				// http://steinsoft.net/index.php?site=Programming/Code%20Snippets/OpenGL/gluperspective
				void gluPerspectiveMESA(float fovy, float aspect, float zNear, float zFar)
				{
					float xmin, xmax, ymin, ymax;
					
					ymax = zNear * tan(fovy * M_PI / 360.0);
					ymin = -ymax;
					xmin = ymin * aspect;
					xmax = ymax * aspect;
					
					//std::cerr << xmin << " " << xmax << " " << ymin << " " << ymax << std::endl;
					glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);
				}

				void Renderer::setPerspectiveView (RealT ratio, RealT fov, RealT near, RealT far)
				{
					// fov is in radians, but internally we are using degrees
					fov = fov * R2D;
					
					// std::cerr << "*** Setting perspective view: ratio=" << ratio << " fov=" << fov << " near=" << near << " far=" << far << std::endl;
					
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					gluPerspectiveMESA(fov, ratio, near, far);
					
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
					
					// std::cerr << "*** Setting orthographic view: left=" << left << " right=" << right << " bottom=" << bottom << " top=" << top << std::endl;
					
					glMatrixMode(GL_PROJECTION);
					glLoadIdentity();
					
					glOrthof(left, right, bottom, top, near, far);
					
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
					glDisable (GL_DEPTH_TEST); // ensure text is not remove by death buffer test.
					// for text alpha blending
					glEnable (GL_BLEND);
					glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					
					// set orthograhic 1:1  pixel transform in local view coords
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glLoadIdentity();
					
					glScalef(2.0f / viewportSize[WIDTH], 2.0f / viewportSize[HEIGHT], 1.0f);
					glTranslatef((viewportSize[WIDTH] / -2.0f), (viewportSize[HEIGHT] / -2.0f), 0.0f);
				}

				void Renderer::finishOrthographicDisplay () {
					glMatrixMode (GL_PROJECTION);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
					
					glEnable(GL_DEPTH_TEST);
					glDisable(GL_BLEND);
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