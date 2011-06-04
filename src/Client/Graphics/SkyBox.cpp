/*
 *  SkyBox.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 2/05/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "SkyBox.h"
#include "../Display/Scene.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			
			
			SkyBox::SkyBox (const String & name, const RealT & size) : m_name(name), m_size(size) {

			}

			SkyBox::SkyBox(const RealT & size) : m_size(size) {
				
			}
			
			void SkyBox::renderFrameForTime (IScene * scene, TimeT time)
			{
				const Vector<3, unsigned> K(IDENTITY, 2);
				const GLushort INDICES[] = {
					0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
					12, 13, 3, 5, 5, 2, 2, 4, 10, 11
				};

				std::vector<Vec2> texCoords(14);
				std::vector<Vec3> vertices(14);
				for (std::size_t i = 0; i < 5; i += 1) {
					texCoords[i*2] = Vec2(0.25*i, 1.0/3.0);
					texCoords[i*2+1] = Vec2(0.25*i, 2.0/3.0);
				}
				
				texCoords[10] = Vec2(0.25, 0.0);
				texCoords[11] = Vec2(0.5, 0.0);
				texCoords[12] = Vec2(0.25, 1.0);
				texCoords[13] = Vec2(0.5, 1.0);
				
				AlignedBox<3> box(-m_size, m_size);
				vertices[0] = box.corner(vec(false, false, false));
				vertices[1] = box.corner(vec(true, false, false));
				vertices[2] = box.corner(vec(false, true, false));
				vertices[3] = box.corner(vec(true, true, false));
				vertices[4] = box.corner(vec(false, true, true));
				vertices[5] = box.corner(vec(true, true, true));
				vertices[6] = box.corner(vec(false, false, true));
				vertices[7] = box.corner(vec(true, false, true));
				vertices[8] = vertices[0];
				vertices[9] = vertices[1];
				
				vertices[10] = vertices[0];
				vertices[11] = vertices[6];
				vertices[12] = vertices[1];
				vertices[13] = vertices[7];
				
				glVertexPointer(3, GLTypeTraits<RealT>::TYPE, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glTexCoordPointer(2, GLTypeTraits<RealT>::TYPE, 0, &texCoords[0]);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				scene->renderer()->textureController()->unit(0)->enable(m_crossTexture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glDrawElements(GL_TRIANGLE_STRIP, sizeof(INDICES) / sizeof(GLushort), GLTypeTraits<GLushort>::TYPE, INDICES);
				scene->renderer()->textureController()->unit(0)->disable();
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			
			void SkyBox::didBecomeCurrent (ISceneManager * manager, IScene * scene)
			{
				if (!m_name.empty())
				{
					TextureParameters params;
					params.generateMipMaps = true;
					params.target = GL_TEXTURE_2D;

					REF(IPixelBuffer) image = manager->resourceLoader()->load<IPixelBuffer>(m_name);
					m_crossTexture = scene->renderer()->textureController()->createTexture(image, params);
				}				
			}
			
			void SkyBox::willRevokeCurrent (ISceneManager * manager, IScene * scene)
			{
				m_crossTexture = NULL;
			}
			
		}
	}
}
