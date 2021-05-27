QT += gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

!include( ../thirdpartylibs.pri  ) {
    error( "No ../thirdpartylibs.pri file found" )
}

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/src/include/
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/include/

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/ext/fmt/include


INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Imath/
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/config/ #for IlmBaseConfig.h
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Iex/ #for IexBaseExc.h

#static lib linking:

LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/libImath-2_4.a

INCLUDEPATH  +=  ../../application
INCLUDEPATH  +=  ../../application/logic

HEADERS += \
    ../../application/logic/model/core/S_Segment.h \
    ../../application/logic/model/core/route.h \
    ../../application/logic/model/core/routeContainer.h \
    ../../application/logic/toolbox/toolbox_conversion.h \
    ../../application/logic/toolbox/toolbox_json.h \
    ../../application/logic/toolbox/toolbox_string.h \
    ../../application/logic/toolbox/toolbox_math_geometry.h \
    ../../application/logic/toolbox/toolbox_math.h \
    ../../application/logic/toolbox/toolbox_pathAndFile.h \

SOURCES += \
    ../../application/logic/model/core/S_Segment.cpp \
    ../../application/logic/model/core/route.cpp \
    ../../application/logic/model/core/routeContainer.cpp \
    ../../application/logic/toolbox/toolbox_conversion.cpp \
    ../../application/logic/toolbox/toolbox_json.cpp \
    ../../application/logic/toolbox/toolbox_string.cpp \
    ../../application/logic/toolbox/toolbox_math_geometry.cpp \
    ../../application/logic/toolbox/toolbox_math.cpp \
    ../../application/logic/toolbox/toolbox_pathAndFile.cpp \
    main.cpp

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
