//
//  Core/Algorithm.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 11/05/12.
//  Copyright (c) 2012 Samuel Williams. All rights reserved.
//

#ifndef _DREAM_CORE_ALGORITHM_H
#define _DREAM_CORE_ALGORITHM_H

namespace Dream {
	namespace Core {
		/// Provides an efficient way to erase elements from an unsorted std::vector
		template <typename t>
		bool erase_element_at_index (std::size_t index, std::vector<t> & array)
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
