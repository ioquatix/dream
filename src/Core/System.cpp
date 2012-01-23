//
//  Core/System.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 27/12/10.
//  Copyright (c) 2010 Samuel Williams. All rights reserved.
//
//

#include "System.h"

// For errno
#include <sys/errno.h>

namespace Dream
{
	namespace Core
	{
		const unsigned int ERROR_DESCRIPTION_LENGTH = 1024;
	
		SystemError::SystemError(StringT message)
			: m_message(message)
		{

		}
		
		SystemError::SystemError(StringT domain, ErrorNumberT errorNumber, StringT errorDescription, StringT errorTarget)
		{
			StringStreamT f;
		
			f << domain << " Error #" << errorNumber << ": " << errorDescription << "(" << errorTarget << ")";
			
			m_message = f.str();
		}
		
		StringT SystemError::what () const
		{
			return m_message;
		}

		void SystemError::check (StringT what)
		{
			if (errno != 0) {
				// Get the system error message.
				char errorDescription[ERROR_DESCRIPTION_LENGTH];
				strerror_r(errno, errorDescription, ERROR_DESCRIPTION_LENGTH);
				
				throw SystemError("System", errno, errorDescription, what);
			}
		}
	
		void SystemError::reset ()
		{
			errno = 0;
		}
	}
}
