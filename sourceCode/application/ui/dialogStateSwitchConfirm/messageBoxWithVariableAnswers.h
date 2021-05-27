#ifndef MessageBoxWithVariableAnswers_H
#define MessageBoxWithVariableAnswers_H

#include <QMessageBox>

#include <QPushButton>

class MessageBoxWithVariableAnswers {

public:

    enum e_MessageBoxPossibleAnswers {
        e_MBPA_YesNo,
        e_MBPA_OKCancel,
        e_MBPA_ResetKeepCancel,
        e_MBPA_EditQuit,
    };

    explicit MessageBoxWithVariableAnswers(
        const QString& strWindowTitle,
        const QString& strMessage,
        const QString& strQuestion,
        e_MessageBoxPossibleAnswers eMessageBoxPossibleAnswers);

    int exec();

    //~MessageBoxWithVariableQuestionAnswers();

private:

    e_MessageBoxPossibleAnswers _eMessageBoxPossibleAnswers;

    QMessageBox _qMsgBox;

    QPushButton *_ptrQPushButtonKeep;

    QPushButton *_ptrQPushButtonEdit;
    QPushButton *_ptrQPushButtonQuit;

};

#endif // MessageBoxWithVariableAnswers_H
