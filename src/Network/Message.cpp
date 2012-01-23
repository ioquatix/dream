//
//  Network/Message.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Message.h"

namespace Dream {
	namespace Network {
		
		using Core::BufferT;
		
#pragma mark -
#pragma mark Message
		
		
		
		const MessageHeader * Message::header () const {
			ensure(headerComplete());
			
			return (const MessageHeader*)m_packet.begin();
		}
		
		MessageHeader * Message::header () {
			ensure(headerComplete());
			
			return (MessageHeader*)&(m_packet[0]);
		}
		
		uint32_t Message::headerLength () const {
			return sizeof(MessageHeader);
		}
		
		uint32_t Message::dataLength () const {
			return m_packet.size() - headerLength();
		}
		
		// These xxxComplete methods are typically used for building messages
		// from incoming data..
		bool Message::headerComplete () const {
			return m_packet.size() >= headerLength();
		}
		
		bool Message::dataComplete () const {
			return headerComplete() && m_packet.size() == (headerLength() + header()->length);
		}
		
		const BufferT & Message::packet () const {
			return m_packet;
		}
		
		BufferT & Message::packet () {
			return m_packet;
		}
		
		void Message::resetHeader () {
			// Allocate space for header...
			if (m_packet.size() < headerLength()) {
				m_packet.resize(headerLength());
			}
			
			MessageHeader * h = header();
			
			h->length = dataLength();
			h->ptype = 0;
		}
		
		void Message::updateSize () {
			ensure(m_packet.size() >= headerLength());
			
			header()->length = dataLength();
		}
		
		bool Message::isValid () const {
			return headerComplete() && dataComplete();
		}
		
#pragma mark -
#pragma mark MessageSender
		
		MessageSender::MessageSender (REF(Message) msg) {
			reset(msg);
		}
		
		MessageSender::MessageSender () {
			reset();
		}
		
		void MessageSender::reset () {
			m_message = NULL;
			m_offset = 0;
		}
		
		void MessageSender::reset (REF(Message) msg) {
			ensure(msg->isValid());
			
			m_message = msg;
			m_offset = 0;
		}
		
		bool MessageSender::hasMessageToSend () const {
			return (bool)m_message;
		}
		
		bool MessageSender::transmissionComplete () const {		
			return m_offset == m_message->packet().size();
		}
		
		bool MessageSender::sendViaSocket(ClientSocket * socket) {
			ensure(hasMessageToSend());
			ensure(socket->isValid());
			
			BufferT &packet = m_message->packet();
			
			m_offset += socket->send(packet, m_offset);
			
			/*
			 if (!transmissionComplete()) {
			 std::cerr << "Warning: Partial transmission of " << m_offset << " bytes out of " << m_message->packet().size() << std::endl;
			 } else {
			 std::cerr << "Complete transmission of " << m_offset << " bytes." << std::endl;
			 }
			 */
			
			return transmissionComplete();
		}
		
#pragma mark -
#pragma mark MessageReceiver
		
		MessageReceiver::MessageReceiver () {
			reset();
		}
		
		void MessageReceiver::reset () {
			m_message = new Message;
		}
		
		REF(Message) MessageReceiver::message () {
			return m_message;
		}
		
		bool MessageReceiver::receiveFromSocket (ClientSocket * socket) {
			IndexT sz = 1;
			
			// Read as much data as possible:
			while (sz > 0 && !m_message->dataComplete()) {
				if (!m_message->headerComplete()) {
					m_message->packet().reserve(m_message->headerLength());
					
					sz = socket->recv(m_message->packet());
				} else if (!m_message->dataComplete()) {
					m_message->packet().reserve(m_message->headerLength() + m_message->header()->length);
					
					sz = socket->recv(m_message->packet());
				}
			}
			
			return m_message->dataComplete();
		}
		
#pragma mark -
#pragma mark class MessageClientSocket
		
		
		
		MessageClientSocket::MessageClientSocket (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
		{
			
		}
		
		MessageClientSocket::MessageClientSocket () {
			
		}
		
		MessageClientSocket::~MessageClientSocket () {
			
		}
		
		void MessageClientSocket::flushSendQueue () {
			m_sendq = QueueT();
		}
		
		void MessageClientSocket::flushReceiveQueue () {
			m_recvq = QueueT();
		}
		
		bool MessageClientSocket::hasMessagesToSend () {
			return m_sender.hasMessageToSend() || m_sendq.size() > 0;
		}
		
		void MessageClientSocket::sendMessage (REF(Message) msg) {
			m_sendq.push(msg);
		}
		
		MessageClientSocket::QueueT & MessageClientSocket::receivedMessages ()
		{
			return m_recvq;
		}
		
		const MessageClientSocket::QueueT & MessageClientSocket::receivedMessages () const
		{
			return m_recvq;
		}
		
		bool MessageClientSocket::updateReceiver () {
			//std::cout << __PRETTY_FUNCTION__ << std::endl;
			// Default state is no message, so we read data
			if (m_receiver.receiveFromSocket(this)) {
				// We have received a complete message, put it on the receive queue.
				m_recvq.push(m_receiver.message());
				
				// Reset the message receiver.
				m_receiver.reset();
				
				if (messageReceivedCallback)
					messageReceivedCallback(this);
				
				return true;
			}
			
			//std::cout << this << " has " << m_receiver.message()->packet().size() << " bytes in buffers" << std::endl;
			
			return false;
		}
		
		void MessageClientSocket::updateSender () {
			// Do we have a message to send?
			if (!m_sender.hasMessageToSend()) {
				// No, no messages currently sending.
				if (m_sendq.size() > 0) {
					//std::cout << __PRETTY_FUNCTION__ << ": Pushing message.." << std::endl;
					// A message is queued to be sent, so lets start sending it.
					m_sender.reset(m_sendq.front());
					m_sendq.pop();
				}
			}  
			
			if (m_sender.hasMessageToSend()) {
				// We are currently sending a message
				if (m_sender.sendViaSocket(this)) {
					//std::cout << __PRETTY_FUNCTION__ << ": Message sent successfully.." << std::endl;
					// Message has beeen sent completely
					m_sender.reset();
				} else {
					//std::cout << __PRETTY_FUNCTION__ << ": Message partially sent.." << std::endl;
					// Some part of message remains...
					// We'll continue to send it next time.
				}
			}
		}
		
		void MessageClientSocket::processEvents(Events::Loop * eventLoop, Events::Event events) {
			if (Events::READ_READY & events)
				updateReceiver();
			
			if (Events::WRITE_READY & events)
				updateSender();
		}
		
#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		
		struct PACKED MsgTest {
			Core::Ordered<uint32_t> a;
			Core::Ordered<uint32_t> b;
			Core::Ordered<uint32_t> c;
		};
		
		UNIT_TEST(Message) {
			testing("Construction");
			
			REF(Message) m1(new Message);
			
			m1->resetHeader();
			m1->header()->ptype = 0xDEAD;
			
			MsgTest body;
			body.a = 5;
			body.b = 10;
			body.c = body.a + body.b;
			
			m1->insert(body);
			m1->updateSize();
			
			check(m1->dataLength() == sizeof(MsgTest)) << "Body is correct size";
			
			check(m1->headerComplete()) << "Header is complete";
			check(m1->dataComplete()) << "Data is complete";
		}
		
#endif
		
	}
}
