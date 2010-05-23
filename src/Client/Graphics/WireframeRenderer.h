/*
 *  Client/Graphics/WireframeRenderer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 22/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_WIREFRAMERENDERER_H
#define _DREAM_CLIENT_GRAPHICS_WIREFRAMERENDERER_H

#include "../../Geometry/AlignedBox.h"
#include "Graphics.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			using namespace Dream::Numerics;
			
			class WireframeRenderer
			{
			protected:
				Vec4 m_color;
				bool m_filled;
				
			public:
				WireframeRenderer ();
				
				void setFilled (bool filled);
				
				void renderGrid (Display::RendererT *);
				void renderAxis (Display::RendererT *);
				
				void render (Display::RendererT *, const std::vector<Vec3> &);
				
				void render (Display::RendererT *, const Geometry::LineSegment<2> &);
				void render (Display::RendererT *, const Geometry::LineSegment<3> &);
								
				void render (Display::RendererT *, const Geometry::AlignedBox<2> &);
				void render (Display::RendererT *, const Geometry::AlignedBox<3> &);
				
				void setPrimaryColor (const Vec4 &);
			};
		}
	}
}

#endif
