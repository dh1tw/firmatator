#-------------------------------------------------
#
# Project created by QtCreator 2012-05-04T18:10:33
#
#-------------------------------------------------

QT       -= gui

TARGET = firmatator
TEMPLATE = lib

DEFINES += FIRMATATOR_LIBRARY

SOURCES += \
    fdevice.cpp \
    util.cpp

HEADERS +=\
    firmatator_global.h \
    fdevice.h \
    firmatator.h \
    include/QSerialDevice/serialportinfo.h \
    include/QSerialDevice/serialport.h \
    util.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE7161521
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = firmatator.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

OTHER_FILES += \
    include/QSerialDevice/libSerialPort.so.1.0.0 \
    include/QSerialDevice/libSerialPort.so.1.0 \
    include/QSerialDevice/libSerialPort.so.1 \
    include/QSerialDevice/libSerialPort.so














win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../serialport-build-simulator-Simulator_Qt_for_GCC__Qt_SDK__Release/src/release/ -lSerialPort
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../serialport-build-simulator-Simulator_Qt_for_GCC__Qt_SDK__Release/src/debug/ -lSerialPort
else:symbian: LIBS += -lSerialPort
else:unix: LIBS += -L$$PWD/../serialport-build-simulator-Simulator_Qt_for_GCC__Qt_SDK__Release/src/ -lSerialPort

INCLUDEPATH += $$PWD/../qserialdevice-qserialdevice/src
DEPENDPATH += $$PWD/../qserialdevice-qserialdevice/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/QSerialDevice/release/ -lSerialPort
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/include/QSerialDevice/debug/ -lSerialPort
else:symbian: LIBS += -lSerialPort
else:unix: LIBS += -L$$PWD/include/QSerialDevice/ -lSerialPort

INCLUDEPATH += $$PWD/include/QSerialDevice
DEPENDPATH += $$PWD/include/QSerialDevice


