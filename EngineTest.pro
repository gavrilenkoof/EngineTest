QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
QMAKE_LFLAGS += -no-pie

CONFIG(debug, debug|release) {

} else {
    DEFINES += RELEASE
}

win32: {
    QMAKE_CXXFLAGS += -Wa,-mbig-obj
    RC_ICONS = icons/engine.ico
}


SOURCES += \
    axistag.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp \
    motorcharacteristics.cpp \
    qcustomplot.cpp \
    realtimegraphs.cpp \
    serialport.cpp \
    settingsdialog.cpp

HEADERS += \
    axistag.h \
    logger.h \
    mainwindow.h \
    motorcharacteristics.h \
    qcustomplot.h \
    realtimegraphs.h \
    serialport.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    realtimegraphs.ui \
    settingsdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res_icons.qrc
