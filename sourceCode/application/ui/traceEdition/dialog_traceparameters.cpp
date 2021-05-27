#include <QPushButton>

#include <QDebug>

#include "../logic/toolbox/toolbox_string.h"

#include "dialog_traceparameters.h"
#include "ui_dialog_traceparameters.h"

Dialog_traceParameters::Dialog_traceParameters(int idRoute, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_traceParameters),
    _bWarned(false)
{
    ui->setupUi(this);

    ui->label_routeID->setText("Trace #"+ QString::number(idRoute));
    ui->lineEdit_routeName->setFocus();

    connect(ui->lineEdit_routeName, &QLineEdit::textEdited, this, &Dialog_traceParameters::slot_lineEdit_routeName_textEdited);
}


void Dialog_traceParameters::slot_lineEdit_routeName_textEdited(const QString &text) {

    qDebug() << __FUNCTION__ << "text = " << text;

    checkStringValidAndUpdateButtonOK(text, true);
}

void Dialog_traceParameters::checkStringValidAndUpdateButtonOK(const QString &text, bool bUsePreviousWarningState) {

    bool bMatch = stringValidAs_azAZ09_dotSpace(text);

    bool bUpdateWarned = true;

    if (!bMatch) {
        if (bUsePreviousWarningState) {
            if (_bWarned) {
                bUpdateWarned = false;
            }
        }
        if (bUpdateWarned) {
            _bWarned = true;
            ui->lineEdit_routeName->setStyleSheet("QLineEdit { color : red; }");
        }
    } else {
        if (bUsePreviousWarningState) {
            if (!_bWarned) {
                bUpdateWarned = false;
            }
        }
        if (bUsePreviousWarningState) {
            _bWarned = false;

            ui->lineEdit_routeName->setStyleSheet("");
        }
    }

    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(!_bWarned);
}

Dialog_traceParameters::~Dialog_traceParameters() {
    delete ui;
}

QString Dialog_traceParameters::getEditedName() {
    if (!ui) {
        return("");
    }
    return(ui->lineEdit_routeName->text());
}


void Dialog_traceParameters::setRouteName(const QString& strRouteName) {
    ui->lineEdit_routeName->setText(strRouteName);
    Dialog_traceParameters::checkStringValidAndUpdateButtonOK(strRouteName, false);
    ui->lineEdit_routeName->setFocus();
}

