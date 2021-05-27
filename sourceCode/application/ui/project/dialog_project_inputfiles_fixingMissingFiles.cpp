#include <QFileDialog>
#include <QDebug>
#include <QCheckBox>
#include <QMessageBox>

#include "../../logic/model/core/ComputationCore_structures.h" //just for e_LayersAcces //@#LP move e_LayersAcces in a dedicated header file

#include "../../logic/OsPlatform/standardPathLocation.hpp"

#include "dialog_project_inputfiles_fixingMissingFiles.h"
#include "ui_dialog_project_inputfiles_fixingMissingFiles.h"

#include "toolbox/toolbox_pathAndFile.h"

Dialog_project_inputFiles_fixingMissingFiles::Dialog_project_inputFiles_fixingMissingFiles(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Dialog_project_inputFiles_fixingMissingFiles),
    _bUiSetLogicFields(true),
    _bFixingMissingFile(false),
    _tbWarned_correlationScore_PX1PX2Together_Unused_DeltaZAlone {false, false, false} ,
    _bIsMissing_deltaZorOther_correlScoreMap(false),
    _bIsMissing_PX1PX2_correlScoreMap(false) {

    ui->setupUi(this);

    _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together] = ui->lineEdit_PX1PX2_correlScoreMap;
    _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone   ] = ui->lineEdit_deltaZorOther_correlScoreMap;
    //should never be used:
    _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX2Alone] = nullptr;

    ui->widget_PX1PX2_inputImageFiles->setSelectFilemode(e_uSFM_twoFiles);
    ui->widget_PX1PX2_inputImageFiles->setFilesTitle({"Px1", "Px2"});

    ui->widget_deltaZorOther_inputImageFiles->setSelectFilemode(e_uSFM_oneFile);

    ui->pushButton_PX1PX2_correlScoreMap->setProperty("associatedLayerSetId", eLA_CorrelScoreForPX1PX2Together);
    ui->pushButton_deltaZorOther_correlsScoreaMap_selectFile->setProperty("associatedLayerSetId", eLA_CorrelScoreForDeltaZAlone);

    ui->lineEdit_PX1PX2_correlScoreMap->setProperty("associatedLayerSetId", eLA_CorrelScoreForPX1PX2Together);
    ui->lineEdit_deltaZorOther_correlScoreMap->setProperty("associatedLayerSetId", eLA_CorrelScoreForDeltaZAlone);

    connect(ui->widget_PX1PX2_inputImageFiles, &WidgetInputFilesImages::signal_lineEdit_aboutInputFilename_emptyStateChanged,
            this, &Dialog_project_inputFiles_fixingMissingFiles::slot_lineEdit_aboutInputImageFilename_emptyStateChanged);

    connect(ui->widget_deltaZorOther_inputImageFiles, &WidgetInputFilesImages::signal_lineEdit_aboutInputFilename_emptyStateChanged,
            this, &Dialog_project_inputFiles_fixingMissingFiles::slot_lineEdit_aboutInputImageFilename_emptyStateChanged);

    connect(ui->pushButton_PX1PX2_correlScoreMap, &QPushButton::clicked, this, &Dialog_project_inputFiles_fixingMissingFiles::slot_pushButton_correlScoreMap_selectFile);
    connect(ui->lineEdit_PX1PX2_correlScoreMap,   &QLineEdit::textEdited, this, &Dialog_project_inputFiles_fixingMissingFiles::slot_lineEdit_correlScoreMap_textEdited);

    connect(ui->pushButton_deltaZorOther_correlsScoreaMap_selectFile, &QPushButton::clicked, this, &Dialog_project_inputFiles_fixingMissingFiles::slot_pushButton_correlScoreMap_selectFile);
    connect(ui->lineEdit_deltaZorOther_correlScoreMap,                &QLineEdit::textEdited, this, &Dialog_project_inputFiles_fixingMissingFiles::slot_lineEdit_correlScoreMap_textEdited);

    connect((ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)), &QPushButton::clicked,
            this, &Dialog_project_inputFiles_fixingMissingFiles::slot_pushButtonOK_clicked);

    _qvectb_layersToCompute.fill(false, 3);
    _qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ.fill(false, 3);
    _qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone.fill(false, 3);

    _qvectb_correlScoreCheckBoxStateWhenGroupEnabled_PX1PX2Together_DeltaZAlone.fill(false, 3);

    //to update Dialog_project_inputFiles starting file selection path when changed in widgets:
    connect(ui->widget_PX1PX2_inputImageFiles       , &WidgetInputFilesImages::signal_startingPathForFileSelectionChanged, this, &Dialog_project_inputFiles_fixingMissingFiles::slot_startingPathForFileSelectionChanged);
    connect(ui->widget_deltaZorOther_inputImageFiles, &WidgetInputFilesImages::signal_startingPathForFileSelectionChanged, this, &Dialog_project_inputFiles_fixingMissingFiles::slot_startingPathForFileSelectionChanged);

    //to update widgets starting file selection path when changed in Dialog_project_inputFiles:
    connect(this, &Dialog_project_inputFiles_fixingMissingFiles::signal_startingPathForFileSelectionChanged, ui->widget_PX1PX2_inputImageFiles       , &WidgetInputFilesImages::slot_startingPathForFileSelectionChanged);
    connect(this, &Dialog_project_inputFiles_fixingMissingFiles::signal_startingPathForFileSelectionChanged, ui->widget_deltaZorOther_inputImageFiles, &WidgetInputFilesImages::slot_startingPathForFileSelectionChanged);

    setStateAsEmptyProject();

}

void Dialog_project_inputFiles_fixingMissingFiles::setStateAsEmptyProject() {
    setMode_fixingMissingFile(false);

    //enable group according to layers to compute
    ui->groupBox_PX1PX2->setEnabled(true);
    ui->groupBox_DeltaZorOther->setEnabled(true);

    setToNoValues();
}

void Dialog_project_inputFiles_fixingMissingFiles::slot_startingPathForFileSelectionChanged(QString strNewStartingPathForFileSelection) {
    _strStartingPathForFileSelection = strNewStartingPathForFileSelection;
    emit signal_startingPathForFileSelectionChanged(_strStartingPathForFileSelection);//send to the widgets for update
}

void Dialog_project_inputFiles_fixingMissingFiles::setMode_fixingMissingFile(bool bFixingMissingFile) {
    _bFixingMissingFile = bFixingMissingFile;
    ui->groupBox_routeSelection->setVisible(!_bFixingMissingFile);
    ui->groupBox_routeInProject->setVisible(_bFixingMissingFile);

   //push button OK enabled in case of fixingMissingFile (files are red highlighted; evaluating condition to enabled the button is a low priority dev task)
   if (_bFixingMissingFile) {
       setEnable_windowPushButtonOK(true);
   }
}

void Dialog_project_inputFiles_fixingMissingFiles::slot_setIntStr_routesetFilename_routeNameOfselectedRouteforRouteImport(
        int routeId, QString strRoutesetFilename,
        QString strRouteName,
        bool bValid_selectedRoute) {

    _selectedRouteId = routeId;
    _strNameOfSelectedRoute = strRouteName;

    _bOneRouteSelectedFromFile = bValid_selectedRoute;

    ui->lineEdit_routesetFile->setText(strRoutesetFilename); //@LP can override the user line edited filename (it's not a problem)
    ui->lineEdit_selectedRouteName->setText(_strNameOfSelectedRoute);
    ui->lineEdit_selectedRouteName->setEnabled(_bOneRouteSelectedFromFile);
    ui->pushButton_selectRoute->setEnabled(true);

    qDebug() << __FUNCTION__ << "call now setEnable_windowPushButtonOK(evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization());";

    setEnable_windowPushButtonOK(evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization());

}


S_InputFiles Dialog_project_inputFiles_fixingMissingFiles::get_inputFiles_imageAndCorrelationScoreMap() {

    qDebug() << __FUNCTION__ << " _inputFiles:";
    _inputFiles.showContent();

    return(_inputFiles);
}


void Dialog_project_inputFiles_fixingMissingFiles::setEditedProjectState_missingFiles_lockedRoute(const S_InputFiles& inputFiles,
                                                                               const QVector<bool>& qvectb_layersToCompute,
                                                                               const QVector<bool>& qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ,
                                                                               const QVector<bool>& qvectb_missingFiles_inputFileCorrelationScore_PX1_PX2_DeltaZ,
                                                                               const QString& strRouteName) {

    setToNoValues();
    setMode_fixingMissingFile(true);

    _inputFiles = inputFiles;

    qDebug() << __FUNCTION__ << "_inputFiles = ";
    _inputFiles.showContent();

    _qvectb_layersToCompute = qvectb_layersToCompute;
    _qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ = qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ;
    _qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone = qvectb_missingFiles_inputFileCorrelationScore_PX1_PX2_DeltaZ;


    //adjusting any other project parameters than fixing missing files is forbidden
    //
    //can not switch layers to compute:
    ui->groupBox_PX1PX2->setCheckable(false);
    ui->groupBox_DeltaZorOther->setCheckable(false);

    //enable group according to layers to compute
    ui->groupBox_PX1PX2->setEnabled(_qvectb_layersToCompute[eLA_PX1]);
    ui->groupBox_DeltaZorOther->setEnabled(_qvectb_layersToCompute[eLA_deltaZ]);

    //set input files:
    if (_qvectb_layersToCompute[eLA_PX1]) {
        ui->widget_PX1PX2_inputImageFiles->set_file1_file2(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX1],
                                                           _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX2]);
    }
    if (_qvectb_layersToCompute[eLA_deltaZ]) {
        ui->widget_deltaZorOther_inputImageFiles->set_fileAlone(_inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_deltaZ]);
    }

    //set text correlation score maps and enable according widgets if used:
    bool bPX1PX2_correlScoreMapUsed = !_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].isEmpty();
    _bIsMissing_PX1PX2_correlScoreMap = _qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[eLA_CorrelScoreForPX1PX2Together];
    ui->lineEdit_PX1PX2_correlScoreMap->setText(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together]);
    ui->lineEdit_PX1PX2_correlScoreMap->setEnabled(bPX1PX2_correlScoreMapUsed);


    ui->label_PX1PX2_correlScoreMap->setEnabled(bPX1PX2_correlScoreMapUsed);
    ui->pushButton_PX1PX2_correlScoreMap->setEnabled(bPX1PX2_correlScoreMapUsed);


    bool bDeltaZ_correlScoreMapUsed = !_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].isEmpty();
    _bIsMissing_deltaZorOther_correlScoreMap = _qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[eLA_CorrelScoreForDeltaZAlone];
    ui->lineEdit_deltaZorOther_correlScoreMap->setText(_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone]);
    ui->lineEdit_deltaZorOther_correlScoreMap->setEnabled(bDeltaZ_correlScoreMapUsed);


    ui->label_deltaZorOther_correlScoreMap->setEnabled(bDeltaZ_correlScoreMapUsed);
    ui->pushButton_deltaZorOther_correlsScoreaMap_selectFile->setEnabled(bDeltaZ_correlScoreMapUsed);


    //disable any route changes
    ui->pushButton_routesetFile->setEnabled(false);
    ui->pushButton_selectRoute->setEnabled(false);

    //feed route infos
    ui->lineEdit_routeName->setText(strRouteName);


    //now warn and permit alteration for missing files:
    //
    //PX1, PX2
    if (_qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ[eLA_PX1]) {
        ui->widget_PX1PX2_inputImageFiles->set_editableState_warnState(e_idxFS_File1, true, true);
    }

    if (_qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ[eLA_PX2]) {
          ui->widget_PX1PX2_inputImageFiles->set_editableState_warnState(e_idxFS_File2, true, true);
    }


    if (_bIsMissing_PX1PX2_correlScoreMap) {


         ui->lineEdit_PX1PX2_correlScoreMap->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
        _tbWarned_correlationScore_PX1PX2Together_Unused_DeltaZAlone[eLA_CorrelScoreForPX1PX2Together] = true;        
    }

    //deltaZ or other
    if (_qvectb_missingFiles_inputFileDisplacementmap_PX1_PX2_DeltaZ[eLA_deltaZ]) {
        ui->widget_deltaZorOther_inputImageFiles->set_editableState_warnState(e_idxFS_Alone, true, true);
    }

    if (_bIsMissing_deltaZorOther_correlScoreMap) {
        ui->lineEdit_deltaZorOther_correlScoreMap->setStyleSheet("QLineEdit { color: rgb(255, 0, 0); }");
        _tbWarned_correlationScore_PX1PX2Together_Unused_DeltaZAlone[eLA_CorrelScoreForDeltaZAlone] = true;
    }
}

void Dialog_project_inputFiles_fixingMissingFiles::slot_lineEdit_aboutInputImageFilename_emptyStateChanged(/*bool bLineEditEmpty*/) {
    qDebug() << __FUNCTION__ << "call now setEnable_windowPushButtonOK(evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization());";
    if (!_bFixingMissingFile) {
        setEnable_windowPushButtonOK(evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization());
    }
}


void Dialog_project_inputFiles_fixingMissingFiles::setEnable_windowPushButtonOK(bool bNewState) {
    (ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok))->setEnabled(bNewState);
}


void Dialog_project_inputFiles_fixingMissingFiles::setStrStartingPathForFileSelection(const QString& strStartingPathForFileSelection) {
    _strStartingPathForFileSelection = strStartingPathForFileSelection;
    emit signal_startingPathForFileSelectionChanged(_strStartingPathForFileSelection);
}

QString Dialog_project_inputFiles_fixingMissingFiles::getStrStartingPathForFileSelection() {
    return(_strStartingPathForFileSelection);
}

void Dialog_project_inputFiles_fixingMissingFiles::setToNoValues() {

    //widgets part:
    ui->widget_PX1PX2_inputImageFiles->setToNoValues();
    ui->widget_deltaZorOther_inputImageFiles->setToNoValues();

    ui->lineEdit_PX1PX2_correlScoreMap->clear();
    ui->lineEdit_deltaZorOther_correlScoreMap->clear();

    setEnable_windowPushButtonOK(false);

    _inputFiles.clear();

   _bAddADefaultTrace = false;
   ui->checkBox_createTraceLater->setCheckState(Qt::CheckState::Unchecked);

   ui->lineEdit_routesetFile->clear();

   _bOneRouteSelectedFromFile = false;
   _strNameOfSelectedRoute.clear();

   _strFilename_routeset_forRouteImport.clear();

   _selectedRouteId = -1;

   updateWidgetsAndFieldsAboutSelectedRoute();

   ui->groupBox_PX1PX2->setCheckable(true);
   ui->groupBox_DeltaZorOther->setCheckable(true);

}

void Dialog_project_inputFiles_fixingMissingFiles::updateWidgetsAndFieldsAboutSelectedRoute() {
    if (_strFilename_routeset_forRouteImport.isEmpty()) {
        _bOneRouteSelectedFromFile = false;
        ui->lineEdit_selectedRouteName->clear();
        ui->lineEdit_selectedRouteName->setEnabled(false);
        ui->pushButton_selectRoute->setEnabled(false);        
    }
}

void Dialog_project_inputFiles_fixingMissingFiles::slot_pushButton_correlScoreMap_selectFile() {

    qDebug() << __FUNCTION__;

    QObject *ptrSender = sender();
    QVariant qvariant_associatedFile = ptrSender->property("associatedLayerSetId");
    bool bOk = false;
    int intAssociatedLayerSetId  = qvariant_associatedFile.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associatedFile.toInt()";
        return;
    }

    if (  (intAssociatedLayerSetId != eLA_CorrelScoreForPX1PX2Together)
        &&(intAssociatedLayerSetId != eLA_CorrelScoreForDeltaZAlone)) {
        qDebug() << __FUNCTION__ << " error: invalid intAssociatedLayerSetId ( " << intAssociatedLayerSetId << " )";
        return;
    }

    qDebug() << __FUNCTION__ << "intAssociatedLayerSetId:" << intAssociatedLayerSetId;

    QString strStartingDir = _strStartingPathForFileSelection;
    qDebug() << __FUNCTION__ << "strStartingDir: " << strStartingDir;

    QString strfileName = QFileDialog::getOpenFileName(this, tr("Open image"),
                                                    strStartingDir,
                                                    tr("*.tif *.tiff *.TIF *.TIFF"));
    if (strfileName.isEmpty()) {
        return;
    }

    _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[intAssociatedLayerSetId] = strfileName;

    if (_tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[intAssociatedLayerSetId]) {
        _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[intAssociatedLayerSetId]->setText(strfileName);
    }

    set_correlationScoreWarned_off(intAssociatedLayerSetId);

    _strStartingPathForFileSelection = getStrPathFromPathFile(strfileName);
    emit signal_startingPathForFileSelectionChanged(_strStartingPathForFileSelection);


    qDebug() << __FUNCTION__ << "intAssociatedLayerSetId:" << intAssociatedLayerSetId;
    qDebug() << __FUNCTION__ << "_inputFiles._qvectStr_CorrelationScore_PX1_PX2_DeltaZ[intAssociatedLayerSetId] is now:"
              << _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[intAssociatedLayerSetId];

    setEnable_windowPushButtonOK(evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization());

}

void Dialog_project_inputFiles_fixingMissingFiles::set_correlationScoreWarned_off(int intAssociatedLayerSetId) {
    if (_tbWarned_correlationScore_PX1PX2Together_Unused_DeltaZAlone[intAssociatedLayerSetId]) {
        _tbWarned_correlationScore_PX1PX2Together_Unused_DeltaZAlone[intAssociatedLayerSetId] = false;

        _tqlineEditPtr_correlationScoreMap_PX1_PX2_DeltaZ[intAssociatedLayerSetId]->setStyleSheet("");
    }
}


void Dialog_project_inputFiles_fixingMissingFiles::slot_lineEdit_correlScoreMap_textEdited(const QString& text) {

    qDebug() << __FUNCTION__ << " entering with text =" << text;

    QObject *ptrSender = sender();

    QVariant qvariant_associatedFile = ptrSender->property("associatedLayerSetId");
    bool bOk = false;
    int intAssociatedLayerSetId  = qvariant_associatedFile.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associatedFile.toInt()";
        return;
    }

    if (  (intAssociatedLayerSetId != eLA_CorrelScoreForPX1PX2Together)
        &&(intAssociatedLayerSetId != eLA_CorrelScoreForDeltaZAlone)) {
        qDebug() << __FUNCTION__ << " error: invalid intAssociatedLayerSetId ( " << intAssociatedLayerSetId << " )";
        return;
    }

    _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[intAssociatedLayerSetId] = text;

    set_correlationScoreWarned_off(intAssociatedLayerSetId);

    qDebug() << __FUNCTION__ << "intAssociatedLayerSetId:" << intAssociatedLayerSetId;
    qDebug() << __FUNCTION__ << "_inputFiles._qvectStr_CorrelationScore_PX1_PX2_DeltaZ[intAssociatedLayerSetId] is now:"
              << _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[intAssociatedLayerSetId];

    setEnable_windowPushButtonOK(evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization());
}

Dialog_project_inputFiles_fixingMissingFiles::~Dialog_project_inputFiles_fixingMissingFiles() {
    delete ui;
}

bool Dialog_project_inputFiles_fixingMissingFiles::get_checkedState_GroupBox_PX1PX2() {

    bool bCheckable = ui->groupBox_PX1PX2->isCheckable();

    bool bChecked  = false;
    if (!bCheckable) {
        bChecked = true;
    } else {
        bChecked  = ui->groupBox_PX1PX2->isChecked();
    }
    return(bChecked);
}

bool Dialog_project_inputFiles_fixingMissingFiles::get_checkedState_GroupBox_DeltaZorOther() {

    bool bCheckable = ui->groupBox_DeltaZorOther->isCheckable();

    bool bChecked  = false;
    if (!bCheckable) {
        bChecked = true;
    } else {
        bChecked  = ui->groupBox_DeltaZorOther->isChecked();
    }
    return(bChecked);
}


bool Dialog_project_inputFiles_fixingMissingFiles::evaluateIfInputFileAndParametersAreValidWithANewProjectInitialization() {

    bool bGroupBoxPX1PX2_checkableAndChecked = get_checkedState_GroupBox_PX1PX2();

    bool bGroupBoxDeltaZorOther_checkableAndChecked = get_checkedState_GroupBox_DeltaZorOther();

    bool b_PX1PX2_inputFeed = false;
    if (bGroupBoxPX1PX2_checkableAndChecked) {
        b_PX1PX2_inputFeed = ui->widget_PX1PX2_inputImageFiles->inputFeed();
    }

    bool b_deltaZOrOther_inputFeed = false;
    if (bGroupBoxDeltaZorOther_checkableAndChecked) {
        b_deltaZOrOther_inputFeed = ui->widget_deltaZorOther_inputImageFiles->inputFeed();
    }

    if (!_bFixingMissingFile) {

        //evaluate:
        if (bGroupBoxPX1PX2_checkableAndChecked) {
            if (!b_PX1PX2_inputFeed) {
                qDebug() <<__FUNCTION__ << "if (!b_PX1PX2_inputFeed) {";
                return(false);
            }
        }

        if (bGroupBoxDeltaZorOther_checkableAndChecked) {
            if (!b_deltaZOrOther_inputFeed) {
                qDebug() <<__FUNCTION__ << "if (!b_deltaZOrOther_inputFeed) {";
                return(false);
            }
        }

        qDebug() <<__FUNCTION__ << "b_PX1PX2_inputFeed = " << b_PX1PX2_inputFeed;
        qDebug() <<__FUNCTION__ << "b_deltaZOrOther_inputFeed = " << b_deltaZOrOther_inputFeed;


        if (bGroupBoxPX1PX2_checkableAndChecked) {
            if (_bIsMissing_PX1PX2_correlScoreMap/*_qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[eLA_CorrelScoreForPX1PX2Together]*/) { //if (ui->checkBox_PX1PX2_correlScoreMap->checkState() == Qt::Checked) {
                if (_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together].isEmpty()) {
                    qDebug() <<__FUNCTION__ << "if case #3";
                    return(false);
                }
            }
        }

        if (bGroupBoxDeltaZorOther_checkableAndChecked) {
            if (_bIsMissing_deltaZorOther_correlScoreMap/*_qvectb_missingFiles_correlationScore_PX1PX2Together_DeltaZAlone[eLA_CorrelScoreForDeltaZAlone]*/) { //if (ui->checkBox_deltaZorOther_correlScoreMap->checkState() == Qt::Checked) {
                if (_inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone].isEmpty()) {
                    qDebug() <<__FUNCTION__ << "if case #4";
                    return(false);
                }
            }
        }
    }

    if (_bAddADefaultTrace) {
        qDebug() <<__FUNCTION__ << "if (_bCreateRouteLater) {";
        return(true);
    }

    //here if _bCreateRouteLater_unChecked

    qDebug() <<__FUNCTION__ << "_bOneRouteSelectedFromFile = " << _bOneRouteSelectedFromFile;

    if (_bFixingMissingFile) {        
        qDebug() <<__FUNCTION__ << "if (_bFixingMissingFile) {";
        return(true);
    }

    qDebug() <<__FUNCTION__ << "return at end of method: _bOneRouteSelectedFromFile = " << _bOneRouteSelectedFromFile;
    return(_bOneRouteSelectedFromFile);
}

void Dialog_project_inputFiles_fixingMissingFiles::slot_pushButtonOK_clicked() {

    //file with only space inside will be seens as not empty; but the check about file existence will see it.
    ui->widget_PX1PX2_inputImageFiles->get_file1_file2(
        _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX1],
        _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX2]);

    _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_deltaZ] = ui->widget_deltaZorOther_inputImageFiles->get_fileAlone();

    qDebug() << __FUNCTION__ << "inputImageFiles_PX1_PX2_DeltaZ :";
    qDebug() << __FUNCTION__ << _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX1];
    qDebug() << __FUNCTION__ << _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_PX2];
    qDebug() << __FUNCTION__ << _inputFiles._qvectStr_inputFile_PX1_PX2_DeltaZ[eLA_deltaZ];

    qDebug() << __FUNCTION__ << "correlationScoreMap_PX1PX2Together_DeltaZ :";
    qDebug() << __FUNCTION__ << _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForPX1PX2Together];
    qDebug() << __FUNCTION__ << _inputFiles._qvectStr_correlationScore_PX1_PX2_DeltaZ[eLA_CorrelScoreForDeltaZAlone];

    emit signal_pushButtonOK_clicked();
}

void Dialog_project_inputFiles_fixingMissingFiles::reject() {
    emit signal_canceled();
}


//dialog modal but we avoid to close the window with ok and the check is handled by the controller side
void Dialog_project_inputFiles_fixingMissingFiles::accept() {
    //model will permits to the window to close only if all check are ok
    //there is many check involved. Prefer to handle all the checks in the controller side

    //do nothing
}

void Dialog_project_inputFiles_fixingMissingFiles::setToNoneChangePossible_viewProjectInputOnly() {
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(true);

}

void Dialog_project_inputFiles_fixingMissingFiles::setToChangePossible() {
    ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setEnabled(true);

}
