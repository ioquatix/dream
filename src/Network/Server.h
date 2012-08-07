//
//  Network/Server.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_NETWORK_SERVER_H
#define _DREAM_NETWORK_SERVER_H

#include "Socket.h"

#include "../Events/Loop.h"

// Threading is often needed for coordinating network servers and clients
// But- a multi-threaded server model (one thread per client) is not advised.
#include <thread>

namespace Dream {
	namespace Network {
		/** A headless controller that manages client connections.

		 The server class is designed to provide a very lightweight container for ServerSockets. As it is possible that there is more than one port for incoming
		 connections (IPv4, IPv6, etc), there may be more than one ServerSocket instance accept()ing incoming connections. Thus, this class can be initialized
		 with a service name and socket type, and will automatically construct all the required ServerSocket instances.

		 It is generally expected that one would subclass and override connection_callback to instantiate an appropriate subclass of ClientSocket, and
		 begin communcation. You also need to override the constructor to call bind_to_service() with appropriate parameters.

		 This class, when overriddden correctly, can act as a gatekeeper, checking the remote address or resource limits, and closing connections depending on
		 circumstances.

		 */
		class Server : public Object {
		protected:
			/// The list of server sockets that are currently accepting connections.
			std::vector<Ref<ServerSocket>> _server_sockets;

			/// The server runloop.
			Ref<Events::Loop> _event_loop;

			/// Override this function to handle incoming connection requests.
			virtual void connection_callback (Events::Loop *, ServerSocket *, const SocketHandleT & h, const Address &) abstract;

			/// Creates a set of sockets bound to the appropriate service.
			/// You need to call this in your subclass to bind to the appropriate ports/services.
			/// This call will schedule any new ServerSocket instances in the attached runloop.
			void bind_to_service (const char * service, SocketType sock_type);

		public:
			/// A server attaches to a runloop. It then should schedule incoming connections on the runloop.
			Server (Ref<Events::Loop> event_loop);

			virtual ~Server ();
		};

		// MARK: -
		// MARK: Threaded Net Server Container

		/** Creates a thread to run an instance of Server.

		 When creating software which manages a server or set of servers, it is useful to have some kind of multi-threaded container for different instances
		 of the server. This allows multiple cores to be used when the client and server are both on the local machine, for example.

		 A server container provides all the needed infrastructure (such as runloop) to run the server correctly, and is designed to provide a very simple
		 interface to starting and stopping a server thread.

		 */
		class ServerContainer : public Object {
		protected:
			bool _run;
			Ref<Events::Loop> _event_loop;

			Ref<Server> _server;
			Shared<std::thread> _thread;

			void run ();

		public:
			/// Construct a server container. This initializes a runloop.
			ServerContainer ();
			virtual ~ServerContainer ();

			/// The runloop for the container. Be careful about accessing this from a different thread.
			Ref<Events::Loop> event_loop ();

			/// Start the container with a given server.
			void start (Ref<Server> server);

			/// Stop the container. May interrupt the server thread if it does not stop in a reasonable timeframe.
			/// (at present, is very nice and won't interrupt, but it may do in future implementation!)
			void stop ();
		};
	}
}

#endif
