/*
 *  Client/Graphics/MaterialLibraryParserDetail.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#include "MaterialLibraryParser.h"

namespace Dream
{
	namespace Client
	{
		namespace Graphics
		{
			namespace Detail
			{
				using namespace std;
				using namespace boost::spirit::classic;
				using namespace boost;
				
				struct MaterialsParserState {
					MaterialsParserState (MaterialLibrary * library, const ILoader * loader) {
						this->materialLibrary = library;
						this->loader = loader;
					}
					
					const ILoader * loader;
					
					REF(MaterialFactory) material;
					MaterialLibrary * materialLibrary;
					
					String defineKey, defineValue;
					TextureUnitProperties textureProperty;
					
					String blendFuncSrc, blendFuncDst;
				};
				
				struct CommentsParser : public grammar<CommentsParser> {
					template <typename ScannerT>
					struct definition {
						definition(CommentsParser const& /*self*/) {
							skip
							=   space_p
							|   comment_p("//")                 // C++ comment
							|   comment_p("/*", "*/")           // C comment
							;
						}
						
						rule<ScannerT> skip;
						
						rule<ScannerT> const& start() const { return skip; }
					};
				};
				
				
				struct MaterialsParser : public grammar<MaterialsParser> {
				public:
					MaterialsParser (MaterialsParserState & state) : m_state(state) {
					}
					
					MaterialsParserState & m_state;
					
					// Blend Function
					void setBlendFuncSrc(const ByteT *s, const ByteT *e) const {
						m_state.blendFuncSrc = String(s, e);
					}
					
					void setBlendFuncDst(const ByteT *s, const ByteT *e) const {
						m_state.blendFuncDst = String(s, e);
					}
					
					void finishBlendProperty () const {
						m_state.material->setBlendFunc(m_state.blendFuncSrc, m_state.blendFuncDst);
					}
					
					// Shader Defines
					void setDefineKey(const ByteT *s, const ByteT *e) const {
						m_state.defineKey = String(s, e);
					}
					
					void setDefineValue(const ByteT *s, const ByteT *e) const {
						m_state.defineValue = String(s, e);
					}
					
					void finishDefineProperty () const {
						m_state.material->setDefine(m_state.defineKey, m_state.defineValue);
					}
					
					// Texturing
					void setTexturePath (const ByteT *s, const ByteT *e) const {
						m_state.textureProperty.resourcePath = String(s,e);
					}
					
					void setTextureName (const ByteT *s, const ByteT *e) const {
						m_state.textureProperty.name = String(s,e);
					}
					
					void setTextureMinFilter (const ByteT *s, const ByteT *e) const {
						m_state.textureProperty.minFilter = String(s,e);
					}
					
					void setTextureMagFilter (const ByteT *s, const ByteT *e) const {
						m_state.textureProperty.magFilter = String(s,e);
					}
					
					void setTextureMipMap (const ByteT *s, const ByteT *e) const {
						String value = String(s,e);
						m_state.textureProperty.generateMipMap = (value == "YES");
					}
					
					void setTextureTarget (const ByteT *s, const ByteT *e) const {
						m_state.textureProperty.target = String(s,e);
					}
					
					void finishTextureProperty () const {
						m_state.material->addTexture(m_state.textureProperty);
						
						TextureUnitProperties reset;
						m_state.textureProperty = reset;
					}
					
					void setShaderPath(const ByteT *s, const ByteT *e) const {
						String shaderPath(s, e);
						m_state.material->setShaderPath(shaderPath);
					}
					
					void beginNewMaterial(const ByteT *s, const ByteT *e) const {
						String materialName(s, e);
						m_state.material = new MaterialFactory(materialName, m_state.loader);
					}
					
					void finishedMaterial () const {
						m_state.materialLibrary->addMaterialFactory(m_state.material);
						m_state.material = NULL;
					}
					
					template <typename ScannerT>
					struct definition {
					protected:
						unsigned c;
						
					public:
						rule<ScannerT> booleanValue, textureTargetValue, minFilterValue, magFilterValue;
						rule<ScannerT> stringValue, commonBlendValue, srcBlendValue, dstBlendValue;
						rule<ScannerT> materials, materialDefinition;
						rule<ScannerT> materialProperty, defineProperty, shaderProperty, blendProperty;
						rule<ScannerT> textureProperty, textureAttributes;
						
						definition (const MaterialsParser & self) 
						{
#define CALLBACK(name) bind(&MaterialsParser::name, self, _1, _2)
							stringValue = lexeme_d[+(alnum_p | "-" | "_" | "/" | ".")];
							
							minFilterValue = (str_p("NEAREST_MIPMAP_NEAREST") | "LINEAR_MIPMAP_NEAREST" | "NEAREST_MIPMAP_LINEAR" | 
											  "LINEAR_MIPMAP_LINEAR" | "NEAREST" | "LINEAR");
							
							magFilterValue = str_p("NEAREST") | "LINEAR";
							
							textureTargetValue = str_p("1D") | "2D" | "3D";
							
							booleanValue = str_p("YES") | "NO";
							
							commonBlendValue = str_p("ZERO") | "ONE";
							
							srcBlendValue = (commonBlendValue | "DST_COLOR" |
											 "ONE_MINUS_DST_COLOR" | "SRC_ALPHA_SATURATE" | "SRC_ALPHA" |
											 "ONE_MINUS_SRC_ALPHA" | "DST_ALPHA" | 
											 "ONE_MINUS_DST_ALPHA"
											 );
							
							dstBlendValue = (commonBlendValue | "SRC_COLOR" | 
											 "ONE_MINUS_SRC_COLOR" | "SRC_ALPHA" | 
											 "ONE_MINUS_SRC_ALPHA" | "DST_ALPHA" |
											 "ONE_MINUS_DST_ALPHA"
											 );
							
							materials = *(materialDefinition) >> end_p;
							
							materialDefinition = str_p("material") >> stringValue[CALLBACK(beginNewMaterial)] >> *(materialProperty) >> 
							eps_p[bind(&MaterialsParser::finishedMaterial, self)];
							
							materialProperty = (
												defineProperty[bind(&MaterialsParser::finishDefineProperty, self)] | 
												shaderProperty |
												textureProperty[bind(&MaterialsParser::finishTextureProperty, self)] |
												blendProperty[bind(&MaterialsParser::finishBlendProperty, self)]
												);
							
							defineProperty = str_p("define") >> stringValue[CALLBACK(setDefineKey)] >> !stringValue[CALLBACK(setDefineValue)];
							shaderProperty = str_p("shader") >> stringValue[CALLBACK(setShaderPath)];
							blendProperty = str_p("blend") >> stringValue[CALLBACK(setBlendFuncSrc)] >> stringValue[CALLBACK(setBlendFuncDst)];
							textureProperty = str_p("texture") >> stringValue[CALLBACK(setTexturePath)] >> *(textureAttributes);
							
							textureAttributes = (
												 (str_p("name") >> stringValue[CALLBACK(setTextureName)]) |
												 (str_p("min-filter") >> minFilterValue[CALLBACK(setTextureMinFilter)]) |
												 (str_p("mag-filter") >> magFilterValue[CALLBACK(setTextureMagFilter)]) |
												 (str_p("mipmap") >> booleanValue[CALLBACK(setTextureMipMap)]) |
												 (str_p("target") >> textureTargetValue[CALLBACK(setTextureTarget)])
												 );
							
						}
						
						const rule<ScannerT> &start() const { 
							return materials;
						}
						
						static void debug (ByteT * s, ByteT * e) {
							std::cout << String(s,e) << std::endl;
						}
					};
				};
			}
			
			MaterialLibrary::MaterialLibrary(const REF(Data) data, const ILoader * loader) : m_loader(loader)
			{
				Detail::CommentsParser commentsParser;
				Detail::MaterialsParserState s(this, loader);
				Detail::MaterialsParser p(s);
				
				using namespace std;
				using namespace boost::spirit::classic;
				
				std::cout << "Parsing materials data..." << std::endl;
				parse_info<const ByteT *> info = parse(data->start(), data->start() + data->size(), p, commentsParser);
				
				if (!info.full) {
					cout << "Error found at location: " << info.stop << endl;
				} else {
					cout << "Success!" << endl;
					cout << "Listing loaded materials..." << endl;
					
					for (iterateEach(m_factories, f)) {
						cout << "material " << (*f).second->resourceName();
						cout << " shader " << (*f).second->shaderPath () << endl;
					}			
				}
			}
			
		}
	}
}