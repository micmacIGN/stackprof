#include <QDebug>

#include "dialog_taskinprogress.h"
#include "ui_dialog_taskinprogress.h"

#include <Qt>

Dialog_taskInProgress::Dialog_taskInProgress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_taskInProgress) {

    ui->setupUi(this);

    //setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);
    setDefaultState();

}

#include <QTimer>

#include <QDateTime>

void Dialog_taskInProgress::setInitialState(
    e_Page_dialogTaskInProgress ePage_dialogTaskInProgress,
    QString strBoxTitle, QString strProgressMessage,
    int progressValueMin, int progressValueMax) {

    ui->stackedWidget->setCurrentIndex(ePage_dialogTaskInProgress);

    setWindowTitle(strBoxTitle);
    ui->label_task->setText(strProgressMessage);

    _progressValueMin = progressValueMin;
    _progressValueMax = progressValueMax;
    _currentProgressValue = _progressValueMin;

    ui->progressBar->setEnabled(true);
    ui->progressBar->setRange(_progressValueMin,_progressValueMax);
    ui->progressBar->reset();

    qDebug() << __FUNCTION__ << "(cmsse)" << QDateTime::currentMSecsSinceEpoch();
    //QTimer::singleShot(500, this, &Dialog_taskInProgress::open);
    //QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    //open();

    //QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

void Dialog_taskInProgress::closeAndClear() {
    qDebug() << __FUNCTION__ << "entering";
    setDefaultState();
    qDebug() << __FUNCTION__ << "after setDefaultState";

    qDebug() << __FUNCTION__ << "(cmsse)" << QDateTime::currentMSecsSinceEpoch();
    //QTimer::singleShot(500, this, &Dialog_taskInProgress::accept);

    //done(QDialog::Accepted);
    accept();//close();

    //QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    qDebug() << __FUNCTION__ << "(cmsse)" << QDateTime::currentMSecsSinceEpoch();

    qDebug() << __FUNCTION__ << "end of method";
}

void Dialog_taskInProgress::reject() {
    qDebug() << __FUNCTION__ << "(cmsse)" << QDateTime::currentMSecsSinceEpoch();
    qDebug() << __FUNCTION__ << "(Dialog_taskInProgress) before QDialog::reject();";
    QDialog::reject();
    qDebug() << __FUNCTION__ << "(Dialog_taskInProgress) after  QDialog::reject();";
}

void Dialog_taskInProgress::accept() {
    qDebug() << __FUNCTION__ << "(cmsse)" << QDateTime::currentMSecsSinceEpoch();
    qDebug() << __FUNCTION__ << "(Dialog_taskInProgress) before QDialog::accept()";
    QDialog::accept();
    qDebug() << __FUNCTION__ << "(Dialog_taskInProgress) after  QDialog::accept()";
}

void Dialog_taskInProgress::setProgressState(int progressValue) {
    qDebug() << __FUNCTION__ << "progressValue = " << progressValue;
    qDebug() << __FUNCTION__ << "(cmsse)" << QDateTime::currentMSecsSinceEpoch();
    _currentProgressValue = progressValue;    
    ui->progressBar->setValue(_currentProgressValue);
    //QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

Dialog_taskInProgress::~Dialog_taskInProgress() {
    delete ui;
}

void Dialog_taskInProgress::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    qDebug() << __FUNCTION__ << "(Dialog_taskInProgress)";
}

void Dialog_taskInProgress::setDefaultState(e_Page_dialogTaskInProgress ePage_dialogTaskInProgress) {

    setWindowTitle("task progression");

    ui->stackedWidget->setCurrentIndex(ePage_dialogTaskInProgress);

    ui->label_task->setText("task in progress...");

    _progressValueMin = 0;
    _progressValueMax = 100;
    _currentProgressValue = 0;

    ui->progressBar->reset();
    ui->progressBar->setRange(_progressValueMin,_progressValueMax);
    ui->progressBar->setEnabled(false);
}

