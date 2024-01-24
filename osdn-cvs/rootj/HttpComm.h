/*!
 *   $Id: HttpComm.h,v 1.2 2005/04/24 13:20:16 mrsa Exp $
 *        �f�[�^��M�N���X...
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

// HTTP�ʐM�̃��\�b�h�w��...
enum httpRequestMethod
{
    HTTP_METHOD_GET,    /* GET���\�b�h  */
    HTTP_METHOD_POST,   /* POST���\�b�h */
    HTTP_METHOD_HEAD,   /* HEAD���\�b�h */
    HTTP_BOTTOM         /* (�_�~�[)     */
};

class HttpComm : public QObject
{
    Q_OBJECT
    
public:
    // �R���X�g���N�^�ƃf�X�g���N�^
    HttpComm();
    HttpComm(QString &arUserAgent, QProgressBar *apProgress = 0);
    ~HttpComm();

public:
    // �f�[�^��M�ς݂��ǂ����H
    bool isTheDataReceived() { return (mDataReceived); }

    // �w�肵��URL����f�[�^���擾���ăN���X���ɕ������ޏ���
    bool getDataFromURL(httpRequestMethod  aMethod,
                        int                aTransactionId,
                        QString           &arUrl,
                        QString           &arReferer,
                        QByteArray        &arValue,
                        bool               aEncode  = false,
                        int                aTimeout = 300);

    // ��M�����f�[�^�u���b�N�̃|�C���^��Ԃ�(�Ȃ���� �ʂ��)
    QIODevice *HttpComm::getReceivedData(void);

    // �w�肵��URL����f�[�^���擾���ăt�@�C���ɋL�^���鏈��
    bool getDataFromURLtoFile(const QString     &arFileName,
                              httpRequestMethod  aMethod,
                              int                aTransactionId,
                              QString           &arUrl,
                              QString           &arReferer,
                              QByteArray        &arValue,
                              bool               aEncode  = false,
                              int                aTimeout = 300);

public slots:
    // �f�[�^��M�������̏���
    void slot_doneHttpCommunication(bool aError);

    // �f�[�^�ʐM�J�n���̏���
    void slot_startedHttpCommunication(int aTransId);

    // �f�[�^��M�I�����̏���
    void slot_finishedHttpCommunication(int aTransId, bool aError);

signals:
    // �f�[�^��M�I����... (signal���s)
    void signal_finishedHttpCommunication(int aTransactionId);

private:
    // URL����z�X�g���ƃp�X�𕪊����鏈��
    void divideHostAndPathFromURL(const QString &arUrl, 
                                  QString       &arHost,
                                  QString       &arPath,
                                  int           &arPortNumber);

    // ��M���ʂ̃_�C�A���O�\��
    void checkRecieveStatus(QString &arCaption, bool aError);

    // �f�[�^����M���鏈��...
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
