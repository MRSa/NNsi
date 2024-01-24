/*!
 *  Qt4�p���C���i�N���j����
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
#include "ui_wizard.h"      // *.ui �t�@�C�����玩����������� (ui_*.h)
#include "screenControl.h"  // �t�H�[�������삳�ꂽ�Ƃ��ɍs���C�x���g�����N���X
#include "screenWidget.h"

int main(int argc, char *argv[])
{
    QApplication    app(argc, argv);
    QPixmap         pixmap(":/pics/splash.png");
    QSplashScreen   splash(pixmap);
    splash.show();

    // �|��t�@�C���̏���
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
    Ui::a2bInstaller appForm;   // *.ui�t�@�C���ɋL�q���Ă��郁�C���E�B���h�E��
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
