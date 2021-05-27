#include <QDebug>
#include <QVector>
#include <QString>

#include "widgetGeoRefInfos.h"

#include "ui_widgetgeorefinfos.h"

WidgetGeoRefInfos::WidgetGeoRefInfos(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widgetGeoRefInfos),
    _qLineEditImageFilenamePtr{nullptr, nullptr, nullptr}

{
    ui->setupUi(this);
    ui->pushButton_setEPSGCode->setEnabled(false);

    _qLineEditImageFilenamePtr[0] = ui->lineEdit_image1;
    _qLineEditImageFilenamePtr[1] = ui->lineEdit_image2;
    _qLineEditImageFilenamePtr[2] = ui->lineEdit_image3;

    setEmpty_lineEditImageFilename();

    connect(ui->lineEdit_EPSGCode_value, &QLineEdit::textEdited, this, &WidgetGeoRefInfos::slot_lineEdit_EPSGCodeValue_textEdited);
    connect(ui->pushButton_setEPSGCode, &QPushButton::clicked, this, &WidgetGeoRefInfos::slot_pushButton_setEPSGCode_clicked);

    //ui->pushButton_EPSGCode_selectFromlist->setEnabled(false);
    connect(ui->pushButton_EPSGCode_selectFromlist, &QPushButton::clicked,
            this, &WidgetGeoRefInfos::slot_pushButton_EPSGCode_selectFromlist_clicked);

    setModeToViewValuesOnly(false);
}

void WidgetGeoRefInfos::setModeToViewValuesOnly(bool bViewValuesOnly) {
    _bViewValuesOnly = bViewValuesOnly;
    ui->lineEdit_EPSGCode_value->setReadOnly(_bViewValuesOnly);
    ui->pushButton_setEPSGCode->setVisible(!_bViewValuesOnly);
    ui->pushButton_EPSGCode_selectFromlist->setVisible(!_bViewValuesOnly);
    ui->label_status_aboutEPSGCode->setVisible(!_bViewValuesOnly);
}

void WidgetGeoRefInfos::clearUiContent_all() {
   setEmpty_lineEditImageFilename();
   clear_worldFileData();
   clear_EPSGCodeline();

   //back to default color
   ui->label_status_aboutEPSGCode->setStyleSheet("");
   ui->label_status_aboutWorldFileData->setStyleSheet("");
   ui->lineEdit_EPSGCode_value->setStyleSheet("");

}

void WidgetGeoRefInfos::setEmpty_lineEditImageFilename() {
    for (int i= 0; i < 3; i++) {
        if (_qLineEditImageFilenamePtr[i]) {
            _qLineEditImageFilenamePtr[i]->clear();
            _qLineEditImageFilenamePtr[i]->setEnabled(false);
        }
    }
}

void WidgetGeoRefInfos::clear_worldFileData() {
    ui->lineEdit_0A_scale_x_readOnly->clear();
    ui->lineEdit_1D_rotationTerms_1_readOnly->clear();
    ui->lineEdit_2B_rotationTerms_2_readOnly->clear();
    ui->lineEdit_3E_scale_y_readOnly->clear();
    ui->lineEdit_4C_centerOfUpperLeftPixel_x_readOnly->clear();
    ui->lineEdit_5F_centerOfUpperLeftPixel_y_readOnly->clear();
}

void WidgetGeoRefInfos::clear_EPSGCodeline() {
    ui->lineEdit_EPSGCode_value->clear();
    ui->label_associatedName->clear();
}

void WidgetGeoRefInfos::slot_pushButton_setEPSGCode_clicked() {
    emit signal_setEPSGCode(ui->lineEdit_EPSGCode_value->text());
}

WidgetGeoRefInfos::~WidgetGeoRefInfos() {
    delete ui;
}

QPushButton* WidgetGeoRefInfos::getPushButton_setEPSGCode() const {
    if (!ui) {
        return(nullptr);
    }
    return(ui->pushButton_setEPSGCode);
}

void WidgetGeoRefInfos::setWorldFileDataFromStr(const QVector<QString>& qvecstrWorldFileParameters) {

    if (qvecstrWorldFileParameters.size() != 6) {
        qDebug() << __FUNCTION__ << "error: qvecstrWorldFileParameters size is not 6";
        return;
    }

    ui->lineEdit_0A_scale_x_readOnly->setText(qvecstrWorldFileParameters.at(0));
    ui->lineEdit_1D_rotationTerms_1_readOnly->setText(qvecstrWorldFileParameters.at(1));
    ui->lineEdit_2B_rotationTerms_2_readOnly->setText(qvecstrWorldFileParameters.at(2));
    ui->lineEdit_3E_scale_y_readOnly->setText(qvecstrWorldFileParameters.at(3));
    ui->lineEdit_4C_centerOfUpperLeftPixel_x_readOnly->setText(qvecstrWorldFileParameters.at(4));
    ui->lineEdit_5F_centerOfUpperLeftPixel_y_readOnly->setText(qvecstrWorldFileParameters.at(5));

}

void WidgetGeoRefInfos::setEPSGCodeFromStr(QString strEPSGCode) {
    ui->lineEdit_EPSGCode_value->setText(strEPSGCode);
}

void WidgetGeoRefInfos::set_associatedNameToEPSGCode(const QString& strAssociatedNameToEPSGCode) {

    int lngStrAssociatedNameToEPSGCode = strAssociatedNameToEPSGCode.size();
    if (!lngStrAssociatedNameToEPSGCode) {
        ui->label_associatedName->setText("---");
        return;
    }

    int lngMaxStrWithDecoration = 53;
    QString strDecorationBefore = "(name: ";
    QString strDecorationAfterIfFull = ")";

    int lngDecorationIfFulll = strDecorationBefore.size() + strDecorationAfterIfFull.size();

    if (lngStrAssociatedNameToEPSGCode <=  lngMaxStrWithDecoration - lngDecorationIfFulll) {
        ui->label_associatedName->setText(strDecorationBefore + strAssociatedNameToEPSGCode + strDecorationAfterIfFull);
        return;
    }

    QString strDecorationAfterIfTroncated = "...)";
    int lngDecorationIfTroncated = strDecorationBefore.size() + strDecorationAfterIfTroncated.size();
    int lngKept = lngStrAssociatedNameToEPSGCode - lngDecorationIfTroncated;
    ui->label_associatedName->setText(strDecorationBefore + strAssociatedNameToEPSGCode.left(lngKept) + strDecorationAfterIfFull);
}

void WidgetGeoRefInfos::setImagesFilename(const QVector<QString>& qvectstr_ImageFilenames_threeMax) {
    int vectSize = qvectstr_ImageFilenames_threeMax.size();
    if (!vectSize) {
        setEmpty_lineEditImageFilename();
    }
    if (vectSize > 3) {
        ui->lineEdit_image1->setText("Dev Warning: WidgetGeoRefInfos received");
        ui->lineEdit_image2->setText("more than thress images filename.");
        ui->lineEdit_image3->setText("Three is the maximum for display.");

        ui->lineEdit_image1->setEnabled(true);
        ui->lineEdit_image2->setEnabled(true);
        ui->lineEdit_image3->setEnabled(true);
    } else {
        for (int i = 0; i < 3; i++) {
            if (i < vectSize) {
                _qLineEditImageFilenamePtr[i]->setText(qvectstr_ImageFilenames_threeMax[i]);
                _qLineEditImageFilenamePtr[i]->setEnabled(true);
            } else {
                _qLineEditImageFilenamePtr[i]->clear();
                _qLineEditImageFilenamePtr[i]->setEnabled(false);
            }
        }
    }
}


void WidgetGeoRefInfos::setStrGeoRefImagesSetStatus(
    QString strMsgAboutTFW,  QString strMsgAboutTFW_errorDetails,
    QString strMsgAboutEPSG, QString strMsgAboutEPSG_errorDetails)  {

    ui->label_status_aboutWorldFileData->setText(strMsgAboutTFW + strMsgAboutTFW_errorDetails);
    ui->label_status_aboutEPSGCode->setText(strMsgAboutEPSG + strMsgAboutEPSG_errorDetails);

}


void WidgetGeoRefInfos::setWidgetTextAndStyleAboutStatus(bool bWorldFileData_available, bool bEPSG_available) {
    if (bWorldFileData_available) {

       ui->label_status_aboutWorldFileData->setStyleSheet("");

    } else {

       ui->label_status_aboutWorldFileData->setStyleSheet("QLabel { color: rgb(245, 120, 0); }");
    }

    if (bEPSG_available) {

       ui->label_status_aboutEPSGCode->setStyleSheet("");
    } else {
       ui->label_status_aboutEPSGCode->setStyleSheet("QLabel { color: rgb(245, 120, 0); }");
    }
}

void WidgetGeoRefInfos::slot_lineEdit_EPSGCodeValue_textEdited(const QString& text) {

    emit signal_EPSGCodelineEditChanged(); //to disable the OK button of any surrounding QDialog

    bool bEPSGCodeContainsOnlyDigit =false;
    bool bColoredWarning = false;
    bool bColoredError = false;
    //https://gis.stackexchange.com/questions/290718/how-many-crs-epsg-codes-are-there
    //'There are 9.906 different spatial references on http://spatialreference.org/ref/?page=199 available.
    //The front page says "4362 EPSG references" but also the last update was in year 2013.

    //consider 50000 as maximum before colored 'warning alert'
    int maxAsColoredWarning = 50000;

    int edited_text_size = text.size();
    if (edited_text_size) {
        QString strDigitsOnly {"0123456789"};       
        bool bFoundOnlyDigit = true;
        for(auto c : text) {
            if (!strDigitsOnly.contains(c)) {
                bFoundOnlyDigit = false;
                break;
            }
        }
        bColoredError = !bFoundOnlyDigit;
        if (bFoundOnlyDigit) {
            bEPSGCodeContainsOnlyDigit = true;
            bool bOk = false;
            int intValue = text.toInt(&bOk);
            if (bOk) {
                bColoredWarning = (intValue >= maxAsColoredWarning);
            }
        }
    }

    ui->pushButton_setEPSGCode->setEnabled(bEPSGCodeContainsOnlyDigit);

    if (bColoredError) {
        ui->lineEdit_EPSGCode_value->setStyleSheet("QLineEdit { color: red; }");
    } else {
        if (bColoredWarning) {
            ui->lineEdit_EPSGCode_value->setStyleSheet("QLineEdit { color: rgb(245, 120, 0); }");
        } else {
            ui->lineEdit_EPSGCode_value->setStyleSheet("");
        }
    }

    ui->label_associatedName->clear();

}

void WidgetGeoRefInfos::slot_pushButton_EPSGCode_selectFromlist_clicked() {
    qDebug() << __FUNCTION__ << "(WidgetGeoRefInfos)";
    emit signal_request_EPSGCodeSelectionFromlist();
}

//we want that the user validates the EPSG code that he selected from the list
void WidgetGeoRefInfos::setEPSGCodeAsEditedText_UserValidationRequiered(const QString& strEPSGCode) {
    qDebug() << __FUNCTION__ << "(WidgetGeoRefInfos)";
    ui->lineEdit_EPSGCode_value->setText(strEPSGCode);
    slot_lineEdit_EPSGCodeValue_textEdited(strEPSGCode);

}

