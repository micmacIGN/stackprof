QT -= gui

CONFIG += c++11 console

CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

!include( ../thirdpartylibs.pri  ) {
    error( "No ../thirdpartylibs.pri file found" )
}

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Imath/
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/config/ #for IlmBaseConfig.h

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Iex/ #for IexBaseExc.h
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/src/include/
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/include/

INCLUDEPATH  +=  ../../application/logic
INCLUDEPATH  +=  ../../application

HEADERS += \
        ../../application/logic/model/dialogProfilsCurves/landmarks.h \
        ../../application/logic/toolbox/toolbox_math.h \
        ../../application/logic/model/core/S_Segment.h \

SOURCES += \
        ../../application/logic/model/dialogProfilsCurves/landmarks.cpp \
        ../../application/logic/toolbox/toolbox_math.cpp \
        ../../application/logic/model/core/S_Segment.cpp \
\
        test_landmarkComputation.cpp \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
