QT += core
QT += widgets

CONFIG += console
CONFIG -= app_bundle

CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = test_geoconvertroute
TEMPLATE = app

!include( ../thirdpartylibs.pri  ) {
    error( "No ../thirdpartylibs.pri file found" )
}


INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/src/include/
INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/include/

INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/ext/fmt/include

LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/lib/libOpenImageIO.a

#openexr header installed here:
INCLUDEPATH += "/usr/local/include/"

INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Imath/
INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/config/ #for IlmBaseConfig.h
INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Iex/ #for IexBaseExc.h
DEPENDPATH  += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/

LIBS += -lz -ldl

#static lib linking:
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Half/libHalf-2_4.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/OpenEXR/IlmImfUtil/libIlmImfUtil-2_4.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/OpenEXR/IlmImf/libIlmImf-2_4.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/IlmThread/libIlmThread-2_4.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Iex/libIex-2_4.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/libImath-2_4.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/IexMath/libIexMath-2_4.a


LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_filesystem.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_system.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_thread.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_chrono.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_date_time.a
LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_atomic.a

LIBS += -lpthread

INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Imath/

#----------------
#PROJ:
#----------------
INCLUDEPATH += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/src

LIBS += -L$$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/buildhere/lib -lproj


LIBS += -lm

LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/libpng-1.6.37/.libs/libpng16.a

LIBS += $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT/jpeg-8b/.libs/libjpeg.a

LIBS +=-ltiff

INCLUDEPATH  +=  ../../application
INCLUDEPATH  +=  ../../application/logic
INCLUDEPATH  +=  ../../application/ui

SOURCES += \
    ../../application/logic/model/core/S_Segment.cpp \
    ../../application/logic/model/georef/geoRefImagesSetStatus.cpp \
    ../../application/logic/model/georef/georefModel_imagesSet.cpp \
    ../../application/logic/model/core/route.cpp \
    ../../application/logic/model/core/routeContainer.cpp \
\
    ../../application/logic/model/core/geoConvertRoute.cpp \
\
    ../../application/logic/georef/ImageGeoRef.cpp \
    ../../application/logic/georef/Proj6_crs_to_crs.cpp \
    ../../application/logic/georef/WorldFile_parameters.cpp \
    ../../application/logic/io/iojsoninfos.cpp \
    ../../application/logic/toolbox/toolbox_json.cpp \
    ../../application/logic/toolbox/toolbox_string.cpp \
    ../../application/logic/toolbox/toolbox_pathAndFile.cpp \
    ../../application/logic/toolbox/toolbox_math.cpp \
    ../../application/logic/toolbox/toolbox_math_geometry.cpp \
    ../../application/logic/toolbox/toolbox_conversion.cpp \
    ../../application/logic/pathesAccess/appDataResourcesAccess.cpp \
    ../../application/logic/pathesAccess/InternalDataPathAccess.cpp \
    main.cpp

HEADERS += \
    ../../application/logic/model/core/S_Segment.h \
    ../../application/logic/model/georef/geoRefImagesSetStatus.hpp \
    ../../application/logic/model/georef/georefModel_imagesSet.hpp \
    ../../application/logic/model/core/route.h \
    ../../application/logic/model/core/routeContainer.h \
\
    ../../application/logic/model/core/geoConvertRoute.h \
\
    ../../application/logic/georef/geo_basic_struct.h \
    ../../application/logic/georef/ImageGeoRef.h \
    ../../application/logic/georef/Proj6_crs_to_crs.h \
    ../../application/logic/georef/WorldFile_parameters.h \
    ../../application/logic/io/iojsoninfos.h \
    ../../application/logic/toolbox/toolbox_json.h \
    ../../application/logic/toolbox/toolbox_string.h \
    ../../application/logic/toolbox/toolbox_pathAndFile.h \
    ../../application/logic/toolbox/toolbox_math.h \
    ../../application/logic/toolbox/toolbox_math_geometry.h \
    ../../application/logic/toolbox/toolbox_conversion.h \
    ../../application/logic/pathesAccess/appDataResourcesAccess.h \
    ../../application/logic/pathesAccess/InternalDataPathAccess.cpp \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
