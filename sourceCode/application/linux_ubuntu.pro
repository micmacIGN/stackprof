#-------------------------------------------------
#
# Stackprof v1.00 ; Qt project file for linux ubuntu target
#
#-------------------------------------------------

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
#QT += gui

QT += concurrent #loading tile in a separated thread

#greaterThan(QT_MAJOR_VERSION, 4):
QT += widgets


TARGET = stackprof
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


!include( thirdpartylibs.pri  ) {
    error( "No thirdpartylibs.pri file found" )
}
message("INPRO__THIRD_PARTY_LIBS_ROOT from thirdpartylibs.pri: " $$INPRO__THIRD_PARTY_LIBS_ROOT)

#OIIO
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/src/include/
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/include/
#
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/ext/fmt/include
#
# static lib linking:
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/oiio-RB-2.1/buildhere/lib/libOpenImageIO.a
#

#openexr header installed here:
INCLUDEPATH += "/usr/local/include/"


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

#static lib linking:
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_filesystem.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_system.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_thread.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_chrono.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_date_time.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/boost170installhere/lib/libboost_atomic.a

LIBS += -lpthread



LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/libpng-1.6.37/.libs/libpng16.a

# 6 is not ok, oiio needs 8 #@LP
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/jpeg-8b/.libs/libjpeg.a

#local directory vs /usr/local/lib -ltiff
LIBS +=-ltiff

#----------------
#GSL
#----------------
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6
# ### LP:  -lm : not needed on MacOS X; see https://www.gnu.org/software/gsl/doc/html/usage.html
# ### DYNAMIC_LIB = m
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6/.libs/libgsl.a
LIBS += $$INPRO__THIRD_PARTY_LIBS_ROOT/gsl-2.6/cblas/.libs/libgslcblas.a

LIBS += -lm
#----------------


#----------------
#PROJ:
#----------------
INCLUDEPATH += $$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/src

LIBS += -L$$INPRO__THIRD_PARTY_LIBS_ROOT/proj-6.3.0/buildhere/lib -lproj
#----------------

CONFIG += c++14

QMAKE_CXXFLAGS += -std=c++14

#for debugging :
##QMAKE_CXXFLAGS += -lefence

#TagToFindAboutReleaseDeliveryOption
#for release comment these lines:
#
#CONFIG+=sanitizer
#CONFIG+=sanitize_address

##out of Qt scope alloc/pointer sanitizer check:
##QMAKE_CXXFLAGS+="-fsanitize=address -fno-omit-frame-pointer"
##QMAKE_CFLAGS+="-fsanitize=address -fno-omit-frame-pointer"
##QMAKE_LFLAGS+="-fsanitize=address"


DEPENDPATH += . logic ui
INCLUDEPATH += \
    . \
    logic \
    ui \
    ui/smallImageNavigation \
    ui/dialogProfilsCurves \
    ui/dialogIOFiles \
    ui/traceEdition \
    ui/project \
    ui/georefinfos \
    ui/stackedProfilBox \
    ui/computationParameters \
    ui/micmacStepValueAndSpatialResolution \
    ui/dialog_stateSwitchConfirm \
    ui/appSettings \
    ui/windowImageView \
    ui/taskInProgress \
    ui/dialogProfilOrientation \
    ui/geoCRSSelect \
    ui/appAbout

SOURCES += \
    logic/model/core/BoxOrienterAndDistributer.cpp \
    logic/model/core/InputFilesCheckerForComputation.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsSpecificItemsOfInterest2.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.cpp \
\
    logic/model/smallImageNavScene/smallimagenav_customgraphicsscene.cpp \
\
    ui/wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.cpp \
\
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetPointItem_notDragrable2.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsSegmentItem2.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsBoxItem2.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetedPointItem2.cpp \
\
    ui/appAbout/dialogabout.cpp \
    logic/model/appVersionInfos/appVersionInfos.cpp \
\
    logic/model/core/exportResult.cpp \
    logic/model/core/geoConvertRoute.cpp \
    ui/geoCRSSelect/dialog_geoCRSSelect_tableview.cpp \
    ui/project/dialog_exportresult.cpp \
    ui/dialogProfilsCurves/dialog_profilcurvesmeanings.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineMouseOver.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineNoEvent.cpp \
    logic/toolbox/toolbox_string.cpp \
\
    ui/dialogProfilsCurves/widgetcurrentboxidedittoreach.cpp \
    logic/io/ZLIStorageContentManager.cpp \
\
    ui/appSettings/dialog_graphicsSettings.cpp \
\
    ui/dialogProfilOrientation/dialog_setprofilorientation.cpp \
    logic/model/appRecall/SAppRecall.cpp \
    logic/model/appRecall/appRecall.cpp \
    logic/pathesAccess/appDataResourcesAccess.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene.cpp \
    logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene_withExtendedXRange.cpp \
    logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene_withExtendedXRange_sceneSpaceValuesInsteadOfPixel.cpp \
    logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements_section_feedScene_withExtendedYRange.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemCurve_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemEnvelop_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemXAdjustersWithComputedLine_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsLineItem.cpp \
\
    ui/project/dialog_project_inputfiles_fixingMissingFiles.cpp \
    ui/project/dialog_selectoneoflayers.cpp \
    ui/project/dialog_selectroutefromrouteset.cpp \
    ui/appSettings/dialog_appsettings.cpp \
\
    logic/model/appSettings/SAppSettings.cpp \
    logic/model/appSettings/appSettings.cpp \
\
    logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.cpp \
    logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.cpp \
    logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.cpp \
\
    logic/model/appSettings/graphicsAppSettings/graphicsAppSettings.cpp \
\
    logic/statusForFuturStateSwitch.cpp \
\
    logic/model/Project_contentState.cpp \
    logic/model/georef/geoRefImagesSetStatus.cpp \
    logic/model/core/IDGenerator.cpp \
    ui/taskInProgress/dialog_taskinprogress.cpp \
    logic/model/core/S_Segment.cpp \
    logic/toolbox/toolbox_conversion.cpp \
    logic/toolbox/toolbox_math.cpp \
    logic/toolbox/toolbox_math_geometry.cpp \
    ui/appSettings/spinbox_withoutTextEdit.cpp \
    ui/computationParameters/dialog_computationparameters.cpp \
    ui/computationParameters/widget_correlScoreMapParameters.cpp \
    ui/dialogProfilsCurves/ComponentsSwitcher.cpp \
    ui/dialogStateSwitchConfirm/messageBoxWithVariableAnswers.cpp \
    ui/stackedProfilBox/dialog_addboxes_automaticdistribution_closeAsPossible.cpp \
    ui/stackedProfilBox/dialog_addboxes_automaticdistribution_distanceBetweenCenter.cpp \
    ui/stackedProfilBox/dialog_addboxesautomaticdistribution_selectmethod.cpp \
    ui/stackedProfilBox/dialog_stackedprofilbox_tableview.cpp \
\
    logic/toolbox/toolbox_json.cpp \
    logic/toolbox/toolbox_pathAndFile.cpp \
    ui/dialogIOFiles/dialog_replaceImageFile.cpp \
    ui/dialogIOFiles/dialog_savefile.cpp \
    logic/model/geoUiInfos/GeoUiInfos.cpp \
    ui/project/dialog_project_inputfiles.cpp \
    ui/project/dialog_project_inputfiles_viewContentOnly.cpp \
\
    ui/stackedProfilBox/dialog_stackedProfilBoxes.cpp \
    ui/stackedProfilBox/widget_stackedprofilbox_edit.cpp \
    ui/stackedProfilBox/widget_stackedprofilbox_editsize.cpp \
    ui/traceEdition/dialog_traceparameters.cpp \
    logic/model/AppDiskPathCacheStorage/AppDiskPathCacheStorage.cpp \
\
    logic/model/georef/georefModel_imagesSet.cpp \
\
    logic/mathComputation/initGSL.cpp \
    logic/mathComputation/meanMedian.cpp \
    logic/mathComputation/linearRegression.cpp \
    logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.cpp \
\
    logic/io/PixelPicker.cpp \
\
    logic/model/core/ComputationCore.cpp \
    logic/model/core/ComputationCore_inheritQATableModel.cpp \
    logic/model/core/ComputationCore_structures.cpp \
    logic/model/core/valueDequantization.cpp \
\
    ui/traceEdition/dialog_traces_edit.cpp \
    ui/smallImageNavigation/windowsmallimagenav.cpp \
    ui/smallImageNavigation/smallimagenav_customgraphicsview.cpp \
\
    logic/model/imageScene/customGraphicsItems/CustomGraphicsVectorDirectionAtPointItem.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetedPointItem.cpp \
    logic/model/imageScene/customGraphicsItems/GraphicsItemsContainer.cpp \
\
    main.cpp \
\
    logic/model/ApplicationModel.cpp \
    logic/model/ApplicationModel_routesetPart.cpp \
    logic/model/ApplicationModel_projectPart.cpp \
\
    logic/model/MenuActionStates.cpp \
    logic/model/MenuStates.cpp \
    logic/model/mainAppState.cpp \
    logic/model/StackedProfilInEdition.cpp \
\
    logic/model/core/route.cpp \
    logic/model/core/routeContainer.cpp \
\
    ui/ApplicationView.cpp \
\
    logic/controller/ApplicationController.cpp \
    logic/controller/ApplicationController_projectPart.cpp \
    logic/controller/ApplicationController_routesetPart.cpp \
\
    logic/io/ZoomOutLevelImageBuilder.cpp \
    logic/io/iojsoninfos.cpp \
    logic/model/imageScene/customgraphicsscene_usingTPDO.cpp \
    ui/windowImageView/customgraphicsview.cpp \
    logic/model/tilesProvider/tileprovider_3.cpp \
    logic/model/tilesProvider/tileprovider_withDisplayOutput.cpp \
    logic/io/InputImageFormatChecker.cpp \
    logic/zoomLevelImage/zoomHandler.cpp \
    logic/vecttype.cpp \
    ui/MainWindowMenu.cpp \
    ui/windowImageView/WidgetImageViewStatusBar.cpp \
    ui/windowImageView/WindowImageView.cpp \
    ui/windowImageView/WidgetImageViewUpBar.cpp \
\
    logic/model/imageScene/customGraphicsItems/GraphicsRouteItem.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsSquareItem.cpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsRectangularHighlightItem.cpp \
    logic/model/imageScene/customGraphicsItems/GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd.cpp \
\
    logic/model/imageScene/S_ForbiddenRouteSection.cpp \
\
    ui/dialogProfilsCurves/customgraphicsview_basic_withAdjustableWithElements.cpp \
\
    ui/dialogProfilsCurves/dialogVLayoutStackProfilEdit.cpp \
    ui/dialogProfilsCurves/stackprofiledit.cpp \
\
    logic/model/dialogProfilsCurves/CustomGraphicsScene_profil_withAdjustableWithElements.cpp \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItem_coloredGripSquare.cpp  \
    logic/model/dialogProfilsCurves/landmarks.cpp \
\
    logic/georef/ImageGeoRef.cpp \
    logic/georef/Proj6_crs_to_crs.cpp \
    logic/georef/WorldFile_parameters.cpp \
\
    ui/dialogIOFiles/widgetInputFilesImages.cpp \
    ui/dialogIOFiles/dialog_inputfiles_images.cpp\
    logic/OsPlatform/standardPathLocation.cpp \
    logic/model/AppFile.cpp \
    ui/georefinfos/widgetGeoRefInfos.cpp \
    ui/georefinfos/dialog_geoRefInfos.cpp \
\
    ui/micmacStepValueAndSpatialResolution/dialog_micmacstepvalueandspatialresolution.cpp \
    ui/micmacStepValueAndSpatialResolution/widget_micmacstepvalueandspatialresolution.cpp \
\
    ui/windowImageView/layersSwitcher.cpp \
\
    logic/pathesAccess/InternalDataPathAccess.cpp \
\
    ui/dialogIOFiles/widgetInputFilesImages_viewOnly.cpp \
    logic/model/geoCRSSelect/geoCRSSelect_inheritQATableModel.cpp

HEADERS += \
    logic/model/core/BoxOrienterAndDistributer.h \
    logic/model/core/InputFilesCheckerForComputation.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsSpecificItemsOfInterest2.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetPointItem_noMouseInteraction_triangle2.h \
\
    logic/model/smallImageNavScene/smallimagenav_customgraphicsscene.h \
\
    ui/wheelEventNoiseHandler/wheelEventZoomInOutNoiseHandler.h \
\
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetPointItem_notDragrable2.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsSegmentItem2.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsBoxItem2.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTargetedPointItem2.h \
\
    ui/appAbout/dialogabout.h \
    logic/model/appVersionInfos/appVersionInfos.hpp \
\
    logic/georef/geo_EPSGCodeData_struct.h \
    logic/model/core/exportResult.h \
    logic/model/core/geoConvertRoute.h \
    ui/geoCRSSelect/dialog_geoCRSSelect_tableview.h \
    ui/project/dialog_exportresult.h \
    ui/dialogProfilsCurves/dialog_profilcurvesmeanings.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineMouseOver.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomLineNoEvent.h \
    logic/toolbox/toolbox_string.h \
    ui/dialogProfilsCurves/widgetcurrentboxidedittoreach.h \
    logic/io/ZLIStorageContentManager.h \
\
    ui/appSettings/dialog_graphicsSettings.h \
\
    ui/dialogProfilOrientation/dialog_setprofilorientation.h \
    logic/model/appRecall/SAppRecall.h \
    logic/model/appRecall/appRecall.h \
    logic/pathesAccess/appDataResourcesAccess.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjusterWithCentereredGrip_withAdjustableWithElements.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemCurve_withAdjustableWithElements.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemEnvelop_withAdjustableWithElements.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemDoubleSidesXAdjustersWithComputedLine_withAdjustableWithElements.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemVerticalAdjuster_withAdjustableWithElements.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemXAdjustersWithComputedLine_withAdjustableWithElements.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsLineItem.h \
\
    logic/model/dialogProfilsCurves/enum_profiladjustmode.h \
    ui/project/dialog_project_inputfiles_fixingMissingFiles.h \
    ui/project/dialog_selectoneoflayers.h \
    ui/project/dialog_selectroutefromrouteset.h \
\
    logic/model/core/enum_automaticDistributionMethod.h \
    logic/model/imageScene/S_ForbiddenRouteSection.h \
    ui/appSettings/dialog_appsettings.h \
\
    logic/model/appSettings/SAppSettings.h \
    logic/model/appSettings/appSettings.h \
\
    logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_baseStruct.h \
    logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_profil.h \
    logic/model/appSettings/graphicsAppSettings/SGraphicsAppSettings_ItemOnImageView.h \
\
    logic/model/appSettings/graphicsAppSettings/graphicsAppSettings.h \
\
    logic/statusForFuturStateSwitch.h \
\
    ui/taskInProgress/dialog_taskinprogress.h \
    logic/model/Project_contentState.h \
    logic/model/core/IDGenerator.h \
    logic/model/core/S_Segment.h \
    logic/model/georef/geoRefImagesSetStatus.hpp \
    logic/toolbox/toolbox_conversion.h \
    logic/toolbox/toolbox_math.h \
    logic/toolbox/toolbox_math_geometry.h \
    ui/appSettings/spinbox_withoutTextEdit.h \
    ui/computationParameters/dialog_computationparameters.h \
    ui/computationParameters/widget_correlScoreMapParameters.h \
    ui/dialogProfilsCurves/componentsSwitcher.h \
    ui/dialogProfilsCurves/e_hexaCodeLayerEditProfil.hpp \
    ui/dialogStateSwitchConfirm/messageBoxWithVariableAnswers.h \
    ui/stackedProfilBox/dialog_addboxes_automaticdistribution_closeAsPossible.h \
    ui/stackedProfilBox/dialog_addboxes_automaticdistribution_distanceBetweenCenter.h \
    ui/stackedProfilBox/dialog_addboxesautomaticdistribution_selectmethod.h \
    ui/stackedProfilBox/dialog_stackedprofilbox_tableview.h \
\
    logic/toolbox/toolbox_json.h \
    logic/toolbox/toolbox_pathAndFile.h \
    ui/dialogIOFiles/dialog_replaceImageFile.h \
    ui/dialogIOFiles/dialog_savefile.h \
    logic/controller/AppState_enum.hpp \
    logic/model/geoUiInfos/GeoUiInfos.hpp \
    ui/project/dialog_project_inputfiles.h \
    ui/project/dialog_project_inputfiles_viewContentOnly.h \
\
    ui/stackedProfilBox/dialog_stackedProfilBoxes.h \
    ui/stackedProfilBox/widget_stackedprofilbox_edit.h \
    ui/stackedProfilBox/widget_stackedprofilbox_editsize.h \
    ui/traceEdition/dialog_traceparameters.h \
    ui/traceEdition/dialog_traces_edit.h \
    logic/model/AppDiskPathCacheStorage/AppDiskPathCacheStorage.hpp \
    logic/model/georef/georefModel_imagesSet.hpp \
\
    logic/mathComputation/initGSL.h \
    logic/mathComputation/meanMedian.h \
    logic/mathComputation/linearRegression.h \
\
    logic/model/core/leftRight_sideAccess.hpp \
    logic/model/dialogProfilsCurves/ConverterPixelToQsc.h \
    logic/model/dialogProfilsCurves/StackProfilEdit_textWidgetPartModel.hpp \
\
\
    logic/model/core/ComputationCore.h \
    logic/model/core/ComputationCore_structures.h \
    logic/model/core/valueDequantization.hpp \
    logic/model/core/ComputationCore_inheritQATableModel.h \
\
    logic/io/PixelPicker.h \
    ui/smallImageNavigation/windowsmallimagenav.h \
    ui/smallImageNavigation/smallimagenav_customgraphicsview.h \
\
    logic/model/imageScene/customGraphicsItems/CustomGraphicsVectorDirectionAtPointItem.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsTinyTargetedPointItem.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsRectangularHighlightItem.h \
    logic/model/imageScene/customGraphicsItems/GraphicsRouteItemWithForbiddenSectionsFromStartAndEnd.h \
\
    logic/model/ApplicationModel.h \
    logic/model/MenuActionStates.hpp \
    logic/model/MenuStates.hpp \
    logic/model/mainAppState.hpp \
    logic/model/StackedProfilInEdition.h \
\
    logic/model/core/route.h \
    logic/model/core/routeContainer.h \
\
    logic/model/imageScene/S_booleanKeyStatus.hpp \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsItemTypes.h \
    logic/model/imageScene/customGraphicsItems/GraphicsItemsContainer.h \
    ui/ApplicationView.h \
\
    logic/controller/ApplicationController.h \
\
    logic/io/ZoomOutLevelImageBuilder.h \
    logic/io/iojsoninfos.h \
    logic/model/imageScene/customgraphicsscene_usingTPDO.h \
    ui/windowImageView/customgraphicsview.h \
    logic/model/tilesProvider/tileprovider_3.h \
    logic/model/tilesProvider/tileprovider_withDisplayOutput.h \
    logic/io/InputImageFormatChecker.h \
    logic/zoomLevelImage/zoomHandler.h \
    logic/vecttype.h \
    ui/MainWindowMenu.h \
    ui/windowImageView/WidgetImageViewStatusBar.h \
    ui/windowImageView/WindowImageView.h \
    ui/windowImageView/WidgetImageViewUpBar.h \
\
    logic/model/imageScene/customGraphicsItems/GraphicsRouteItem.h \
    logic/model/imageScene/customGraphicsItems/CustomGraphicsSquareItem.h \
\
    ui/dialogProfilsCurves/customgraphicsview_basic.h \
    ui/dialogProfilsCurves/dialogVLayoutStackProfilEdit.h \
    ui/dialogProfilsCurves/stackprofiledit.h \
\
    logic/model/dialogProfilsCurves/CustomGraphicsScene_profil.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItem_coloredGripSquare.h \
    logic/model/dialogProfilsCurves/customGraphicsItems/graphicProfil/CustomGraphicsItemType_v2.h \
    logic/model/dialogProfilsCurves/landmarks.h \
\
    logic/georef/geo_basic_struct.h \
    logic/georef/ImageGeoRef.h \
    logic/georef/Proj6_crs_to_crs.h \
    logic/georef/WorldFile_parameters.h \
\
    ui/dialogIOFiles/widgetInputFilesImages.h \
    ui/dialogIOFiles/dialog_inputfiles_images.h \
    ui/ui_enums.hpp \
    logic/OsPlatform/standardPathLocation.hpp \
    logic/model/AppFile.hpp \
    ui/georefinfos/widgetGeoRefInfos.h \
    ui/georefinfos/dialog_geoRefInfos.h \
\
    ui/micmacStepValueAndSpatialResolution/dialog_micmacstepvalueandspatialresolution.h \
    ui/micmacStepValueAndSpatialResolution/widget_micmacstepvalueandspatialresolution.h \
\
    ui/windowImageView/layersSwitcher.h \
\
    logic/pathesAccess/InternalDataPathAccess.h \
\
    ui/dialogIOFiles/widgetInputFilesImages_viewOnly.h \
    logic/model/geoCRSSelect/geoCRSSelect_inheritQATableModel.h

FORMS += \
\
    dotui/appAbout/dialogabout.ui \
    dotui/appSettings/dialog_appsettings.ui \
    dotui/appSettings/dialog_graphicsSettings.ui \
\
    dotui/dialogProfilOrientation/dialog_setprofilorientation.ui \
\
    dotui/dialogProfilsCurves/componentsSwitcher.ui \
    dotui/dialogProfilsCurves/dialog_profilcurvesmeanings.ui \
    dotui/dialogProfilsCurves/widgetcurrentboxidedittoreach.ui \
    dotui/geoCRSSelect/dialog_geoCRSSelect_tableview.ui \
    dotui/micmacStepValueAndSpatialResolution/dialog_micmacstepvalueandspatialresolution.ui \
    dotui/micmacStepValueAndSpatialResolution/widget_micmacstepvalueandspatialresolution.ui \
\
    dotui/computationParameters/widget_correlScoreMapParameters.ui \
    dotui/project/dialog_exportresult.ui \
    dotui/project/dialog_project_inputfiles_fixingMissingFiles.ui \
    dotui/project/dialog_selectroutefromrouteset.ui \
    dotui/project/dialog_selectoneoflayers.ui \
    dotui/taskInProgress/dialog_taskinprogress.ui \
\
    dotui/dialogIOFiles/dialog_replacefile.ui \
    dotui/mainwindowMenu.ui \
\
    dotui/dialogIOFiles/dialog_inputfiles_images.ui \
    dotui/dialogIOFiles/widget_inputFiles_images.ui \
    dotui/dialogIOFiles/dialog_savefile.ui \
\
    dotui/georefinfos/dialog_georefinfos.ui \
\
    dotui/stackedProfilBox/dialog_addboxes_automaticdistribution_closeAsPossible.ui \
    dotui/stackedProfilBox/dialog_addboxes_automaticdistribution_distanceBetweenCenter.ui \
    dotui/traceEdition/dialog_traceparameters.ui \
    dotui/traceEdition/dialog_traces_edit.ui \
\
    dotui/stackedProfilBox/dialog_stackedProfilBoxes.ui \
    dotui/stackedProfilBox/widget_stackedprofilbox_edit.ui \
    dotui/stackedProfilBox/widget_stackedprofilbox_editsize.ui \
    dotui/stackedProfilBox/dialog_stackedprofilbox_tableview.ui \
    dotui/georefinfos/widgetgeorefinfos.ui \
    dotui/windowImageView/widgetImageviewStatusBar.ui \
    dotui/windowImageView/widgetImageviewUpBar.ui \
    dotui/windowImageView/windowimageview.ui \
    dotui/windowImageView/layersSwitcher.ui \
\
    dotui/windowsmallimagenav/windowsmallimagenav.ui \
\
    dotui/dialogProfilsCurves/stackprofiledit.ui \
    dotui/dialogProfilsCurves/dialogVLayoutStackProfilEdit.ui \
\
    dotui/project/dialog_project_inputfiles.ui \
    dotui/project/dialog_project_inputfiles_viewContentOnly.ui \
    dotui/dialogIOFiles/widget_inputFiles_images_viewOnly.ui \
\
    dotui/computationParameters/dialog_computationparameters.ui \
    dotui/stackedProfilBox/dialog_addboxesautomaticdistribution_selectmethod.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
