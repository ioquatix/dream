/*
 *  TexturedShapes.cpp
 *  Dream
 *
 *  Created by Samuel Williams on 14/09/10.
 *  Copyright 2010 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "TexturedShapes.h"
#include "../Display/Scene.h"

namespace Dream {
	namespace Client {
		namespace Graphics {
			
			void buildSphere (Model & model, const RealT & radius, unsigned stacks, unsigned slices)
			{
				// In order to texture properly, we need to add in one more slice where tex coords can be 1.0
				slices += 1;
				
				for (unsigned int stackNumber = 0; stackNumber <= stacks; ++stackNumber)
				{
					for (unsigned int sliceNumber = 0; sliceNumber < slices; ++sliceNumber)
					{
						RealT theta = stackNumber * M_PI / stacks;
						RealT phi = sliceNumber * 2 * M_PI / (slices - 1);
						RealT sinTheta = Math::sin(theta);
						RealT sinPhi = Math::sin(phi);
						RealT cosTheta = Math::cos(theta);
						RealT cosPhi = Math::cos(phi);
						
						Vec2 tc((RealT)sliceNumber / (RealT)(slices - 1), (RealT)stackNumber / (RealT)stacks);
						Vec3 pt(radius * cosPhi * sinTheta, radius * sinPhi * sinTheta, radius * cosTheta);
						
						model.vertexArray.push_back(pt);
						model.normalArray.push_back(pt.normalizedVector());
						model.texCoordArray.push_back(tc);				
					}
				}
				
				for (unsigned int stackNumber = 0; stackNumber < stacks; ++stackNumber)
				{
					for (unsigned int sliceNumber = 0; sliceNumber <= slices; ++sliceNumber)
					{
						model.indexArray.push_back((stackNumber * slices) + (sliceNumber % slices));
						model.indexArray.push_back(((stackNumber + 1) * slices) + (sliceNumber % slices));
					}
				}	
			}
			
			void buildPlane (Model & model, Vec2u div, Vec2 size, Vec2 offset, RealT z)
			{
				Vec2 factor = size / div;
				
				for (unsigned y = 0; y <= div[Y]; y += 1)
				{
					for (unsigned x = 0; x <= div[X]; x += 1)
					{
						// Adust this to change front-facing / back-facing behaviour.
						Vec2 r(x, div[Y] - y);
						Vec3 pt = ((r * factor) - offset) << z;
						
						model.vertexArray.push_back(pt);
						model.normalArray.push_back(Vec3(0.0, 0.0, 1.0));
						model.texCoordArray.push_back((pt.reduce() / 320.0) * 2.0);
					}
				}
				
				bool first = true;
				unsigned row = div[X] + 1;
				for (unsigned y = 0; y < div[Y]; y += 1)
				{
					// Add some degenerate triangles so we can begin the next row
					if (!first) {
						//std::cout << "[" << ((row - 1) + (y * row)) << "] ";
						
						// Add the last index of the row
						model.indexArray.push_back((row - 1) + (y * row));
						// Add the first index of the row
						model.indexArray.push_back(    0     + (y * row));
					} else {
						first = false;
					}
					
					for (unsigned x = 0; x < row; x += 1)
					{
						//std::cout << (x + (   y    * row)) << " " << (x + ((y + 1) * row)) << ";";
						
						model.indexArray.push_back(x + (   y    * row));
						model.indexArray.push_back(x + ((y + 1) * row));
					}
					
					std::cout << std::endl;
				}	
			}
			
			void drawModel (Model & model)
			{	
				glVertexPointer(3, GL_FLOAT, 0, &model.vertexArray[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glNormalPointer(GL_FLOAT, 0, &model.normalArray[0]);
				glEnableClientState(GL_NORMAL_ARRAY);
				
				glTexCoordPointer(2, GL_FLOAT, 0, &model.texCoordArray[0]);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				glDrawElements(GL_TRIANGLE_STRIP, model.indexArray.size(), GL_UNSIGNED_SHORT, &model.indexArray[0]);	
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
			
#pragma mark -

			

				ModelRenderer::ModelRenderer(Model & model, PTR(RenderState) renderState)
					: m_model(model), m_renderState(renderState)
				{
				
				}

				void ModelRenderer::renderFrameForTime (IScene * scene, TimeT time)
				{
					scene->renderer()->updateCurrentRenderState(m_renderState);
					drawModel(m_model);
					scene->renderer()->updateCurrentRenderState(NULL);
				}
			
#pragma mark -
			
			
			
			TexturedBox::TexturedBox (const StringT & name, const RealT & size, const RealT & inset)
				: m_name(name), m_size(size), m_inset(inset)
			{

			}

			TexturedBox::TexturedBox(const RealT & size) : m_size(size) {
				
			}
			
			void TexturedBox::setInset(RealT inset)
			{
				m_inset = inset;
			}
			
			void TexturedBox::setVerticalRotation(RealT rotation)
			{
				m_rotation = rotation;
			}
			
			void TexturedBox::setSize(RealT size)
			{
				m_size = size;
			}
			
			void TexturedBox::setTexture (Side s, PTR(Texture) tex) {
				m_sides[s] = tex;
			}
			
			void TexturedBox::renderFrameForTime (IScene * scene, TimeT time)
			{
				std::vector<Vec3> vertices;
				std::vector<Vec2> texCoords;
				std::vector<Vec3> normals;
				
				// Reduce the chance of seams showing up.
				RealT e = m_inset - m_size;
				
				vertices.push_back(vec(e, -m_size, -m_size));
				texCoords.push_back(vec(0, 0));
				normals.push_back(vec(-1, 0, 0));
				
				vertices.push_back(vec(e, -m_size, m_size));
				texCoords.push_back(vec(0, 1));
				normals.push_back(vec(-1, 0, 0));
				
				vertices.push_back(vec(e, m_size, -m_size));
				texCoords.push_back(vec(1, 0));
				normals.push_back(vec(-1, 0, 0));
				
				vertices.push_back(vec(e, m_size, m_size));
				texCoords.push_back(vec(1, 1));
				normals.push_back(vec(-1, 0, 0));
				
				glColor4f(1.0, 1.0, 1.0, 1.0);
				
				glVertexPointer(3, GLTypeTraits<RealT>::TYPE, 0, &vertices[0]);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				glTexCoordPointer(2, GLTypeTraits<RealT>::TYPE, 0, &texCoords[0]);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				
				glNormalPointer(GLTypeTraits<RealT>::TYPE, 0, &normals[0]);
				glEnableClientState(GL_NORMAL_ARRAY);
				
				PTR(TextureUnit) textureUnit = scene->renderer()->textureController()->unit(0);
				
				glPushMatrix();
				for (std::size_t i = 0; i < 4; i++) {
					textureUnit->enable(m_sides[i]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glRotatef(-90, 0, 0, 1);
				}
				
				glRotatef(-90, 0, 1, 0);
				
				for (std::size_t i = 4; i < 6; i++) {
					textureUnit->enable(m_sides[i]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
					glRotatef(180, 0, 0, 1);
				}				
				
				glPopMatrix();
				
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_NORMAL_ARRAY);
				
				textureUnit->disable();
			}
			
			void TexturedBox::didBecomeCurrent (ISceneManager * manager, IScene * scene) {
				const char * names[] = {"north", "east", "south", "west", "down", "up"};
				
				if (!m_name.empty()) {
					TextureParameters params;
					params.generateMipMaps = true;
					params.target = GL_TEXTURE_2D;

					for (unsigned i = 0; i < SIDES; i += 1) {
						StringStreamT name;
						name << m_name << "_" << names[i];
					
						REF(IPixelBuffer) image = manager->resourceLoader()->load<IPixelBuffer>(name.str());
						
						if (image)
							m_sides[i] = scene->renderer()->textureController()->createTexture(image, params);
						
						if (!m_sides[i] && i != 0)
							m_sides[i] = m_sides[i-1];
					}
				}				
			}
			
			void TexturedBox::willRevokeCurrent (ISceneManager * manager, IScene * scene) {
				for (unsigned i = 0; i < SIDES; i += 1) {
					m_sides[i] = NULL;
				}
			}
			
		}
	}
}
