/*
 *  Core/STLAdditions.h
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Samuel Williams on 4/05/09.
 *  Copyright 2009 Orion Transfer Ltd. All rights reserved.
 *
 */

#ifndef _DREAM_CORE_STLADDITIONS_H
#define _DREAM_CORE_STLADDITIONS_H

#include "Core.h"
#include <vector>
#include <queue>

namespace Dream
{
	namespace Core
	{
	
		/// Provides an efficient way to erase elements from a std::vector
		template <typename t>
		bool eraseElementAtIndex (IndexT index, std::vector<t> & array)
		{
			if (array.size() == (index+1)) {
				array.pop_back();
				return false;
			} else {
				array[index] = array.back();
				array.pop_back();
				return true;
			}
		}
		
	}
}

#endif
