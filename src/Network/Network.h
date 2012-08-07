//
//  Network/Network.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_NETWORK_NETWORK_H
#define _DREAM_NETWORK_NETWORK_H

#include "../Framework.h"

#include "../Core/Endian.h"
#include "../Core/Data.h"
#include "../Events/Loop.h"

#include <exception>
#include <stdexcept>

namespace Dream {
	/// Network related functions for clients and servers.
	namespace Network {
		/** Indicates that a connection error of some sort has occurred.

		 */
		class ConnectionError : public std::runtime_error {
		public:
			explicit ConnectionError (const std::string & what);
		};

		/** Indicates that the socket has disconnected, typically by error (e.g. phsysical disconnect).

		 @sa Socket::close
		 */
		class ConnectionResetByPeer : public ConnectionError {
		public:
			explicit ConnectionResetByPeer (const std::string & what);
		};

		/** Indiciates that the operation cannot complete because the pipe has been shutdown.

		 If you attempt to read from the remote end and it has been shutdown, this error will be thrown.

		 @sa Socket::shutdown
		 */
		class ConnectionShutdown : public Events::FileDescriptorClosed {
		protected:
			const std::string & _what;

		public:
			explicit ConnectionShutdown (const std::string & what);
		};
	}
}

#endif
