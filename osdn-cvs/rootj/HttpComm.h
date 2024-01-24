/*!
 *   $Id: HttpComm.h,v 1.2 2005/04/24 13:20:16 mrsa Exp $
 *        データ受信クラス...
 */
#ifndef ROOTJ_HTTP_COMM_H
#define ROOTJ_HTTP_COMM_H

#include "qobject.h"
#include "qhttp.h"
#include "qfile.h"
#include "qbuffer.h"

class QString;
class QProgressBar;
namespace rootj {

// HTTP通信のメソッド指定...
enum httpRequestMethod
{
    HTTP_METHOD_GET,    /* GETメソッド  */
    HTTP_METHOD_POST,   /* POSTメソッド */
    HTTP_METHOD_HEAD,   /* HEADメソッド */
    HTTP_BOTTOM         /* (ダミー)     */
};

class HttpComm : public QObject
{
    Q_OBJECT
    
public:
    // コンストラクタとデストラクタ
    HttpComm();
    HttpComm(QString &arUserAgent, QProgressBar *apProgress = 0);
    ~HttpComm();

public:
    // データ受信済みかどうか？
    bool isTheDataReceived() { return (mDataReceived); }

    // 指定したURLからデータを取得してクラス内に抱え込む処理
    bool getDataFromURL(httpRequestMethod  aMethod,
                        int                aTransactionId,
                        QString           &arUrl,
                        QString           &arReferer,
                        QByteArray        &arValue,
                        bool               aEncode  = false,
                        int                aTimeout = 300);

    // 受信したデータブロックのポインタを返す(なければ ぬるぽ)
    QIODevice *HttpComm::getReceivedData(void);

    // 指定したURLからデータを取得してファイルに記録する処理
    bool getDataFromURLtoFile(const QString     &arFileName,
                              httpRequestMethod  aMethod,
                              int                aTransactionId,
                              QString           &arUrl,
                              QString           &arReferer,
                              QByteArray        &arValue,
                              bool               aEncode  = false,
                              int                aTimeout = 300);

public slots:
    // データ受信完了時の処理
    void slot_doneHttpCommunication(bool aError);

    // データ通信開始時の処理
    void slot_startedHttpCommunication(int aTransId);

    // データ受信終了時の処理
    void slot_finishedHttpCommunication(int aTransId, bool aError);

signals:
    // データ受信終了報告... (signal発行)
    void signal_finishedHttpCommunication(int aTransactionId);

private:
    // URLからホスト名とパスを分割する処理
    void divideHostAndPathFromURL(const QString &arUrl, 
                                  QString       &arHost,
                                  QString       &arPath,
                                  int           &arPortNumber);

    // 受信結果のダイアログ表示
    void checkRecieveStatus(QString &arCaption, bool aError);

    // データを受信する処理...
    bool getDataFromURLlocal(QIODevice         *apOutput,
                             httpRequestMethod  aMethod,
                             QString           &arUrl,
                             QString           &arReferer, 
                             QByteArray        &arValue,
                             bool               aEncode,
                             int                aTimeout);
    
private:
    QString           mUserAgent;
    QString           mTargetUrl;
    bool              mDataReceived;
    bool              mDataReceivedFile;
    QProgressBar     *mpProgress;
    QHttp             mHttpAccessor;
    QBuffer          *mBuffer;
    QFile             mOutputFile;
    int               mTransactionId;
};

}  // namespace rootj {
#endif // #ifndef ROOTJ_HTTP_COMM_H
