#-------------------------------------------------
#
# Project created by QtCreator 2016-05-25T09:38:21
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = zizhu
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    transdialog.cpp \
    detaildialog.cpp \
    functions.cpp \
    zzkeypad.cpp \
    pos/appdebug.cpp \
    pos/apputils.cpp \
    pos/comm.cpp \
    pos/ctls.cpp \
    pos/iccmd.cpp \
    pos/iso8583.cpp \
    pos/keymngr.cpp \
    pos/prmmngr.cpp \
    pos/prndisp.cpp \
    pos/trans.cpp \
    pos/zontalkap.cpp \
    transsocket.cpp \
    pos/Proc8583.cpp \
    pos/sd_des.cpp \
    zzprinter.cpp \
    cardreader.cpp \
    singlecoldialog.cpp

HEADERS  += widget.h \
    transdialog.h \
    detaildialog.h \
    functions.h \
    zzkeypad.h \
    pos/appdebug.h \
    pos/apputils.h \
    pos/comm.h \
    pos/cupms.h \
    pos/define.h \
    pos/iccmd.h \
    pos/iso8583.h \
    pos/multdef.h \
    pos/prmmngr.h \
    pos/prndisp.h \
    pos/trans.h \
    transsocket.h \
    pos/Proc8583.h \
    pos/sd_des.h \
    zzprinter.h \
    cardreader.h \
    singlecoldialog.h \
    C:/Users/Administrator/Desktop/新建文件夹 (2)/singleton.h \
    C:/Users/Administrator/Desktop/新建文件夹 (2)/singleton.h \
    C:/Users/Administrator/Desktop/新建文件夹 (2)/singleton.h \
    singleton.h \
    call_once.h

FORMS    += widget.ui \
    transdialog.ui \
    detaildialog.ui \
    singlecoldialog.ui
