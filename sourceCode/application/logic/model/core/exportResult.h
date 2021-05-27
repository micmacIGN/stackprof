#ifndef EXPORTRESULT_H
#define EXPORTRESULT_H

#include <QString>

#include <QMetaType>

enum e_ExportResult_fileType {
    e_ERfT_ascii,
    e_ERfT_json
};

enum e_ExportResult_boxesContent {
    e_ERbC_all,
    e_ERbC_onlyCurrent
};

struct S_ExportSettings { //to avoid Qt enum and METATYPE issue, use int instead of enum
    int _ieExportResult_fileType;
    int _ieExportResult_boxesContent;
    int _idOfCurrentBox;    
    bool _bIncludeProfilesCurvesData;

    bool _bIfWarnFlagByUser_setLinearRegressionData_asEmpty;
    bool _bIfWarnFlagByUser_setProfilesCurvesData_asEmpty;

    S_ExportSettings();
};
Q_DECLARE_METATYPE(S_ExportSettings);


#include <QString>

#include <QJsonObject>
#include <QJsonDocument>

class FieldSeparator {
    public:
        static QString forColumn() { return(";"); }
};


bool qJsonObject_to_asciiFormat_sectionWithIdentation(const QJsonObject& qJsonObject_exportFileContent, QByteArray& QByteArray_ascii);
bool qJsonObject_to_asciiFormat_arrayOfsectionWithIdentation(const QJsonObject& qJsonObject_exportFileContent, QByteArray& QByteArray_ascii);

QString boolToAsciiBoolString(bool bValue);

#endif // EXPORTRESULT_H
