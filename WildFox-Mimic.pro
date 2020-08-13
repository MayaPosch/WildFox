#-------------------------------------------------
#
# Project created by QtCreator 2011-11-22T21:37:41
#
#-------------------------------------------------

QT       += core gui webkit network sql widgets webkit webkitwidgets uitools

TARGET = WildFox
TEMPLATE = app

CONFIG += console

#QT += uitools

SOURCES += main.cpp\
        mainwindow.cpp \
    wfwebview.cpp \
    iofile.cpp \
    optionsdialog.cpp \
    cookiejar.cpp \
    tldutil.cpp \
    cookiemanagedialog.cpp \
    json.cpp \
    bookmarks.cpp \
    chrome/chromejsinterface.cpp \
    chrome/chromebookmarks.cpp \
    wfwebpage.cpp \
    wfwebhistoryinterface.cpp \
    historydatabase.cpp \
    addressbareventfilter.cpp \
    addressbardelegate.cpp \
    historydialog.cpp \
    addressbarcompleter.cpp

HEADERS  += mainwindow.h \
    wfwebview.h \
    iofile.h \
    optionsdialog.h \
    cookiejar.h \
    tldutil.h \
    qurltlds_p.h \
    cookiemanagedialog.h \
    json.h \
    types.h \
    bookmarks.h \
    chrome/chromejsinterface.h \
    chrome/chromebookmarks.h \
    wfwebpage.h \
    wfwebhistoryinterface.h \
    historydatabase.h \
    addressbareventfilter.h \
    addressbardelegate.h \
    historydialog.h \
    addressbarcompleter.h

FORMS    += mainwindow.ui \
    optionsdialog.ui \
    cookiemanagedialog.ui \
    historydialog.ui

RESOURCES += \
    images.qrc

#LIBS += -lgcc_s





