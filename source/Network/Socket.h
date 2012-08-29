//
//  Network/Socket.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/10/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_NETWORK_SOCKET_H
#define _DREAM_NETWORK_SOCKET_H

#include "Address.h"
#include "../Core/Buffer.h"
#include "../Events/Source.h"

namespace Dream {
	namespace Network {
		/// Represents a system-level socket handle. On unix, this is generally an int.
		typedef int SocketHandleT;

		/** Represents a socket handle and provides a standard set of operations for sockets.

		 @sa ServerSocket
		 @sa ClientSocket
		 */
		class Socket : public Object, implements Events::IFileDescriptorSource {
		protected:
			SocketHandleT _socket;

			// Already connected socket
			Socket (SocketHandleT s);
			void open_socket (const Address & address);
			void open_socket (AddressFamily af, SocketType st, SocketProtocol sp = 0);

		public:
			/// Initialize an invalid socket. An invalid client socket needs to be connected before it can be used.
			Socket ();

			/// Create a socket with the given parameters.
			Socket (AddressFamily af, SocketType st, SocketProtocol sp = 0);

			virtual ~Socket ();

			/// Check whether an underlying socket handle is allocated.
			bool is_valid () const;

			/// Check if the underlying socket is connected to a remote peer using getpeername.
			bool is_connected () const;

			/// Shutdown the read end of the socket.
			void shutdown_read();
			/// Shutdown the write end of the socket.
			void shutdown_write();
			/// Shutdown either end of the socket, or both.
			void shutdown(int mode = SHUT_RDWR);

			/// Close and deallocate the socket. Generally, don't use this function. Use shutdown() instead.
			/// When the socket goes out of scope, it will be closed automatically.
			void close ();

			/// Return any errors that have occurred on the socket.
			int socket_specific_error () const;

			/// Write data to the socket.
			std::size_t send (const Core::Buffer & buf, std::size_t offset = 0, int flags = 0);

			/// Read data from the socket.
			/// Set buffer capacity before calling with buf.reserve(buf.size() + sz to read)
			/// We won't explicity allocate memory in this function
			/// @returns 0 when the remote peer has closed its end of the connection
			std::size_t recv (Core::ResizableBuffer & buf, int flags = 0);

			/// The internal file descriptor handle for the socket.
			virtual FileDescriptorT file_descriptor () const;

			/// Set the TCP NO DELAY option for the socket, if it is available.
			void set_no_delay_mode (bool mode);
		};

		/** A socket that can be bound to a local address and accept connections.

		 */
		class ServerSocket : public Socket {
		protected:
			Address _bound_address;

			/// Bind to the given address.
			bool bind (const Address & address, bool reuse_address = true);
			/// Listen for n incoming connections.
			void listen (unsigned n);
			/// Reuse the address if some other older socket was bound to it.
			void set_reuse_address (bool enabled);

		public:
			/// Create a socket that is bound to the supplied address.
			/// @sa Address::interface_addresses_for_port
			ServerSocket (const Address &server_address, unsigned listen_count = 1000);
			virtual ~ServerSocket ();

			/// Accept an incoming connection request. These details are then supplied to a ClientSocket to create a working connection.
			bool accept (SocketHandleT & h, Address & na);

			/// Returns the address the socket is bound to.
			const Address & bound_address () const;

			/// Fire of events. Generally handled via connection_callback.
			/// @sa connection_callback
			virtual void process_events (Events::Loop *, Events::Event);

			/// Delegate function to handle incoming connections.
			std::function<void (Events::Loop *, ServerSocket *, const SocketHandleT & h, const Address & na)> connection_callback;
		};

		/** A ClientSocket represents either the server-side or client-side endpoint of a connection.

		 A ClientSocket can be constructed from the arguments returned from ServerSocket::accept, or connect() to an existing service.
		 */
		class ClientSocket : public Socket {
		protected:
			Address _remote_address;

		public:
			/// Construct a client socket from an incoming connection from the given address.
			/// @sa ServerSocket::accept
			ClientSocket(const SocketHandleT & h, const Address & address);

			/// Construct a socket ready to be connected.
			/// @sa connect()
			ClientSocket();

			virtual ~ClientSocket ();

			/// The remote address that this socket is connected to.
			const Address & remote_address () const;

			/// Connect to a single address.
			/// @returns true if successfully connected.
			bool connect (const Address & address);

			/// Try connecting to a number of addresses in sequence, and return when a connection is made successfully.
			/// @returns true if successfully connected.
			bool connect (const AddressesT & addresses);

			/// Handle incoming events for the socket.
			virtual void process_events (Events::Loop *, Events::Event);
		};
	}
}

#endif
