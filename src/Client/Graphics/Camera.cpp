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
			IMPLEMENT_CLASS(Camera)
			
			Camera::~Camera ()
			{
				
			}
						
			bool Camera::button(const ButtonInput &ipt)
			{
				return false;
			}
			
			bool Camera::motion(const MotionInput &ipt)
			{
				return false;
			}
			
			ViewportEyeSpace Camera::convertFromViewportToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, 
																	   const AlignedBox<2,int> & viewport, const Vec2 & c)
			{
				// Reverse the viewport transformation
				Vec2 n;
				
				// Calculate position in normal projection box
				n[X] = ((c[X] - viewport.origin()[X]) / viewport.size()[X]) * 2.0 - 1.0;
				n[Y] = ((c[Y] - viewport.origin()[Y]) / viewport.size()[Y]) * 2.0 - 1.0;
				
				return convertFromProjectionSpaceToObjectSpace (projectionMatrix, modelViewMatrix, n);
			}
			
			ViewportEyeSpace Camera::convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec2 & _n)
			{
				// Reverse the viewport transformation from normalized device coordinates back into object space
				Vec3 n = _n << -1;
				
				ViewportEyeSpace result;
				
				Mat44 inverseModelView(modelViewMatrix.inverseMatrix());
				Mat44 inverseProjection(projectionMatrix.inverseMatrix());
				
				result.origin = Vec3(inverseModelView.at(3,0), inverseModelView.at(3,1), inverseModelView.at(3,2));

				Vec4 p1 = (inverseModelView * (inverseProjection * (n << 1.0)));
				
				n[Z] = 1;
				Vec4 p2 = (inverseModelView * (inverseProjection * (n << 1.0)));
								
				p1 /= p1[W];
				p2 /= p2[W];
				
				result.forward = Line<3>(p1.reduce(), (p2 - p1).reduce());
				
				// Calculate up direction
				Vec3 eyeUp(0.0, 1.0, 0.0);
				// Can't remember why we set these elements to zero...?
				//inverseModelView.at(3,0) = inverseModelView.at(3,1) = inverseModelView.at(3,2) = 0.0;
				//inverseModelView.at(0,3) = inverseModelView.at(1,3) = inverseModelView.at(2,3) = 0.0;
				result.up = inverseModelView * eyeUp;
				
				return result;
			}
			
			Vec4 Camera::convertFromProjectionSpaceToObjectSpace (const Mat44 & projectionMatrix, const Mat44 & modelViewMatrix, const Vec3 & n)
			{
				Mat44 inverseModelView(modelViewMatrix.inverseMatrix());
				Mat44 inverseProjection(projectionMatrix.inverseMatrix());
				
				return (inverseModelView * (inverseProjection * (n << 1.0)));
			}
			
#pragma mark -
			
			IMPLEMENT_CLASS(CameraNode)
			
			CameraNode::CameraNode(PTR(Camera) camera) : m_camera(camera)
			{
			}
			
			CameraNode::~CameraNode()
			{
			}
			
			void CameraNode::setCamera(PTR(Camera) camera)
			{
				m_camera = camera;
			}
			
			PTR(Camera) CameraNode::camera()
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
