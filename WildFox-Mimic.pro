#-------------------------------------------------
#
# Project created by QtCreator 2011-11-22T21:37:41
#
#-------------------------------------------------

QT       += core gui webkit network sql

TARGET = WildFox-Mimic
TEMPLATE = app

CONFIG += uitools

SOURCES += main.cpp\
        mainwindow.cpp \
    wfwebview.cpp \
    iofile.cpp \
    optionsdialog.cpp \
    cookiejar.cpp \
    tldutil.cpp \
    cookiemanagedialog.cpp \
    json.cpp

HEADERS  += mainwindow.h \
    wfwebview.h \
    iofile.h \
    optionsdialog.h \
    cookiejar.h \
    tldutil.h \
    qurltlds_p.h \
    cookiemanagedialog.h \
    json.h

FORMS    += mainwindow.ui \
    optionsdialog.ui \
    cookiemanagedialog.ui

RESOURCES += \
    images.qrc

#LIBS += -lgcc_s




