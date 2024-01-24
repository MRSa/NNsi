/*****************************************************************************
  $Id: HttpComm.cpp,v 1.2 2005/04/24 13:20:16 mrsa Exp $
       HTTP�ʐM���s���N���X
 *****************************************************************************/

#include "qstring.h"
#include "qhttp.h"
#include "qprogressbar.h"
#include "qmessagebox.h"
#include "HttpComm.h"

namespace rootj {

/*!
 *   �R���X�g���N�^
 */
HttpComm::HttpComm()
 : QObject(), mUserAgent("qNNsi : 0.00"),
   mDataReceived(false), mDataReceivedFile(false),
   mpProgress(0), mHttpAccessor(), mBuffer(0), mOutputFile(),mTransactionId(0)
{
    // HTTP�ʐM�����̃V�O�i���ƒʐM�I�����ɍs����������(�X���b�g)��ڑ�����
    connect(&mHttpAccessor, SIGNAL(done(bool)),
            this,           SLOT(slot_doneHttpCommunication(bool)));
}

/*!
 *   �R���X�g���N�^ (���̂Q)
 */
HttpComm::HttpComm(QString &arUserAgent, QProgressBar *apProgress)
 : QObject(), mUserAgent(arUserAgent), mDataReceivedFile(false),
   mDataReceived(false), mpProgress(apProgress), 
   mHttpAccessor(), mBuffer(0), mOutputFile(),mTransactionId(0)
{
    // HTTP�ʐM�����̃V�O�i���ƒʐM�������ɍs����������(�X���b�g)��ڑ�����
    connect(&mHttpAccessor, SIGNAL(done(bool)),
            this,           SLOT(slot_doneHttpCommunication(bool)));

    // HTTP�ʐM�J�n�̃V�O�i���ƒʐM�J�n���ɍs����������(�X���b�g)��ڑ�����
    connect(&mHttpAccessor, SIGNAL(requestStarted(int)),
            this,           SLOT(slot_startedHttpCommunication(int)));

    // HTTP�ʐM�I���̃V�O�i���ƒʐM�I�����ɍs����������(�X���b�g)��ڑ�����
    connect(&mHttpAccessor, SIGNAL(requestFinished(int,bool)),
            this,           SLOT(slot_finishedHttpCommunication(int,bool)));

    // �v���O���X�o�[�̍X�V���s�����߂ɁA�V�O�i���ƃX���b�g��ڑ�����
    mpProgress->setEnabled(true);
    connect(&mHttpAccessor, SIGNAL(dataReadProgress(int,int)),
            mpProgress,     SLOT(setProgress(int,int)));    
}

/*!
 *  �f�X�g���N�^
 */
HttpComm::~HttpComm()
{
    // SIGNAL/SLOT�̐ڑ���؂����ق����ǂ���...
    if (mpProgress != 0)
    {
        mpProgress->setEnabled(false);
        disconnect(&mHttpAccessor, SIGNAL(dataReadProgress(int,int)),
                   mpProgress,     SLOT(setProgress(int,int)));    
    }

    // �t�@�C�����I�[�v�����Ă����Ƃ��ɂ̓N���[�Y������
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

// URL���I�[�v�����A�t�@�C���Ƀf�[�^���L�^����
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
        // �t�@�C���I�[�v���Ɏ��s����...(�_�C�A���O��\������)
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
 *     �w�肵��URL����f�[�^���擾����(�{����)
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
    
    // URL���z�X�g���Ǝ擾�p�X�A�ڑ�����port�ԍ��ɕ�������
    // (�����I�ɂ́Ahttp �� https �̋�ʂ��K�v��������Ȃ�)
    mTargetUrl = arUrl;
    divideHostAndPathFromURL(arUrl, hostName, path, portNumber);
    
#ifdef QNNSI_DEBUG_INFORMATION
    // �z�X�g�Ǝ擾�f�[�^�p�X���f�o�b�O�\������
    QString debugCaption = tr("Debug Info.");
    QString debugMessage = "Host name: " + hostName + "\n   path: " + path;
    QMessageBox::information(NULL,debugCaption,debugMessage,QMessageBox::Ok);
#endif // #ifdef QNNSI_DEBUG_INFORMATION
    
    // HTTP�̗v���w�b�_���쐬����
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
        // ���t�@�����ݒ肳��Ă���΁A���t�@����ݒ肷��
        httpHeader.setValue("Referer", arReferer);
    }
    if (aEncode == true)
    {
        // URL�G���R�[�h���꽃f�[�^�𑗂�ꍇ�ɂ́AContentType��ݒ肷
        httpHeader.setContentType("application/x-www-form-urlencoded");
    }
    
    // HTTP̗v���𑗐M����...
    mDataReceived     = false;
    mDataReceivedFile = false;
    mHttpAccessor.setHost(hostName, portNumber);
    if (arValue.isEmpty() == true)
    {
        // �f�[�^�𑗐M���Ȃ��ꍇ...
        mHttpAccessor.request(httpHeader, 0, apOutput);
    }
    else
    {
        // �f�[�^�𑗐M����ꍇ...
        mHttpAccessor.request(httpHeader, arValue, apOutput);
    }

    //mHttpAccessor.closeConnection();
    return (true);
}

/*!
 *  divideHostAndPathFromURL()
 *     URL����zX�g���ƃp�X�A�|�[�g�ԍ���؂�o������
 */
void HttpComm::divideHostAndPathFromURL(const QString &arUrl,
                                        QString       &arHost,
                                        QString       &arPath,
                                        int           &arPortNumber)
{
    // �z�X�g�������𒊏o����
    arHost = arUrl.section('/', 2, 2);

    // �|�[�g�ԍ����ݒ肳��Ă��邩�m�F����
    bool  isPortNumberSpecified = false;     
    QString port = arHost.section(':', 1,1);
    if (port.isEmpty() == false)
    {
        // �|�[�g�ԍ����ݒ肳��Ă���ꍇ�A�|�[�gԍ���z�Xg����؂蕪����
        arPortNumber = port.toInt(&isPortNumberSpecified);
        arHost = arHost.section('/', 0, 0); 
    }
    // �|�[�g�ԍ����ݒ肳��Ă��Ȃ��ꍇ�ɂ́A�f�t�H���g��http�|�[�g�ԍ���ݒ�
    if (isPortNumberSpecified != true)
    {
        arPortNumber = 80;
    }
    
    // �p�X�����𒊏o����
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
 *    ��M���ʂ̕\��
 *
 */
void HttpComm::checkRecieveStatus(QString &arCaption, bool aError)
{
    // ��M���ʂ�\������...
    QString debugMessage = mTargetUrl;
    if (aError == TRUE)
    {
        // ��M�G���[���������Ă����I
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
 *        (�V�O�i�� QHttp::done() ����M�����Ƃ��̏���)
 */
void HttpComm::slot_doneHttpCommunication(bool aError)
{
    // �ʐM�I���̃X���b�g���Ăяo���ďI��...
    slot_finishedHttpCommunication(0, aError);

    // HTTP�ʐM���I�������[�A�ƕ�
    emit signal_finishedHttpCommunication(mTransactionId);

    return;
}

// �f�[�^�ʐM�J�n���̏���
void HttpComm::slot_startedHttpCommunication(int aTransId)
{
    // ���ɉ������Ȃ�...
}

// �f�[�^��M�I�����̏���
void HttpComm::slot_finishedHttpCommunication(int aTransId, bool aError)
{
    checkRecieveStatus(tr("QHttp::requestFinished()"), aError);
    if (aError != true)
    {
        mDataReceived = true;
    }

    // �v���O���X�o�[����������
    if (mpProgress != 0)
    {
        mpProgress->reset();
        mpProgress->setEnabled(false);
    }

    // �t�@�C�����I�[�v�����Ă����Ƃ��ɂ̓N���[�Y������
    if (mOutputFile.isOpen() == true)
    {
        mOutputFile.close();
        mDataReceivedFile = true;
    }
    return;
}

// ��M�����f�[�^�u���b�N�̃|�C���^��Ԃ�
QIODevice *HttpComm::getReceivedData(void)
{
    // �܂���M���Ă��Ȃ��ꍇ�ɂ́A�ʂ�ۂ�����
    if (mDataReceived == false)
    {
        return (0);              
    }

    // �t�@�C���Ƀf�[�^����M�����ꍇ...
    if (mDataReceivedFile == true)
    {
        return (&mOutputFile);
    }

    // �f�[�^�u���b�N�Ƀf�[�^����M�����ꍇ
    return (mBuffer);
}

} // namespace rootj {
/** EOF **/
