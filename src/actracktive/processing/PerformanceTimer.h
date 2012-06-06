/*
 * PerformanceTimer.h
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

#ifndef PERFORMANCETIMER_H_
#define PERFORMANCETIMER_H_

#include <boost/signals2/signal.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <list>

class PerformanceTimer
{
public:
	boost::signals2::signal<void()> onUpdate;

	PerformanceTimer();

	void start();
	void pause();
	void resume();
	void stop();
	void reset();

	double getAverageExecutionTime() const;
	double getExecutionsPerSecond() const;

private:
	static const unsigned int EXECUTION_TIMES_WINDOW;

	std::list<boost::posix_time::time_duration> executionTimes;
	std::list<boost::posix_time::ptime> executionTimestamps;
	boost::posix_time::time_duration executionTimeSum;

	boost::posix_time::time_duration executionTime;
	boost::posix_time::ptime startTime;

	double averageExecutionTime;
	double executionsPerSecond;

	void updateExecutionTimes(boost::posix_time::time_duration executionTime);

};

#endif
