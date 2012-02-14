//
//  WorkQueue.h
//  Dream
//
//  Created by Samuel Williams on 14/02/12.
//  Copyright (c) 2012 Orion Transfer Ltd. All rights reserved.
//

#ifndef WorkQueue
#define WorkQueue

namespace Dream {
	namespace Events {
	
		template <typename ElementT>
		class WorkQueue {
		protected:

		public:
			struct Range {
				std::size_t offset, length, end;
			};
			
		};
		
	}
}

#endif
