#ifndef toolbox_json_h
#define toolbox_json_h

#include <QString>
#include <QJsonObject>

bool getQJsonObjectFromJson(const QJsonObject& qJsonObj, const QString& str_key, QJsonObject& QJsonObjOut);
bool getQJsonValueFromJson(const QJsonObject& qJsonObj, const QString& qstr_key, QJsonValue& QJsonVal);

bool getStringValueFromJson   (const QJsonObject& qJsonObj, const QString& str_key, QString& strValue);
bool getSignedIntValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, int& intValue);

bool getBoolValueFromJson  (const QJsonObject& qJsonObj, const QString& str_key, bool& bValue);
bool getDoubleValueFromJson(const QJsonObject& qJsonObj, const QString& str_key, double& doubleValue);
bool getIntValueFromJson   (const QJsonObject& qJsonObj, const QString& str_key, int& intValue);

bool qrealPoint_toQJsonArray(qreal x, qreal y, int outFixedPrecision, QJsonArray& qjsonArrayOut);

//these two methods use replaceDirSeparatorBySlash and replaceSlashByDirSeparator
//to use slash as directory separator in json value (for .json file write and load) on any OS
void splitStrPathAndFilename_toQJsonObject(const QString& strPathAndFilename, QJsonObject& qJsonObj);
bool joinQJsonObjectPathAndFilename_toQString(const QJsonObject& qJsonObj, QString& strPathAndFilename);

//bool fileInfos_toQJsonObject(const QString& strFilename, QJsonObject& QJsonObject_fileInfos);

#include <QDateTime>

bool getQDateTimeFromJson_stringWithSpecificDateTimeFormat(const QJsonObject& qJsonObj,
                                                           /*Qt::DateFormat qDateformat*/const QString& strDateTimeFormat,
                                                           const QString& str_key,
                                                           QDateTime& qDateTimeFromQjsonObj);

#endif

