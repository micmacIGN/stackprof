#including common variable definition, THIRD_PARTY_LIBS_ROOT typically
include ../../Makefile_commonDef

CXXFLAGS = -std=c++14

ROOTPATH_OIIO=$(THIRD_PARTY_LIBS_ROOT)/oiio-RB-2.1

INCLUDE_OIIO= \
-I$(ROOTPATH_OIIO)/src/include \
-I$(ROOTPATH_OIIO)/buildhere/include \
-I$(ROOTPATH_OIIO)/ext/fmt/include

STATIC_LIB_OIIO=$(ROOTPATH_OIIO)/buildhere/lib/libOpenImageIO.a

ROOTPATH_OPENEXR=$(THIRD_PARTY_LIBS_ROOT)/openexr-2.4.0

LIBPATH_OPENEXR=$(ROOTPATH_OPENEXR)/buildhere

STATIC_LIB_OPENEXR=\
 $(LIBPATH_OPENEXR)/IlmBase/Half/libHalf-2_4.a \
 $(LIBPATH_OPENEXR)/OpenEXR/IlmImfUtil/libIlmImfUtil-2_4.a \
 $(LIBPATH_OPENEXR)/OpenEXR/IlmImf/libIlmImf-2_4.a \
 $(LIBPATH_OPENEXR)/IlmBase/IlmThread/libIlmThread-2_4.a \
 $(LIBPATH_OPENEXR)/IlmBase/Iex/libIex-2_4.a \
 $(LIBPATH_OPENEXR)/IlmBase/Imath/libImath-2_4.a \
 $(LIBPATH_OPENEXR)/IlmBase/IexMath/libIexMath-2_4.a

ROOTPATH_BOOST=$(THIRD_PARTY_LIBS_ROOT)/boost170installhere

LIBPATH_BOOST=$(ROOTPATH_BOOST)/lib

STATIC_LIB_BOOST=\
 $(LIBPATH_BOOST)/libboost_filesystem.a \
 $(LIBPATH_BOOST)/libboost_system.a \
 $(LIBPATH_BOOST)/libboost_thread.a \
 $(LIBPATH_BOOST)/libboost_chrono.a \
 $(LIBPATH_BOOST)/libboost_date_time.a \
 $(LIBPATH_BOOST)/libboost_atomic.a

SRCPATH=./


STATIC_LIB_PNG=$(THIRD_PARTY_LIBS_ROOT)/libpng-1.6.37/.libs/libpng16.a

STATIC_LIB_JPEG=$(THIRD_PARTY_LIBS_ROOT)/jpeg-8b/.libs/libjpeg.a

DYNAMIC_LIB_TIFF=-ltiff

FOR_LINKING=\
$(STATIC_LIB_OIIO) \
$(STATIC_LIB_OPENEXR) \
$(STATIC_LIB_BOOST) \
$(STATIC_LIB_PNG) $(STATIC_LIB_JPEG) \
$(DYNAMIC_LIB_TIFF) \
-lpthread -lz -ldl

target:
	g++ $(CXXFLAGS) $(INCLUDE_OIIO) \
-o makeTestImageStackProfil_withrandom_usingCache \
$(SRCPATH)/makeTestImageStackProfil_withrandom_usingCache.cpp \
$(FOR_LINKING)

clean:
#	rm -f output/*.tiff
	rm -f makeTestImageStackProfil_withrandom_usingCache
