/*
 *  Client/Graphics/Camera.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/06/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "Camera.h"

namespace Dream {
	namespace Client {
		namespace Graphics {			
			ICamera::~ICamera ()
			{
				
			}
			
#pragma mark -
			
			CameraNode::CameraNode(PTR(ICamera) camera) : m_camera(camera)
			{
			}
			
			CameraNode::~CameraNode()
			{
			}
			
			void CameraNode::setCamera(PTR(ICamera) camera)
			{
				m_camera = camera;
			}
			
			PTR(ICamera) CameraNode::camera()
			{
				return m_camera;
			}
			
			bool CameraNode::process(const Input & input)
			{
				return m_camera->process(input);
			}
			
			void CameraNode::renderFrameForTime (IScene * scene, TimeT time)
			{
				if (m_camera) {
					glPushMatrix();
					glMultMatrixf(m_camera->viewMatrix().value());
				}
				
				Group::renderFrameForTime(scene, time);
				
				if (m_camera)
					glPopMatrix();
			}
		}
	}
}
