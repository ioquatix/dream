//
//  Renderer/Camera.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 9/06/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "Camera.h"

namespace Dream
{
	namespace Renderer
	{
		ICamera::~ICamera ()
		{
		}

		Camera::Camera(const Mat44 & view_matrix) : _view_matrix(view_matrix)
		{
		}

		Camera::~Camera ()
		{
		}

		Mat44 Camera::view_matrix() const
		{
			return _view_matrix;
		}

		Mat44 & Camera::view_matrix()
		{
			return _view_matrix;
		}
	}
}
