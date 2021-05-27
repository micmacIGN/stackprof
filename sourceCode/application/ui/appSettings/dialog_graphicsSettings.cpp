#include "dialog_graphicsSettings.h"
#include "ui_dialog_graphicsSettings.h"

#include <QColorDialog>

#include <QDebug>

#include <QCloseEvent>

Dialog_graphicsSettings::Dialog_graphicsSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_graphicsSettings),
    _bFeed_sGPProfil_reference(false),
    //_bFeed_sGPitemOnImgView_reference(false),
    _eIDpBSC_inColorSelection(e_id_pBSC_invalid) {

    ui->setupUi(this);

    setUi_allEmptyAndDisabled();

    initializePart_profil();
    //initializePart_ItemOnImageView();
}

void Dialog_graphicsSettings::initializePart_profil() {

    _qDialColor.setOption(QColorDialog::DontUseNativeDialog);
    connect(&_qDialColor, &QColorDialog::currentColorChanged, this, &Dialog_graphicsSettings::slot_qColorDialog_currentColorChanged);

    connect(ui->checkBox_profil_curve_antialiasing,           &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_profil_curve_antialiasing_use);
    connect(ui->checkBox_profil_envelop_area_antialising,     &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_envelop_area_antialising_use);
    connect(ui->checkBox_linearReg_computedLines_antialising, &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_linearReg_computedLines_antialising_use);

    ui->spinBox_profil_curve_thickness           ->setProperty("sBId", e_id_sBSC_profil_curve);
    ui->spinBox_linearReg_xAdjusters_thickness   ->setProperty("sBId", e_id_sBSC_linearReg_xAdjusters);
    ui->spinBox_linearReg_xCentralAxis_thickness ->setProperty("sBId", e_id_sBSC_linearReg_xCentralAxis);
    ui->spinBox_linearReg_computedLines_thickness->setProperty("sBId", e_id_sBSC_linearReg_computedLines);

    _qhash_int_dThicknessPtr.insert(e_id_sBSC_profil_curve,            &(_sGP_Profil_workingOn._sGP_PCurveAndEnvelop._sCTA_profilCurve._dThickness));
    _qhash_int_dThicknessPtr.insert(e_id_sBSC_linearReg_xAdjusters,    &(_sGP_Profil_workingOn._sGP_LinearRegression._sCT_xAdjuster._dThickness));
    _qhash_int_dThicknessPtr.insert(e_id_sBSC_linearReg_xCentralAxis,  &(_sGP_Profil_workingOn._sGP_LinearRegression._sCT_xCentralAxis._dThickness));
    _qhash_int_dThicknessPtr.insert(e_id_sBSC_linearReg_computedLines, &(_sGP_Profil_workingOn._sGP_LinearRegression._sCTA_computedLine._dThickness));

    qDebug() << __FUNCTION__ << "&dThicknessToSet _qhash_int_dThicknessPtr.value(e_id_sBSC_profil_curve) = "
             << _qhash_int_dThicknessPtr.value(e_id_sBSC_profil_curve);

    connect(ui->spinBox_profil_curve_thickness           , QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
    connect(ui->spinBox_linearReg_xAdjusters_thickness   , QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
    connect(ui->spinBox_linearReg_xCentralAxis_thickness , QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
    connect(ui->spinBox_linearReg_computedLines_thickness, QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);

    ui->pushButton_profil_curve_selectColor           ->setProperty("pBId", e_id_pBSC_profil_curve);
    ui->pushButton_profil_envelop_area_selectColor    ->setProperty("pBId", e_id_pBSC_profil_envelop_area);
    ui->pushButton_linearReg_xAdjusters_selectColor   ->setProperty("pBId", e_id_pBSC_linearReg_xAdjusters);
    ui->pushButton_linearReg_xCentralAxis_selectColor ->setProperty("pBId", e_id_pBSC_linearReg_xCentralAxis);
    ui->pushButton_linearReg_computedLines_selectColor->setProperty("pBId", e_id_pBSC_linearReg_computedLines);

    _qhash_int_qcolorPtr_initialForColorDialog.insert(e_id_pBSC_profil_curve,            &(_sGP_Profil_colorInEdition._sGP_PCurveAndEnvelop._sCTA_profilCurve._qColor));
    _qhash_int_qcolorPtr_initialForColorDialog.insert(e_id_pBSC_profil_envelop_area,     &(_sGP_Profil_colorInEdition._sGP_PCurveAndEnvelop._sCA_enveloppArea._qColor));
    _qhash_int_qcolorPtr_initialForColorDialog.insert(e_id_pBSC_linearReg_xAdjusters,    &(_sGP_Profil_colorInEdition._sGP_LinearRegression._sCT_xAdjuster._qColor));
    _qhash_int_qcolorPtr_initialForColorDialog.insert(e_id_pBSC_linearReg_xCentralAxis,  &(_sGP_Profil_colorInEdition._sGP_LinearRegression._sCT_xCentralAxis._qColor));
    _qhash_int_qcolorPtr_initialForColorDialog.insert(e_id_pBSC_linearReg_computedLines, &(_sGP_Profil_colorInEdition._sGP_LinearRegression._sCTA_computedLine._qColor));

    connect(ui->pushButton_profil_curve_selectColor,            &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_pushButton_selectColor_clicked);
    connect(ui->pushButton_profil_envelop_area_selectColor,     &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_pushButton_selectColor_clicked);
    connect(ui->pushButton_linearReg_xAdjusters_selectColor,    &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_pushButton_selectColor_clicked);
    connect(ui->pushButton_linearReg_xCentralAxis_selectColor,  &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_pushButton_selectColor_clicked);
    connect(ui->pushButton_linearReg_computedLines_selectColor, &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_pushButton_selectColor_clicked);

    ui->pushButton_preset1 ->setProperty("presetId", 0);
    ui->pushButton_preset2 ->setProperty("presetId", 1);
    ui->pushButton_preset3 ->setProperty("presetId", 2);

    connect(ui->pushButton_preset1, &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_graphicsSettings_profil_preset_clicked);
    connect(ui->pushButton_preset2, &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_graphicsSettings_profil_preset_clicked);
    connect(ui->pushButton_preset3, &QPushButton::clicked, this, &Dialog_graphicsSettings::slot_graphicsSettings_profil_preset_clicked);

}

/*
void Dialog_graphicsSettings::initializePart_ItemOnImageView() {

    connect(ui->checkBox_ItemOnImgView_box_antialiasing,      &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_itemOnImgView_box_antialiasing_use);
    connect(ui->checkBox_ItemOnImgView_segment_antialiasing,  &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_temOnImgView_segment_antialiasing_use);
    connect(ui->checkBox_ItemOnImgView_pointInEdition_antialiasing,  &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_itemOnImgView_pointInEdition_antialiasing_use);
    connect(ui->checkBox_ItemOnImgView_firstAndLastPoint_antialiasing,  &QCheckBox::stateChanged, this, &Dialog_graphicsSettings::slot_itemOnImgView_firstAndLastPoint_antialiasing_use);

    ui->spinBox_ItemOnImgView_box_thickness           ->setProperty("sBId", e_id_sBSC_itemOnImageView_box);
    ui->spinBox_ItemOnImgView_segment_thickness       ->setProperty("sBId", e_id_sBSC_itemOnImageView_trace_segment);
    ui->spinBox_ItemOnImgView_pointInEdition_thickness->setProperty("sBId", e_id_sBSC_itemOnImageView_trace_pointInEdition);
    ui->spinBox_ItemOnImgView_firstLastPoint_thickness->setProperty("sBId", e_id_sBSC_itemOnImageView_trace_firstAndLastPoint);

    _qhash_int_dThicknessPtr.insert(e_id_sBSC_itemOnImageView_box,                     &(_sGP_itemOnImgView_workingOn._sTA_box._dThickness));
    _qhash_int_dThicknessPtr.insert(e_id_sBSC_itemOnImageView_trace_segment,           &(_sGP_itemOnImgView_workingOn._sTA_trace_segment._dThickness));
    _qhash_int_dThicknessPtr.insert(e_id_sBSC_itemOnImageView_trace_pointInEdition,    &(_sGP_itemOnImgView_workingOn._sTA_trace_point._dThickness));
    _qhash_int_dThicknessPtr.insert(e_id_sBSC_itemOnImageView_trace_firstAndLastPoint, &(_sGP_itemOnImgView_workingOn._sTA_trace_firstAndLastPoint._dThickness));

    connect(ui->spinBox_ItemOnImgView_box_thickness           , QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
    connect(ui->spinBox_ItemOnImgView_segment_thickness       , QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
    connect(ui->spinBox_ItemOnImgView_pointInEdition_thickness, QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
    connect(ui->spinBox_ItemOnImgView_firstLastPoint_thickness, QOverload<int>::of(&QSpinBox::valueChanged), this, &Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged);
}
*/

void Dialog_graphicsSettings::slot_graphicsSettings_profil_preset_clicked() {

    QObject *ptrSender = sender();

    qDebug() << __FUNCTION__ << "sender()->objectName(); = " << sender()->objectName();

    QVariant qvariant_associated_id = ptrSender->property("presetId");
    bool bOk = false;
    int int_associated_id  = qvariant_associated_id.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associated_id.toInt()";
        return;
    }

    emit signal_graphicsSettings_profil_preset_changed(int_associated_id);
}


void Dialog_graphicsSettings::accept() {

    _sGP_Profil_reference = _sGP_Profil_workingOn;
    //_sGP_itemOnImgView_reference = _sGP_itemOnImgView_workingOn;

    emit signal_graphicsSettings_profil_changed(_sGP_Profil_workingOn, false);
    //emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_workingOn, false);

    QDialog::accept();
}

void Dialog_graphicsSettings::reject() {

    _sGP_Profil_reference.debugShow_sgpp();
    //_sGP_itemOnImgView_reference.debugShow_sgpp();

    feed_profil(_sGP_Profil_reference);
    //feed_itemOnImageView(_sGP_itemOnImgView_reference);

    emit signal_graphicsSettings_profil_changed(_sGP_Profil_reference, false);
    //emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_reference, false);

    QDialog::reject();
}

void Dialog_graphicsSettings::closeEvent(QCloseEvent *event) {

    _sGP_Profil_reference.debugShow_sgpp();

    feed_profil(_sGP_Profil_reference);
    //feed_itemOnImageView(_sGP_itemOnImgView_reference);

    emit signal_graphicsSettings_profil_changed(_sGP_Profil_reference, false);
    //emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_reference, false);

    event->accept();
}

void Dialog_graphicsSettings::slot_qColorDialog_currentColorChanged(const QColor &color) {

    if (_eIDpBSC_inColorSelection == e_id_pBSC_invalid) {
        return;
    }
    QColor *qColorPtr = _qhash_int_qcolorPtr_initialForColorDialog.value(_eIDpBSC_inColorSelection, nullptr);
    if (!qColorPtr) {
        return;
    }
    *qColorPtr = color;    

    emit signal_graphicsSettings_profil_changed(_sGP_Profil_colorInEdition, true);
}


void Dialog_graphicsSettings::slot_pushButton_selectColor_clicked() {

    QObject *ptrSender = sender();

    qDebug() << __FUNCTION__ << "sender()->objectName(); = " << sender()->objectName();

    QVariant qvariant_associated_id = ptrSender->property("pBId");
    bool bOk = false;
    int int_associated_id  = qvariant_associated_id.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associated_id.toInt()";
        return;
    }

    if (  (int_associated_id < e_id_pBSC_profil_curve)
        ||(int_associated_id > e_id_pBSC_linearReg_computedLines)) {
        qDebug() << __FUNCTION__ << " error: invalid int_associated_id ( " << int_associated_id << " )";
        return;
    }

    _eIDpBSC_inColorSelection = static_cast<e_id_pushButtonSelectColor>(int_associated_id);

    qDebug() << __FUNCTION__ << "qhash_int_qcolor_initialForColorDialog.value(int_associated_id) = " << *(_qhash_int_qcolorPtr_initialForColorDialog.value(int_associated_id));

    for (int iKey = e_id_pBSC_profil_curve; iKey <= e_id_pBSC_linearReg_computedLines; iKey++) {
        _qDialColor.setCustomColor(iKey,*(_qhash_int_qcolorPtr_initialForColorDialog.value(iKey)));
    }
    _qDialColor.setCurrentColor((*(_qhash_int_qcolorPtr_initialForColorDialog.value(int_associated_id))).rgb());

    int iDialExecReport = _qDialColor.exec();
    if (iDialExecReport == QDialog::Rejected) {
        _sGP_Profil_colorInEdition = _sGP_Profil_workingOn;
        syncUiTo_profil(&_sGP_Profil_workingOn);
        emit signal_graphicsSettings_profil_changed(_sGP_Profil_workingOn, true);

    } else { //QDialog::Accepted
        _sGP_Profil_workingOn = _sGP_Profil_colorInEdition;
        syncUiTo_profil(&_sGP_Profil_workingOn);

    }
}

void Dialog_graphicsSettings::slot_spinBox_thickness_valueChanged(int intValue) {
    QObject *ptrSender = sender();
    qDebug() << __FUNCTION__ << "sender()->objectName(); = " << sender()->objectName();

    QVariant qvariant_associated_id = ptrSender->property("sBId");
    bool bOk = false;
    int int_associated_id  = qvariant_associated_id.toInt(&bOk);
    if (!bOk) {
        qDebug() << __FUNCTION__ << " error: fail in qvariant_associated_id.toInt()";
        return;
    }
    if (  (int_associated_id < e_id_sBSC_profil_curve)
        ||(int_associated_id > e_id_sBSC_itemOnImageView_trace_firstAndLastPoint)) {
        qDebug() << __FUNCTION__ << " error: invalid int_associated_id ( " << int_associated_id << " )";
        return;
    }

    qDebug() << __FUNCTION__ << "intValue = " << intValue;
    if (intValue < 1) {
        intValue = 1;
        qDebug() << __FUNCTION__ << "intValue forced to  = " << intValue;
    }
    double *dThicknessToSet = _qhash_int_dThicknessPtr.value(int_associated_id, nullptr);
    if (!dThicknessToSet) {
        qDebug() << __FUNCTION__ << " if (!dThicknessToSet) {";
        return;
    }

    qDebug() << __FUNCTION__ << "&dThicknessToSet =" << dThicknessToSet;
    *dThicknessToSet =  static_cast<double>(intValue);

    if (int_associated_id < e_id_sBSC_itemOnImageView_box) {
        _sGP_Profil_colorInEdition = _sGP_Profil_workingOn; //to be able to mix colorInEditionAnd
        emit signal_graphicsSettings_profil_changed(_sGP_Profil_workingOn, true);
    } else {
        //emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_workingOn, true);
    }

}

Dialog_graphicsSettings::~Dialog_graphicsSettings() {
    delete ui;
}

void Dialog_graphicsSettings::slot_profil_curve_antialiasing_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_Profil_workingOn._sGP_PCurveAndEnvelop._sCTA_profilCurve._bAntialiasing = bChecked;
    _sGP_Profil_colorInEdition = _sGP_Profil_workingOn; //to be able to mix colorInEdition and workingOn
    emit signal_graphicsSettings_profil_changed(_sGP_Profil_workingOn, true);
}

void Dialog_graphicsSettings::slot_envelop_area_antialising_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_Profil_workingOn._sGP_PCurveAndEnvelop._sCA_enveloppArea._bAntialiasing = bChecked;
    _sGP_Profil_colorInEdition = _sGP_Profil_workingOn; //to be able to mix colorInEdition and workingOn
    emit signal_graphicsSettings_profil_changed(_sGP_Profil_workingOn, true);
}

void Dialog_graphicsSettings::slot_linearReg_computedLines_antialising_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_Profil_workingOn._sGP_LinearRegression._sCTA_computedLine._bAntialiasing = bChecked;
    _sGP_Profil_colorInEdition = _sGP_Profil_workingOn; //to be able to mix colorInEdition and workingOn
    emit signal_graphicsSettings_profil_changed(_sGP_Profil_workingOn, true);
}

/*
void Dialog_graphicsSettings::slot_itemOnImgView_box_antialiasing_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_itemOnImgView_workingOn._sTA_box._bAntialiasing = bChecked;
    emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_workingOn, true);
}

void Dialog_graphicsSettings::slot_temOnImgView_segment_antialiasing_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_itemOnImgView_workingOn._sTA_trace_segment._bAntialiasing = bChecked;
    emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_workingOn, true);
}

void Dialog_graphicsSettings::slot_itemOnImgView_pointInEdition_antialiasing_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_itemOnImgView_workingOn._sTA_trace_point._bAntialiasing = bChecked;
    emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_workingOn, true);
}

void Dialog_graphicsSettings::slot_itemOnImgView_firstAndLastPoint_antialiasing_use(int iCheckState) {
    bool bChecked = (iCheckState == Qt::Checked);
    _sGP_itemOnImgView_workingOn._sTA_trace_firstAndLastPoint._bAntialiasing = bChecked;
    emit signal_graphicsSettings_itemOnImgView_changed(_sGP_itemOnImgView_workingOn, true);
}
*/

void Dialog_graphicsSettings::setUi_allEmptyAndDisabled() {
    ui->page_profilEdition->setEnabled(false);
    //ui->page_ItemOnImageView->setEnabled(false);
}

void Dialog_graphicsSettings::feed_profil(const S_GraphicsParameters_Profil& sGP_Profil_current) {
    _sGP_Profil_reference = sGP_Profil_current;
    _bFeed_sGPProfil_reference = true;
    _sGP_Profil_workingOn = _sGP_Profil_reference;
    _sGP_Profil_colorInEdition = _sGP_Profil_workingOn;
    syncUiTo_profil(&_sGP_Profil_workingOn);
    ui->page_profilEdition->setEnabled(true);
}

/*
void Dialog_graphicsSettings::feed_itemOnImageView(const S_GraphicsParameters_ItemOnImageView& sGP_itemOnImageView_current) {
    _sGP_itemOnImgView_reference = sGP_itemOnImageView_current;
    _bFeed_sGPitemOnImgView_reference = true;
    _sGP_itemOnImgView_workingOn = _sGP_itemOnImgView_reference;
    syncUiTo_itemOnImageView(&_sGP_itemOnImgView_workingOn);
    ui->page_ItemOnImageView->setEnabled(true);
}
*/

void Dialog_graphicsSettings::feed_profilFromPreset_cancelable(const S_GraphicsParameters_Profil& sGP_Profil) {
    _sGP_Profil_workingOn = sGP_Profil;
    _sGP_Profil_colorInEdition = _sGP_Profil_workingOn;
    syncUiTo_profil(&_sGP_Profil_workingOn);
    ui->page_profilEdition->setEnabled(true);
}

void Dialog_graphicsSettings::syncUiTo_profil(S_GraphicsParameters_Profil *ptr_sGraphicsParameters_Profil) {
    ui->label_profil_curve_colorPreview           ->setStyleSheet(buildQString_backgroundStyleSheet(ptr_sGraphicsParameters_Profil->_sGP_PCurveAndEnvelop._sCTA_profilCurve._qColor));
    ui->label_profil_envelop_area_colorPreview    ->setStyleSheet(buildQString_backgroundStyleSheet(ptr_sGraphicsParameters_Profil->_sGP_PCurveAndEnvelop._sCA_enveloppArea._qColor));
    ui->label_linearReg_xAdjusters_colorPreview   ->setStyleSheet(buildQString_backgroundStyleSheet(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCT_xAdjuster._qColor));
    ui->label_linearReg_xCentralAxis_colorPreview ->setStyleSheet(buildQString_backgroundStyleSheet(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCT_xCentralAxis._qColor));
    ui->label_linearReg_computedLines_colorPreview->setStyleSheet(buildQString_backgroundStyleSheet(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCTA_computedLine._qColor));

    ui->checkBox_profil_curve_antialiasing          ->setChecked(ptr_sGraphicsParameters_Profil->_sGP_PCurveAndEnvelop._sCTA_profilCurve._bAntialiasing);
    ui->checkBox_profil_envelop_area_antialising    ->setChecked(ptr_sGraphicsParameters_Profil->_sGP_PCurveAndEnvelop._sCA_enveloppArea._bAntialiasing);
    ui->checkBox_linearReg_computedLines_antialising->setChecked(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCTA_computedLine._bAntialiasing);

    ui->spinBox_profil_curve_thickness           ->setValue(static_cast<int>(ptr_sGraphicsParameters_Profil->_sGP_PCurveAndEnvelop._sCTA_profilCurve._dThickness));
    ui->spinBox_linearReg_xAdjusters_thickness   ->setValue(static_cast<int>(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCT_xAdjuster._dThickness));
    ui->spinBox_linearReg_xCentralAxis_thickness ->setValue(static_cast<int>(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCT_xCentralAxis._dThickness));
    ui->spinBox_linearReg_computedLines_thickness->setValue(static_cast<int>(ptr_sGraphicsParameters_Profil->_sGP_LinearRegression._sCTA_computedLine._dThickness));
}

/*
void Dialog_graphicsSettings::syncUiTo_itemOnImageView(S_GraphicsParameters_ItemOnImageView *ptr_sGraphicsParameters_itemOnImageView) {
    ui->checkBox_ItemOnImgView_box_antialiasing    ->setChecked(ptr_sGraphicsParameters_itemOnImageView->_sTA_box._bAntialiasing);
    ui->checkBox_ItemOnImgView_segment_antialiasing->setChecked(ptr_sGraphicsParameters_itemOnImageView->_sTA_trace_segment._bAntialiasing);
    ui->checkBox_ItemOnImgView_pointInEdition_antialiasing   ->setChecked(ptr_sGraphicsParameters_itemOnImageView->_sTA_trace_point._bAntialiasing);
    ui->checkBox_ItemOnImgView_firstAndLastPoint_antialiasing->setChecked(ptr_sGraphicsParameters_itemOnImageView->_sTA_trace_firstAndLastPoint._bAntialiasing);

    ui->spinBox_ItemOnImgView_box_thickness    ->setValue(static_cast<int>(ptr_sGraphicsParameters_itemOnImageView->_sTA_box._dThickness));
    ui->spinBox_ItemOnImgView_segment_thickness->setValue(static_cast<int>(ptr_sGraphicsParameters_itemOnImageView->_sTA_trace_segment._dThickness));
    ui->spinBox_ItemOnImgView_pointInEdition_thickness->setValue(static_cast<int>(ptr_sGraphicsParameters_itemOnImageView->_sTA_trace_point._dThickness));
    ui->spinBox_ItemOnImgView_firstLastPoint_thickness->setValue(static_cast<int>(ptr_sGraphicsParameters_itemOnImageView->_sTA_trace_firstAndLastPoint._dThickness));
}
*/

QString Dialog_graphicsSettings::buildQString_backgroundStyleSheet(const QColor& qColor) {
    int r=0, g=0, b=0;
    QString qstrRGB;

    qColor.getRgb(&r,&g,&b);
    //example: background-color: rgb(245, 121, 0);
    qstrRGB= QString("%1, %2, %3").arg(r).arg(g).arg(b);
    QString qstrStyleSheet = "QLabel { background-color: rgb(" + qstrRGB +"); }";

    //qDebug() << __FUNCTION__ << "r g b = " << r << ", " <<g << ", " << b;
    //qDebug() << __FUNCTION__ << "qstrStyleSheet = " << qstrStyleSheet;

    return(qstrStyleSheet);
}
