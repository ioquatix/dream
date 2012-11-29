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

// strerror_r
#include <string.h>

namespace Dream
{
	namespace Core
	{
		const unsigned int ERROR_DESCRIPTION_LENGTH = 1024;

		SystemError::SystemError(StringT message) : _error_number(-1), _message(message) {
		}

		SystemError::SystemError(StringT domain, ErrorNumberT error_number, StringT error_description, StringT error_target) : _error_number(error_number) {
			StringStreamT f;

			f << domain << " Error #" << error_number << ": " << error_description << "(" << error_target << ")";

			_message = f.str();
		}

		StringT SystemError::what () const
		{
			return _message;
		}

		void SystemError::check (StringT what)
		{
			if (errno != 0) {
				// Get the system error message.
				char error_description[ERROR_DESCRIPTION_LENGTH];
				strerror_r(errno, error_description, ERROR_DESCRIPTION_LENGTH);

				throw SystemError("System", errno, error_description, what);
			}
		}

		void SystemError::reset ()
		{
			errno = 0;
		}
	}
}
