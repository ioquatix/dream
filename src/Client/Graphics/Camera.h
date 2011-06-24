/*
 *  Client/Graphics/Camera.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 9/06/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_CAMERA_H
#define _DREAM_CLIENT_GRAPHICS_CAMERA_H

#include "Graphics.h"
#include "../../Numerics/Vector.h"
#include "../../Numerics/Matrix.h"
#include "../../Geometry/Line.h"
#include "../../Geometry/AlignedBox.h"
#include "../Display/Input.h"

namespace Dream {
	namespace Client {
		namespace Graphics {			
			using namespace Dream::Client::Display;
			
			using Geometry::Line;
			using Geometry::AlignedBox;
			
			class ICamera : implements IObject, implements IInputHandler {
			public:				
				virtual ~ICamera ();
				
				virtual Mat44 viewMatrix() const abstract;
			};
			
			class CameraNode : public Group {
				protected:
					REF(ICamera) m_camera;
				
				public:
					CameraNode(PTR(ICamera) camera);
					virtual ~CameraNode();
					
					virtual void setCamera(PTR(ICamera) camera);
					virtual PTR(ICamera) camera();
					
					virtual bool process(const Input & input);
					
					virtual void renderFrameForTime (IScene * scene, TimeT time);
			};
		}
	}
}

#endif
