#include <QString>

#include <QDebug>

#include "messageBoxWithVariableAnswers.h"

MessageBoxWithVariableAnswers::MessageBoxWithVariableAnswers(
    const QString& strWindowTitle,
    const QString& strMessage,
    const QString& strQuestion,
    e_MessageBoxPossibleAnswers eMessageBoxPossibleAnswers):
        _ptrQPushButtonKeep(nullptr),
        _ptrQPushButtonEdit(nullptr),
        _ptrQPushButtonQuit(nullptr)
    {

    _eMessageBoxPossibleAnswers = eMessageBoxPossibleAnswers;

    _qMsgBox.setWindowTitle(strWindowTitle);
    _qMsgBox.setText(strMessage);
    _qMsgBox.setInformativeText(strQuestion);
    _qMsgBox.setIcon(QMessageBox::Question);

    switch (_eMessageBoxPossibleAnswers) {
        case e_MBPA_YesNo:
            _qMsgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
            _qMsgBox.setDefaultButton(QMessageBox::No);
            break;

        case e_MBPA_OKCancel:
            _qMsgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            _qMsgBox.setDefaultButton(QMessageBox::Cancel);
            break;

        case e_MBPA_ResetKeepCancel:
           _qMsgBox.addButton(QMessageBox::RestoreDefaults);
           _ptrQPushButtonKeep = _qMsgBox.addButton("Keep", QMessageBox::AcceptRole);
           _qMsgBox.addButton(QMessageBox::Cancel);
            break;

        case e_MBPA_EditQuit:
           _ptrQPushButtonEdit = _qMsgBox.addButton("Edit", QMessageBox::AcceptRole);
           _ptrQPushButtonQuit = _qMsgBox.addButton("Quit",QMessageBox::RejectRole);
            break;
    }
}

int MessageBoxWithVariableAnswers::exec() {
    int iReturnCode = _qMsgBox.exec();
    return(iReturnCode);
}
