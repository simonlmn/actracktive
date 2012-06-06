/*
 * NetUtil.cpp
 *
 * Copyright (C) 2012 Simon Lehmann
 *
 * This file is part of Actracktive.
 *
 * Actracktive is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Actracktive is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "actracktive/util/NetUtil.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <cerrno>

namespace netutil
{

	static const std::string DEFAULT_ADDRESS = "127.0.0.1";

	std::string getHostByName(const std::string& hostname)
	{
		struct hostent* host = gethostbyname(hostname.c_str());
		if (host) {
			struct in_addr address;
			memcpy(&address, host->h_addr_list[0], host->h_length);

			char addressString[INET_ADDRSTRLEN];
			if (inet_ntop(AF_INET, &address, addressString, INET_ADDRSTRLEN) == NULL) {
				return DEFAULT_ADDRESS;
			}
			return std::string(addressString);
		} else {
			return DEFAULT_ADDRESS;
		}
	}

	std::string getLocalAddress(const std::string& remoteHost, unsigned int remotePort)
	{
		int sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock == -1) {
			return DEFAULT_ADDRESS;
		}

		sockaddr_in remote;
		memset(&remote, 0, sizeof(remote));
		remote.sin_family = AF_INET;
		remote.sin_addr.s_addr = inet_addr(remoteHost.c_str());
		remote.sin_port = htons(remotePort);

		int err = connect(sock, (const sockaddr*) &remote, sizeof(remote));
		if (err == -1) {
			close(sock);
			return DEFAULT_ADDRESS;
		}

		sockaddr_in name;
		socklen_t namelen = sizeof(name);
		err = getsockname(sock, (sockaddr*) &name, &namelen);
		if (err == -1) {
			close(sock);
			return DEFAULT_ADDRESS;
		}

		char localAddress[INET_ADDRSTRLEN];
		if (inet_ntop(AF_INET, &name.sin_addr, localAddress, INET_ADDRSTRLEN) == NULL) {
			close(sock);
			return DEFAULT_ADDRESS;
		}

		close(sock);
		return std::string(localAddress);
	}

}
