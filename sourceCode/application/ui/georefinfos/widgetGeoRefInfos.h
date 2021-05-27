#ifndef WIDGETGEOREFINFOS_H
#define WIDGETGEOREFINFOS_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

namespace Ui {
    class widgetGeoRefInfos;
}

class WidgetGeoRefInfos : public QWidget
{
    Q_OBJECT

public:
    WidgetGeoRefInfos(QWidget *parent = nullptr);
    ~WidgetGeoRefInfos();

    void setWorldFileDataFromStr(const QVector<QString>& qvecstrWorldFileParameters);
    void setEPSGCodeFromStr(QString strEPSGCode);
    void setImagesFilename(const QVector<QString>& qvectstr_ImageFilenames_threeMax);

    void set_associatedNameToEPSGCode(const QString& strAssociatedNameToEPSGCode);

    void setModeToViewValuesOnly(bool bViewValuesOnly);

    void clearUiContent_all();

    void setStrGeoRefImagesSetStatus(
        QString strMsgAboutTFW,  QString strMsgAboutTFW_errorDetails,
        QString strMsgAboutEPSG, QString strMsgAboutEPSG_errorDetails);

    void setWidgetTextAndStyleAboutStatus(bool bWorldFileData_available, bool bEPSG_available);
    QPushButton* getPushButton_setEPSGCode() const;

    void setEPSGCodeAsEditedText_UserValidationRequiered(const QString& strEPSGCode);

public slots:
    void slot_lineEdit_EPSGCodeValue_textEdited(const QString& text);
    void slot_pushButton_setEPSGCode_clicked();

    void slot_pushButton_EPSGCode_selectFromlist_clicked();

signals:
    void signal_setEPSGCode(QString strEPSGCode);
    void signal_EPSGCodelineEditChanged();

    void signal_request_EPSGCodeSelectionFromlist();

private:
    void setEmpty_lineEditImageFilename();
    void clear_worldFileData();
    void clear_EPSGCodeline();

private:
    Ui::widgetGeoRefInfos *ui;

    bool _bViewValuesOnly;

    QLineEdit *_qLineEditImageFilenamePtr[3];
};


#endif // WIDGETGEOREFINFOS_H


