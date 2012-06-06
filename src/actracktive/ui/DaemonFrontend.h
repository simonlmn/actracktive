/*
 * DaemonFrontend.h
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

#ifndef DAEMONFRONTEND_H_
#define DAEMONFRONTEND_H_

#include <boost/date_time/posix_time/posix_time.hpp>

class DaemonFrontend
{
public:
	DaemonFrontend();
	virtual ~DaemonFrontend();

	void setTimerOutput(int timerOutput);

private:
	static const int DEFAULT_TIMER_OUTPUT;

	int timerOutput;
	boost::posix_time::ptime lastPerformanceLogTime;

	static void terminate(int signal);
	void logPerformanceData();

};

#endif
