/*
 * TUIOSender.h
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

#ifndef TUIOSENDER_H_
#define TUIOSENDER_H_

#include "actracktive/processing/Node.h"
#include "actracktive/processing/nodes/ObjectSource.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>
#include <string>

class TUIOSourceId
{
public:
	TUIOSourceId(const std::string& name = "default");

	void setName(const std::string& name);
	void setVersion(const std::string& version);
	void setAddress(const std::string& address);

	const std::string& get() const;

private:
	std::string name;
	std::string version;
	boost::posix_time::ptime timestamp;
	std::string address;

	std::string sourceId;

	void rebuildSourceId();

};

class UdpSocket;

class TUIOSender: public Node
{
public:
	static const Node::Type& TYPE();
	const Node::Type& getType() const;

	TUIOSender(const std::string& id, const std::string& name = "TUIO Sender");

	virtual void start();
	virtual void step();
	virtual void stop();

private:
	ValueProperty<bool> enabled;
	ValueProperty<std::string> oscAddress;
	ValueProperty<std::string> host;
	ValueProperty<unsigned short> port;
	ValueProperty<unsigned int> idleRate;
	TypedNodeConnection<ObjectSource> source;

	boost::scoped_ptr<UdpSocket> socket;

	TUIOSourceId sourceId;
	unsigned int frameSequenceNumber;
	unsigned int idleCount;

	void setupSocket();
	void send(const Objects& objects);

};

#endif
