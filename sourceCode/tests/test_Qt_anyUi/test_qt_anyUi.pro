QT       += core gui

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

INCLUDEPATH  +=  ../../application/ui/appSettings

SOURCES += \
    main.cpp \
    mainwindow.cpp \
\
    ../../application/ui/appSettings/dialog_graphicsSettings.cpp \
\
    ../../application/logic/model/appSettings/SAppSettings.cpp \
    ../../application/logic/model/appSettings/appSettings.cpp \
\
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.cpp \
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.cpp \
\
    ../../application/logic/toolbox/toolbox_pathAndFile.cpp

HEADERS += \
    mainwindow.h \
    ../../application/ui/appSettings/dialog_graphicsSettings.h \
\
    ../../application/logic/model/appSettings/SAppSettings.h \
    ../../application/logic/model/appSettings/appSettings.h \
\
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h \
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h \
\
    ../../application/logic/toolbox/toolbox_pathAndFile.h

FORMS += \
    mainwindow.ui \
    ../../application/dotui/appSettings/dialog_graphicsSettings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
