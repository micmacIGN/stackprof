#ifndef WIDGET_STACKEDPROFILBOX_EDITSIZE_H
#define WIDGET_STACKEDPROFILBOx_EDITSIZE_H

#include <QWidget>
#include <QLineEdit>

namespace Ui {
class Widget_stackedProfilBox_editSize;
}

class Widget_stackedProfilBox_editSize : public QWidget
{
    Q_OBJECT

public:
    enum e_Page_withWithoutSetButton {
        page_withSetButton = 0,
        page_withoutSetButton
    };

    void setPage(e_Page_withWithoutSetButton ePage_withWithoutSetButton);

    void setFocusToFirstEditableWidget();

    explicit Widget_stackedProfilBox_editSize(QWidget *parent = nullptr);
    ~Widget_stackedProfilBox_editSize() override;

    //void showEvent(QShowEvent *event) override;

    void set_widthLength(int width, int length);
    void get_widthLength(int& width, int& length);

    bool getWidthLengthValidForComputation();

    void setNoValues();

signals:

    void signal_editedWidthLengthValidity(bool bAllValidForComputation, int oddPixelWidth, int oddPixelLength);

    void signal_pushButton_set_clicked(int oddPixelWidth, int oddPixelLength);

public slots:
    void slot_lineText_textEdited_width(const QString &text);
    void slot_lineText_textEdited_length(const QString &text);
    void slot_pushButton_set_clicked(bool checked);

private:
    bool pixelLength_validForComputation(int pixelLength);
    bool pixelWidth_validForComputation(int pixelWidth);

    bool setPixelWidthFromStr_ifValidForComputation(const QString& strText);
    bool setPixelLengthFromStr_ifValidForComputation(const QString& strText);

    void lineText_textEdited(QLineEdit *ptrLineEdit, bool bValidForComputation);

    void updateEnableState_of_setPushButton(bool bAllValidForComputation);

private:

    Ui::Widget_stackedProfilBox_editSize *ui;

    e_Page_withWithoutSetButton _ePage_withWithoutSetButton;

    int _oddPixelWidth;
    int _oddPixelLength;

    bool _bWidthValidForComputation;
    bool _bLengthValidForComputation;

};

#endif // WIDGET_STACKEDPROFILBOX_EDITSIZE_H
