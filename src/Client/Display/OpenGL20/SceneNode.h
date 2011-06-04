/*
 *  Client/Display/OpenGL20/SceneNode.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 27/04/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_OPENGL20_SCENENODE_H
#define _DREAM_CLIENT_DISPLAY_OPENGL20_SCENENODE_H

#include "../../Framework.h"
#include "../../Core/Timer.h"

#include "../Display/Renderer.h"

#include "RenderState.h"
#include "Shader.h"

#include "../../Numerics/Quaternion.h"
#include "../../Numerics/Vector.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			using namespace Core;
			using namespace Display;
			
			class ISceneNode : implements IObject
			{
			public:
				virtual ~ISceneNode ();
				
				virtual void render (Renderer *, TimeT time) const abstract;
				
				//virtual AlignedBox<3> boundingBox () abstract;
			};
			
			class SceneNode : public Object, implements ISceneNode {
			protected:
				REF(IRenderState) m_renderState;
				
				bool m_translate;
				Vec3 m_translation;
				
				bool m_rotate;
				Quaternion<> m_rotation;
				
				// This function sets up the render pipeline
				void prepareToRender (Renderer * renderer) const;
			public:
				SceneNode();
				virtual ~SceneNode();
				
				void setRenderState (PTR(RenderState) renderState) { m_renderState = renderState; }
				REF(IRenderState) renderState () { return m_renderState; }
				
				/// Helper to create associated render states
				REF(RenderState) createRenderState (REF(IShader) shader);
				
				virtual void render (Renderer * renderer, TimeT time) const;
			};
			
		}
	}
}

#endif
