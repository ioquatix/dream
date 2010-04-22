/*
 *  Network/Address.cpp
 *  This file is part of the "Dream" project, and is licensed under the GNU GPLv3.
 *
 *  Created by Administrator on 21/10/07.
 *  Copyright 2007 Samuel Williams. All rights reserved.
 *
 */

#include "Address.h"

#include <sstream>

namespace Dream {
	namespace Network {
		AddressResolutionError::AddressResolutionError (const std::string & what, int errorCode) : std::runtime_error(what), m_errorCode(errorCode)
		{
		}
		
		int AddressResolutionError::errorCode ()
		{
			return m_errorCode;
		}
		
		// Currently unused and untested
		Address addressForSocket (int s, bool remote)
		{
			addrinfo ai;
			sockaddr_storage ss;
			socklen_t len;
			int result;
			SocketType socketType;
			
			len = sizeof(socketType);
			result = getsockopt(s, SOL_SOCKET, SO_TYPE, &socketType, &len);
			if (result == -1) perror(__PRETTY_FUNCTION__);
			
			len = sizeof(ss);
			if (!remote)
				result = getsockname(s, (sockaddr*)&ss, &len);
			else
				result = getpeername(s, (sockaddr*)&ss, &len);
			
			if (result == -1) perror(__PRETTY_FUNCTION__);
			
			ai.ai_socktype = socketType;
			ai.ai_addr = (sockaddr*)&ss;
			ai.ai_addrlen = len;
			ai.ai_family = ss.ss_family;
			ai.ai_protocol = 0;
			ai.ai_next = 0;
			ai.ai_canonname = 0;
			
			return Address(&ai);
			
		}
				
		Address::Address() {
			m_addressData.ss_family = 0;
			
			/*zero the address info */
			m_protocol = 0;
			m_protocolFamily = 0;
			m_socketType = 0;
			
			m_addressDataSize = 0;
		}
		
		Address::Address (const Address & copy, sockaddr * sa, IndexT size)
		{
			copyFromAddress(copy);
			setAddressData(sa, size);
		}
		
		Address::Address (const addrinfo * ai) {
			copyFromAddressInfo(ai);
		}
		
		void Address::copyFromAddress (const Address & na) {
			// Copy local values
			this->m_protocol = na.m_protocol;
			this->m_protocolFamily = na.m_protocolFamily;
			this->m_socketType = na.m_socketType;
		}
		
		Address::Address (const Address & na) {
			copyFromAddress(na);
			setAddressData(na.addressData(), na.addressDataSize());
		}
		
		Address & Address::operator= (const Address & na) {
			copyFromAddress(na);
			setAddressData(na.addressData(), na.addressDataSize());
			
			return *this;
		}
		
		IndexT Address::addressDataSize () const
		{
			return m_addressDataSize;
		}
		
		sockaddr * Address::addressData ()
		{
			return (sockaddr*)&m_addressData;
		}
		
		const sockaddr * Address::addressData () const 
		{
			return (const sockaddr*)&m_addressData;
		}
		
		AddressFamily Address::addressFamily () const
		{
			return m_addressData.ss_family;
		}
		
		ProtocolFamily Address::protocolFamily () const
		{
			return m_protocolFamily;
		}
		
		SocketType Address::socketType () const 
		{
			return m_socketType;
		}
		
		SocketProtocol Address::socketProtocol () const
		{
			return m_protocol;
		}
		
		void Address::setAddressData(const sockaddr * sa, IndexT size) {
			ensure(sa != NULL); // wtf?
			ensure(size <= sizeof(m_addressData));
			
			memcpy (&m_addressData, sa, size);
			m_addressDataSize = size;
		}
		
		bool Address::isValid () const
		{
			return m_addressData.ss_family != 0;
		}
		
		void Address::copyFromAddressInfo (const addrinfo * ai) {
			ensure(ai != NULL);
			
			setAddressData(ai->ai_addr, ai->ai_addrlen);
			
			this->m_protocol = ai->ai_protocol;
			this->m_protocolFamily = ai->ai_family;
			this->m_socketType = ai->ai_socktype;
		}
		
		const char * Address::addressFamilyName() const {
			return addressFamilyName(addressFamily());
		}
		
		const char * Address::socketTypeName() const {
			return socketTypeName(socketType());
		}
		
		SocketType Address::socketTypeForString(const std::string & s) {
			if (s == "tcp" || s == "STREAM") {
				return SOCK_STREAM;
			} else if (s == "udp" || s == "DGRAM") {
				return SOCK_DGRAM;
			} else if (s == "raw" || s == "RAW") {
				return SOCK_RAW;
			}
			
			return 0;
		}
		
		AddressesT Address::addressesForURI(const URI & uri, SocketType socketType) {
			return addressesForName(uri.hostname().c_str(), uri.service().c_str(), socketType);
		}
		
		const char * Address::socketTypeName(SocketType st) {
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
		
		const char * Address::addressFamilyName(AddressFamily af) {
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
		
		AddressesT Address::addressesForName(const char * host, const char * service, addrinfo * hints) {
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
				
				freeaddrinfo (res0);
			}
			
			return addrs;
		}
		
		AddressesT Address::addressesForName(const char * host, const char * service, SocketType sockType) {
			struct addrinfo hints;
			
			memset (&hints, 0, sizeof(hints));
			
			hints.ai_socktype = sockType;
			hints.ai_family = AF_UNSPEC;
			
			AddressesT addrs = addressesForName(host, service, &hints);
			
			return addrs;
		}
		
		int Address::nameInfoForAddress(std::string * name, std::string * service, int flags) const {
			int err;
			char _nameBuf[NI_MAXHOST];
			char _serviceBuf[NI_MAXSERV];
			
			char *nameBuf = _nameBuf, *serviceBuf = _serviceBuf;
			
			int nameBufSz = NI_MAXHOST, serviceBufSz = NI_MAXSERV;
			
			if (name == NULL) {
				nameBuf = NULL;
				nameBufSz = 0;
			}
			
			if (service == NULL) {
				serviceBuf = NULL;
				serviceBufSz = 0;
			}
			
			/* getnameinfo() case. NI_NUMERICHOST avoids DNS lookup. */
			err = getnameinfo(addressData(), addressDataSize(), nameBuf, nameBufSz, serviceBuf, serviceBufSz, flags);
			
			if (err != 0) return err;
			
			if (nameBuf)
				*name = std::string(nameBuf);
			
			if (serviceBuf)
				*service = std::string(serviceBuf);
			
			/* no error */
			return 0;
		}
		
		std::string Address::socketProtocolName () const {
			protoent *ent;
			ent = getprotobynumber(m_protocol);
			
			return ent->p_name;
		};
		
		PortNumber Address::portNumber () const {
			std::string portString;
			PortNumber port = 0;
			
			int err = nameInfoForAddress(NULL, &portString, NI_NUMERICSERV);
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			std::stringstream str(portString);
			str >> port;
			
			return port;
		}
		
		std::string Address::serviceName () const {
			std::string portString;
			
			int err = nameInfoForAddress(NULL, &portString, NI_NAMEREQD);
			
			if (err == EAI_NONAME) {
				err = nameInfoForAddress(NULL, &portString, NI_NUMERICSERV);
			}
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			return portString;
		}
		
		std::string Address::canonicalName () const {
			std::string hostString;
			
			int err = nameInfoForAddress(&hostString, NULL, NI_NAMEREQD);
			
			if (err == EAI_NONAME) {
				err = nameInfoForAddress(&hostString, NULL, NI_NUMERICHOST);
			}
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			return hostString;
		}
		
		std::string Address::canonicalNumericName () const {
			std::string hostString;
			
			int err = nameInfoForAddress(&hostString, NULL, NI_NUMERICHOST);
			
			if (err) {
				perror(gai_strerror(err));
				
				throw AddressResolutionError(__PRETTY_FUNCTION__, err);
			}
			
			return hostString;
		}
		
		AddressesT Address::interfaceAddressesForService(const char * service, SocketType sockType) {
			struct addrinfo hints;
			
			memset(&hints, 0, sizeof(hints));
			
			// set-up hints structure
			hints.ai_family = AF_UNSPEC;
			hints.ai_flags = AI_PASSIVE; /* listening address */
			hints.ai_socktype = sockType;
			
			return addressesForName(NULL, service, &hints);
		}
		
		AddressesT Address::interfaceAddressesForPort(PortNumber port, SocketType sockType) {
			std::stringstream s;
			
			s << port;
			
			return interfaceAddressesForService(s.str().c_str(), sockType);
		}
		
		std::string Address::description () const {
			std::stringstream s;
			
			if (addressFamily() == AF_INET6) {
				s << "[" << canonicalName() << "]:";
			} else {
				s << canonicalName() << ":";
			}
			
			s << portNumber();
			
			return s.str();
		}

#pragma mark -
#pragma mark Unit Tests
		
#ifdef ENABLE_TESTING
		
		void debugAddresses (const char * desc, const AddressesT & addresses)
		{
			using namespace std;
			
			cout << desc << endl;
			
			foreach(const Address & a, addresses)
			{
				cout << a.description() << endl;
			}
		}
		
		UNIT_TEST(Address) {
			testing("Construction");
			
			AddressesT addrs1 = Address::interfaceAddressesForPort(1024, SOCK_STREAM);
			assertTrue(addrs1.size() > 0, "Interface addresses available");
			debugAddresses("interfaceAddressesForPort(1024, SOCK_STREAM)", addrs1);
			
			bool foundIPv4AddressFamily;
			foreach(const Address & a, addrs1)
			{
				if (a.addressFamily() == AF_INET)
					foundIPv4AddressFamily = true;
			}
			
			assertTrue(foundIPv4AddressFamily, "IPv4 address was present");
			
			bool exceptionThrown = false;
			try
			{
				Address::addressesForName("localhost", "ThisServiceDoesNotExist", SOCK_STREAM);
			}
			catch (AddressResolutionError & ex)
			{
				exceptionThrown = true;
			}
			
			assertTrue(exceptionThrown, "Address resolution failed");
			
			AddressesT addrs2 = Address::addressesForName("localhost", "http", SOCK_STREAM);
			assertTrue(addrs2.size() > 0, "Host addresses available");
			debugAddresses("addressesForName(localhost, IMAP, SOCK_STREAM)", addrs2);
			
			AddressesT addrs3 = Address::addressesForURI(Core::URI("http://localhost"));
			assertTrue(addrs3.size() > 0, "Host addresses available");
			debugAddresses("addressesForURI(http://localhost)", addrs3);
		}
		
#endif
		
	}	
}