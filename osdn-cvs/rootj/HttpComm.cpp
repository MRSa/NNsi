/*****************************************************************************
  $Id: HttpComm.cpp,v 1.2 2005/04/24 13:20:16 mrsa Exp $
       HTTP通信を行うクラス
 *****************************************************************************/

#include "qstring.h"
#include "qhttp.h"
#include "qprogressbar.h"
#include "qmessagebox.h"
#include "HttpComm.h"

namespace rootj {

/*!
 *   コンストラクタ
 */
HttpComm::HttpComm()
 : QObject(), mUserAgent("qNNsi : 0.00"),
   mDataReceived(false), mDataReceivedFile(false),
   mpProgress(0), mHttpAccessor(), mBuffer(0), mOutputFile(),mTransactionId(0)
{
    // HTTP通信完了のシグナルと通信終了時に行いたい処理(スロット)を接続する
    connect(&mHttpAccessor, SIGNAL(done(bool)),
            this,           SLOT(slot_doneHttpCommunication(bool)));
}

/*!
 *   コンストラクタ (その２)
 */
HttpComm::HttpComm(QString &arUserAgent, QProgressBar *apProgress)
 : QObject(), mUserAgent(arUserAgent), mDataReceivedFile(false),
   mDataReceived(false), mpProgress(apProgress), 
   mHttpAccessor(), mBuffer(0), mOutputFile(),mTransactionId(0)
{
    // HTTP通信完了のシグナルと通信完了時に行いたい処理(スロット)を接続する
    connect(&mHttpAccessor, SIGNAL(done(bool)),
            this,           SLOT(slot_doneHttpCommunication(bool)));

    // HTTP通信開始のシグナルと通信開始時に行いたい処理(スロット)を接続する
    connect(&mHttpAccessor, SIGNAL(requestStarted(int)),
            this,           SLOT(slot_startedHttpCommunication(int)));

    // HTTP通信終了のシグナルと通信終了時に行いたい処理(スロット)を接続する
    connect(&mHttpAccessor, SIGNAL(requestFinished(int,bool)),
            this,           SLOT(slot_finishedHttpCommunication(int,bool)));

    // プログレスバーの更新を行うために、シグナルとスロットをﾚ続する
    mpProgress->setEnabled(true);
    connect(&mHttpAccessor, SIGNAL(dataReadProgress(int,int)),
            mpProgress,     SLOT(setProgress(int,int)));    
}

/*!
 *  デストラクタ
 */
HttpComm::~HttpComm()
{
    // SIGNAL/SLOTの接続を切ったほうが良いか...
    if (mpProgress != 0)
    {
        mpProgress->setEnabled(false);
        disconnect(&mHttpAccessor, SIGNAL(dataReadProgress(int,int)),
                   mpProgress,     SLOT(setProgress(int,int)));    
    }

    // ファイルがオープンしていたときにはクローズさせる
    if (mOutputFile.isOpen() == true)
    {
        mOutputFile.close();
    }

    if (mBuffer != 0)
    {
        delete mBuffer;
        mBuffer = 0;
        mDataReceived = true;
    }
}

bool HttpComm::getDataFromURL(httpRequestMethod  aMethod,
                              int                aTransactionId,
                              QString           &arUrl,
                              QString           &arReferer, 
                              QByteArray        &arValue,
                              bool               aEncode,
                              int                aTimeout)
{
    mTransactionId = aTransactionId;
    if (mBuffer != 0)
    {
        delete mBuffer;
        mBuffer = 0;
        mDataReceived = true;
    }
    mBuffer = new QBuffer();
    if (mBuffer == 0)
    {
        return (false);
    }
    return (getDataFromURLlocal(mBuffer,
                                 aMethod,
                                 arUrl,
                                 arReferer,
                                 arValue,
                                 aEncode,
                                 aTimeout));
}

// URLをオープンし、ファイルにデータを記録する
bool HttpComm::getDataFromURLtoFile(const QString     &arFileName,
                                    httpRequestMethod  aMethod,
                                    int                aTransactionId,
                                    QString           &arUrl,
                                    QString           &arReferer, 
                                    QByteArray        &arValue,
                                    bool               aEncode,
                                    int                aTimeout)
{
    mTransactionId = aTransactionId;
    mOutputFile.setName(arFileName);
    if (mOutputFile.open(IO_WriteOnly) != true)
    {
        // ファイルオープンに失敗した...(ダイアログを表示する)
        QString debugMessage = tr("Can not open a File.") 
                               + "\n  " +  arFileName
                               +  "\n   (" + mOutputFile.errorString() + ")";
        QMessageBox::critical(NULL, tr("File Open Error"), debugMessage,
                              QMessageBox::Ok, QMessageBox::NoButton);
        return (false);
    }
    return (getDataFromURLlocal(&mOutputFile,
                                 aMethod,
                                 arUrl,
                                 arReferer,
                                 arValue,
                                 aEncode,
                                 aTimeout));
}

/*!
 * getDataFromURLlocal()
 *     指定したURLからデータを取得する(本処理)
 */
bool HttpComm::getDataFromURLlocal(QIODevice         *apOutput,
                                   httpRequestMethod  aMethod,
                                   QString           &arUrl,
                                   QString           &arReferer, 
                                   QByteArray        &arValue,
                                   bool               aEncode,
                                   int                /*aTimeout*/)
{
    int     portNumber;
    QString hostName, path;
    
    // URLをホスト名と取得パス、接続するport番号に分解する
    // (将来的には、http と https の区別が必要かもしれない)
    mTargetUrl = arUrl;
    divideHostAndPathFromURL(arUrl, hostName, path, portNumber);
    
#ifdef QNNSI_DEBUG_INFORMATION
    // ホストと取得データパスをデバッグ表示する
    QString debugCaption = tr("Debug Info.");
    QString debugMessage = "Host name: " + hostName + "\n   path: " + path;
    QMessageBox::information(NULL,debugCaption,debugMessage,QMessageBox::Ok);
#endif // #ifdef QNNSI_DEBUG_INFORMATION
    
    // HTTPの要求ヘッダを作成する
    QHttpRequestHeader httpHeader;
    QString httpMethod;
    switch (aMethod)
    {
      case HTTP_METHOD_POST:
        httpMethod = "POST";
        break;
    
      case HTTP_METHOD_HEAD:
        httpMethod = "HEAD";
        break;
    
      case HTTP_METHOD_GET:
      default:
        httpMethod = "GET";
        break;
    }
    httpHeader.setRequest(httpMethod, path);
    httpHeader.setValue("Host", hostName);
    httpHeader.setValue("User-Agent", mUserAgent); 
    //httpHeader.setValue("Connection", "close");
    httpHeader.setValue("Pragma", "no-cache");
    httpHeader.setValue("Accept", "text/html, */*");
    if (arReferer.isEmpty() != true)
    {
        // リファラが設定されていれば、リファラを設定する
        httpHeader.setValue("Referer", arReferer);
    }
    if (aEncode == true)
    {
        // URLエンコードされｽデータを送る場合には、ContentTypeを設定す
        httpHeader.setContentType("application/x-www-form-urlencoded");
    }
    
    // HTTPﾌ要求を送信する...
    mDataReceived     = false;
    mDataReceivedFile = false;
    mHttpAccessor.setHost(hostName, portNumber);
    if (arValue.isEmpty() == true)
    {
        // データを送信しない場合...
        mHttpAccessor.request(httpHeader, 0, apOutput);
    }
    else
    {
        // データを送信する場合...
        mHttpAccessor.request(httpHeader, arValue, apOutput);
    }

    //mHttpAccessor.closeConnection();
    return (true);
}

/*!
 *  divideHostAndPathFromURL()
 *     URLからホXト名とパス、ポート番号を切り出す処理
 */
void HttpComm::divideHostAndPathFromURL(const QString &arUrl,
                                        QString       &arHost,
                                        QString       &arPath,
                                        int           &arPortNumber)
{
    // ホスト名部分を抽出する
    arHost = arUrl.section('/', 2, 2);

    // ポート番号が設定されているか確認する
    bool  isPortNumberSpecified = false;     
    QString port = arHost.section(':', 1,1);
    if (port.isEmpty() == false)
    {
        // ポート番号が設定されていｽ場合、ポートﾔ号とzスg名を切り分ける
        arPortNumber = port.toInt(&isPortNumberSpecified);
        arHost = arHost.section('/', 0, 0); 
    }
    // ポート番号が設定されていない場合には、デフォルトのhttpポート番号を設定
    if (isPortNumberSpecified != true)
    {
        arPortNumber = 80;
    }
    
    // パス部分を抽出する
    int index = arUrl.find('/');
    index = index + 2;
    index = arUrl.find('/', index);
    arPath = arUrl.mid(index);
    if (arPath.isEmpty() == true)
    {
        arPath = "/";
    }
    return;
}

/*!
 *    受信結果の表示
 *
 */
void HttpComm::checkRecieveStatus(QString &arCaption, bool aError)
{
    // 受信結果を表示する...
    QString debugMessage = mTargetUrl;
    if (aError == TRUE)
    {
        // 受信エラーが発生していた！
        debugMessage = tr("Receive Error") + 
                       "\n url :" + debugMessage +
                       "\n  (" + mHttpAccessor.errorString() + ")";
        QMessageBox::critical(NULL, arCaption, debugMessage,
                              QMessageBox::Ok, QMessageBox::NoButton);
    }
    else    
    {
#ifdef QNNSI_DEBUG_INFORMATION
        debugMessage = tr("Received data successfully.") + "\n" + debugMessage;
        QMessageBox::information(NULL, arCaption, debugMessage,
                                 QMessageBox::Ok);
#endif // #ifdef QNNSI_DEBUG_INFORMATION
    }
    return;
}

/*!
 *   slot_doneHttpCommunication()
 *        (シグナル QHttp::done() を受信したときの処理)
 */
void HttpComm::slot_doneHttpCommunication(bool aError)
{
    // 通信終了のスロットを呼び出して終了...
    slot_finishedHttpCommunication(0, aError);

    // HTTP通信が終わったよー、と報告
    emit signal_finishedHttpCommunication(mTransactionId);

    return;
}

// データ通信開始時の処理
void HttpComm::slot_startedHttpCommunication(int aTransId)
{
    // 特に何もしない...
}

// データ受信終了時の処理
void HttpComm::slot_finishedHttpCommunication(int aTransId, bool aError)
{
    checkRecieveStatus(tr("QHttp::requestFinished()"), aError);
    if (aError != true)
    {
        mDataReceived = true;
    }

    // プログレスバーを消去する
    if (mpProgress != 0)
    {
        mpProgress->reset();
        mpProgress->setEnabled(false);
    }

    // ファイルがオープンしていたときにはクローズさせる
    if (mOutputFile.isOpen() == true)
    {
        mOutputFile.close();
        mDataReceivedFile = true;
    }
    return;
}

// 受信したデータブロックのポインタを返す
QIODevice *HttpComm::getReceivedData(void)
{
    // まだ受信していない場合には、ぬるぽを応答
    if (mDataReceived == false)
    {
        return (0);              
    }

    // ファイルにデータを受信した場合...
    if (mDataReceivedFile == true)
    {
        return (&mOutputFile);
    }

    // データブロックにデータを受信した場合
    return (mBuffer);
}

} // namespace rootj {
/** EOF **/
