
# for the different tests involving Qt, INPRO__THIRD_PARTY_LIBS_ROOT is set by default from ../application/thirdpartylibs.pri
# but you can set to a different value if you need for tests purpose, defining INPRO__THIRD_PARTY_LIBS_ROOT here instead

#default setting from ../application/thirdpartylibs.pri:

#include( ../application/thirdpartylibs.pri )
!include( ../application/thirdpartylibs.pri  ) {
    error( "No ../application/thirdpartylibs.pri file found" )
}

