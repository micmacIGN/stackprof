#include <QCoreApplication>
#include <QDir>

#include <QDebug>

#include "../../application/logic/model/georef/georefModel_imagesSet.hpp"

//test from qcs => to geoXY crs 3066  => to lon,lat crs 4326:
bool test_convertGeoPoint_1();

//test from qcs => to geoXY crs 3066  => to geoXY crs 2039 => to lon,lat 4326
//test from qcs => to geoXY crs 3066  ======================> to lon,lat 4326
//and compare the lon,lat couple to see if similar
//(@LP: test using the same image and same tfw data)
bool test_convertGeoPoint_2();




//test from qcs => to geoXY crs 4267  => to geoXY crs 26716 => to lon,lat 4326 + to Qcs in dst image
//test from qcs => to geoXY crs 4267  =======================> to lon,lat 4326
//
//compare the lon,lat couple to see if similar
//compare the pixel location and pixel content at the Qcs in dst image
//
//(@LP: tfw data for two images are different)
bool test_convertGeoPoint_3();




//test from qcs => to geoXY crs 26771 => to 4326 => to geoXY crs 26716 => to Qcs in dst image + lon, lat
//                                  \=> to lon,lat
//compare lon, lat src dst
//compare the src dst pixel location and pixel content at src and dst
//
//(@LP: tfw data for two images are different)
bool test_convertGeoPoint_4();

//same EPSG
//tfw data different
bool test_convertGeoPoint_sameEPSGCode();

//#TagToFindAboutReleaseDeliveryOption
//#define DEF_APP_DELIVERY_RELEASE

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stdout, "Dbg: %s\n", localMsg.constData());
        //fprintf(stdout, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        fprintf(stdout, "Inf: %s\n", localMsg.constData());
        //fprintf(stdout, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stdout, "warning: %s\n", localMsg.constData());
        //fprintf(stdout, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stdout, "critical: %s\n", localMsg.constData());
        //fprintf(stdout, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stdout, "fatal: %s\n", localMsg.constData());
        //fprintf(stdout, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char *argv[]) {

#ifndef DEF_APP_DELIVERY_RELEASE
    qInstallMessageHandler(myMessageOutput); // Install the handler
#endif

    //test_convertGeoPoint_1();
    //test_convertGeoPoint_2();

    test_convertGeoPoint_3();

    //test_convertGeoPoint_4();
    //test_convertGeoPoint_sameEPSGCode();

    return(0);
}

//test from qcs => to geoXY crs 3066  => to lon,lat crs 4326:
bool test_convertGeoPoint_1() {

    bool bSomeLoadedButNoUsable = false;
    //EPSG:3066 ED50 / Jordan TM
    GeoRefModel_imagesSet GeoRefModel_imagesSet_src;
    bool bsrc_setOK = GeoRefModel_imagesSet_src.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        {"/home/laurent/04fev2021root/testconv/landsat8/Landsat_8_Aqaba_15m.tif"},
        bSomeLoadedButNoUsable);

    if (!bsrc_setOK) {
        qDebug() << __FUNCTION__ << ": error: in setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    bsrc_setOK = GeoRefModel_imagesSet_src.setEPSGCode_and_storeInCacheForAllImagesSet("3066");
    if (!bsrc_setOK) {
        qDebug() << __FUNCTION__ << ": error: in setEPSGCode_and_storeInCacheForAllImagesSet";
        return(false);
    }

    bsrc_setOK = GeoRefModel_imagesSet_src.check_TFWDataSet();
    if (!bsrc_setOK) {
        qDebug() << __FUNCTION__ << ": error: in check_TFWDataSet";
        return(false);
    }

    GeoRefModel_imagesSet_src.showContent();


    //convert using EPSG projection from src to dest

    //#LP move to a dedicated app-sys-config class
    QString qstrApplicationDirPath = "."; //QCoreApplication::applicationDirPath();
    const QString strPathProjDB = qstrApplicationDirPath + QDir::separator() + "projdb" +  QDir::separator();

    qDebug() << __FUNCTION__ << "strPathProjDB = " << strPathProjDB;

    bool bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    bool bCreatedCrsToCrs  = GeoRefModel_imagesSet_src.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");
    if (!bCreatedCrsToCrs) {
        qDebug() << __FUNCTION__ << ": error: createCrsToCrs_to_specifiedTargetEPSGCode";
        return(false);
    }

    bool bErrorOccured = false;

    QVector<QPointF> vectpoint_ofRoute_srcPoint;

    vectpoint_ofRoute_srcPoint.push_back({0.0, 0.0});
    //vectpoint_ofRoute_srcPoint.push_back({100.0, 200.0});
    //vectpoint_ofRoute_srcPoint.push_back({300.0, 400.0});

    QVector<QPointF> vectpoint_ofRoute_converted;

    for (auto pointIter: vectpoint_ofRoute_srcPoint) {

        qDebug();
        qDebug() << "******";
        qDebug() << "* in: qcsXY_src pointIter = " << pointIter;
        qDebug() << "******";

        //test from qcs => to geoXY crs 3066  => to lon,lat crs 4326:

        S_xy qcsXY_src {pointIter.x(),pointIter.y()};
        S_xy geoXY_src {.0,.0};
        qDebug() << "src.convertXY_from_Qcs_to_WF:";
        bool bConvertFromRouteImportJsonQcs_toXY_srcGeoRef =
                GeoRefModel_imagesSet_src.convertXY_fromQcs_toWF(qcsXY_src, geoXY_src);
        if (!bConvertFromRouteImportJsonQcs_toXY_srcGeoRef) { //should never happen
            qDebug() << __FUNCTION__ << ": error: convertXY_fromQcs_toWF";
            bErrorOccured= true;
            break;
        }

        //S_xy geoXY_dst {.0,.0};
        S_lonlat lonlat_src {.0,.0};
        bool bProjReport = GeoRefModel_imagesSet_src.projXY_fromWF_toLonLat(geoXY_src, lonlat_src);
        if (!bProjReport) {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toLonLat";
            bErrorOccured= true;
            break;
        }
        qDebug() << "  lonlat_src = " << lonlat_src._lon << ", " << lonlat_src._lat;

        S_lonlat directFromQcs_lonlat_src {.0,.0};
        bProjReport = GeoRefModel_imagesSet_src.projXY_fromQcs_toLonLat(pointIter, directFromQcs_lonlat_src);
        if (!bProjReport) {
            qDebug() << __FUNCTION__ << ": error: projXY_fromQcs_toLonLat";
            bErrorOccured= true;
            break;
        }
        qDebug() << "  directFromQcs_lonlat_src = " << directFromQcs_lonlat_src._lon << ", " << directFromQcs_lonlat_src._lat;

        QString strLatLon;
        bProjReport = GeoRefModel_imagesSet_src.projXY_fromQcs_toStrLatLon(pointIter, strLatLon);
        if (!bProjReport) {
            qDebug() << __FUNCTION__ << ": error: projXY_fromQcs_toStrLatLon";
            bErrorOccured= true;
            break;
        }
        qDebug() << "  strLatLon = " << strLatLon;
    }

    return(!bErrorOccured);
}


//test from qcs => to geoXY crs 3066  => to geoXY crs 2039 => to lon,lat 4326
//test from qcs => to geoXY crs 3066  ======================> to lon,lat 4326
//and compare the lon,lat couple to see if similar
//(@LP: test using the same image and same tfw data)
bool test_convertGeoPoint_2() {

    bool bSomeLoadedButNoUsable = false;
    //EPSG:3066 ED50 / Jordan TM
    GeoRefModel_imagesSet GeoRefModel_imagesSet_src;
    bool bsrc_setOK = GeoRefModel_imagesSet_src.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        {"/home/laurent/04fev2021root/testconv/landsat8/Landsat_8_Aqaba_15m.tif"},
        bSomeLoadedButNoUsable);
    GeoRefModel_imagesSet_src.setEPSGCode_and_storeInCacheForAllImagesSet("3066");
    if (!bsrc_setOK) {
        qDebug() << __FUNCTION__ << ": error: in src setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    //EPSG:2039 Israel 1993 / Israeli TM Grid
    GeoRefModel_imagesSet GeoRefModel_imagesSet_dst;
    bool bdst_setOK = GeoRefModel_imagesSet_dst.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
        {"/home/laurent/04fev2021root/testconv/landsat8/Landsat_8_Aqaba_15m.tif"},
        bSomeLoadedButNoUsable);//{"/home/laurent/testconv/landsat8/Landsat_8_Tiran_15m_Lat_Longv2.tif"});
    GeoRefModel_imagesSet_dst.setEPSGCode_and_storeInCacheForAllImagesSet("2039");    
    if (!bdst_setOK) {
        qDebug() << __FUNCTION__ << ": error: in dst setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    bsrc_setOK &= GeoRefModel_imagesSet_src.check_TFWDataSet();
    bdst_setOK &= GeoRefModel_imagesSet_dst.check_TFWDataSet();

    GeoRefModel_imagesSet_src.showContent();
    GeoRefModel_imagesSet_dst.showContent();

    if (!bsrc_setOK || !bdst_setOK) {
        qDebug() << __FUNCTION__ << ": error: in some check_TFWDataSet";
        return(false);
    }

    //convert using EPSG projection from src to dest //#LP TBC method (proj_XY_from_WF_to_WF)

    //move to dedicated app-sys-config class
    QString qstrApplicationDirPath = "."; //QCoreApplication::applicationDirPath();
    const QString strPathProjDB = qstrApplicationDirPath + QDir::separator() + "projdb" +  QDir::separator();

    bool bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error createProjContext: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_dst.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error createProjContext: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //EPSG:2039 Israel 1993 / Israeli TM Grid
    //3066 to 2039
    bool bCreatedCrsToCrs  = GeoRefModel_imagesSet_src.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:2039");

    //2039 to 4326 (to get lon lat)
         bCreatedCrsToCrs &= GeoRefModel_imagesSet_dst.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");

    if (!bCreatedCrsToCrs) {
        qDebug() << __FUNCTION__ << ": error: createCrsToCrs_to_specifiedTargetEPSGCode";
        return(false);
    }

    QVector<QPointF> vectpoint_ofRoute_srcPoint;

    vectpoint_ofRoute_srcPoint.push_back({0.0, 0.0});
    //vectpoint_ofRoute_srcPoint.push_back({100.0, 200.0});
    //vectpoint_ofRoute_srcPoint.push_back({300.0, 400.0});

    bool bErrorOccured = false;

    for (auto pointIter: vectpoint_ofRoute_srcPoint) {

        //S_xy qcs_src {pointIter.x(), pointIter.y()};

        qDebug();
        qDebug() << "******";
        qDebug() << "* in: qcsXY_src pointIter = " << pointIter;
        qDebug() << "******";

        //test from qcs => to geoXY crs 3066  => to geoXY crs 2039 => to lon,lat 4326
        //test from qcs => to geoXY crs 3066  ======================> to lon,lat 4326
        //and compare the lon,lat couple to see if similar

        S_xy qcsXY_src {pointIter.x(),pointIter.y()};
        S_xy geoXY_src {.0,.0};
        qDebug() << "src.convertXY_from_Qcs_to_WF:";
        bool bConvertFromRouteImportJsonQcs_toXY_srcGeoRef =
                GeoRefModel_imagesSet_src.convertXY_fromQcs_toWF(qcsXY_src, geoXY_src);
        if (!bConvertFromRouteImportJsonQcs_toXY_srcGeoRef) { //should never happen
            qDebug() << __FUNCTION__ << ": error: convertXY_fromQcs_toWF";
            bErrorOccured= true;
            break;
        }

        S_xy geoXY_dst {.0,.0};
        bool bProjXYXY = GeoRefModel_imagesSet_src.projXY_fromWF_toWF(geoXY_src, geoXY_dst);
        if (!bProjXYXY)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toWF";
            bErrorOccured= true;
            break;
        }
        qDebug() << "  geoXY_dst = " << geoXY_dst._x << ", " << geoXY_dst._y;

        S_lonlat lonlat_dst {.0,.0};
        bool bProjXYLonLat = GeoRefModel_imagesSet_dst.projXY_fromWF_toLonLat(geoXY_dst, lonlat_dst);
        if (!bProjXYLonLat)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toLonLat";
            bErrorOccured= true;
            break;
        }

        qDebug() << "  lonlat_dst = " << lonlat_dst._lon << ", " << lonlat_dst._lat;

        {
            //EPSG:3066 ED50 / Jordan TM
            GeoRefModel_imagesSet GeoRefModel_imagesSet_src_to4326;
            bool bsrc_setOK2 = GeoRefModel_imagesSet_src_to4326.setAndLoad_imageWidthHeight_TFWDataIfAvailable(
                {"/home/laurent/04fev2021root/testconv/landsat8/Landsat_8_Aqaba_15m.tif"},
                bSomeLoadedButNoUsable);
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: setAndLoad_imageWidthHeight_TFWDataIfAvailable";
                bErrorOccured= true;
                break;
            }

            bsrc_setOK2 &= GeoRefModel_imagesSet_src_to4326.setEPSGCode_and_storeInCacheForAllImagesSet("3066");
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: setEPSGCode_and_storeInCacheForAllImagesSet";
                bErrorOccured= true;
                break;
            }

            bsrc_setOK2 &= GeoRefModel_imagesSet_src_to4326.check_TFWDataSet();
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: check_TFWDataSet";
                bErrorOccured= true;
                break;
            }

            bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src_to4326.createProjContext(strPathProjDB);
            if (!bReport_geoRefModelImage_forRouteImport_createContext) {
                qDebug() << __FUNCTION__ << ": error: createProjContext: " << strPathProjDB;
                bErrorOccured= true;
                break;
            }
            bCreatedCrsToCrs &= GeoRefModel_imagesSet_src_to4326.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");
            if (!bCreatedCrsToCrs) {
                qDebug() << __FUNCTION__ << ": error createCrsToCrs_to_specifiedTargetEPSGCode";
                bErrorOccured= true;
                break;
            }

            QString strLatLonFromSrcQcs_to4326;
            bool bProjReport = GeoRefModel_imagesSet_src_to4326.projXY_fromQcs_toStrLatLon(pointIter, strLatLonFromSrcQcs_to4326);
            if (!bProjReport) {
                qDebug() << __FUNCTION__ << ": error projXY_fromQcs_toStrLonLat";
                bErrorOccured= true;
                break;
            }

            qDebug() << " strLatLonFromSrcQcs_to4326 = " << strLatLonFromSrcQcs_to4326;
        }
    }

    return(!bErrorOccured);
}



/*
-----------------------
src image: SP27GTIF.TIF  w x h : 699 x 929
---
TFW Data:
     32.8
      0.0000000000
      0.0000000000
    -32.8
 681494.4000000000
1913033.6000000000

           32.7999999999
            0.0000000000
            0.0000000000
          -32.8000000000
       681496.4000000000
      1913033.6000000001

---
Code EPSG: EPSG:26771
-----------------------

-----------------------
dst image: UTM2GTIF.TIF w x h : 699 x 929
---
TFW Data:
       10.0000000000
        0.0000000000
        0.0000000000
      -10.0000000000
   444655.0000000000
  4640505.0000000000


---
Code EPSG: EPSG:26716
-----------------------

//in a GIS there is a little difference visible between the two in term of location/rotation


        test_georefimage("/home/laurent/Downloads/SP27GTIF.tiff", "EPSG:26771", "image_SP27GTIF__EPSG_26771");
        test_georefimage("/home/laurent/Downloads/UTM2GTIF.tiff", "EPSG:26716", "image_UTM2GTIF__EPSG_26716");


*/

//test from qcs => to geoXY crs 4267  => to geoXY crs 26716 => to lon,lat 4326 + to Qcs in dst image
//test from qcs => to geoXY crs 4267  =======================> to lon,lat 4326
//
//compare the lon,lat couple to see if similar
//compare the pixel location and pixel content at the Qcs in dst image
//
//(@LP: tfw data for two images are different)
bool test_convertGeoPoint_3() {

    QString strPath { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/" };

    bool bSomeLoadedButNoUsable = false;
    //SP27GTIF
    GeoRefModel_imagesSet GeoRefModel_imagesSet_src;
    bool bsrc_setOK = GeoRefModel_imagesSet_src.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strPath+"SP27GTIF.TIF"}, bSomeLoadedButNoUsable);
    GeoRefModel_imagesSet_src.setEPSGCode_and_storeInCacheForAllImagesSet("26771");
    if (!bsrc_setOK) {
        qDebug() << __FUNCTION__ << ": error: in src setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    //UTM2GTIF
    GeoRefModel_imagesSet GeoRefModel_imagesSet_dst;
    bool bdst_setOK = GeoRefModel_imagesSet_dst.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strPath+"UTM2GTIF.TIF"}, bSomeLoadedButNoUsable);
    GeoRefModel_imagesSet_dst.setEPSGCode_and_storeInCacheForAllImagesSet("26716");
    if (!bdst_setOK) {
        qDebug() << __FUNCTION__ << ": error: in dst setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    bsrc_setOK &= GeoRefModel_imagesSet_src.check_TFWDataSet();
    bdst_setOK &= GeoRefModel_imagesSet_dst.check_TFWDataSet();

    if (!bsrc_setOK || !bdst_setOK) {
        qDebug() << __FUNCTION__ << ": error: in some check_TFWDataSet";
        return(false);
    }

    GeoRefModel_imagesSet_src.showContent();
    GeoRefModel_imagesSet_dst.showContent();

    //move to dedicated app-sys-config class
    QString qstrApplicationDirPath = "."; //QCoreApplication::applicationDirPath();
    const QString strPathProjDB = qstrApplicationDirPath + QDir::separator() + "projdb" +  QDir::separator();

    bool bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error forRouteImport: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_dst.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error forRouteImport: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    bool bCreatedCrsToCrs = true;

    //26771 to 26716
    bCreatedCrsToCrs &= GeoRefModel_imagesSet_src.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:26716");

    //26716 to 4326
    bCreatedCrsToCrs &= GeoRefModel_imagesSet_dst.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");

    if (!bCreatedCrsToCrs) {
        qDebug() << __FUNCTION__ << ": error: in some createCrsToCrs_to_specifiedTargetEPSGCode";
        return(false);
    }


    QVector<QPointF> vectpoint_ofRoute_srcPoint;
    double dW = 699.0;
    double dH = 929.0;

    vectpoint_ofRoute_srcPoint.push_back({   0.0,  0.0});
    vectpoint_ofRoute_srcPoint.push_back({  dW-1, dH-1});
    vectpoint_ofRoute_srcPoint.push_back({  dW-1,  0.0});
    vectpoint_ofRoute_srcPoint.push_back({   0.0, dH-1});

    vectpoint_ofRoute_srcPoint.push_back({ dW/2.0, dH/2.0});

    bool bErrorOccured = false;

    for (auto pointIter: vectpoint_ofRoute_srcPoint) {

        qDebug();
        qDebug() << "******";
        qDebug() << "* in: qcsXY_src pointIter = " << pointIter;
        qDebug() << "******";

        //test from qcs => to geoXY crs 26771  => to geoXY crs 26716 => to lon,lat 4326
        //test from qcs => to geoXY crs 26771  =======================> to lon,lat 4326
        //and compare the lon,lat couple to see if similar

        S_xy qcsXY_src {pointIter.x(),pointIter.y()};
        S_xy geoXY_src {.0,.0};
        qDebug() << "src.convertXY_from_Qcs_to_WF:";
        bool bConvertFromRouteImportJsonQcs_toXY_srcGeoRef =
                GeoRefModel_imagesSet_src.convertXY_fromQcs_toWF(qcsXY_src, geoXY_src);
        if (!bConvertFromRouteImportJsonQcs_toXY_srcGeoRef) { //should never happen
            qDebug() << __FUNCTION__ << ": error: convertXY_fromQcs_toWF";
            bErrorOccured= true;
            break;
        }

        S_xy geoXY_dst {.0,.0};
        bool bProjXYXY = GeoRefModel_imagesSet_src.projXY_fromWF_toWF(geoXY_src, geoXY_dst);
        if (!bProjXYXY)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toWF";
            bErrorOccured= true;
            break;
        }
        qDebug() << "  geoXY_dst = " << geoXY_dst._x << ", " << geoXY_dst._y;

        S_lonlat lonlat_dst {.0,.0};
        bool bProjXYLonLat = GeoRefModel_imagesSet_dst.projXY_fromWF_toLonLat(geoXY_dst, lonlat_dst);
        if (!bProjXYLonLat)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toLonLat";
            bErrorOccured= true;
            break;
        }

        qDebug() << "* lonlat_dst = " << lonlat_dst._lon << ", " << lonlat_dst._lat;



        S_xy qcs_xy_dst {.0,.0};
        bool bConvGeoXYtoQcs = GeoRefModel_imagesSet_dst.convertXY_fromWF_toQcs(geoXY_dst, qcs_xy_dst);
        if (!bConvGeoXYtoQcs)  {
            qDebug() << __FUNCTION__ << ": error: convertXY_fromWF_toQcs";
            bErrorOccured= true;
            break;
        }

        qDebug() << "* qcs_xy_dst = " << qcs_xy_dst._x << ", " << qcs_xy_dst._y;


        {
            //SP27GTIF 26771
            GeoRefModel_imagesSet GeoRefModel_imagesSet_src_to4326;
            bool bsrc_setOK2 = GeoRefModel_imagesSet_src_to4326.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strPath+"SP27GTIF.TIF"}, bSomeLoadedButNoUsable);
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: setAndLoad_imageWidthHeight_TFWDataIfAvailable";
                bErrorOccured= true;
                break;
            }

            GeoRefModel_imagesSet_src_to4326.setEPSGCode_and_storeInCacheForAllImagesSet("26771");

            bsrc_setOK2 &= GeoRefModel_imagesSet_src_to4326.check_TFWDataSet();
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: setEPSGCode_and_storeInCacheForAllImagesSet";
                bErrorOccured= true;
                break;
            }

            bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src_to4326.createProjContext(strPathProjDB);
            if (!bReport_geoRefModelImage_forRouteImport_createContext) {
                qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
                bErrorOccured= true;
                break;
            }
            bCreatedCrsToCrs &= GeoRefModel_imagesSet_src_to4326.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");
            if (!bCreatedCrsToCrs) {
                qDebug() << __FUNCTION__ << ": error: createCrsToCrs_to_specifiedTargetEPSGCode";
                bErrorOccured= true;
                break;
            }

            QString strLatLonFromSrcQcs_to4326;
            bool bProjReport = GeoRefModel_imagesSet_src_to4326.projXY_fromQcs_toStrLatLon(pointIter, strLatLonFromSrcQcs_to4326);
            if (!bProjReport) {
                qDebug() << __FUNCTION__ << ": error: projXY_fromQcs_toStrLonLat";
                bErrorOccured= true;
                break;
            }

            qDebug() << "* strLatLonFromSrcQcs_to4326 = " << strLatLonFromSrcQcs_to4326;
        }


    }
    return(!bErrorOccured);

/*

results:

******
* in: qcsXY_src pointIter =  QPointF(0,0)
******
* lonlat_dst =  -87.6666 ,  41.9171
* qcs_xy_dst =  7.44537 ,  -6.33504
* strLonLatFromSrcQcs_to4326 =  "-87.6666, 41.9171"

******
* in: qcsXY_src pointIter =  QPointF(698,928)
******
* lonlat_dst =  -87.5834 ,  41.8331
* qcs_xy_dst =  690.51 ,  931.796
* strLonLatFromSrcQcs_to4326 =  "-87.5834, 41.8331"

******
* in: qcsXY_src pointIter =  QPointF(698,0)
******
* lonlat_dst =  -87.5825 ,  41.9166
* qcs_xy_dst =  704.915 ,  4.50917
* strLonLatFromSrcQcs_to4326 =  "-87.5825, 41.9166"

******
* in: qcsXY_src pointIter =  QPointF(0,928)
******
* lonlat_dst =  -87.6674 ,  41.8336
* qcs_xy_dst =  -6.96063 ,  920.97
* strLonLatFromSrcQcs_to4326 =  "-87.6674, 41.8336"

******
* in: qcsXY_src pointIter =  QPointF(349.5,464.5)
******
* lonlat_dst =  -87.6249 ,  41.8751
* qcs_xy_dst =  349.468 ,  463.242
* strLonLatFromSrcQcs_to4326 =  "-87.6249, 41.8751"

and checking with the input image and https://www.gps-coordinates.net/
*/


}

//test from qcs => to geoXY crs 26771 => to 4326 => to geoXY crs 26716 => to Qcs in dst image + lon, lat
//                                  \=> to lon,lat
//compare lon, lat src dst
//compare the src dst pixel location and pixel content at src and dst
//
//(@LP: tfw data for two images are different)
bool test_convertGeoPoint_4() {

    QString strPath { "/home/laurent/inputDataForTests/georef/fromGeoTIFF/chicago/" };

    bool bSomeLoadedButNoUsable = false;
    //SP27GTIF
    GeoRefModel_imagesSet GeoRefModel_imagesSet_src;
    bool bsrc_setOK = GeoRefModel_imagesSet_src.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strPath+"SP27GTIF.TIF"}, bSomeLoadedButNoUsable);
    GeoRefModel_imagesSet_src.setEPSGCode_and_storeInCacheForAllImagesSet("26771");
    if (!bsrc_setOK) {
        qDebug() << __FUNCTION__ << ": error: in src setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    //UTM2GTIF
    GeoRefModel_imagesSet GeoRefModel_imagesSet_dst;
    bool bdst_setOK = GeoRefModel_imagesSet_dst.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strPath+"UTM2GTIF.TIF"}, bSomeLoadedButNoUsable);
    GeoRefModel_imagesSet_dst.setEPSGCode_and_storeInCacheForAllImagesSet("26716");
    if (!bdst_setOK) {
        qDebug() << __FUNCTION__ << ": error: in dst setAndLoad_imageWidthHeight_TFWDataIfAvailable";
        return(false);
    }

    bsrc_setOK &= GeoRefModel_imagesSet_src.check_TFWDataSet();
    bdst_setOK &= GeoRefModel_imagesSet_dst.check_TFWDataSet();

    if (!bsrc_setOK || !bdst_setOK) {
        qDebug() << __FUNCTION__ << ": error: in some check_TFWDataSet";
        return(false);
    }

    GeoRefModel_imagesSet_src.showContent();
    GeoRefModel_imagesSet_dst.showContent();

    //move to dedicated app-sys-config class
    QString qstrApplicationDirPath = QCoreApplication::applicationDirPath();
    const QString strPathProjDB = qstrApplicationDirPath + QDir::separator() + "projdb" +  QDir::separator();

    bool bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error forRouteImport: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_dst.createProjContext(strPathProjDB);
    if (!bReport_geoRefModelImage_forRouteImport_createContext) {
        qDebug() << __FUNCTION__ << ": error forRouteImport: creating context for Proj; PathProjDB used: " << strPathProjDB;
        return(false);
    }

    //26771 to 4326
    bool bCreatedCrsToCrs  = GeoRefModel_imagesSet_src.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");

    //26716 to 4326
         bCreatedCrsToCrs &= GeoRefModel_imagesSet_dst.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");

    if (!bCreatedCrsToCrs) {
        qDebug() << __FUNCTION__ << ": error: in some createCrsToCrs_to_specifiedTargetEPSGCode";
        return(false);
    }


    QVector<QPointF> vectpoint_ofRoute_srcPoint;
    double dW = 699.0;
    double dH = 929.0;

    vectpoint_ofRoute_srcPoint.push_back({   0.0,  0.0});
    vectpoint_ofRoute_srcPoint.push_back({  dW-1, dH-1});
    vectpoint_ofRoute_srcPoint.push_back({  dW-1,  0.0});
    vectpoint_ofRoute_srcPoint.push_back({   0.0, dH-1});
    vectpoint_ofRoute_srcPoint.push_back({ dW/2.0, dH/2.0});

    bool bErrorOccured = false;

    for (auto pointIter: vectpoint_ofRoute_srcPoint) {

        qDebug();
        qDebug() << "******";
        qDebug() << "* in: qcsXY_src pointIter = " << pointIter;
        qDebug() << "******";

        //test from qcs => to geoXY crs 26771  => to geoXY crs 26771 => to lon,lat 4326
        //test from qcs => to geoXY crs 26771  =======================> to lon,lat 4326
        //and compare the lon,lat couple to see if similar

        S_xy qcsXY_src {pointIter.x(),pointIter.y()};
        S_xy geoXY_src {.0,.0};
        qDebug() << "src.convertXY_from_Qcs_to_WF:";
        bool bConvertFromRouteImportJsonQcs_toXY_srcGeoRef =
                GeoRefModel_imagesSet_src.convertXY_fromQcs_toWF(qcsXY_src, geoXY_src);
        if (!bConvertFromRouteImportJsonQcs_toXY_srcGeoRef) { //should never happen
            qDebug() << __FUNCTION__ << ": error: convertXY_fromQcs_toWF";
            bErrorOccured= true;
            break;
        }

        qDebug() << "  geoXY_src = " << geoXY_src._x << ", " << geoXY_src._y;

        S_lonlat lonlat_src {.0,.0};
        bool bProjXYLonLat = GeoRefModel_imagesSet_src.projXY_fromWF_toLonLat(geoXY_src, lonlat_src);
        if (!bProjXYLonLat)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toLonLat";
            bErrorOccured= true;
            break;
        }
        qDebug() << "* lonlat_src = " << lonlat_src._lon << ", " << lonlat_src._lat;


        S_xy geoXY_dst {.0,.0};
        bProjXYLonLat = GeoRefModel_imagesSet_dst.projXY_fromLonLat_toWF(lonlat_src, geoXY_dst);
        if (!bProjXYLonLat)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromLonLat_toWF";
            bErrorOccured= true;
            break;
        }

        qDebug() << "* geoXY_dst = " << geoXY_dst._x << ", " << geoXY_dst._y;

        S_xy qcs_dst {.0,.0};
        bool bConvXY = GeoRefModel_imagesSet_dst.convertXY_fromWF_toQcs(geoXY_dst, qcs_dst);
        if (!bConvXY)  {
            qDebug() << __FUNCTION__ << ": error: convertXY_fromWF_toQcs";
            bErrorOccured= true;
            break;
        }

        qDebug() << "* qcs_dst = " << qcs_dst._x << ", " << qcs_dst._y;


        /*
        S_xy geoXY_dst {.0,.0};
        bool bProjXYXY = GeoRefModel_imagesSet_src.projXY_fromWF_toWF(geoXY_src, geoXY_dst);
        if (!bProjXYXY)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toWF";
            bErrorOccured= true;
            break;
        }
        qDebug() << "  geoXY_dst = " << geoXY_dst._x << ", " << geoXY_dst._y;

        S_lonlat lonlat_dst {.0,.0};
        bool bProjXYLonLat = GeoRefModel_imagesSet_dst.projXY_fromWF_toLonLat(geoXY_dst, lonlat_dst);
        if (!bProjXYLonLat)  {
            qDebug() << __FUNCTION__ << ": error: projXY_fromWF_toLonLat";
            bErrorOccured= true;
            break;
        }

        qDebug() << "* lonlat_dst = " << lonlat_dst._lon << ", " << lonlat_dst._lat;

        S_xy qcs_xy_dst {.0,.0};
        bool bConvGeoXYtoQcs = GeoRefModel_imagesSet_dst.convertXY_fromWF_toQcs(geoXY_dst, qcs_xy_dst);
        if (!bConvGeoXYtoQcs)  {
            qDebug() << __FUNCTION__ << ": error: convertXY_fromWF_toQcs";
            bErrorOccured= true;
            break;
        }

        qDebug() << "* qcs_xy_dst = " << qcs_xy_dst._x << ", " << qcs_xy_dst._y;

        {
            //SP27GTIF 26771
            GeoRefModel_imagesSet GeoRefModel_imagesSet_src_to4326;
            bool bsrc_setOK2 = GeoRefModel_imagesSet_src_to4326.setAndLoad_imageWidthHeight_TFWDataIfAvailable({strPath+"SP27GTIF.TIF"});
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: setAndLoad_imageWidthHeight_TFWDataIfAvailable";
                bErrorOccured= true;
                break;
            }

            GeoRefModel_imagesSet_src_to4326.setEPSGCode_and_storeInCacheForAllImagesSet("26771");

            bsrc_setOK2 &= GeoRefModel_imagesSet_src_to4326.check_TFWDataSet();
            if (!bsrc_setOK2) {
                qDebug() << __FUNCTION__ << ": error: setEPSGCode_and_storeInCacheForAllImagesSet";
                bErrorOccured= true;
                break;
            }

            bReport_geoRefModelImage_forRouteImport_createContext = GeoRefModel_imagesSet_src_to4326.createProjContext(strPathProjDB);
            if (!bReport_geoRefModelImage_forRouteImport_createContext) {
                qDebug() << __FUNCTION__ << ": error creating context for Proj; PathProjDB used: " << strPathProjDB;
                bErrorOccured= true;
                break;
            }
            bCreatedCrsToCrs &= GeoRefModel_imagesSet_src_to4326.createCrsToCrs_to_specifiedTargetEPSGCode("EPSG:4326");
            if (!bCreatedCrsToCrs) {
                qDebug() << __FUNCTION__ << ": error: createCrsToCrs_to_specifiedTargetEPSGCode";
                bErrorOccured= true;
                break;
            }

            QString strLonLatFromSrcQcs_to4326;
            bool bProjReport = GeoRefModel_imagesSet_src_to4326.projXY_fromQcs_toStrLonLat(pointIter, strLonLatFromSrcQcs_to4326);
            if (!bProjReport) {
                qDebug() << __FUNCTION__ << ": error: projXY_fromQcs_toStrLonLat";
                bErrorOccured= true;
                break;
            }

            qDebug() << "* strLonLatFromSrcQcs_to4326 = " << strLonLatFromSrcQcs_to4326;
        }
        */

    }
    return(!bErrorOccured);

/*result:

******
* in: qcsXY_src pointIter =  QPointF(0,0)
******
* lonlat_src =  -87.6666 ,  41.9171
* geoXY_dst =  444724 ,  4.64057e+06
* qcs_dst =  7.44537 ,  -6.33504

******
* in: qcsXY_src pointIter =  QPointF(698,928)
******
* lonlat_src =  -87.5834 ,  41.8331
* geoXY_dst =  451555 ,  4.63119e+06
* qcs_dst =  690.51 ,  931.796

******
* in: qcsXY_src pointIter =  QPointF(698,0)
******
* lonlat_src =  -87.5825 ,  41.9166
* geoXY_dst =  451699 ,  4.64046e+06
* qcs_dst =  704.915 ,  4.50917

******
* in: qcsXY_src pointIter =  QPointF(0,928)
******
* lonlat_src =  -87.6674 ,  41.8336
* geoXY_dst =  444580 ,  4.6313e+06
* qcs_dst =  -6.96063 ,  920.97

******
* in: qcsXY_src pointIter =  QPointF(349.5,464.5)
******
* lonlat_src =  -87.6249 ,  41.8751
* geoXY_dst =  448145 ,  4.63588e+06
* qcs_dst =  349.468 ,  463.242

*/

}


//same EPSG
//tfw data different

bool test_convertGeoPoint_sameEPSGCode() {

    QString strPath { "/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/fake_tfwDiff/faked/" };

    QString qstrFilenameMid  {strPath+"UTM2GTIF_midScaleUp.TIF"};
    QString qstrFilenameFull {strPath+"UTM2GTIF.TIF"};

    QString qstrFileA =  qstrFilenameMid;
    QString qstrFileB = qstrFilenameFull;

    bool bErrorOccured = false;

    bool bSomeLoadedButNoUsable = false;

    for (int i=0; i< 2; i++) {

        if (i) {
            QString strExchange = qstrFileA;
            qstrFileA = qstrFileB;
            qstrFileB = strExchange;
        }

        qDebug() << __FUNCTION__ << "----------------------------------------------";

        //UTM2GTIF
        GeoRefModel_imagesSet GeoRefModel_imagesSet_src;
        bool bsrc_setOK = GeoRefModel_imagesSet_src.setAndLoad_imageWidthHeight_TFWDataIfAvailable({qstrFileA}, bSomeLoadedButNoUsable);
        GeoRefModel_imagesSet_src.setEPSGCode_and_storeInCacheForAllImagesSet("26716");
        if (!bsrc_setOK) {
            qDebug() << __FUNCTION__ << ": error: in src setAndLoad_imageWidthHeight_TFWDataIfAvailable";
            return(false);
        }

        //UTM2GTIF
        GeoRefModel_imagesSet GeoRefModel_imagesSet_dst;
        bool bdst_setOK = GeoRefModel_imagesSet_dst.setAndLoad_imageWidthHeight_TFWDataIfAvailable({qstrFileB},bSomeLoadedButNoUsable);
        GeoRefModel_imagesSet_dst.setEPSGCode_and_storeInCacheForAllImagesSet("26716");
        if (!bdst_setOK) {
            qDebug() << __FUNCTION__ << ": error: in dst setAndLoad_imageWidthHeight_TFWDataIfAvailable";
            return(false);
        }

        bsrc_setOK &= GeoRefModel_imagesSet_src.check_TFWDataSet();
        bdst_setOK &= GeoRefModel_imagesSet_dst.check_TFWDataSet();

        if (!bsrc_setOK || !bdst_setOK) {
            qDebug() << __FUNCTION__ << ": error: in some check_TFWDataSet";
            return(false);
        }

        GeoRefModel_imagesSet_src.showContent();
        GeoRefModel_imagesSet_dst.showContent();



        QVector<QPointF> vectpoint_ofRoute_srcPoint;
        double dW = 699.0;
        double dH = 929.0;

        vectpoint_ofRoute_srcPoint.push_back({   0.0,  0.0});
        vectpoint_ofRoute_srcPoint.push_back({  dW-1, dH-1});
        vectpoint_ofRoute_srcPoint.push_back({  dW-1,  0.0});
        vectpoint_ofRoute_srcPoint.push_back({   0.0, dH-1});

        vectpoint_ofRoute_srcPoint.push_back({ dW/2.0, dH/2.0});

        for (auto pointIter: vectpoint_ofRoute_srcPoint) {

            qDebug();
            qDebug() << "******";
            qDebug() << "* in: qcsXY_src pointIter = " << pointIter;
            qDebug() << "******";

            S_xy qcs_src {pointIter.x(), pointIter.y()};
            S_xy geoXY_src {.0,.0};
            bool bConvert_qcsToWF_src =
                    GeoRefModel_imagesSet_src.convertXY_fromQcs_toWF(qcs_src, geoXY_src);
            if (!bConvert_qcsToWF_src) { //should never happen
                qDebug() << "error: convertXY_fromQcs_toWF";
                bErrorOccured= true;
                break;
            }

            qDebug() << "* geoXY_src   = " << geoXY_src._x << ", " << geoXY_src._y;

            S_xy qcs_dst {.0,.0};
            bool bConvertFromRoute_geoXY_to_dstQcs =
                    GeoRefModel_imagesSet_dst.convertXY_fromWF_toQcs(geoXY_src, qcs_dst);
            if (!bConvertFromRoute_geoXY_to_dstQcs) { //should never happen
                qDebug() << "error: convertXY_fromWF_toQcs";
                bErrorOccured= true;
                break;
            }

            qDebug() << "* qcs_dst   = " << qcs_dst._x << ", " << qcs_dst._y;
        }
        if (bErrorOccured) {
            break;
        }
    }

    return(!bErrorOccured);


    /*
    result:

----------------------------------------------
ImageGeoRef:: showContent _qvectqstrWFCandidate[i] = "/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/fake_tfwDiff/faked/UTM2GTIF_midScaleUp.TFW"
ImageGeoRef:: showContent _qvectqstrWFCandidate[i] = "/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/fake_tfwDiff/faked/UTM2GTIF.TFW"

******
* in: qcsXY_src pointIter =  QPointF(0,0)
******
* geoXY_src =  446394 ,  4.63819e+06
* qcs_dst   =  174.375 ,  231.75

******
* in: qcsXY_src pointIter =  QPointF(698,928)
******
* geoXY_src =  449884 ,  4.63355e+06
* qcs_dst   =  523.375 ,  695.75

******
* in: qcsXY_src pointIter =  QPointF(698,0)
******
* geoXY_src =  449884 ,  4.63819e+06
* qcs_dst   =  523.375 ,  231.75

******
* in: qcsXY_src pointIter =  QPointF(0,928)
******
* geoXY_src =  446394 ,  4.63355e+06
* qcs_dst   =  174.375 ,  695.75

******
* in: qcsXY_src pointIter =  QPointF(349.5,464.5)
******
* geoXY_src =  448141 ,  4.63587e+06
* qcs_dst   =  349.125 ,  464

----------------------------------------------
"/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/fake_tfwDiff/faked/UTM2GTIF.TFW"
"/home/laurent/work/dev_techtest_sandbox/georef/test_proj6/fake_tfwDiff/faked/UTM2GTIF_midScaleUp.TFW"

******
* in: qcsXY_src pointIter =  QPointF(0,0)
******
* geoXY_src =  444650 ,  4.64051e+06
* qcs_dst   =  -348.75 ,  -463.5

******
* in: qcsXY_src pointIter =  QPointF(698,928)
******
* geoXY_src =  451630 ,  4.63123e+06
* qcs_dst   =  1047.25 ,  1392.5

******
* in: qcsXY_src pointIter =  QPointF(698,0)
******
* geoXY_src =  451630 ,  4.64051e+06
* qcs_dst   =  1047.25 ,  -463.5

******
* in: qcsXY_src pointIter =  QPointF(0,928)
******
* geoXY_src =  444650 ,  4.63123e+06
* qcs_dst   =  -348.75 ,  1392.5

******
* in: qcsXY_src pointIter =  QPointF(349.5,464.5)
******
* geoXY_src =  448145 ,  4.63587e+06
* qcs_dst   =  350.25 ,  465.5
*
*
*/

}










