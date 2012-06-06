/*
 * NetUtil.h
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

#ifndef NETUTIL_H_
#define NETUTIL_H_

#include <string>
#include <stdexcept>

namespace netutil
{

	std::string getHostByName(const std::string& hostname);

	/**
	 * Determines the address of the local network interface which will
	 * be used for communication with the specified host. Note that no
	 * communication will actually take place.
	 *
	 * @param remoteHost IPv4 address of a host which is in the desired network 
	 * @param remotePort (optional) remote port number to use
	 * @returns A human-readable string representation of the local address
	 */
	std::string getLocalAddress(const std::string& remoteHost, unsigned int remotePort = 3333);

}

#endif
