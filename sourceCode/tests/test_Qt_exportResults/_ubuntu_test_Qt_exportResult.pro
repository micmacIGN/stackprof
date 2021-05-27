#QT += gui

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



#include( thirdpartylibs.pri )
!include( ../thirdpartylibs.pri  ) {
    error( "No ../thirdpartylibs.pri file found" )
}

unix:!macx {
    INPRO__THIRD_PARTY_LIBS_ROOT = $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT
}
macx: {
    INPRO__THIRD_PARTY_LIBS_ROOT = $$MACOS_INPRO__THIRD_PARTY_LIBS_ROOT
}

# adjust INPRO__THIRD_PARTY_LIBS_ROOT to target the third party libs root directory
#INPRO__THIRD_PARTY_LIBS_ROOT = /home/laurent/thirdpartylibs/src

#
#INCLUDEPATH += "/Users/laurent/testcompil/fmt-7.1.0/include"
#INCLUDEPATH += "/home/laurent/thirdpartylibs/src/oiio-RB-2.1__2.1.20__/oiio-RB-2.1
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/src/include/
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/include/
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/ext/fmt/include
#
#
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/lib/libOpenImageIO.a
#
#LIBS += "/Users/laurent/testcompil/fmt-7.1.0/28oct/libfmt.a"

#openexr header installed here:
INCLUDEPATH += "/usr/local/include/"
#
#
#
#
#

INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Imath/
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/config/ #for IlmBaseConfig.h
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/IlmBase/Iex/ #for IexBaseExc.h
DEPENDPATH  += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/

LIBS += -lz -ldl #@LP because missing ref when compiling

#static lib linking:
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Half/libHalf-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/OpenEXR/IlmImfUtil/libIlmImfUtil-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/OpenEXR/IlmImf/libIlmImf-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/IlmThread/libIlmThread-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Iex/libIex-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/Imath/libImath-2_4.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/openexr-2.4.0/buildhere/IlmBase/IexMath/libIexMath-2_4.a

#LIBS += "/opt/local/lib/libHalf.a"
#LIBS += "/opt/local/lib/libIlmImfUtil.a"
#LIBS += "/opt/local/lib/libIlmImf.a"
#LIBS += "/opt/local/lib/libIlmThread.a"
#LIBS += "/opt/local/lib/libIex.a"
#LIBS += "/opt/local/lib/libImath.a"
#LIBS += "/opt/local/lib/libIexMath.a"

#static lib linking:
#LIBS += "/usr/lib/x86_64-linux-gnu/libboost_filesystem.a"
#LIBS += "/usr/lib/x86_64-linux-gnu/libboost_system.a"
#LIBS += "/usr/lib/x86_64-linux-gnu/libboost_thread.a"
#LIBS += "/usr/lib/x86_64-linux-gnu/libboost_chrono.a"
#LIBS += "/usr/lib/x86_64-linux-gnu/libboost_date_time.a"
#LIBS += "/usr/lib/x86_64-linux-gnu/libboost_atomic.a"

LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_filesystem.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_system.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_thread.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_chrono.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_date_time.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_atomic.a

#LIBS += "/usr/lib/x86_64-linux-gnu/libpthread.a"
LIBS += -lpthread


#installed using brew
#static:
#
#LIBS += "/usr/local/opt/libpng/lib/libpng.a"
#LIBS += "/usr/local/opt/libtiff/lib/libtiff.a"
#LIBS += "/usr/local/opt/libjpeg/lib/libjpeg.a"
#

#installed using brew
#dylib:
#LIBS += -L"/usr/local/opt/libtiff/lib" -ltiff
#11sept try, removing jpeg dependencied from libtiff (this one: /usr/local/opt/libtiff/lib)
#do not forget to update makeAppBundle.sh if the copy and relocation of the libtiff is not what we want)
#
# for release:
#LIBS += -L"/Users/laurent/testcompil/libtiff-master_withSomeFeaturesOff_try11091/lpbuild11091/libtiff" -ltiff
#LIBS += -L"/usr/local/opt/libjpeg/lib" -ljpeg
#LIBS += -L"/usr/local/opt/libpng/lib" -lpng
#


# for release :
#LIBS += "/usr/local/opt/libpng/lib/libpng.a"
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/libpng-1.6.37/.libs/libpng16.a

#LIBS += "/usr/local/opt/libjpeg/lib/libjpeg.a"
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/jpeg-8b/.libs/libjpeg.a

#local directory vs /usr/local/lib -ltiff
#LIBS += -L"/home/laurent/work/thirdpartylibs/libtiff-master/buildhere/libtiff" -ltiff
LIBS +=-ltiff

#alternative for debug purpose:
# = no LIBS linking



#from local compilation:
#LIBS += -ltiff
# #LP please add lib tiff with the other .so lib provided with the app

#from local compilation:
# 6 is not ok, oiio needs 8
#@LP (note that the provided configure file needs a dos2unix conversion)
##LIBS += "/home/laurent/work/libjpegsr6/jpeg-6b/libjpeg.a"

#LIBS += -ljpeg
###  ls -l /usr/lib/x86_64-linux-gnu/*jpeg*




#----------------
#OIIO
#----------------
#
#
#
#
#
#
#
# openexr/IlmBase include for math vector usage
#
#
#
#
#
#
#not used:
#
#
##
#
#
#----------------


#----------------
#GSL
#----------------
# ipgp
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6
# ### LP:  -lm : not needed on MacOS X; see https://www.gnu.org/software/gsl/doc/html/usage.html
# ### DYNAMIC_LIB = m
#
#
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6/.libs/libgsl.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6/cblas/.libs/libgslcblas.a

LIBS += -lm
#----------------

#----------------
#GSL
#----------------
# home
#
# ###
# ###
#
#
#
#----------------


#----------------
#PROJ:
#----------------
#
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/src

#home:
#

#
#
#

#
#
#
#LIBS += -L"/home/laurent/work/thirdpartylibs/proj-6.3.0/buildhere/lib" -lproj
LIBS += -L$$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/buildhere/lib -lproj

#
#
#----------------

#



#
#
#
#
#
#
#
#
#
#
#
#
#


#
#
#

#
#
########
##
##
##

#
#






CONFIG += c++14
#QMAKE_CXXFLAGS += -std=c++11
#CONFIG += strict_c++ #check if needs or not

#QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=c++14

#for debugging :
#QMAKE_CXXFLAGS += -lefence

DEPENDPATH += . \
              ../../application/logic \
#             ../../../application/logic/core
#             ../../../

INCLUDEPATH += ../../application/logic \
               ../../application \
               ../../ \

HEADERS += \
    ./../../application/logic/model/ApplicationModel.h \
    ./../../application/logic/model/core/ComputationCore.h \
    ./../../application/logic/model/core/ComputationCore_inheritQATableModel.h \
    ./../../application/logic/model/core/ComputationCore_structures.h \
    ./../../application/logic/model/core/ComputationCore_inputFilesChecker.h \
    ./../../application/logic/model/core/S_Segment.h \
    ./../../application/logic/model/core/route.h \
    ./../../application/logic/model/core/routeContainer.h \
    ./../../application/logic/toolbox/toolbox_conversion.h \
    ./../../application/logic/toolbox/toolbox_string.h \
    ./../../application/logic/toolbox/toolbox_math.h \
    ./../../application/logic/toolbox/toolbox_math_geometry.h \
    ./../../application/logic/toolbox/toolbox_pathAndFile.h \
    ./../../application/logic/toolbox/toolbox_json.h \
    ./../../application/logic/io/PixelPicker.h \
    ./../../application/logic/model/core/valueDequantization.h \
    ./../../application/logic/mathComputation/meanMedian.h \
    ./../../application/logic/mathComputation/linearRegression.h \
    ./../../application/logic/model/core/IDGenerator.h \
    ./../../application/logic/io/InputImageFormatChecker.h \
    ./../../application/logic/vecttype.h \
    ./../../application/logic/model/core/ComputationCore_boxOrienterAndDistributer.h \
    ./../../application/logic/model/imageScene/S_ForbiddenRouteSection.hpp \
    ./../../application/logic/model/core/exportResult.h \
#    ./../../application/logic/model/core/correlationScoreValue.h \
#    ./../../application/logic/model/core/enum_exportResultFileType.hpp \

SOURCES += \
    main.cpp \
    ./../../application/logic/model/ApplicationModel.cpp \
    ./../../application/logic/model/core/ComputationCore.cpp \
    ./../../application/logic/model/core/ComputationCore_inheritQATableModel.cpp \
    ./../../application/logic/model/core/ComputationCore_structures.cpp \
    ./../../application/logic/model/core/ComputationCore_inputFilesChecker.cpp \
    ./../../application/logic/model/core/S_Segment.cpp \
    ./../../application/logic/model/core/route.cpp \
    ./../../application/logic/model/core/routeContainer.cpp \
    ./../../application/logic/toolbox/toolbox_conversion.cpp \
    ./../../application/logic/toolbox/toolbox_string.cpp \
    ./../../application/logic/toolbox/toolbox_math.cpp \
    ./../../application/logic/toolbox/toolbox_math_geometry.cpp \
    ./../../application/logic/toolbox/toolbox_pathAndFile.cpp \
    ./../../application/logic/toolbox/toolbox_json.cpp \
    ./../../application/logic/io/PixelPicker.cpp \
    ./../../application/logic/model/core/valueDequantization.cpp \
    ./../../application/logic/mathComputation/meanMedian.cpp \
    ./../../application/logic/mathComputation/linearRegression.cpp \
    ./../../application/logic/model/core/IDGenerator.cpp \
    ./../../application/logic/io/InputImageFormatChecker.cpp \
    ./../../application/logic/vecttype.cpp \
    ./../../application/logic/model/core/ComputationCore_boxOrienterAndDistributer.cpp \
    ./../../application/logic/model/imageScene/S_ForbiddenRouteSection.cpp \
    ./../../application/logic/model/core/exportResult.cpp \
#    ./../../application/logic/model/core/correlationScoreValue.cpp \
#    ./../../application/logic/model/core/enum_exportResultFileType.cpp \
#    ../../../logic/mathComputation/initGSL.cpp \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
