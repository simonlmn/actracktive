/*
 * TUIOSender.cpp
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

#include "actracktive/processing/nodes/TUIOSender.h"
#include "actracktive/processing/NodeFactory.h"
#include "actracktive/util/NetUtil.h"
#include "actracktive/AppInfo.h"
#include "ip/UdpSocket.h"
#include "osc/OscOutboundPacketStream.h"
#include <boost/format.hpp>
#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("TUIOSender");

static const std::size_t OUTBOUND_PACKET_STREAM_BUFFER_SIZE = 65536;

TUIOSourceId::TUIOSourceId(const std::string& name)
	: name(name), version("0"), timestamp(boost::posix_time::microsec_clock::local_time()), address("127.0.0.1")
{
	rebuildSourceId();
}

void TUIOSourceId::setName(const std::string& name)
{
	this->name = name;
	rebuildSourceId();
}

void TUIOSourceId::setVersion(const std::string& version)
{
	this->version = version;
	rebuildSourceId();
}

void TUIOSourceId::setAddress(const std::string& address)
{
	this->address = address;
	rebuildSourceId();
}

const std::string& TUIOSourceId::get() const
{
	return sourceId;
}

void TUIOSourceId::rebuildSourceId()
{
	std::ostringstream sourceId;
	sourceId << this->name << ":" << this->version << ":" << boost::posix_time::to_iso_string(this->timestamp) << "@" << this->address;

	this->sourceId = sourceId.str();
}

const Node::Type& TUIOSender::TYPE()
{
	static const Node::Type type = Node::Type::of<TUIOSender>("TUIOSender", Node::TYPE());
	return type;
}

const Node::Type& TUIOSender::getType() const
{
	return TYPE();
}

TUIOSender::TUIOSender(const std::string& id, const std::string& name)
	: Node(id, name), enabled("enabled", "Enabled", mutex, true), oscAddress("oscAddress", "OSC Address", mutex, "/tuio"),
		host("host", "Host", mutex, "127.0.0.1"), port("port", "Port", mutex, 3333, Constraint<unsigned short>(0, 65535)),
		idleRate("idleRate", "Idle Rate", mutex, 10, Constraint<unsigned int>(1, 60)), source("source", "Source", mutex), socket(),
		sourceId(), frameSequenceNumber(0), idleCount(0)
{
	settings.add(enabled);
	settings.add(oscAddress);
	settings.add(host);
	settings.add(port);
	settings.add(idleRate);
	connections.add(source);
}

void TUIOSender::start()
{
	Node::start();

	frameSequenceNumber = 0;

	sourceId.setName(AppInfo::NAME);
	sourceId.setVersion(AppInfo::VERSION);

	setupSocket();

	host.onChange.connect(boost::bind(&TUIOSender::setupSocket, this));
	port.onChange.connect(boost::bind(&TUIOSender::setupSocket, this));
}

void TUIOSender::step()
{
	Node::step();

	timer.resume();

	if (enabled && source) {
		Lock lock(source);

		timer.pause();
		const Objects& objects = source->get();
		timer.resume();

		if (!objects.isEmpty() || idleCount == 0) {
			send(objects);
			idleCount = 0;
		}

		if (objects.isEmpty()) {
			idleCount = (idleCount + 1) % idleRate;
		}
	}

	timer.pause();
}

void TUIOSender::stop()
{
	Node::stop();

	host.onChange.disconnect(boost::bind(&TUIOSender::setupSocket, this));
	port.onChange.disconnect(boost::bind(&TUIOSender::setupSocket, this));
}

void TUIOSender::setupSocket()
{
	Mutex::scoped_lock lock(mutex);

	try {
		sourceId.setAddress(netutil::getLocalAddress(netutil::getHostByName(host)));

		socket.reset(new UdpSocket());
		socket->Connect(IpEndpointName(host.getValue().c_str(), port));

		LOG4CPLUS_DEBUG(
			logger,
			"Setup of socket complete (sourceId: " << sourceId.get() << ", destination: " << host.getValue() << ":" << port.getValue() << ")");
	} catch (std::runtime_error& e) {
		socket.reset();
		LOG4CPLUS_ERROR(logger, "Setup of socket failed: " << e.what());
	}
}

void TUIOSender::send(const Objects& objects)
{
	Mutex::scoped_lock lock(mutex);

	if (!socket) {
		return;
	}

	try {
		char buffer[OUTBOUND_PACKET_STREAM_BUFFER_SIZE];
		osc::OutboundPacketStream p(buffer, OUTBOUND_PACKET_STREAM_BUFFER_SIZE);

		p << osc::BeginBundleImmediate;

		p << osc::BeginMessage(oscAddress.getValue().c_str()) << "source" << sourceId.get().c_str() << osc::EndMessage;

		p << osc::BeginMessage(oscAddress.getValue().c_str());
		p << "alive";
		for (Objects::ConstIterator object = objects.begin(); object != objects.end(); ++object) {
			if ((*object)->isAlive()) {
				p << int((*object)->getId());
			}
		}
		p << osc::EndMessage;

		for (Objects::ConstIterator object = objects.begin(); object != objects.end(); ++object) {
			if ((*object)->isAlive()) {
				p << osc::BeginMessage(oscAddress.getValue().c_str()) << "set" << (*object) << osc::EndMessage;
			}
		}

		p << osc::BeginMessage(oscAddress.getValue().c_str()) << "fseq" << int(frameSequenceNumber++) << osc::EndMessage;

		p << osc::EndBundle;

		socket->Send(p.Data(), p.Size());
	} catch (osc::OutOfBufferMemoryException& e) {
		LOG4CPLUS_ERROR(logger, "Sending objects failed, too many objects to send (" << objects.getSize() << ")");
	}
}

static bool __registered = registerNodeType<TUIOSender>();
