TEMPLATE = app
QT += serialport
QT -= gui

SOURCES += main.cpp \
    modbusparser.cpp \
    debuglogger.cpp

HEADERS += \
    modbusparser.h \
    test.h \
    debuglogger.h
