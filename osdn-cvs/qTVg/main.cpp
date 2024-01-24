/*!
 *  Qt4用メイン（起動）部分
 *
 *
 */
#include <QCoreApplication>
#include <QApplication>
#include <QTranslator>
#include <QString>
#include "qmainwindow.h"
#include "mainControl.h"

int main(int argc, char *argv[])
{
    QApplication    app(argc, argv);
    QMainWindow     window;

    // 制御クラスの生成
    mainControl  *guiController = new mainControl();

    // 翻訳ファイルがある場合には、ロードする
    QTranslator translator;
    QString     transFile = guiController->getTranslateFileName();
    if (transFile.isEmpty() == false)
    {
        translator.load(transFile);
        app.installTranslator(&translator);
    }

    // メインウィンドウを設定する
    guiController->setupMainUi(&window);

    // 起動処理...
    guiController->startApp(argc, argv);

    // 画面の表示
    window.show();
    int ret = app.exec();

    // 終了処理...
    guiController->finishApp();
    delete guiController;
    return (ret);
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
