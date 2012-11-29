//
//  Network/Server.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 17/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Server.h"

#include "../Core/Timer.h"
#include "../Events/Loop.h"

namespace Dream {
	namespace Network {
		using namespace Events;
		using std::bind;

// MARK: -
// MARK: ServerContainer

		ServerContainer::ServerContainer () : _run(false)
		{
			_event_loop = new Loop;
		}

		ServerContainer::~ServerContainer ()
		{
			stop();
		}

		void ServerContainer::run ()
		{
			std::cout << "Server container running..." << std::endl;

			_event_loop->run_forever();

			std::cout << "Server container stopped." << std::endl;
		}

		Ref<Loop> ServerContainer::event_loop ()
		{
			return _event_loop;
		}

		void ServerContainer::start (Ref<Server> server) {
			if (!_run) {
				_server = server;

				_run = true;

				std::cerr << "Starting server container..." << std::endl;

				DREAM_ASSERT(!_thread);

				_thread = new std::thread(std::bind(&ServerContainer::run, this));
			}
		}

		void ServerContainer::stop () {
			if (_run) {
				std::cerr << "Stopping server container..." << std::endl;

				// Stop the runloop
				_event_loop->stop();

				_thread->join();
				_thread = NULL;

				_run = false;
			}
		}

// MARK: -
// MARK: class Server



		Server::Server (Ref<Loop> event_loop) : _event_loop(event_loop)
		{
		}

		Server::~Server ()
		{
			if (_event_loop) {
				for (auto server_socket : _server_sockets)
				{
					_event_loop->stop_monitoring_file_descriptor(server_socket);
				}
			}
		}

		void Server::bind_to_service (const char * service, SocketType sock_type)
		{
			AddressesT server_addresses = Address::interface_addresses_for_service(service, sock_type);

			for (auto address : server_addresses) {
				Ref<ServerSocket> server_socket(new ServerSocket(address));
				server_socket->connection_callback = std::bind(&Server::connection_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

				_server_sockets.push_back(server_socket);

				_event_loop->monitor(server_socket);
			}
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING

		int global_message_received_count;
		class TestServerClientSocket : public ClientSocket {
		public:
			TestServerClientSocket (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
			{
			}

			virtual void process_events(Loop * event_loop, Event events)
			{
				if (events & Events::READ_READY) {
					DynamicBuffer buf(1024, true);

					recv(buf);

					std::string incominglobal_message(buf.begin(), buf.end());

					global_message_received_count += 1;

					std::cerr << "Message received by " << this << " fd " << this->file_descriptor() << " : " << incominglobal_message << std::endl;

					event_loop->stop_monitoring_file_descriptor(this);
				}
			}
		};

		class TestServer : public Server {
		protected:
			// (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
			virtual void connection_callback (Loop * event_loop, ServerSocket * server_socket, const SocketHandleT & h, const Address & a)
			{
				Ref<ClientSocket> client_socket(new TestServerClientSocket(h, a));

				std::cerr << "Accepted connection " << client_socket << " from " << client_socket->remote_address().description();
				std::cerr << " (" << client_socket->remote_address().address_family_name() << ")" << std::endl;

				event_loop->monitor(client_socket);
			}

		public:
			TestServer (Ref<Loop> event_loop, const char * service_name, SocketType socket_type) : Server(event_loop)
			{
				bind_to_service(service_name, socket_type);
			}

			virtual ~TestServer ()
			{
			}
		};

		Ref<TimerSource> global_timers[3];

		static void stop_timers_callback (Loop * event_loop, TimerSource *, Event event)
		{
			std::cerr << "Stoping connection timers..." << std::endl;

			global_timers[0]->cancel();
			global_timers[1]->cancel();
			global_timers[2]->cancel();
		}

		static void stop_callback (Loop * event_loop, TimerSource *, Event event)
		{
			std::cerr << "Stopping test" << std::endl;
			event_loop->stop();
		}

		int global_message_sent_count;
		int global_address_index;
		AddressesT global_connect_addresses;
		static void connect_callback (Loop * event_loop, TimerSource *, Event event)
		{
			Ref<ClientSocket> test_connection(new ClientSocket);

			test_connection->connect(global_connect_addresses[global_address_index++ % global_connect_addresses.size()]);

			StaticBuffer buf = StaticBuffer::for_cstring("Hello World?", false);

			global_message_sent_count += 1;
			test_connection->send(buf);

			test_connection->close();
		}

		UNIT_TEST(Server) {
			testing("Connecting and Message Sending");

			Ref<Loop> event_loop = new Loop;
			Ref<TestServer> server = new TestServer(event_loop, "7979", SOCK_STREAM);

			global_address_index = 0;
			global_message_received_count = 0;
			global_message_sent_count = 0;

			global_connect_addresses = Address::addresses_for_name("localhost", "7979", SOCK_STREAM);

			event_loop->schedule_timer(new TimerSource(stop_timers_callback, 0.4));
			event_loop->schedule_timer(new TimerSource(stop_callback, 0.5));

			global_timers[0] = new TimerSource(connect_callback, 0.05, true);
			event_loop->schedule_timer(global_timers[0]);

			global_timers[1] = new TimerSource(connect_callback, 0.1, true);
			event_loop->schedule_timer(global_timers[1]);

			global_timers[2] = new TimerSource(connect_callback, 0.11, true);
			event_loop->schedule_timer(global_timers[2]);

			event_loop->run_forever();

			check(global_message_sent_count >= 1) << "Messages sent";
			check(global_message_sent_count == global_message_received_count) << "Messages sent and received successfully";
		}

#endif
	}
}
