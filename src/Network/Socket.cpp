/*
 *  Network/Socket.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 26/10/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Socket.h"

#include "../Events/Loop.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <errno.h>
//#include <execinfo.h>
#include <stdio.h>

namespace Dream {
	namespace Network {
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
		
#pragma mark -
#pragma mark class Socket
		
		IMPLEMENT_CLASS(Socket)
		
		Socket::Socket (int s) : m_socket(s) {
		}
		
		Socket::Socket () : m_socket(-1) {
		}
		
		Socket::~Socket () {
			if (isValid()) {
				close();
			}
		}
		
		void Socket::openSocket (AddressFamily af, SocketType st, SocketProtocol sp) {
			ensure(m_socket == -1);
			
			m_socket = ::socket(af, st, sp);
			// std::cerr << "=====> Opening socket " << m_socket << std::endl;
			
			if (m_socket == -1) {
				perror("Socket::openSocket");
			}
		}
		
		void Socket::openSocket (const Address & a) {
			openSocket(a.addressFamily(), a.socketType(), a.socketProtocol());
		}
		
		Socket::Socket (AddressFamily af, SocketType st, SocketProtocol sp) : m_socket(-1) {
			openSocket(af, st, sp);
		}
		
		void Socket::shutdown (int mode) {
			ensure(isValid());
			
			if (::shutdown(m_socket, mode) == -1) {
				perror("Socket::shutdown");
			}
		}
		
		void Socket::close () {
			ensure(isValid());
			
			// std::cerr << "=====< Closing socket " << m_socket << std::endl;
			
			if (::close(m_socket) == -1) {
				perror("Socket::close");
			}
			
			m_socket = -1;
		}
		
		FileDescriptorT Socket::fileDescriptor () const
		{
			return m_socket; 
		}
		
		void Socket::setNoDelayMode (bool mode) {
			int flag = mode ? 1 : 0;
			
			// IPPROTO_TCP / SOL_SOCKET
			int result = setsockopt(m_socket, SOL_SOCKET, TCP_NODELAY, (char*)&flag, sizeof(int));
			
			if (result < 0) {
				perror("Socket::setNoDelayMode");
			}
		}
		
		int Socket::socketSpecificError () const {
			ensure(isValid());
			
			int error = 0;
			socklen_t len = sizeof(error);
			
			//Get error code specific to this socket
			getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &error, &len);
			
			return error;
		}
		
		bool Socket::isValid () const {
			return m_socket != -1;
		}
		
		bool Socket::isConnected () const {
			sockaddr sa;
			socklen_t len = sizeof(sockaddr);
			
			int err = getpeername(m_socket, &sa, &len);
			
			if (errno == ENOTCONN || err == -1) {
				return false;
			}
			
			return true;
		}
		
		void Socket::shutdownRead()
		{
			shutdown(SHUT_RD); 
		}

		void Socket::shutdownWrite()
		{
			shutdown(SHUT_WR);
		}
		
		IndexT Socket::send (const Core::Buffer & buf, IndexT offset, int flags) {
			ensure(buf.size() > 0); 
			ensure(offset < buf.size());
			
			//std::cout << "Sending " << buf.size() << " bytes..." << std::endl;
			
			IndexT sz = buf.size() - offset;
			
			const ByteT * data = buf.begin() + offset;
			
			sz = ::send(m_socket, data, sz, flags);
			
			if (sz == 0)
				throw ConnectionShutdown("write shutdown");
			
			if (sz == -1) {
				if (errno == ECONNRESET)
					throw ConnectionResetByPeer("write error");
				
				perror("Socket::send");
				sz = 0;
			}
			
			return sz;
		}
		
		IndexT Socket::recv (Core::ResizableBuffer & buf, int flags) {
			ensure(buf.size() < buf.capacity() && "Please make sure you have reserved space for incoming data");
			
			//std::cout << "Receiving " << (buf.capacity() - buf.size()) << " bytes..." << std::endl;
			
			// Find out where we are writing data
			IndexT offset = buf.size();
			
			// Firstly, we maximize size in one go
			buf.resize(buf.capacity());
			
			// We read the size in the buffer
			IndexT sz = ::recv(m_socket, (void*)&buf[offset], buf.size() - offset, flags);
			
			if (sz == 0)
				throw ConnectionShutdown("read shutdown");
			
			if (sz == -1) {
				if (errno == ECONNRESET)
					throw ConnectionResetByPeer("read error");
				
				perror("Socket::recv");
				sz = 0;
			}
			
			// We resize to 
			buf.resize(offset + sz);
			
			return sz;
		}
		
#pragma mark -
#pragma mark class ServerSocket
		
		IMPLEMENT_CLASS(ServerSocket)
		
		ServerSocket::ServerSocket (const Address &serverAddress, unsigned listenCount) {
			bind(serverAddress);
			listen(listenCount);
			
			setWillBlock(false);
						
			std::cerr << "Server " << this << " starting on address: " << serverAddress.description() << " fd: " << fileDescriptor() << std::endl;
		}
		
		ServerSocket::~ServerSocket () {
			
		}
		
		void ServerSocket::processEvents (Events::Loop * eventLoop, Events::Event events)
		{
			if (events & Events::READ_READY)
			{
				ensure(connectionCallback);
				
				SocketHandleT socketHandle;
				Address address;
				
				while(accept(socketHandle, address))
					connectionCallback(eventLoop, this, socketHandle, address);
			}
		}
		
		bool ServerSocket::bind (const Address & na, bool reuseAddr) {
			openSocket(na);
			
			ensure(isValid() && na.isValid());
			
			if (reuseAddr) {
				setReuseAddress(true);
			}
			
			if (::bind(m_socket, na.addressData(), na.addressDataSize()) == -1) {
				perror("Socket::bind");
				return false;
			}
			
			m_boundAddress = na;
			
			return true;
		}
		
		void ServerSocket::listen (unsigned n) {
			if (::listen(m_socket, n) == -1) {
				perror("Socket::listen");
			}
		}
		
		const Address & ServerSocket::boundAddress () const
		{
			return m_boundAddress;
		}
		
		bool ServerSocket::accept (SocketHandleT & h, Address & na) {
			ensure(isValid());
			
			socklen_t len = sizeof(sockaddr_storage);
			sockaddr_storage ss;
			h = ::accept(m_socket, (sockaddr*)&ss, &len);
			
			if(h == -1)
			{
				if (errno != EWOULDBLOCK)
					perror("Socket::accept");
				
				return false;
			}
			
			// Copy address
			na = Address(m_boundAddress, (sockaddr*)&ss, len);
			
			return true;
		}
		
		void ServerSocket::setReuseAddress (bool enabled) {
			ensure(isValid());
			
			int val = (int)enabled;
			int r = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int));
			
			if (r == -1) {
				perror("Socket::setReuseAddress");
			}
		}
		
#pragma mark -
#pragma mark class ClientSocket
		
		IMPLEMENT_CLASS(ClientSocket)
		
		ClientSocket::ClientSocket(const SocketHandleT & h, const Address & address) {
			m_socket = h;
			m_remoteAddress = address;
		}
		
		ClientSocket::ClientSocket() {
			
		}
		
		ClientSocket::~ClientSocket () {
			
		}
		
		const Address & ClientSocket::remoteAddress () const
		{
			return m_remoteAddress;
		}
		
		bool ClientSocket::connect (const Address & na) {
			ensure(!isValid());
			
			openSocket(na);
			
			ensure(isValid() && na.isValid());
			
			if (::connect(m_socket, na.addressData(), na.addressDataSize()) == -1)
			{
				if (errno == ECONNRESET)
					throw ConnectionResetByPeer("connect error");
				else
					perror(__PRETTY_FUNCTION__);
				
				return false;
			}
			
			m_remoteAddress = na;
			
			return true;
		}
		
		bool ClientSocket::connect (const AddressesT & addresses)
		{
			foreach (const Address & address, addresses)
			{
				if (connect(address))
					return true;
			}
					
			return false;
		}
		
		void ClientSocket::processEvents (Events::Loop * eventLoop, Events::Event events)
		{
			
		}
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		
		bool g_messageSent;
		IndexT g_messageLengthSent, g_messageLengthReceived;
		bool g_clientConnected;
		bool g_messageReceived;
		
		const std::string g_message("Hello World!");
		
		class TestClientSocket : public ClientSocket
		{
			EXPOSE_CLASS(TestClientSocket)
			
			class Class : public ClientSocket::Class
			{
				EXPOSE_CLASSTYPE
			};
			
			TestClientSocket (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
			{				
				Core::StaticBuffer buf = Core::StaticBuffer::forCString(g_message.c_str(), false);
				
				std::cerr << "Sending message from " << this << "..." << std::endl;
				
				g_messageLengthSent = send(buf);
				
				std::cerr << g_messageLengthSent << " bytes sent" << std::endl;
				g_messageSent = true;
			}
			
			TestClientSocket ()
			{	
			}
			
			virtual void processEvents(Events::Loop * eventLoop, Events::Event events)
			{
				if (events & Events::READ_READY) {
					Core::DynamicBuffer buf(1024, true);
					
					recv(buf);
					
					g_messageReceived = true;
					g_messageLengthReceived = buf.size();
					
					std::string incomingMessage(buf.begin(), buf.end());
					
					std::cerr << "Message received by " << this << " fd " << this->fileDescriptor() << " : " << incomingMessage << std::endl;
					
					g_messageReceived = (g_message == incomingMessage);
					
					eventLoop->stopMonitoringFileDescriptor(this);
				}
			}
		};
		
		IMPLEMENT_CLASS(TestClientSocket)
		
		class TestServerSocket : public ServerSocket
		{
			EXPOSE_CLASS(TestServerSocket)
			
			class Class : public ServerSocket::Class
			{
				EXPOSE_CLASSTYPE
			};
			
			REF(TestClientSocket) m_testSocket;
			
			virtual void processEvents(Events::Loop * eventLoop, Events::Event events)
			{
				if (events & Events::READ_READY & !m_testSocket) {
					std::cerr << "Test server has received connection..." << std::endl;
					g_clientConnected = true;
					
					SocketHandleT h;
					Address a;
					if (accept(h, a))
						m_testSocket = new TestClientSocket(h, a);
					eventLoop->monitorFileDescriptor(m_testSocket);
					
					eventLoop->stopMonitoringFileDescriptor(this);
				} else {
					std::cerr << "More than one connection received!" << std::endl;
				}
			}
			
			TestServerSocket (const Address &serverAddress, unsigned listenCount = 100) : ServerSocket(serverAddress, listenCount)
			{
				
			}
			
			virtual ~TestServerSocket ()
			{
				std::cerr << "Server shutting down..." << std::endl;
			}
		};
		
		IMPLEMENT_CLASS(TestServerSocket)
		
		static void stopCallback (Events::Loop * eventLoop, Events::TimerSource *, Events::Event event)
		{
			std::cerr << "Stopping test" << std::endl;
			eventLoop->stop();
		}
		
		UNIT_TEST(Socket) {
			testing("Network Communication");
			g_clientConnected = g_messageSent = g_messageReceived = false;
			g_messageLengthSent = g_messageLengthReceived = 0;
			
			REF(Events::Loop) eventLoop = Events::Loop::klass.init();
			
			// This is a very simple example of a network server listening on a single port.
			// This server can only accept one connection 
			
			{
				Address localhost = Address::interfaceAddressesForPort(2000, SOCK_STREAM)[0];
				std::cerr << "Initializing server..." << std::endl;
				REF(TestServerSocket) serverSocket = new TestServerSocket(localhost);
				std::cerr << "Initializing client..." << std::endl;
				REF(TestClientSocket) clientSocket = new TestClientSocket;
				
				std::cerr << "Connecting to server..." << std::endl;
				clientSocket->connect(localhost);
			
				eventLoop->monitorFileDescriptor(serverSocket);
				eventLoop->monitorFileDescriptor(clientSocket);
				eventLoop->scheduleTimer(Events::TimerSource::klass.init(stopCallback, 1));
			}
			
			eventLoop->runForever ();
			eventLoop = NULL;
			
			check(g_clientConnected) << "Client connected";
			check(g_messageSent) << "Message sent";
			check(g_messageLengthSent == g_messageLengthReceived) << "Message length is correct";
			check(g_messageReceived) << "Message received";
		}
#endif
		
	}
}