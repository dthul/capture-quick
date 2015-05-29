cmake_minimum_required(VERSION 2.8.11)

project(CaptureQuick)


### Compile the libgphoto2 wrapper library libgp

# For MacPorts
include_directories(/opt/local/include)
link_directories(/opt/local/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11 -stdlib=libc++")

set(LIB_SOURCES
    src/lib/gputil.cpp
    src/lib/gpwrap.cpp)

add_library(gp STATIC ${LIB_SOURCES})
target_link_libraries(gp -lgphoto2 -lgphoto2_port)


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

add_executable(CaptureQuick ${UI_SOURCES} ${RESOURCES})
target_link_libraries(CaptureQuick Qt5::Quick Qt5::Widgets Qt5::SerialPort gp)
