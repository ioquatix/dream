/*
 *  SkyBox.h
 *  Dream
 *
 *  Created by Samuel Williams on 2/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_GRAPHICS_SKYBOX_H
#define _DREAM_CLIENT_GRAPHICS_SKYBOX_H

#include "Graphics.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			class SkyBox : public Object, IMPLEMENTS(Layer) {
				EXPOSE_CLASS(SkyBox)
				
				class Class : public Object::Class, IMPLEMENTS(Layer::Class) {
				public:
					EXPOSE_CLASSTYPE
				};
				
				static const int SIDES = 6;
				
			protected:
				String m_name;
				RealT m_size;
				
				REF(Texture) m_crossTexture;
				
			public:				
				SkyBox(const String & name, const RealT & size);
				SkyBox(const RealT & size);
				
				void setSize(RealT size) { m_size = size; }
				
				virtual void renderFrameForTime (IScene * scene, TimeT time);
				
				virtual void didBecomeCurrent (ISceneManager * manager, IScene * scene);
				virtual void willRevokeCurrent (ISceneManager * manager, IScene * scene);
			};
		}
	}
}

#endif
