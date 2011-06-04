/*
 *  TexturedShapes.h
 *  Dream
 *
 *  Created by Samuel Williams on 14/09/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_TEXTUREDSHAPES_H
#define _DREAM_CLIENT_GRAPHICS_TEXTUREDSHAPES_H

#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			/// A very basic structure for the generation of triangle-strip models.
			struct Model
			{
				std::vector<Vec3> vertexArray;
				std::vector<Vec3> normalArray;
				std::vector<Vec2> texCoordArray;
				std::vector<GLushort> indexArray;
			};
			
			void buildSphere (Model & model, const RealT & radius, unsigned stacks, unsigned slices);
			void buildPlane (Model & model, Vec2u, Vec2 size, Vec2 offset, RealT z);
			void drawModel (Model & model);
			
			class ModelRenderer : public Object, implements ILayer {
				protected:
					Model m_model;
					REF(RenderState) m_renderState;
					
				public:
					ModelRenderer(Model & model, PTR(RenderState) renderState);

					virtual void renderFrameForTime (IScene * scene, TimeT time);
			};
						
			class TexturedBox : public Object, implements ILayer {
				static const int SIDES = 6;
				
			protected:
				String m_name;
				RealT m_size;
				
				RealT m_rotation;
				RealT m_inset;
				
				REF(Texture) m_sides[SIDES];
				
			public:
				enum Side {
					NORTH = 0,
					EAST = 1,
					SOUTH = 2,
					WEST = 3,
					DOWN = 4,
					UP = 5
				};
				
				TexturedBox(const String & name, const RealT & size, const RealT & inset = 0);
				TexturedBox(const RealT & size);
				
				void setInset(RealT inset);
				void setVerticalRotation(RealT rotation);
				void setSize(RealT size);
				
				void setTexture (Side s, PTR(Texture) tex);
				
				virtual void renderFrameForTime (IScene * scene, TimeT time);
				
				virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
				virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
			};
		}
	}
}

#endif
