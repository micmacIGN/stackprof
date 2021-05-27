QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

message("Qt version:" $$QT_VERSION)

#versionAtLeast(QT_VERSION, 5.6versionNumber)  # introduced in qt 5.10
lessThan(QT_MAJOR_VERSION, 5) {
    error("this projet needs Qt version >= Qt 5.12")
}

equals(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MINOR_VERSION, 12) {
        error("this projet needs Qt version >= Qt 5.12")
    }
}

# #TagToFindAboutReleaseDeliveryOption
# RELEASE DELIVERY: --------
#
#CONFIG += release
#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

QT += core

QT += concurrent #loading tile in a separated thread

#greaterThan(QT_MAJOR_VERSION, 4):
QT += widgets

TARGET = test_relocation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
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
DEPENDPATH  += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/

LIBS += -lm

CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14

INCLUDEPATH  +=  ../../application
INCLUDEPATH  +=  ../../application/logic

SOURCES += \
    ../../application/logic/toolbox/toolbox_math_geometry.cpp \
    ../../application/logic/toolbox/toolbox_math.cpp \
    ../../application/logic/model/core/S_Segment.cpp \
    main.cpp

HEADERS += \
    ../../application/logic/toolbox/toolbox_math_geometry.h \
    ../../application/logic/toolbox/toolbox_math.h \
    ../../application/logic/model/core/S_Segment.h \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
