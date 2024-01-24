/**
 *  ���O�����N���X(2�����˂�u���E�U�Ԃ�dat�t�@�C�����R�s�[����)
 *   
 * 
 */
#include <QSet>
#include <QFile>
#include <QMessageBox>
#include "syncLog.h"
#include "screenData.h"
#include "a2BcConstants.h"

/**
 *  �R���X�g���N�^
 * 
 */
syncLog::syncLog(QWidget *parent)
 : QWidget(parent), mpBrowserParser(0), mpA2BbrowserParser(0), mIsReady(false)
{
    // �����ł͉������Ȃ�..
}

/*!
 *   �f�X�g���N�^
 * 
 */
syncLog::~syncLog()
{
    mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
    mOtherBrowserFactory.deleteOtherBrowserFactory(mpBrowserParser);
}


/*!
 *  ���s�̏���
 * 
 */
bool syncLog::prepare(screenData &arDB)
{
    QString message;

    // �ꗗ�f�[�^�̓ǂݍ��ݏ���
    QString fileName = arDB.getBbsTableFileName();
    bool readyBbs = mBbsTableParser.prepare(fileName);
    if (readyBbs == false)
    {
        // �ꗗ�̉�͎��s...�I������
        return (false);
    }
    message = tr("read :") + " " + fileName;
    emit updateMessage(message);

    // a2B�p�̃��O��̓N���X���擾�A���������s����
    mpA2BbrowserParser = mOtherBrowserFactory.createOtherBrowserFactory(OTHERBROWSER_A2B);
    if (mpA2BbrowserParser == 0)
    {
        return (false);
    }
    QString baseDirectory = fileName;
    baseDirectory.replace(QRegExp("bbstable.html$"), "");
    mpA2BbrowserParser->setPath(baseDirectory);
    mpA2BbrowserParser->setBbsTable(&mBbsTableParser, fileName);
    if (mpA2BbrowserParser->prepare() == false)
    {
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
        return (false);
    }
    
    // ��͌���(dat�t�@�C����)��\������
    message.setNum(mpA2BbrowserParser->datFileCount());
    message = tr("parsed ") + mpA2BbrowserParser->getBrowserName() + " : " + message;
    emit updateMessage(message);

    // �����p�̃��O��̓N���X���擾�A���������s
    mpBrowserParser = mOtherBrowserFactory.createOtherBrowserFactory(arDB.getOtherBrowserType());
    if (mpBrowserParser == 0)
    {
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
        return (false);
    }
    QString logPath = arDB.getOtherBrowserDirectory();
    mpBrowserParser->setPath(logPath);
    mpBrowserParser->setBbsTable(&mBbsTableParser, fileName);
    if (mpBrowserParser->prepare() == false)
    {
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpA2BbrowserParser);
        mOtherBrowserFactory.deleteOtherBrowserFactory(mpBrowserParser);
        return (false);
    }

    // ��͌���(dat�t�@�C����)��\������
    message.setNum(mpBrowserParser->datFileCount());
    message = tr("parsed ") + mpBrowserParser->getBrowserName() + " : " + message;
    emit updateMessage(message);

    mIsReady = true;
    return (mIsReady);
}

/*!
 *   ������(a2B)�̃u���E�U�����擾����
 * 
 */
QString syncLog::getSourceBrowserName()
{
    if (mIsReady == false)
    {
        return ("(not ready)");
    }
    QString message;
    message = mpA2BbrowserParser->getBrowserName();
    return (message);
}

/*!
 *   ��������̃u���E�U�����擾����
 * 
 */
QString syncLog::getDestinationBrowserName()
{
    if (mIsReady == false)
    {
        return ("(not ready)");
    }
    QString message;
    message = mpBrowserParser->getBrowserName();
    return (message);
}

/*!
 *   a2B�ŊǗ�����Ă��Ȃ�dat�t�@�C���𑼃u���E�U�Ɉړ�������
 * 
 */
void syncLog::moveToUnmanagedLogFile(int aSyncType)
{
    bool force = false;
    if (aSyncType == SYNC_FROM_a2B)
    {
        // a2B����㏑������ꍇ...
        force = true;
    }

    // a2B�ŊǗ�����Ă��Ȃ�dat�t�@�C���̐������߂�
    int count = mpA2BbrowserParser->checkUnmanagedDatFile();
    for (int loop = 0; loop < count; loop++)
    {
        QString nickName = mpA2BbrowserParser->getUnmanagedDatNickName(loop);
        QString fileName = mpA2BbrowserParser->getUnmanagedDatFileName(loop);
        QString srcName  = mpA2BbrowserParser->getFullPathDatFileName(nickName, fileName);
        QString dstName  = mpBrowserParser->getReceiveDatFileName(nickName, fileName);
        QString message = "\n  " + srcName + " ==> " + dstName + "\n";
        emit updateMessage(message);
        
        // �R�s�[��t�@�C�������݂��Ă����Ƃ�...
        if (QFile::exists(dstName) == true)
        {
            // overwrite���邩�ǂ����̊m�F���s��...
            int ans = QMessageBox::No;
            if (force != true)
            {
                message = nickName + "/" + dstName + " " + tr("is already exists, overwrite it?");
                ans = QMessageBox::question(0, tr("confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::YesAll);
                if (ans == QMessageBox::YesAll)
                {
                    force = true;
                }
            }
            if ((force == true)||(ans == QMessageBox::Yes))
            {
                // �㏑���m�F��Yes��I�񂾏ꍇ�A���邢�́AYesAll��I�񂾏ꍇ�ɂ́A�t�@�C���������ăR�s�[����
                QFile::remove(dstName);
            }
        }
        // �t�@�C�����ړ�������
        mpBrowserParser->receiveDatFile(nickName, fileName, dstName, srcName, mpA2BbrowserParser->isSameLogFormat(), 1, 0);
        message = tr("Move log") + " " + srcName + " ==> " + dstName + "  (" + nickName + ")";
        emit updateMessage(message);
    }
    return;
}

/*!
 *   �����̎��s
 * 
 */
bool syncLog::doSync(int aSyncType, Qt::CheckState aBackupErrorLog, Qt::CheckState aCopySubjectTxt)
{
    bool ret = false;
    
    // a2B�̃G���[���O�𑼃u���E�U�ֈړ������邩�ǂ���...
    if (aBackupErrorLog == Qt::Checked)
    {
        // a2B�ŊǗ�����Ă��Ȃ�dat�t�@�C���𑼃u���E�U�Ɉړ�������ꍇ...
        moveToUnmanagedLogFile(aSyncType);        
    }

    QString message;
    switch (aSyncType)
    {
      case SYNC_TO_a2B:
        // ���u���E�U �� a2B
        message = tr("Sync ") + getDestinationBrowserName() + " ==> " + getSourceBrowserName();
        emit updateMessage(message);
        ret = logCopyMain(mpA2BbrowserParser, mpBrowserParser, aBackupErrorLog, aCopySubjectTxt);
        break;

      case SYNC_FROM_a2B:
        // a2B �� ���u���E�U
        message = tr("Sync ") + getSourceBrowserName() + " ==> " + getDestinationBrowserName();
        emit updateMessage(message);
        ret = logCopyMain(mpBrowserParser, mpA2BbrowserParser, aBackupErrorLog, aCopySubjectTxt);
        break;

      case SYNC_ALL:
      default:
        // ���O�𓯊�������ꍇ...
        message = tr("Sync ") + getSourceBrowserName() + " <==> " + getDestinationBrowserName();
        emit updateMessage(message);
        ret = synchronizeMain(mpBrowserParser, mpA2BbrowserParser, aBackupErrorLog, aCopySubjectTxt);
        break;
    }

    // ���Ƃ��܂�
    mpA2BbrowserParser->finalize();
    mpBrowserParser->finalize();

    message = tr("synchronize end...");
    emit completedSynchronize(message);
    return (ret);
}

/*!
 *   ���O�����̎��s...
 * 
 */
bool syncLog::synchronizeMain(a2BC_AbstractOtherBrowserParser *apDst, a2BC_AbstractOtherBrowserParser *apSrc, Qt::CheckState aBackupErrorLog, Qt::CheckState aCopySubjectTxt)
{
    bool ret = false;

    return (ret);
}

/*!
 *   ���O�R�s�[�̎��s...
 * 
 */
bool syncLog::logCopyMain(a2BC_AbstractOtherBrowserParser *apDst, a2BC_AbstractOtherBrowserParser *apSrc, Qt::CheckState aRemoveErrorLog, Qt::CheckState aCopySubjectTxt)
{
    QSet<QString>  nickSet;
    bool force = false;
    bool ret = false;

    // �ϐ����N���A
    nickSet.clear();

    // �v���O���X�o�[�̍ő�l�ݒ�
    emit setMaximum(apSrc->datFileCount() - 2);

    // apSrc �� apDst�̃��O�㏑���R�s�[...
    int count = apSrc->datFileCount();
    for (int loop = 0; loop < count; loop++)
    {
        QString srcNick    = apSrc->getBoardNick(loop);
        QString srcFile    = apSrc->getDatFileName(loop);
        int    readPoint  = apSrc->getReadPoint(loop);
        qint64  lastUpdate = apSrc->getLastUpdate (loop);
        QString srcName    = apSrc->getFullPathDatFileName(srcNick, srcFile);
        QString dstName    = apDst->getReceiveDatFileName(srcNick, srcFile);

        if ((aCopySubjectTxt == Qt::Checked)&&(nickSet.contains(srcNick) != true))
        {
            // �j�b�N�l�[�����L�^����Ă��Ȃ�����...
            nickSet.insert(srcNick);
        }

        // �R�s�[��t�@�C�����𐶐�����...
        int ans = QMessageBox::Yes;
        if (QFile::exists(dstName) == true)
        {
            // overwrite���邩�ǂ����̊m�F���s��...
            if (force != true)
            {
                QFile srcFileInfo(srcName);
                QFile dstFileInfo(dstName);
                qint64 srcSize, dstSize;
                QString srcNum, dstNum;
                srcSize = srcFileInfo.size();
                dstSize = dstFileInfo.size();
                srcNum.setNum(srcSize);
                dstNum.setNum(dstSize);
                QString message = dstName + " " + tr("is already exists, overwrite it?") +"\n\n\t";
                message = message +  tr("src File") + " : " + srcNum + " bytes\n\t" +  tr("dst File") + " : " + dstNum + " bytes";
                if (srcSize == dstSize)
                {
                    message = message + "\n\n" + tr("file size is same.");
                }
                if (srcSize < dstSize)
                {
                    message = message + "\n\n" + tr("the destination file is bigger.");
                }
                ans = QMessageBox::question(0, tr("confirmation"), message, QMessageBox::Yes, QMessageBox::No, QMessageBox::YesAll);
                if (ans == QMessageBox::YesAll)
                {
                    force = true;
                    ans = QMessageBox::Yes;
                }
            }
            if ((force == true)||(ans == QMessageBox::Yes))
            {
                // �㏑���m�F��Yes��I�񂾏ꍇ�A���邢�́AYesAll��I�񂾏ꍇ�ɂ́A�t�@�C���������ăR�s�[����
                QFile::remove(dstName);
            }
        }
        if (ans == QMessageBox::Yes)
        {
            // �t�@�C�����R�s�[����
            apDst->copyDatFile(srcNick, srcFile, dstName, srcName, apSrc->isSameLogFormat(), readPoint, lastUpdate);
            QString message = "\n  " + srcName + " --> " + dstName + "\t  (" + srcNick + ")";
            emit updateMessage(message);
        }
        
        // �v���O���X�o�[�̍X�V...
        emit updateValue(loop);
    }

    // subject.txt���R�s�[����w�肪�������ꍇ�B�B�B
    if (aCopySubjectTxt == Qt::Checked)
    {
        QSet<QString>::const_iterator item = nickSet.begin();
        while (item != nickSet.end())
        {
            QString nick     = *item;
            QString srcFile  = apSrc->getSubjectTxtFilePath(nick);
            QString dstFile  = apDst->getSubjectTxtFilePath(nick);
            if (QFile::exists(dstFile) == true)
            {
                // �t�@�C�������݂����ꍇ�ɂ͍폜���Ă���R�s�[����...
                QFile::remove(dstFile);
            }
            QFile::copy(srcFile, dstFile);
            QString message = "\n  " + srcFile + " -> " + dstFile;
            emit updateMessage(message);

            item++;
        }
    }
    return (ret);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
