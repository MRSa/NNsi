/*!
 *  Qt4�p���C���i�N���j����
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

    // ����N���X�̐���
    mainControl  *guiController = new mainControl();

    // �|��t�@�C��������ꍇ�ɂ́A���[�h����
    QTranslator translator;
    QString     transFile = guiController->getTranslateFileName();
    if (transFile.isEmpty() == false)
    {
        translator.load(transFile);
        app.installTranslator(&translator);
    }

    // ���C���E�B���h�E��ݒ肷��
    guiController->setupMainUi(&window);

    // �N������...
    guiController->startApp(argc, argv);

    // ��ʂ̕\��
    window.show();
    int ret = app.exec();

    // �I������...
    guiController->finishApp();
    delete guiController;
    return (ret);
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
