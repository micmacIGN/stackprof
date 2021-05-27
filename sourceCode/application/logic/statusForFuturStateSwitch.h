#ifndef STATUSFORFUTURSTATESWITCH_H
#define STATUSFORFUTURSTATESWITCH_H

#include <QString>

enum e_StatusForFuturStateSwitch {
    e_SFFSS_acceptable_noMsgForUser,
    e_SFFSS_rejected_noMsgForUser,

    e_SFFSS_lastStatus_noMsgForUser = e_SFFSS_rejected_noMsgForUser,

    e_SFFSS_acceptable_butWarning_QuestionYesNo,
    e_SFFSS_acceptable_butWarning_QuestionOKCancel,
    e_SFFSS_acceptable_butWarning_QuestionResetKeepCancel,
    e_SFFSS_rejected_msgForUser
};

struct S_StatusForFuturStateSwitch_withMessage {
    e_StatusForFuturStateSwitch _eStatusForFuturStateSwitch;
    QString _qstrMessage;
    QString _qstrQuestion;
    S_StatusForFuturStateSwitch_withMessage();
};

#endif // STATUSFORFUTURSTATESWITCH_H
