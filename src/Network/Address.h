//
//  Network/Address.h
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 21/10/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#ifndef _DREAM_NETWORK_ADDRESS_H
#define _DREAM_NETWORK_ADDRESS_H

#include "Network.h"
#include "../Core/Strings.h"
#include "../Core/URI.h"

#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
//#include <netinet6/in6.h>
#include <netdb.h>

namespace Dream {
	namespace Network {
		
		using Dream::Core::URI;
		
		/** Indicates a failure to resolve an address.
		
		*/
		class AddressResolutionError : public std::runtime_error
		{
		protected:
			int _error_code;
			
		public:
			AddressResolutionError (const std::string & what, int error_code);
			
			int error_code ();
		};
		
		/// PF_* eg PF_INET
		typedef int ProtocolFamily;
		/// AF_INET, AF_INET6
		typedef int AddressFamily;
		/// SOCK_STREAM, SOCK_DGRAM, SOCK_RAW
		typedef int SocketType;
		/// IPPROTO_TCP, IPPROTO_UDP, IPPROTO_IPX, etc
		typedef int SocketProtocol;
		/// A TCP/UDP port number
		typedef int PortNumber;
	
		class Address;
		/// An array of addresses
		typedef std::vector<Address> AddressesT;
		
		/** An address for a local or remote service.
		 
		 An address contains all the information required to connect or provide a service to other networked software. Typically, an address is used to bind to
		 the local interfaces and connect to remote interfaces.
		 */
		class Address {
		private:
			void copy_from_address (const Address &);
			void set_address_data (const sockaddr *, IndexT size);
			void copy_from_addressInfo (const addrinfo *);
			int name_infoForAddress(std::string *, std::string *, int) const;
			
			static SocketType socket_typeForString(const std::string &);
		
		protected:
			/// PF_xxx eg PF_INET
			int _protocol_family;
			/// SOCK_xxx eg SOCK_STREAM
			int _socket_type;
			/// o or IPPROTO_xxx for IPv4/IPv6
			int _protocol;
		
		private:
			/// Address data (sockaddr)
			sockaddr_storage _address_data;
			/// Address data length
			unsigned _address_dataSize;
			
		public:
			/// Blank constructor. is_valid() returns false.
			Address ();
			
			/// Construct from another address and a <tt>sockaddr *</tt>. This is used when receiving a connection, for example, from the bind system call.
			Address (const Address & copy, sockaddr * sa, IndexT size);
			/// Construct from an addrinfo struct. This is used when using the new style APIs such as getaddrinfo.
			Address (const addrinfo *);
			
			/// Copy constructor. Duplicates internal data structures.
			Address (const Address &);
			/// Copy operator. Duplicates internal data structures.
			Address & operator= (const Address &);
			
			/// Returns whether or not the address is valid or not. Even if an address is valid, it is not guaranteed to be successful in other operations.
			bool is_valid () const;
			
			/// The size of the actual address data.
			IndexT address_dataSize () const;
			/// A pointer to the <tt>sockaddr *</tt>
			sockaddr * address_data ();
			/// A const pointer to the <tt>sockaddr *</tt>
			const sockaddr * address_data () const;
			
			/// The address family, such as AF_INET
			AddressFamily address_family () const;
			/// A string representation of the address family, such as "IPv4"
			const char * address_familyName () const;
			/// A string representation of the address family, such as "IPv4". Need to supply the AddressFamily value.
			static const char * address_familyName (AddressFamily);
			
			/// The protocol family, such as PF_INET. Typically equal in value to the address_family().
			ProtocolFamily protocol_family () const;
			
			/// The socket type, such as SOCK_STREAM or SOCK_DGRAM. Represents whether the address represents TCP or UDP connections, for example.
			SocketType socket_type () const;
			/// The string representation of the socket type. Such as "STREAM" or "DGRAM"
			const char * socket_typeName () const;
			/// The string representation of the socket type. Such as "STREAM" or "DGRAM". Need to supply the SocketType value. 
			static const char * socket_typeName (SocketType);	
			
			/// The socket protocol, such as IPPROTO_TCP
			SocketProtocol socket_protocol () const;
			
			/// The name from /etc/protocol for the given SocketProtocol.
			std::string socket_protocol_name () const;
			
			/// The port number if it is applicable.
			PortNumber port_number () const;
			
			/// The service name if it is applicable. Retrieved from /etc/services based on the port number.
			std::string service_name () const;
			
			/// Typically returns the hostname if one is available, otherwise returns the numeric address.
			std::string canonical_name () const;
			
			/// Returns the numeric address.
			std::string canonical_numeric_name () const;
			
			/// A string that represents the address in a lossy human-readable form.
			std::string description () const;
			
			/// Returns addresses for binding a server on the local machine. Supply a port number.
			/// @sa ServerSocket::bind
			static AddressesT interface_addresses_for_port (PortNumber port, SocketType sock_type);
			/// Returns addresses for binding a server on the local machine. Supply a service name.
			/// @sa ServerSocket::bind
			static AddressesT interface_addresses_for_service (const char * service, SocketType sock_type);
			
			/// Returns addresses for connecting to a remote service.
			/// @sa ClientSocket::connect
			static AddressesT addresses_for_name (const char * host, const char * service, SocketType sock_type);
			/// Returns addresses for connecting to a remote service.
			/// @sa ClientSocket::connect			
			static AddressesT addresses_for_name (const char * host, const char * service, addrinfo * hints);
			
			/// Returns addresses for a given URI.
			/// Format of the URI is service://hostname/
			/// e.g. http://www.google.com or www.google.com:80
			static AddressesT addresses_for_uri (const URI &, SocketType socket_type = SOCK_STREAM);
		};
	}
}

#endif
