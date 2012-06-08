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

echo "Making distributable package..."
(
	DIST_OUTPUT_PATH=dist

	cd "$PROJECT_ROOT";
	
	VERSION_INFO=`./version.sh ./src`
	REVISION=`echo "$VERSION_INFO" | cut -f1`
	VERSION=`echo "$VERSION_INFO" | cut -f2`
	DATE=`echo "$VERSION_INFO" | cut -f3`
	NAME="$PROJECT_NAME"

	DIST_NAME="$NAME"-"$VERSION"-linux
	
	DIST_DIR="$DIST_OUTPUT_PATH"/"$DIST_NAME"
	
	if [ -e "$DIST_DIR" ]; then
		rm -rf "$DIST_DIR"
	fi
	
	mkdir -p "$DIST_DIR"
	
	cp -f "$BINARY_OUTPUT_PATH"/"$BUILD_ARTIFACT" "$DIST_DIR"
	cp -r -f "$BINARY_OUTPUT_PATH"/resources "$DIST_DIR"
	cp -f "README.txt" "$DIST_DIR"
	cp -f "BUILDING.txt" "$DIST_DIR"
	cp -f "CHANGELOG.txt" "$DIST_DIR"
	cp -f "LICENSE.txt" "$DIST_DIR"
	
	cd "$DIST_OUTPUT_PATH"
	
	tar cvzf "$DIST_NAME".tar.gz "$DIST_NAME"
	
	rm -rf "$DIST_NAME"
)
