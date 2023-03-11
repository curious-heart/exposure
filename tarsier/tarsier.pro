#QT       += core gui serialbus serialport xml network websockets quick qml virtualkeyboard
QT       += core gui serialbus serialport xml network websockets
QTPLUGIN += qtvirtualkeyboardplugin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ExpoParamSettingdialog.cpp \
    exitsystem.cpp \
    fpdmodels.cpp \
    fpdsetting.cpp \
    globalapplication.cpp \
    imagelabel.cpp \
    imageoperation.cpp \
    logger.cpp \
    lowbatterywarning.cpp \
    main.cpp \
    mainwindow.cpp \
    maskwidget.cpp \
    mycontroller.cpp \
    myfpd.cpp \
    settingcfg.cpp \
    systemsetting.cpp \
    thirdpart/qtlocalpeer.cpp \
    thirdpart/qtlockedfile.cpp \
    thirdpart/qtlockedfile_unix.cpp \
    thirdpart/qtlockedfile_win.cpp \
    thirdpart/qtsingleapplication.cpp \
    thirdpart/qtsinglecoreapplication.cpp \
    version_def.cpp

HEADERS += \
    ExpoParamSettingdialog.h \
    IRayInclude.h \
    exitsystem.h \
    fpdmodels.h \
    fpdsetting.h \
    globalapplication.h \
    imagelabel.h \
    imageoperation.h \
    include/IRayAttrDef.h \
    include/IRayCmdDef.h \
    include/IRayCommon.h \
    include/IRayEnumDef.h \
    include/IRayErrDef.h \
    include/IRayEventDef.h \
    include/IRayFpdSys.h \
    include/IRayFpdSysEx.h \
    include/IRayImage.h \
    include/IRayVariant.h \
    include/TiffParser.h \
    logger.h \
    lowbatterywarning.h \
    mainwindow.h \
    maskwidget.h \
    mycontroller.h \
    myfpd.h \
    pzm/sdk_4.1.16/ComApi/NetCom.h \
    settingcfg.h \
    systemsetting.h \
    thirdpart/qtlocalpeer.h \
    thirdpart/qtlockedfile.h \
    thirdpart/qtsingleapplication.h \
    thirdpart/qtsinglecoreapplication.h \
    version_def.h

FORMS += \
    ExpoParamSettingdialog.ui \
    exitsystem.ui \
    fpdsetting.ui \
    lowbatterywarning.ui \
    mainwindow.ui \
    maskwidget.ui \
    systemsetting.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    source.qrc

DISTFILES += \
    data/base.xml \
    data/settingCfg.ini \
    images/green.png \
    images/red.png \
    workdir_path
#INCLUDEPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\winrt"
LIBS += -lpowrprof
