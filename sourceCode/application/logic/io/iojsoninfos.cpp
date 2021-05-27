#include "iojsoninfos.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include <QByteArray>

#include <QDebug>





bool IOJsonInfos::loadFromFile(const QString& qstrInputFileInfos) {

    QJsonObject qjsonObjEmpty;
    _qjsonObjInfos = qjsonObjEmpty;

    QFile inputFile(qstrInputFileInfos);
    if (!inputFile.open(QIODevice::ReadOnly| QIODevice::Text)) {
        qDebug() << __FUNCTION__ << ": error: (read open) Can not open file " << qstrInputFileInfos;
        return(false);
    }
    QByteArray qbArrayData = inputFile.readAll();
    if (qbArrayData.isEmpty()) {
        qDebug() << __FUNCTION__ <<  ": error: zero byte read from file " << qstrInputFileInfos;
        return(false);
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(qbArrayData); //from text
    if (jsonDoc.isNull()) {
        qDebug() << __FUNCTION__ <<  ": error: failed to create jsonDoc" ;
        return(false);
    }
    if (!jsonDoc.isObject()) {
        qDebug() << __FUNCTION__ <<  ": error: jsonDoc is not an object";
        return(false);
    }
    _qjsonObjInfos = jsonDoc.object();
    return(true);
}

bool IOJsonInfos::saveToFile(const QString& qstrOuputFileInfos) {
    QFile outFile(qstrOuputFileInfos);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "error: (write open) Can not open file " << qstrOuputFileInfos;
        return(false);
    }
    QJsonDocument jsonDoc(_qjsonObjInfos);

    QByteArray qbArrayData = jsonDoc.toJson(QJsonDocument::JsonFormat::Indented); //to text
    qint64 wroteBytes = outFile.write(qbArrayData);
    if (wroteBytes <= 0) {
         qDebug() << "error: writing byte to file " << qstrOuputFileInfos;
         return(false);
    }
    return(true);
}

void IOJsonInfos::fromVariantMap(const QVariantMap &map) {

    QVariantMap::const_iterator qvarmapCstIter = map.cbegin();
    for(; qvarmapCstIter != map.cend(); ++qvarmapCstIter) {
        qDebug() << __FUNCTION__ << "map  iter [key]= value:  [" << qvarmapCstIter.key() << "] = " << qvarmapCstIter.value();
    }

    _qjsonObjInfos = QJsonObject::fromVariantMap(map);

    //dbg
    QJsonDocument jsonDoc(_qjsonObjInfos);
    QByteArray qbArrayData = jsonDoc.toJson(); //to text
    qDebug() << __FUNCTION__ << "qbArrayData=" << qbArrayData;

    QJsonObject::const_iterator qjsonobjCstIter = _qjsonObjInfos.constBegin();
    for(; qjsonobjCstIter != _qjsonObjInfos.constEnd(); ++qjsonobjCstIter) {
        qDebug() << __FUNCTION__ << "_qjsonObjInfos iter [key]= value:  [" << qjsonobjCstIter.key() << "] = " << qjsonobjCstIter.value();
    }
}






QVariantMap IOJsonInfos::toVariantMap() {
	return(_qjsonObjInfos.toVariantMap());
}


void IOJsonInfos::setQJsonObject(const QJsonObject &QJsonObj) {
    _qjsonObjInfos = QJsonObj;
}

QJsonObject IOJsonInfos::getQJsonObject() {
    return(_qjsonObjInfos);
}

