/*
 *   画面制御部..
 *
 */
#include <QCoreApplication>
#include <QtGlobal>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDate>
#include <QTextStream>
#include "aplVersion.h"
#include "mainProcedures.h"

/*
 *  コンストラクタ (画面表示準備)
 *
 */
mainProcedures::mainProcedures(Ui::mainForm *uiForm, QWidget *parent) : QWidget(parent)
{
    // フォームを記録する
    ui = uiForm;

    // 準備メソッド
    prepare();
}

/*
 *  デストラクタ (画面表示のあとしまつ)
 *
 */
mainProcedures::~mainProcedures()
{
    delete mpHttpConn2;
    delete mpHttpConn1;
    delete mpHttpConn0;
}

/*
 *  クラスの準備...
 *
 */
void mainProcedures::prepare(void)
{
    mpHttpConn0 = new a2BChttpConnection(&mHttpIndex0);
    mpHttpConn1 = new a2BChttpConnection(&mHttpIndex1);
    mpHttpConn2 = new a2BChttpConnection(&mHttpIndex2);

    connect(mpHttpConn0, SIGNAL(completedCommunication(int, QString &)),
            this,        SLOT(completedCommunication(int, QString &)));

    connect(mpHttpConn1, SIGNAL(completedCommunication(int, QString &)),
            this,        SLOT(completedCommunication(int, QString &)));

    connect(mpHttpConn2, SIGNAL(completedCommunication(int, QString &)),
            this,        SLOT(completedCommunication(int, QString &)));


    return;
}

/*
 *  Qtの情報を表示
 *
 */
void mainProcedures::show_aboutQt(bool checked)
{
    // Qtのバージョン情報を表示する
    QMessageBox::aboutQt(this);
    return;
}

/*
 *  バージョン情報の表示
 *
 */
void mainProcedures::show_version(bool checked)
{
    QString caption = "about " + APL_NAME;
    QString message = APL_CREDIT + "\n" + APL_VERSION + " (" + APL_DATE + ")\n\n" + APL_COMMENT;

    QMessageBox::information(this, caption, message, QMessageBox::Ok);
    return;
}

/*
 *  アプリケーションのスタート！
 *
 */
void mainProcedures::startup(void)
{
    return;
}

/*
 *  アプリケーションの開始！
 *
 */
void mainProcedures::startApp(void)
{
    // データファイルの読み出し...
    QString dataFileName = QCoreApplication::applicationDirPath() + "/qTVg.dat";
    if (QFile::exists(dataFileName) == false)
    {
        // データファイルがなかった...終了する
        return;
    }
    QFile dataFile(dataFileName);
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // データファイルオープンに失敗、、、終了する
        return;
    }

    // データファイルを１行づつ読み出す
    int count = 0;
    QTextStream in(&dataFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        extractData(count, line);
        count++;
    }
    dataFile.close();

    // データを画面に反映！
    updateLogInformation();
    return;
}

/*
 *  データを読み出して展開する
 */
void mainProcedures::extractData(int lineData, QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        // データなし...何もせず終了する
    }

    int     index = 0;
    QString dataString = arLineData.section(";", 0, 0);
    switch (lineData)
    {
      case 0:
        // データ記録ディレクトリ名
        (ui->fldDataDirectory)->setText(dataString);
        break;

      case 1:
        // 地域情報
        index = (ui->cmbAreaId)->findData(QVariant(dataString.toInt()));
        if (index >= 0)
        {
            (ui->cmbAreaId)->setCurrentIndex(index);
        }
        break;

      default:
        // 何もしない
        break;
    }
    //    QMessageBox::information(0, "", dataFile);
    return;
}

/*
 *  アプリケーションの終了！
 *
 */
void mainProcedures::finishApp(void)
{
    // データファイルの書き出し...
    QString dataFileName = QCoreApplication::applicationDirPath() + "/qTVg.dat";

    // 古いファイルがあった場合、削除する
    if (QFile::exists(dataFileName) == true)
    {
        QFile::remove(dataFileName);
    }

    // 書き出し用にオープン
    QFile dataFile(dataFileName);
    if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&dataFile);
    out << (ui->fldDataDirectory)->text() << ";" << endl;

    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();
    out << areaId << ";" << endl;

    out.flush();
    dataFile.close();

    return;
}

/*
 *  「ディレクトリ設定」ボタンを押したとき...
 *
 */
void mainProcedures::selectDirectory()
{
    QString fileName = (ui->fldDataDirectory)->text();
    fileName = QFileDialog::getExistingDirectory(0, tr("Directory Selection"), fileName);
    if (fileName.isEmpty() != true)
    {
        (ui->fldDataDirectory)->setText(fileName);
    }
    return;
}

/*
 *  「データ取得」ボタンを押したとき...
 *
 */
void mainProcedures::getDataFile()
{
    if (QMessageBox::question(0, tr("HTTP COMM."), tr("Data Get, Ready?"), 
        (QMessageBox::Ok | QMessageBox::Cancel)) != QMessageBox::Ok)
    {
        // キャンセルされた...取得しない。（終了する）
        return;
    }

    // 地域IDを取得する
    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();

    // ファイル名ヘッダを作成する
    QString cookie;
    cookie.setNum(areaId);
    cookie = "areaId=" + cookie + "; span=24";
    QDate currentDate = (ui->calendarWidget)->selectedDate();
    QString dateString;
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-";
    dateString = (ui->fldDataDirectory)->text() + "/" + dateString;
    QString urlHeader = "http://tv.so-net.ne.jp/guest/pc/chart.action?&span=24&head=" + currentDate.toString("yyyyMMdd") + "0500" + "&index=";

    if ((ui->checkBox_0)->isChecked() == true)
    {
        // 地上波のデータファイルを取得する
        QString dataFileName = dateString + "0.html";
        QString urlToGet = urlHeader + "0";
        mpHttpConn0->setCookie(cookie);
        mpHttpConn0->getFileFromUrl(dataFileName, urlToGet, false);
    }

    if ((ui->checkBox_1)->isChecked() == true)
    {
        // UHF/BSのデータファイルを取得する
        QString dataFileName = dateString + "1.html";
        QString urlToGet = urlHeader + "1";
        mpHttpConn1->setCookie(cookie);
        mpHttpConn1->getFileFromUrl(dataFileName, urlToGet, false);
    }

    if ((ui->checkBox_2)->isChecked() == true)
    {
        // BSデジタルのデータファイルを削除する
        QString dataFileName = dateString + "2.html";
        QString urlToGet = urlHeader + "2";
        mpHttpConn2->setCookie(cookie);
        mpHttpConn2->getFileFromUrl(dataFileName, urlToGet, false);
    }

    // 情報を更新する
    updateLogInformation();
}

/*
 *  「データ削除」ボタンを押したとき...
 *
 */
void mainProcedures::deleteDataFile()
{
    if (QMessageBox::question(0, tr("delete File"), tr("File Delete, OK?"), 
        (QMessageBox::Ok | QMessageBox::Cancel)) != QMessageBox::Ok)
    {
        // キャンセルされた...削除しない。（終了する）
        return;
    }

    // 地域IDを取得する
    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();

    // ファイル名ヘッダを作成する
    QDate currentDate = (ui->calendarWidget)->selectedDate();
    QString dateString;
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-";
    dateString = (ui->fldDataDirectory)->text() + "/" + dateString;

    if ((ui->checkBox_0)->isChecked() == true)
    {
        // 地上波のデータファイルを削除する
        QString dataFileName = dateString + "0.html";
        if (QFile::exists(dataFileName) == true)
        {
            QFile::remove(dataFileName);
        }
    }

    if ((ui->checkBox_1)->isChecked() == true)
    {
        // UHF/BSのデータファイルを削除する
        QString dataFileName = dateString + "1.html";
        if (QFile::exists(dataFileName) == true)
        {
            QFile::remove(dataFileName);
        }
    }

    if ((ui->checkBox_2)->isChecked() == true)
    {
        // BSデジタルのデータファイルを削除する
        QString dataFileName = dateString + "2.html";
        if (QFile::exists(dataFileName) == true)
        {
            QFile::remove(dataFileName);
        }
    }

    // 情報を更新する
    updateLogInformation();
}

/*
 *  「今日」ボタンを押したときの処理...
 *
 */
void mainProcedures::moveToToday()
{
    (ui->calendarWidget)->setSelectedDate(QDate::currentDate());
    updateLogInformation();
}

/*
 *  画面情報を更新する
 *
 */
void mainProcedures::updateLogInformation()
{
    QDate currentDate = (ui->calendarWidget)->selectedDate();
    QString dateString = currentDate.toString("yyyy/MM/dd");
    (ui->lblDate)->setText(dateString);

    // 地域IDを取得する
    int index = (ui->cmbAreaId)->currentIndex();
    int areaId = ((ui->cmbAreaId)->itemData(index)).toInt();

    // 地上波のファイルがあるかどうか？
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-" + "0.html";
    bool isIndex0 = QFile::exists((ui->fldDataDirectory)->text() + "/" + dateString);
    if (isIndex0 == true)
    {
        (ui->dataIndex_0)->setText(tr("Exist"));
    }
    else
    {
        (ui->dataIndex_0)->setText(tr("None"));
    }

    // UHF/BSのファイルがあるか？
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-" + "1.html";
    bool isIndex1 = QFile::exists((ui->fldDataDirectory)->text() + "/" + dateString);
    if (isIndex1 == true)
    {
        (ui->dataIndex_1)->setText(tr("Exist"));
    }
    else
    {
        (ui->dataIndex_1)->setText(tr("None"));
    }

    // BSデジタルのファイルがあるか？
    dateString.setNum(areaId);
    dateString = currentDate.toString("yyyyMMdd") + "-" + dateString + "-" + "2.html";
    bool isIndex2 = QFile::exists((ui->fldDataDirectory)->text() + "/" + dateString);
    if (isIndex2 == true)
    {
        (ui->dataIndex_2)->setText(tr("Exist"));
    }
    else
    {
        (ui->dataIndex_2)->setText(tr("None"));
    }

    // 
    bool enableGetButton = true;
    if (mpHttpConn0 != 0)
    {
       if (mpHttpConn0->isCommunicating() == true)
       {
           enableGetButton = false;
       }
    }
    if (mpHttpConn1 != 0)
    {
       if (mpHttpConn1->isCommunicating() == true)
       {
           enableGetButton = false;
       }
    }
    if (mpHttpConn2 != 0)
    {
       if (mpHttpConn2->isCommunicating() == true)
       {
           enableGetButton = false;
       }
    }

    // ログ取得ボタンを制御する
    (ui->btnGet)->setEnabled(enableGetButton);
    (ui->btnClear)->setEnabled(enableGetButton);
}

/**
 *  通信終了を受信。。。
 *
 */
void mainProcedures::completedCommunication(int id, QString &arMsg)
{
    updateLogInformation();
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
