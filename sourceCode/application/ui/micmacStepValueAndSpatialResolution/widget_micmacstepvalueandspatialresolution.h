#ifndef WIDGET_MICMACSTEPVALUEANDSPATIALRESOLUTION_H
#define WIDGET_MICMACSTEPVALUEANDSPATIALRESOLUTION_H

#include <QWidget>
#include <QLineEdit>

#include "ui_enums.hpp"

namespace Ui {
class Widget_MicMacStepValueAndSpatialResolution;
}

class Widget_MicMacStepValueAndSpatialResolution : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_MicMacStepValueAndSpatialResolution(QWidget *parent = nullptr);
    ~Widget_MicMacStepValueAndSpatialResolution();

    void setToNoValues();

    void setFileTitlemode(e_UiFileTitleMode eUiFileTitleMode);

    void setFileTitle(const QString& qStrFileTitle);
    void setFilename(const QString& qStrFilename);    

    void setFromQStr_micMacStepAndSpatialResolution(const QString& qstrMicMacStep, const QString& qstrSpatialResolution);


    bool inputValid(bool& bMicMacStepValid, bool& bSpatialResolutionValid);


    bool get_micMacStep(float& fMicMacStep) const;
    bool get_spatialResolution(float& fSpatialResolution) const;

    bool get_micMacStep_asString(QString& fMicMacStep) const;
    bool get_spatialResolution_asString(QString& fSpatialResolution) const;

public slots:
    void slot_lineEdit_micMacStep_textEdited(const QString &text);
    void slot_lineEdit_spatialResolution_textEdited(const QString &text);

signals:
    //for outside:
    void signal_lineEdit_aboutMicMacStepOrSpatialResolution_validityChanged();

private:

    enum e_Page_wFileTitle {
        page_fileWithSmallTitle = 0,
        page_fileNoTitle
    };

    void setCurrentIndex(e_Page_wFileTitle ePage_wFileTitle);

    bool updateFieldAndWidget(const QString &text, QLineEdit *qlineEditPtr, QString &qstr, bool &bValid);

    void lineEdit_aboutSpatialResolution_validityChanged();
    void lineEdit_aboutMicMacStep_validityChanged();

private:
    Ui::Widget_MicMacStepValueAndSpatialResolution *ui;

    e_UiFileTitleMode _eUiFileTitleMode;

    QLineEdit* _qlineEditPtr_micMacStep;
    QLineEdit* _qlineEditPtr_spatialResolution;

    bool _bMicMacStepValid;
    bool _bSpatialResolutionValid;

    QString _qstrMicMacStep;
    QString _qstrSpatialResolution;
};


#endif // WIDGET_MICMACSTEPVALUEANDSPATIALRESOLUTION_H
