# Deployment

## Mac OS X

Call macdeployqt (bundled with Qt) on the compiled application bundle to
include all required libs.
Also specify the root directory containing the .qml files so that the
proper QtQuick modules can be included in the application bundle.

$ macdeployqt CaptureQuick.app -qmldir=/Users/dthul/Documents/capture/CaptureQuick/

Since libgphoto2 tries to load shared libraries at runtime macdeplyqt does not
know about these libs and won't include them in the application bundle. These
libraries need to be copied and relinked manually:

Copy the iolibs and camlibs (in a MacPorts installation these can be found in
/opt/local/lib/libgphoto2_port and /opt/local/lib/libgphoto2 respectively) into
the bundle's "Frameworks" folder. So after this step you will have for example
a file "./Frameworks/libgphoto2/2.5.7/ptp2.so" in your application bundle.
Btw: You can delete all shared libs from the libgphoto2 folder that do not
correspond to a camera you want to use, so I only kept the ptp2.so (it seems
that the canon.so is not needed for the EOS 700D).

Copy also libusb-0.1.4.dylib and libusb-1.0.0.dylib into the Frameworks folder,
which are needed by libgphoto2_port's usb.so and usb1.so libs.

If you want/need to copy the libs somewhere else you have to adjust the main.cpp
accordingly where the IOLIBS and CAMLIBS environment variables are set.

At last the manually copied libs need to be relinked (a step that macdeployqt
already did to the other libs). With "otool -L <filename>" you can see which
libraries are dynamically linked to this file. If you apply this call to the
bundle's libgphoto2.6.dylib you will see that some libs like libexif are linked
with a path relative to the application bundle. The manually copied libs still
reference some libs with absolute paths. This can be fixed with the
install_name_tool command (adjust paths / versions to your needs):

cd to the bundle's Frameworks folder, then:

mkdir -p libgphoto2/2.5.7
cp /opt/local/lib/libgphoto2/2.5.7/ptp2.so libgphoto2/2.5.7/
cp -r /opt/local/lib/libgphoto2_port .
cp /opt/local/lib/liblzma.5.dylib .
cp /opt/local/lib/libusb-0.1.4.dylib .
cp /opt/local/lib/libusb-1.0.0.dylib .
cp /opt/local/lib/libxml2.2.dylib .
cp /opt/local/lib/libz.1.dylib .

install_name_tool -id @executable_path/../Frameworks/liblzma.5.dylib liblzma.5.dylib
install_name_tool -id @executable_path/../Frameworks/libusb-0.1.4.dylib libusb-0.1.4.dylib
install_name_tool -change /opt/local/lib/libusb-0.1.4.dylib @executable_path/../Frameworks/libusb-0.1.4.dylib libgphoto2_port/0.12.0/usb.so
install_name_tool -id @executable_path/../Frameworks/libusb-1.0.0.dylib libusb-1.0.0.dylib
install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.0.dylib libusb-0.1.4.dylib
install_name_tool -change /opt/local/lib/libusb-1.0.0.dylib @executable_path/../Frameworks/libusb-1.0.0.dylib libgphoto2_port/0.12.0/usb1.so
install_name_tool -id @executable_path/../Frameworks/libxml2.2.dylib libxml2.2.dylib
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib libxml2.2.dylib
install_name_tool -change /opt/local/lib/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib libxml2.2.dylib
install_name_tool -change /opt/local/lib/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib libxml2.2.dylib
install_name_tool -id @executable_path/../Frameworks/libz.1.dylib libz.1.dylib

for LIB in disk.so ptpip.so serial.so usb.so usb1.so usbdiskdirect.so usbscsi.so
do
    install_name_tool -change /opt/local/lib/libgphoto2_port.12.dylib @executable_path/../Frameworks/libgphoto2_port.12.dylib libgphoto2_port/0.12.0/$LIB
    install_name_tool -change /opt/local/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.7.dylib libgphoto2_port/0.12.0/$LIB
    install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib libgphoto2_port/0.12.0/$LIB
done

for LIB in ptp2.so
do
    install_name_tool -change /opt/local/lib/libgphoto2.6.dylib @executable_path/../Frameworks/libgphoto2.6.dylib libgphoto2/2.5.7/$LIB
    install_name_tool -change /opt/local/lib/libgphoto2_port.12.dylib @executable_path/../Frameworks/libgphoto2_port.12.dylib libgphoto2/2.5.7/$LIB
    install_name_tool -change /opt/local/lib/libltdl.7.dylib @executable_path/../Frameworks/libltdl.7.dylib libgphoto2/2.5.7/$LIB
    install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib libgphoto2/2.5.7/$LIB
    install_name_tool -change /opt/local/lib/libexif.12.dylib @executable_path/../Frameworks/libexif.12.dylib libgphoto2/2.5.7/$LIB
    install_name_tool -change /opt/local/lib/libiconv.2.dylib @executable_path/../Frameworks/libiconv.2.dylib libgphoto2/2.5.7/$LIB
    install_name_tool -change /opt/local/lib/libxml2.2.dylib @executable_path/../Frameworks/libxml2.2.dylib libgphoto2/2.5.7/$LIB
done
