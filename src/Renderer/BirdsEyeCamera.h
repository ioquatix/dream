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

namespace Dream
{
	namespace Renderer
	{
		
		class BirdsEyeCamera : public Object, implements ICamera {
		protected:
			Vec3 m_up;
			Vec3 m_right;
			
			/* Cached m_up.cross(m_left) */
			Vec3 m_back;
			
			Vec3 m_center;
			
			RealT m_distance;
			RealT m_twist, m_azimuth, m_incidence;
			
			Vec3 m_multiplier;
			
			void regenerate ();
		public:
			BirdsEyeCamera ();
			virtual ~BirdsEyeCamera ();
			
			virtual bool button(const ButtonInput &);
			virtual bool motion(const MotionInput &);
			
			virtual Mat44 viewMatrix () const;
			
			void setCenter (const Vec3 &newCenter);
			const Vec3& center () const {
				return m_center;
			}
			
			void setDistance (const RealT& amnt, bool relative = false);
			const RealT& distance () const { return m_distance; }
			
			void setTwist (const RealT& amnt, bool relative = false);
			const RealT& twist () const { return m_twist; }
			
			void setAzimuth (const RealT& amnt, bool relative = false);
			const RealT& azimuth () const { return m_azimuth; }
			
			void setMultiplier (const Vec3 &m);
			const Vec3 &multiplier ();
			
			void setIncidence (const RealT &amnt, bool relative = false);
			const RealT& incidence () const { return m_incidence; }
			
			void setUp (const Vec3 &up);
			const Vec3 & up () const { return m_up; }
			
			void setRight (const Vec3 &right);
			const Vec3 & right () const { return m_right; }
		};	
	}
}

#endif
