/*
 *  Client/Graphics/BirdsEyeCamera.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 16/10/06.
 *  Copyright 2006 Samuel G. D. Williams. All rights reserved.
 *
 */

#include "BirdsEyeCamera.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
						
			BirdsEyeCamera::BirdsEyeCamera () : m_up(0.0, 0.0, 1.0), m_right(1.0, 0.0, 0.0), m_center(ZERO), m_multiplier(IDENTITY, 1) {
				
				m_distance = 100;
				m_azimuth = R45;
				m_incidence = R45;
				m_twist = 0;
				
				regenerate();
			}
			
			BirdsEyeCamera::~BirdsEyeCamera () {
			
			}
			
			void BirdsEyeCamera::regenerate () {
				m_back = m_up.cross(m_right);
				m_right = - m_up.cross(m_back);
			}
			
			Mat44 BirdsEyeCamera::viewMatrix () const
			{
				Vec3 glUp(0.0, 1.0, 0.0), glRight(1.0, 0.0, 0.0), glIn(0.0, 0.0, -1.0);
				Vec3 far = m_back * m_distance;
				
				Mat44 m = Mat44::rotatingMatrix(glUp, m_up, m_back);
				m = m.translatedMatrix(far);
				m = m.rotatedMatrix(-m_incidence, m_right);
				m = m.rotatedMatrix(-m_azimuth, m_up);
				m = m.translatedMatrix(-m_center);				
				m = m.rotatedMatrix(m_twist, m_up);
				
				return m;
			}
			
			bool BirdsEyeCamera::button(const ButtonInput &) {
				return false;
			}
			
			bool BirdsEyeCamera::motion(const MotionInput & input) {
				const Vec3 & d = input.motion();

				if (input.buttonPressedOrDragged(MouseLeftButton)) {
					RealT k = -1.0, i = Math::mod(m_incidence, R360);
					
					if (i > R90 && i < R270)
						k = 1.0;
						
					m_azimuth += (k * d[X] * m_multiplier[X] * (R90 / 90));
					m_incidence += (d[Y] * m_multiplier[Y] * (R90 / 90));
					
					return true;
				} else if (input.key().button() == MouseScroll) {
					m_distance += (d[Y] * m_multiplier[Z]);
					
					return true;
				} else {
					return false;
				}
			}
			
			void BirdsEyeCamera::setMultiplier (const Vec3 &m) {
				m_multiplier = m;
			}
			
			const Vec3 &BirdsEyeCamera::multiplier () {
				return m_multiplier;
			}
			
			void BirdsEyeCamera::setCenter (const Vec3 &newCenter) {
				m_center = newCenter;
			}
			
			void BirdsEyeCamera::setUpDirection(const Vec3 &up) {
				if (m_up != up) {
					m_up = up;
					/* Regenerate Cache */
					regenerate();
				}
			}
			
			void BirdsEyeCamera::setRightDirection(const Vec3 &right) {
				if (m_right != right) {
					m_right = right;
					/* Regenerate Cache */
					regenerate();
				}
			}
			
			void BirdsEyeCamera::setDistance (const RealT& amnt, bool relative) {
				if (relative)
					m_distance += amnt;
				else
					m_distance = amnt;
			}
			
			void BirdsEyeCamera::setTwist (const RealT& amnt, bool relative) {
				if (relative)
					m_twist += amnt;
				else
					m_twist = amnt;
			}
			
			void BirdsEyeCamera::setAzimuth (const RealT& amnt, bool relative) {
				if (relative)
					m_azimuth += amnt;
				else
					m_azimuth = amnt;
			}
			
			void BirdsEyeCamera::setIncidence (const RealT &amnt, bool relative) {
				if (relative)
					m_incidence += amnt;
				else
					m_incidence = amnt;
			}
		}
	}
}
