/*!
 *   実処理部分...
 *
 */
#include "screenData.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

/*!
 *  コンストラクタ
 */
screenData::screenData(QWidget *parent)
  : QWidget(parent), wizardForm(0), currentScene(0)
{
  // do nothing!
}

/*!
 *  準備する
 *
 */
void screenData::prepare(Ui::a2bInstaller *apForm)
{
    wizardForm = apForm;
    return;
}

/*!
 *  アボートボタンが押されたとき、、、
 *
 */
void screenData::doAbort(void)
{
    if (currentScene < 3)
    {
        // キャンセルを実行するか確認する...
        if (QMessageBox::question(0, tr("Cancel Confirmation"), tr("End program, are you OK?"), QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
        {
            // 終了しない、、、元にもどる
            return;
        }
    }
    // アプリケーションの終了...
    QApplication::exit();
    return;
}

/*!
 *  「戻る」ボタンを押したとき...
 *
 */
void screenData::doBack(void)
{
    if (currentScene != 0)
    {
        currentScene--;
    }
    updateScreen();
    return;
}

/*!
 *  「次へ」ボタンを押したとき...
 *
 */
void screenData::doNext(void)
{
    // シーケンスを増やす...
    currentScene++;
    updateScreen();
    return;
}

/*!
 *  初期画面を開く...
 *
 */
void screenData::doStart(void)
{
    currentScene = 0;
    updateScreen();
    return;
}

/*!
 *  「詳細」ボタンが押されたときの処理...
 *
 */
void screenData::setDetail(void)
{
     QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
     if (dir.isEmpty() == false)
     {
        // ディレクトリエリアに反映させる
        (wizardForm->lineEdit)->setText(dir);
        editTextFinished();
     }
     return;
}

/*!
 *  画面を切り替える...
 *
 */
void screenData::updateScreen(void)
{
    switch (currentScene)
    {
      case 3:
      default:
        // コピー実行の確認
        setEndScreen();
        break;

      case 2:
        // コピー実行の確認
        setConfirmCopy();
        break;

      case 1:
        // ログ格納ディレクトリの指定
        setSpecifyDirectory();
        break;

      case 0:
        // ようこそ画面...
        setStartScreen();
        break;
    }
    return;
}

/*!
 *  開始画面の設定...
 *
 */
void screenData::setStartScreen(void)
{
    // 「次へ」と「キャンセル」は有効、「戻る」は無効
    (wizardForm->btnBack)->setEnabled(false);
    (wizardForm->btnNext)->setEnabled(true);
    (wizardForm->btnAbort)->setEnabled(true);

    // ディレクトリ関連の設定は表示しない...
    (wizardForm->label)->hide();
    (wizardForm->lineEdit)->hide();
    (wizardForm->detailButton)->hide();

    (wizardForm->textField)->setEnabled(true);
    (wizardForm->textField)->setHtml(tr("<html><head>Start</head><body>Welcome!</body></html>"));

    return;
}

/*!
 *  ディレクトリを指定する
 *
 */
void screenData::setSpecifyDirectory(void)
{
    // 戻る、進む、キャンセルは有効...
    (wizardForm->btnBack)->setEnabled(true);
    (wizardForm->btnNext)->setEnabled(false);
    (wizardForm->btnAbort)->setEnabled(true);

    // ディレクトリ関連の設定は表示する...
    (wizardForm->label)->show();
    (wizardForm->lineEdit)->show();
    (wizardForm->detailButton)->show();

    (wizardForm->textField)->setEnabled(true);
    (wizardForm->textField)->setHtml(tr("<html><head>Specify a directory</head><body>Please Specify directory.</body></html>"));

    return;
}

/*!
 *  コピー先の確認を行う...
 *
 */
void screenData::setConfirmCopy(void)
{
    (wizardForm->btnBack)->setEnabled(true);
    (wizardForm->btnNext)->setEnabled(true);
    (wizardForm->btnAbort)->setEnabled(true);

    (wizardForm->label)->hide();
    (wizardForm->lineEdit)->hide();
    (wizardForm->detailButton)->hide();

    (wizardForm->textField)->setEnabled(true);

    logDirectory  = (wizardForm->lineEdit)->text();
    if (logDirectory.endsWith("/") == false)
    {
        logDirectory = logDirectory + "/";
    }
    int pos = logDirectory.indexOf(":");  // only windows...
    copyDirectory = logDirectory.left(pos) + ":/PRIVATE/DATA_FOLDER/PC_INOUT/";

    QString body = tr("<html><head>confirmation</head><body>");
    body = body + tr("Are you ready?");
    body = body + tr("<br><hr><UL><LI>");
    body = body + tr("bbstable.html : <B>");
    body = body + logDirectory;
    body = body + tr("</B></LI><LI>");
    body = body + tr("a2B.jad & a2B.jar : <B>");
    body = body + copyDirectory;
    body = body + tr("</B></LI><hr></UL></body></html>");

    (wizardForm->textField)->setHtml(body);

    return;
}

/*!
 *  終了画面
 *
 */
void screenData::setEndScreen(void)
{
     // コピーの実行...
    if (executeCopyFiles() == false)
    {
        currentScene--;
        return;
    }

    (wizardForm->btnBack)->hide();
    (wizardForm->btnNext)->hide();
    (wizardForm->btnAbort)->setEnabled(true);
    (wizardForm->btnAbort)->setText(tr("End Program"));

    (wizardForm->label)->hide();
    (wizardForm->lineEdit)->hide();
    (wizardForm->detailButton)->hide();

    (wizardForm->textField)->setEnabled(true);
    (wizardForm->textField)->setHtml(tr("<html><head>End copy</head><body>Copy End...</body></html>"));

    return;
}

/*!
 *  テキスト編集が終了したとき...
 *
 */
void screenData::editTextFinished(void)
{
    QString dir = (wizardForm->lineEdit)->text();
    if (dir.indexOf(":/PRIVATE/DATA_FOLDER") >= 0)
    {
        // 次にすすめる場合...
        (wizardForm->btnNext)->setEnabled(true);
    }
    else
    {
        // 次にすすめない場合...
        (wizardForm->btnNext)->setEnabled(false);
    }
    return;
}

/*!
 *  ファイルコピー処理の実行...
 *
 */
bool screenData::executeCopyFiles(void)
{
    // WX310用のディレクトリを認識する...
    int intPos = logDirectory.indexOf("DATA_FOLDER/");
    QString wx310Dir = "file:///SD:/" + logDirectory.mid(intPos + 12);

    // bbstable.html のコピー...
    QString bbsTable = logDirectory + "bbstable.html";
    if (QFile::exists(bbsTable) == true)
    {
        // bbstable.htmlが既にある場合。。。
        if (QMessageBox::question(0, tr("Exists bbstable.html"), tr("bbstable.html exists, overwrite it?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes)
        {
            // 上書きする場合...
            if (QFile::exists(bbsTable + ".keep") == true)
            {
                QFile::remove(bbsTable + ".keep");
            }
            QFile::rename(bbsTable, bbsTable + ".keep");
	    QFile::copy("bbstable.html", bbsTable);
        }
    }
    else
    {
        QFile::copy("bbstable.html", bbsTable);
    }

    // a2B.jarのコピー...
    QString jarFile = copyDirectory + "a2B.jar";
    if (QFile::exists(jarFile) == true)
    {
        QFile::remove(jarFile);
    }
    QFile::copy("a2B.jar", jarFile);

    // a2B.jadのコピー...
    QString jadFile = copyDirectory + "a2B.jad";
    if (QFile::exists(jadFile) == true)
    {
        QFile::remove(jadFile);
    }

    QFile inputFile("a2B.jad");
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }
    QFile outputFile(jadFile);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return (false);
    }

    QTextStream in (&inputFile);
    QTextStream out(&outputFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        int pos = line.indexOf("a2B-Directory:");
        if (pos < 0)
        {
            out << line << endl;
        }
    }
    inputFile.close();

    out << "a2B-Directory: " << wx310Dir << endl;
    out.flush();

    outputFile.close();
    return (true);
}

