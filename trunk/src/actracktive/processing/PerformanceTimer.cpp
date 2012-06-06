/*
 * PerformanceTimer.cpp
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

#include "actracktive/processing/PerformanceTimer.h"
#include <cfloat>

const unsigned int PerformanceTimer::EXECUTION_TIMES_WINDOW = 60;

PerformanceTimer::PerformanceTimer()
	: executionTimes(), executionTimestamps(), executionTimeSum(0, 0, 0, 0), executionTime(0, 0, 0, 0), startTime(),
		averageExecutionTime(0), executionsPerSecond(0)
{
	reset();
}

void PerformanceTimer::start()
{
	if (!executionTime.is_not_a_date_time()) {
		return;
	}

	executionTime = boost::posix_time::time_duration(0, 0, 0, 0);
	resume();
}

void PerformanceTimer::pause()
{
	if (startTime.is_not_a_date_time()) {
		return;
	}

	boost::posix_time::ptime pauseTime(boost::posix_time::microsec_clock::local_time());

	executionTime += (pauseTime - startTime);
	startTime = boost::posix_time::ptime();
}

void PerformanceTimer::resume()
{
	if (!startTime.is_not_a_date_time()) {
		return;
	}

	startTime = boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time());
}

void PerformanceTimer::stop()
{
	if (executionTime.is_not_a_date_time()) {
		return;
	}

	pause();

	updateExecutionTimes(executionTime);
	executionTime = boost::posix_time::time_duration(boost::posix_time::not_a_date_time);
}

void PerformanceTimer::reset()
{
	executionTimes.clear();
	executionTimestamps.clear();
	executionTimeSum = boost::posix_time::time_duration(0, 0, 0, 0);
	averageExecutionTime = 0;
	executionsPerSecond = 0;

	onUpdate();
}

double PerformanceTimer::getAverageExecutionTime() const
{
	return averageExecutionTime;
}

double PerformanceTimer::getExecutionsPerSecond() const
{
	return executionsPerSecond;
}

void PerformanceTimer::updateExecutionTimes(boost::posix_time::time_duration executionTime)
{
	if (executionTimes.size() > EXECUTION_TIMES_WINDOW) {
		executionTimeSum -= executionTimes.back();
		executionTimes.pop_back();
		executionTimestamps.pop_back();
	}

	executionTimeSum += executionTime;
	executionTimes.push_front(executionTime);
	executionTimestamps.push_front(boost::posix_time::ptime(boost::posix_time::microsec_clock::local_time()));

	double timeSum = double(executionTimeSum.total_milliseconds());
	double executionsDuration = double((executionTimestamps.front() - executionTimestamps.back()).total_milliseconds());
	unsigned int numberOfTimes = executionTimes.size();

	if (numberOfTimes > 0 && executionsDuration > 0) {
		averageExecutionTime = timeSum / numberOfTimes;
		executionsPerSecond = (numberOfTimes / executionsDuration) * 1000.0;

		onUpdate();
	}
}
