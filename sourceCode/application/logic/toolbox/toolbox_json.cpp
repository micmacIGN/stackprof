#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

#include <QDebug>

#include "toolbox_json.h"
#include "toolbox_pathAndFile.h"
#include "toolbox_conversion.h"

#include "../io/InputImageFormatChecker.h"


bool getStringValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, QString& strValue) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    strValue = qjson_value.toString();
    return(true);
}

bool getSignedIntValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, int& intValue) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    intValue = qjson_value.toInt();
    return(true);
}

bool getBoolValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, bool& bValue) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    bValue = qjson_value.toBool();
    return(true);
}


bool getIntValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, int& intValue) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    intValue = qjson_value.toInt();
    return(true);
}

bool getDoubleValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, double& doubleValue) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    doubleValue = qjson_value.toDouble();
    return(true);
}

bool getQDateTimeFromJson_stringWithSpecificDateTimeFormat(const QJsonObject& qJsonObj, /*Qt::DateFormat qDateformat*/const QString& strDateTimeFormat, const QString& str_key,
                                                           QDateTime& qDateTimeFromQjsonObj) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        qDebug() << __FUNCTION__ << "if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) { str_key used: " << str_key;
        return(false);
    }
    QString strDateTime = qjson_value.toString();
    qDebug() << __FUNCTION__ << "strDateTime:" << strDateTime;

    qDateTimeFromQjsonObj = QDateTime::fromString(strDateTime, strDateTimeFormat);

    qDebug() << __FUNCTION__ << "qDateTimeFromQjsonObj: " << qDateTimeFromQjsonObj;

    return(true);
}

/*
bool setStringWithSpecificDateTimeFormat_fromQDateTime(const QString& qStrDateTime, const QString& str_key, QDateTime& qdateTime) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    QString strDateTime = qjson_value.toString();
    qdateTime = QDateTime::fromString(strDateTime);
    return(true);
}
*/




bool getQJsonObjectFromJson(const QJsonObject& qJsonObj, const QString& str_key, QJsonObject& QJsonObjOut) {
    QJsonValue qjson_value;
    if (!getQJsonValueFromJson(qJsonObj, str_key, qjson_value)) {
        return(false);
    }
    QJsonObjOut = qjson_value.toObject();
    return(true);
}

bool getQJsonValueFromJson(const QJsonObject& qJsonObj, const QString& qstr_key, QJsonValue& QJsonVal) {

    if (!qJsonObj.contains(qstr_key)) {
        qDebug() << __FUNCTION__ << ": key not found:" << qstr_key;
        return(false);
    }
    QJsonVal = qJsonObj.value(qstr_key);
    //qDebug() << __FUNCTION__ << "qjson_value = " << QJsonVal;
    return(true);
}

void splitStrPathAndFilename_toQJsonObject(const QString& strPathAndFilename, QJsonObject& qJsonObj) {

    QString qstrAbsolutePathOnly;
    QString qstrFilenameOnly;
    splitStrPathAndFile(strPathAndFilename, qstrAbsolutePathOnly, qstrFilenameOnly);

    qstrAbsolutePathOnly = replaceDirSeparatorBySlash(qstrAbsolutePathOnly);

    //filename and absolutePath splitted in json file for convenience (can be more human edition friendly)
    qJsonObj.insert("filename"    , qstrFilenameOnly);
    qJsonObj.insert("absolutePath", qstrAbsolutePathOnly);
}

bool joinQJsonObjectPathAndFilename_toQString(const QJsonObject& qJsonObj, QString& strPathAndFilename) {

    QString qstrFilenameOnly;
    bool bStrFilenameGot = getStringValueFromJson(qJsonObj, "filename", qstrFilenameOnly);

    QString qstrAbsolutePathOnly;
    bool bStrAbsolutePatGot = getStringValueFromJson(qJsonObj, "absolutePath", qstrAbsolutePathOnly);

    if (  (!bStrFilenameGot)
        ||(!bStrAbsolutePatGot)) {
        return(false);
    }

    //all empty: considered as an unused file
    if (qstrAbsolutePathOnly.isEmpty() && qstrFilenameOnly.isEmpty()) {
        return(true);
    }

    //if one of them is empty, then it's an error
    if (  qstrAbsolutePathOnly.isEmpty()
        ||qstrFilenameOnly.isEmpty()) {
        return(false);
    }

    qstrAbsolutePathOnly = replaceSlashByDirSeparator(qstrAbsolutePathOnly);
    strPathAndFilename = qstrAbsolutePathOnly + QDir::separator() + qstrFilenameOnly;

    return(true);
}

bool qrealPoint_toQJsonArray(qreal x, qreal y, int outFixedPrecision, QJsonArray& qjsonArrayOut) {

    //@#LP to merge with qPointFtoNumberFixedPrecision

    qreal qrx=.0, qry=.0;
    bool bOK = true;
    bOK &= qrealToRealNumberFixedPrecision_f(x, outFixedPrecision, qrx);
    bOK &= qrealToRealNumberFixedPrecision_f(y, outFixedPrecision, qry);

    if (!bOK) {
        qDebug() << __FUNCTION__ << ": error: formatting double";
        return(false);
    }

    QJsonArray qJsonArray_pointXYvaluesDouble;
    qJsonArray_pointXYvaluesDouble.insert(0, qrx);
    qJsonArray_pointXYvaluesDouble.insert(1, qry);
    qjsonArrayOut = qJsonArray_pointXYvaluesDouble;
    return(true);
}
