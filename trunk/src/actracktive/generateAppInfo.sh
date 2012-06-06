#!/bin/sh

#
# generateAppInfo.sh
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

echo "Generating version information for AppInfo class..."
INPUT_FILE=$1
INPUT_FILE_BASE=`dirname "$INPUT_FILE"`/`basename "$INPUT_FILE" ".in"`

VERSION_INFO=`./version.sh ./src`
REVISION=`echo "$VERSION_INFO" | cut -f1`
VERSION=`echo "$VERSION_INFO" | cut -f2`
DATE=`echo "$VERSION_INFO" | cut -f3`
NAME="$PROJECT_NAME"

cat "$INPUT_FILE" | sed "s@!!REVISION!!@${REVISION}@g;s@!!VERSION!!@${VERSION}@g;s@!!DATE!!@${DATE}@g;s@!!NAME!!@${NAME}@g" > "$INPUT_FILE_BASE"
