
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = txt
TEMPLATE = app


SOURCES += main.cpp\
        txtedit.cpp

HEADERS  += txtedit.h

RESOURCES += \
    menu.qrc
RC_FILE += ico.rc
