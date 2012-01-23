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
				ErrorNumberT m_errorNumber;
				StringT m_message;
				StringT m_formattedMessage;
			
			public:
				SystemError(StringT domain, ErrorNumberT errorNumber, StringT errorDescription, StringT errorTarget);
				SystemError(StringT message);
				
				StringT what () const;
				
				static void check (StringT what);
				static void reset ();
		};

	}
}

#endif
