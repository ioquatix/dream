//
//  Imaging/PixelBufferSaver.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 15/05/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_IMAGING_PIXELBUFFERSAVER_H
#define _DREAM_IMAGING_PIXELBUFFERSAVER_H

#include "PixelBuffer.h"
#include "../Core/Data.h"

namespace Dream
{
	namespace Imaging
	{
		
		Ref<Core::IData> save_pixel_buffer_as_png (IPixelBuffer * pixel_buffer);
		
	}
}

#endif
