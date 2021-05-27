#include <QString>

#include "WorldFile_parameters.h"

#include "../toolbox/toolbox_pathAndFile.h"

#include <OpenImageIO/imageio.h>

#include "ImageGeoRef.h"

using namespace OIIO;

#include <QDebug>

#include "toolbox/toolbox_conversion.h"

ImageGeoRef::ImageGeoRef(QString qstrImagePathAndFilename):
    _qstrImagePathAndFilename(qstrImagePathAndFilename),
    _imageWidth {.0},
    _imageHeight {.0},
    _bImageSizeGot {false},
    _inputFileStatus_geoTFWData(e_iFS_geoTFWData_notSetValue),
    _upperLeftImageCorner {.0, .0},
    _bUpperLeftImageCornerComputed {false} {
}

ImageGeoRef::ImageGeoRef():ImageGeoRef("") {

}

void ImageGeoRef::clear() {
    _qstrImagePathAndFilename.clear();
    _imageWidth = 0;
    _imageHeight = 0;
    _bImageSizeGot = false;
    _inputFileStatus_geoTFWData = e_iFS_geoTFWData_notSetValue;
    _upperLeftImageCorner = {0, 0};
    _bUpperLeftImageCornerComputed = false;
}

bool ImageGeoRef::toQJsonObject(QJsonObject& qjsonObj) const {

    if (_inputFileStatus_geoTFWData != e_iFS_geoTFWData_loaded) {
        qDebug() << __FUNCTION__ << " error inputFileStatus_geoTFWData is not e_iFS_geoTFWData_loaded";
        return(false);
    }   

    QString qstrAbsolutePathOnly;
    QString qstrFilenameOnly;

    splitStrPathAndFile(_qstrImagePathAndFilename, qstrAbsolutePathOnly, qstrFilenameOnly);

    qstrAbsolutePathOnly = replaceDirSeparatorBySlash(qstrAbsolutePathOnly);

    //filename and absolutePath splitted in json file for convenience (can be more human edition friendly)
    QJsonObject qjsonObj_pathAndFilename;
    qjsonObj_pathAndFilename.insert("filename"    , qstrFilenameOnly);
    qjsonObj_pathAndFilename.insert("absolutePath", qstrAbsolutePathOnly);

    QString str_pathAndFilename { "pathAndFilename" };
    qjsonObj.insert(str_pathAndFilename, qjsonObj_pathAndFilename);

    //WFparameters
    QJsonObject qjsonObjWFparameters;
    /*bool bReport = */_worldFileParameters.toQJsonObject(qjsonObjWFparameters);

    QString str_worldFileContent { "worldFileContent" };
    qjsonObj.insert(str_worldFileContent, qjsonObjWFparameters);

    return(true);
}


bool ImageGeoRef::fromQJsonObject(const QJsonObject& qjsonObj) {

    //avoid usage of not fresh ImageGeoRef instance
    if (_inputFileStatus_geoTFWData != e_iFS_geoTFWData_notSetValue) {
        qDebug() << __FUNCTION__ << " error _inputFileStatus_geoTFWData is not e_iFS_geoTFWData_notSetValue";
        return(false);
    }

    _inputFileStatus_geoTFWData = e_iFS_geoTFWData_errorWhileLoading; //@#LP _inputFileStatus_geoTFWData re-used for json read file status

    //filename and absolutePath splitted in json file for convenience (can be more human edition friendly)
    QString str_key_pathAndFilename { "pathAndFilename" };


    if (!qjsonObj.contains(str_key_pathAndFilename)) {
        qDebug() << __FUNCTION__ << " error: key not found: " << str_key_pathAndFilename;
        return(false);
    }
    QJsonObject qjsonObj_pathFilename = qjsonObj.value(str_key_pathAndFilename).toObject();

    QString str_key_path { "absolutePath" };
    if (!qjsonObj_pathFilename.contains(str_key_path)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << str_key_path;
        return(false);
    }
    QString qstrAbsolutePathOnly = qjsonObj_pathFilename.value(str_key_path).toString();

    QString str_key_filename { "filename" };


    if (!qjsonObj_pathFilename.contains(str_key_filename)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << str_key_filename;
        return(false);
    }
    QString qstrFilenameOnly = qjsonObj_pathFilename.value(str_key_filename).toString();


    QString str_key_worldFileContent { "worldFileContent" };
    if (!qjsonObj.contains(str_key_worldFileContent)) {
        qDebug() << __FUNCTION__ <<  ": error: key not found: " << str_key_worldFileContent;
        return(false);
    }
    QJsonObject qjsonObj_worldFileContent = qjsonObj.value(str_key_worldFileContent).toObject();

    //WFparameters
    bool bReport = _worldFileParameters.fromQJsonObject(qjsonObj_worldFileContent);
    if (!bReport) {
        return(false);
    }

    qstrAbsolutePathOnly = replaceSlashByDirSeparator(qstrAbsolutePathOnly);
    _qstrImagePathAndFilename = concatStrPathAndFile(qstrAbsolutePathOnly, qstrFilenameOnly);

    if (!computeImageCornersInGeoXY()) {
        _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_loaded_butNotUsable;
        qDebug() << __FUNCTION__ << " error in computeImageCornersInGeoXY() ";
        return(false);
    }

    _inputFileStatus_geoTFWData = e_iFS_geoTFWData_loaded;
    return(bReport);
}

QVector<QString> ImageGeoRef::getqvectstr_worldFileParameters() {

    QVector<QString> qvectstr_worldFileParameters {"","","","","",""};
    const U_WFparameters& WFparametersref = _worldFileParameters.getConstRef_WFparameters();
    if (!_worldFileParameters.canBeUsed()) {
        return(qvectstr_worldFileParameters);
    }
    qvectstr_worldFileParameters.clear();
    for (int i = 0; i < 6; i++) {
        qvectstr_worldFileParameters.push_back(doubleToQStringPrecision_justFit(WFparametersref._raw._doubleArray[i], 15));

        qDebug() << __FUNCTION__ << "WFparametersref._raw._doubleArray[" << i << "] = " << WFparametersref._raw._doubleArray[i];
        qDebug() << __FUNCTION__ << "QString::number(WFparametersref._raw._doubleArray[i]) = " << QString::number(WFparametersref._raw._doubleArray[i]);
    }

    qDebug() << __FUNCTION__ << " qvectstr_worldFileParameters =" << qvectstr_worldFileParameters;

    return(qvectstr_worldFileParameters);
}

QString ImageGeoRef::getImagePathAndFilename() const {
    return(_qstrImagePathAndFilename);
}

e_inputFileStatus_geoTFWData ImageGeoRef::get_inputFileStatus_geoTFWData() const {
    return(_inputFileStatus_geoTFWData);
}

//return false if no TFW found or if computeImageCornersInGeoXY failed

bool ImageGeoRef::setAndLoad_imageWidthHeight_TFWDataIfAvailable(bool& bLoaded_butNotUsable) {

    bLoaded_butNotUsable = false;

    bool bLoadImageSize = loadImageWidthHeightFromImageFile();
    if (!bLoadImageSize) {
        qDebug() << __FUNCTION__ << " minor warning: fail in loadImageWidthHeight()";
        qDebug() << __FUNCTION__ << "                Image corners in GeoXY will not be computed";
    }
    bool bOnlyOneFound = false;
    bool bFound = findWorldFilesForImage(bOnlyOneFound);
    if (!bFound) {
        _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_noneFound;
        qDebug() << __FUNCTION__ <<" if (!bFound) {";
        return(false);

    } else {
        if (bOnlyOneFound) {
            _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_oneFound;
        } else {
            _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_moreThanOneFound;
            qDebug() << __FUNCTION__ <<" (!bOnlyOneFound) {";
        }
        bool bLoaded = loadWorldFileFoundForImage();
        if (bLoaded) {
            _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_loaded;
        } else {
            _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_errorWhileLoading;
            qDebug() << __FUNCTION__ << " if (!bLoaded) (fail on loadWorldFileFoundForImage)";
            return(false);
        }        

        //_worldFileParameters.canBeUsed() checked in computeImageCornersInGeoXY =>
        bLoaded_butNotUsable = !computeImageCornersInGeoXY();
        if (bLoaded_butNotUsable) {
            _inputFileStatus_geoTFWData = e_inputFileStatus_geoTFWData::e_iFS_geoTFWData_loaded_butNotUsable;
            qDebug() << __FUNCTION__ << " error in computeImageCornersInGeoXY() ";
            return(false);
        }
    }
    return(true);
}

bool ImageGeoRef::loadImageWidthHeightFromImageFile() {
    _bImageSizeGot = false;
    if (_qstrImagePathAndFilename.isEmpty()) {
        return(_bImageSizeGot);
    }
    auto in = ImageInput::open(_qstrImagePathAndFilename.toStdString());
    if (!in) {
        qDebug() << __FUNCTION__ << ": error: can not open for read; file " << _qstrImagePathAndFilename;
        return(_bImageSizeGot);
    }
    _imageWidth  = in->spec().width;
    _imageHeight = in->spec().height;

    qDebug() << __FUNCTION__ << "got: _imageWidth  = " << _imageWidth;
    qDebug() << __FUNCTION__ << "got: _imageHeight = " << _imageHeight;

    _bImageSizeGot = true;

    return(_bImageSizeGot);
}

//return false if none tfw file found, return true if one or more tfw file found.
bool ImageGeoRef::findWorldFilesForImage(bool& bOnlyOneFound) {
    _qvectqstrWFCandidate.clear();
    _qstrWordlFile_pathAndFilenameUsed.clear();

    if (_qstrImagePathAndFilename.isEmpty()) {
        return(false);
    }
    if (!_worldFileParameters.findWorldFilesFromAssociatedImagePathAndFilename(_qstrImagePathAndFilename, _qvectqstrWFCandidate, _qstrWordlFile_pathAndFilenameUsed)) {
        return(false);
    }
    if (!_qstrWordlFile_pathAndFilenameUsed.isEmpty()) { //one found and ready to be loaded
        bOnlyOneFound = true;
    }
    return(true);
}

bool ImageGeoRef::loadWorldFileFoundForImage() {
    if (_qstrWordlFile_pathAndFilenameUsed.isEmpty()) { //no tfw set
        qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_qstrWordlFile_pathAndFilenameUsed is empty";
        return(false);
    }
    bool bLoaded = _worldFileParameters.loadFromFile(_qstrWordlFile_pathAndFilenameUsed); //preset file {
    return(bLoaded);




}

void ImageGeoRef::showContent() const {
    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_qstrImagePathAndFilename =" << _qstrImagePathAndFilename;
    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_bImageSizeGot =" << _bImageSizeGot;
    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "  _imageWidth = "  << _imageWidth;
    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "  _imageHeight = "  << _imageHeight;

    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_inputFileStatus_geoTFWData = "  << _inputFileStatus_geoTFWData;
    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_worldFileParameters.content():";
    _worldFileParameters.showContent();

    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_qstrWordlFile_pathAndFilenameUsed = "  << _qstrWordlFile_pathAndFilenameUsed;

    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_bUpperLeftImageCornerComputed = " << _bUpperLeftImageCornerComputed;
    qDebug() << "ImageGeoRef::" << __FUNCTION__ << "  _upperLeftImageCorner (x, y):" <<  _upperLeftImageCorner._x << ", " << _upperLeftImageCorner._y;

    if (_qvectqstrWFCandidate.isEmpty()) {
        qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_qvectqstrWFCandidate is empty";
    } else {
        int i = 0;
        for(const auto& iter: _qvectqstrWFCandidate) {
            qDebug() << "ImageGeoRef::" << __FUNCTION__ << "_qvectqstrWFCandidate[i] =" << iter;
            i++;
        }
    }
}

bool ImageGeoRef::geoImageXYComputable() const {
    qDebug() << __FUNCTION__;
    return(_worldFileParameters.canBeUsed() && _bUpperLeftImageCornerComputed);

}

bool ImageGeoRef::computeImageCornersInGeoXY() {

    qDebug() << __FUNCTION__;

    _bUpperLeftImageCornerComputed = false;

    if ( !_worldFileParameters.canBeUsed()) {

        qDebug() << __FUNCTION__ << "!_worldFileParameters.canBeUsed()";
        return(false);
    }

    const U_WFparameters& TFWparameters = _worldFileParameters.getConstRef_WFparameters();


    S_xy centerOfUpperLeftPixel {
        TFWparameters._parameters._xTranslationTerm_centerOfUpperLeftPixel,
        TFWparameters._parameters._yTranslationTerm_centerOfUpperLeftPixel}; //from tfw: C,F



    //from tfw, but could be taken from the tiff header also (TBC)
    double xPixelDimension = TFWparameters._parameters._xScale;
    double yPixelDimension = TFWparameters._parameters._yScale;

    qDebug() << __FUNCTION__ << "xPixelDimension: " << xPixelDimension;
    qDebug() << __FUNCTION__ << "yPixelDimension: " << yPixelDimension;





    _upperLeftImageCorner  = { centerOfUpperLeftPixel._x - xPixelDimension/2.0,
                               centerOfUpperLeftPixel._y - yPixelDimension/2.0 };

    _bUpperLeftImageCornerComputed = true;

    if (_bImageSizeGot) {

        S_xy lowerRightImageCorner { _upperLeftImageCorner._x + _imageWidth  * xPixelDimension,
                                     _upperLeftImageCorner._y + _imageHeight * yPixelDimension };

        S_xy centerOfLowerRightPixel { lowerRightImageCorner._x - xPixelDimension/2.0,
                                       lowerRightImageCorner._y - yPixelDimension/2.0 };

        qDebug() << __FUNCTION__ << "upperLeftImageCorner :" << _upperLeftImageCorner._x << ", " << _upperLeftImageCorner._y;
        qDebug() << __FUNCTION__ << "lowerRightImageCorner:" << lowerRightImageCorner._x << ", " << lowerRightImageCorner._y;

        qDebug() << __FUNCTION__ << "centerUpperLeftPixel : " <<  centerOfUpperLeftPixel._x << ", " <<  centerOfUpperLeftPixel._y;
        qDebug() << __FUNCTION__ << "centerLowerRightPixel: " << centerOfLowerRightPixel._x << ", " << centerOfLowerRightPixel._y;

    } else {
        qDebug() << __FUNCTION__ << "upperLeftImageCorner :" << _upperLeftImageCorner._x << ", " << _upperLeftImageCorner._y;
    }

    return(true);
}

bool ImageGeoRef::convertXY_fromQcs_toWF(S_xy qcsXY, S_xy& geoXY) const {

    if (!_bUpperLeftImageCornerComputed) {
        qDebug() << __FUNCTION__ << " if (!_bUpperLeftImageCornerComputed) {";
        return(false);
    }

    //multiply by pixel dimension to match with geo XY size, with y sign set for the next step
    geoXY._x = qcsXY._x * _worldFileParameters.getConstRef_WFparameters()._parameters._xScale;
    geoXY._y = qcsXY._y * _worldFileParameters.getConstRef_WFparameters()._parameters._yScale;

    qDebug() << __FUNCTION__ << "_worldFileParameters.getConstRef_WFparameters()._parameters._xScale = " << _worldFileParameters.getConstRef_WFparameters()._parameters._xScale;
    qDebug() << __FUNCTION__ << "_worldFileParameters.getConstRef_WFparameters()._parameters._yScale = " << _worldFileParameters.getConstRef_WFparameters()._parameters._yScale;

    qDebug() << __FUNCTION__ << "GeoXY image coordinate after * scale: " << geoXY._x << ", " << geoXY._y;

    qDebug() << __FUNCTION__ << "   _upperLeftImageCorner._x: " << _upperLeftImageCorner._x;
    qDebug() << __FUNCTION__ << "   _upperLeftImageCorner._y: " << _upperLeftImageCorner._y;

    //convert in geo XY coordinates:
    geoXY._x += _upperLeftImageCorner._x;
    geoXY._y += _upperLeftImageCorner._y;

    qDebug() << __FUNCTION__ << "GeoXY image coordinate: " << geoXY._x << ", " << geoXY._y;
    //fprintf(stdout, "GeoXY image coordinate: %f %f\n", geoXY._x, geoXY._y);


    return(true);

}


//considers here as a fail if xcale != yscale
bool ImageGeoRef::convertDistance_fromQcs_toWF_withSameXScaleAndYScale(
    qreal pixel_distanceFromStartingRoutePointToRoutePoint,
    qreal& WF_distanceFromStartingRoutePointToRoutePoint) const {

    //considers here as a fail if xcale != yscale
    if (!_worldFileParameters.canBeUsed()) {
        return(false);
    }

    double xScale = .0;
    bool bXScaleGot = get_xScale(xScale);
    if (!bXScaleGot) {
        return(false);
    }

    WF_distanceFromStartingRoutePointToRoutePoint =
        pixel_distanceFromStartingRoutePointToRoutePoint *= xScale;

    return(true);
}



bool ImageGeoRef::convertXY_fromWF_toQcs(S_xy geoXY, S_xy& qcsXY) const {

    if (!_bUpperLeftImageCornerComputed) {
        qDebug() << __FUNCTION__ << " if (!_bUpperLeftImageCornerComputed) {";
        return(false);
    }

    qDebug() << __FUNCTION__ << "step #1: image coordinate building: _upperLeftImageCorner x,y: " << _upperLeftImageCorner._x << ", " << _upperLeftImageCorner._y;
    //fprintf(stdout, "step #1: image coordinate building: _upperLeftImageCorner x,y:  %f %f\n", _upperLeftImageCorner._x, _upperLeftImageCorner._y);

    //convert in image coordinates: (find deltas to qcs left top)
    qcsXY._x = geoXY._x - _upperLeftImageCorner._x;
    qcsXY._y = geoXY._y - _upperLeftImageCorner._y;

    //fprintf(stdout, "step #1: image coordinate building:\n x-upperleft: %f\n y-upperleft: %f\n", qcsXY._x, qcsXY._y);

    qDebug() << __FUNCTION__ << "step #1: image coordinate building: x-upperleft, y-upperleft: " << qcsXY._x << ", " << qcsXY._y;

    //divide by pixel dimension to match with image pixel size and adjust the sign for y
    qcsXY._x/=_worldFileParameters.getConstRef_WFparameters()._parameters._xScale;
    qcsXY._y/=_worldFileParameters.getConstRef_WFparameters()._parameters._yScale;

    qDebug() << __FUNCTION__ << "QcsXY image coordinate: " << qcsXY._x << ", " << qcsXY._y;
    //fprintf(stdout, "QcsXY image coordinate: %f %f\n", qcsXY._x, qcsXY._y);

    return(true);
}


bool ImageGeoRef::checkSame_geoTFWDataconst(const ImageGeoRef& imageGeoRefToCompare) const {
    return(_worldFileParameters.checkSame(imageGeoRefToCompare._worldFileParameters));
}


bool ImageGeoRef::get_xScale(double& xScale) const {
    return(_worldFileParameters.get_xScale(xScale));
}

bool ImageGeoRef::get_yScale(double& yScale) const {
    return(_worldFileParameters.get_yScale(yScale));
}

bool ImageGeoRef::setImagePathFilename(const QString& strImagePathAndFilename) {
    _qstrImagePathAndFilename = strImagePathAndFilename;

    //this method is used in a very specific case; settings the field below should not have any effect as already set at this value from the start
    _imageWidth = 0.0;
    _imageHeight = 0.0;
    _bImageSizeGot = false;
    return(true);
}


bool ImageGeoRef::getImageSizeF(QSizeF& sizef) const {
    if (!_bImageSizeGot) {
        sizef = {.0,.0};
        return(false);
    }
    sizef = { static_cast<qreal>(_imageWidth), static_cast<qreal>(_imageHeight)};
    return(true);
}

void ImageGeoRef::setDefaultValuesAsNoGeorefUsed() {

    _inputFileStatus_geoTFWData = e_iFS_geoTFWData_loaded;
    _worldFileParameters.setDefaultAsNoGeorefUsed();
    _qvectqstrWFCandidate.clear();
    _qstrWordlFile_pathAndFilenameUsed.clear();

    _upperLeftImageCorner = {.0,.0};
    _bUpperLeftImageCornerComputed = true;

    loadImageWidthHeightFromImageFile();
}

