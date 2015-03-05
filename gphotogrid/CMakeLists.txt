cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

find_package(wxWidgets REQUIRED core)
include(${wxWidgets_USE_FILE})

# for MacPorts
include_directories(/opt/local/include)
link_directories(/opt/local/lib)

set(CMAKE_CXX_FLAGS "-O2 -g -Wall -Wextra -std=c++11 -stdlib=libc++")
add_definitions(-DHAVE_TYPE_TRAITS) # otherwise wxWidgets headers will try to include non-existing files

add_library(gp STATIC gpwrap.cpp gputil.cpp)
# Enable some C++11 features
# target_compile_features(gp PRIVATE cxx_rvalue_references cxx_deleted_functions cxx_auto_type)
target_link_libraries(gp -lgphoto2 -lgphoto2_port)

add_executable(gphotogrid gphotogrid.cpp optionspanel.cpp imagepanel.cpp timeline.cpp previewfeed.cpp gfxui.cpp app.cpp)
target_link_libraries(gphotogrid gp ${wxWidgets_LIBRARIES})

add_executable(paraphotos paraphotos.cpp signalhandler.cpp)
target_link_libraries(paraphotos gp)

add_executable(timelinetest timelinetest.cpp timeline.cpp)
target_link_libraries(timelinetest ${wxWidgets_LIBRARIES})

add_executable(gpwraptest gptest.cpp signalhandler.cpp)
target_link_libraries(gpwraptest gp)
