/*
 *  Client/Display/Renderer.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 21/04/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CLIENT_DISPLAY_RENDERER_H
#define _DREAM_CLIENT_DISPLAY_RENDERER_H

#ifdef DREAM_USE_OPENGL20
#include "OpenGL20/OpenGL20.h"
#include "OpenGL20/Renderer.h"
#include "OpenGL20/Shader.h"
#include "OpenGL20/Texture.h"
#include "OpenGL20/RenderState.h"
#elif DREAM_USE_OPENGL31
#include "OpenGL31/OpenGL31.h"
#include "OpenGL31/Renderer.h"
#elif DREAM_USE_OPENGLES11
#include "OpenGLES11/OpenGLES11.h"
#include "OpenGLES11/Renderer.h"
#include "OpenGLES11/RenderState.h"
#endif

#endif
