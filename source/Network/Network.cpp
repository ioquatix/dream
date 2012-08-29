//
//  Network/Network.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 13/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Network.h"

namespace Dream
{
	namespace Network
	{
		ConnectionError::ConnectionError (const std::string & what) : std::runtime_error(what)
		{
		}

		ConnectionResetByPeer::ConnectionResetByPeer (const std::string & what) : ConnectionError(what)
		{
		}

		ConnectionShutdown::ConnectionShutdown (const std::string & what) : _what(what)
		{
		}
	}
}
