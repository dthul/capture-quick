# CaptureQuick

This software is a graphical UI for the controlling of the camera capture rig that was designed and built by
Konsta Hölttä for his [master's thesis](https://github.com/sooda/thesis). It is a setup that consists of (at the moment)
9 Canon 700D DSLRs.

CaptureQuick allows the user to see a live view of the cameras, control basic settings such as the ISO, shutter speed
and aperture and of course to capture images.

## Requirements

### System

The code was written and tested on Mac OS X. With only a few minor changes it will also run on Linux and Windows.
If you want to use it on any of the latter, contact me and/or send a pull request.

### Software

* A C++11-capable compiler and libc++
* CMake (>= 2.8.11)
* Qt 5 (tested with 5.4)
* libgphoto2 (tested with 2.5.7)

## Basic Build Steps

* Create an empty directory for the build
* Execute `cmake <path-to-capture-quick-root>` out of the build directory
  * If successful you should have a Makefile now
* Compile with `make`

If you want to deploy the build to another machine, take a look at the README.DEPLOY.md file.

For more verbose install instructions, take a look at the INSTALL.md and INSTALL_OSX.md files.
