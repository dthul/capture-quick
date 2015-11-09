# Requirements

  * Qt5
  * libgphoto2


# Install steps

## Linux and Mac OS

Create a build directory and execute cmake, then make.
Make sure that the libgphoto2 and libgphoto2_port headers and libraries can be
found.

If your Qt5 installation is in a non-standard path you can call for example

$ cmake -DCMAKE_PREFIX_PATH=/Users/dthul/Qt/5.5/clang_64/ ..

## Windows

Create a Visual Studio project with CMake and compile it.
