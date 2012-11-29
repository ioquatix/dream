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

// MARK: -
// MARK: Message



		const MessageHeader * Message::header () const {
			DREAM_ASSERT(header_complete());

			return (const MessageHeader*)_packet.begin();
		}

		MessageHeader * Message::header () {
			DREAM_ASSERT(header_complete());

			return (MessageHeader*)&(_packet[0]);
		}

		uint32_t Message::header_length () const {
			return sizeof(MessageHeader);
		}

		uint32_t Message::data_length () const {
			return _packet.size() - header_length();
		}

		// These xxx_complete methods are typically used for building messages
		// from incoming data..
		bool Message::header_complete () const {
			return _packet.size() >= header_length();
		}

		bool Message::data_complete () const {
			return header_complete() && _packet.size() == (header_length() + header()->length);
		}

		const BufferT & Message::packet () const {
			return _packet;
		}

		BufferT & Message::packet () {
			return _packet;
		}

		void Message::reset_header () {
			// Allocate space for header...
			if (_packet.size() < header_length()) {
				_packet.resize(header_length());
			}

			MessageHeader * h = header();

			h->length = data_length();
			h->packet_type = 0;
		}

		void Message::update_size () {
			DREAM_ASSERT(_packet.size() >= header_length());

			header()->length = data_length();
		}

		bool Message::is_valid () const {
			return header_complete() && data_complete();
		}

// MARK: -
// MARK: MessageSender

		MessageSender::MessageSender (Ref<Message> msg) {
			reset(msg);
		}

		MessageSender::MessageSender () {
			reset();
		}

		void MessageSender::reset () {
			_message = NULL;
			_offset = 0;
		}

		void MessageSender::reset (Ref<Message> msg) {
			DREAM_ASSERT(msg->is_valid());

			_message = msg;
			_offset = 0;
		}

		bool MessageSender::has_message_to_send () const {
			return (bool)_message;
		}

		bool MessageSender::transmission_complete () const {
			return _offset == _message->packet().size();
		}

		bool MessageSender::send_via_socket(ClientSocket * socket) {
			DREAM_ASSERT(has_message_to_send());
			DREAM_ASSERT(socket->is_valid());

			BufferT &packet = _message->packet();

			_offset += socket->send(packet, _offset);

			/*
			 if (!transmission_complete()) {
			 std::cerr << "Warning: Partial transmission of " << _offset << " bytes out of " << _message->packet().size() << std::endl;
			 } else {
			 std::cerr << "Complete transmission of " << _offset << " bytes." << std::endl;
			 }
			 */

			return transmission_complete();
		}

// MARK: -
// MARK: MessageReceiver

		MessageReceiver::MessageReceiver () {
			reset();
		}

		void MessageReceiver::reset () {
			_message = new Message;
		}

		Ref<Message> MessageReceiver::message () {
			return _message;
		}

		bool MessageReceiver::receive_from_socket (ClientSocket * socket) {
			std::size_t sz = 1;

			// Read as much data as possible:
			while (sz > 0 && !_message->data_complete()) {
				if (!_message->header_complete()) {
					_message->packet().reserve(_message->header_length());

					sz = socket->recv(_message->packet());
				} else if (!_message->data_complete()) {
					_message->packet().reserve(_message->header_length() + _message->header()->length);

					sz = socket->recv(_message->packet());
				}
			}

			return _message->data_complete();
		}

// MARK: -
// MARK: class MessageClientSocket



		MessageClientSocket::MessageClientSocket (const SocketHandleT & h, const Address & address) : ClientSocket(h, address)
		{
		}

		MessageClientSocket::MessageClientSocket () {
		}

		MessageClientSocket::~MessageClientSocket () {
		}

		void MessageClientSocket::flush_send_queue () {
			_sendq = QueueT();
		}

		void MessageClientSocket::flush_receive_queue () {
			_recvq = QueueT();
		}

		bool MessageClientSocket::has_messages_to_send () {
			return _sender.has_message_to_send() || _sendq.size() > 0;
		}

		void MessageClientSocket::send_message (Ref<Message> msg) {
			_sendq.push(msg);
		}

		MessageClientSocket::QueueT & MessageClientSocket::received_messages ()
		{
			return _recvq;
		}

		const MessageClientSocket::QueueT & MessageClientSocket::received_messages () const
		{
			return _recvq;
		}

		Ref<Message> MessageClientSocket::pop() {
			if (_recvq.size()) {
				Ref<Message> front = _recvq.front();
				_recvq.pop();
				return front;
			} else {
				return NULL;
			}
		}

		bool MessageClientSocket::update_receiver () {
			//std::cout << __PRETTY_FUNCTION__ << std::endl;
			// Default state is no message, so we read data
			if (_receiver.receive_from_socket(this)) {
				// We have received a complete message, put it on the receive queue.
				_recvq.push(_receiver.message());

				// Reset the message receiver.
				_receiver.reset();

				if (message_received_callback)
					message_received_callback(this);

				return true;
			}

			//std::cout << this << " has " << _receiver.message()->packet().size() << " bytes in buffers" << std::endl;

			return false;
		}

		void MessageClientSocket::update_sender () {
			// Do we have a message to send?
			if (!_sender.has_message_to_send()) {
				// No, no messages currently sending.
				if (_sendq.size() > 0) {
					//std::cout << __PRETTY_FUNCTION__ << ": Pushing message.." << std::endl;
					// A message is queued to be sent, so lets start sending it.
					_sender.reset(_sendq.front());
					_sendq.pop();
				}
			}

			if (_sender.has_message_to_send()) {
				// We are currently sending a message
				if (_sender.send_via_socket(this)) {
					//std::cout << __PRETTY_FUNCTION__ << ": Message sent successfully.." << std::endl;
					// Message has beeen sent completely
					_sender.reset();
				} else {
					//std::cout << __PRETTY_FUNCTION__ << ": Message partially sent.." << std::endl;
					// Some part of message remains...
					// We'll continue to send it next time.
				}
			}
		}

		void MessageClientSocket::process_events(Events::Loop * event_loop, Events::Event events) {
			if (Events::READ_READY & events)
				update_receiver();

			if (Events::WRITE_READY & events)
				update_sender();
		}

// MARK: -
// MARK: Unit Tests

#ifdef ENABLE_TESTING

		struct PACKED MsgTest {
			Core::Ordered<uint32_t> a;
			Core::Ordered<uint32_t> b;
			Core::Ordered<uint32_t> c;
		};

		UNIT_TEST(Message) {
			testing("Construction");

			Ref<Message> m1(new Message);

			m1->reset_header();
			m1->header()->packet_type = 0xDEAD;

			MsgTest body;
			body.a = 5;
			body.b = 10;
			body.c = body.a + body.b;

			m1->insert(body);
			m1->update_size();

			check(m1->data_length() == sizeof(MsgTest)) << "Body is correct size";

			check(m1->header_complete()) << "Header is complete";
			check(m1->data_complete()) << "Data is complete";
		}

#endif
	}
}
