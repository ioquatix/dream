/*
 *  Imaging/PixelBufferSaver.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 15/05/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_IMAGING_PIXELBUFFERSAVER_H
#define _DREAM_IMAGING_PIXELBUFFERSAVER_H

#include "PixelBuffer.h"
#include "../Core/Data.h"

namespace Dream
{
	namespace Imaging
	{
		
		REF(Core::Data) savePixelBufferAsPNG (IPixelBuffer * pixelBuffer);
		
	}
}

#endif
