#-------------------------------------------------
#
# Project created by QtCreator 2017-07-18T11:28:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = customRide
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    leftwidget.cpp \
    qcustomplot.cpp \
    plottime.cpp \
    plotfrequency.cpp

HEADERS  += mainwindow.h \
    leftwidget.h \
    qcustomplot.h \
    plottime.h \
    plotfrequency.h

QT += axcontainer

RESOURCES += \
    images/image.qrc

DISTFILES +=
