#include <QDebug>

#include "widgetcurrentboxidedittoreach.h"
#include "ui_widgetcurrentboxidedittoreach.h"

widgetCurrentBoxIdEditToReach::widgetCurrentBoxIdEditToReach(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetCurrentBoxIdEditToReach)
{
    ui->setupUi(this);

    _currentPage = 0;

    connect(ui->pushbutton_flat_current_slash_max, &QPushButton::clicked,
            this, &widgetCurrentBoxIdEditToReach::slot_pushbutton_current_slash_max_clicked);

    connect(ui->pushbutton_flat_slash_max, &QPushButton::clicked,
            this, &widgetCurrentBoxIdEditToReach::slot_pushbutton_pushbutton_flat_slash_max);

    connect(ui->lineEdit_boxIDToReach, &QLineEdit::textEdited,
            this, &widgetCurrentBoxIdEditToReach::slot_lineEdit_boxIDToReach_textEdited);

    connect(ui->lineEdit_boxIDToReach, &QLineEdit::returnPressed,
            this, &widgetCurrentBoxIdEditToReach::slot_lineEdit_boxIDToReach_returnPressed);


    setCurrentAndBoxCount(-1,-1);
}

void widgetCurrentBoxIdEditToReach::setCurrentAndBoxCount(int currentBoxId, int boxCount) {
   _currentBoxId = currentBoxId;
   _boxCount = boxCount;

   if (  (_currentBoxId == -1)
       ||(_boxCount == -1)) {

       ui->pushbutton_flat_current_slash_max->setText("---/---");
       ui->pushbutton_flat_slash_max->setText("/---");

       ui->pushbutton_flat_current_slash_max->setEnabled(false);
       _currentPage = 0;
       ui->stackedWidget->setCurrentIndex(_currentPage);

   } else {
       ui->pushbutton_flat_current_slash_max->setText(QString::number(currentBoxId+1) + "/" + QString::number(boxCount)) ;
       ui->lineEdit_boxIDToReach->setText(QString::number(currentBoxId+1));

       ui->pushbutton_flat_slash_max->setText("/" + QString::number(boxCount));

       ui->pushbutton_flat_current_slash_max->setEnabled(true);
       _currentPage = 0;
       ui->stackedWidget->setCurrentIndex(_currentPage);
   }
   _boxIdInEditionWaitingUserValidation = -1;

}

void widgetCurrentBoxIdEditToReach::slot_pushbutton_current_slash_max_clicked() {
    _boxIdInEditionWaitingUserValidation = _currentBoxId;

    _currentPage = 1;
    ui->stackedWidget->setCurrentIndex(_currentPage);
    ui->lineEdit_boxIDToReach->setFocus();


    ui->lineEdit_boxIDToReach->setStyleSheet("");
}

void widgetCurrentBoxIdEditToReach::slot_pushbutton_pushbutton_flat_slash_max() {
    _currentPage = 0;
    ui->stackedWidget->setCurrentIndex(_currentPage);
    ui->pushbutton_flat_current_slash_max->setFocus();
}

void widgetCurrentBoxIdEditToReach::slot_lineEdit_boxIDToReach_textEdited(const QString &text) {

    bool bOk = false;
    int intEditedBoxId = text.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in text.toInt()";
        return;
    }

    if (  (intEditedBoxId < 1)
        ||(intEditedBoxId > _boxCount)) {
        ui->lineEdit_boxIDToReach->setStyleSheet("QLineEdit { color : red; }");
        _boxIdInEditionWaitingUserValidation = -1;
        return;
    }

    ui->lineEdit_boxIDToReach->setStyleSheet("");
    _boxIdInEditionWaitingUserValidation = intEditedBoxId-1;
}

void widgetCurrentBoxIdEditToReach::slot_lineEdit_boxIDToReach_returnPressed() {
    if (_boxIdInEditionWaitingUserValidation != -1) {
        emit signal_boxIdToReachFromEditedId(_boxIdInEditionWaitingUserValidation);
    }
}

widgetCurrentBoxIdEditToReach::~widgetCurrentBoxIdEditToReach() {
    delete ui;
}
