cmake_minimum_required(VERSION 2.8.11)

project(CaptureQuick)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11 -stdlib=libc++")


### Compile the libgphoto2 wrapper library libgp

find_path(LIBGPHOTO_INC_DIR
    NAMES gphoto2.h
    PATH_SUFFIXES include/gphoto2
    DOC "libgphoto2 include path")

if(NOT LIBGPHOTO_INC_DIR)
    message(SEND_ERROR "The libgphoto2 includes could not be found. Please specify libgphoto2's root folder in the CMAKE_PREFIX_PATH (for a Homebrew installation this could be something like '/usr/local/Cellar/libgphoto2/2.5.7/')")
endif()

find_library(LIBGPHOTO_LIB
    NAMES libgphoto2.dylib libgphoto2.so
    PATH_SUFFIXES lib
    DOC "libgphoto2 library path")

if(NOT LIBGPHOTO_LIB)
    message(SEND_ERROR "The libgphoto2 library could not be found. Please specify libgphoto2's root folder in the CMAKE_PREFIX_PATH (for a Homebrew installation this could be something like '/usr/local/Cellar/libgphoto2/2.5.7/')")
endif()

find_library(LIBGPHOTO_PORT_LIB
    NAMES libgphoto2_port.dylib libgphoto2_port.so
    PATH_SUFFIXES lib
    DOC "libgphoto2-port library path")

if(NOT LIBGPHOTO_PORT_LIB)
    message(SEND_ERROR "The libgphoto2-port library could not be found. Please specify libgphoto2's root folder in the CMAKE_PREFIX_PATH (for a Homebrew installation this could be something like '/usr/local/Cellar/libgphoto2/2.5.7/')")
endif()

include_directories(${LIBGPHOTO_INC_DIR})

set(LIB_SOURCES
    src/lib/gputil.cpp
    src/lib/gpwrap.cpp)

add_library(gp STATIC ${LIB_SOURCES})
target_link_libraries(gp ${LIBGPHOTO_LIB} ${LIBGPHOTO_PORT_LIB})


### Compile the main application

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the Qt libraries
find_package(Qt5 COMPONENTS Quick Widgets SerialPort REQUIRED)

set(UI_SOURCES
    src/camera.cpp
    src/cameracontroller.cpp
    src/cameraeventlistener.cpp
    src/capture.cpp
    src/image.cpp
    src/liveimageprovider.cpp
    src/main.cpp
    src/persist.cpp
    src/triggerbox.cpp
    src/util.cpp
    src/videoimporter.cpp)

set(UI_RESOURCES
    src/fonts.qrc
    src/images.qrc
    src/qml.qrc)

qt5_add_resources(RESOURCES ${UI_RESOURCES})

include_directories(src/lib)

if (APPLE)
    set(UI_SOURCES ${UI_SOURCES} src/icon.icns)
    set_source_files_properties(src/icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif()

add_executable(CaptureQuick
    WIN32
    MACOSX_BUNDLE
    ${UI_SOURCES}
    ${RESOURCES})
target_link_libraries(CaptureQuick Qt5::Quick Qt5::Widgets Qt5::SerialPort gp)
set_target_properties(CaptureQuick
    PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in"
    MACOSX_BUNDLE_BUNDLE_NAME "CaptureQuick"
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
    MACOSX_BUNDLE_LONG_VERSION_STRING "0.1"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2015 Daniel Thul."
    MACOSX_BUNDLE_GUI_IDENTIFIER "fi.aalto.capturequick"
    MACOSX_BUNDLE_ICON_FILE icon.icns)
