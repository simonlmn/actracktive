<p align='center'><b>Read the <a href='UserGuide.md'>User Guide</a> or <a href='Building.md'>Build Instructions</a></b></p>

Actracktive is a highly configurable real-time computer vision/image processing
platform written in C++. It provides a framework and runtime environment for
developing and executing complex processing graphs which can for example capture
images from a camera, pre-process them using various image filters, extract
features, track the detected objects of interest over time and finally send
information about the objects to another application via network.

Actracktive originally started as a fork of the [CCV](http://ccv.nuigroup.com/)
project used for vision-based finger tracking in the multitouch/interactive
tabletop systems. As a more modular and configurable solution was needed, the
idea for Actracktive was born and a new architecture and implementation was
built from scratch. It aims at providing a general and easily extensible
framework for implementing most computer vision tasks (2D and 3D), but also
other kinds of (frame-based) signal processing.

<img src='http://actracktive.googlecode.com/svn/wiki/actracktive-r2-osx_medium.jpg' />

The key features are:

  * Freely configurable graphs (as opposed to linear pipelines) of processing nodes via simple XML description files
  * Generic graphical user interface for configuration of arbitrary settings on processing nodes
  * Non-interactive daemon mode, allowing execution of pre-configured graphs in the background with maximum performance
  * Flexible and extensible software framework to develop new and custom processing nodes which integrate well with existing nodes and the generic UI
  * Modern C++ implementation making use of the standard library as well as the boost library
  * Readily available processing nodes, including:
    * camera capture (via Firewire and generic USB)
    * camera calibration (distortion correction and rectification)
    * video playback
    * static image source (JPEG or PNG)
    * various image filters
    * detection temporal tracking of touches and fiducial markers
    * grid based object calibration
    * OSC/TUIO transmission