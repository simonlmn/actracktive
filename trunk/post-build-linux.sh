#!/bin/sh

#
# post-build-linux.sh
#
# Copyright (C) 2012 Simon Lehmann
#
# This file is part of Actracktive.
#
# Actracktive is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Actracktive is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
#
#
# Post-build script for Linux builds
#

echo "Copying built artifact to binary output path..."
(
	if [ -e "$BINARY_OUTPUT_PATH"/"$BUILD_ARTIFACT" ]; then
		rm -rf "$BINARY_OUTPUT_PATH"/"$BUILD_ARTIFACT";
	fi
	
	if [ -e "$BINARY_OUTPUT_PATH"/resources ]; then
		rm -rf "$BINARY_OUTPUT_PATH"/resources;
	fi
	
	mkdir -p "$BINARY_OUTPUT_PATH"
	mkdir -p "$BINARY_OUTPUT_PATH"/resources
	
	cp -f "$CWD"/"$BUILD_ARTIFACT" "$BINARY_OUTPUT_PATH"/"$BUILD_ARTIFACT"
	
	rsync -r --exclude=.svn "$PROJECT_ROOT"/aux/resources/ "$BINARY_OUTPUT_PATH"/resources
	
	chmod -R og-w "$BINARY_OUTPUT_PATH"/resources
	
)
