#include "exportResult.h"

#include <QString>

#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

S_ExportSettings::S_ExportSettings():
    _ieExportResult_fileType { e_ERfT_ascii },
    _ieExportResult_boxesContent { e_ERbC_all},
    _idOfCurrentBox {-1},
    _bIncludeProfilesCurvesData {false},
    _bIfWarnFlagByUser_setLinearRegressionData_asEmpty {true}, //by default at true as the first goal of the warnFlag: avoid to export data about the linear regression
    _bIfWarnFlagByUser_setProfilesCurvesData_asEmpty {false} { //by default at false to let a change to the user to process outside the curves data

}

bool qJsonObject_to_asciiFormat_sectionWithIdentation(const QJsonObject& qJsonObject_exportFileContent, QByteArray& QByteArray_ascii) {

    QJsonDocument JsonDoc(qJsonObject_exportFileContent);
    QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Compact); //to text

    qDebug() << __FUNCTION__ << "qbArrayData = [" << qbArrayData << "]";

    QByteArray qbArrayData_toAscii_1 = qbArrayData.replace("\"", "");

    qDebug() << __FUNCTION__ << "qbArrayData_toAscii_1 = [" << qbArrayData_toAscii_1 << "]";

    QString qstrLineSpaceIndent = "  ";

    int currentLineSpaceIndent = 0;
    int qbyteArraySize = qbArrayData_toAscii_1.size();
    for (int i = 0; i< qbyteArraySize; i++) {
        bool bProcessed = false;
        if (qbArrayData_toAscii_1[i] == '{' ) { // initial {
            if (i == 0) {
                bProcessed = true;
            }
        }
        if (qbArrayData_toAscii_1[i] == ':' ) {
            if (i == qbyteArraySize-1) {
                return(false);
            }
            if (qbArrayData_toAscii_1[i+1] == '{' ) { // :{ start of group
                currentLineSpaceIndent++;
                QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                i++;
            } else {
                QByteArray_ascii+=";"; //key value separator
            }
            bProcessed = true;
        }
        if (!bProcessed) {
            if (qbArrayData_toAscii_1[i] == '}' ) {
                if (i == qbyteArraySize-1) {
                    break;
                }
                if (qbArrayData_toAscii_1[i+1] == ',' ) { // }, end of group
                    currentLineSpaceIndent--;
                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    i++;
                } else {
                    currentLineSpaceIndent--;
                }
                bProcessed = true;
            }
        }
        if (!bProcessed) {
            if (qbArrayData_toAscii_1[i] == ',' ) { //field separator in same group
                QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                bProcessed = true;
            }
        }
        if (!bProcessed) {
            QByteArray_ascii+=qbArrayData_toAscii_1[i];
        }
    }

    qDebug() << __FUNCTION__ << "QByteArray_ascii = [" << QByteArray_ascii << "]";

    return(true);
}


bool qJsonObject_to_asciiFormat_arrayOfsectionWithIdentation(const QJsonObject& qJsonObject_exportFileContent, QByteArray& QByteArray_ascii) {

    QJsonDocument JsonDoc(qJsonObject_exportFileContent);
    QByteArray qbArrayData = JsonDoc.toJson(QJsonDocument::JsonFormat::Compact); //to text

    qDebug() << __FUNCTION__ << "qbArrayData = [" << qbArrayData << "]";

    QByteArray qbArrayData_toAscii_1 = qbArrayData.replace("\"", "");

    qDebug() << __FUNCTION__ << "qbArrayData_toAscii_1 = [" << qbArrayData_toAscii_1 << "]";

    QString qstrLineSpaceIndent = "  ";

    int currentLineSpaceIndent = 0;
    int qbyteArraySize = qbArrayData_toAscii_1.size();
    bool bJustEndOfSectionInsideArray = false;

    for (int i = 0; i< qbyteArraySize; i++) {
        bool bProcessed = false;
        if (qbArrayData_toAscii_1[i] == '{' ) { // initial {
            if (i == 0) {
                bProcessed = true;
            }
        }
        if (qbArrayData_toAscii_1[i] == ':' ) {
            if (i == qbyteArraySize-1) {
                return(false);
            }
            if (qbArrayData_toAscii_1[i+1] == '[' ) { // :[ start of array
                if (i+2 == qbyteArraySize-1) {
                    return(false);
                }
                currentLineSpaceIndent++;
                QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                i++;

                if (qbArrayData_toAscii_1[i+1] == '{' ) { // :{ start of first inside array

                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    i++;
                }
                bProcessed = true;
            } else {
                if (qbArrayData_toAscii_1[i+1] == '{' ) { // :{ start of group
                    currentLineSpaceIndent++;
                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    i++;
                } else {
                    QByteArray_ascii+=";"; //key value separator
                }
                bProcessed = true;
            }
        }
        if (!bProcessed) {
            if (qbArrayData_toAscii_1[i] == '}' ) {
                if (i == qbyteArraySize-1) {
                    break;
                }
                if (qbArrayData_toAscii_1[i+1] == ',' ) { // }, end of group
                    currentLineSpaceIndent--;
                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    i++;
                } else {
                    currentLineSpaceIndent--;
                }
                bProcessed = true;
            }
        }
        if (!bProcessed) {
            if (qbArrayData_toAscii_1[i] == ']' ) {
                if (i == qbyteArraySize-1) {
                    break;
                }
                if (qbArrayData_toAscii_1[i+1] == ',' ) { // }, end of sub array
                    currentLineSpaceIndent--;
                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    i++;
                    bProcessed = true;
                } else {
                    if (qbArrayData_toAscii_1[i+1] == '}' ) { // }, end of section inside array

                        QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                        i++;
                        bProcessed = true;
                        bJustEndOfSectionInsideArray = true;
                    }
                }
            }
        }
        if (!bProcessed) {
            if (qbArrayData_toAscii_1[i] == ',' ) {   //EndOfSectionInsideArray or field separator in same group
                if (bJustEndOfSectionInsideArray) { //EndOfSectionInsideArray
                    bJustEndOfSectionInsideArray = false;
                    currentLineSpaceIndent--;
                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    i++;
                    bProcessed = true;
                } else { //field separator in same group
                    QByteArray_ascii+="\n" + qstrLineSpaceIndent.repeated(currentLineSpaceIndent);
                    bProcessed = true;
                }
            }
        }
        if (!bProcessed) {
            QByteArray_ascii+=qbArrayData_toAscii_1[i];
            bJustEndOfSectionInsideArray = false;
        }
    }

    qDebug() << __FUNCTION__ << "QByteArray_ascii = [" << QByteArray_ascii << "]";

    return(true);
}


QString boolToAsciiBoolString(bool bValue) {
    if (bValue) {
        return("true");
    }
    return("false");
}


