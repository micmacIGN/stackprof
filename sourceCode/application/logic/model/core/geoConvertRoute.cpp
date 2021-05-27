#include <QDebug>

#include "routeContainer.h"
#include "../georef/georefModel_imagesSet.hpp"

#include "geoConvertRoute.h"
#include "../../pathesAccess/InternalDataPathAccess.h"
#include "../../toolbox/toolbox_math.h"
#include "../../toolbox/toolbox_math_geometry.h"

S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute::S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute():
    _bConversionStatus(false),
    _bError_routeContainerAndIdxSelectedRouteCleared(false),
    _bRejected_routeOusideTheImage(false) {

}

bool geoConvertSelectedRouteFromRouteset_andKeepAlone(
    RouteContainer &routeContainer,
    int selectedRouteId_forRouteImport,
    GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet,
    GeoRefModel_imagesSet &forRouteImport_geoRefModelImgSetAboutRouteset,
    S_boolAndStrMsgStatus_aboutGeoConvertAndImportRoute &sbaStrMsgStatus_aboutGeoConvertAndImportRoute) {

    sbaStrMsgStatus_aboutGeoConvertAndImportRoute = {};
    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage = "Trace importation";
    qDebug() << __FUNCTION__;

    if (!ptrGeoRefModel_imagesSet) {
        qDebug() << __FUNCTION__ << ": error: _ptrGeoRefModel_imagesSet is nullptr";
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "Dev Error #11";
        return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
    }

    qDebug() << __FUNCTION__ << "_routeContainer.showContent(): >>>";
    routeContainer.showContent();
    qDebug() << __FUNCTION__ << "<<< _routeContainer.showContent()";

    if (!routeContainer.checkRouteIdExists(selectedRouteId_forRouteImport)) {
        qDebug() << __FUNCTION__ << ": error: _selectedRouteId_forRouteImport:" << selectedRouteId_forRouteImport << "does not exist in _routeContainer";
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "Dev Error #12";
        return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
    }

    forRouteImport_geoRefModelImgSetAboutRouteset.showContent();

    bool bTFWData_areEqual = forRouteImport_geoRefModelImgSetAboutRouteset.compareWorldFileDataWith(*ptrGeoRefModel_imagesSet);
    bool EPSGCode_areEqual = (0 == forRouteImport_geoRefModelImgSetAboutRouteset.get_qstrEPSGCode().compare(ptrGeoRefModel_imagesSet->get_qstrEPSGCode()));

    qDebug() << __FUNCTION__ << "bTFWData_areEqual = " << bTFWData_areEqual;
    qDebug() << __FUNCTION__ << "EPSGCode_areEqual = " << EPSGCode_areEqual;

    qDebug() << __FUNCTION__ << "selectedRouteId_forRouteImport = " << selectedRouteId_forRouteImport;

    //simplest case: none conversion required
    if (bTFWData_areEqual && EPSGCode_areEqual) {

        qDebug() << __FUNCTION__ << "(bTFWData_areEqual && EPSGCode_areEqual)";

        //just remove all the routes except the one the user want to import
        //int routeCount = routeContainer.routeCount();
        bool bRemoveAllRouteExceptOne = routeContainer.removeAllRouteExceptOne(selectedRouteId_forRouteImport);
        if (!bRemoveAllRouteExceptOne) {
            qDebug() << __FUNCTION__ << ": error: bRemoveAllRouteExceptOne selectedRouteId_forRouteImport:" << selectedRouteId_forRouteImport;
            selectedRouteId_forRouteImport = -1;
            routeContainer.clear();

            sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;
            sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "removing all traces except one #" + QString::number(selectedRouteId_forRouteImport);//(extended details)
            return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
        }

        qDebug() << __FUNCTION__ << "end of removing all except the route to import , _routeContainer.routeCount() = " << routeContainer.routeCount();

        //add a check about route location not in the image
        QSizeF qcsImageSizeF {.0, .0};
        bool bGot_qcsImageSizeF = ptrGeoRefModel_imagesSet->getImageSizeF(qcsImageSizeF);
        if (bGot_qcsImageSizeF) {

            Route& routeRef_toImport = routeContainer.getRouteRef(0);
            if (routeRef_toImport.getId() == -1) {
                selectedRouteId_forRouteImport = -1;
                routeContainer.clear();

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "failed getting trace #0";
                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }

            QVector<QPointF> vectpoint_ofRoute_converted;

            int pointCountOfRouteToImport = routeRef_toImport.pointCount();
            for (int iPoint = 0; iPoint < pointCountOfRouteToImport; iPoint++) {
                vectpoint_ofRoute_converted.push_back(routeRef_toImport.getPoint(iPoint));
            }

            bool bIntersectImage = routeBoundingBoxIntersectWithImage_pixelImageUnit(
                {.0,.0}, qcsImageSizeF, vectpoint_ofRoute_converted);

            if (!bIntersectImage) {

                qDebug() << __FUNCTION__ << "The selected trace for importation does not intersect with the project image(s)";

                //stay on the same _selectedRouteId_forRouteImport
                //and let the _routeContainer with all routes, for possible new selection by the user;

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "the selected trace is outside of the project image(s) area";
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bRejected_routeOusideTheImage = true;

                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }
        }
        sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus = true;
        return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);

    } else {

        //bTFWData_areEqual = false;
        //EPSGCode_areEqual = false;


        const Route& routeRef_toConvert = routeContainer.getRouteRef(selectedRouteId_forRouteImport);
        const QVector<QPointF>& vectpoint_ofRoute = routeRef_toConvert.getCstRefVectPoint();

        QString strRouteNameToKeep = routeRef_toConvert.getName();

        QVector<QPointF> vectpoint_ofRoute_converted;

        //world file data differents but same EPSG code
        if (  (!bTFWData_areEqual)
            &&(EPSGCode_areEqual)) {

            qDebug() << __FUNCTION__  << "(!bTFWData_areEqual)&&(EPSGCode_areEqual)";

            //convert qcs to qcs using World file data src and dest

            bool bErrorOccured = false;

            for (auto pointIter: vectpoint_ofRoute) {

                S_xy qcs_src {pointIter.x(), pointIter.y()};
                S_xy geoXY_src {.0,.0};
                bool bConvertFromRouteImportJsonQcs_toXY_srcGeoRef =
                        forRouteImport_geoRefModelImgSetAboutRouteset.convertXY_fromQcs_toWF(qcs_src, geoXY_src);
                if (!bConvertFromRouteImportJsonQcs_toXY_srcGeoRef) { //should never happen
                    bErrorOccured= true;
                    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "converting source XYs from computer image coordinates to GeoXYs";
                    break;
                }

                S_xy qcs_dst {.0,.0};
                bool bConvertFromRoute_geoXY_to_dstQcs =
                        ptrGeoRefModel_imagesSet->convertXY_fromWF_toQcs(geoXY_src, qcs_dst);
                if (!bConvertFromRoute_geoXY_to_dstQcs) { //should never happen
                    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "converting source GeoXYs to target image coordinates";
                    bErrorOccured= true;
                    break;
                }

                qDebug() << "qcs_src   = " << qcs_src._x << ", " << qcs_src._y;
                qDebug() << "geoXY_src = " << geoXY_src._x << ", " << geoXY_src._y;
                qDebug() << "qcs_dst   = " << qcs_dst._x << ", " << qcs_dst._y;

                vectpoint_ofRoute_converted.push_back({qcs_dst._x, qcs_dst._y});
            }

            if (bErrorOccured) {
                selectedRouteId_forRouteImport = -1;
                routeContainer.clear();
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;
                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }
        }

        //EPSG Codes are different
        if (!EPSGCode_areEqual) {

            qDebug() << __FUNCTION__  << "(!EPSGCode_areEqual)";

            //convert using EPSG projection from src to dest

            const QString strPathProjDB = getProjDBPath();

            bool bReport_geoRefModelImage_forRouteImport_createContext = forRouteImport_geoRefModelImgSetAboutRouteset.createProjContext(strPathProjDB);
            if (!bReport_geoRefModelImage_forRouteImport_createContext) {
                qDebug() << __FUNCTION__ << ": error forRouteImport: creating context for Proj; PathProjDB used: " << strPathProjDB;

                selectedRouteId_forRouteImport = -1;
                routeContainer.clear();

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "creating context for Proj; PathProjDB used: " + strPathProjDB;
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;

                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }

            bool bCreatedCrsToCrs = forRouteImport_geoRefModelImgSetAboutRouteset.
                    createCrsToCrs_to_specifiedTargetEPSGCode(ptrGeoRefModel_imagesSet->get_qstrEPSGCode());
            if (!bCreatedCrsToCrs) {
                qDebug() << __FUNCTION__ << ": error createCrsToCrs_to_specifiedTargetEPSGCode";
                selectedRouteId_forRouteImport = -1;
                routeContainer.clear();

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails =
                        "creating crs to crs. Source: " +forRouteImport_geoRefModelImgSetAboutRouteset.get_qstrEPSGCode()
                        + " => target: " + ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;

                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }

            bool bErrorOccured = false;

            for (auto pointIter: vectpoint_ofRoute) {

                S_xy qcs_src {pointIter.x(), pointIter.y()};
                S_xy geoXY_src {.0,.0};
                bool bConvertFromRouteImportJsonQcs_toXY_srcGeoRef =
                        forRouteImport_geoRefModelImgSetAboutRouteset.convertXY_fromQcs_toWF(qcs_src, geoXY_src);
                if (!bConvertFromRouteImportJsonQcs_toXY_srcGeoRef) { //should never happen
                    qDebug() << __FUNCTION__ << ": error bConvertFromRouteImportJsonQcs_toXY_srcGeoRef";
                    bErrorOccured= true;

                    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "converting source XYs from computer image coordinates to GeoXYs";

                    break;
                }

                S_xy geoXY_dst {.0,.0};
                bool bProjReport = forRouteImport_geoRefModelImgSetAboutRouteset.projXY_fromWF_toWF(geoXY_src, geoXY_dst);
                if (!bProjReport) {
                    bErrorOccured= true;
                    qDebug() << __FUNCTION__ << ": error _forRouteImport_geoRefModelImgSetAboutRouteset";
                    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "projecting source GeoXYs to target GeoXYs";
                    break;
                }

                S_xy qcs_dst {.0,.0};
                bool bConvertFromRoute_dstGeoXY_to_dstQcs =
                        ptrGeoRefModel_imagesSet->convertXY_fromWF_toQcs(geoXY_dst, qcs_dst);
                if (!bConvertFromRoute_dstGeoXY_to_dstQcs) { //should never happen
                    qDebug() << __FUNCTION__ << ": error convertXY_fromWF_toQcs";

                    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "converting target GeoXYs to target image coordinates";

                    bErrorOccured= true;
                    break;
                }

                qDebug() << "* in: qcs_src   = " << qcs_src._x   << ", " << qcs_src._y;
                qDebug() << "*     geoXY_src = " << geoXY_src._x << ", " << geoXY_src._y;
                qDebug() << "*     geoXY_dst = " << geoXY_dst._x << ", " << geoXY_dst._y;
                qDebug() << "* out: qcs_dst  = " << qcs_dst._x   << ", " << qcs_dst._y;

                vectpoint_ofRoute_converted.push_back({qcs_dst._x, qcs_dst._y});
            }

            if (bErrorOccured) {
                selectedRouteId_forRouteImport = -1;
                routeContainer.clear();

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;

                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }
        }

        //add a check about route location not in the image at the end of the conversion
        QSizeF qcsImageSizeF {.0, .0};
        bool bGot_qcsImageSizeF = ptrGeoRefModel_imagesSet->getImageSizeF(qcsImageSizeF);
        if (bGot_qcsImageSizeF) {

            bool bIntersectImage = routeBoundingBoxIntersectWithImage_pixelImageUnit(
                {.0,.0}, qcsImageSizeF, vectpoint_ofRoute_converted);

            if (!bIntersectImage) {

                qDebug() << __FUNCTION__ << "The selected trace for importation does not intersect with the project image(s)";

                //stay on the same _selectedRouteId_forRouteImport
                //and let the _routeContainer with all routes, for possible new selection by the user;

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "the selected trace is outside of the project image(s) area";
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bRejected_routeOusideTheImage = true;

                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }
        }

        routeContainer.clear();
        int addedRouteId = routeContainer.addRoute();
        if (addedRouteId == -1) {
            qDebug() << __FUNCTION__ << "error: failed to addRoute()";

            selectedRouteId_forRouteImport = -1;
            routeContainer.clear();

            sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "internal error #13";
            sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;

            return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
        }
        Route& routeRef = routeContainer.getRouteRef(addedRouteId);
        int idxPoint = 0;
        for (const auto& point: vectpoint_ofRoute_converted) {
            bool bThisIsTheFirstPointForTheRoute = false; //not used
            bool bPointAdded = routeRef.tryAddPointAtRouteEnd(point, bThisIsTheFirstPointForTheRoute);
            if (!bPointAdded) {
                qDebug() << __FUNCTION__ << "error: failed to tryAddPointAtRouteEnd for idxPoint: " << idxPoint;

                selectedRouteId_forRouteImport = -1;
                routeContainer.clear();

                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._strMessage_errorDetails = "internal error #14";
                sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bError_routeContainerAndIdxSelectedRouteCleared = true;

                return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
            }
            idxPoint++;
        }
        routeRef.setName(strRouteNameToKeep);
    }

    sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus = true;
    return(sbaStrMsgStatus_aboutGeoConvertAndImportRoute._bConversionStatus);
}

