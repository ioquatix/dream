//
//  Network/Address.cpp
//  This file is part of the "Dream" project, and is released under the MIT license.
//
//  Created by Samuel Williams on 21/10/07.
//  Copyright (c) 2007 Samuel Williams. All rights reserved.
//
//

#include "Address.h"

#include <sstream>

namespace Dream {
	namespace Network {
		AddressResolutionError::AddressResolutionError (const std::string & what, int error_code) : std::runtime_error(what), _error_code(error_code)
		{
		}
		
		int AddressResolutionError::error_code ()
		{
			return _error_code;
		}
		
		// Currently unused and untested
		Address address_for_socket (int s, bool remote)
		{
			addrinfo ai;
			sockaddr_storage ss;
			socklen_t len;
			int result;
			SocketType socket_type;
			
			len = sizeof(socket_type);
			result = getsockopt(s, SOL_SOCKET, SO_TYPE, &socket_type, &len);
			if (result == -1) perror(__PRETTY_FUNCTION__);
			
			len = sizeof(ss);
			if (!remote)
				result = getsockname(s, (sockaddr*)&ss, &len);
			else
				result = getpeername(s, (sockaddr*)&ss, &len);
			
			if (result == -1) perror(__PRETTY_FUNCTION__);
			
			ai.ai_socktype = socket_type;
			ai.ai_addr = (sockaddr*)&ss;
			ai.ai_addrlen = len;
			ai.ai_family = ss.ss_family;
			ai.ai_protocol = 0;
			ai.ai_next = 0;
			ai.ai_canonname = 0;
			
			return Address(&ai);
			
		}
				
		Address::Address() {
			_address_data.ss_family = 0;
			
			/*zero the address info */
			_protocol = 0;
			_protocol_family = 0;
			_socket_type = 0;
			
			_address_data_size = 0;
		}
		
		Address::Address (const Address & copy, sockaddr * sa, IndexT size)
		{
			copy_from_address(copy);
			set_address_data(sa, size);
		}
		
		Address::Address (const addrinfo * ai) {
			copy_from_address_info(ai);
		}
		
		void Address::copy_from_address (const Address & na) {
			// Copy local values
			this->_protocol = na._protocol;
			this->_protocol_family = na._protocol_family;
			this->_socket_type = na._socket_type;
		}
		
		Address::Address (const Address & na) {
			copy_from_address(na);
			set_address_data(na.address_data(), na.address_data_size());
		}
		
		Address & Address::operator= (const Address & na) {
			copy_from_address(na);
			set_address_data(na.address_data(), na.address_data_size());
			
			return *this;
		}
		
		IndexT Address::address_data_size () const
		{
			return _address_data_size;
		}
		
		sockaddr * Address::address_data ()
		{
			return (sockaddr*)&_address_data;
		}
		
		const sockaddr * Address::address_data () const 
		{
			return (const sockaddr*)&_address_data;
		}
		
		AddressFamily Address::address_family () const
		{
			return _address_data.ss_family;
		}
		
		ProtocolFamily Address::protocol_family () const
		{
			return _protocol_family;
		}
		
		SocketType Address::socket_type () const 
		{
			return _socket_type;
		}
		
		SocketProtocol Address::socket_protocol () const
		{
			return _protocol;
		}
		
		void Address::set_address_data(const sockaddr * sa, IndexT size) {
			DREAM_ASSERT(sa != NULL); // wtf?
			DREAM_ASSERT(size <= sizeof(_address_data));
			
			memcpy (&_address_data, sa, size);
			_address_data_size = size;
		}
		
		bool Address::is_valid () const
		{
			return _address_data.ss_family != 0;
		}
		
		void Address::copy_from_address_info (const addrinfo * ai) {
			DREAM_ASSERT(ai != NULL);
			
			set_address_data(ai->ai_addr, ai->ai_addrlen);
			
			this->_protocol = ai->ai_protocol;
			this->_protocol_family = ai->ai_family;
			this->_socket_type = ai->ai_socktype;
		}
		
		const char * Address::address_family_name() const {
			return address_family_name(address_family());
		}
		
		const char * Address::socket_type_name() const {
			return socket_type_name(socket_type());
		}
		
		SocketType Address::socket_type_for_string(const std::string & s) {
			if (s == "tcp" || s == "STREAM") {
				return SOCK_STREAM;
			} else if (s == "udp" || s == "DGRAM") {
				return SOCK_DGRAM;
			} else if (s == "raw" || s == "RAW") {
				return SOCK_RAW;
			}
			
			return 0;
		}
		
		AddressesT Address::addresses_for_uri(const URI & uri, SocketType socket_type) {
			return addresses_for_name(uri.hostname().c_str(), uri.service().c_str(), socket_type);
		}
		
		const char * Address::socket_type_name(SocketType st) {
			switch (st) {
				case SOCK_STREAM:
					return "STREAM";
				case SOCK_DGRAM:
					return "DGRAM";
				case SOCK_RDM:
					return "RDM";
				case SOCK_SEQPACKET:
					return "SEQPACKET";
				case SOCK_RAW:
					return "RAW";
				default:
					return "-Unknown-";
			}
		}
		
		const char * Address::address_family_name(AddressFamily af) {
			switch (af) {
#ifdef AF_8022
				case AF_8022:		return "802.2";
#endif
				case AF_APPLETALK:	return "AppleTalk";
//				case AF_CCITT:		return "CCITT";
//				case AF_CHAOS:		return "CHAOS";
//				case AF_CNT:		return "CNT";
//				case AF_COIP:		return "COIP";
//				case AF_DATAKIT:	return "DataKit";
				case AF_DECnet:		return "DECnet";
//				case AF_DLI:		return "DLI";
#ifdef AF_DNS
				case AF_DNS:		return "DNS";
#endif
//				case AF_ECMA:		return "ECMA";
//				case AF_HYLINK:		return "HYLINK";
//				case AF_IMPLINK:	return "IMPLINK";
				case AF_IPX:		return "IPX";
				case AF_INET:		return "IPv4";
				case AF_INET6:		return "IPv6";
//				case AF_ISDN:		return "ISDN"; //AF_E164
//				case AF_LAT:		return "LAT";
//				case AF_LINK:		return "LINK";
				case AF_MAX:		return "MAX";
//				case AF_NATM:		return "NATM";
//				case AF_NDRV:		return "NDRV";
//				case AF_NETBIOS:	return "NETBIOS";
//				case AF_NS:			return "NS";
//				case AF_ISO:		return "ISO"; //AF_OSI
//				case AF_PPP:		return "PPP";
//				case AF_PUP:		return "PUP";
				case AF_ROUTE:		return "ROUTE";
//				case AF_SIP:		return "SIP";
				case AF_SNA:		return "SNA";
//				case AF_SYSTEM:		return "SYSTEM";
				case AF_UNIX:		return "UNIX"; //AF_LOCAL
				case AF_UNSPEC:		return "-Unspecified-";
				default:			return "-Unknown-";
			}
		}
		
		AddressesT Address::addresses_for_name(const char * host, const char * service, addrinfo * hints) {
			struct addrinfo *res, *res0;
			int error;
			AddressesT addrs;
			
			if (!host && !service) service = "0";
			
			error = getaddrinfo(host, service, hints, &res);
			
			res0 = res;
			
			if (error) {
				perror(gai_strerror(error));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, error);
			} else {
				while (res) {
					addrs.push_back(Address(res));
					
					res = res->ai_next;
				}
				
				freeaddrinfo(res0);
			}
			
			return addrs;
		}
		
		AddressesT Address::addresses_for_name(const char * host, const char * service, SocketType sock_type) {
			struct addrinfo hints;
			
			memset (&hints, 0, sizeof(hints));
			
			hints.ai_socktype = sock_type;
			hints.ai_family = AF_UNSPEC;
			
			AddressesT addrs = addresses_for_name(host, service, &hints);
			
			return addrs;
		}
		
		int Address::name_info_for_address(std::string * name, std::string * service, int flags) const {
			int error;
			
			char * name_buffer = nullptr, * service_buffer = nullptr;
			int name_buffer_size = 0, service_buffer_size = 0;
			
			if (name != NULL) {
				name_buffer = (char *)alloca(NI_MAXHOST);
				name_buffer_size = NI_MAXHOST;
			}
			
			if (service != NULL) {
				service_buffer = (char *)alloca(NI_MAXSERV);
				service_buffer_size = NI_MAXSERV;
			}
			
			/* getnameinfo() case. NI_NUMERICHOST avoids DNS lookup. */
			error = getnameinfo(address_data(), address_data_size(), name_buffer, name_buffer_size, service_buffer, service_buffer_size, flags);
			
			if (error != 0) return error;
			
			if (name_buffer)
				*name = std::string(name_buffer);
			
			if (service_buffer)
				*service = std::string(service_buffer);
			
			/* no error */
			return 0;
		}
		
		std::string Address::socket_protocol_name () const {
			protoent *ent;
			ent = getprotobynumber(_protocol);
			
			return ent->p_name;
		};
		
		PortNumber Address::port_number () const {
			std::string port_string;
			PortNumber port = 0;
			
			int err = name_info_for_address(NULL, &port_string, NI_NUMERICSERV);
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			std::stringstream str(port_string);
			str >> port;
			
			return port;
		}
		
		std::string Address::service_name () const {
			std::string port_string;
			
			int err = name_info_for_address(NULL, &port_string, NI_NAMEREQD);
			
			if (err == EAI_NONAME) {
				err = name_info_for_address(NULL, &port_string, NI_NUMERICSERV);
			}
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			return port_string;
		}
		
		std::string Address::canonical_name () const {
			std::string host_string;
			
			int err = name_info_for_address(&host_string, NULL, NI_NAMEREQD);
			
			if (err == EAI_NONAME) {
				err = name_info_for_address(&host_string, NULL, NI_NUMERICHOST);
			}
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			return host_string;
		}
		
		std::string Address::canonical_numeric_name () const {
			std::string host_string;
			
			int err = name_info_for_address(&host_string, NULL, NI_NUMERICHOST);
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			return host_string;
		}
		
		AddressesT Address::interface_addresses_for_service(const char * service, SocketType sock_type) {
			struct addrinfo hints;
			
			memset(&hints, 0, sizeof(hints));
			
			// set-up hints structure
			hints.ai_family = AF_UNSPEC;
			hints.ai_flags = AI_PASSIVE; /* listening address */
			hints.ai_socktype = sock_type;
			
			return addresses_for_name(NULL, service, &hints);
		}
		
		AddressesT Address::interface_addresses_for_port(PortNumber port, SocketType sock_type) {
			std::stringstream s;
			
			s << port;
			
			return interface_addresses_for_service(s.str().c_str(), sock_type);
		}
		
		std::string Address::description () const {
			std::stringstream s;
			
			if (address_family() == AF_INET6) {
				s << "[" << canonical_name() << "]:";
			} else {
				s << canonical_name() << ":";
			}
			
			s << port_number();
			
			return s.str();
		}

// MARK: -
// MARK: Unit Tests
		
#ifdef ENABLE_TESTING
		
		void debug_addresses (const char * desc, const AddressesT & addresses)
		{
			using namespace std;
			
			cout << desc << endl;
			
			foreach(a, addresses)
			{
				cout << a->description() << endl;
			}
		}
		
		UNIT_TEST(Address) {
			testing("Construction");
			
			AddressesT addrs1 = Address::interface_addresses_for_port(1024, SOCK_STREAM);
			check(addrs1.size() > 0) << "Interface addresses available";
			debug_addresses("interface_addresses_for_port(1024, SOCK_STREAM)", addrs1);
			
			bool found_ipv4AddressFamily;
			foreach(a, addrs1)
			{
				if (a->address_family() == AF_INET)
					found_ipv4AddressFamily = true;
			}
			
			check(found_ipv4AddressFamily) << "IPv4 address was present";
			
			bool exception_thrown = false;
			try
			{
				Address::addresses_for_name("localhost", "ThisServiceDoesNotExist", SOCK_STREAM);
			}
			catch (AddressResolutionError & ex)
			{
				exception_thrown = true;
			}
			
			check(exception_thrown) << "Address resolution failed";
			
			AddressesT addrs2 = Address::addresses_for_name("localhost", "http", SOCK_STREAM);
			check(addrs2.size() > 0) << "Host addresses available";
			debug_addresses("addresses_for_name(localhost, IMAP, SOCK_STREAM)", addrs2);
			
			AddressesT addrs3 = Address::addresses_for_uri(Core::URI("http://localhost"));
			check(addrs3.size() > 0) << "Host addresses available";
			debug_addresses("addresses_for_uri(http://localhost)", addrs3);
		}
		
#endif
		
	}	
}
