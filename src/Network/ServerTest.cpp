//
//  Network/ServerTest.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 19/12/08.
//  Copyright (c) 2008 Samuel Williams. All rights reserved.
//
//

#include "Network.h"
#include "Message.h"
#include "Server.h"

#include "../Numerics/Average.h"
#include <functional>

#ifdef ENABLE_TESTING

namespace Dream
{
	namespace Network
	{
		using namespace Core;
		using namespace Events;
		
		const unsigned PK_PING = 0xAF;
		
		Numerics::Average<TimeT> g_latency;
		std::mutex g_latencyLock, g_outputLock;
		typedef std::lock_guard<std::mutex> scoped_lock;

		class Pinger : public MessageClientSocket {
		protected:
			int m_ttl;
			Timer m_timer;
			Numerics::Average<TimeT> m_avg;
			bool m_started;
			
		public:
			Pinger (const SocketHandleT & h, const Address & a) : m_ttl(50), MessageClientSocket(h, a), m_started(false) {
				messageReceivedCallback = std::bind(&Pinger::receivedMessage, this);
				sendPing ();
			}
			
			Pinger () : m_ttl(50), m_started(false) {
				messageReceivedCallback = std::bind(&Pinger::receivedMessage, this);
			}
			
			virtual ~Pinger () {
				//{
				//	scoped_lock _s(g_outputLock);
				//	std::cout << "Individual average is: " << (m_avg.average() * 1000.0) << "ms" << std::endl;
				//}
				
				if (m_avg.hasSamples()) {
					scoped_lock lock(g_latencyLock);
					g_latency.addSamples(m_avg);
				}
			}
			
			virtual void processEvents (Loop * runloop, Event events) {
				if (events & Events::WRITE_READY && m_started == false) {
					m_started = true;
					sendPing();
				}
				
				MessageClientSocket::processEvents(runloop, events);
			}
			
			void receivedMessage () {
				TimeT total = m_timer.time();
				
				REF(Message) recvMsg = receivedMessages().front();
				receivedMessages().pop();
								
				if (recvMsg->header()->ptype == PK_PING) {					
					m_avg.addSample(total);
				}
				
				m_ttl -= 1;
				if (m_ttl > 0) {
					sendPing();
				} else {
					shutdown();
				}
			}
			
			void sendPing () {				
				REF(Message) sendMsg (new Message);
				
				sendMsg->resetHeader();
				sendMsg->header()->ptype = PK_PING;
				
				sendMessage(sendMsg);
				
				m_timer.reset();
			}
		};

		

		void runEfficientClientProcess (int k) {
			AddressesT serverAddresses = Address::addressesForName("localhost", "1404", SOCK_STREAM);
			
			{
				REF(Loop) clients = new Loop;
				
				// Connect k times.
				for (unsigned i = 0; i < k; i += 1) {
					REF(Pinger) s (new Pinger);
					s->connect(serverAddresses);
					
					if (s->isConnected()) {
						clients->monitorFileDescriptor(s);
					}
				}
				
				clients->runForever();
			}
		}
				
		class PingPongServer : public Server
		{
		protected:
			void messageReceived (MessageClientSocket * client) {
				while (client->receivedMessages().size()) {
					REF(Message) msg = client->receivedMessages().front();
					client->receivedMessages().pop();
					
					REF(Message) pongMsg = new Message;
					pongMsg->resetHeader();
					pongMsg->header()->ptype = PK_PING;
					
					client->sendMessage(pongMsg);
				}
			}
			
			virtual void connectionCallbackHandler (Loop * eventLoop, ServerSocket * serverSocket, const SocketHandleT & h, const Address & a)
			{
				REF(MessageClientSocket) clientSocket = new MessageClientSocket(h, a);
				
				//std::cerr << "Accepted connection " << clientSocket << " from " << clientSocket->remoteAddress().description();
				//std::cerr << " (" << clientSocket->remoteAddress().addressFamilyName() << ")" << std::endl;
				
				clientSocket->messageReceivedCallback = std::bind(&PingPongServer::messageReceived, this, std::placeholders::_1);
				
				eventLoop->monitorFileDescriptor(clientSocket);
			}
			
		public:
			PingPongServer (REF(Loop) eventLoop, const char * serviceName, SocketType socketType) : Server(eventLoop)
			{
				bindToService(serviceName, socketType);
			}
			
			virtual ~PingPongServer ()
			{
				
			}
		};
	
				
		
		UNIT_TEST(CompleteServer) {
			testing("Server and Clients");
			
			int k = 100;
			
			for (int i = 0; i < 2; i++)
			{	
				std::cerr << "Run " << i << std::endl;
				
				//g_latency = Numerics::Average<TimeT>();
				
				REF(ServerContainer) container(new ServerContainer);
				
				REF(Server) server(new PingPongServer(container->eventLoop(), "1404", SOCK_STREAM));
				container->start(server);
				
				std::vector<std::thread> children;
				
				sleep(1);
				children.push_back(std::thread(runEfficientClientProcess, k));
				children.push_back(std::thread(runEfficientClientProcess, k));
							
				sleep(1);
				children.push_back(std::thread(runEfficientClientProcess, k));
				children.push_back(std::thread(runEfficientClientProcess, k));

				sleep(1);
				children.push_back(std::thread(runEfficientClientProcess, k));
				children.push_back(std::thread(runEfficientClientProcess, k));
				
				foreach(thread, children) {
					thread->join();
				}
				
				container->stop();
				
				{
					scoped_lock lock(g_latencyLock);
					std::cout << "Average latency (whole time): " << g_latency.average() * 1000.0 << "ms" << std::endl;
				}
			}
		}

	}
}

#endif
