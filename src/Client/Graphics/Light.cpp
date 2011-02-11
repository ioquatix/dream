/*
 *  Light.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 4/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#include "Light.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
	
			IMPLEMENT_CLASS(Light)
			
			Light::Light ()
				: m_ambient(0, 0, 0, 1), m_diffuse(1, 1, 1, 1), m_specular(1, 1, 1, 1), m_emission(0, 0, 0, 1),
				m_position(0, 0, 1, 0), m_spotDirection(0, 0, -1), m_spotExponent(0), m_spotCutoff(R180),
				m_attenuation(1, 0, 0)
			{
			
			}
			
			Light::~Light () {
			
			}
				
			void Light::enable(GLenum lightIndex)
			{
				glEnable(GL_LIGHT0 + lightIndex);
				
				glLightfv(GL_LIGHT0 + lightIndex, GL_AMBIENT, m_ambient.value());
				glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, m_diffuse.value());
				glLightfv(GL_LIGHT0 + lightIndex, GL_SPECULAR, m_specular.value());
				//glLightfv(GL_LIGHT0 + lightIndex, GL_EMISSION, m_emission.value());
				
				glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, m_position.value());
				//glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_DIRECTION, m_spotDirection.value());
				//glLightf(GL_LIGHT0 + lightIndex, GL_SPOT_EXPONENT, m_spotExponent);
				//glLightf(GL_LIGHT0 + lightIndex, GL_SPOT_CUTOFF, m_spotCutoff * R2D);
				
				//glLightf(GL_LIGHT0 + lightIndex, GL_CONSTANT_ATTENUATION, m_attenuation[0]);
				//glLightf(GL_LIGHT0 + lightIndex, GL_LINEAR_ATTENUATION, m_attenuation[1]);
				//glLightf(GL_LIGHT0 + lightIndex, GL_QUADRATIC_ATTENUATION, m_attenuation[2]);
			}
			
			void Light::disable(GLenum lightIndex)
			{
				glDisable(GL_LIGHT0 + lightIndex);
			}

#pragma mark -

			IMPLEMENT_CLASS(LightingGroup)
			
			void LightingGroup::renderFrameForTime (IScene * scene, TimeT time)
			{
				glEnable(GL_LIGHTING);
				for (std::size_t i = 0; i < m_lights.size(); i++) {
					m_lights[i]->enable(i);
				}
				
				glColor(m_color);
				
				Group::renderFrameForTime(scene, time);
				
				for (std::size_t i = 0; i < m_lights.size(); i++) {
					m_lights[i]->disable(i);
				}
				glDisable(GL_LIGHTING);
			}
		}
	}
}