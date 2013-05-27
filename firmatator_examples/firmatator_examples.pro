#-------------------------------------------------
#
# Project created by QtCreator 2012-05-08T18:37:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = firmatator_examples
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    include/firmatator/moc_serialport.cpp \
    ftest.cpp

#
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/firmatator/release/ -lfirmatator
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/include/firmatator/debug/ -lfirmatator
#else:symbian: LIBS += -lfirmatator
#else:unix: LIBS += -L$$PWD/include/firmatator/ -lfirmatator
#
#INCLUDEPATH += $$PWD/include/firmatator
#DEPENDPATH += $$PWD/include/firmatator
#

HEADERS += \
    include/firmatator/firmatator.h \
    include/firmatator/fdevice.h \
    ftest.h




win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/firmatator/release/ -lfirmatator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/include/firmatator/debug/ -lfirmatator
else:symbian: LIBS += -lfirmatator
else:unix: LIBS += -L$$PWD/include/firmatator/ -lfirmatator

INCLUDEPATH += $$PWD/include/firmatator
DEPENDPATH += $$PWD/include/firmatator

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/include/QSerialDevice/release/ -lSerialPort
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/include/QSerialDevice/debug/ -lSerialPort
else:symbian: LIBS += -lSerialPort
else:unix: LIBS += -L$$PWD/include/QSerialDevice/ -lSerialPort

INCLUDEPATH += $$PWD/include/QSerialDevice
DEPENDPATH += $$PWD/include/QSerialDevice


