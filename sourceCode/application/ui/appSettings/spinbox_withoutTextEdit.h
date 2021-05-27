#ifndef SPINBOX_WITHOUTTEXTEDIT_H
#define SPINBOX_WITHOUTTEXTEDIT_H

#include <QSpinBox>

//this spinbox is used typically for RAM amount selection app settings; which have a value step greater than 1.T
//to simplify the way to handle entered value not sync to the step value, we avoid text edition of the spinbox value.
//
class SpinBox_withoutTextEdit : public QSpinBox {

public:
    SpinBox_withoutTextEdit(QWidget *parent = nullptr);
    void set_min_max_default(int min, int max, int defaultValue);

public slots:
    void stepBy(int steps);

protected:

private:
    mutable int _currentValue;

};

#endif // SPINBOX_WITHOUTTEXTEDIT_H
