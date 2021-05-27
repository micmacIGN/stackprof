#ifndef S_BOOLEANKEYSTATUS_HPP
#define S_BOOLEANKEYSTATUS_HPP

#include <QtGlobal>

#include <QGuiApplication>

/*
 https://doc.qt.io/qt-5/macos-issues.html
 ' Special Keys
   To provide the expected behavior for Qt applications on macOS,
   the Qt::Key_Meta, Qt::MetaModifier, and Qt::META enum values correspond to the Control keys on the standard Apple keyboard,
   and the Qt::Key_Control, Qt::ControlModifier, and Qt::CTRL enum values correspond to the Command keys. '
*/
struct S_booleanKeyStatus {
    bool _bShiftKeyAlone;
    bool _bAltKeyAlone;
    bool _bCtrlKeyAlone;
    bool _bNonePressed;
    S_booleanKeyStatus() {
        Qt::KeyboardModifiers KeyModifiers = QGuiApplication::queryKeyboardModifiers();
        _bShiftKeyAlone = KeyModifiers & Qt::ShiftModifier;
        _bAltKeyAlone  = KeyModifiers & Qt::AltModifier;

//@LP for an unknown reason, #elseif and #elif don't seems to work properly using here with Q_OS_... Hence use separated #ifdef instead

#ifdef Q_OS_LINUX
        #define DEF_FOUND_OS_S_BOOLEANKEYSTATUS_HPP
        _bCtrlKeyAlone = KeyModifiers & Qt::ControlModifier;
#endif

#ifdef Q_OS_OSX //Q_OS_DARWIN //Q_OS_MACOS
        #ifdef  DEF_FOUND_OS_S_BOOLEANKEYSTATUS_HPP
            #error OS defined macros conflict  (DEF_FOUND_OS___S_BOOLEANKEYSTATUS_HPP)
        #endif
        #define DEF_FOUND_OS_S_BOOLEANKEYSTATUS_HPP
        _bCtrlKeyAlone = KeyModifiers & Qt::MetaModifier;
#endif

#ifdef Q_OS_WIN
        #ifdef  DEF_FOUND_OS_S_BOOLEANKEYSTATUS_HPP
            #error OS defined macros conflict  (DEF_FOUND_OS___S_BOOLEANKEYSTATUS_HPP)
        #endif
        #define DEF_FOUND_OS_S_BOOLEANKEYSTATUS_HPP
        _bCtrlKeyAlone = KeyModifiers & Qt::ControlModifier;
#endif

#ifndef DEF_FOUND_OS_S_BOOLEANKEYSTATUS_HPP
        #error _bCtrlKeytAlone value not defined for this platform, please indicate
#endif

        _bNonePressed = !(_bShiftKeyAlone || _bAltKeyAlone || _bCtrlKeyAlone);

    }
};

#endif // S_BOOLEANKEYSTATUS_HPP

