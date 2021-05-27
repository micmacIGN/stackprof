#include <QString>
#include <QVector>

#include <QFileInfo>

#include <QDebug>

//---
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <iomanip>

#include <cstring>

using namespace std;

#include <QFile>
//---

//for array copy
#include <algorithm>
#include <iterator>


#include "../toolbox/toolbox_math.h"
#include "../toolbox/toolbox_pathAndFile.h"
#include "../toolbox/toolbox_conversion.h"

#include "WorldFile_parameters.h"

WorldFile_parameters::WorldFile_parameters():
    _WFparameters {{ .0,.0,.0,.0,.0,.0 }},
    _WFparameters_seenAsZero  {{ true,true,true,true, true, true }},
    _bWFparametersCanBeUsed(false) {
}

bool WorldFile_parameters::findWorldFilesFromAssociatedImagePathAndFilename(QString strImagePathAndFilename, QVector<QString>& qvectqtr_worlfFileFound, QString& qstrWorldFilePreset) {

    qvectqtr_worlfFileFound.clear();
    _qstrWorldFilePathFilename.clear();
    qstrWorldFilePreset.clear();

    //####LP activate the symlink follow up:
    _qstrAssociatedImagePathFilename = strImagePathAndFilename; //_qstrAssociatedImagePathFilename = getPathAndFilenameFromPotentialSymlink(strImagePathAndFilename.c_str());

    QFileInfo fileInfoAssociatedImagePathFilename(_qstrAssociatedImagePathFilename);

    //from the image, we build the path and filename of the according world file

    /* From https://en.wikipedia.org/wiki/World_file :
       There are 3 conventions for filenaming tfw:
       1. Add a 'w' at the end of the image filename
          example: .tif => .tifw
       2. Concat first extension character, last extension character, and a 'w' at the end of the image filename
          example: .tif => .tfw
       3. Use .wld extention
    */
    QString qstrImageExtension = fileInfoAssociatedImagePathFilename.suffix(); //the extension part after the last '.'

    bool bSomeWorldFileExist = false;

    int imageExtensionSize = qstrImageExtension.size();

    if (!imageExtensionSize) {
        return(false);
    }

    QString qstr_filename_noExtension = _qstrAssociatedImagePathFilename;
    qstr_filename_noExtension.chop(imageExtensionSize);

    QChar firstExtChar = qstrImageExtension[0];
    bool bFirstExtCharIsUpperCase = (firstExtChar.toUpper() == firstExtChar);

    QChar qChar_w = 'w';
    if (bFirstExtCharIsUpperCase) {
        qChar_w = qChar_w.toUpper();
    }

    //convention #1 (add a 'w' at the end of the image filename)
    {
        QString qstr_TFWExtension_convention1 = qstrImageExtension/*.toLower()*/ + qChar_w; //'w';

        QString qstr_TFWFilename_convention1 = qstr_filename_noExtension + qstr_TFWExtension_convention1;

        qDebug() << __FUNCTION__ << "qstr_TFWFilename_convention1 = " << qstr_TFWFilename_convention1;
        if (fileExists(qstr_TFWFilename_convention1)) {
            qvectqtr_worlfFileFound.push_back(qstr_TFWFilename_convention1);
            bSomeWorldFileExist = true;
            qDebug() << __FUNCTION__ <<  "Found!";
        } else {
            qDebug() << __FUNCTION__ <<  "not found";
        }
    }

    //convention #2 Concat first extension character, last extension character, and a 'w' at the end of the image filename)
    {
        if (imageExtensionSize < 2) {
            qDebug() << __FUNCTION__ << "image extention too short to build the .tfw extention convention #1: first+last+'w'";

        } else {
            //build the 'tfw' extension:
            QString qstr_TFWExtension_convention2;
            qstr_TFWExtension_convention2.append(qstrImageExtension[0]);
            qstr_TFWExtension_convention2.append(qstrImageExtension[imageExtensionSize-1]);
            qstr_TFWExtension_convention2= qstr_TFWExtension_convention2/*.toLower()*/ + qChar_w; //+ 'w';

            QString qstr_TFWFilename_convention2 = qstr_filename_noExtension + qstr_TFWExtension_convention2;

            qDebug() << __FUNCTION__ <<  "qstr_TFWFilename_convention2 = " << qstr_TFWFilename_convention2;
            if (fileExists(qstr_TFWFilename_convention2)) {
                qvectqtr_worlfFileFound.push_back(qstr_TFWFilename_convention2);
                bSomeWorldFileExist = true;
                qDebug() << __FUNCTION__ <<  "Found!";
            } else {
                qDebug() << __FUNCTION__ <<  "not found";
            }
        }
    }

    //convention3 (.wld extention)
    {
        QString strWldExtention = "wld";
        if (bFirstExtCharIsUpperCase) {
            strWldExtention = strWldExtention.toUpper();
        }

        QString qstr_TFWFilename_convention3 = qstr_filename_noExtension + strWldExtention;
        qDebug() << __FUNCTION__ <<  "qstr_TFWFilename_convention3 = " << qstr_TFWFilename_convention3;

        if (fileExists(qstr_TFWFilename_convention3)) {
            qvectqtr_worlfFileFound.push_back(qstr_TFWFilename_convention3);
            bSomeWorldFileExist = true;
            qDebug() << __FUNCTION__ <<  "Found!";
        } else {
            qDebug() << __FUNCTION__ <<  "not found";
        }
    }

    if (!bSomeWorldFileExist) {
        return(false);
    }

    if (qvectqtr_worlfFileFound.count() == 1) {
        //preset:
        qstrWorldFilePreset = qvectqtr_worlfFileFound[0];
        return(true);
    }

    return(true); //more than one world file available, but no preset done
}


bool WorldFile_parameters::canBeUsed() const {
    return(_bWFparametersCanBeUsed);
}

bool WorldFile_parameters::get_xScale(double& xScale) const {
    if (!canBeUsed()) {
        return(false);
    }
    xScale = _WFparameters._parameters._xScale;
    return(true);
}

bool WorldFile_parameters::get_yScale(double& yScale) const {
    if (!canBeUsed()) {
        return(false);
    }
    yScale = _WFparameters._parameters._yScale;
    return(true);
}

void WorldFile_parameters::setDefaultAsNoGeorefUsed() {

   _WFparameters = {
       {
         1.0, //_xScale
          .0, //_rotationTerms_1
          .0, //_rotationTerms_2
        -1.0, //_yScale
          .0, //_xTranslationTerm_centerOfUpperLeftPixel
          .0  //_yTranslationTerm_centerOfUpperLeftPixel
       }
    };

    _WFparameters_seenAsZero = {
        {
            false, //_b_xScale;
            true,  //_b_rotationTerms_1,
            true,  //_b_rotationTerms_2,
            false, //_b_yScale,
            true,  //_b_xTranslationTerm_centerOfUpperLeftPixel,
            true   //_b_yTranslationTerm_centerOfUpperLeftPixel
        }
    };

    _bWFparametersCanBeUsed = true;
    _qstrWorldFilePathFilename.clear();
    _qstrAssociatedImagePathFilename.clear();

}

const U_WFparameters& WorldFile_parameters::getConstRef_WFparameters() const {
    return(_WFparameters);
}

bool WorldFile_parameters::loadPresetWorldFile() {
    if (_qstrWorldFilePathFilename.isEmpty()) {
        qDebug() << __FUNCTION__ <<  "ImageGeoRef::" << __FUNCTION__ << "_qstrWorldFilePathFilename is empty";
        return(false);
    }
    bool bLoaded = loadWFParametersFromWorldFile(_qstrWorldFilePathFilename, _WFparameters, _WFparameters_seenAsZero);

    computeFlag_bWFparametersCanBeUsed();

    show_WFParameters();

    return(bLoaded);
}

void WorldFile_parameters::computeFlag_bWFparametersCanBeUsed() {

    _bWFparametersCanBeUsed = true;

    //avoid division by zero
    if (  (_WFparameters_seenAsZero._b._b_xScale)
        ||(_WFparameters_seenAsZero._b._b_yScale) ) {        
        qDebug() << __FUNCTION__ << "xScale or yScale is zero";
        _bWFparametersCanBeUsed = false;
    }

    //rotation not handled
    if (   (!_WFparameters_seenAsZero._b._b_rotationTerms_1)
         ||(!_WFparameters_seenAsZero._b._b_rotationTerms_2) ) {
        qDebug() << __FUNCTION__ << "rotationTerms_1 or rotationTerms_2 is not zero";
        _bWFparametersCanBeUsed = false;
    }

    if (!_bWFparametersCanBeUsed) {
        return;
    }

    //whole box computation is warranty only for square pixel
    QString str_xScale = QString::number(qAbs(_WFparameters._parameters._xScale));
    QString str_yScale = QString::number(qAbs(_WFparameters._parameters._yScale));

    qDebug() << __FUNCTION__ << "_WFparameters._parameters._xScale = " << _WFparameters._parameters._xScale << " str_xScale= " << str_xScale;
    qDebug() << __FUNCTION__ << "_WFparameters._parameters._yScale = " << _WFparameters._parameters._yScale << " str_yScale= " << str_yScale;

    bool bSameStr_XScale_YScale = (str_xScale == str_yScale);
    _bWFparametersCanBeUsed = bSameStr_XScale_YScale;

}

bool WorldFile_parameters::loadFromFile(QString strWorldFile) {
   _qstrWorldFilePathFilename =  /*getPathAndFilenameFromPotentialSymlink(*/strWorldFile/*)*/;
   return (loadPresetWorldFile());
}



bool WorldFile_parameters::toQJsonObject(QJsonObject& qjsonObj) const {

    QString str_0_A_xScale           {"0_A_xScale"};
    QString str_1_D_rotationTerms_1  {"1_D_rotationTerms_1"};
    QString str_2_B_rotationTerms_2  {"2_B_rotationTerms_2"};
    QString str_3_E_yScale           {"3_E_yScale"};
    QString str_4_C_xTranslationTerm {"4_C_xTranslationTerm"};
    QString str_5_F_yTranslationTerm {"5_F_yTranslationTerm"};

    QString strKey_WFparameters[6] = {
        str_0_A_xScale,
        str_1_D_rotationTerms_1,
        str_2_B_rotationTerms_2,
        str_3_E_yScale,
        str_4_C_xTranslationTerm,
        str_5_F_yTranslationTerm
    };

    for (int i = 0; i < 6; i++) {
        if (_WFparameters_seenAsZero._raw._boolArray[i]) {
            qjsonObj.insert(strKey_WFparameters[i], 0);
        } else {
            qjsonObj.insert(strKey_WFparameters[i], /*doubleToQStringPrecision(*/_WFparameters._raw._doubleArray[i]/*,15)*/);
        }
    }
    return(true);
}


bool WorldFile_parameters::fromQJsonObject(const QJsonObject &qjsonObj) {

    QString str_0_A_xScale           {"0_A_xScale"};
    QString str_1_D_rotationTerms_1  {"1_D_rotationTerms_1"};
    QString str_2_B_rotationTerms_2  {"2_B_rotationTerms_2"};
    QString str_3_E_yScale           {"3_E_yScale"};
    QString str_4_C_xTranslationTerm {"4_C_xTranslationTerm"};
    QString str_5_F_yTranslationTerm {"5_F_yTranslationTerm"};

    QString strKey_WFparameters[6] = {
        str_0_A_xScale,
        str_1_D_rotationTerms_1,
        str_2_B_rotationTerms_2,
        str_3_E_yScale,
        str_4_C_xTranslationTerm,
        str_5_F_yTranslationTerm
    };

    for (int i = 0; i < 6; i++) {
        auto qjsonObj_WFparameter_iter = qjsonObj.find(strKey_WFparameters[i]);
        if (qjsonObj_WFparameter_iter == qjsonObj.end()) {
            qDebug() << __FUNCTION__ << " error: key not found: " << strKey_WFparameters[i];
            return(false);
        }
        double WFparameter_doubleValue = qjsonObj_WFparameter_iter.value().toDouble();

        qDebug() << __FUNCTION__ << strKey_WFparameters[i];

        show_WFParameters();

        _WFparameters_seenAsZero._raw._boolArray[i] = valueIsCloseToZero_deltaLowerThan1ExpMinus14(WFparameter_doubleValue);
        if (_WFparameters_seenAsZero._raw._boolArray[i]) {
            _WFparameters._raw._doubleArray[i] = .0;
        } else {
            _WFparameters._raw._doubleArray[i] = WFparameter_doubleValue;
        }
    }

    computeFlag_bWFparametersCanBeUsed();
    return(true);
}



void WorldFile_parameters::showContent() const {
    qDebug() << "WorldFile_parameters::" << __FUNCTION__ << "_qstrWorldFilePathFilename = " << _qstrWorldFilePathFilename;
    qDebug() << "WorldFile_parameters::" << __FUNCTION__ << "_qstrAssociatedImagePathFilename = " << _qstrAssociatedImagePathFilename;
    qDebug() << "WorldFile_parameters::" << __FUNCTION__ << "_bWFparametersCanBeUsed =" << _bWFparametersCanBeUsed;
    show_WFParameters();
    for (int i = 0; i < 6; i++) {
        qDebug() << "WorldFile_parameters::" << __FUNCTION__
                 << "_WFparameters_seenAsZero._raw._boolArray[" << i << "] = "
                 <<  _WFparameters_seenAsZero._raw._boolArray[i];
    }
}

#include <string>
#include <iostream>
#include <sstream>
void WorldFile_parameters::show_WFParameters() const {

    qDebug() << "WorldFile_parameters::" << __FUNCTION__;
    //cout << __FUNCTION__;
    //cout << endl;

    std::stringstream strStream;
    strStream << "_xScale = " << std::setprecision(15) << _WFparameters._parameters._xScale << "; ";

    strStream << "_rotationTerms_1 = "  << std::setprecision(15) << _WFparameters._parameters._rotationTerms_1 << "; ";

    strStream << "_rotationTerms_2 = "  << std::setprecision(15) << _WFparameters._parameters._rotationTerms_2 << "; ";

    strStream << "_yScale = "  << std::setprecision(15) << _WFparameters._parameters._yScale << "; ";

    strStream << "_xTranslationTerm = "  << std::setprecision(15) << _WFparameters._parameters._xTranslationTerm_centerOfUpperLeftPixel << "; ";

    strStream << "_yTranslationTerm = "  << std::setprecision(15) << _WFparameters._parameters._yTranslationTerm_centerOfUpperLeftPixel << "; ";

    qDebug() << "WorldFile_parameters::" << __FUNCTION__ << QString::fromStdString(strStream.str()); strStream.clear();


    qDebug() << __FUNCTION__ << "using doubleToQStringPrecision:";
    qDebug() << __FUNCTION__ << doubleToQStringPrecision_f_amountOfDecimal(_WFparameters._parameters._xScale, 15);
    qDebug() << __FUNCTION__ << doubleToQStringPrecision_f_amountOfDecimal(_WFparameters._parameters._rotationTerms_1, 15);
    qDebug() << __FUNCTION__ << doubleToQStringPrecision_f_amountOfDecimal(_WFparameters._parameters._rotationTerms_2, 15);
    qDebug() << __FUNCTION__ << doubleToQStringPrecision_f_amountOfDecimal(_WFparameters._parameters._yScale, 15);
    qDebug() << __FUNCTION__ << doubleToQStringPrecision_f_amountOfDecimal(_WFparameters._parameters._xTranslationTerm_centerOfUpperLeftPixel, 15);
    qDebug() << __FUNCTION__ << doubleToQStringPrecision_f_amountOfDecimal(_WFparameters._parameters._yTranslationTerm_centerOfUpperLeftPixel, 15);
    qDebug() << __FUNCTION__ << "___";


    /*
    //avoiding qDebug to display double ( https://stackoverflow.com/questions/39146527/can-you-set-the-qdebug-floating-point-precision-and-number-format-globally/39147947 )
    cout << "_xScale = " << std::setprecision(15) << _WFparameters._parameters._xScale << endl;
    cout << endl;
    cout << "_rotationTerms_1 = "  << std::setprecision(15) << _WFparameters._parameters._rotationTerms_1 << endl;
    cout << "_rotationTerms_2 = "  << std::setprecision(15) << _WFparameters._parameters._rotationTerms_2 << endl;
    cout << endl;
    cout << "_yScale = "  << std::setprecision(15) << _WFparameters._parameters._yScale << endl;
    cout << endl;
    cout << "_xTranslationTerm = "  << std::setprecision(15) << _WFparameters._parameters._xTranslationTerm_centerOfUpperLeftPixel << endl;
    cout << "_yTranslationTerm = "  << std::setprecision(15) << _WFparameters._parameters._yTranslationTerm_centerOfUpperLeftPixel << endl;
    cout << endl;
    */
}

bool WorldFile_parameters::checkSame(const WorldFile_parameters& wFparamForCheck) const {

    //avoid compare not usable WorldFile_parameters
    if (  (!_bWFparametersCanBeUsed)
        ||(!wFparamForCheck._bWFparametersCanBeUsed)) {
        qDebug() <<__FUNCTION__ << " warning: can not compare WorldFile_parameters: some _bWFparametersCanBeUsed set to false";
        return(false);
    }

    const int wFparametersCount= 6;

    //compare _WFparameters_seenAsZero
    for (int idx = 0; idx < wFparametersCount; idx++) {
        if (_WFparameters_seenAsZero._raw._boolArray[idx] != wFparamForCheck._WFparameters_seenAsZero._raw._boolArray[idx]) {
            qDebug() <<__FUNCTION__ << " _WFparameters_seenAsZero not equal [ " << idx << " ]";
            return(false);
        }
    }

    //compare U_WFparameters, but for_WFparameters_seenAsZero at false only
    for (int idx = 0; idx < wFparametersCount; idx++) {

        if (!_WFparameters_seenAsZero._raw._boolArray[idx]) {
            if (!valueIsCloseToZero_deltaLowerThan1ExpMinus14(_WFparameters._raw._doubleArray[idx] - wFparamForCheck._WFparameters._raw._doubleArray[idx])) {
                qDebug() <<__FUNCTION__ << " _WFparameters not equal [ " << idx
                                        << " ] : " << _WFparameters._raw._doubleArray[idx]
                                        << " != "  << wFparamForCheck._WFparameters._raw._doubleArray[idx];
                return(false);
            }
        }
    }

    return(true);
}



bool WorldFile_parameters::loadWFParametersFromWorldFile(QString qstrFilename, U_WFparameters &TFWparameters, U_WFparameters_seenAsZero &TFWparameters_seenAsZero) {

    std::memset(&TFWparameters, 0, sizeof(U_WFparameters));
    std::memset(&TFWparameters_seenAsZero, 0, sizeof(TFWparameters_seenAsZero));

    QFile qf_TFW(qstrFilename);
    //###LP add a link follow has the tiff can be a link also
    bool bOpen = qf_TFW.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!bOpen) {
        qDebug() << __FUNCTION__ << " WorldFile_parameters::" << __FUNCTION__ << "failed to open " << qstrFilename;

        qDebug() << __FUNCTION__ << "error: " << qf_TFW.error();
        qDebug() << __FUNCTION__ << "errorString: " << qf_TFW.errorString();
        return(false);
    }

    bool bError = false;
    for (int iLine = 0; iLine < 6; iLine++) {


        #define DEF_readBufferByteSize 128
        char readBuffer[DEF_readBufferByteSize];

        std::memset(readBuffer, 0, DEF_readBufferByteSize);
        qint64 nbBytesRead = qf_TFW.readLine(readBuffer, DEF_readBufferByteSize-1);

        if (nbBytesRead <= 0) { //-1 as error; 0 as 0 bytes read
            qDebug() << __FUNCTION__ << " WorldFile_parameters::" << __FUNCTION__ << "readLine error: Line #" << iLine+1;
            bError= true;
            break;
        }

        QString qtrParameterValue(readBuffer);

        //detect zero value from text

        qDebug() << __FUNCTION__ << " before trim: [" << qtrParameterValue << "]";
        qtrParameterValue.remove(QChar('\n'), Qt::CaseInsensitive);
        qtrParameterValue.remove(QChar('\r'), Qt::CaseInsensitive);
        qtrParameterValue.remove(QChar(' '), Qt::CaseInsensitive);
        qtrParameterValue.remove(QChar('\t'), Qt::CaseInsensitive);
        qDebug() << __FUNCTION__ << " trimmed: [" << qtrParameterValue << "]";

        int qstrSize_afterTrim = qtrParameterValue.size();
        if (!qstrSize_afterTrim) {
            qDebug() << __FUNCTION__ << " WorldFile_parameters::" << __FUNCTION__ << " error: qstrSize_afterTrim is zero. Line #" << iLine+1;
            bError= true;
            break;
        }
        int countZeroAndDot = 0;
        for (auto iter: qtrParameterValue) {
            if ((iter == '0')||(iter == '.')) {
                countZeroAndDot++;
            }
        }

        qDebug() << __FUNCTION__ << " qstrSize_afterTrim = " << qstrSize_afterTrim;
        qDebug() << __FUNCTION__ << " countZeroAndDot = " << countZeroAndDot;
        if (countZeroAndDot == qstrSize_afterTrim) {
           TFWparameters_seenAsZero._raw._boolArray[iLine] = true;
           TFWparameters._raw._doubleArray[iLine] = .0;
           qDebug() << __FUNCTION__ << " seenAsZero (iLine = " << iLine;
        } else {
            qDebug() << __FUNCTION__ << " seen not Zero (iLine = " << iLine;
            TFWparameters_seenAsZero._raw._boolArray[iLine] = false;
            bool bConvOk = true;
            TFWparameters._raw._doubleArray[iLine] = qtrParameterValue.toDouble(&bConvOk);
            if (!bConvOk) {
                qDebug() << __FUNCTION__ << " WorldFile_parameters::" << __FUNCTION__ << " error: toDouble. Line #" << iLine+1;
                bError= true;
                break;
            }
        }
    }

    //qf_TFW.close();  //no needs; RAII
    return(!bError);
}

