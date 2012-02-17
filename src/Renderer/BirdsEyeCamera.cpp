//
//  Renderer/BirdsEyeCamera.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#include "BirdsEyeCamera.h"
#include "../Events/Logger.h"

namespace Dream {
	namespace Renderer {
		
		using namespace Events::Logging;
		
		BirdsEyeCamera::BirdsEyeCamera() : _up(0.0, 0.0, 1.0), _right(1.0, 0.0, 0.0), _center(ZERO), _multiplier(IDENTITY, 1), _reverse(false) {
			_distance = 100;
			_azimuth = R45;
			_incidence = R45;
			_twist = 0;
			
			regenerate();
		}
		
		BirdsEyeCamera::~BirdsEyeCamera () {
		
		}
		
		void BirdsEyeCamera::regenerate () {
			_back = _up.cross(_right);
			_right = - _up.cross(_back);
		}
		
		Mat44 BirdsEyeCamera::view_matrix () const
		{
			Vec3 glUp(0.0, 1.0, 0.0), glRight(1.0, 0.0, 0.0), glIn(0.0, 0.0, -1.0);
			Vec3 far = _back * _distance;
			
			Mat44 m = Mat44::rotating_matrix(glUp, _up, _back);
			m = m.translated_matrix(far);
			m = m.rotated_matrix(-_incidence, _right);
			m = m.rotated_matrix(-_azimuth, _up);
			m = m.translated_matrix(-_center);				
			m = m.rotated_matrix(_twist, _up);
			
			return m;
		}
		
		bool BirdsEyeCamera::button(const ButtonInput &) {
			return false;
		}
		
		bool BirdsEyeCamera::motion(const MotionInput & input) {
			const Vec3 & d = input.motion();

			if (input.button_pressed_or_dragged(MouseLeftButton)) {
				RealT k = -1.0, i = Math::mod(_incidence, R360);
				
				if (i < 0) i += R360;
				
				// Reverse motion if we are upside down:
				if (_reverse && i > R180 && i < R360)
					k *= -1.0;
				
				// Find the relative position of the mouse, if it is in the lower half,
				// reverse the rotation.
				Vec2 relative = input.bounds().relative_offset_of(input.current_position().reduce());
				
				//logger()->log(LOG_DEBUG, LogBuffer() << "Motion: " << d);
				
				// If mouse button is in lower half of view:
				if (relative[Y] <= 0.5)
					k *= -1.0;
					
				_azimuth += (k * d[X] * _multiplier[X] * (R90 / 90));
				_incidence += (d[Y] * _multiplier[Y] * (R90 / 90));
				
				return true;
			} else if (input.key().button() == MouseScroll) {
				_distance += (d[Y] * _multiplier[Z]);
				
				return true;
			} else {
				return false;
			}
		}
		
		void BirdsEyeCamera::set_multiplier (const Vec3 &m) {
			_multiplier = m;
		}
		
		const Vec3 & BirdsEyeCamera::multiplier () {
			return _multiplier;
		}
		
		void BirdsEyeCamera::set_center (const Vec3 &new_center) {
			_center = new_center;
		}
		
		void BirdsEyeCamera::set_up (const Vec3 &up) {
			if (_up != up) {
				_up = up;
				/* Regenerate Cache */
				regenerate();
			}
		}
		
		void BirdsEyeCamera::set_right (const Vec3 &right) {
			if (_right != right) {
				_right = right;
				/* Regenerate Cache */
				regenerate();
			}
		}
		
		void BirdsEyeCamera::set_distance (const RealT& amnt, bool relative) {
			if (relative)
				_distance += amnt;
			else
				_distance = amnt;
		}
		
		void BirdsEyeCamera::set_twist (const RealT& amnt, bool relative) {
			if (relative)
				_twist += amnt;
			else
				_twist = amnt;
		}
		
		void BirdsEyeCamera::set_azimuth (const RealT& amnt, bool relative) {
			if (relative)
				_azimuth += amnt;
			else
				_azimuth = amnt;
		}
		
		void BirdsEyeCamera::set_incidence (const RealT &amnt, bool relative) {
			if (relative)
				_incidence += amnt;
			else
				_incidence = amnt;
		}
	}
	
}
