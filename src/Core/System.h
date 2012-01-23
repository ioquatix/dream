//
//  Core/System.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/12/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_CORE_SYSTEM_H
#define _DREAM_CORE_SYSTEM_H

#include "../Framework.h"
#include "Strings.h"

namespace Dream
{
	namespace Core
	{
	
		typedef int ErrorNumberT;
	
		class SystemError
		{
			protected:
				ErrorNumberT _errorNumber;
				StringT _message;
				StringT _formatted_message;
			
			public:
				SystemError(StringT domain, ErrorNumberT error_number, StringT error_description, StringT error_target);
				SystemError(StringT message);
				
				StringT what () const;
				
				static void check (StringT what);
				static void reset ();
		};

	}
}

#endif
