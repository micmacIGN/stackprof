
# Adjust UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT or MACOS_INPRO__THIRD_PARTY_LIBS_ROOT
# to target the third party libs root directory for the OS target

UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT = /home/laurent/work/thirdpartylibs

MACOS_INPRO__THIRD_PARTY_LIBS_ROOT = /Users/laurent/testcompil

# INPRO__THIRD_PARTY_LIBS_ROOT is used in .pro qt project files

unix:!macx {
    INPRO__THIRD_PARTY_LIBS_ROOT = $$UBUNTU_INPRO__THIRD_PARTY_LIBS_ROOT
}
macx: {
    INPRO__THIRD_PARTY_LIBS_ROOT = $$MACOS_INPRO__THIRD_PARTY_LIBS_ROOT
}

message("INPRO__THIRD_PARTY_LIBS_ROOT in thirdpartylibs.pri: " $$INPRO__THIRD_PARTY_LIBS_ROOT)
