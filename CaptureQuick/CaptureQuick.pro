TEMPLATE = app

QT += qml quick widgets

CONFIG += c++11

SOURCES += main.cpp \
    liveimageprovider.cpp \
    camera.cpp \
    capture.cpp \
    cameracontroller.cpp \
    cameraeventlistener.cpp \
    image.cpp

RESOURCES += qml.qrc \
    images.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    liveimageprovider.h \
    camera.h \
    capture.h \
    cameracontroller.h \
    cameraeventlistener.h \
    image.h

# For MacPorts
QMAKE_LIBDIR += /opt/local/lib

# Link against libgp and libgphoto2

# libgphoto2 is assumed to be available system wide
LIBS += -lgphoto2 -lgphoto2_port

# libg is assumed to be in the folder ../gphotogrid/build
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../gphotogrid/build/release/ -lgp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../gphotogrid/build/debug/ -lgp
else:unix: LIBS += -L$$PWD/../gphotogrid/build/ -lgp

INCLUDEPATH += $$PWD/../gphotogrid
DEPENDPATH += $$PWD/../gphotogrid

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../gphotogrid/build/release/libgp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../gphotogrid/build/debug/libgp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../gphotogrid/build/release/gp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../gphotogrid/build/debug/gp.lib
else:unix: PRE_TARGETDEPS += $$PWD/../gphotogrid/build/libgp.a
