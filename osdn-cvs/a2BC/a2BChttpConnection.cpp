/*!
 *   HTTP�ʐM��..
 *
 */
#include <QtGlobal>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include "a2BChttpConnection.h"
#include "screenControl.h"


// �ʐM���ʂ̉����R�[�h
const int  DATA_UPDATED         =  1;
const int  DATA_NOTMODIFIED     =  0;
const int  DATA_GETERROR        = -1;
const int  DATA_GETDATAWRONG    = -2;

/*!
 *  �R���X�g���N�^
 *
 */
a2BChttpConnection::a2BChttpConnection(QHttp *apHttp, QWidget *parent)
  : QWidget(parent), mpHttp(apHttp), mpFile(0), mHttpGetId(0), mFileName(""), 
    mFirstGet(false), mGetResult(DATA_NOTMODIFIED)
{
    // �Ƃ肠����...
}

/*!
 *  �f�X�g���N�^
 *
 */
a2BChttpConnection::~a2BChttpConnection()
{
    // �f�[�^�̏���
    cleanup();
}

/*!
 *  �N���X����������
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
 *    URL����t�@�C�����擾����
 *
 */
void a2BChttpConnection::getFileFromUrl(QString &arFileName, QString &arUrl, bool isPartGet)
{
    if (mHttpGetId != 0)
    {
        // ���ݒʐM��...�������Ȃ�
        QMessageBox::information(this, tr("communicating"), tr("now communicating, aborted."));
        return;
    }
    cleanup();
    
    QUrl url(arUrl);
    QString fileName = arFileName;
    qint64 fileSize = 0;
    if (QFile::exists(fileName) == true)
    {
        QFileInfo fileInfo(fileName);
        fileSize = fileInfo.size();
        if (isPartGet == true)
        {
            // �����擾�̏ꍇ�A�t�@�C���T�C�Y���擾����
            fileSize = fileSize - 1;
            if (fileSize < 0)
            {
                fileSize = 0;
            }
        }
        else
        {
            // �S���擾�Ńt�@�C��������Ƃ��A�Â��t�@�C�������l�[������
            if (fileSize > 0)
            {
                QFile::remove(fileName + ".bak");
                QFile::rename(fileName, fileName + ".bak");
            }
            QFile::remove(fileName);
        }
    }
    mFileName = fileName;

    // �v���w�b�_�𐶐�����
    QHttpRequestHeader header("GET", url.path(), 1, 0);
    header.setValue("Host", url.host());
    header.setValue("User-Agent", "Monazilla/1.00  (a2BC/0.00)");
    header.setValue("Accept", "text/html, image/jpeg, */*");
    header.setValue("Accept-Encoding", "identity");
    header.setValue("Content-Language", " ja, en");
    header.setValue("Pragma", "no-cache");
    header.setValue("Connection", "close");

    // �����擾�̏ꍇ�A�w�b�_��Range���i�[����
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
 *   �f�[�^�̓ǂݍ���
 * 
 */
void a2BChttpConnection::readyRead(const QHttpResponseHeader &resp)
{
    // ��M�N���X�����������A�A�I������
    if (mpHttp == 0)
    {
        return;
    }
    
    // �����R�[�h�̊m�F (���҂��Ă���̂́A 200 �܂��� 206)
    int status = resp.statusCode();
    if ((status != 200)&&(status != 206))
    {
        // ���҂��������R�[�h�ł͂Ȃ�...�Ȃɂ������I������
        mGetResult = DATA_GETERROR;

        QString message;
        message.setNum(status);
        message = "  " + tr("ERROR : ") + resp.reasonPhrase() + " [" + message + "]  " + tr("file : ") + mFileName;
        emit updateMessage(message);
        return;
    }

    // �f�[�^��ǂݏo��
    QByteArray body = mpHttp->readAll();
    if ((mFirstGet == true)&&(mIsPartGet == true))
    {
         // �����擾�ŁA�����f�[�^����M������...
         if ((body.size() == 1)&&(body.data()[0] == 10))
         {
            // �X�V�Ȃ��A�I������B
            mGetResult = DATA_NOTMODIFIED;
            return;
         }

         // �����擾�ŁA�f�[�^�̐擪����؂蕶�����m�F
         if (body.data()[0] != 10)
         {
            // �G���[�i���ځ[�񌟏o�H�H�j
            mGetResult = DATA_GETDATAWRONG;
            return;
         }
         
         // �����擾��M�`�F�b�NOK�A1�o�C�g�f�[�^�����
         body.remove(0, 1);
    }
    mGetResult = DATA_UPDATED;
    mFirstGet = false;

    // �����o���t�@�C�����I�[�v������
    QFile outputFile(mFileName);
    QIODevice::OpenMode openMode = QIODevice::WriteOnly | QIODevice::Append;
    if (outputFile.open(openMode) == false)
    {
        // �G���[�_�C�A���O��\������
        QMessageBox::information(this, tr("file open failure"),
                           tr("Unable to save the file %1: %2.")
                           .arg(mFileName).arg(outputFile.errorString()));

        // �Â��t�@�C�����������ꍇ�A���̃t�@�C���ɖ߂�
        if (QFile::exists(mFileName + ".bak") == true)
        {
            QFile::remove(mFileName);            
            QFile::rename(mFileName + ".bak", mFileName);
        }
        return;
    }

    // �t�@�C���������o��
    outputFile.write(body);

    // �t�@�C�����N���[�Y����
    outputFile.close();

    return;
} 

/*!
 *   HTTP�ʐM�̏I��...
 *
 */
void a2BChttpConnection::done(bool error)
{
    QString message;
    
    // �ʐM���ʂ̊m�F
    if (error == true)
    {
        // �ʐM�G���[����...
        message = "  " + mpHttp->errorString();
/*
        // �Â��t�@�C�����������ꍇ�A���̃t�@�C���ɖ߂�
        if (QFile::exists(mFileName + ".bak") == true)
        {
            QFile::remove(mFileName);
            QFile::rename(mFileName + ".bak", mFileName);
        }
*/
    }
    else if (mGetResult == DATA_GETERROR)
    {
        //  HTTP Response NG�A�A�t�@�C�������l�[������
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
        // ���ځ[�񌟏o�H ���O�����l�[������
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
        // ���쐬�� �i�X�V�Ȃ��j
        message = "  " + tr("not modified");
    }
    else
    {
        // ���쐬��
        // �Â��t�@�C�����������ꍇ�A�t�@�C�����폜����
        if (QFile::exists(mFileName + ".bak") == true)
        {
            QFile::remove(mFileName + ".bak");
        }
        message = "  ";
    }

    // �ʐM�I��...
    mHttpGetId = 0;
    mFileName  = "";
    mFirstGet = false;

    // ��ʂɃt�@�C���擾�̏I����񍐂���
    emit completedCommunication(mGetResult, message);

    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
