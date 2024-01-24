#ifndef MAINPROCEDURES_H
#define MAINPROCEDURES_H

#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <Qhttp>

//
//  UIC自動生成フォーム
//
#include "ui_mainForm.h"


//
//  ユーザー制御クラス
//
#include "a2BChttpConnection.h"

/*!
 *  主制御クラス
 *
 */
class mainProcedures : public QWidget
{
    Q_OBJECT

public:
    mainProcedures(Ui::mainForm *uiForm, QWidget *parent = 0);
    virtual ~mainProcedures();

public:
    void startup(void);
    void startApp(void);
    void finishApp(void);

public slots:
    void show_aboutQt(bool checked);
    void show_version(bool checked);
    void selectDirectory();
    void getDataFile();
    void deleteDataFile();
    void moveToToday();
    void updateLogInformation();
    void completedCommunication(int id, QString &arMsg);

signals:

private:
    void prepare(void);
    void extractData(int lineData, QString &arLineData);

private:
    Ui::mainForm   *ui;

    QHttp               mHttpIndex0;
    QHttp               mHttpIndex1;
    QHttp               mHttpIndex2;

    a2BChttpConnection *mpHttpConn0;
    a2BChttpConnection *mpHttpConn1;
    a2BChttpConnection *mpHttpConn2;
};

#endif // #ifndef MAINCONTROL_H
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
