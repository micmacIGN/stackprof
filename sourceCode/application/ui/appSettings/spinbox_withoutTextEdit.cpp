#include <QSpinBox>

#include <QDebug>

#include "spinbox_withoutTextEdit.h"

SpinBox_withoutTextEdit::SpinBox_withoutTextEdit(QWidget *parent): QSpinBox(parent) {

    setRange(64,16384); //64MByte to 16GigaByte
    _currentValue = 512; //defaultValue
}

void SpinBox_withoutTextEdit::stepBy(int steps) {

    switch(steps) {
        case 1 :    setValue(value()*2);
                    break;
        case -1 :   setValue(value()/2);
                    break;
        default:    QSpinBox::stepBy(steps);
                    break;
    }
    _currentValue = value();
}
