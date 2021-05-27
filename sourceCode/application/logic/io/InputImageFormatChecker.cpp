#include <OpenImageIO/imageio.h>

using namespace OIIO;

#include <QDebug>

#include <string>

using namespace std;

#include "../toolbox/toolbox_pathAndFile.h"
#include <QFileInfo>

#include "InputImageFormatChecker.h"

#include "../toolbox/toolbox_json.h"

#include <QDateTime>
#include "../toolbox/toolbox_pathAndFile.h"

S_ImageFileAttributes::S_ImageFileAttributes():
    _width(0),
    _height(0),
    _nchannels(0),
    _basetype(0),
    _fileSize(0),
    _bAttributesFeed(false) {

}
void S_ImageFileAttributes::clear() {
    _width = 0;
    _height = 0;
    _nchannels = 0;
    _basetype = 0;
    _fileSize = 0;
    _bAttributesFeed = false;

}

bool S_ImageFileAttributes::toQJsonObject(QJsonObject& qjsonObj) const {

    qjsonObj.insert("width", _width);
    qjsonObj.insert("height", _height);
    qjsonObj.insert("nchannels", _nchannels);
    qjsonObj.insert("basetype", _basetype);
    qjsonObj.insert("fileSize", static_cast<double>(_fileSize));

    return(true);
}

void S_ImageFileAttributes::showContent() {
    qDebug() << "(S_ImageFileAttributes)" << __FUNCTION__ << "_width  = " << _width;
    qDebug() << "(S_ImageFileAttributes)" << __FUNCTION__ << "_height = " << _height;
    qDebug() << "(S_ImageFileAttributes)" << __FUNCTION__ << "_nchannels = " << _nchannels;
    qDebug() << "(S_ImageFileAttributes)" << __FUNCTION__ << "_basetype  = " << _basetype;
    qDebug() << "(S_ImageFileAttributes)" << __FUNCTION__ << "_fileSize  = " << _fileSize;
    qDebug() << "(S_ImageFileAttributes)" << __FUNCTION__ << "* _bAttributesFeed = " << _bAttributesFeed;
}

bool S_ImageFileAttributes::compareTo(const S_ImageFileAttributes& compareToMe) {

    if (!_bAttributesFeed) {
          return(false);
    }
    if (!compareToMe._bAttributesFeed) {
          return(false);
    }

    bool bSame = true;

    bSame &= (_width     == compareToMe._width);
    qDebug() << "width:" << _width << "cmp to: " << compareToMe._width;

    bSame &= (_height    == compareToMe._height);
    qDebug() << "height:" << _height << "cmp to: " << compareToMe._height;

    bSame &= (_nchannels == compareToMe._nchannels);
    qDebug() << "nchannels:" << _nchannels << "cmp to: " << compareToMe._nchannels;

    bSame &= (_basetype  == compareToMe._basetype);
    qDebug() << "basetype:" << _basetype << "cmp to: " << compareToMe._basetype;

    bSame &= (_fileSize  == compareToMe._fileSize);
    qDebug() << "fileSize:" << _fileSize << "cmp to: " << compareToMe._fileSize;

    return(bSame);
}


bool S_ImageFileAttributes::fromQJsonObject(const QJsonObject& qjsonObj) {

    bool bGot = true;

    bGot &= getIntValueFromJson(qjsonObj, "width", _width);
    bGot &= getIntValueFromJson(qjsonObj, "height", _height);
    bGot &= getIntValueFromJson(qjsonObj, "nchannels", _nchannels);
    bGot &= getIntValueFromJson(qjsonObj, "basetype", _basetype);

    double dblFileSizeFromJson = 0;
    bGot &= getDoubleValueFromJson(qjsonObj, "fileSize", dblFileSizeFromJson);
    _fileSize = static_cast<quint64>(dblFileSizeFromJson);

    if (!bGot) {
        clear();
        qDebug() << __FUNCTION__ << " error: qjsonObj does not contains all keys";
        return(false);
    }

    _bAttributesFeed = true;
    return(true);
}


bool S_ImageFileAttributes::fromFile(const QString& strFilename) {

    clear();

    if (!fileExists(strFilename)) {
        return(false);
    }

    ImageSpec imgSpec;
    bool bImageSpecGot = InputImageFormatChecker::getImageSpec(strFilename.toStdString(), imgSpec);
    if (!bImageSpecGot) {
        return(false);
    }

    //@#LP no check about fileSize 0
    QFileInfo qfinfoAboutFilename(strFilename);
    _fileSize = static_cast<quint64>(qfinfoAboutFilename.size());
    _width     = imgSpec.width;
    _height    = imgSpec.height;
    _nchannels = imgSpec.nchannels;
    _basetype  = imgSpec.channelformat(0).basetype;

    _bAttributesFeed = true;

    return(true);
}


bool InputImageFormatChecker::check(const string& inputFilename) {
    ImageSpec gotImgSpec;
    return(check(inputFilename, gotImgSpec));
}


bool InputImageFormatChecker::check(const string& inputFilename, ImageSpec& imgSpec) {

    ImageSpec gotImgSpec;
    bool bImgSpecGot = getImageSpec(inputFilename, gotImgSpec);
    if (!bImgSpecGot) {
        return(false);
    }
    if (!checkImageSpecValidity(gotImgSpec)) {
        qDebug() << __FUNCTION__ << "error: not supported; file " << inputFilename.c_str();
        return(false);
    }
    imgSpec = gotImgSpec; //set imgSpec only if success
    return(true);
}

bool InputImageFormatChecker::getImageSpec(const string& inputFilename, ImageSpec& imgSpec) {
    auto in = ImageInput::open(inputFilename);
    if (!in) {
        qDebug() << __FUNCTION__ << ": error: can not open for read; file " << inputFilename.c_str();
        return(false);
    }
    imgSpec = in->spec();
    return(true);
}

//@#LP could add subimage and miplevel count and consider a rejection in case of more than one subimage or more than one miplevel
bool InputImageFormatChecker::checkImageSpecValidity(const ImageSpec& ImgSpec) {
    if ((ImgSpec.width < 2)||(ImgSpec.height < 2)) {
        qDebug() << __FUNCTION__ << "error: w|h < 2";
        return(false);
    }
    if ((ImgSpec.width > 100000)||(ImgSpec.height > 100000)) { //Qt scrollbar seems to be limited to 100000
        qDebug() << __FUNCTION__ << "error: w|h < 2";
        return(false);
    }

    return(formatIsAnAcceptedFormat(ImgSpec));
}

bool InputImageFormatChecker::formatIsAnAcceptedFormatForDisplacementMap(const ImageSpec& ImgSpec) {

    if (ImgSpec.nchannels != 1) {
        qDebug() << __FUNCTION__ << " nchannels != 1";
        return(false);
    }
    unsigned char typeDescBaseType = ImgSpec.channelformat(0).basetype;
    qDebug() << __FUNCTION__ << " (ImgSpec.channelformat(0).basetype =) typeDescBaseType =" << typeDescBaseType;

    if (   (typeDescBaseType != TypeDesc::FLOAT)
        && (typeDescBaseType != TypeDesc::INT16)) {
        qDebug() << __FUNCTION__ << " typeDescBaseType != FLOAT and != INT16 (" << typeDescBaseType;
        return(false);
    }
    return(true);
}

bool InputImageFormatChecker::formatIsAnAcceptedFormatForCorrelationScoreMap(const ImageSpec& ImgSpec) {

    if (ImgSpec.nchannels != 1) {
        qDebug() << __FUNCTION__ << " nchannels != 1";
        return(false);
    }

    unsigned char typeDescBaseType = ImgSpec.channelformat(0).basetype;

    qDebug() << __FUNCTION__ << " (ImgSpec.channelformat(0).basetype =) typeDescBaseType =" << typeDescBaseType;

    if (  (typeDescBaseType != TypeDesc::UINT8)
        &&(typeDescBaseType != TypeDesc::FLOAT)) {
        qDebug() << __FUNCTION__ << " typeDescBaseType != UINT8 && != FLOAT";
        return(false);
    }

    return(true);
}

bool InputImageFormatChecker::formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver(const string& inputFilename) {

    ImageSpec gotImgSpec;
    bool bImgSpecGot = getImageSpec(inputFilename, gotImgSpec);
    if (!bImgSpecGot) {
        return(false);
    }
    if (!formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver(gotImgSpec)) {
        qDebug() << __FUNCTION__ << "error: not supported; file " << inputFilename.c_str();
        return(false);
    }
    return(true);
}

bool InputImageFormatChecker::formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver(const ImageSpec& ImgSpec) {
    return(checkImageSpecValidity(ImgSpec));
}

bool InputImageFormatChecker::formatIsAnAcceptedFormat(const ImageSpec& ImgSpec) {

    if (ImgSpec.nchannels == 1){
        qDebug() << __FUNCTION__ << "if (ImgSpec.nchannels == 1) {";
        unsigned char typeDescBaseType = ImgSpec.channelformat(0).basetype;
        switch (typeDescBaseType) {
            case TypeDesc::UINT8: //                        ; typically: correlation score map ; or image to draw trace over only
            case TypeDesc::FLOAT: //                        ; typically: displacement map
            case TypeDesc::INT16: //signed 16 bits integer  ; typically: displacement map
            case TypeDesc::UINT16: //aded for               ; typically: image to draw trace over only
                return(true);
            default:
                break;
        }
        qDebug() << "one channel but not UINT8, not FLOAT, not INT16, not UINT16. Its basetype is: " << typeDescBaseType;
        return(false);
    }

    if (ImgSpec.nchannels == 3) {
        qDebug() << __FUNCTION__ << "if (ImgSpec.nchannels == 3) {";
        for (int ic = 0; ic < ImgSpec.nchannels; ++ic) {
            unsigned char typeDescBaseType = ImgSpec.channelformat(ic).basetype;
            if (typeDescBaseType != TypeDesc::UINT8) {
                qDebug() << __FUNCTION__ << "channel #" << ic << "is not UINT8, its type is: " << typeDescBaseType;
                return(false);
            }
         }
         return(true);
    }

    qDebug() << __FUNCTION__ << "nchannels != 1 and != 3";
    return(false);
}

bool InputImageFormatChecker::checkSameWidthHeightBaseType(const ImageSpec& imageSpec1, const ImageSpec& imageSpec2, bool bCheckBaseType) {
    bool bSame = true;
    bSame &= (imageSpec1.width  == imageSpec2.width);
    bSame &= (imageSpec1.height == imageSpec2.height);
    if (bCheckBaseType) {
        bSame &= (imageSpec1.channelformat(0).basetype == imageSpec2.channelformat(0).basetype);
    }
    return(bSame);
}


S_ImageFileAttributes_with_dateTimeLastModified::S_ImageFileAttributes_with_dateTimeLastModified() {
    clear();
}

void S_ImageFileAttributes_with_dateTimeLastModified::clear() {
    _bAttributesFeed = false;
    _sImageFileAttributes.clear();
    //_qDateTime_birthTime = {}; //set as invalid
    _qDateTime_lastModified = {}; //set as invalid
}

void S_ImageFileAttributes_with_dateTimeLastModified::showContent() {
    qDebug() << "(S_ImageFileAttributes_with_birthTimeAndLastModified)" << __FUNCTION__ << "* _bAttributesFeed = " << _bAttributesFeed;
    //qDebug() << "(S_ImageFileAttributes_with_birthTimeAndLastModified)" << __FUNCTION__ << "_qDateTime_birthTime  = " << _qDateTime_birthTime;
    qDebug() << "(S_ImageFileAttributes_with_birthTimeAndLastModified)" << __FUNCTION__ << "_qDateTime_lastModified = " << _qDateTime_lastModified;
    _sImageFileAttributes.showContent();
}

bool S_ImageFileAttributes_with_dateTimeLastModified::compareTo(const S_ImageFileAttributes_with_dateTimeLastModified& compareToMe) {

    if (!_bAttributesFeed) {
        return(false);
    }
    if (!compareToMe._bAttributesFeed) {
        return(false);
    }

    bool b_SImageFileAttributes_same = _sImageFileAttributes.compareTo(compareToMe._sImageFileAttributes);
    if (!b_SImageFileAttributes_same) {
        return(false);
    }

    bool bSame = true;

    //bSame &= (_qDateTime_birthTime == compareToMe._qDateTime_birthTime);
    //qDebug() << "_qDateTime_birthTime:" << _qDateTime_birthTime << "cmp to: " << compareToMe._qDateTime_birthTime;

    bSame &= (_qDateTime_lastModified == compareToMe._qDateTime_lastModified);
    qDebug() << "_qDateTime_lastModified:" << _qDateTime_lastModified << "cmp to: " << compareToMe._qDateTime_lastModified;

    return(bSame);
}

bool S_ImageFileAttributes_with_dateTimeLastModified::toQJsonObject(QJsonObject& qjsonObj) const {

    _sImageFileAttributes.toQJsonObject(qjsonObj);

    QString strDateTimeFormat = "yyyy.MM.dd hh:mm:ss:zzz";

    //QString strBirthDateTime        = _qDateTime_birthTime.toString(/*Qt::RFC2822Date*/strDateTimeFormat);
    QString strLastModifiedDateTime = _qDateTime_lastModified.toString(/*Qt::RFC2822Date*/strDateTimeFormat);

    //qjsonObj.insert("dateTimeBirth", strBirthDateTime);
    qjsonObj.insert("dateTimeLastModified", strLastModifiedDateTime);

    return(true);
}

bool S_ImageFileAttributes_with_dateTimeLastModified::fromQJsonObject(const QJsonObject& qjsonObj) {

    bool bGot = true;

    bGot &= _sImageFileAttributes.fromQJsonObject(qjsonObj);

    if (!bGot) {
        clear();
        qDebug() << __FUNCTION__ << " error: qjsonObj does not contains all keys (from _sImageFileAttributes)";
        return(false);
    }

    qDebug() << __FUNCTION__ << "_sImageFileAttributes.showContent() call:";
    _sImageFileAttributes.showContent();

    QString strDateTimeFormat = "yyyy.MM.dd hh:mm:ss:zzz";

    //bool bGot_dateTime_birth = getQDateTimeFromJson_stringWithSpecificDateTimeFormat(qjsonObj, /*Qt::RFC2822Date*/static_cast<const QString&>(strDateTimeFormat), "dateTimeBirth", _qDateTime_birth);
    bool bGot_dateTime_lastModified = getQDateTimeFromJson_stringWithSpecificDateTimeFormat(qjsonObj, strDateTimeFormat, "dateTimeLastModified",
                                                                                            _qDateTime_lastModified);

    bGot &= bGot_dateTime_lastModified;

    if (!bGot) {
        clear();
        qDebug() << __FUNCTION__ << " error: qjsonObj does not contains all keys";
        return(false);
    }

    //qDebug() << __FUNCTION__ << " _qDateTime_birthTime:    " << _qDateTime_birthTime;
    qDebug() << __FUNCTION__ << " _qDateTime_lastModified: " << _qDateTime_lastModified;

    _bAttributesFeed = true;
    return(true);
}

bool S_ImageFileAttributes_with_dateTimeLastModified::fromFile(const QString& strFilename) {

    clear();

    bool b_fromFile = true;

    b_fromFile &= _sImageFileAttributes.fromFile(strFilename);
    b_fromFile &= getDateTimeLastModified_aboutFile(strFilename, _qDateTime_lastModified);

    if (!b_fromFile) {
        clear();
        return(false);
    }

    //qDebug() << __FUNCTION__ << "_qDateTime_birthTime   :" << _qDateTime_birthTime;
    qDebug() << __FUNCTION__ << "_qDateTime_lastModified:" << _qDateTime_lastModified;

    _bAttributesFeed = true;
    return(true);
}

