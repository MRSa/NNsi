/**
 *   HTTP通信部..
 *
 */
#include <QtGlobal>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include "a2BChttpConnection.h"


// 通信結果の応答コード
const int  DATA_UPDATED         =  1;
const int  DATA_NOTMODIFIED     =  0;
const int  DATA_GETERROR        = -1;
const int  DATA_GETDATAWRONG    = -2;

/**
 *  コンストラクタ
 *
 */
a2BChttpConnection::a2BChttpConnection(QHttp *apHttp, QWidget *parent)
  : QWidget(parent), mpHttp(apHttp), mpFile(0), mHttpGetId(0), mFileName(""), 
    mFirstGet(false), mGetResult(DATA_NOTMODIFIED)
{
    // HTTP通信クラスとシグナル/スロットを接続する
    connect(apHttp, SIGNAL(done(bool)),
            this,   SLOT(done(bool)));
    connect(apHttp, SIGNAL(readyRead(const QHttpResponseHeader &)),
            this,   SLOT(readyRead(const QHttpResponseHeader &)));

    mCookie    = "";
    mUserAgent = "";

}

/**
 *  デストラクタ
 *
 */
a2BChttpConnection::~a2BChttpConnection()
{
    // データの消去
    cleanup();
}

/**
 *  クラスを消去する
 *
 */
void a2BChttpConnection::cleanup()
{
    if (mpFile != 0)
    {
        delete mpFile;
        mpFile = 0;
    }
    mHttpGetId = 0;
    mFileName  = "";
    mFirstGet  = false;
    mIsPartGet = false;
    
    mGetResult = DATA_NOTMODIFIED;
}




/*!
 *    URLからファイルを取得する
 *
 */
void a2BChttpConnection::getFileFromUrl(QString &arFileName, QString &arUrl, bool isPartGet)
{
    if (mHttpGetId != 0)
    {
        // 現在通信中...何もしない
        QMessageBox::information(this, tr("communicating"), tr("now communicating, aborted."));
        return;
    }
    cleanup();
    
    QUrl url(arUrl, QUrl::TolerantMode);
    QString fileName = arFileName;
    qint64 fileSize = 0;
    if (QFile::exists(fileName) == true)
    {
        QFileInfo fileInfo(fileName);
        fileSize = fileInfo.size();
        if (isPartGet == true)
        {
            // 差分取得の場合、ファイルサイズを取得する
            fileSize = fileSize - 1;
            if (fileSize < 0)
            {
                fileSize = 0;
            }
        }
        else
        {
            // 全部取得でファイルがあるとき、古いファイルをリネームする
            if (fileSize > 0)
            {
                QFile::remove(fileName + ".bak");
                QFile::rename(fileName, fileName + ".bak");
            }
            QFile::remove(fileName);
        }
    }
    mFileName = fileName;

    // 要求ヘッダを生成する
    QHttpRequestHeader header("GET", url.path() + "?" + url.encodedQuery(), 1, 0);
    header.setValue("Host", url.host());
    if (mUserAgent.isEmpty() == true)
    {
        header.setValue("User-Agent", "Mozilla/1.00  (qTVg/0.00)");
    }
    else
    {
        header.setValue("User-Agent", mUserAgent);
    }
    if (mCookie.isEmpty() != true)
    {
        header.setValue("Cookie", mCookie);
    }
    header.setValue("Accept", "text/html, image/jpeg, */*");
    header.setValue("Accept-Encoding", "identity");
    header.setValue("Content-Language", " ja, en");
    header.setValue("Pragma", "no-cache");
    header.setValue("Connection", "close");

    // 差分取得の場合、ヘッダにRangeを格納する
    if ((isPartGet == true)&&(fileSize != 0))
    {
        QString value;
        value.setNum(fileSize);
        value = "bytes=" + value + "-"; 
        header.setValue("Range", value);
    }
    mIsPartGet = isPartGet;
    
    int portNum = (url.port() != -1) ? url.port() : 80;
    mFirstGet = true;
    mpHttp->setHost(url.host(), portNum);

    QByteArray  sendBuf("a2BC");
    mHttpGetId = mpHttp->request(header);
//  mHttpGetId = mpHttp->request(header, sendBuf);
//  mHttpGetId = mpHttp->get(url.path(), mpFile);

    QString message = "  " + tr("host: ") + url.host() + "  " + tr("file : ") + url.path();
    emit updateMessage(message);

    return;
}

/*!
 *  Cookieを設定する
 *
 */
void a2BChttpConnection::setCookie(QString &arCookie)
{
    mCookie = arCookie;
}

/*!
 *  ユーザーエージェントを設定する
 *
 */
void a2BChttpConnection::setUserAgent(QString &arUserAgent)
{
    mUserAgent = arUserAgent;
}

/*
 *  通信中かどうか？
 *
 */
bool a2BChttpConnection::isCommunicating()
{
    if (mHttpGetId == 0)
    {
        return (false);
    }
    return (true);
}

/*!
 *   データの読み込み
 * 
 */
void a2BChttpConnection::readyRead(const QHttpResponseHeader &resp)
{
    // 受信クラスがおかしい、、終了する
    if (mpHttp == 0)
    {
        return;
    }
    
    // 応答コードの確認 (期待しているのは、 200 または 206)
    int status = resp.statusCode();
    if ((status != 200)&&(status != 206))
    {
        // 期待した応答コードではない...なにもせず終了する
        mGetResult = DATA_GETERROR;

        QString message;
        message.setNum(status);
        message = "  " + tr("ERROR : ") + resp.reasonPhrase() + " [" + message + "]  " + tr("file : ") + mFileName;
        emit updateMessage(message);
        return;
    }

    // データを読み出す
    QByteArray body = mpHttp->readAll();
    if ((mFirstGet == true)&&(mIsPartGet == true))
    {
         // 差分取得で、初期データを受信した時...
         if ((body.size() == 1)&&(body.data()[0] == 10))
         {
            // 更新なし、終了する。
            mGetResult = DATA_NOTMODIFIED;
            return;
         }

         // 差分取得で、データの先頭が区切り文字か確認
         if (body.data()[0] != 10)
         {
            // エラー（あぼーん検出？？）
            mGetResult = DATA_GETDATAWRONG;
            return;
         }
         
         // 差分取得受信チェックOK、1バイトデータを削る
         body.remove(0, 1);
    }
    mGetResult = DATA_UPDATED;
    mFirstGet = false;

    // 書き出すファイルをオープンする
    QFile outputFile(mFileName);
    QIODevice::OpenMode openMode = QIODevice::WriteOnly | QIODevice::Append;
    if (outputFile.open(openMode) == false)
    {
        // エラーダイアログを表示する
        QMessageBox::information(this, tr("file open failure"),
                           tr("Unable to save the file %1: %2.")
                           .arg(mFileName).arg(outputFile.errorString()));

        // 古いファイルがあった場合、元のファイルに戻す
        if (QFile::exists(mFileName + ".bak") == true)
        {
            QFile::remove(mFileName);            
            QFile::rename(mFileName + ".bak", mFileName);
        }
        return;
    }

    // ファイルを書き出す
    outputFile.write(body);

    // ファイルをクローズする
    outputFile.close();

    return;
} 

/*!
 *   HTTP通信の終了...
 *
 */
void a2BChttpConnection::done(bool error)
{
    QString message;
    
    // 通信結果の確認
    if (error == true)
    {
        // 通信エラー発生...
        message = "  " + mpHttp->errorString();
/*
        // 古いファイルがあった場合、元のファイルに戻す
        if (QFile::exists(mFileName + ".bak") == true)
        {
            QFile::remove(mFileName);
            QFile::rename(mFileName + ".bak", mFileName);
        }
*/
    }
    else if (mGetResult == DATA_GETERROR)
    {
        //  HTTP Response NG、、ファイルをリネームする
        if (QFile::exists(mFileName) == true)
        {
            if (QFile::exists(mFileName + ".bak.dat") == true)
            {
                QFile::remove(mFileName + ".bak.dat");
            }
            QFile::rename(mFileName, mFileName + ".bak.dat");
//          QFile::copy(mFileName, mFileName + ".bak.dat");
        }
        message = "  " + tr("response NG");
    }
    else if (mGetResult == DATA_GETDATAWRONG)
    {
        // あぼーん検出？ ログをリネームする
        if (QFile::exists(mFileName) == true)
        {
            if (QFile::exists(mFileName + "._bak.dat") == true)
            {
                QFile::remove(mFileName + "._bak.dat");
            }
            QFile::rename(mFileName, mFileName + "._bak.dat");
//          QFile::copy(mFileName, mFileName + "._bak.dat");
        }
        message = "  " + tr("data error");
    }
    else if (mGetResult == DATA_NOTMODIFIED)
    {
        // 動作成功 （更新なし）
        message = "  " + tr("not modified");
    }
    else
    {
        // 動作成功
        // 古いファイルがあった場合、ファイルを削除する
        if (QFile::exists(mFileName + ".bak") == true)
        {
            QFile::remove(mFileName + ".bak");
        }
        message = "  ";
    }

    // 通信終了...
    mHttpGetId = 0;
    mFileName  = "";
    mFirstGet = false;

    // 上位にファイル取得の終了を報告する
    emit completedCommunication(mGetResult, message);

    return;
}
/*
 *  qTVg
 *  Copyright (C) 2007- NNsi Project
 *  (see qTVg-src.txt for detail.)
 */
