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
		
#pragma mark -
#pragma mark ServerContainer
		
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
		
#pragma mark -
#pragma mark class Server

		
				
		Server::Server (Ref<Loop> event_loop) : _event_loop(event_loop)
		{
		}
		
		Server::~Server ()
		{
			if (_event_loop)
			{
				foreach(server_socket, _server_sockets)
				{
					_event_loop->stop_monitoring_file_descriptor(*server_socket);
				}
			}
		}
		
		void Server::bind_to_service (const char * service, SocketType sock_type)
		{
			AddressesT server_addresses = Address::interface_addresses_for_service(service, sock_type);
			
			foreach(addr, server_addresses) {
				Ref<ServerSocket> server_socket(new ServerSocket(*addr));
				server_socket->connection_callback = std::bind(&Server::connection_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

				_server_sockets.push_back(server_socket);

				_event_loop->monitor(server_socket);
			}
		}
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		
		int g_message_receivedCount;
		class TestServerClientSocket : public ClientSocket
		{
			public:
				TestServerClientSocket (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
				{
				}
				
				virtual void process_events(Loop * event_loop, Event events)
				{
					if (events & Events::READ_READY) {
						DynamicBuffer buf(1024, true);
						
						recv(buf);
						
						std::string incoming_message(buf.begin(), buf.end());
						
						g_message_receivedCount += 1;
						
						std::cerr << "Message received by " << this << " fd " << this->file_descriptor() << " : " << incoming_message << std::endl;
						
						event_loop->stop_monitoring_file_descriptor(this);
					}
				}
		};
		
		class TestServer : public Server
		{
		protected:
			// (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
			virtual void connection_callback (Loop * event_loop, ServerSocket * server_socket, const SocketHandleT & h, const Address & a)
			{
				Ref<ClientSocket> client_socket(new TestServerClientSocket(h, a));
				
				std::cerr << "Accepted connection " << client_socket << " from " << client_socket->remote_address().description();
				std::cerr << " (" << client_socket->remote_address().address_familyName() << ")" << std::endl;
				
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
		
		Ref<TimerSource> g_timer1, g_timer2, g_timer3;
		
		static void stop_timers_callback (Loop * event_loop, TimerSource *, Event event)
		{
			std::cerr << "Stoping connection timers..." << std::endl;
			
			g_timer1->cancel();
			g_timer2->cancel();
			g_timer3->cancel();	
		}
		
		static void stop_callback (Loop * event_loop, TimerSource *, Event event)
		{
			std::cerr << "Stopping test" << std::endl;
			event_loop->stop();
		}
		
		int g_messageSentCount;
		int g_addressIndex;
		AddressesT g_connectAddresses;
		static void connect_callback (Loop * event_loop, TimerSource *, Event event)
		{
			Ref<ClientSocket> test_connection(new ClientSocket);
			
			test_connection->connect(g_connectAddresses[g_addressIndex++ % g_connectAddresses.size()]);
			
			StaticBuffer buf = StaticBuffer::for_cstring("Hello World?", false);

			g_messageSentCount += 1;
			test_connection->send(buf);
			
			test_connection->close();
		}
		
		UNIT_TEST(Server) {
			testing("Connecting and Message Sending");
			
			Ref<Loop> event_loop = new Loop;
			Ref<TestServer> server = new TestServer(event_loop, "7979", SOCK_STREAM);
						
			g_addressIndex = 0;
			g_message_receivedCount = 0;
			g_messageSentCount = 0;
			
			g_connectAddresses = Address::addresses_for_name("localhost", "7979", SOCK_STREAM);

			event_loop->schedule_timer(new TimerSource(stop_timers_callback, 0.4));
			event_loop->schedule_timer(new TimerSource(stop_callback, 0.5));
			
			g_timer1 = new TimerSource(connect_callback, 0.05, true);
			event_loop->schedule_timer(g_timer1);
			
			g_timer2 = new TimerSource(connect_callback, 0.1, true);
			event_loop->schedule_timer(g_timer2);
			
			g_timer3 = new TimerSource(connect_callback, 0.11, true);
			event_loop->schedule_timer(g_timer3);

			event_loop->run_forever();
			
			check(g_messageSentCount >= 1) << "Messages sent";
			check(g_messageSentCount == g_message_receivedCount) << "Messages sent and received successfully";
		}
		
#endif
		
	}
}
