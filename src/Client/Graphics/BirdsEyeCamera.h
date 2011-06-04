/*
 *  Client/Graphics/BirdsEyeCamera.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 16/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_BIRDSEYECAMERA_H
#define _DREAM_CLIENT_GRAPHICS_BIRDSEYECAMERA_H

#include "../../Framework.h"
#include "Camera.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			class BirdsEyeCamera : public Camera {
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
				
				void setUpDirection(const Vec3 &up);
				void setRightDirection(const Vec3 &right);
			};	
		}
	}
}

#endif
