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
#include <QPixmap>
#include <QSplashScreen>

#include "qmainwindow.h"
#include "ui_wizard.h"      // *.ui ファイルから自動生成される (ui_*.h)
#include "screenControl.h"  // フォームが操作されたときに行うイベント処理クラス
#include "screenWidget.h"

int main(int argc, char *argv[])
{
    QApplication    app(argc, argv);
    QPixmap         pixmap(":/pics/splash.png");
    QSplashScreen   splash(pixmap);
    splash.show();

    // 翻訳ファイルの準備
    QTranslator translator;
    translator.load("translate_jp");
    app.installTranslator(&translator);

    // app.processEvents();
    // splash->showMessage("Established connections");
    // app.processEvents();
    // splash->showMessage("Established connections");
    //  :
    //  :

    QMainWindow      window;
    Ui::a2bInstaller appForm;   // *.uiファイルに記述しているメインウィンドウ名
    screenWidget     anotherWidget;
    screenControl   *guiController;

    appForm.setupUi(&window);
    guiController = new screenControl(&appForm);

    int ret = -1;
    if (anotherWidget.prepare(&window, &appForm, guiController) != false)
    {
        splash.finish(&window);
        ret = app.exec();
    }
    return (ret);
}
