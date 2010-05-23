/*
 *  Client/Display/OpenGL20/Renderer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 20/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_RENDERER_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_RENDERER_H

// This is a private header, and should not be used as public API.

#include "../../../Core/Strings.h"
#include "../../../Geometry/AlignedBox.h"
#include "OpenGL20.h"

#include <set>

namespace Dream
{
	namespace Client
	{
		namespace Display
		{			
			namespace OpenGL20
			{
				using namespace Dream::Core;
				using namespace Dream::Geometry;
				
				class Renderer;
				class TextureController;
				class RenderState;
				
				class Renderer : public Object
				{
					EXPOSE_CLASS(Renderer)
					
					class Class : public Object::Class
					{
						EXPOSE_CLASSTYPE
					};
					
				protected:				
					REF(RenderState) m_currentRenderState;
					TextureController * m_textureController;
					
					std::set<String> m_extensions;
					
				public:
					Renderer ();
					virtual ~Renderer ();
					
					/// Renderer Capabilities
					float softwareVersion () const;
					const std::set<String> & softwareExtensions () const;
					
					void updateCurrentRenderState (REF(RenderState) renderState);
					
					void setViewport (const Vector<2, int> & origin, const Vector<2, int> & size);
					AlignedBox<2, int> viewport () const;
					
					void setPerspectiveView (RealT ratio, RealT fov, RealT near, RealT far);
					void setOrthographicView (const AlignedBox<2> & box, RealT near, RealT far);
					
					void setBackgroundColor (const Vec4 & color);
					void clearBuffers (int mask);
					
					Mat44 projectionMatrix () const;
					Mat44 modelViewMatrix () const;
					
					void setupOrthographicDisplay (const Vector<2, unsigned> & viewportSize);
					void finishOrthographicDisplay ();
					
					void setupView (const Mat44 & viewMatrix);
					
					TextureController * textureController();
				};
				
			}
			
			typedef OpenGL20::Renderer RendererT;
		}
	}
}

#endif
