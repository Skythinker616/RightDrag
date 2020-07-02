QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
    APP_VERSION=\\\"1.0\\\"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutdialog.cpp \
    donatedialog.cpp \
    grab/floatdialog.cpp \
    grab/grabber.cpp \
    main.cpp \
    mainwindow.cpp \
    net/ocr.cpp \
    net/trans.cpp \
    settingsdialog.cpp

HEADERS += \
    aboutdialog.h \
    donatedialog.h \
    grab/floatdialog.h \
    grab/grabber.h \
    mainwindow.h \
    net/ocr.h \
    net/trans.h \
    settingsdialog.h

FORMS += \
    aboutdialog.ui \
    donatedialog.ui \
    grab/floatdialog.ui \
    mainwindow.ui \
    settingsdialog.ui

RC_FILE = resource.rc

OTHER_FILES += \
    help.md \
    README.md \
    LICENSE \
    icon.ico \
    icon.png \
    wechat.png \
    alipay.png



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
