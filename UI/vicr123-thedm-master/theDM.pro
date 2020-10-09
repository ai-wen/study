#-------------------------------------------------
#
# Project created by QtCreator 2016-10-19T22:21:21
#
#-------------------------------------------------

QT       += core gui x11extras dbus thelib
CONFIG   += c++11
LIBS     += -lX11 -lpam -lpam_misc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = thedm
TEMPLATE = app

DBUS_ADAPTORS += org.freedesktop.DisplayManager.Seat.xml

SOURCES += main.cpp\
        mainwindow.cpp \
    coverframe.cpp \
    pam.cpp \
    dbusseat.cpp

HEADERS  += mainwindow.h \
    coverframe.h \
    pam.h \
    dbusseat.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc


unix {
    target.path = /usr/bin/

    #translations.files = translations/*
    #translations.path = /usr/share/thedm/translations

    systemd.files = thedm.service
    systemd.path = /usr/lib/systemd/system

    pam.files = thedm.pam
    pam.path = /etc/pam.d/

    INSTALLS += target systemd pam #translations
}

DISTFILES += \
    thedm.service \
    thedm.pam
