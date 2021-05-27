#include <QDebug>

#include <QString>

#include "dialog_geoCRSSelect_tableview.h"
#include "ui_dialog_geoCRSSelect_tableview.h"

#include "../logic/model/geoCRSSelect/geoCRSSelect_inheritQATableModel.h"

Dialog_geoCRSSelect_tableView::Dialog_geoCRSSelect_tableView(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_geoCRSSelect_tableView),
    _geoCRSSelect_inheritQATableModel_ptr(nullptr),
    _bEnableState_pushButtonOK(false),
    _selectedCodeTableViewRow(-1),
    _bOneCodeSelected(false)
{

    ui->setupUi(this);

    resetToDefault();

    //hardcoded keywords list
    _qhash_UIGCRSSKID_string = {
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_WGS84_UTM, "WGS 84 UTM"},
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_UTM,"UTM"},
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_WGS84,"WGS 84"},
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_NAD27,"NAD27"},
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_NAD83,"NAD83"},
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_ETRS89,"ETRS89"},
        {e_UIGeoCRSSelectKeywordID::e_UIGCRSSKID_zone,"zone"}
    };

    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw0);
    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw1);
    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw2);
    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw3);
    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw4);
    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw5);
    _qvect_QPushButtonPtr_keywordPreset.push_back(ui->pushButton_addKeywordAnSearch_keyw6);

    for (int ieUIGeoCRSSelectKeywordID = e_UIGCRSSKID_WGS84_UTM;
         ieUIGeoCRSSelectKeywordID <= e_UIGCRSSKID_zone;
         ieUIGeoCRSSelectKeywordID++) {

        QString strKeyword = _qhash_UIGCRSSKID_string.value(static_cast<e_UIGeoCRSSelectKeywordID>(ieUIGeoCRSSelectKeywordID));
        _qvect_QPushButtonPtr_keywordPreset[ieUIGeoCRSSelectKeywordID]->setText(strKeyword);
        _qvect_QPushButtonPtr_keywordPreset[ieUIGeoCRSSelectKeywordID]->setProperty("associatedKeyword", strKeyword);

        connect(_qvect_QPushButtonPtr_keywordPreset[ieUIGeoCRSSelectKeywordID], &QPushButton::clicked,
                this, &Dialog_geoCRSSelect_tableView::slot_pushButtonKeyword_clicked);
    }

    connect(ui->lineEdit_keywordsToSearch, &QLineEdit::textEdited,
            this, &Dialog_geoCRSSelect_tableView::slot_lineEdit_keywordsToSearch_textEdited);

    connect(ui->pushButton_search, &QPushButton::clicked,
            this, &Dialog_geoCRSSelect_tableView::slot_pushButton_search_clicked);

    connect(ui->pushButton_clear, &QPushButton::clicked,
            this, &Dialog_geoCRSSelect_tableView::slot_pushButton_clear_clicked);

    slot_searchFromKeywords_resultCount(0/*, false*/);

    /*connect(ui->pushButton_test, &QPushButton::clicked,
            this, &Dialog_geoCRSSelect_tableView::slot_pushButton_clicked);*/

}

void Dialog_geoCRSSelect_tableView::slot_pushButtonKeyword_clicked() {
    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView) _strList_keywordsToSearch:" << _strList_keywordsToSearch;

    QObject *ptrSender = sender();
    QVariant qvariant_associatedFile = ptrSender->property("associatedKeyword");
    QString qstrAssociatedKeyword = qvariant_associatedFile.toString();

    QString strCurrent_lineEdit_keywordsToSearch = ui->lineEdit_keywordsToSearch->text();

    if (!strCurrent_lineEdit_keywordsToSearch.contains(qstrAssociatedKeyword)) {
        strCurrent_lineEdit_keywordsToSearch += ' ' + qstrAssociatedKeyword;
        ui->lineEdit_keywordsToSearch->setText(strCurrent_lineEdit_keywordsToSearch);
        slot_lineEdit_keywordsToSearch_textEdited(strCurrent_lineEdit_keywordsToSearch);
        slot_pushButton_search_clicked();
    }
}

void Dialog_geoCRSSelect_tableView::resetToDefault() {
    ui->lineEdit_keywordsToSearch->clear();
    _strList_keywordsToSearch.clear();
    setEnableStateOfSearchAndClearPushButton();

    clearAboutOneCodeSelected();
    setEnableState_widgetAboutOneCodeSelected(false);
}

/*void Dialog_geoCRSSelect_tableView::clear_and_send_signal_keywordListCleared_toModel() {
    resetToDefault();
    emit signal_keywordListCleared();
}*/

void Dialog_geoCRSSelect_tableView::setEnableState_widgetAboutOneCodeSelected(bool bState) {
    _bOneCodeSelected = bState;
    ui->lineEdit_selected_EPSGCode->setEnabled(_bOneCodeSelected);
    ui->lineEdit_selected_name->setEnabled(_bOneCodeSelected);
}

void Dialog_geoCRSSelect_tableView::slot_lineEdit_keywordsToSearch_textEdited(const QString &text) {

    _strList_keywordsToSearch.clear();

    if (text.size()) {
        _strList_keywordsToSearch = text.split(' ', QString::SplitBehavior::SkipEmptyParts);
    }

    setEnableStateOfSearchAndClearPushButton();
}

void Dialog_geoCRSSelect_tableView::setEnableStateOfSearchAndClearPushButton() {

    _strList_keywordsToSearch.removeDuplicates();

    bool bEnableButton = false;

    bool bSomeKeyword = _strList_keywordsToSearch.count();
    if (bSomeKeyword) {
        int countChar = 0;
        /*int count_length_min = 100;
        int count_length_max = 0;
        for (const auto& iter: _strList_keywordsToSearch) {
            if (iter.size() > count_length_max) { count_length_max = iter.size(); }
            if (iter.size() < count_length_min) { count_length_min = iter.size(); }
        }*/
        for (const auto& iter: _strList_keywordsToSearch) {
            countChar += iter.count();
        }
        if (countChar >= 2) {
            bEnableButton = true;
        }
    }
    ui->pushButton_search->setEnabled(bEnableButton);

    ui->pushButton_clear->setEnabled(bSomeKeyword);
}

void Dialog_geoCRSSelect_tableView::slot_pushButton_search_clicked() {

    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView) _strList_keywordsToSearch:" << _strList_keywordsToSearch;

    //should neve occurs
    if (!_strList_keywordsToSearch.count()) {
        return;
    }

    emit signal_searchFromKeywords(_strList_keywordsToSearch);
}

void Dialog_geoCRSSelect_tableView::slot_pushButton_clear_clicked() {

    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView)";

    ui->lineEdit_keywordsToSearch->clear();
    ui->lineEdit_keywordsToSearch->setFocus();
    setEnableState_pushButtonOK(false);

    slot_lineEdit_keywordsToSearch_textEdited("");
    emit signal_keywordListCleared();
}

void Dialog_geoCRSSelect_tableView::slot_searchFromKeywords_resultCount(int resultCount/*, bool bMaxToSearchReached*/) {

    if (!resultCount) {
        ui->tableView->setEnabled(false);
        _selectedCodeTableViewRow = -1;
    } else {

        ui->tableView->clearSelection(); //tableview_unselectAll();

        ui->tableView->setEnabled(true);
        _selectedCodeTableViewRow = -1;
    }

    clearAboutOneCodeSelected();
    setEnableState_pushButtonOK(false);
}

void Dialog_geoCRSSelect_tableView::setModelsPtr_geoCRSSelect_tableView(geoCRSSelect_inheritQATableModel *geoCRSSelect_inheritQATableModel_ptr) {
    _geoCRSSelect_inheritQATableModel_ptr = geoCRSSelect_inheritQATableModel_ptr;
    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView) @_geoCRSSelect_inheritQATableModel_ptr = "
             << (void*)_geoCRSSelect_inheritQATableModel_ptr;
}

void Dialog_geoCRSSelect_tableView::initConnectionModelViewSignalsSlots() {
    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView)";

    if (!ui) {
        qDebug() << __FUNCTION__ << " error: ui is nullptr";
        return;
    }

    qDebug() << __FUNCTION__ << "(Dialog_stackedProfilBox_tableView) @_geoCRSSelect_inheritQATableModel_ptr = "
             << (void*)_geoCRSSelect_inheritQATableModel_ptr;

    ui->tableView->setModel(_geoCRSSelect_inheritQATableModel_ptr);

    int widthFirstColumn  = 43;//32;
    int widthSecondColumn = 86;
    ui->tableView->setColumnWidth(0,widthFirstColumn);
    ui->tableView->setColumnWidth(1,widthSecondColumn);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);

    connect(this, &Dialog_geoCRSSelect_tableView::signal_searchFromKeywords,
            _geoCRSSelect_inheritQATableModel_ptr, &geoCRSSelect_inheritQATableModel::slot_searchFromKeywords);

    connect(this, &Dialog_geoCRSSelect_tableView::signal_keywordListCleared,
            _geoCRSSelect_inheritQATableModel_ptr, &geoCRSSelect_inheritQATableModel::slot_keywordListCleared);

    connect(_geoCRSSelect_inheritQATableModel_ptr, &geoCRSSelect_inheritQATableModel::signal_searchFromKeywords_resultCount,
            this, &Dialog_geoCRSSelect_tableView::slot_searchFromKeywords_resultCount);


    connect( ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
             this, &Dialog_geoCRSSelect_tableView::slot_selectionChanged);

    connect(this, &Dialog_geoCRSSelect_tableView::signal_selectionChangedInTableView,
            _geoCRSSelect_inheritQATableModel_ptr, &geoCRSSelect_inheritQATableModel::slot_selectionChangedInTableView);

    connect(_geoCRSSelect_inheritQATableModel_ptr, &geoCRSSelect_inheritQATableModel::signal_EPSGDataAboutSelection,
            this, &Dialog_geoCRSSelect_tableView::slot_EPSGDataAboutSelection);

}

Dialog_geoCRSSelect_tableView::~Dialog_geoCRSSelect_tableView() {
    delete ui;
}


void Dialog_geoCRSSelect_tableView::slot_selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

    qDebug() << __FUNCTION__ << "(Dialog_geoCRSSelect_tableView)" << "selected.indexes() = " << selected.indexes();

    if (!selected.indexes().count()) {
        _selectedCodeTableViewRow = -1;
        setEnableState_widgetAboutOneCodeSelected(false);
    } else {
        _selectedCodeTableViewRow = selected.indexes().first().row();
        emit signal_selectionChangedInTableView(_selectedCodeTableViewRow);
    }

}

void Dialog_geoCRSSelect_tableView::slot_EPSGDataAboutSelection(const QString& strEPSGCode, const QString& strName) {

    _sSelectedEPSGCode_data.strEPSGCode = strEPSGCode;
    _sSelectedEPSGCode_data._sDataForAKnownEPSGCode_fromProjDB.strName = strName;

    ui->lineEdit_selected_EPSGCode->setText(_sSelectedEPSGCode_data.strEPSGCode);
    ui->lineEdit_selected_name->setText(_sSelectedEPSGCode_data._sDataForAKnownEPSGCode_fromProjDB.strName);

    setEnableState_widgetAboutOneCodeSelected(true);

    setEnableState_pushButtonOK(true);

}


void Dialog_geoCRSSelect_tableView::setEnableState_pushButtonOK(bool bState) {
    _bEnableState_pushButtonOK = bState;
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(_bEnableState_pushButtonOK);

}

void Dialog_geoCRSSelect_tableView::clearAboutOneCodeSelected() {

    _sSelectedEPSGCode_data.strEPSGCode.clear();
    _sSelectedEPSGCode_data._sDataForAKnownEPSGCode_fromProjDB = {"", ""};
    _bOneCodeSelected = false;
    ui->lineEdit_selected_EPSGCode->clear();
    ui->lineEdit_selected_name->clear();

}

#include <QCloseEvent>

void Dialog_geoCRSSelect_tableView::closeEvent(QCloseEvent *event) {
    clearAboutOneCodeSelected();
    setEnableState_widgetAboutOneCodeSelected(false);
    clearTableViewRowSelection();
    QDialog::reject();
    event->accept();
}

void Dialog_geoCRSSelect_tableView::reject() {
    clearAboutOneCodeSelected();
    setEnableState_widgetAboutOneCodeSelected(false);
    clearTableViewRowSelection();
    QDialog::reject();
}

void Dialog_geoCRSSelect_tableView::clearTableViewRowSelection() {
    _selectedCodeTableViewRow = -1;
    setEnableState_pushButtonOK(false);

    ui->tableView->clearSelection(); //tableview_unselectAll();

    ui->tableView->setEnabled(false);

    emit signal_keywordListCleared();

}

/*
void Dialog_geoCRSSelect_tableView::tableview_unselectAll() {

    if (!_geoCRSSelect_inheritQATableModel_ptr) {
        return;
    }

    QModelIndex topLeft;
    QModelIndex bottomRight;

    _geoCRSSelect_inheritQATableModel_ptr->getModelIndex_TL_BR(topLeft, bottomRight);

    QItemSelection qiSelection;
    qiSelection.select(topLeft, bottomRight);

    ui->tableView->selectionModel()->select(qiSelection, QItemSelectionModel::Clear);
}
*/

void Dialog_geoCRSSelect_tableView::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);

    if (ui->lineEdit_keywordsToSearch->text().size()) {
        ui->pushButton_search->setFocus();
    } else {
        ui->lineEdit_keywordsToSearch->setFocus();
    }
    /*
    QString strCurrentKeywordsInLineEdit = ui->lineEdit_keywordsToSearch->text();
    ui->lineEdit_keywordsToSearch->setFocus();
    ui->lineEdit_keywordsToSearch->clear();
    ui->lineEdit_keywordsToSearch->setText(strCurrentKeywordsInLineEdit);
    //ui->lineEdit_keywordsToSearch->deselect();
    */

}

void Dialog_geoCRSSelect_tableView::accept() {
    if (!_bOneCodeSelected) {
        return;
    }
    emit signal_EPSGCodeSelectedFromList(_sSelectedEPSGCode_data.strEPSGCode);
    QDialog::accept();
}

