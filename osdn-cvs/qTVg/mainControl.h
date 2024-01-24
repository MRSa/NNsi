#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <QMainWindow>
#include <QWidget>
#include <QString>

//
//  UIC���������t�H�[��
//
#include "ui_mainForm.h"

//
//  ���C���̎葱��(?)��
//
#include "mainProcedures.h"

/*!
 *  �吧��N���X
 *
 */
class mainControl : public QWidget
{
    Q_OBJECT

public:
    mainControl(QWidget *parent = 0);
    virtual ~mainControl();

public:
    virtual void setupMainUi(QMainWindow *apWidget);
    virtual QString getTranslateFileName(void);

    virtual void startApp(int argc, char *argv[]);
    virtual void finishApp(void);

private:
    void prepareObjects(void);
    void prepareDatas(void);
    void connectSignals(void);
    void startup(void);

private:
    Ui::mainForm       *mpMainForm;
    mainProcedures     *mpMainProcedures;

};
#endif // #ifndef MAINCONTROL_H
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
