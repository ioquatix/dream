//
//  Renderer/BirdsEyeCamera.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 16/10/06.
//  Copyright (c) 2006 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_RENDERER_BIRDSEYECAMERA_H
#define _DREAM_RENDERER_BIRDSEYECAMERA_H

#include "Camera.h"

#include <Euclid/Numerics/Angle.h>

namespace Dream
{
	namespace Renderer
	{
		using Euclid::Numerics::Radians;

		class BirdsEyeCamera : public Object, implements ICamera {
		protected:
			Vec3 _up;
			Vec3 _right;

			/* Cached _up.cross(_right) */
			Vec3 _forward;

			Vec3 _center;

			RealT _distance;
			Radians<> _twist, _azimuth, _incidence;

			Vec3 _multiplier;

			bool _reverse;

			mutable bool _invalid;
			mutable Mat44 _view_matrix_cache;
			
			void regenerate ();
		public:
			BirdsEyeCamera ();
			virtual ~BirdsEyeCamera ();

			virtual bool button(const ButtonInput &);
			virtual bool motion(const MotionInput &);

			bool reverse() { return _reverse; }
			void set_reverse(bool reverse) { _reverse = reverse; }

			virtual Mat44 view_matrix () const;

			void set_center (const Vec3 &new_center);
			const Vec3& center () const {
				return _center;
			}

			void set_distance (const RealT& amnt, bool relative = false);
			const RealT & distance () const { return _distance; }

			void set_twist (const RealT& amnt, bool relative = false);
			const Radians<> & twist () const { return _twist; }

			void set_azimuth (const RealT& amnt, bool relative = false);
			const Radians<> & azimuth () const { return _azimuth; }

			void set_multiplier (const Vec3 &m);
			const Vec3 &multiplier ();

			void set_incidence (const RealT &amnt, bool relative = false);
			const Radians<> & incidence () const { return _incidence; }

			void set_up (const Vec3 &up);
			const Vec3 & up () const { return _up; }

			void set_right (const Vec3 &right);
			const Vec3 & right () const { return _right; }
		};
	}
}

#endif
