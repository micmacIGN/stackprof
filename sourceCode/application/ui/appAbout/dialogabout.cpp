#include "dialogabout.h"
#include "ui_dialogabout.h"

#include "../../logic/model/appVersionInfos/appVersionInfos.hpp"

DialogAbout::DialogAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAbout) {

    ui->setupUi(this);

    ui->label_appname->setText(AppVersionInfos::_sctct_softwareName);

    QString AppVersionInfos = AppVersionInfos::_sctct_appVersion + QString("  (") + AppVersionInfos::_sctct_appVersionDeliveryDate + QString(")") ;
    ui->label_appVersionInfos->setText(AppVersionInfos);

    ui->label_contactAdress->setText(AppVersionInfos::_sctct_mainContact);
    ui->label_credits->setText(AppVersionInfos::_sctct_developpementCreditsText);
    ui->label_licenseType->setText(AppVersionInfos::_sctct_licenceType);

    auto qlistStr = AppVersionInfos::get_thirdPartyOpenSourceLibsUsed_nameAndVersion();
    int qlistStrSize = qlistStr.size();
    QString strThirdPartyLibOpenSourcelibrariesUsed;
    for (int i = 0; i < qlistStrSize; i++) {
        strThirdPartyLibOpenSourcelibrariesUsed += qlistStr[i];
        if (i < qlistStrSize-1) {
            strThirdPartyLibOpenSourcelibrariesUsed+="\n";
        }
    }
    ui->textBrowser_thirdPartyLibOpenSourcelibrariesUsed->setText(strThirdPartyLibOpenSourcelibrariesUsed);


    connect(ui->pushButton_close, &QPushButton::clicked, this, &QDialog::accept);
}


DialogAbout::~DialogAbout() {
    delete ui;
}
