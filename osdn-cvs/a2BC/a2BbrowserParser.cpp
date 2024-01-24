/*!
 *   a2B�̃��O�ǂݏ����N���X
 * 
 * 
 */
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "a2BcConstants.h"
#include "a2BbrowserParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BbrowserParser::a2BbrowserParser()
 : mpBbsTableParser(0), mCount(0), mUnmanagedCount(0)
{
    // �Ȃɂ����Ȃ�...
}

/*!
 *  �f�X�g���N�^
 * 
 */
a2BbrowserParser::~a2BbrowserParser()
{
    // �Ȃɂ����Ȃ�...
}

/*!
 *   �u���E�U�̖��̂���������
 * 
 */
QString a2BbrowserParser::getBrowserName(void)
{
    QString name = "a2B";
    return (name);
}

/*!
 *   �Ǘ��p�X�̎w��...
 *   (���O�t�@�C�����i�[���郋�[�g�f�B���N�g����)
 */
void a2BbrowserParser::setPath(QString &arPath)
{
    mBaseDirectory = arPath;
    return;
}

/*!
 *   bbstable��̓N���X�i��bbstable.html�j�̎w��...
 * 
 */
void a2BbrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
{
    mpBbsTableParser = apBbsTable;
    return;
}

/*!
 *   ��͂̏���...
 * 
 */    
bool a2BbrowserParser::prepare(void)
{
    /// �f�[�^���N���A����...
    mUnmanagedDatFilePath.clear();
    mUnmanagedDatFileName.clear();
    mFavoriteParser.clear();
    mIndexParser.clearIndexFile();
    mCount = 0;

    /// ���C�ɓ���f�[�^��ǂݍ���...
    mFavoriteParser.parseIndexFile(mBaseDirectory + "/favorite.idx");

    /// �f�B���N�g�����������āA���O�t�@�C���𒊏o����
    bool ret = checkDirectory(mBaseDirectory);
    return (ret);
}

/*!
 *   ��͂̏���(�f�B���N�g��������...)
 * 
 */    
bool a2BbrowserParser::checkDirectory(QString &arPath)
{
    QDir targetDir(arPath);
    QFileInfoList fileList = targetDir.entryInfoList();
    for (QFileInfoList::const_iterator ite = fileList.constBegin(); ite != fileList.constEnd(); ite++)
    {
        QFileInfo fileInfo = (*ite);
        if (fileInfo.isDir() == true)
        {
            QString filePath = fileInfo.absoluteFilePath();
            if (filePath.endsWith(".") != true)
            {
                checkDirectory(filePath);
            }
        }

        // �t�@�C�����ǂ����`�F�b�N
        if (fileInfo.isFile() == true)
        {
            // a2B�̃��O�Ǘ��f�[�^�t�@�C��...
            if ( fileInfo.fileName() == "a2b.idx")
            {
                QString idxFile = fileInfo.absolutePath() + "/a2b.idx";
                parseIndexFile(idxFile);
            }
        }
    }
    return (true);
}

/*!
 *   �Ǘ��O��dat�t�@�C�����`�F�b�N����
 * 
 */
int  a2BbrowserParser::checkUnmanagedDatFile(void)
{
//    QMessageBox::information(0, "checkUnmanagedDatFile()", mBaseDirectory, QMessageBox::Ok);

    /// �f�B���N�g�����������āA���O�t�@�C���𒊏o����
    mUnmanagedCount = 0;
    checkUnmanagedFiles(mBaseDirectory);

    return (mUnmanagedCount);
}

/*!
 *  �Ǘ�����Ă��Ȃ�dat�t�@�C�������o����(���C���̏���)
 * 
 */
void  a2BbrowserParser::checkUnmanagedFiles(QString &arPath)
{
    QDir targetDir(arPath);
    QFileInfoList fileList = targetDir.entryInfoList();
    for (QFileInfoList::const_iterator ite = fileList.constBegin(); ite != fileList.constEnd(); ite++)
    {
        QFileInfo fileInfo = (*ite);
        if (fileInfo.isDir() == true)
        {
            QString filePath = fileInfo.absoluteFilePath();
            if (filePath.endsWith(".") != true)
            {
                checkUnmanagedFiles(filePath);
            }
        }
        if (fileInfo.isFile() == true)
        {
            QString filePath = fileInfo.absolutePath();
            QString fileName = fileInfo.fileName();
            QString suffix   = fileInfo.suffix();
            if (suffix == "dat")
            {
                bool unmanaged = false;
                int index = mIndexParser.exists(fileName);
                if (index == -1)
                {
                    unmanaged = true;
                }
                else
                {
                    int nofMsg = mIndexParser.getNofMessage(index);
                    if (nofMsg > 1000)
                    {
                        unmanaged = true;
                    }
                }
                if (index == -1)
//                if (unmanaged == true)
                {
                    // ���O���Ǘ�����Ă��Ȃ�����...
                    mUnmanagedDatFilePath.append(filePath);
                    mUnmanagedDatFileName.append(fileName);
                    mUnmanagedCount++;
                }
            }
        }
    }
    return;    
}

/*!
 *  �f�[�^�Ǘ��t�@�C���̓ǂݏo��...
 * 
 * 
 */
void a2BbrowserParser::parseIndexFile(QString &arFileName)
{
//    QMessageBox::information(0, "Info(index File)", arFileName, QMessageBox::Ok);
    mCount = mCount + mIndexParser.parseIndexFile(arFileName);
    return;   
}

/*!
 *   �f�[�^�t�@�C���̐�����������
 * 
 */
int a2BbrowserParser::datFileCount(void)
{
    return (mCount);
}

/*!
 *    �Ǘ��O��dat�t�@�C�������擾����
 * 
 */
QString a2BbrowserParser::getUnmanagedDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mUnmanagedCount))
    {
        return ("");
    }
    return (mUnmanagedDatFileName[aIndex]);
}


/*
 *     �Ǘ��O��dat�j�b�N�l�[�����擾����
 * 
 */
QString a2BbrowserParser::getUnmanagedDatNickName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mUnmanagedCount))
    {
        return ("");
    }
    QString nick = mUnmanagedDatFilePath[aIndex];
    QString baseDir = mBaseDirectory.toLower();
    nick = nick.toLower();
    nick.replace(baseDir, "");
    return (nick + "/");
}

/*!
 *  dat�t�@�C����full-path���擾����
 * 
 */
QString a2BbrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    return (mBaseDirectory + arNick + arFileName);
}


/*!
 *   dat�t�@�C����(full path)�̎�M�p�t�@�C�����𐶐�����
 * 
 */
QString a2BbrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QDir path(mBaseDirectory);
    path.mkpath(mBaseDirectory + arNick);

    QString fullPath = mBaseDirectory + arNick + fileName + ".dat";
    return (fullPath);
}

/*!
 *  dat�t�@�C�������擾����
 * 
 */
QString a2BbrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return ("");
    } 

    QString fileName = mIndexParser.getFileName(aIndex);
    return (fileName);
}

/*!
 *  ��Nick�����擾����
 * 
 */
QString a2BbrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return ("");
    } 

    QString nick = mIndexParser.getNickName(aIndex);
    return (nick + "/");
}

/*!
 *  ���ǈʒu�̎擾
 * 
 */
int  a2BbrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return (1);
    } 

    int curMsg = mIndexParser.getCurrentMessage(aIndex);
    return (curMsg);
}

/*!
 *  �ŏI�X�V���Ԃ̎擾
 * 
 */
qint64 a2BbrowserParser::getLastUpdate(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mCount))
    {
        return (0);
    } 

    qint64 lastUpdate = mIndexParser.getLastUpdate(aIndex);
    return (lastUpdate);
}

/*
 *   subject.txt�̃t�@�C����(full path)���擾����
 * 
 */
QString a2BbrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString fullPath = mBaseDirectory + arNick + "subject.txt";
    return (fullPath);
}

/*!
 *   dat�t�@�C������M����
 * 
 */
bool a2BbrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // �f�B���N�g�����@��
    QDir path(mBaseDirectory);
    path.mkpath(mBaseDirectory + arNick);

    // dat�t�@�C�����ړ�����
    datFileUtil util;
    util.moveDatFile(arSrc, isSame, arDst, true);

    // dat�t�@�C�����Ǘ�����Ă��邩�ǂ����m�F����
    int index = mIndexParser.exists(arFile);
    if (index >= 0)
    {
        // �Ǘ�����Ă���dat�t�@�C��������...
        if (aReadPoint > 0)
        {
            mIndexParser.setCurrentMessage(index, aReadPoint);
            mIndexParser.setFileStatus(index, A2BCSTATUS_UPDATE);
        }
    }
    else
    {
        // �Ǘ�����Ă��Ȃ�dat�t�@�C��������...�o�^����
        insertData(arNick, arFile, aReadPoint, aLastUpdate);
    }
    
    return (true);
}

/*!
 *   dat�t�@�C�����R�s�[����
 * 
 */
bool a2BbrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // �f�B���N�g�����@��
    QDir path(mBaseDirectory);
    path.mkpath(mBaseDirectory + arNick);

    // dat�t�@�C�����R�s�[����
    datFileUtil util;
    util.copyDatFile(arSrc, isSame, arDst, true);

    // dat�t�@�C�����Ǘ�����Ă��邩�ǂ����m�F����
    int index = mIndexParser.exists(arFile);
    if (index >= 0)
    {
        // �Ǘ�����Ă���dat�t�@�C��������...
        if (aReadPoint > 0)
        {
            mIndexParser.setCurrentMessage(index, aReadPoint);
            mIndexParser.setFileStatus(index, A2BCSTATUS_UPDATE);
        }
    }
    else
    {
        // �Ǘ�����Ă��Ȃ�dat�t�@�C��������...�o�^����
        insertData(arNick, arFile, aReadPoint, aLastUpdate);
    }

    return (true);
}

/*!
 *   �Ǘ��t�@�C���֓o�^����
 * 
 */
bool a2BbrowserParser::insertData(QString &arNick, QString &arFile, int aReadPoint, qint64 aLastUpdate)
{
    //  QMessageBox::information(0, "insertData()", arNick + " | " + arFile, QMessageBox::Ok);

    datFileUtil util;
    QString target = mBaseDirectory + arNick;
    int pos = target.lastIndexOf("/");
    QString path = target.left(pos);
    int nofMsg = util.checkResCount(path, arFile);
    int curMsg = aReadPoint;
    if (curMsg < 1)
    {
        curMsg = 1;
    }
    QString title   = util.pickupDatFileTitle(path, arFile);
    QString idxFile = mBaseDirectory + arNick + "a2b.idx";
    QString option  = "0";
    mIndexParser.insertData(idxFile, arFile, option, title, curMsg, nofMsg, aLastUpdate);
    return (false);
}

/*!
 *   ���Ƃ��܂����s����
 * 
 */
bool a2BbrowserParser::finalize(void)
{
    // "a2b.idx" ���o�͂���B�B�B
    mIndexParser.outputIndexFile(false, false, false, 0);
    return (true);    
}
/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
