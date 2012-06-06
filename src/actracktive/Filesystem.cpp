/*
 * Filesystem.cpp
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

#include "actracktive/Filesystem.h"
#include "actracktive/util/Utils.h"

#ifdef TARGET_OSX
#include "CoreFoundation/CFBundle.h"
#endif

#include <log4cplus/logger.h>

static log4cplus::Logger logger = log4cplus::Logger::getInstance("filesystem");

namespace filesystem
{

	static const std::string DATA_DIRECTORY_ENV_NAME = "ACTRACKTIVE_DATA_DIRECTORY";
	static const std::string DATA_DIRECTORY_DEFAULT_NAME = "Actracktive Data";

	static boost::filesystem::path resources;
	static boost::filesystem::path data;

	static bool valid(const boost::filesystem::path& path)
	{
		try {
			if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path)) {
				return true;
			}
		} catch (const boost::filesystem::filesystem_error& ex) {
		}

		return false;
	}

#ifdef TARGET_OSX
	static boost::filesystem::path convertCFUrlToPath(CFURLRef url)
	{
		CFStringRef str = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
		CFIndex size = CFStringGetMaximumSizeForEncoding(CFStringGetLength(str), kCFStringEncodingASCII) + 1;

		char* cstr = new char[size];
		CFStringGetCString(str, cstr, size, kCFStringEncodingASCII);

		CFRelease(str);
		CFRelease(url);

		std::string pathStr(cstr);
		delete[] cstr;

		return boost::filesystem::path(pathStr);
	}
#endif

	void initialize()
	{
		static bool initialized = false;
		if (initialized) {
			return;
		}

		// Determine path for resources
#ifdef TARGET_OSX
		CFBundleRef mainBundle = CFBundleGetMainBundle();
		if (mainBundle) {
			boost::filesystem::path bundlePath = convertCFUrlToPath(CFBundleCopyBundleURL(mainBundle));
			boost::filesystem::path resourcesPath = convertCFUrlToPath(CFBundleCopyResourcesDirectoryURL(mainBundle));

			resources = bundlePath / resourcesPath;
		}
#elif TARGET_LINUX
		resources = boost::filesystem::read_symlink(boost::filesystem::path("/proc/self/exe")).parent_path() / boost::filesystem::path("resources");
#endif

		if (!valid(resources)) {
			throw std::runtime_error("Unable to determine resource directory!");
		}

		// Determine data path
		char const* dataPathEnv = getenv(DATA_DIRECTORY_ENV_NAME.c_str());
		if (dataPathEnv) {
			data = boost::filesystem::path(dataPathEnv);
		} else {
			boost::filesystem::path userHome;

			char const* home = util::coalesce(getenv("HOME"), getenv("USERPROFILE"));
			if (home) {
				userHome = boost::filesystem::path(home);
			} else {
				char const* homedrive = getenv("HOMEDRIVE");
				char const* homepath = getenv("HOMEPATH");
				if (homedrive && homepath) {
					userHome = boost::filesystem::path(std::string(homedrive) + homepath);
				}
			}

			if (!valid(userHome)) {
				throw std::runtime_error("Unable to determine user home directory!");
			}

			data = userHome / DATA_DIRECTORY_DEFAULT_NAME;

			ensureDirectoryExists(data);
		}

		initialized = true;
	}

	boost::filesystem::path getDataDirectory()
	{
		return data;
	}

	boost::filesystem::path getResourcesDirectory()
	{
		return resources;
	}

	boost::filesystem::path toData(boost::filesystem::path path)
	{
		initialize();

		return boost::filesystem::absolute(path, data);
	}

	boost::filesystem::path toResource(boost::filesystem::path path)
	{
		initialize();

		return boost::filesystem::absolute(path, resources);
	}

	boost::filesystem::path relative(boost::filesystem::path basefile, boost::filesystem::path path)
	{
		if (path.is_absolute()) {
			return path;
		}

		return basefile.parent_path() / path;
	}

	boost::filesystem::path coalesceFiles(boost::filesystem::path first, boost::filesystem::path second)
	{
		if (boost::filesystem::exists(first) && boost::filesystem::is_regular_file(first)) {
			return first;
		} else {
			return second;
		}
	}

	boost::filesystem::path copyIfNotExisting(boost::filesystem::path from, boost::filesystem::path to)
	{
		if (!boost::filesystem::exists(to)) {
			boost::filesystem::create_directories(to.parent_path());
			boost::filesystem::copy_file(from, to);
		}

		return to;
	}

	void ensureDirectoryExists(boost::filesystem::path directory)
	{
		if (!boost::filesystem::exists(directory)) {
			boost::filesystem::create_directories(directory);
		}
	}

}
