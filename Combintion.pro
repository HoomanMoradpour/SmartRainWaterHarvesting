QT      += core gui network widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += _version6

INCLUDEPATH += ../Projects/WiringPi

#DEFINES += GPIO

SOURCES += \
    CSVParser.cpp \
    downloader.cpp \
    DistanceSensor.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    CSVParser.h \
    DistanceSensor.h \
    downloader.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

LIBS += -lwiringPi

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
