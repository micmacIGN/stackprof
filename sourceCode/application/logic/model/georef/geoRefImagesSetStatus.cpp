#include <QDebug>

#include <QString>
#include <QHash>

#include "georefModel_imagesSet.hpp"

#include "geoRefImagesSetStatus.hpp"

//the differents use cases with wordfile parameters and epsg code as available/not avalaible/in error/etc are handled using a S_e_geoRefImagesSetStatus
S_e_geoRefImagesSetStatus::S_e_geoRefImagesSetStatus() {
    qDebug() << __FUNCTION__ << "S_e_geoRefImagesSetStatus::S_e_geoRefImagesSetStatus()";
    _eiSSgTFW_allStatusCase = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_notSetValue;
    _eiSS_geoEPSG = e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_notSetValue;
    _eiSSgTFW_reducedToBigCases = e_imagesSetStatus_geoTFWData_reducedToBigCases::e_iSS_geoTFWData_rTBC_notSetValue;
}

S_boolAndStrMsgStatusAbout_geoRefImageSetStatus::S_boolAndStrMsgStatusAbout_geoRefImageSetStatus() {
    _bWorldFileData_available = false;
    _bEPSG_available = false;
}

void S_boolAndStrMsgStatusAbout_geoRefImageSetStatus::showContent() {

    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus)";
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _bWorldFileData_available =" << _bWorldFileData_available;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _strMsgAboutWorldFileData =" << _strMsgAboutWorldFileData;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _strMsgAboutWorldFileData_errorDetails =" << _strMsgAboutWorldFileData_errorDetails;
    qDebug() << __FUNCTION__;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _bEPSG_available =" << _bEPSG_available;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _strMsgAboutEPSG =" << _strMsgAboutEPSG;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _strMsgAboutEPSG_errorDetails =" << _strMsgAboutEPSG_errorDetails;
    qDebug() << __FUNCTION__ << "(S_boolAndStrMsgStatusAbout_geoRefImageSetStatus) _strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation =" << _strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation;

}




//@#LP no GeoRefImageSetStatus update, it's just about sbaStrMsg_gRIS_routeSetEdition
void routesetEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_routeSetEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_routeSetEdition) {

    sbaStrMsg_gRIS_routeSetEdition._bEPSG_available = false;
    sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG = "EPSG code: ";
    sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG_errorDetails.clear();
    sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation.clear();

    QHash<e_imagesSetStatus_geoEPSG, QString> qhash_EPSGstrMsg_errorDetails;
    //can happen:
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_unknowError, "Unknown error #3");
    //should really never happen:
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notSetValue, "Unknow error #1");
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_unsyncEPSGData, "Unknow error #2");
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_noneFound, "None Found");
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_available, "Not and error, the file is available");

#define DEF_searchinTheSameDirectoryOfTheInputFile_status

    switch (segeoRefImageSetStatus_routeSetEdition._eiSS_geoEPSG) {
        case e_iSS_geoEPSG_available:
            sbaStrMsg_gRIS_routeSetEdition._bEPSG_available = true;
#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG += "set from cache";
#endif

#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG += "set from .jepsg file";
#endif
            break;

        case e_iSS_geoEPSG_notAvailable_noneFound:
#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG += "not found in cache";
#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG += "not found from .jepsg file";
#endif
            break;

        //should never happen:
        case e_iSS_geoEPSG_notSetValue:
        case e_iSS_geoEPSG_notAvailable_unsyncEPSGData:
        //can happens:
        case e_iSS_geoEPSG_notAvailable_unknowError:
#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG = "Error reading cache";
#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutEPSG = "Error reading .jepsg file";
#endif
            qhash_EPSGstrMsg_errorDetails.value(segeoRefImageSetStatus_routeSetEdition._eiSS_geoEPSG, "Unknow error #4");
            break;
    }
}


void routesetEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_routeSetEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_routeSetEdition) {

    qDebug() << __FUNCTION__ << ": eimagesSetStatus_forOutside._eiSSgTFW_allStatusCase = " << segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase;

    switch (segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase) {
        case e_iSS_geoTFWData_available:
            segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_available;
            break;

        case e_iSS_geoTFWData_notAvailable_noneFound:
            segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_noneFound;
            break;

        case e_iSS_geoTFWData_notAvailable_errorWhileLoading:
        case e_iSS_geoTFWData_notAvailable_loadedButNotUsable:
        case e_iSS_geoTFWData_notAvailable_unknowError:
        //values which should never occurs when editing routeset:
        case e_iSS_geoTFWData_notAvailable_unsyncTFWData: // editing routeset is made when only one background image
        case e_iSS_geoTFWData_notSetValue:
            segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
    }

    sbaStrMsg_gRIS_routeSetEdition._bWorldFileData_available = false;
    sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData = "WorldFile data: ";
    sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData_errorDetails = "";

    QHash<e_imagesSetStatus_geoTFWData, QString> qhash_TFWstrMsg_errorDetails;
    //can happens:
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_errorWhileLoading, "Error while loading");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_loadedButNotUsable, "Loaded but can not be used");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_unknowError, "Unknown error #3");
    //(key,value) pairs which should really never been used below (cases processed separately):
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notSetValue, "Unknow error #1");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_unsyncTFWData, "Unknow error #2");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_noneFound, "None Found");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_available, "Not and error, the file is available");

    switch (segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_reducedToBigCases) {
        case e_iSS_geoTFWData_rTBC_available:
            sbaStrMsg_gRIS_routeSetEdition._bWorldFileData_available = true;
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData += "set from file";
            break;

        case e_iSS_geoTFWData_rTBC_notAvailable_noneFound:
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData += "no file found";
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData_errorDetails = "World file data file for image not found or failed to load";
            break;

        //should never happen:
        case e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData: //never used value for routeset (only one image for background)
        case e_iSS_geoTFWData_rTBC_notSetValue:
        //can happen:
        case e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason:
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData += "error reading file";
            sbaStrMsg_gRIS_routeSetEdition._strMsgAboutWorldFileData_errorDetails =
                    qhash_TFWstrMsg_errorDetails.value(segeoRefImageSetStatus_routeSetEdition._eiSSgTFW_allStatusCase, "Unknow error #4");
            break;
    }
}


void projectEditionFunction_updateGeoRefImageSetStatus_TFWDataPart(
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_projectEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_projectEdition) {

    qDebug() << __FUNCTION__ << ": eimagesSetStatus_forOutside._eiSSgTFW_allStatusCase = " << segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase;

    switch (segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase) {
        case e_iSS_geoTFWData_available:
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_available;
            break;

        case e_iSS_geoTFWData_notAvailable_noneFound:
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_noneFound;
            break;

        case e_iSS_geoTFWData_notAvailable_unsyncTFWData:
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData;
            break;

        case e_iSS_geoTFWData_notAvailable_errorWhileLoading:
        case e_iSS_geoTFWData_notAvailable_loadedButNotUsable:
        case e_iSS_geoTFWData_notAvailable_unknowError:
        //values which should never occurs when editing project:
        case e_iSS_geoTFWData_notSetValue:
            segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases = e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason;
    }

    sbaStrMsg_gRIS_projectEdition._bWorldFileData_available = false;
    sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData = "WorldFile data: ";
    sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails = "";

    QHash<e_imagesSetStatus_geoTFWData, QString> qhash_TFWstrMsg_errorDetails;
    //can happens:
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_errorWhileLoading, "Error while loading");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_loadedButNotUsable, "Loaded but can not be used");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_unknowError, "Unknown error #3");
    //(key,value) pairs which should really never been used below (cases processed separately):
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notSetValue, "Unknow error #1");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_noneFound, "None Found");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_available, "Not and error, the file is available");
    qhash_TFWstrMsg_errorDetails.insert(e_iSS_geoTFWData_notAvailable_unsyncTFWData, "World file data found from images are unsync");

    switch (segeoRefImageSetStatus_projectEdition._eiSSgTFW_reducedToBigCases) {
        case e_iSS_geoTFWData_rTBC_available:
            sbaStrMsg_gRIS_projectEdition._bWorldFileData_available = true;
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData += "set from found file.";
            break;

        case e_iSS_geoTFWData_rTBC_notAvailable_noneFound:
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData += "no file found";
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails = "World file data file for image not found or failed to load";
            break;

        case e_iSS_geoTFWData_rTBC_notAvailable_unsyncTFWData:
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData += "Unsync World file data";
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails = "World file data found from images are unsync";
            break;

        //should never happen:
        case e_iSS_geoTFWData_rTBC_notSetValue:
        //can happen:
        case e_iSS_geoTFWData_rTBC_notAvailable_anonymizedReason:
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData += "error reading file";
            sbaStrMsg_gRIS_projectEdition._strMsgAboutWorldFileData_errorDetails =
                    qhash_TFWstrMsg_errorDetails.value(segeoRefImageSetStatus_projectEdition._eiSSgTFW_allStatusCase, "Unknow error #4");
            break;
    }
}

void projectEditionFunction_updateGeoRefImageSetStatus_EPSGCodePart(
        GeoRefModel_imagesSet *ptrGeoRefModel_imagesSet,
        S_e_geoRefImagesSetStatus& segeoRefImageSetStatus_projectEdition,
        S_boolAndStrMsgStatusAbout_geoRefImageSetStatus& sbaStrMsg_gRIS_projectEdition) {

    sbaStrMsg_gRIS_projectEdition._bEPSG_available = false;
    sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "EPSG code: ";
    sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails.clear();
    sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation.clear();

    QHash<e_imagesSetStatus_geoEPSG, QString> qhash_EPSGstrMsg_errorDetails;
    //can happen:
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_unknowError, "Unknown error #3");
    //key,value pairs which should never been used below:
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notSetValue, "Unknow error #1");
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_noneFound, "None Found");

#ifdef DEF_searchinCache__status
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_unsyncEPSGData, "Unsync EPSG Code found in cache for images");
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_available, "Not and error, set from cache");
#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_notAvailable_unsyncEPSGData, "Unsync EPSG Code found in .jepsg files for images");
    qhash_EPSGstrMsg_errorDetails.insert(e_iSS_geoEPSG_available, "Not and error, set from .jepsg file");
#endif

    //extra adjustements specific to projectEdition:
    QVector<S_DetailsAboutEPSGCodeFoundFromCacheForOneImage> qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage;
    e_DetailsAboutEPSGCodeFoundFromCache_forTheImageSet eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_notSetValue;
    if (ptrGeoRefModel_imagesSet) {
        ptrGeoRefModel_imagesSet->get_detailsAboutEPSGCodeFoundFromCacheForTheImageSet(
            qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage, eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet);
    }

    qDebug() << __FUNCTION__ << __LINE__ << "eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = " << eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;

    switch (eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet) {
        case e_DetailsAboutEPSGCFC_fTIS_notSetValue:
            //override segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG
            segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "Dev Error #10";
            return;

        case e_DetailsAboutEPSGCFC_fTIS_noneImage:
            //use case that should be identified some where else
            //override segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG
            segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "None image set";
            return;

        //For project edition, we consider that error reading EPSG code in the cache can hide possible unsync EPSG code between images.
        //And in this case, we do not want to hide this problem, and we do not want to propose to the user to set a EPSG code read for the cache
        //to the others images with read error. This could be wrong, like pollution to a more sane context.
        case e_DetailsAboutEPSGCFC_fTIS_noneFound_allReadError:
        case e_DetailsAboutEPSGCFC_fTIS_noneFound_someReadError:
        case e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundWithSomeReadError:
            //override segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG
            segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;
#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "Error reading cache";
#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "Error reading .jepsg file";
#endif
            return;

        //standard sane context:
        case e_DetailsAboutEPSGCFC_fTIS_noneFound_noneReadError:
            break;

        case e_DetailsAboutEPSGCFC_fTIS_allFound:
        case e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundButNoneReadError:
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_EPSGCodeRevelantForUserConfirmation = ptrGeoRefModel_imagesSet->get_qstrEPSGCode();
            break;
    }

    //root foundation:
    switch (segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG) {
        case e_iSS_geoEPSG_available:
            sbaStrMsg_gRIS_projectEdition._bEPSG_available = true;
#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG += "set from cache";
#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG += "set from .jepsg file";
#endif
        break;

        case e_iSS_geoEPSG_notAvailable_noneFound:        
#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG += "None found in cache";
#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG += "None found from .jepsg file";
#endif
            break;

        case e_iSS_geoEPSG_notAvailable_unsyncEPSGData:
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "EPSG code Error";

#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails= "Unsync EPSG Code found in cache for images";

#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails= "Unsync EPSG Code found in .jepsg files for images";
#endif
             break;

        //should never happen:
        case e_iSS_geoEPSG_notSetValue:

        //can happens:
        case e_iSS_geoEPSG_notAvailable_unknowError:
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG = "EPSG code Error";

#ifdef DEF_searchinCache__status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails = "Error reading cache";

#endif
#ifdef DEF_searchinTheSameDirectoryOfTheInputFile_status
            sbaStrMsg_gRIS_projectEdition._strMsgAboutEPSG_errorDetails = "Error reading .jepsg file";
#endif
            //qhash_EPSGstrMsg_errorDetails.value(segeoRefImageSetStatus_projectEdition._eiSS_geoEPSG, "Unknow error #4");
            break;
    }
}
