//#include "mainwindow.h"

#include <QApplication>

#include "../../application/ui/appSettings/dialog_graphicsSettings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();

    Dialog_graphicsSettings test_dial_graphicsSettings;
    test_dial_graphicsSettings.show();

    return a.exec();
}
