//
//  Network/Message.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 1/11/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_NETWORK_MESSAGE_H
#define _DREAM_NETWORK_MESSAGE_H

#include "Socket.h"
#include "../Core/Endian.h"
#include "../Core/Buffer.h"

#include <queue>

namespace Dream {
	namespace Network {
		/// The message header contains the type and length of the message that has been sent or received.
		struct alignas(32) MessageHeader {
			/// The length in bytes.
			Core::Ordered<uint32_t> length;
			/// The packet type.
			Core::Ordered<uint16_t> packet_type;
		};

		/** A message that can be sent across the network.

		 This class aids in the construction and interpretation of structured data sent across the network. It provides a basic header structure and size
		 information so discrete data can be conveniently sent across the network. It ties in with MessageClientSocket which can send and receive messages
		 reliably.

		 */
		class Message : public Object {
		protected:
			Core::BufferT _packet;

		public:
			/// The length of the header segment.
			uint32_t header_length () const;

			/// The length of the data segment.
			uint32_t data_length () const;

			/// Used to indiciate that a complete header is currently available in the message.
			bool header_complete () const;
			/// Used to indicate that the correct amount of data has been received for this message.
			bool data_complete () const;

			/// Returns a pointer to the header structure so that it can be easily interpreted and manipulated.
			const MessageHeader * header () const;
			MessageHeader * header ();

			/// Returns a pointer to the entire message data buffer.
			const Core::BufferT & packet () const;
			Core::BufferT & packet ();

			/// Reset the message to zero-size.
			void reset_header ();
			/// After adding data into the message, you need to update the header before data is sent.
			void update_size ();
			/// Check if the message data is valid.
			bool is_valid () const;

			/// Read structured data out of the message buffer.
			template <typename type_t>
			bool read (type_t & s, std::size_t offset = 0) const {
				offset += header_length();
				std::size_t sz = sizeof(type_t);

				if (offset + sz > _packet.size()) {
					return false;
				}

				memcpy(&s, _packet.begin() + offset, sz);
				return true;
			}

			/// Write structured data into the message buffer.
			template <typename type_t>
			void insert (type_t & s) {
				std::size_t offset = _packet.size();
				std::size_t sz = sizeof(type_t);

				// Make room at the end
				_packet.resize(offset + sz);

				memcpy(&_packet[offset], &s, sz);
				update_size();
			}
		};

		/** Sends a Message via a ClientSocket.

		 This class will send a single message. Once it is done, it can be reset with another message to send.

		 */
		class MessageSender {
		protected:
			Ref<Message> _message;
			unsigned _offset;

		public:
			MessageSender (Ref<Message> msg);
			MessageSender ();

			/// Cancel sending the current message. Note: Resetting the message part way through will corrupt the connection if the remote end expects
			/// a complete message. Therefore, a MessageSender should only be reset once a complete message has been sent.
			void reset ();

			/// Prepare to send another message.
			void reset (Ref<Message> msg);

			/// Returns true if a message is currently waiting to be sent or in progress.
			bool has_message_to_send () const;

			/// Writes data to the socket to send the message to the remote peer.
			bool send_via_socket(ClientSocket * socket);

			/// Returns true once the message has been sent completely.
			bool transmission_complete () const;
		};

		/** Receives a Message via a ClientSocket.

		 This class will receive a single message. Once it is done, it returns true, so that it can be reset.

		 */
		class MessageReceiver {
		protected:
			Ref<Message> _message;

		public:
			MessageReceiver ();

			/// Resets the message.
			/// This should be done when the receive_from_socket() method returns true.
			void reset ();

			/// Retrieve the complete or partial message.
			Ref<Message> message ();

			/// Read data from the socket to add to the incoming message.
			/// @returns true when the message is complete.
			bool receive_from_socket (ClientSocket * socket);
		};

// MARK: -
// MARK: class MessageClientSocket

		/** Provides asynchronous message sending and retrival with good efficiency and reliability.

		 This class contains two queues, a receive queue and send queue. These queues feed directly into an instance of both MessageSender and MessageReceiver.
		 Messages in the queues will be sent and received in the background, and can be pushed and popped as needed.

		 It is expected that this class will provide the basis for any custom network APIs.

		 */
		class MessageClientSocket : public ClientSocket {
		protected:
			MessageSender _sender;
			MessageReceiver _receiver;

			typedef std::queue<Ref<Message>> QueueT;
			QueueT _recvq, _sendq;

			/// Processes any outgoing messages.
			void update_sender ();

			/// @returns true when a complete message was received.
			bool update_receiver ();

		public:
			MessageClientSocket (const SocketHandleT & h, const Address & address);
			MessageClientSocket ();

			virtual ~MessageClientSocket ();

			/// Cancel all messages on the send queue.
			void flush_send_queue ();

			/// Remove any messages in the receive queue.
			void flush_receive_queue ();

			/// @returns true if there are currently messages to be sent or being sent.
			bool has_messages_to_send ();

			/// Queues a message to be sent.
			void send_message (Ref<Message> msg);

			/// Returns the queue containing incoming messages
			QueueT & received_messages ();
			const QueueT & received_messages () const;

			/// Pop the front message off the receive queue and return it, otherwise NULL.
			Ref<Message> pop();

			/// Calls update_sender() and update_receiver() as needed.
			virtual void process_events (Events::Loop *, Events::Event);

			/// Delegate function to handle incoming messages. Called when a message is received.
			std::function<void (MessageClientSocket *)> message_received_callback;
		};
	}
}

#endif
