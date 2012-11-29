//
//  Network/Socket.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 26/10/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Socket.h"

#include "../Events/Loop.h"
#include "../Events/Thread.h"
#include "../Events/Logger.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>

//#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>

namespace Dream {
	namespace Network {
		using namespace Events::Logging;

		/*
		 void print_backtrace(void) {
		 void *addresses[10];
		 char **strings;

		 int size = backtrace(addresses, 10);
		 strings = backtrace_symbols(addresses, size);
		 printf("Stack frames: %d\n", size);
		 for(int i = 0; i < size; i++)
		 {
		 //printf("%d: %X\n", i, (int)addresses[i]);
		 printf("%s\n", strings[i]);
		 }
		 free(strings);
		 }
		 */
		//	using namespace std;

// MARK: -
// MARK: class Socket

		Socket::Socket (int s) : _socket(s) {
		}

		Socket::Socket () : _socket(-1) {
		}

		Socket::~Socket () {
			if (is_valid()) {
				close();
			}
		}

		void Socket::open_socket (AddressFamily af, SocketType st, SocketProtocol sp) {
			DREAM_ASSERT(_socket == -1);

			_socket = ::socket(af, st, sp);
			// std::cerr << "=====> Opening socket " << _socket << std::endl;

			if (_socket == -1) {
				logger()->system_error("socket()");
			}
		}

		void Socket::open_socket (const Address & a) {
			open_socket(a.address_family(), a.socket_type(), a.socket_protocol());
		}

		Socket::Socket (AddressFamily af, SocketType st, SocketProtocol sp) : _socket(-1) {
			open_socket(af, st, sp);
		}

		void Socket::shutdown (int mode) {
			DREAM_ASSERT(is_valid());

			if (::shutdown(_socket, mode) == -1) {
				logger()->system_error(__func__);
			}
		}

		void Socket::close () {
			DREAM_ASSERT(is_valid());

			if (::close(_socket) == -1) {
				logger()->system_error("close()");
			}

			_socket = -1;
		}

		FileDescriptorT Socket::file_descriptor () const
		{
			return _socket;
		}

		void Socket::set_no_delay_mode (bool mode) {
			int flag = mode ? 1 : 0;

			// IPPROTO_TCP / SOL_SOCKET
			int result = setsockopt(_socket, SOL_SOCKET, TCP_NODELAY, (char*)&flag, sizeof(int));

			if (result < 0) {
				logger()->system_error(__func__);
			}
		}

		int Socket::socket_specific_error () const {
			DREAM_ASSERT(is_valid());

			int error = 0;
			socklen_t len = sizeof(error);

			//Get error code specific to this socket
			getsockopt(_socket, SOL_SOCKET, SO_ERROR, &error, &len);

			return error;
		}

		bool Socket::is_valid () const {
			return _socket != -1;
		}

		bool Socket::is_connected () const {
			sockaddr sa;
			socklen_t len = sizeof(sockaddr);

			int err = getpeername(_socket, &sa, &len);

			if (errno == ENOTCONN || err == -1) {
				return false;
			}

			return true;
		}

		void Socket::shutdown_read()
		{
			shutdown(SHUT_RD);
		}

		void Socket::shutdown_write()
		{
			shutdown(SHUT_WR);
		}

		std::size_t Socket::send (const Core::Buffer & buf, std::size_t offset, int flags) {
			DREAM_ASSERT(buf.size() > 0);
			DREAM_ASSERT(offset < buf.size());

			//std::cout << "Sending " << buf.size() << " bytes..." << std::endl;

			ssize_t sz = buf.size() - offset;

			const ByteT * data = buf.begin() + offset;

			sz = ::send(_socket, data, sz, flags);

			if (sz == 0)
				throw ConnectionShutdown("write shutdown");

			if (sz == -1) {
				if (errno == ECONNRESET)
					throw ConnectionResetByPeer("write error");

				logger()->system_error("send()");

				sz = 0;
			}

			return sz;
		}

		std::size_t Socket::recv (Core::ResizableBuffer & buf, int flags) {
			DREAM_ASSERT(buf.size() < buf.capacity() && "Please make sure you have reserved space for incoming data");

			//std::cout << "Receiving " << (buf.capacity() - buf.size()) << " bytes..." << std::endl;

			// Find out where we are writing data
			std::size_t offset = buf.size();

			// Firstly, we maximize size in one go
			buf.resize(buf.capacity());

			// We read the size in the buffer
			ssize_t sz = ::recv(_socket, (void*)&buf[offset], buf.size() - offset, flags);

			if (sz == 0)
				throw ConnectionShutdown("read shutdown");

			if (sz == -1) {
				if (errno == ECONNRESET)
					throw ConnectionResetByPeer("read error");

				logger()->system_error("recv()");

				sz = 0;
			}

			// We resize to
			buf.resize(offset + sz);

			return sz;
		}

// MARK: -
// MARK: class ServerSocket

		ServerSocket::ServerSocket (const Address &server_address, unsigned listen_count) {
			bind(server_address);
			listen(listen_count);

			set_will_block(false);

			logger()->log(LOG_INFO, LogBuffer() << "Server " << this << " starting on address: " << server_address.description() << " fd: " << file_descriptor());
		}

		ServerSocket::~ServerSocket () {
		}

		void ServerSocket::process_events (Events::Loop * event_loop, Events::Event events)
		{
			if (events & Events::READ_READY) {
				DREAM_ASSERT(!!connection_callback);

				SocketHandleT socket_handle;
				Address address;

				while (accept(socket_handle, address))
					connection_callback(event_loop, this, socket_handle, address);
			}
		}

		bool ServerSocket::bind (const Address & na, bool reuse_addr) {
			open_socket(na);

			DREAM_ASSERT(is_valid() && na.is_valid());

			if (reuse_addr) {
				set_reuse_address(true);
			}

			if (::bind(_socket, na.address_data(), na.address_data_size()) == -1) {
				logger()->system_error("bind()");

				return false;
			}

			_bound_address = na;

			return true;
		}

		void ServerSocket::listen (unsigned n) {
			if (::listen(_socket, n) == -1) {
				logger()->system_error("listen()");
			}
		}

		const Address & ServerSocket::bound_address () const
		{
			return _bound_address;
		}

		bool ServerSocket::accept (SocketHandleT & h, Address & na) {
			DREAM_ASSERT(is_valid());

			socklen_t len = sizeof(sockaddr_storage);
			sockaddr_storage ss;
			h = ::accept(_socket, (sockaddr*)&ss, &len);

			if (h == -1) {
				if (errno != EWOULDBLOCK) {
					logger()->system_error("accept()");
				}

				return false;
			}

			// Copy address
			na = Address(_bound_address, (sockaddr*)&ss, len);

			return true;
		}

		void ServerSocket::set_reuse_address (bool enabled) {
			DREAM_ASSERT(is_valid());

			int val = (int)enabled;
			int r = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));

			if (r == -1) {
				logger()->system_error("setsockopt(reuse address)");
			}
		}

// MARK: -
// MARK: class ClientSocket



		ClientSocket::ClientSocket(const SocketHandleT & h, const Address & address) {
			_socket = h;
			_remote_address = address;
		}

		ClientSocket::ClientSocket() {
		}

		ClientSocket::~ClientSocket () {
		}

		const Address & ClientSocket::remote_address () const
		{
			return _remote_address;
		}

		bool ClientSocket::connect (const Address & na) {
			DREAM_ASSERT(!is_valid());

			open_socket(na);

			DREAM_ASSERT(is_valid() && na.is_valid());

			if (::connect(_socket, na.address_data(), na.address_data_size()) == -1) {
				if (errno == ECONNRESET) {
					throw ConnectionResetByPeer("connect error");
				} else {
					logger()->system_error("connect()");
				}

				return false;
			}

			_remote_address = na;

			return true;
		}

		bool ClientSocket::connect (const AddressesT & addresses)
		{
			for (auto address : addresses) {
				if (connect(address))
					return true;
			}

			return false;
		}

		void ClientSocket::process_events (Events::Loop * event_loop, Events::Event events)
		{
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING

		bool global_message_sent;
		std::size_t global_message_length_sent, global_message_length_received;
		bool global_client_connected;
		bool global_message_received;

		const std::string global_message("Hello World!");

		class TestClientSocket : public ClientSocket {
		public:
			TestClientSocket (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
			{
				Core::StaticBuffer buf = Core::StaticBuffer::for_cstring(global_message.c_str(), false);

				std::cerr << "Sending message from " << this << "..." << std::endl;

				global_message_length_sent = send(buf);

				std::cerr << global_message_length_sent << " bytes sent" << std::endl;
				global_message_sent = true;
			}

			TestClientSocket ()
			{
			}

			virtual void process_events(Events::Loop * event_loop, Events::Event events)
			{
				if (events & Events::READ_READY) {
					Core::DynamicBuffer buf(1024, true);

					recv(buf);

					global_message_received = true;
					global_message_length_received = buf.size();

					std::string incominglobal_message(buf.begin(), buf.end());

					std::cerr << "Message received by " << this << " fd " << this->file_descriptor() << " : " << incominglobal_message << std::endl;

					global_message_received = (global_message == incominglobal_message);

					event_loop->stop_monitoring_file_descriptor(this);
				}
			}
		};

		class TestServerSocket : public ServerSocket {
			Ref<TestClientSocket> _test_socket;

		public:
			virtual void process_events(Events::Loop * event_loop, Events::Event events)
			{
				if (events & Events::READ_READY & !_test_socket) {
					std::cerr << "Test server has received connection..." << std::endl;
					global_client_connected = true;

					SocketHandleT h;
					Address a;
					if (accept(h, a))
						_test_socket = new TestClientSocket(h, a);

					event_loop->monitor(_test_socket);

					event_loop->stop_monitoring_file_descriptor(this);
				} else {
					std::cerr << "More than one connection received!" << std::endl;
				}
			}

			TestServerSocket (const Address &server_address, unsigned listen_count = 100) : ServerSocket(server_address, listen_count)
			{
			}

			virtual ~TestServerSocket ()
			{
				std::cerr << "Server shutting down..." << std::endl;
			}
		};



		static void stop_callback (Events::Loop * event_loop, Events::TimerSource *, Events::Event event)
		{
			std::cerr << "Stopping test" << std::endl;
			event_loop->stop();
		}

		UNIT_TEST(Socket) {
			testing("Network Communication");
			global_client_connected = global_message_sent = global_message_received = false;
			global_message_length_sent = global_message_length_received = 0;

			Ref<Events::Loop> event_loop = new Events::Loop;

			// This is a very simple example of a network server listening on a single port.
			// This server can only accept one connection

			{
				Address localhost = Address::interface_addresses_for_port(2000, SOCK_STREAM)[0];
				std::cerr << "Initializing server..." << std::endl;
				Ref<TestServerSocket> server_socket = new TestServerSocket(localhost);
				std::cerr << "Initializing client..." << std::endl;
				Ref<TestClientSocket> client_socket = new TestClientSocket;

				std::cerr << "Connecting to server..." << std::endl;
				client_socket->connect(localhost);

				event_loop->monitor(server_socket);
				event_loop->monitor(client_socket);
				event_loop->schedule_timer(new Events::TimerSource(stop_callback, 1));
			}

			event_loop->run_forever ();
			event_loop = NULL;

			check(global_client_connected) << "Client connected";
			check(global_message_sent) << "Message sent";
			check(global_message_length_sent == global_message_length_received) << "Message length is correct";
			check(global_message_received) << "Message received";
		}
#endif
	}
}
