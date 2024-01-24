/*!
 *   画面制御部..
 *
 */
#include <QtGlobal>
#include <QCoreApplication>
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include "screenControl.h"

/*!
 *   コンストラクタ
 *
 */
screenControl::screenControl(Ui::a2bInstaller *apForm, QWidget *parent)
  : QWidget(parent), mpForm(apForm)
{
    mDB.prepare(apForm);
    connectSignals();
}

/*!
 *  connectSignals()  : シグナルとスロットをつなぐ処理...
 *
 */
void screenControl::connectSignals(void)
{
    qDebug("screenControl::connectSignals()");

    // Abortボタンが押されたとき...
    connect(mpForm->btnAbort, SIGNAL(clicked()),
            this,             SLOT(clicked_ButtonAbort()));

    // 戻るボタンが押されたとき...
    connect(mpForm->btnBack, SIGNAL(clicked()),
            this,            SLOT(clicked_ButtonBack()));

    // 次へボタンが押されたとき...
    connect(mpForm->btnNext, SIGNAL(clicked()),
            this,            SLOT(clicked_ButtonNext()));

    // 参照ボタンが押されたとき...
    connect(mpForm->detailButton, SIGNAL(clicked()),
            this,                 SLOT(clicked_detailButton()));

    // テキストが更新されたとき...
    connect(mpForm->lineEdit, SIGNAL(returnPressed()),
            this,             SLOT(clicked_EditText()));
    connect(mpForm->lineEdit, SIGNAL(editingFinished()),
            this,             SLOT(clicked_EditText()));

    return;
}

/*!
 *    ボタンが押されたときの処理
 *
 */
void screenControl::clicked_ButtonAbort()
{
    qDebug("screenControl::clicked_ButtonAbort()");

    mDB.doAbort();
    return;
}

/*!
 *    戻るボタンが押されたときの処理
 *
 */
void screenControl::clicked_ButtonBack()
{
    qDebug("screenControl::clicked_ButtonBack()");

    mDB.doBack();
    return;
}

/*!
 *    次へボタンが押されたときの処理
 *
 */
void screenControl::clicked_ButtonNext()
{
    qDebug("screenControl::clicked_ButtonNext()");

    mDB.doNext();
    return;
}

/*!
 *    参照ボタンが押されたときの処理
 *
 */
void screenControl::clicked_detailButton()
{
    qDebug("screenControl::clicked_detailButton()");

    mDB.setDetail();
    return;
}

/*!
 *   Qt のクレジットを表示する
 *
 */
void screenControl::aboutQt()
{
    qDebug("screenControl::aboutQt()");

    QApplication::aboutQt();
    return;
}

/*!
 *  必要なファイルが存在するか確認する
 *
 */
bool  screenControl::checkFilesExist(void)
{
    // a2B.jadの存在チェック
    if (QFile::exists("a2B.jad") == false)
    {
        QMessageBox::critical(0, tr("ERROR(jad)"), tr("Cannot find a2B.jad."));
        return (false);
    }

    // a2B.jarの存在チェック
    if (QFile::exists("a2B.jar") == false)
    {
        QMessageBox::critical(0, tr("ERROR(jar)"), tr("Cannot find a2B.jar."));
        return (false);
    }

    // bbstable.htmlの存在チェック
    if (QFile::exists("bbstable.html") == false)
    {
        QMessageBox::critical(0, tr("ERROR(bbstable)"), tr("Cannot find bbstable.html."));
        return (false);
    }
    return (true);
}

/*!
 *    テキスト編集が行われたときの処理
 *
 */
void screenControl::clicked_EditText()
{
    qDebug("screenControl::clicked_EditText()");

    mDB.editTextFinished();
    return;
}

/*!
 *  画面の初期化
 *
 */
void screenControl::prepare(void)
{
    mDB.doStart();
    return;
}
