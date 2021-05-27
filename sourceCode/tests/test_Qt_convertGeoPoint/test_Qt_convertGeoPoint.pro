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

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/src/include/
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/include/
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/ext/fmt/include
#
#
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/lib/libOpenImageIO.a

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Imath/
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/config/ #for IlmBaseConfig.h
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Iex/ #for IexBaseExc.h
DEPENDPATH  += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/

LIBS += -lz -ldl

#static lib linking:
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Half/libHalf-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/OpenEXR/IlmImfUtil/libIlmImfUtil-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/OpenEXR/IlmImf/libIlmImf-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/IlmThread/libIlmThread-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Iex/libIex-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/libImath-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/IexMath/libIexMath-2_4.a

LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_filesystem.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_system.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_thread.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_chrono.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_date_time.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_atomic.a

LIBS += -lpthread

LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/libpng-1.6.37/.libs/libpng16.a

LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/jpeg-8b/.libs/libjpeg.a

LIBS +=-ltiff

#PROJ:
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/src

LIBS += -L$$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/buildhere/lib -lproj

INCLUDEPATH +=../../application/logic

HEADERS += \
    ../../application/logic/model/georef/georefModel_imagesSet.hpp \
    ../../application/logic/model/georef/geoRefImagesSetStatus.hpp \
    ../../application/logic/georef/Proj6_crs_to_crs.h \
    ../../application/logic/georef/ImageGeoRef.h \
    ../../application/logic/georef/WorldFile_parameters.h \
    ../../application/logic/toolbox/toolbox_math.h \
    ../../application/logic/toolbox/toolbox_conversion.h \
    ../../application/logic/toolbox/toolbox_json.h \
    ../../application/logic/toolbox/toolbox_pathAndFile.h \
    ../../application/logic/io/iojsoninfos.h \

SOURCES += \
    ../../application/logic/model/georef/georefModel_imagesSet.cpp \
    ../../application/logic/model/georef/geoRefImagesSetStatus.cpp \
    ../../application/logic/georef/Proj6_crs_to_crs.cpp \
    ../../application/logic/georef/ImageGeoRef.cpp \
    ../../application/logic/georef/WorldFile_parameters.cpp \
    ../../application/logic/toolbox/toolbox_math.cpp \
    ../../application/logic/toolbox/toolbox_conversion.cpp \
    ../../application/logic/toolbox/toolbox_json.cpp \
    ../../application/logic/toolbox/toolbox_pathAndFile.cpp \
    ../../application/logic/io/iojsoninfos.cpp \
    test_Qt_convertGeoPoint.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
