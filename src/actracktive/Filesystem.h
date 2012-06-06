/*
 * Filesystem.h
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

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <string>
#include <boost/filesystem.hpp>

namespace filesystem
{

	void initialize();

	boost::filesystem::path getDataDirectory();

	boost::filesystem::path getResourcesDirectory();

	boost::filesystem::path toData(boost::filesystem::path path);

	boost::filesystem::path toResource(boost::filesystem::path path);

	boost::filesystem::path relative(boost::filesystem::path basefile, boost::filesystem::path path);

	boost::filesystem::path coalesceFiles(boost::filesystem::path first, boost::filesystem::path second);

	boost::filesystem::path copyIfNotExisting(boost::filesystem::path from, boost::filesystem::path to);

	void ensureDirectoryExists(boost::filesystem::path directory);

}

#endif
