#!/bin/sh

#
# post-build-osx.sh
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
# Post-build script for Mac OSX builds
#

BUNDLE_PATH="$BINARY_OUTPUT_PATH"/"$BUILD_ARTIFACT".app
BUNDLE_BINARY="$BUNDLE_PATH"/Contents/MacOS/"$BUILD_ARTIFACT"
BUNDLE_LIBRARIES_PATH="$BUNDLE_PATH"/Contents/Libraries
BUNDLE_RESOURCES_PATH="$BUNDLE_PATH"/Contents/Resources

repackageDylibs() {
	echo "repackageDylibs for $1"
	
	local LIBRARY_INSTALL_NAME=
	for LIBRARY_INSTALL_NAME in `otool -L "$1" | tail +2 | grep -v -E -e '(/usr)|(/System)|(@rpath)' | cut -d '(' -f 1 | sed 's/^[[:space:]]*\(.*\)[[:space:]]*$/\1/'`; do
		local LIBRARY_NAME=`basename "$LIBRARY_INSTALL_NAME"`
		
		if [ ! -e "$BUNDLE_LIBRARIES_PATH"/"$LIBRARY_NAME" ]; then
			cp -f "$MACPORTS_LIB_PATH"/"$LIBRARY_NAME" "$BUNDLE_LIBRARIES_PATH"/"$LIBRARY_NAME"
			install_name_tool -id "@rpath/$LIBRARY_NAME" "$BUNDLE_LIBRARIES_PATH"/"$LIBRARY_NAME"
			
			repackageDylibs "$BUNDLE_LIBRARIES_PATH"/"$LIBRARY_NAME"
		fi
		
		install_name_tool -change "$LIBRARY_INSTALL_NAME" "@rpath/$LIBRARY_NAME" "$1"
	done
}

echo "Copying built artifact to binary output path..."
(
	if [ -e "$BUNDLE_PATH" ]; then
		rm -rf "$BUNDLE_PATH"
	fi
	
	mkdir -p "$BUNDLE_PATH"
	mkdir -p "$BUNDLE_PATH"/Contents
	mkdir -p "$BUNDLE_PATH"/Contents/MacOS
	mkdir -p "$BUNDLE_LIBRARIES_PATH"
	mkdir -p "$BUNDLE_RESOURCES_PATH"
	
	cp -f "$CWD"/"$BUILD_ARTIFACT" "$BUNDLE_BINARY"
	cp -f "$PROJECT_ROOT"/aux/Info.plist "$BUNDLE_PATH"/Contents/Info.plist
	rsync -r --exclude=.svn "$PROJECT_ROOT"/aux/resources/ "$BUNDLE_RESOURCES_PATH"
	echo -n "APPL????" > "$BUNDLE_PATH"/Contents/PkgInfo
	
	repackageDylibs "$BUNDLE_BINARY"
	
	chmod -R og-w "$BUNDLE_PATH"
	
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

	DIST_NAME="$NAME"-"$VERSION"-osx
	
	DIST_DIR="$DIST_OUTPUT_PATH"/"$DIST_NAME"
	
	if [ -e "$DIST_DIR" ]; then
		rm -rf "$DIST_DIR"
	fi
	
	mkdir -p "$DIST_DIR"
	
	cp -r -f "$BUNDLE_PATH" "$DIST_DIR"
	cp -f "README.txt" "$DIST_DIR"
	cp -f "BUILDING.txt" "$DIST_DIR"
	cp -f "CHANGELOG.txt" "$DIST_DIR"
	cp -f "LICENSE.txt" "$DIST_DIR"
	
	cd "$DIST_OUTPUT_PATH"
	
	zip -r "$DIST_NAME".zip "$DIST_NAME"
	
	rm -rf "$DIST_NAME"
)
