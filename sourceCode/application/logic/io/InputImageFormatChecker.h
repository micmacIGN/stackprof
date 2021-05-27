#ifndef INPUTIMAGEFORMATCHECKER
#define INPUTIMAGEFORMATCHECKER

#include <OpenImageIO/imageio.h>
using namespace OIIO;

#include <string>
using namespace std;

#include <QtGlobal>

#include <QJsonObject>

#include <QDateTime>

struct S_ImageFileAttributes {
    int _width;
    int _height;
    int _nchannels;
    /*unsigned char*/int _basetype;
    quint64 _fileSize;
    bool _bAttributesFeed;

    S_ImageFileAttributes();
    void clear();
    bool toQJsonObject(QJsonObject& qjsonObj) const;
    bool fromQJsonObject(const QJsonObject& qjsonObj);
    bool fromFile(const QString& strFilename);
    void showContent();
    bool compareTo(const S_ImageFileAttributes& compareToMe);   
};

struct S_ImageFileAttributes_with_dateTimeLastModified {

    S_ImageFileAttributes_with_dateTimeLastModified();
    void clear();
    bool toQJsonObject(QJsonObject& qjsonObj) const;
    bool fromQJsonObject(const QJsonObject& qjsonObj);
    bool fromFile(const QString& strFilename);
    void showContent();
    bool compareTo(const S_ImageFileAttributes_with_dateTimeLastModified& compareToMe);

    bool _bAttributesFeed;

    S_ImageFileAttributes _sImageFileAttributes;
    QDateTime _qDateTime_birth;
    QDateTime _qDateTime_lastModified;
};


//bool get_imageFileAttributes_fromFile(const QString& strFilename, S_ImageFileAttributes& imageFileAttributes);

class InputImageFormatChecker {

public:
    bool check(const string& inputFilename);
    bool check(const string& inputFilename, ImageSpec& imgSpec);

    static bool getImageSpec(const string& inputFilename, ImageSpec& imgSpec);

    ////bool check(const string& inputFilename, unsigned char& typeDescBaseTypeChannel0);

    bool formatIsAnAcceptedFormatForDisplacementMap(const ImageSpec& ImgSpec);
    bool formatIsAnAcceptedFormatForCorrelationScoreMap(const ImageSpec& ImgSpec);

    bool formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver(const string& inputFilename);

    bool checkSameWidthHeightBaseType(const ImageSpec& imageSpec1, const ImageSpec& imageSpec2, bool bCheckBaseType);

private:
    bool checkImageSpecValidity(const ImageSpec& ImgSpec);
    bool formatIsAnAcceptedFormat(const ImageSpec& ImgSpec);

    bool formatIsAnAcceptedFormatForBackgroundImageToDrawRouteOver(const ImageSpec& ImgSpec);

};

#endif

