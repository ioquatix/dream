//
//  Core/STLAdditions.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 4/05/09.
//  Copyright (c) 2009 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_STLADDITIONS_H
#define _DREAM_CORE_STLADDITIONS_H

#include "Core.h"
#include <vector>
#include <queue>

namespace Dream
{
	namespace Core
	{
	
		/// Provides an efficient way to erase elements from an unsorted std::vector
		template <typename t>
		bool erase_element_at_index (IndexT index, std::vector<t> & array)
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
