#ifndef IOJSONINFOS_H
#define IOJSONINFOS_H

#include <QString>
#include <QVariantMap>
#include <QJsonObject>

class IOJsonInfos {

    public:
        //IOJsonInfos();
        bool loadFromFile(const QString& qstrInputFileInfos);
        bool saveToFile(const QString& qstrOuputFileInfos);
        //void insertInfos(const QString& strKey, QJsonValue qjsonV);
        //QVariantMap getAllinfos();

        void fromVariantMap(const QVariantMap &map);
        QVariantMap toVariantMap();

        void setQJsonObject(const QJsonObject &QJsonObj);
        QJsonObject getQJsonObject();

    private:
        QJsonObject _qjsonObjInfos;
};

#endif // INPUTIMAGEINFOSJSON_H
