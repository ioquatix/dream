/*
 *  Layers.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 11/09/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "Layers.h"
#include "../Display/Scene.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
#pragma mark -
			
			
			
			TransformedGroup::TransformedGroup(const Mat44 & transform) : m_transform(transform)
			{
			}
			
			void TransformedGroup::setTransform(const Mat44 & transform)
			{
				m_transform = transform;
			}
			
			void TransformedGroup::renderFrameForTime (IScene * scene, TimeT time)
			{
				glPushMatrix();
				glMultMatrixf(m_transform.value());
				
				Group::renderFrameForTime(scene, time);
				
				glPopMatrix();
			}
			
#pragma mark -

			

			void DelegateLayer::renderFrameForTime (IScene * scene, TimeT time)
			{
				if (renderFrameForTimeCallback)
					renderFrameForTimeCallback(scene, time);
			}
			
			void DelegateLayer::didBecomeCurrent (ISceneManager * manager, IScene * scene)
			{
				if (didBecomeCurrentCallback)
					didBecomeCurrentCallback(manager, scene);
			}
			
			void DelegateLayer::willRevokeCurrent (ISceneManager * manager, IScene * scene)
			{
				if (willRevokeCurrentCallback)
					willRevokeCurrentCallback(manager, scene);
			}

#pragma mark -
			
			
			
			BackgroundLayer::BackgroundLayer(const Vec4 & color) : m_color(color) {
			
			}
				
			void BackgroundLayer::renderFrameForTime (IScene * scene, TimeT time) {
				glClearColor(m_color[0], m_color[1], m_color[2], m_color[3]);
				glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			}

#pragma mark -

			
			
			void AxisLayer::renderFrameForTime (IScene * scene, TimeT time) {
				WireframeRenderer axisRenderer;
				
				axisRenderer.renderAxis(scene->renderer());
			}
			
		}
	}
}