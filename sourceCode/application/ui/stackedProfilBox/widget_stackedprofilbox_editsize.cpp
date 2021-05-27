#include <QLineEdit>
#include <QDebug>

#include "widget_stackedprofilbox_editsize.h"
#include "ui_widget_stackedprofilbox_editsize.h"

#include "../../logic/toolbox/toolbox_math.h"

Widget_stackedProfilBox_editSize::Widget_stackedProfilBox_editSize(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget_stackedProfilBox_editSize),
    _ePage_withWithoutSetButton(e_Page_withWithoutSetButton::page_withSetButton),
    _oddPixelWidth(0),
    _oddPixelLength(0),
    _bWidthValidForComputation(false),
    _bLengthValidForComputation(false)
{
    ui->setupUi(this);

    setPage(_ePage_withWithoutSetButton);

    setNoValues();

    connect(ui->lineEdit_width_withSB,  &QLineEdit::textEdited, this, &Widget_stackedProfilBox_editSize::slot_lineText_textEdited_width);
    connect(ui->lineEdit_length_withSB, &QLineEdit::textEdited, this, &Widget_stackedProfilBox_editSize::slot_lineText_textEdited_length);

    connect(ui->lineEdit_width_withoutSB,  &QLineEdit::textEdited, this, &Widget_stackedProfilBox_editSize::slot_lineText_textEdited_width);
    connect(ui->lineEdit_length_withoutSB, &QLineEdit::textEdited, this, &Widget_stackedProfilBox_editSize::slot_lineText_textEdited_length);

    connect(ui->pushButton_set, &QPushButton::clicked, this, &Widget_stackedProfilBox_editSize::slot_pushButton_set_clicked);

}

bool Widget_stackedProfilBox_editSize::getWidthLengthValidForComputation() {
    return(_bWidthValidForComputation && _bLengthValidForComputation);
}

void Widget_stackedProfilBox_editSize::setNoValues() {

    _oddPixelWidth = .0;
    ui->lineEdit_width_withSB   ->clear();
    ui->lineEdit_width_withoutSB->clear();

    _oddPixelLength = .0;
    ui->lineEdit_length_withSB   ->clear();
    ui->lineEdit_length_withoutSB->clear();

    _bWidthValidForComputation  = false;
    _bLengthValidForComputation = false;

    ui->pushButton_set->setEnabled(false);
}


void Widget_stackedProfilBox_editSize::setPage(e_Page_withWithoutSetButton ePage_withWithoutSetButton) {
    _ePage_withWithoutSetButton = ePage_withWithoutSetButton;
    ui->stackedWidget->setCurrentIndex(_ePage_withWithoutSetButton);
}


void Widget_stackedProfilBox_editSize::setFocusToFirstEditableWidget() {
    switch(_ePage_withWithoutSetButton) {
        case page_withSetButton:

            ui->label_length_withSB->setFocus();
            qDebug() << __FUNCTION__ << "ui->label_length_withSB->setFocus()";
            break;
        case page_withoutSetButton:

            ui->label_length_withoutSB->setFocus();
            qDebug() << __FUNCTION__ << "ui->label_length_withoutSB->setFocus();";
    }
}


void Widget_stackedProfilBox_editSize::get_widthLength(int& width, int& length) {
    width = static_cast<int>(_oddPixelWidth);
   length = static_cast<int>(_oddPixelLength);
}

void Widget_stackedProfilBox_editSize::set_widthLength(int width, int length) {

    _oddPixelWidth = width;
    ui->lineEdit_width_withSB   ->setText(QString::number(_oddPixelWidth));
    ui->lineEdit_width_withoutSB->setText(QString::number(_oddPixelWidth));

    _oddPixelLength = length;
    ui->lineEdit_length_withSB   ->setText(QString::number(_oddPixelLength));
    ui->lineEdit_length_withoutSB->setText(QString::number(_oddPixelLength));

    _bWidthValidForComputation = pixelWidth_validForComputation(_oddPixelWidth);
    lineText_textEdited(ui->lineEdit_width_withSB, _bWidthValidForComputation);

    _bLengthValidForComputation = pixelWidth_validForComputation(_oddPixelWidth);
    lineText_textEdited(ui->lineEdit_width_withoutSB, _bLengthValidForComputation);

    lineText_textEdited(ui->lineEdit_length_withoutSB, pixelLength_validForComputation(_oddPixelLength));
    lineText_textEdited(ui->lineEdit_length_withSB,    pixelLength_validForComputation(_oddPixelLength));
}

void Widget_stackedProfilBox_editSize::slot_lineText_textEdited_width(const QString &text) {
    QLineEdit *sender_as_QLineEdit = dynamic_cast<QLineEdit *>(sender());
    _bWidthValidForComputation = setPixelWidthFromStr_ifValidForComputation(text);
    lineText_textEdited(sender_as_QLineEdit, _bWidthValidForComputation);
}

void Widget_stackedProfilBox_editSize::slot_lineText_textEdited_length(const QString &text) {
    QLineEdit *sender_as_QLineEdit = dynamic_cast<QLineEdit *>(sender());
    _bLengthValidForComputation = setPixelLengthFromStr_ifValidForComputation(text);
    lineText_textEdited(sender_as_QLineEdit, _bLengthValidForComputation);
}

bool Widget_stackedProfilBox_editSize::setPixelWidthFromStr_ifValidForComputation(const QString& strText) {
    bool bOK = false;
    int intValue = strText.toInt(&bOK);
    if (!bOK) {
        return(false);
    }
    bool bCompValid = pixelWidth_validForComputation(intValue);
    if (bCompValid) {
        _oddPixelWidth = intValue;
    }
    return(bCompValid);
}

bool Widget_stackedProfilBox_editSize::setPixelLengthFromStr_ifValidForComputation(const QString& strText) {
    bool bOK = false;
    int intValue = strText.toInt(&bOK);
    if (!bOK) {
        return(false);
    }
    bool bCompValid = pixelLength_validForComputation(intValue);
    if (bCompValid) {
        _oddPixelLength = intValue;
    }
    return(bCompValid);
}

bool Widget_stackedProfilBox_editSize::pixelWidth_validForComputation(int pixelWidth) {
    qDebug() << __FUNCTION__ << "pixelWidth = " << pixelWidth;

    bool bEven = bCheckDoubleIsEven(static_cast<double>(pixelWidth));
    if (bEven) {
        qDebug() << __FUNCTION__ << "if (bEven) {";
        return(false);
    }
    bool bEqualOrGreaterThanOne = (pixelWidth >= 1);

    qDebug() << __FUNCTION__ << "bEqualOrGreaterThanOne = " << bEqualOrGreaterThanOne;
    return(bEqualOrGreaterThanOne);
}

bool Widget_stackedProfilBox_editSize::pixelLength_validForComputation(int pixelLength) {
    qDebug() << __FUNCTION__ << "pixelLength = " << pixelLength;

    bool bEven = bCheckDoubleIsEven(static_cast<double>(pixelLength));
    if (bEven) {
        qDebug() << __FUNCTION__ << "if (bEven) {";
        return(false);
    }
    bool bEqualOrGreaterThanThree = (pixelLength >= 3);

    qDebug() << __FUNCTION__ << "bEqualOrGreaterThanThree = " << bEqualOrGreaterThanThree;
    return(bEqualOrGreaterThanThree);
}


//push button is enabled when the values width and depth are odd
void Widget_stackedProfilBox_editSize::slot_pushButton_set_clicked(bool checked) {
    qDebug() << __FUNCTION__ ;
    emit signal_pushButton_set_clicked(_oddPixelWidth, _oddPixelLength);
}

void Widget_stackedProfilBox_editSize::lineText_textEdited(QLineEdit *ptrLineEdit, bool bValidForComputation) {
    if (bValidForComputation) {

         ptrLineEdit->setStyleSheet("");
    } else {
        ptrLineEdit->setStyleSheet("QLineEdit { color : red; }");
    }

    bool bAllValidForComputation = _bWidthValidForComputation && _bLengthValidForComputation;

    qDebug() << __FUNCTION__ << "_bWidthValidForComputation  =" << _bWidthValidForComputation;
    qDebug() << __FUNCTION__ << "_bLengthValidForComputation =" << _bLengthValidForComputation;
    qDebug() << __FUNCTION__ << "bAllValidForComputation =" << bAllValidForComputation;


    updateEnableState_of_setPushButton(bAllValidForComputation);
}

void Widget_stackedProfilBox_editSize::updateEnableState_of_setPushButton(bool bAllValidForComputation) {
    ui->pushButton_set->setEnabled(bAllValidForComputation);
    emit signal_editedWidthLengthValidity(bAllValidForComputation, _oddPixelWidth, _oddPixelLength);
}

Widget_stackedProfilBox_editSize::~Widget_stackedProfilBox_editSize() {
    delete ui;
}
