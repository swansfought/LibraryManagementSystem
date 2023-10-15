
QT       += core gui sql charts webenginewidgets webchannel network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

RC_ICONS = AppIcon.ico #应用图标

msvc:QMAKE_CXXFLAGS += -execution-charset:utf-8
msvc:QMAKE_CXXFLAGS += -source-charset:utf-8

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    db/dbsql.cpp \
    db/dbtool.cpp \
    main.cpp \
    ui/home.cpp \
    ui/login.cpp

HEADERS += \
    db/dbenum.h \
    db/dbsql.h \
    db/dbtool.h \
    global.h \
    ui/home.h \
    ui/login.h

FORMS += \
    ui/home.ui \
    ui/login.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
