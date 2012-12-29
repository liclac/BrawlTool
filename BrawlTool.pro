#-------------------------------------------------
#
# Project created by QtCreator 2012-12-23T19:02:48
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BrawlTool
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    ClickableLabel.cpp

HEADERS  += MainWindow.h \
    ClickableLabel.h

FORMS    += MainWindow.ui

RESOURCES += \
    Resources.qrc

OTHER_FILES += \
    Grid_Brawl.xml
