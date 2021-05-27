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
CONFIG += release
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
#
#---------------------------


QT += core

QT += concurrent #loading tile in a separated thread

#greaterThan(QT_MAJOR_VERSION, 4):
QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
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
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/lib/libOpenImageIO.a

#openexr header installed here:
INCLUDEPATH += "/usr/local/include/"
#


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

#----------------
#GSL
#----------------
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6
# ### LP:  -lm : not needed on MacOS X; see https://www.gnu.org/software/gsl/doc/html/usage.html
# ### DYNAMIC_LIB = m
#
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6/.libs/libgsl.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6/cblas/.libs/libgslcblas.a

LIBS += -lm

#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/src


LIBS += -L$$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/buildhere/lib -lproj

CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14

#for debugging :
#QMAKE_CXXFLAGS += -lefence

DEPENDPATH += . \
              ../../application/logic \
              ../../application/ui

INCLUDEPATH += . \
               ../../application/logic \
               ../../application \
               ../../ \

SOURCES += \
    main.cpp \
\
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsSpecificItemsOfInterest2.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.cpp \
\
    ../../application/logic/model/smallImageNavScene/smallimagenav_customgraphicsscene.cpp \
\
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetPointItem_notDragrable2.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsSegmentItem2.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsBoxItem2.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetedPointItem2.cpp \
\
    ../../application/logic/model/appVersionInfos/appVersionInfos.cpp \
\
    ../../application/logic/model/core/exportResult.cpp \
    ../../application/logic/model/core/geoConvertRoute.cpp \
\
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineMouseOver.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineNoEvent.cpp \
    ../../application/logic/toolbox/toolbox_string.cpp \
\
    ../../application/logic/io/ZLIStorageContentManager.cpp \
\
    ../../application/logic/model/appRecall/SAppRecall.cpp \
    ../../application/logic/model/appRecall/appRecall.cpp \
    ../../application/logic/pathesAccess/appDataResourcesAccess.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene.cpp \
    ../../application/logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene_withExtendedXRange.cpp \
    ../../application/logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene_withExtendedXRange_sceneSpaceValuesInsteadOfPixel.cpp \
    ../../application/logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene_withExtendedYRange.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemCurve_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemEnvelop_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemXAdjustersWithComputedLine_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsLineItem.cpp \
\
    ../../application/logic/model/appSettings/SAppSettings.cpp \
    ../../application/logic/model/appSettings/appSettings.cpp \
\
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.cpp \
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.cpp \
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.cpp \
\
    ../../application/logic/model/appSettings/graphicsAppSettings/graphicsAppSettings.cpp \
\
    ../../application/logic/statusForFuturStateSwitch.cpp \
\
    ../../application/logic/model/Project_contentState.cpp \
    ../../application/logic/model/georef/geoRefImagesSetStatus.cpp \
    ../../application/logic/model/core/IDGenerator.cpp \
    ../../application/logic/model/core/S_Segment.cpp \
    ../../application/logic/toolbox/toolbox_conversion.cpp \
    ../../application/logic/toolbox/toolbox_math.cpp \
    ../../application/logic/toolbox/toolbox_math_geometry.cpp \
    ../../application/logic/model/core/BoxOrienterAndDistributer.cpp \
\
    ../../application/logic/toolbox/toolbox_json.cpp \
    ../../application/logic/toolbox/toolbox_pathAndFile.cpp \
    ../../application/logic/model/geoUiInfos/GeoUiInfos.cpp \
\
    ../../application/logic/model/AppDiskPathCacheStorage/AppDiskPathCacheStorage.cpp \
\
    ../../application/logic/model/georef/georefModel_imagesSet.cpp \
\
    ../../application/logic/mathComputation/initGSL.cpp \
    ../../application/logic/mathComputation/meanMedian.cpp \
    ../../application/logic/mathComputation/linearRegression.cpp \
    ../../application/logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.cpp \
\
    ../../application/logic/model/core/InputFilesCheckerForComputation.cpp \
    ../../application/logic/io/PixelPicker.cpp \
\
    ../../application/logic/model/core/ComputationCore.cpp \
    ../../application/logic/model/core/ComputationCore_inheritQATableModel.cpp \
    ../../application/logic/model/core/ComputationCore_structures.cpp \
    ../../application/logic/model/core/valueDequantization.cpp \
\
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsVectorDirectionAtPointItem.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetedPointItem.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/GraphicsItemsContainer.cpp \
\
    ../../application/logic/model/ApplicationModel.cpp \
    ../../application/logic/model/ApplicationModel_routesetPart.cpp \
    ../../application/logic/model/ApplicationModel_projectPart.cpp \
\
    ../../application/logic/model/MenuActionStates.cpp \
    ../../application/logic/model/MenuStates.cpp \
    ../../application/logic/model/mainAppState.cpp \
    ../../application/logic/model/StackedProfilInEdition.cpp \
\
    ../../application/logic/model/core/route.cpp \
    ../../application/logic/model/core/routeContainer.cpp \
\
    ../../application/logic/io/ZoomOutLevelImageBuilder.cpp \
    ../../application/logic/io/iojsoninfos.cpp \
    ../../application/logic/model/imageScene/customgraphicsscene_usingTPDO.cpp \ #_withLoadByThread
\
    ../../application/logic/model/tilesProvider/tileprovider_3.cpp \
    ../../application/logic/model/tilesProvider/tileprovider_withDisplayOutput.cpp \
    ../../application/logic/io/InputImageFormatChecker.cpp \
    ../../application/logic/zoomLevelImage/zoomHandler.cpp \
    ../../application/logic/vecttype.cpp \
\
    ../../application/logic/model/imageScene/customGraphicsItems/GraphicsRouteItem.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsSquareItem.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsRectangularHighlightItem.cpp \
    ../../application/logic/model/imageScene/customGraphicsItems/GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd.cpp \
\
    ../../application/logic/model/imageScene/S_ForbiddenRouteSection.cpp \
\
    ../../application/logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements.cpp \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItem_coloredGripSquare.cpp  \
    ../../application/logic/model/dialogProfilsCurves/landmarks.cpp \
\
    ../../application/logic/georef/ImageGeoRef.cpp \
    ../../application/logic/georef/Proj6_crs_to_crs.cpp \
    ../../application/logic/georef/WorldFile_parameters.cpp \
\
    ../../application/logic/OsPlatform/standardPathLocation.cpp \
    ../../application/logic/model/AppFile.cpp \
\
    ../../application/logic/pathesAccess/InternalDataPathAccess.cpp \
\
    ../../application/logic/model/geoCRSSelect/geoCRSSelect_inheritQATableModel.cpp



HEADERS += \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsSpecificItemsOfInterest2.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.h \
\
    ../../application/logic/model/smallImageNavScene/smallimagenav_customgraphicsscene.h \
\
    ../../application/ui/wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.h \
\
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetPointItem_notDragrable2.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsSegmentItem2.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsBoxItem2.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetedPointItem2.h \
\
    ../../application/logic/model/appVersionInfos/appVersionInfos.hpp \
\
    ../../application/logic/georef/geo_EPSGCodeData_struct.h \
    ../../application/logic/model/core/exportResult.h \
    ../../application/logic/model/core/geoConvertRoute.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineMouseOver.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineNoEvent.h \
    ../../application/logic/toolbox/toolbox_string.h \
\
    ../../application/logic/io/ZLIStorageContentManager.h \
\
\
    ../../application/logic/model/appRecall/SAppRecall.h \
    ../../application/logic/model/appRecall/appRecall.h \
    ../../application/logic/pathesAccess/appDataResourcesAccess.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemCurve_withAdjustableWithElements.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemEnvelop_withAdjustableWithElements.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemXAdjustersWithComputedLine_withAdjustableWithElements.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsLineItem.h \
\
    ../../application/logic/model/dialogProfilsCurves/enum_profiladjustmode.h \
\
    ../../application/logic/model/core/enum_automaticDistributionMethod.h \
    ../../application/logic/model/imageScene/S_ForbiddenRouteSection.h \
\
    ../../application/logic/model/appSettings/SAppSettings.h \
    ../../application/logic/model/appSettings/appSettings.h \
\
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h \
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h \
    ../../application/logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h \
\
    ../../application/logic/model/appSettings/graphicsAppSettings/graphicsAppSettings.h \
\
    ../../application/logic/statusForFuturStateSwitch.h \
\
    ../../application/logic/model/Project_contentState.h \
    ../../application/logic/model/core/IDGenerator.h \
    ../../application/logic/model/core/S_Segment.h \
    ../../application/logic/model/georef/geoRefImagesSetStatus.hpp \
    ../../application/logic/toolbox/toolbox_conversion.h \
    ../../application/logic/toolbox/toolbox_math.h \
    ../../application/logic/toolbox/toolbox_math_geometry.h \
    ../../application/logic/model/core/BoxOrienterAndDistributer.h \
\
    ../../application/logic/toolbox/toolbox_json.h \
    ../../application/logic/toolbox/toolbox_pathAndFile.h \
\
    ../../application/logic/model/geoUiInfos/GeoUiInfos.hpp \
\
    ../../application/logic/model/AppDiskPathCacheStorage/AppDiskPathCacheStorage.hpp \
    ../../application/logic/model/georef/georefModel_imagesSet.hpp \
\
    ../../application/logic/mathComputation/initGSL.h \
    ../../application/logic/mathComputation/meanMedian.h \
    ../../application/logic/mathComputation/linearRegression.h \
\
    ../../application/logic/model/core/leftRight_sideAccess.hpp \
    ../../application/logic/model/dialogProfilsCurves/ConverterPixelToQsc.h \
    ../../application/logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp \
\
    ../../application/logic/model/core/InputFilesCheckerForComputation.h \
\
    ../../application/logic/model/core/ComputationCore.h \
    ../../application/logic/model/core/ComputationCore_structures.h \
    ../../application/logic/model/core/valueDequantization.hpp \
    ../../application/logic/model/core/ComputationCore_inheritQATableModel.h \
\
    ../../application/logic/io/PixelPicker.h \
\
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsVectorDirectionAtPointItem.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetedPointItem.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsRectangularHighlightItem.h \
    ../../application/logic/model/imageScene/customGraphicsItems/GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd.h \
\
    ../../application/logic/model/ApplicationModel.h \
    ../../application/logic/model/MenuActionStates.hpp \
    ../../application/logic/model/MenuStates.hpp \
    ../../application/logic/model/mainAppState.hpp \
    ../../application/logic/model/StackedProfilInEdition.h \
\
    ../../application/logic/model/core/route.h \
    ../../application/logic/model/core/routeContainer.h \
\
    ../../application/logic/model/imageScene/S_booleanKeyStatus.hpp \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsItemTypes.h \
    ../../application/logic/model/imageScene/customGraphicsItems/GraphicsItemsContainer.h \
\
    ../../application/logic/io/ZoomOutLevelImageBuilder.h \
    ../../application/logic/io/iojsoninfos.h \
    ../../application/logic/model/imageScene/customgraphicsscene_usingTPDO.h \ #_withLoadByThread
    ../../application/logic/model/tilesProvider/tileprovider_3.h \
    ../../application/logic/model/tilesProvider/tileprovider_withDisplayOutput.h \
    ../../application/logic/io/InputImageFormatChecker.h \
    ../../application/logic/zoomLevelImage/zoomHandler.h \
    ../../application/logic/vecttype.h \
\
    ../../application/logic/model/imageScene/customGraphicsItems/GraphicsRouteItem.h \
    ../../application/logic/model/imageScene/customGraphicsItems/CustomGraphicsSquareItem.h \
\
    ../../application/logic/model/dialogProfilsCurves/CustomGraphicsScene_profil.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItem_coloredGripSquare.h \
    ../../application/logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemType_v2.h \
    ../../application/logic/model/dialogProfilsCurves/landmarks.h \
\
    ../../application/logic/georef/geo_basic_struct.h \
    ../../application/logic/georef/ImageGeoRef.h \
    ../../application/logic/georef/Proj6_crs_to_crs.h \
    ../../application/logic/georef/WorldFile_parameters.h \
\
    ../../application/ui/ui_enums.hpp \
    ../../application/logic/OsPlatform/standardPathLocation.hpp \
    ../../application/logic/model/AppFile.hpp \
\
    ../../application/logic/pathesAccess/InternalDataPathAccess.h \
\
    ../../application/logic/model/geoCRSSelect/geoCRSSelect_inheritQATableModel.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
