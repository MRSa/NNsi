/*!
 *  Qt4用メイン（起動）部分
 *
 *
 */
#include <QCoreApplication>
#include <QApplication>
#include <QTranslator>
#include <QtPlugin>
#include <QTextCodecPlugin>
#include <QPluginLoader>
#include "qmainwindow.h"
#include "ui_a2BC.h"
#include "screenControl.h"

Q_IMPORT_PLUGIN(qjpcodecs)

int main(int argc, char *argv[])
{
    QApplication      app(argc, argv);
    QMainWindow       window;
    Ui::a2BCMain      appForm;
    screenControl    *guiController;

    QString path = QCoreApplication::applicationDirPath();
    app.addLibraryPath(path); 

    QTranslator translator;
    translator.load("a2BC_jp");
    app.installTranslator(&translator);

    appForm.setupUi(&window);
    guiController = new screenControl(&appForm, path);

    window.show();
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    int ret = app.exec();
    guiController->finishApp();
    delete guiController;
    return (ret);
}
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
