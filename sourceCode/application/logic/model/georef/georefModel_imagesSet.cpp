#include <QDebug>
#include <QFileInfo>
#include <QDir>

#include <QPointF>

#include "../../georef/ImageGeoRef.h"

#include "georefModel_imagesSet.hpp"

#include "../../toolbox/toolbox_pathAndFile.h"

#include "../../io/iojsoninfos.h"

#include "../../georef/Proj6_crs_to_crs.h"

#include "../../toolbox/toolbox_json.h"

#define DEF_searchinTheSameDirectoryOfTheInputFile

GeoRefModel_imagesSet::GeoRefModel_imagesSet(QObject *parent): QObject(parent),
    _imagesSetStatus_geoTFWData(e_iSS_geoTFWData_notSetValue),
    _imagesSetStatus_geoEPSG(e_iSS_geoEPSG_notSetValue),
    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection(-1) {

    _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_notSetValue;
    _bUseGeoRef = true; //true by default
}

S_DetailsAboutEPSGCodeFoundFromCacheForOneImage::S_DetailsAboutEPSGCodeFoundFromCacheForOneImage():
    _eImagesSetStatus_geoEPSG(e_iSS_geoEPSG_notSetValue) {
}

void S_DetailsAboutEPSGCodeFoundFromCacheForOneImage::clear(){
    _strImageFilename.clear();
    _eImagesSetStatus_geoEPSG = e_iSS_geoEPSG_notSetValue;
    _qstrEPSGCode.clear();
}

void S_DetailsAboutEPSGCodeFoundFromCacheForOneImage::show() {
    qDebug() << __FUNCTION__ << "_strImageFilename = " << _strImageFilename;
    qDebug() << __FUNCTION__ << "_eImagesSetStatus_geoEPSG = " << _eImagesSetStatus_geoEPSG;
    qDebug() << __FUNCTION__ << "_qstrEPSGCode = " << _qstrEPSGCode;
}

S_DetailsAboutEPSGCodeFoundFromCacheForOneImage::S_DetailsAboutEPSGCodeFoundFromCacheForOneImage(
        const QString& strImageFilename,
        e_imagesSetStatus_geoEPSG eimagesSetStatus_geoEPSG,
        const QString& qstrEPSGCode):
    _strImageFilename(strImageFilename),
    _eImagesSetStatus_geoEPSG(eimagesSetStatus_geoEPSG),
    _qstrEPSGCode(qstrEPSGCode) {
}

void GeoRefModel_imagesSet::clear() {
    _qvectImageGeoRef.clear();
    _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_notSetValue;
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notSetValue;
    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = -1;

    _qstrEPSGCode.clear();
    _proj6crsTocrs.clear_PJ_PJContext();
    _bUseGeoRef = true; //true by default
}

bool GeoRefModel_imagesSet::toQJsonObject(QJsonObject& qjsonObj) const {

    if (_qvectImageGeoRef.isEmpty()) {
        qDebug() << __FUNCTION__ << " error: _qvectImageGeoRef is empty";
        return(false);
    }

    //should never happens:
    if (_imagesSetStatus_geoTFWData != e_iSS_geoTFWData_available) {
        qDebug() << __FUNCTION__ << " error: status geoTFWData: not available";
        return(false);
    }
    //should never happens:
    if (_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available) {
        qDebug() << __FUNCTION__ << " error: status geoEPSG: not available";
        qDebug() << __FUNCTION__ << "        _imagesSetStatus_geoEPSG = " << _imagesSetStatus_geoEPSG;
        return(false);
    }
    //should never happens:
    if (_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection == -1) {
        qDebug() << __FUNCTION__ << " error: _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection is -1";
        return(false);
    }

    bool bReport_toQJsonObject = false;

    QJsonObject qjsonObj_ImageGeoRef;
    bReport_toQJsonObject = _qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].toQJsonObject(qjsonObj_ImageGeoRef);
    if (!bReport_toQJsonObject) {
        qDebug() << __FUNCTION__ << " error: fail to get QJsonObject from _qvectImageGeoRef[ " << _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection << " ]";
        return(false);
    }

    QString qstr_key_imageGeoRef { "imageGeoRef" };
    qjsonObj.insert(qstr_key_imageGeoRef , qjsonObj_ImageGeoRef);

    QString  qstr_key_EPSGCode { "EPSG_code" };
    QJsonObject qjsonObj_EPSGCode;
    qjsonObj_EPSGCode.insert(qstr_key_EPSGCode, _qstrEPSGCode);
    qjsonObj.insert("coordRefSys_coordTrans", qjsonObj_EPSGCode);

    return(true);
}

//this method considers that the GeoRefModel_imagesSet instance is cleared
bool GeoRefModel_imagesSet::fromQJsonObject(const QJsonObject& qjsonObj) {

    //should never happens:
    if (!_qvectImageGeoRef.isEmpty()) {
        qDebug() << __FUNCTION__ << " error: _qvectImageGeoRef is not empty";
        return(false);
    }

    bool bValueGot = false;

    QString qstr_key_imageGeoRef { "imageGeoRef" };
    QJsonObject QjsonObj_imageGeoRef;
    bValueGot = getQJsonObjectFromJson(qjsonObj, qstr_key_imageGeoRef, QjsonObj_imageGeoRef);
    if (!bValueGot) {
        return(false);
    }

    ImageGeoRef imageGeoRefFromJson;
    bValueGot = imageGeoRefFromJson.fromQJsonObject(QjsonObj_imageGeoRef);
    if (!bValueGot) {
        qDebug() << __FUNCTION__ << " error in ImageGeoRef.fromQJsonObject()";
        return(false);
    }

    QString qstr_key_crsct { "coordRefSys_coordTrans" };
    QJsonObject qjsonObj_crsct;
    bValueGot = getQJsonObjectFromJson(qjsonObj, qstr_key_crsct, qjsonObj_crsct);
    if (!bValueGot) {
        return(false);
    }
    qDebug() << __FUNCTION__ << "qjsonObj_crsct = " << qjsonObj_crsct;

    QString qstr_key_EPSGCode { "EPSG_code" };
    QString qstr_EPSGCodeValue;
    bValueGot = getStringValueFromJson(qjsonObj_crsct, qstr_key_EPSGCode, qstr_EPSGCodeValue);
    if (!qjsonObj_crsct.contains(qstr_key_EPSGCode)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << qstr_key_EPSGCode;
        return(false);
    }   
    qDebug() << __FUNCTION__ << "qstr_EPSGCodeValue:" << qstr_EPSGCodeValue;

    QString qstr_EPSGCodeValue_startWithTag = {"EPSG:"}; //add a check to prevent valid hacker file edition
    if (!qstr_EPSGCodeValue.startsWith(qstr_EPSGCodeValue_startWithTag)) {
        qDebug() << __FUNCTION__ <<  ": error: EPSG code value for key " <<  qstr_key_EPSGCode << " has to start with string: " << qstr_EPSGCodeValue_startWithTag;
        return(false);
    }

    //@LP EPSG string validity is checked later in the code
    _qstrEPSGCode = qstr_EPSGCodeValue;
    _imagesSetStatus_geoEPSG = e_imagesSetStatus_geoEPSG::e_iSS_geoEPSG_available;

    //all check ok; push the imageGeoRefLoadedFromJsonFile
    _qvectImageGeoRef.push_back(imageGeoRefFromJson);
    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = 0;
    _imagesSetStatus_geoTFWData = e_imagesSetStatus_geoTFWData::e_iSS_geoTFWData_available;

    return(true);
}

int GeoRefModel_imagesSet::imageCount() const{
    return(_qvectImageGeoRef.count());
}

bool GeoRefModel_imagesSet::compareWorldFileDataWith(const GeoRefModel_imagesSet& GRMimgSet_compareToMe) const {
    if (!imageCount()||(!GRMimgSet_compareToMe.imageCount())) {
        return(false);
    }
    return( _qvectImageGeoRef[0].checkSame_geoTFWDataconst(GRMimgSet_compareToMe._qvectImageGeoRef[0]));
}


bool GeoRefModel_imagesSet::setAndLoad_imageWidthHeight_TFWDataIfAvailable(const QVector<QString>& qvectqstrImagesPathAndFilename, bool& bSomeLoadedButNotUsable) {

    _qvectImageGeoRef.clear();
    bSomeLoadedButNotUsable = false;
    qDebug() << __FUNCTION__ << " qvectqstrImagesPathAndFilename =" << qvectqstrImagesPathAndFilename;

    //@#LP not empty if filled with just space
    if (qvectqstrImagesPathAndFilename.isEmpty()) {
        qDebug() << __FUNCTION__ << " error: qvectqstrImagesPathAndFilename is empty";
        return(false);
    }
    bool bReport = true;
    int countFilenameEmpty = 0;

    //for each input image, find and load the TFW data if found
    for (const auto& iterqstr: qvectqstrImagesPathAndFilename) {

        if (iterqstr.isEmpty()) {
            countFilenameEmpty++;
            continue;
        }

        ImageGeoRef imgGeoRef(iterqstr);
        bool bLoaded_butNotUsable = false;
        bool bLoaded = imgGeoRef.setAndLoad_imageWidthHeight_TFWDataIfAvailable(bLoaded_butNotUsable);

        if (!bLoaded) {

            if (bLoaded_butNotUsable) {
                qDebug() << __FUNCTION__ << " error in setAndLoad_imageWidthHeight_TFWDataIfAvailable(): bLoaded_butNotUsable";
                bReport = false;
                bSomeLoadedButNotUsable = true;
                break;
            }

            qDebug() << __FUNCTION__ << " setAndLoad_imageWidthHeight_TFWDataIfAvailable() not loaded";

            continue; //@LP for debug purpose: commenting out  continue;  to allow to push the not loaded in _qvectImageGeoRef
        }

        _qvectImageGeoRef.push_back(imgGeoRef);
    }

    if (!bReport) {
        _qvectImageGeoRef.clear();
        return(false);
    }

    //we accept that some file are empty but not all
    if (countFilenameEmpty == qvectqstrImagesPathAndFilename.size()) {
        return(false);
    }

    //we needs at least one file loaded
    return(_qvectImageGeoRef.size() != 0);

}

e_imagesSetStatus_geoEPSG GeoRefModel_imagesSet::setToOneImage_and_getEPSGCodeFromCacheIfAvailable(const QString& strOneImagePathAndFilename, QString& strFoundEPSGCode) {
    strFoundEPSGCode.clear();
    clear();
    ImageGeoRef imgGeoRef(strOneImagePathAndFilename);
    _qvectImageGeoRef.push_back(imgGeoRef);
    e_imagesSetStatus_geoEPSG eimagesSetStatus_geoEPSG = searchInCache_associatedEPSGCode();

    if (eimagesSetStatus_geoEPSG ==e_iSS_geoEPSG_available) {
        strFoundEPSGCode = _qstrEPSGCode;
    }
    return(eimagesSetStatus_geoEPSG);
}

//@LP use it only in very specific cases like missing a routeset background image when opening a routeset (because the image moved or renammed)
//if you need to call this method about a project handling, your implementation needs to be changed; do not call this method
bool GeoRefModel_imagesSet::replace_soleImageFilename(const QString& strImagePathAndFilename) {

    if (imageCount() != 1) {
        qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << " error: imageCount not 1";
        return(false);
    }

    qDebug() << __FUNCTION__ << "strImagePathAndFilename : " << strImagePathAndFilename;

    //@LP use it only in very specific cases like missing a routeset background image when opening a routeset (because the image as moved or was renammed)
    //if you need to call this method about a project handling, your implementation needs to be changed; do not call this method
    _qvectImageGeoRef[0].setImagePathFilename(strImagePathAndFilename);
    return(true);
}

bool GeoRefModel_imagesSet::replace_imagesFilename(const QVector<QString>& qvectStrImageFilename) {

    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << "qvectStrImageFilename= " << qvectStrImageFilename;

    int qvectStrImageFilename_size = qvectStrImageFilename.size();
    if (imageCount() != qvectStrImageFilename_size) {
        qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << " error: imageCount not ok";
        return(false);
    }
    for(int i = 0; i < qvectStrImageFilename_size; i++) {
        _qvectImageGeoRef[i].setImagePathFilename(qvectStrImageFilename[i]);
    }

    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << "_qvectImageGeoRef[...] now is:";
    for(int i = 0; i < qvectStrImageFilename_size; i++) {
        _qvectImageGeoRef[i].showContent();
    }
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << "_qvectImageGeoRef[...] now is (again):";
    for(int i = 0; i < _qvectImageGeoRef.size(); i++) {
        _qvectImageGeoRef[i].showContent();
    }

    return(true);
}


bool GeoRefModel_imagesSet::alignContent_vectImageGeoRefSize_andGeoInfosOnFirst(int size) {

    int qvectImageGeoRef_count = _qvectImageGeoRef.count();
    if (!qvectImageGeoRef_count) {//we align duplicating the first ImageGeorRef data
         return(false);
    }

    if (!size) {
        return(false);
    }

    if (size == qvectImageGeoRef_count) { //but do nothing if size already aligned
        return(true);
    }

    ImageGeoRef ImgGeoRefFirst = _qvectImageGeoRef[0];
    _qvectImageGeoRef.clear();
    for (int i = 0; i < size; i++) {
        _qvectImageGeoRef.push_back(ImgGeoRefFirst);
    }
    for (int i = 0; i < size; i++) {
        _qvectImageGeoRef[i].setImagePathFilename("");
    }

    return(true);
}

void GeoRefModel_imagesSet::showContent() const {

    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << __LINE__ << "_qvectImageGeoRef.size() = " << _qvectImageGeoRef.size();

    if (_qvectImageGeoRef.isEmpty()) {
        qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << " _qvectImageGeoRef is empty";
    } else {
        int i = 0;
        for (const auto& iter: _qvectImageGeoRef) {
            qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ << "_qvectImageGeoRef[" << i << "].showContent():";
            iter.showContent();
            i++;
        }
    }
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ <<  " _qstrEPSGCode = " << _qstrEPSGCode;
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ <<  "_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = " << _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection;
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ <<  "_imagesSetStatus_geoTFWData = " << _imagesSetStatus_geoTFWData;
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ <<  "_imagesSetStatus_geoEPSG = " << _imagesSetStatus_geoEPSG;
#ifdef DEF_searchinCache
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ <<  "_qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages = " << _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages;
#endif
    qDebug() << "GeoRefModel_imagesSet: " << __FUNCTION__ <<  "_bUseGeoRef = " << _bUseGeoRef;

    //qDebug() << __FUNCTION__ << _proj6crsTocrs.showContent();
}

bool GeoRefModel_imagesSet::getUniqueImageGeoRef(ImageGeoRef& imgeGeoRef) const {
    imgeGeoRef.clear();
    if (_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection == -1) {
        return(false);
    }
    imgeGeoRef = _qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection];
    return(true);
}

bool GeoRefModel_imagesSet::setDataFromAnotherGeoRefModelImagesSet(const GeoRefModel_imagesSet& geoRM_imgSet) {

    qDebug() << __FUNCTION__ << "--- content of GeoRefModel_imagesSet to get data from:";
    geoRM_imgSet.showContent();
    qDebug() << __FUNCTION__ << "sdaEPSGCFC_aboutEachImage._qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage:";
    for (auto sdaEPSGCFC_aboutEachImage: geoRM_imgSet._qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
        sdaEPSGCFC_aboutEachImage.show();
    }
    qDebug() << __FUNCTION__ << "geoRM_imgSet._eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = " <<  geoRM_imgSet._eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;

    qDebug() << __FUNCTION__ ;
    qDebug() << __FUNCTION__ << "--- content before:";
    showContent();
    qDebug() << __FUNCTION__ << "_qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage:";
    for (auto sdaEPSGCFC_aboutEachImage: _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
        sdaEPSGCFC_aboutEachImage.show();
    }
    qDebug() << __FUNCTION__ << "_eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = " <<  _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;


    if (_qvectImageGeoRef.size()) {
        qDebug() << __FUNCTION__ << " error: none ImageGeoRef set";
        return(false);
    }

    _qvectImageGeoRef = geoRM_imgSet._qvectImageGeoRef;
    _qstrEPSGCode = geoRM_imgSet._qstrEPSGCode;
    _imagesSetStatus_geoEPSG = geoRM_imgSet._imagesSetStatus_geoEPSG;  //geoRM_imgSet._imagesSetStatus_geoEPSG must be e_iSS_geoEPSG_available with correct implementation
    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = geoRM_imgSet._idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection;
    _bUseGeoRef = geoRM_imgSet._bUseGeoRef;

    qDebug() << __FUNCTION__ ;
    qDebug() << __FUNCTION__ << "--- content after#1:";
    showContent();
    qDebug() << __FUNCTION__ << "_qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage:";
    for (auto sdaEPSGCFC_aboutEachImage: _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
        sdaEPSGCFC_aboutEachImage.show();
    }
    qDebug() << __FUNCTION__ << "_eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = " <<  _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;

    check_TFWDataSet();

    qDebug() << __FUNCTION__ ;
    qDebug() << __FUNCTION__ << "--- content after#2:";
    showContent();    
    qDebug() << __FUNCTION__ << "_qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage:";
    for (auto sdaEPSGCFC_aboutEachImage: _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
        sdaEPSGCFC_aboutEachImage.show();
    }
    qDebug() << __FUNCTION__ << "_eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = " <<  _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;


    bool bSetOK = true;
    bSetOK &= (_imagesSetStatus_geoEPSG == e_iSS_geoEPSG_available);
    bSetOK &= (_imagesSetStatus_geoTFWData == e_iSS_geoTFWData_available);

    return(bSetOK);

}

bool GeoRefModel_imagesSet::setDefaultValuesAsNoGeorefUsed(const QString& strImageFilename) {

    ImageGeoRef imgGeoRef;
    imgGeoRef.setImagePathFilename(strImageFilename);
    imgGeoRef.setDefaultValuesAsNoGeorefUsed();

    _qvectImageGeoRef = {imgGeoRef};

    _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_available;

    //-------------
    //for the set:
    _qstrEPSGCode = "EPSG:notSet";
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_available; //to be able to write in the output .jtraces
    _bUseGeoRef = false;
    //---------

    _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage = {};

    _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_notSetValue;

    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = 0;

    return(true);
}


bool GeoRefModel_imagesSet::setDefaultValuesAsNoGeorefUsed_fromInputImageFilenames(const QVector<QString>& qvectqstrImagesPathAndFilename) {

    _qvectImageGeoRef.clear();

    qDebug() << __FUNCTION__ << " qvectqstrImagesPathAndFilename =" << qvectqstrImagesPathAndFilename;

    //@#LP not empty if filled with just space
    if (qvectqstrImagesPathAndFilename.isEmpty()) {
        qDebug() << __FUNCTION__ << " error: qvectqstrImagesPathAndFilename is empty";
        return(false);
    }
    //bool bReport = true;
    int countFilenameEmpty = 0;

    //for each input image, set default value for TFW data
    for (const auto& iterqstr: qvectqstrImagesPathAndFilename) {

        if (iterqstr.isEmpty()) {
            countFilenameEmpty++;
            continue;
        }

        ImageGeoRef imgGeoRef(iterqstr);
        imgGeoRef.setDefaultValuesAsNoGeorefUsed();

        _qvectImageGeoRef.push_back(imgGeoRef);
    }

    //we accept that some file are empty but not all
    if (countFilenameEmpty == qvectqstrImagesPathAndFilename.size()) {
        return(false);
    }

    //we needs at least one file set
    if (!_qvectImageGeoRef.size()) {
        return(false);
    }

    _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_available;

    //-------------
    //for the set:
    _qstrEPSGCode = "EPSG:notSet";
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_available; //to be able to write in the output .jtraces
    _bUseGeoRef = false;
    //---------

    _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage = {};

    _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_notSetValue;

    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = 0;

    return(true);
}

bool GeoRefModel_imagesSet::get_bUseGeoRef() const {
    qDebug() << __FUNCTION__ << "(GeoRefModel_imagesSet)";
    showContent();
    return(_bUseGeoRef);
}

void GeoRefModel_imagesSet::set_bUseGeoRef(bool bUseGeoRef) {
    _bUseGeoRef = bUseGeoRef;
}


e_imagesSetStatus_geoTFWData GeoRefModel_imagesSet::check_TFWDataSet() {

    _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_notSetValue;
    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = -1;

    int qvectImgGR_size = _qvectImageGeoRef.size();

    qDebug() << __FUNCTION__ << "qvectImgGR_size = " << qvectImgGR_size;

    if (!qvectImgGR_size) {
        qDebug() << __FUNCTION__ << " error: none ImageGeoRef set";
        _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_notAvailable_noneFound;
        return(_imagesSetStatus_geoTFWData);
    }

    if (qvectImgGR_size == 1) {
        _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = 0;
        _imagesSetStatus_geoTFWData = convert_iFSGeoTFWData_to_imagesSetStatusGeoTFWData(
            _qvectImageGeoRef[0].get_inputFileStatus_geoTFWData());
        return(_imagesSetStatus_geoTFWData);
    }

    bool bSomecheckSameMade = false;
    int idxOneOfFile_availableForCheckSame = -1;

    //test all combinations
    for (int idx1 = 0; idx1 < qvectImgGR_size; idx1++) {
        if (_qvectImageGeoRef[idx1].get_inputFileStatus_geoTFWData() != e_iFS_geoTFWData_loaded) {
            qDebug() << __FUNCTION__ << "idx1 _qvectImageGeoRef[ " << idx1 << " ] not e_iFS_geoTFWData_loaded";
            continue;
        }
        idxOneOfFile_availableForCheckSame = idx1;
        for (int idx2 = idx1+1; idx2 < qvectImgGR_size; idx2++) {
            if (_qvectImageGeoRef[idx2].get_inputFileStatus_geoTFWData() != e_iFS_geoTFWData_loaded) {
                qDebug() << __FUNCTION__ << " idx2 _qvectImageGeoRef[ " << idx2 << " ] not e_iFS_geoTFWData_loaded";
                continue;
            }
            qDebug() << __FUNCTION__ << " compare [idx1 = " << idx1 << " ] with [idx2 = " << idx2 << " ]";
            bool bAreSame = _qvectImageGeoRef[idx1].checkSame_geoTFWDataconst(_qvectImageGeoRef[idx2]);
            if (!bAreSame) {
                _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_notAvailable_unsyncTFWData;
                return(_imagesSetStatus_geoTFWData);
            }
            bSomecheckSameMade = true;
        }
    }

    if (bSomecheckSameMade) {
        qDebug() << __FUNCTION__ << " some comparison between ImageGeoRef made";
    }

    if (idxOneOfFile_availableForCheckSame == -1) {
        qDebug() << __FUNCTION__ << " error: impossible case: idxOneOfFile_availableForCheckSame is -1";
        _imagesSetStatus_geoTFWData = e_iSS_geoTFWData_notAvailable_unknowError;
        return(_imagesSetStatus_geoTFWData);
    }

    _imagesSetStatus_geoTFWData = convert_iFSGeoTFWData_to_imagesSetStatusGeoTFWData(
        _qvectImageGeoRef[idxOneOfFile_availableForCheckSame].get_inputFileStatus_geoTFWData());

    _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = idxOneOfFile_availableForCheckSame;

    return(_imagesSetStatus_geoTFWData);
}

e_imagesSetStatus_geoTFWData GeoRefModel_imagesSet::convert_iFSGeoTFWData_to_imagesSetStatusGeoTFWData(
                    e_inputFileStatus_geoTFWData einputFileStatus_geoTFWData) const {

    switch(einputFileStatus_geoTFWData) {

        case e_iFS_geoTFWData_noneFound:
            return(e_iSS_geoTFWData_notAvailable_noneFound);

        case e_iFS_geoTFWData_notSetValue:
        case e_iFS_geoTFWData_oneFound:
        case e_iFS_geoTFWData_moreThanOneFound:
            return(e_iSS_geoTFWData_notAvailable_unknowError);

        case e_iFS_geoTFWData_errorWhileLoading:
            return(e_iSS_geoTFWData_notAvailable_errorWhileLoading);

        case e_iFS_geoTFWData_loaded:
            return(e_iSS_geoTFWData_available);

        case e_iFS_geoTFWData_loaded_butNotUsable:
            return(e_iSS_geoTFWData_notAvailable_loadedButNotUsable);
    }

    return(e_iSS_geoTFWData_notAvailable_unknowError);

}

#ifdef DEF_searchinCache
void GeoRefModel_imagesSet::setRootForEPSGCacheStorage(const QString& qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages) {
    _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages = qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages;
}
#endif

QString GeoRefModel_imagesSet::get_qstrEPSGCode() const {
    return(_qstrEPSGCode);
}

bool GeoRefModel_imagesSet::createProjContext(const QString& strPathProjDB) {
    _proj6crsTocrs.clear_PJ_PJContext();
    bool bReport = _proj6crsTocrs.createContext(strPathProjDB.toStdString().c_str());
    return(bReport);
}

bool GeoRefModel_imagesSet::createCrsToCrs_to_WGS84as4326() {
    qDebug() << __FUNCTION__;
    QString qstr_targetEPSGCode = "EPSG:4326";
    return(createCrsToCrs_to_specifiedTargetEPSGCode(qstr_targetEPSGCode));
}

bool GeoRefModel_imagesSet::createCrsToCrs_to_specifiedTargetEPSGCode(const QString& qstr_targetEPSGCode) {
    qDebug() << __FUNCTION__ << "qstr_targetEPSGCode:" << qstr_targetEPSGCode;

    int i_projCreateCrsToCrs_report = _proj6crsTocrs.create_crs_to_crs(
        _qstrEPSGCode.toStdString().c_str(),
        qstr_targetEPSGCode.toStdString().c_str(),
        true);

    if (i_projCreateCrsToCrs_report) {
        qDebug() << __FUNCTION__ << " : error: create_crs_to_crs failed with error code: " << i_projCreateCrsToCrs_report;
        return(false);
    }
    return(true);
}

e_imagesSetStatus_geoEPSG GeoRefModel_imagesSet::searchInCache_associatedEPSGCode() {

    _qstrEPSGCode.clear();

    _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.clear();
    _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_noneImage;

    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notAvailable_noneFound;

    int qvectImgGR_size = _qvectImageGeoRef.size();

    if (!qvectImgGR_size) {
        qDebug() << __FUNCTION__ << " error: none ImageGeoRef set";


        return(_imagesSetStatus_geoEPSG);
    }

#ifdef DEF_searchinCache
    if (_qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages.isEmpty()) {
        qDebug() << __FUNCTION__ << ": error: _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages is empty";
        _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;
        return(_imagesSetStatus_geoEPSG);
    }
 #endif

    QVector<QString> qvectStr_readEPSGCodeFromCache;

    for (int idx = 0; idx < qvectImgGR_size; idx++) {

#ifdef DEF_searchinCache

        QString strImagePathAndFilename_fromImageGeoRef = _qvectImageGeoRef[idx].getImagePathAndFilename();
        QString strImagePathAndFilename = getPathAndFilenameFromPotentialSymlink(strImagePathAndFilename_fromImageGeoRef);

        QFileInfo fileInfoFinalTarget(strImagePathAndFilename);
        QString qStrFinalFileTarget = fileInfoFinalTarget.fileName();

        QString qStrFileNameForEPSGCode = qStrFinalFileTarget + "_EPSG";

        QString qStrFullPathFileNameForEPSGCode =
                _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages
                + QDir::separator()
                + qStrFileNameForEPSGCode;

        qDebug() << __FUNCTION__ << strImagePathAndFilename_fromImageGeoRef << " =>"
                 << "qStrFullPathFileNameForEPSGCode = " << qStrFullPathFileNameForEPSGCode;
#endif

#ifdef DEF_searchinTheSameDirectoryOfTheInputFile

        //ex: /<possible_symbolic_link>/location/of/the/file.extension
        QString strImagePathAndFilename_fromImageGeoRef = _qvectImageGeoRef[idx].getImagePathAndFilename();
        QString strImagePathAndFilename = getPathAndFilenameFromPotentialSymlink(strImagePathAndFilename_fromImageGeoRef);
        //ex: /this/is/the/location/of/the/file.extension

        QFileInfo fileInfoFinalTarget(strImagePathAndFilename);
        QString qStrFileTarget = fileInfoFinalTarget.fileName();
        //ex: file.extension
        QString qstrImageExtension = fileInfoFinalTarget.suffix(); //the extension part after the last '.'
        int imageExtensionSize = qstrImageExtension.size();
        //ex: extension
        QString qstr_fileTarget_filename_noExtension = qStrFileTarget;
        qstr_fileTarget_filename_noExtension.chop(imageExtensionSize);
        //ex: file

        //special use cases about filename and extensions after chop:
        //filename with extension | filename no extension |  suffix   |  result wanted at the end
        //         file           |        file           |  <empty>  |    file.jepsg
        //        .file           |         .             |   file    |  __file.jepsg
        //         file.          |        file.          |  <emty>   |    file__.jepsg
        //        file..          |        file..         |  <empty>  |    file.__.jepsg

        int fileTarget_filename_noExtension_size = qstr_fileTarget_filename_noExtension.size();
        //remove last '.' if present:
        if (fileTarget_filename_noExtension_size) {
            if (qstr_fileTarget_filename_noExtension[fileTarget_filename_noExtension_size-1] == '.') {
                qstr_fileTarget_filename_noExtension.chop(1);
            }
        }

        QString strImagePath, strImageFilename;
        splitStrPathAndFile(strImagePathAndFilename, strImagePath, strImageFilename);
        //ex: /this/is/the/location/of/the

        QString qStrFileNameForEPSGCode = qstr_fileTarget_filename_noExtension + "_" + qstrImageExtension + ".jepsg";
        //ex: file--extension.jepsg

        QString qStrFullPathFileNameForEPSGCode =
                strImagePath
                + QDir::separator()
                + qStrFileNameForEPSGCode;
        //ex: /this/is/the/location/of/the/file--extension.jepsg

        qDebug() << __FUNCTION__ << strImagePathAndFilename_fromImageGeoRef << " =>"
                 << "qStrFullPathFileNameForEPSGCode = " << qStrFullPathFileNameForEPSGCode;
#endif

        if (!fileExists(qStrFullPathFileNameForEPSGCode)) {
            qDebug() << __FUNCTION__ << " : EPSG file infos not found";
            _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.push_back({strImagePathAndFilename, e_iSS_geoEPSG_notAvailable_noneFound, ""});
            continue;
        }

        IOJsonInfos ioInputImageEPSGCode;
        bool bRead = ioInputImageEPSGCode.loadFromFile(qStrFullPathFileNameForEPSGCode);
        if (!bRead) {
            qDebug() << __FUNCTION__ << ": error: getting EPSG infos from file: " << qStrFullPathFileNameForEPSGCode;
            _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.push_back({strImagePathAndFilename, e_iSS_geoEPSG_notAvailable_unknowError, ""});
            continue;
        }

        QVariantMap qvmapInputImageInfos = ioInputImageEPSGCode.toVariantMap();

        const QString strJsonKey_EPSGCode { "EPSG_code" };

        //check that the json have all the characteristics
        bool bContainsAll = true;
        bContainsAll &= qvmapInputImageInfos.contains(strJsonKey_EPSGCode);
        if (!bContainsAll) {
            qDebug() << __FUNCTION__ << " error: json does not contains all keys";
            _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.push_back({strImagePathAndFilename, e_iSS_geoEPSG_notAvailable_unknowError, ""});
            continue;
        }

        QVariant qvariantDefault = -888; //as invalid value
        QVariant qVarianEPSGCode = qvmapInputImageInfos.value(strJsonKey_EPSGCode, qvariantDefault);

        if (qVarianEPSGCode == qvariantDefault) {
            qDebug() << __FUNCTION__ << " error: qVarianEPSGCode value read error ";
            _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.push_back({strImagePathAndFilename, e_iSS_geoEPSG_notAvailable_unknowError, ""});
            continue;
        }

        QString qstrEPSGCodeFromVariant = qVarianEPSGCode.toString();
        qDebug() << __FUNCTION__ << "qstrEPSGCodeFromVariant =" << qstrEPSGCodeFromVariant;

        qvectStr_readEPSGCodeFromCache.push_back(qstrEPSGCodeFromVariant);

        _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.push_back({strImagePathAndFilename, e_iSS_geoEPSG_available, qstrEPSGCodeFromVariant});

    }

    //add extra details status about EPSG code found or not from cache
    //some cases will be probably merged in the user interface. Typically the cases about errors
    _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_notSetValue;

    int detailsVectSize = _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage.size();
    if (!detailsVectSize) {
        _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_noneImage;
    } else {
        int count_e_iSS_geoEPSG_notAvailable_noneFound = 0;
        int count_e_iSS_geoEPSG_notAvailable_unknowError = 0;
        int count_e_iSS_geoEPSG_available = 0;

        for (auto iter: _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
            switch (iter._eImagesSetStatus_geoEPSG) {
            case e_iSS_geoEPSG_notAvailable_noneFound:   count_e_iSS_geoEPSG_notAvailable_noneFound++; break;
            case e_iSS_geoEPSG_notAvailable_unknowError: count_e_iSS_geoEPSG_notAvailable_unknowError++; break;
            case e_iSS_geoEPSG_available:                count_e_iSS_geoEPSG_available++; break;

            //never used here:
            case e_iSS_geoEPSG_notSetValue:
            case e_iSS_geoEPSG_notAvailable_unsyncEPSGData:
                break;
            }
        }


        qDebug() << __FUNCTION__ << "count_e_iSS_geoEPSG_available = " << count_e_iSS_geoEPSG_available;
        qDebug() << __FUNCTION__ << "count_e_iSS_geoEPSG_notAvailable_noneFound = " << count_e_iSS_geoEPSG_notAvailable_noneFound;
        qDebug() << __FUNCTION__ << "count_e_iSS_geoEPSG_notAvailable_unknowError = " << count_e_iSS_geoEPSG_notAvailable_unknowError;
        qDebug() << __FUNCTION__ << "detailsVectSize = " << detailsVectSize;

        if (count_e_iSS_geoEPSG_available == detailsVectSize) { //all found
            _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_allFound;            
        } else {
            if (count_e_iSS_geoEPSG_notAvailable_noneFound == detailsVectSize) { //none found in cache (without errors: EPSG file not present in cache)
                _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_noneFound_noneReadError;
            } else {
                if (count_e_iSS_geoEPSG_notAvailable_unknowError) { //some read error from cache (EPSG file presents in cache but error reading)

                    if (count_e_iSS_geoEPSG_notAvailable_unknowError == detailsVectSize) { //all read error
                        _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_noneFound_allReadError;
                    } else {
                        if (count_e_iSS_geoEPSG_available) {
                            //here we do not take into account that some can be 'not found without error'
                            _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundWithSomeReadError;
                        } else {
                            //count_e_iSS_geoEPSG_notAvailable_noneFound should be different 0
                            _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_noneFound_someReadError;
                        }
                    }
                } else {
                    if (count_e_iSS_geoEPSG_available) {
                        //here we do not take into account that some can be 'not found without error'
                        _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_someFound_otherNotFoundButNoneReadError;
                    } else {
                    _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = e_DetailsAboutEPSGCFC_fTIS_noneFound_noneReadError;
                    }
                }
            }
        }
    }

    //-----
    qDebug() << __FUNCTION__ << "_qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage:";
    for (auto iter: _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
        qDebug() << __FUNCTION__ << iter._strImageFilename << " => ImagesSetStatus_geoEPSG: " << iter._eImagesSetStatus_geoEPSG <<  " ; EPSG string = " <<  iter._qstrEPSGCode;
    }
    qDebug() << __FUNCTION__ << " ==> _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet: " << _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;
    //----

    qDebug() << __FUNCTION__ << "qvectStr_readEPSGCodeFromCache:" << qvectStr_readEPSGCodeFromCache;

    int qvectStr_readEPSGCodeFromCache_size = qvectStr_readEPSGCodeFromCache.size();

    if (!qvectStr_readEPSGCodeFromCache_size) {


        return(_imagesSetStatus_geoEPSG);
    }

    if (qvectStr_readEPSGCodeFromCache_size == 1) {

        _qstrEPSGCode = qvectStr_readEPSGCodeFromCache[0];
        _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_available;
        return(_imagesSetStatus_geoEPSG);
    }

    bool bSomecheckSameMade = false;
    int idxOneOfEPSGCode_availableForCheckSame = -1;

    //test all combinations
    for (int idx1 = 0; idx1 < qvectStr_readEPSGCodeFromCache_size; idx1++) {
        idxOneOfEPSGCode_availableForCheckSame = idx1;
        for (int idx2 = idx1+1; idx2 < qvectStr_readEPSGCodeFromCache_size; idx2++) {
            qDebug() << __FUNCTION__ << " compare [idx1 = " << idx1 << " ] with [idx2 = " << idx2 << " ]";
            bool bAreSame = (0 == qvectStr_readEPSGCodeFromCache[idx1].compare(qvectStr_readEPSGCodeFromCache[idx2], Qt::CaseInsensitive));
            if (!bAreSame) {
                qDebug() << "=> not same";
                _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notAvailable_unsyncEPSGData;
                return(_imagesSetStatus_geoEPSG);
            }
            bSomecheckSameMade = true;
        }
    }

    if (bSomecheckSameMade) {
        qDebug() << __FUNCTION__ << " some comparison between ImageGeoRef made";
    }

    if (idxOneOfEPSGCode_availableForCheckSame == -1) {
        qDebug() << __FUNCTION__ << " error: impossible case: idxOneOfEPSGCode_availableForCheckSame is -1";
        _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;
        return(_imagesSetStatus_geoEPSG);
    }

    _qstrEPSGCode = qvectStr_readEPSGCodeFromCache[idxOneOfEPSGCode_availableForCheckSame];
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_available;

    qDebug() << "_qstrEPSGCode is set to: " << _qstrEPSGCode;
    qDebug() << "_imagesSetStatus_geoEPSG:" << _imagesSetStatus_geoEPSG;

    return(_imagesSetStatus_geoEPSG);
}

/*
bool GeoRefModel_imagesSet::projXY_fromQcs_toStrLonLat(const QPointF& sceneMousePosF_noZLI, QString& qstrLonlat) const {
    //qDebug() << __FUNCTION__ << "sceneMousePosF_noZLI = " << sceneMousePosF_noZLI;// << "fPixelValue=" << fPixelValue;
    S_lonlat lonlat;
    if (!projXY_fromQcs_toLonLat(sceneMousePosF_noZLI, lonlat)) {
        qstrLonlat = "..., ..."; //@#LP move to dedicated string
        return(false);
    } else {
        qstrLonlat = QString::number(lonlat._lon) + ", " +  QString::number(lonlat._lat); //static QString number(double, char f='g', int prec=6);
        return(true);
    }
}
*/

bool GeoRefModel_imagesSet::projXY_fromQcs_toStrLatLon(const QPointF& sceneMousePosF_noZLI, QString& strLatLon) const {
    //qDebug() << __FUNCTION__ << "sceneMousePosF_noZLI = " << sceneMousePosF_noZLI;// << "fPixelValue=" << fPixelValue;
    S_lonlat sLonlat;
    if (!projXY_fromQcs_toLonLat(sceneMousePosF_noZLI, sLonlat)) {
        strLatLon = "..., ..."; //@#LP move to dedicated string
        return(false);
    } else {
        strLatLon = QString::number(sLonlat._lat) + ", " +  QString::number(sLonlat._lon); //static QString number(double, char f='g', int prec=6);
        return(true);
    }
}


//from world file geo XY to targeted crs (lon, lat)
bool GeoRefModel_imagesSet::projXY_fromWF_toLonLat(const S_xy& geoXY, S_lonlat& sLonlat) const {
    //qDebug() << __FUNCTION__;

    PJ_COORD c_in_xy_1 = set_PJ_COORD_withXY(geoXY);

    PJ_COORD c_out_1to2;
    bool bReport = _proj6crsTocrs.trans(c_in_xy_1, PJ_FWD, c_out_1to2);
    if (!bReport) {
        qDebug() << __FUNCTION__ << " fail in proj6crsTocrs.trans";
        return(false);
    }

    sLonlat._lon = lonOf(c_out_1to2);
    sLonlat._lat = latOf(c_out_1to2);

    //qDebug() << __FUNCTION__ << "lon: " << lonlat._lon;
    //qDebug() << __FUNCTION__ << "lat: " << lonlat._lat;

    return(true);
}

bool GeoRefModel_imagesSet::projXY_fromLonLat_toWF(const S_lonlat& lonlat, S_xy& geoXY) const {

    qDebug() << __FUNCTION__;

    PJ_COORD c_in_lonlat_1 = set_PJ_COORD_withLonLat(lonlat._lon, lonlat._lat);

    PJ_COORD c_out_1to2;
    bool bReport = _proj6crsTocrs.trans(c_in_lonlat_1, PJ_INV, c_out_1to2);
    if (!bReport) {
        qDebug() << __FUNCTION__ << " fail in proj6crsTocrs.trans";
        return(false);
    }

    geoXY._x = xOf(c_out_1to2);
    geoXY._y = yOf(c_out_1to2);

    qDebug() << __FUNCTION__ << "geoXY _x, _y: " << geoXY._x << ", " << geoXY._y;

    return(true);
}

//from world file geo XY to targeted crs (lon, lat)
bool GeoRefModel_imagesSet::projXY_fromWF_toWF(const S_xy& geoXYsrc, S_xy& geoXYdst) const {
    qDebug() << __FUNCTION__;

    PJ_COORD c_in_xy_1 = set_PJ_COORD_withXY(geoXYsrc);

    PJ_COORD c_out_1to2;
    bool bReport = _proj6crsTocrs.trans(c_in_xy_1, PJ_FWD, c_out_1to2);
    if (!bReport) {
        qDebug() << __FUNCTION__ << " fail in proj6crsTocrs.trans";
        return(false);
    }

    geoXYdst._x = xOf(c_out_1to2);
    geoXYdst._y = yOf(c_out_1to2);

    qDebug() << __FUNCTION__ << "geoXYdst._x: " << geoXYdst._x;
    qDebug() << __FUNCTION__ << "geoXYdst._y: " << geoXYdst._y;

    return(true);
}

QString GeoRefModel_imagesSet::qstrFor_outOfImage() const {
    qDebug() << __FUNCTION__ << "";
    return("___, ___");//@#LP move to dedicated string
}

bool GeoRefModel_imagesSet::convertXY_fromQcs_toWF(S_xy qcsXY, S_xy& geoXY) const {
    if (_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available) {
        qDebug() << __FUNCTION__ << "_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available";
        return(false);
    }
    if (_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection == -1) {
        return(false);
    }
    return(_qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].
            convertXY_fromQcs_toWF(qcsXY, geoXY));
}

bool GeoRefModel_imagesSet::convertXY_fromWF_toQcs(S_xy geoXY, S_xy& qcsXY) const {
    if (_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available) {
        qDebug() << __FUNCTION__ << "_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available";
        return(false);
    }
    if (_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection == -1) {
        return(false);
    }
    return(_qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].
            convertXY_fromWF_toQcs(geoXY, qcsXY));
}


bool GeoRefModel_imagesSet::projXY_fromQcs_toLonLat(const QPointF& sceneMousePosF_noZLI, S_lonlat& sLonlat) const {

    if (_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available) {
        qDebug() << __FUNCTION__ << "_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available";
        return(false);
    }

    S_xy xy_qcs_src { sceneMousePosF_noZLI.x(), sceneMousePosF_noZLI.y() };
    S_xy xy_wf_src { .0, .0 };

    bool bReport = _qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].
            convertXY_fromQcs_toWF(xy_qcs_src, xy_wf_src);
    if (!bReport) {
        return(false);
    }

    //qDebug() << __FUNCTION__ << "xy_qcs_src:" << xy_qcs_src._x << ", " << xy_qcs_src._y;
    //qDebug() << __FUNCTION__ << "xy_wf_src :" << xy_wf_src._x  << ", " << xy_wf_src._y;

    return(projXY_fromWF_toLonLat(xy_wf_src, sLonlat));
}

void GeoRefModel_imagesSet::sendStrWorldFileDataToView() {
    qDebug() << __FUNCTION__;
    QVector<QString> qvectStr_worldFileData {"","","","","",""};
    if (_imagesSetStatus_geoTFWData != e_iSS_geoTFWData_available) {
        qDebug() << __FUNCTION__ << " emit now signal_strWorldFileData with: " << qvectStr_worldFileData;
        emit signal_strWorldFileData(qvectStr_worldFileData);
        return;
    }
    qDebug() << __FUNCTION__ << " _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection = " << _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection;

    qvectStr_worldFileData = _qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].getqvectstr_worldFileParameters();
    qDebug() << __FUNCTION__ << " emit now signal_strWorldFileData with: " << qvectStr_worldFileData;
    emit signal_strWorldFileData(qvectStr_worldFileData);
}

void GeoRefModel_imagesSet::sendImagesFilenameToView() {

    qDebug() << __FUNCTION__;

    int vectDetails_size = _qvectImageGeoRef.size();
    if (!vectDetails_size) {
        emit signal_imagesFilename({"","",""});
        return;
    }
    if (vectDetails_size > 3) {
        emit signal_imagesFilename({"Dev warning: sending more than three files",
                                    "is not handled by GeoRefModel_imagesSet",
                                    "( sendImagesFilenameToView() )"});
        return;
    }
    QVector<QString> qvectStr_ImagesFilename;
    for (auto iter: _qvectImageGeoRef) {
        qvectStr_ImagesFilename.push_back(iter.getImagePathAndFilename());
    }

    qDebug() << __FUNCTION__ << "qvectStr_ImagesFilename = " << qvectStr_ImagesFilename;

    emit signal_imagesFilename(qvectStr_ImagesFilename);
}


void GeoRefModel_imagesSet::sendEPSGCodeToView() {
    qDebug() << __FUNCTION__;
    if (_imagesSetStatus_geoEPSG != e_iSS_geoEPSG_available) {
        qDebug() << __FUNCTION__ << " emit now signal_strEPSGCode with empty string";
        emit signal_strEPSGCode("");
        return;
    }
    qDebug() << __FUNCTION__ << " emit now signal_strEPSGCode with: " << _qstrEPSGCode;

    QString qstr_EPSGCodeValue_startWithTag = {"EPSG:"};
    QString qstr_EPSGCodeValue_noTag = _qstrEPSGCode; //contains tag
    if (qstr_EPSGCodeValue_noTag.startsWith(qstr_EPSGCodeValue_startWithTag)) {
        qstr_EPSGCodeValue_noTag = qstr_EPSGCodeValue_noTag.mid(qstr_EPSGCodeValue_startWithTag.size());
    }
    emit signal_strEPSGCode(qstr_EPSGCodeValue_noTag);  
}

QVector<QString> GeoRefModel_imagesSet::get_qvectOfStrImagePathAndFilenameSet() {
    QVector<QString> qvectOfstrImagePathAndFilenameSet;
    for(const auto& iter: _qvectImageGeoRef) {
        qvectOfstrImagePathAndFilenameSet.push_back(iter.getImagePathAndFilename());
    }

    qDebug() << __FUNCTION__ << " qvectOfstrImagePathAndFilenameSet: " << qvectOfstrImagePathAndFilenameSet;
    return(qvectOfstrImagePathAndFilenameSet);
}

e_imagesSetStatus_geoEPSG GeoRefModel_imagesSet::forceEPSGCode(const QString& qstrEPSGCode) {
    QString qstrHeaderEPSG = "EPSG:";

    QString qstrEPSGCode_noHeader = qstrEPSGCode;
    if (qstrEPSGCode_noHeader.startsWith(qstrHeaderEPSG)) {
        qstrEPSGCode_noHeader = qstrEPSGCode_noHeader.remove(qstrHeaderEPSG);
    }

    qDebug() << __FUNCTION__ << "reveived qstrEPSGCode = " << qstrEPSGCode;
    qDebug() << __FUNCTION__ << "qstrEPSGCode_noHeader = " << qstrEPSGCode_noHeader;

    _qstrEPSGCode.clear();
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;

    int qvectImgGR_size = _qvectImageGeoRef.size();

    if (!qvectImgGR_size) {
        qDebug() << __FUNCTION__ << " error: none ImageGeoRef set";
        return(_imagesSetStatus_geoEPSG);
    }

    _qstrEPSGCode = qstrHeaderEPSG + qstrEPSGCode_noHeader;
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_available;
    return(_imagesSetStatus_geoEPSG);
}


//here the qstrEPSGCode method argument does not contains the starting "EPSG:"; it will be added in this method
e_imagesSetStatus_geoEPSG GeoRefModel_imagesSet::setEPSGCode_and_storeInCacheForAllImagesSet(const QString& qstrEPSGCode) {

    qDebug() << __FUNCTION__;

    _qstrEPSGCode.clear();
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_notAvailable_unknowError;

    int qvectImgGR_size = _qvectImageGeoRef.size();

    if (!qvectImgGR_size) {
        qDebug() << __FUNCTION__ << " error: none ImageGeoRef set";


        return(_imagesSetStatus_geoEPSG);
    }

    _qstrEPSGCode = "EPSG:" + qstrEPSGCode;
    _imagesSetStatus_geoEPSG = e_iSS_geoEPSG_available;



    bool bNoneWriteError = true;

    for (int idx = 0; idx < qvectImgGR_size; idx++) {
        qDebug() << __FUNCTION__ << " idx: " << idx << "_qvectImageGeoRef[idx].getImagePathAndFilename() = " << _qvectImageGeoRef[idx].getImagePathAndFilename();
        bNoneWriteError &= storeInCache_EPSGCode_forImage(_qvectImageGeoRef[idx].getImagePathAndFilename());
    }

    if (!bNoneWriteError) {
        qDebug() << __FUNCTION__ << ": warning: some write operation in cache about EPSG code failed";
    }

    return(_imagesSetStatus_geoEPSG);
}


bool GeoRefModel_imagesSet::storeInCache_EPSGCode_forImage(const QString& strImageFilename) {

    qDebug() << __FUNCTION__;

#ifdef DEF_searchinCache
    if (_qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages.isEmpty()) {
        qDebug() << __FUNCTION__ << ": error: _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages is empty";
        return(false);
    }

    QString strImagePathAndFilename= getPathAndFilenameFromPotentialSymlink(strImageFilename);

    QFileInfo fileInfoFinalTarget(strImagePathAndFilename);
    QString qStrFinalFileTarget = fileInfoFinalTarget.fileName();

    QString qStrFileNameForEPSGCode = qStrFinalFileTarget + "_EPSG";

    QString qStrFullPathFileNameForEPSGCode =
            _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages
            + QDir::separator()
            + qStrFileNameForEPSGCode;

    qDebug() << __FUNCTION__ << strImageFilename << " =>"
             << "qStrFullPathFileNameForEPSGCode = " << qStrFullPathFileNameForEPSGCode;
#endif

#ifdef DEF_searchinTheSameDirectoryOfTheInputFile

        //ex: /<possible_symbolic_link>/location/of/the/file.extension
        QString strImagePathAndFilename = getPathAndFilenameFromPotentialSymlink(strImageFilename);
        //ex: /this/is/the/location/of/the/file.extension

        QFileInfo fileInfoFinalTarget(strImagePathAndFilename);
        QString qStrFileTarget = fileInfoFinalTarget.fileName();
        //ex: file.extension
        QString qstrImageExtension = fileInfoFinalTarget.suffix(); //the extension part after the last '.'
        int imageExtensionSize = qstrImageExtension.size();
        //ex: extension
        QString qstr_fileTarget_filename_noExtension = qStrFileTarget;
        qstr_fileTarget_filename_noExtension.chop(imageExtensionSize);
        //ex: file

        //special use cases about filename and extensions after chop:
        //filename with extension | filename no extension |  suffix   |  result wanted at the end
        //         file           |        file           |  <empty>  |    file.jepsg
        //        .file           |         .             |   file    |  __file.jepsg
        //         file.          |        file.          |  <emty>   |    file__.jepsg
        //        file..          |        file..         |  <empty>  |    file.__.jepsg

        int fileTarget_filename_noExtension_size = qstr_fileTarget_filename_noExtension.size();
        //remove last '.' if present:
        if (fileTarget_filename_noExtension_size) {
            if (qstr_fileTarget_filename_noExtension[fileTarget_filename_noExtension_size-1] == '.') {
                qstr_fileTarget_filename_noExtension.chop(1);
            }
        }

        QString strImagePath, strImageFilename_notUsed;
        splitStrPathAndFile(strImagePathAndFilename, strImagePath, strImageFilename_notUsed);
        //ex: /this/is/the/location/of/the

        QString qStrFileNameForEPSGCode = qstr_fileTarget_filename_noExtension + "_" + qstrImageExtension + ".jepsg";
        //ex: file--extension.jepsg

        QString qStrFullPathFileNameForEPSGCode =
                strImagePath
                + QDir::separator()
                + qStrFileNameForEPSGCode;
        //ex: /this/is/the/location/of/the/file--extension.jepsg

        qDebug() << __FUNCTION__ << strImageFilename << " =>"
                 << "qStrFullPathFileNameForEPSGCode = " << qStrFullPathFileNameForEPSGCode;
#endif

    const QString strJsonKey_EPSGCode { "EPSG_code" };
    IOJsonInfos ioInputImageEPSGCode;
    QJsonObject qjsonObj_EPSGCode;
    qjsonObj_EPSGCode.insert(strJsonKey_EPSGCode, _qstrEPSGCode);
    ioInputImageEPSGCode.setQJsonObject(qjsonObj_EPSGCode);

    bool bWrite = ioInputImageEPSGCode.saveToFile(qStrFullPathFileNameForEPSGCode);
    if (!bWrite) {
        qDebug() << __FUNCTION__ << ": error: writing EPSG infos to file: " << qStrFullPathFileNameForEPSGCode;
    }

    return(bWrite);
}

bool GeoRefModel_imagesSet::storeInCache_EPSGCode_forImagesWhichDoNotHave() {

    qDebug() << __FUNCTION__ << ": entering";

#ifdef DEF_searchinCache
    if (_qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages.isEmpty()) {
        qDebug() << __FUNCTION__ << ": error: _qstrDirDiskCacheStorageAboutEPSGCodesAssociatedToImages is empty";
        return(false);
    }
#endif

    bool bWrite = true;
    for (auto iter: _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage) {
        qDebug() << __FUNCTION__ << ": iter: " << iter._strImageFilename;
        qDebug() << __FUNCTION__ << ": iter._eImagesSetStatus_geoEPSG: " << iter._eImagesSetStatus_geoEPSG;

        if (iter._eImagesSetStatus_geoEPSG == e_iSS_geoEPSG_notAvailable_noneFound) {
           bWrite &= storeInCache_EPSGCode_forImage(iter._strImageFilename);
        }
    }

    qDebug() << __FUNCTION__ << ": exiting with bWrite = " << bWrite;

    return(bWrite);
}

void GeoRefModel_imagesSet::get_detailsAboutEPSGCodeFoundFromCacheForTheImageSet(
        QVector<S_DetailsAboutEPSGCodeFoundFromCacheForOneImage>& qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage,
        e_DetailsAboutEPSGCodeFoundFromCache_forTheImageSet& eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet) const {
    qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage = _qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage;
    eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet = _eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;

    //qDebug() << __FUNCTION__ << ": qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage = " << qvectSDetailsAboutEPSGCodeFoundFromCache_aboutEachImage;
    qDebug() << __FUNCTION__ << ": eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet =" << eDetailsAboutEPSGCodeFoundFromCache_forTheImageSet;
}

bool GeoRefModel_imagesSet::getImageSizeF(QSizeF& sizef) const { //all images have the same w h, uses  _idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection to get size
    if (!checkOneAvailable()) {
        return(false);
    }
    return(_qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].getImageSizeF(sizef));
}

bool GeoRefModel_imagesSet::get_xScale(double &xScale) const {
    if (!checkOneAvailable()) {
        return(false);
    }
    return(_qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].get_xScale(xScale));
}

bool GeoRefModel_imagesSet::get_yScale(double &yScale) const {
    if (!checkOneAvailable()) {
        return(false);
    }
    return(_qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].get_yScale(yScale));
}


bool GeoRefModel_imagesSet::checkOneAvailable() const {
    if (!imageCount()) {
        return(false);
    }
    if (_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection == -1) {
        return(false);
    }
    return(true);
}

bool GeoRefModel_imagesSet::convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
    qreal pixel_distanceFromStartingRoutePointToRoutePoint,
    qreal& WF_distanceFromStartingRoutePointToRoutePoint) const {

    WF_distanceFromStartingRoutePointToRoutePoint = .0;

    if (_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection == -1) {
        return(false);
    }
    return(_qvectImageGeoRef[_idxOfOneOfImageGeoRefWithAvailableTFWData_forProjection].
            convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
               pixel_distanceFromStartingRoutePointToRoutePoint,
               WF_distanceFromStartingRoutePointToRoutePoint));
}


