#ifndef DIALOG_GEOCRSSELECT_TABLEVIEW_H
#define DIALOG_GEOCRSSELECT_TABLEVIEW_H

#include <QDialog>

#include <QItemSelection>

#include "../../logic/georef/geo_EPSGCodeData_struct.h"

namespace Ui {
class Dialog_geoCRSSelect_tableView;
}

class geoCRSSelect_inheritQATableModel;

class Dialog_geoCRSSelect_tableView : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_geoCRSSelect_tableView(QWidget *parent = nullptr);
    ~Dialog_geoCRSSelect_tableView();

    void setModelsPtr_geoCRSSelect_tableView(geoCRSSelect_inheritQATableModel *geoCRSSelect_inheritQATableModel_ptr);
    void initConnectionModelViewSignalsSlots();

    virtual void closeEvent(QCloseEvent *event); //@LP about close window button typically
    virtual void reject(); //@LP about escape key typically

    virtual void accept();

    void showEvent(QShowEvent *event);

    //void clear_and_send_signal_keywordListCleared_toModel();
    void resetToDefault();

public slots:
    void slot_lineEdit_keywordsToSearch_textEdited(const QString &text);
    void slot_pushButton_search_clicked();
    void slot_pushButton_clear_clicked();

    void slot_searchFromKeywords_resultCount(int resultCount/*, bool bMaxToSearchReached*/);
    void slot_pushButtonKeyword_clicked();
    void slot_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void slot_EPSGDataAboutSelection(const QString& strEPSGCode, const QString& strName);

signals:
    void signal_searchFromKeywords(QStringList strListKeywords);
    void signal_keywordListCleared();
    void signal_selectionChangedInTableView(int index);

    //for outside:
    void signal_EPSGCodeSelectedFromList(QString strSelectedEPSGCode);

private:
    void setEnableStateOfSearchAndClearPushButton();
    void setEnableState_pushButtonOK(bool bState);
    void setEnableState_widgetAboutOneCodeSelected(bool bState);
    void clearAboutOneCodeSelected();
    void clearTableViewRowSelection();

    //void tableview_unselectAll();

private:
    enum e_UIGeoCRSSelectKeywordID {
        e_UIGCRSSKID_WGS84_UTM,
        e_UIGCRSSKID_UTM,
        e_UIGCRSSKID_WGS84,
        e_UIGCRSSKID_NAD27,
        e_UIGCRSSKID_NAD83,
        e_UIGCRSSKID_ETRS89,
        e_UIGCRSSKID_zone,
    };

private:
    Ui::Dialog_geoCRSSelect_tableView *ui;

    QStringList _strList_keywordsToSearch;

    geoCRSSelect_inheritQATableModel *_geoCRSSelect_inheritQATableModel_ptr;

    QHash<e_UIGeoCRSSelectKeywordID, QString> _qhash_UIGCRSSKID_string;

    QVector<QPushButton*> _qvect_QPushButtonPtr_keywordPreset;

    bool _bEnableState_pushButtonOK;

    int _selectedCodeTableViewRow;

    bool _bOneCodeSelected;
    struct S_SelectedEPSGCode_data {
        QString strEPSGCode;
        S_dataForAKnownEPSGCode_fromProjDB _sDataForAKnownEPSGCode_fromProjDB;
    } _sSelectedEPSGCode_data;

};

#endif // DIALOG_GEOCRSSELECT_TABLEVIEW_H
