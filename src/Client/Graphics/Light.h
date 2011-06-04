/*
 *  Light.h
 *  Dream
 *
 *  Created by Samuel Williams on 4/05/08.
 *  Copyright 2008 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_LIGHT_H
#define _DREAM_CLIENT_GRAPHICS_LIGHT_H

#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
		
			class Light : public Object
			{
			protected:
				Vec4 m_position;
				Vec4 m_diffuse, m_specular, m_ambient, m_emission;
				
				Vec3 m_spotDirection;
				RealT m_spotExponent, m_spotCutoff;
				
				Vec3 m_attenuation;
				
			public:
				Light ();
				virtual ~Light ();
				
				void enable(GLenum lightIndex);
				static void disable(GLenum lightIndex);
				
				Vec4 position() { return m_position; }
				
				void setPosition(const Vec4 & position) { m_position = position; }
				void setDiffuse(const Vec4 & diffuse) { m_diffuse = diffuse; }
				void setSpecular(const Vec4 & specular) { m_specular = specular; }
				void setAmbient(const Vec4 & ambient) { m_ambient = ambient; }
				void setEmission(const Vec4 & emission) { m_emission = emission; }
				
				void setSpotDirection(const Vec3 & spotDirection) { m_spotDirection = spotDirection; }
				void setSpotExponent(const RealT & spotExponent) { m_spotExponent = spotExponent; }
				void setSpotCutoff(const RealT & spotCutoff) { m_spotCutoff = spotCutoff; }

				void setAttenuation(const Vec3 & attenuation) { m_attenuation = attenuation; }
			};
			
			class LightingGroup : public Group
			{
				typedef std::vector<REF(Light)> LightsT;
				
				protected:
					std::vector<REF(Light)> m_lights;
					Vec4 m_color;
					
				public:
					void addLight(PTR(Light) light) { m_lights.push_back(light); }

					void setColor(const Vec4 & color) { m_color = color; }
					
					virtual void renderFrameForTime (IScene * scene, TimeT time);
			};
			
		}
	}
}

#endif
