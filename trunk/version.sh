#!/bin/sh

#
# version.sh
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
# Determines the current version information of the source files being used.
# 
# The script checks for various SVN infos to determine if the current version is
# a tagged release, development branch or a any trunk revision.
# 
# It outputs the following fields:
# 
# 	REVISION\tNAME\tDATE
# 
# REVISION is the plain SVN revision number. NAME is the unique identifier of
# the current version and might be something like "r12345", "1.2.3" or "r12345
# [some-branch]". DATE is the last-modified date of the current version. If no
# revision information can be determined (e.g. if not invoked on an SVN working
# copy), it outputs an empty revision number, "Unknown Version" as the name and
# the current date.
#

# Tools used in this script
AWK="awk"
SVNVERSION="svnversion"
DATE="date"
SVNINFO="svn info"

# Determining root directory
ROOT_DIR=$1
if [ -z "$ROOT_DIR" -o ! -d "$ROOT_DIR" ]; then
	cd `dirname "$0"`
	ROOT_DIR=`pwd`
fi

# Output fields
REVISION=""
NAME=""
DATE=""

SVNVERSION_OUTPUT=`LC_ALL=C $SVNVERSION "$ROOT_DIR"`
SVNINFO_OUTPUT=`LC_ALL=C $SVNINFO "$ROOT_DIR"`

if [ "$SVNVERSION_OUTPUT" != "exported" ]; then
	BRANCH_NAME=`echo "$SVNINFO_OUTPUT" | $AWK '/^URL:.*branches/ { split($2, a, "/"); for(i in a) if (a[i]=="branches") { print a[i+1]; break } }'`
	TAG_NAME=`echo "$SVNINFO_OUTPUT" | $AWK '/^URL:.*tags/ { split($2, a, "/"); for(i in a) if (a[i]=="tags") { print a[i+1]; break } }'`
	REVISION_NUMBER=`echo "$SVNINFO_OUTPUT" | $AWK '/^Last Changed Rev:/ { print $4 }'`
	REVISION_DATE=`echo "$SVNINFO_OUTPUT" | $AWK '/^Last Changed Date:/ { print $4 }'`
	
	REVISION="$REVISION_NUMBER"
	
	if [ -n "$TAG_NAME" ]; then
		NAME="$TAG_NAME"
	else
		NAME="r$REVISION_NUMBER"
		
		if [ -n "$BRANCH_NAME" ]; then
			NAME="$NAME [$BRANCH_NAME]"
		fi
	fi
	
	if [ -n "`echo \"$SVNVERSION_OUTPUT\" | grep 'M'`" ]; then
		NAME="$NAME (MODIFIED)"
	fi
	
	DATE="$REVISION_DATE"
else
	# Impossible to determine version info from exported sources...
	NAME="Unknown Version"
	DATE=`date -u "+%Y-%m-%d"`
fi

echo "$REVISION\t$NAME\t$DATE"
