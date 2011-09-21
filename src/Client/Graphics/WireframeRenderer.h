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
			
			/// These primatives are really designed for debugging purposes.
			class WireframeRenderer
			{
			protected:
				Vec4 m_color;
				
			public:
				WireframeRenderer ();
								
				void renderGrid (Display::RendererT *) const;
				void renderAxis (Display::RendererT *) const;
				
				void render (Display::RendererT *, const std::vector<Vec3> &) const;
				
				void render (Display::RendererT *, const Geometry::LineSegment<2> &) const;
				void render (Display::RendererT *, const Geometry::LineSegment<3> &) const;
								
				void render (Display::RendererT *, const Geometry::AlignedBox<2> &) const;
				void render (Display::RendererT *, const Geometry::AlignedBox<3> &) const;
				
				void setPrimaryColor (const Vec4 &);
			};
			
			class SolidRenderer
			{
			protected:
				Vec4 m_color;
			
			public:
				SolidRenderer ();
								
				void render (Display::RendererT *, const Geometry::AlignedBox<2> &) const;
				void render (Display::RendererT *, const Geometry::AlignedBox<3> &) const;
				
				void setPrimaryColor (const Vec4 &);
			};
		}
	}
}

#endif
