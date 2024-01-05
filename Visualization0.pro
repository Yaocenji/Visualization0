QT       += core gui
QT       += network
QT       += charts
INCLUDEPATH += E:\Qt\5.15.2\mingw81_64\bin\OpenSSL-Win64\include

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

requires(qtConfig(tablewidget))

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myjson.cpp \
    myrequester.cpp \
    themewidget.cpp

HEADERS += \
    mainwindow.h \
    myjson.h \
    myrequester.h \
    themewidget.h

FORMS += \
    themewidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
