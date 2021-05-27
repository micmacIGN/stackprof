#ifndef DIALOG_STACKEDPROFILBOX_TABLEVIEW_H
#define DIALOG_STACKEDPROFILBOX_TABLEVIEW_H

#include <QDialog>

class ComputationCore_inheritQATableModel;

namespace Ui {
class Dialog_stackedProfilBox_tableView;
}

class Dialog_stackedProfilBox_tableView : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_stackedProfilBox_tableView(QWidget *parent = nullptr);
    ~Dialog_stackedProfilBox_tableView();

    void setModelsPtr_stackedProfilBox_tableView(ComputationCore_inheritQATableModel *computationCore_iqaTableModel_ptr);
    void initConnectionModelViewSignalsSlots();

private:
    Ui::Dialog_stackedProfilBox_tableView *ui;

    ComputationCore_inheritQATableModel *_computationCore_iqaTableModel_ptr;
};

#endif // DIALOG_STACKEDPROFILBOX_TABLEVIEW_H
