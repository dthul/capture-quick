# Install the requirements

  * XCode from the App Store
    * make sure that you also install the XCode Command Line tools using `$ xcode-select --install`
  * CMake (https://cmake.org/download/)
  * Qt5 SDK (http://www.qt.io/download/)
    * It is fine to install it to $HOME/Qt instead of system-wide
    * You can uncheck stuff like Android armv7, iOS... We only need the prebuilt clang 64-bit libs and Qt Quick Controls
  * libgphoto2, for example using Homebrew (http://brew.sh/):
    * `$ brew install libgphoto2`
    * mine got installed to `/usr/local/Cellar/libgphoto2/2.5.7`

# Download and build CaptureQuick

Check out the repository:

    $ git clone https://github.com/dthul/capture-quick.git

Create a build folder, configure the project, compile and link it (of course you have to adjust the path to Qt5. CMake will tell you if something went wrong though):

    $ cd capture-quick
    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_PREFIX_PATH=/Users/dthul/Qt/5.5/clang_64/ ..
    $ make

You should now have the CaptureQuick application in your `build` folder.

Should CMake be unable to find libgphoto2 you also have to add its path to the CMake search paths, e.g.:

    $  cmake -DCMAKE_PREFIX_PATH=/Users/dthul/Qt/5.5/clang_64/:/usr/local/Cellar/libgphoto2/2.5.7 ..
