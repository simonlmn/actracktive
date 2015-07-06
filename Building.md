# Building #



# Build Requirements #

Actracktive has been built/tested on the following systems:

  * Mac OS X 10.6.8, Apple GCC 4.2.1
  * Ubuntu 12.04, GNU GCC 4.6.1

It depends on several libraries, which are required at build time and usually at runtime as well, if no explicit static linking is done. All build configurations require the following libraries:

  * boost (>= 1.46.0)
  * dc1394 (>= 2.1.3)
  * freetype (>= 2.4.4)
  * log4cplus (>= 1.0.3)
  * opencv (>= 2.3.0)

These libraries are usually not installed by default on any Mac OS X system and partially on most Linux systems. For information on how to get and install the required libraries, see 'Installing Required Libraries' below.

Compiling on Mac OS X additionally requires:

  * CoreFoundation.framework
  * OpenGL.framework
  * GLUT.framework
  * QTKit.framework

Compiling on Linux additionally requires:

  * GL
  * GLU
  * freeglut (>= 2.6.0)

# Installing Required Libraries #

## Mac OS X ##

Install the required libraries using MacPorts:

```
$ sudo port install boost libdc1394 freetype log4cplus opencv
```

## Linux ##

Install the required libraries using a package manager, for example apt on
Ubuntu 12.04:

```
$ apt-get install libboost-dev libdc1394-22-dev libfreetype6-dev \
  liblog4cplus-dev libopencv-dev
```

# Building using Eclipse CDT #

Select the appropriate build configuration, depending on the platform:

  * Linux Debug
  * Linux Release
  * OSX Debug
  * OSX Release

Start building by choosing "Build Project" from the "Project" menu. The runnable binary/application bundle will be placed in the `bin directory.

_Note: On Mac OS X, All dynamically loaded libraries and all their dependencies are automatically repackaged into the application bundle to make it fully self-contained._