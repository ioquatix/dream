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
			
			IMPLEMENT_CLASS(BirdsEyeCamera)
			
			BirdsEyeCamera::BirdsEyeCamera () : m_up(0.0, 1.0, 0.0), m_left(-1.0, 0.0, 0.0), m_center(ZERO), m_multiplier(IDENTITY, 1) {
				
				m_distance = -100;
				m_azimuth = R45;
				m_incidence = R45;
				m_twist = 0;
				
				regenerate();
			}
			
			BirdsEyeCamera::~BirdsEyeCamera () {
			
			}
			
			void BirdsEyeCamera::regenerate () {
				m_back = m_up.cross(m_left);
				m_left = - m_up.cross(m_back);
			}
			
			Mat44 BirdsEyeCamera::viewMatrix () const
			{
				Vec3 glUp(0.0, 1.0, 0.0), glLeft(-1.0, 0.0, 0.0), glIn(0.0, 0.0, -1.0);
				Vec3 far = m_back * -m_distance;
				
				Vec3 p = m_up.cross(glUp);
				RealT angle = m_up.angleBetween(glUp);
				
				Mat44 m(IDENTITY);

				m = m.rotatedMatrix(angle, p);
				m = m.translatedMatrix(far);
				m = m.rotatedMatrix(-m_incidence, m_left);
				m = m.rotatedMatrix(-m_azimuth, m_up);
				m = m.translatedMatrix(-m_center);
				
				return m;
			}
			
			bool BirdsEyeCamera::button(const ButtonInput &) {
				return false;
			}
			
			bool BirdsEyeCamera::motion(const MotionInput & input) {
				const Vec3 & d = input.motion();
				
				m_azimuth -= (d[X] * m_multiplier[X] * (R90 / 90));
				m_incidence += (d[Y] * m_multiplier[Y] * (R90 / 90));
				
				m_distance += (d[Z] * m_multiplier[Z]) / 8.0;
				
				std::cout << "azi: " << m_azimuth << " inc: " << m_incidence << " dist: " << m_distance << std::endl;
				
				return true;
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
			
			void BirdsEyeCamera::setLeftDirection(const Vec3 &left) {
				if (m_left != left) {
					m_left = left;
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
