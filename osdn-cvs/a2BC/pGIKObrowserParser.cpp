#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "pGIKObrowserParser.h"
#include "pGIKOacquiredParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/*!
 *  �R���X�g���N�^
 * 
 */
pGIKObrowserParser::pGIKObrowserParser()
 : mpBbsTableParser(0), mBbsTable(0), mLogPath(0)
{
    // �����ł͉������Ȃ�
}


/*!
 *  �f�X�g���N�^
 * 
 */
pGIKObrowserParser::~pGIKObrowserParser()
{
    // �����ł͉������Ȃ�
}

/*!
 *   �u���E�U�̖��̂���������
 * 
 */
QString pGIKObrowserParser::getBrowserName(void)
{
    QString name = "pGIKO";
    return (name);
}

/*!
 *   �Ǘ��p�X�̎w��...
 * 
 */
void pGIKObrowserParser::setPath(QString &arPath)
{
    mLogPath = arPath;
}

/*!
 *   bbstable.html�̎w��...
 * 
 */
void pGIKObrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
{
    // BBS��̓N���X
    mpBbsTableParser = apBbsTable;

    // bbstable.html
    mBbsTable = arBbsTable;
}

/*!
 *   ��͂̏���...
 * 
 */    
bool pGIKObrowserParser::prepare(void)
{
    mDatNickName.clear();
    mDatFilePath.clear();
    mDatFileName.clear();
    mIdxFilePath.clear();
    mIdxTitle.clear();
    mIdxReadNumber.clear();
    mAcquiredNickName.clear();
    mAcquiredFileName.clear();
    clearAcquiredParser();

    bool ret = checkDirectory(mLogPath);
    return (ret);
}

/*!
 *   ��͂̏���(�f�B���N�g�����@��...)
 * 
 */    
bool pGIKObrowserParser::checkDirectory(QString &arPath)
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
        if (fileInfo.isFile() == true)
        {
            QString filePath = fileInfo.absolutePath();
            QString fileName = fileInfo.fileName();
            QString suffix   = fileInfo.suffix();
            if (suffix == "dat")
            {
                int pos  = filePath.lastIndexOf("/dat");
                int pos2 = filePath.lastIndexOf("/", (pos - 1));
                QString nickName = filePath.mid((pos2 + 1), (pos - pos2));
                mDatNickName.append(nickName);
                mDatFilePath.append(filePath);
                mDatFileName.append(fileName);
            }

            // �ۂ��M�R�̃��O�Ǘ��f�[�^�t�@�C��...
            if (fileName == "acquired.txt")
            {
                QString idxFile = filePath + "/acquired.txt";
                int pos = filePath.lastIndexOf("/");
                QString nickName = filePath.mid(pos + 1) + "/";
                parseIndexFile(nickName, idxFile);
            }
        }
    }
    return (true);
}

/*!
 *  �ۂ��M�R�̖��ǈʒu�Ǘ��t�@�C���̓ǂݏo��...
 * 
 * 
 */
void pGIKObrowserParser::parseIndexFile(QString &arNickName, QString &arFileName)
{
//    QMessageBox::information(0, "Info(index File)", arNickName + " | " + arFileName, QMessageBox::Ok);
    pGIKOacquiredParser *parser = new pGIKOacquiredParser();
    parser->parseFile(arFileName);

    mAcquiredNickName.append(arNickName);
    mAcquiredFileName.append(arFileName);
    mAcquiredParser.append(parser);

    return;
}

/*!
 * 
 * 
 */
int pGIKObrowserParser::datFileCount(void)
{
    return (mDatFilePath.size());
}

/*!
 *   �Ǘ��O��dat�t�@�C�����`�F�b�N����
 * 
 */
int  pGIKObrowserParser::checkUnmanagedDatFile(void)
{
    return (0);
}

/*!
 *   �Ǘ��O��dat�t�@�C�������擾����
 * 
 */
QString pGIKObrowserParser::getUnmanagedDatFileName(int aIndex)
{
    return ("");
}

/*!
 *   �Ǘ��O��dat��Nick�����擾����
 * 
 */
QString pGIKObrowserParser::getUnmanagedDatNickName(int aIndex)
{
    return ("");
}

/*!
 *   dat�t�@�C����(full path)�𐶐�����
 * 
 */
QString pGIKObrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    QString nick = arNick;
    nick.replace("/", "");
    QString pathName = mpBbsTableParser->getUrl(nick);
    pathName.replace("http:/", "");

    QString datName = mLogPath + pathName + "dat/" + arFileName;
    return (datName);
}

/*!
 *   dat�t�@�C����(full path)�𐶐�����
 * 
 */
QString pGIKObrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QString nick = arNick;
    nick.replace("/", "");

    QString pathName = mpBbsTableParser->getUrl(nick);
    pathName.replace("http:/", "");

    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QDir path(mLogPath);
    path.mkpath(mLogPath + pathName + "dat/");

    QString datName = mLogPath + pathName + "dat/" + fileName + ".dat";
    return (datName);
}

/*!
 *  dat�t�@�C�������擾����
 * 
 */
QString pGIKObrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileName.size()))
    {
        return ("");
    }
    return (mDatFileName[aIndex]);    
}

/*!
 *  ��Nick�����擾����
 * 
 */
QString pGIKObrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFilePath.size()))
    {
        return ("");
    }
    QString filePath = mDatFilePath[aIndex];
    filePath.replace(QRegExp("/dat$"), "");
    int pos = filePath.lastIndexOf(QRegExp("[\\\\/]"));  // '\' �� '/' �Ƀ}�b�`����...
    pos++;
    return (filePath.mid(pos) + "/");    
}

/*!
 *  ���ǈʒu�̎擾
 * 
 */
int  pGIKObrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFilePath.size()))
    {
        return (1);
    }
    QString nickName = mDatNickName[aIndex];
    int limit = mAcquiredNickName.size();
    for (int loop = 0; loop < limit; loop++)
    {
        if (mAcquiredNickName[loop] == nickName)
        {
            int index = (mAcquiredParser[loop])->exists(mDatFileName[aIndex]);
            if (index < 0)
            {
                return (1);
            }
            return ((mAcquiredParser[loop])->getCurrentMessage(index));
        }
    }
    return (1);
}

/*
 *   subject.txt�̃t�@�C����(full path)���擾����
 * 
 */
QString pGIKObrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString nick = arNick;
    nick.replace("/", "");

    QString pathName = mpBbsTableParser->getUrl(nick);
    pathName.replace("http:/", "");

    QString datName = mLogPath + pathName + "subject.txt";
    return (datName);
}

/*!
 *  �ŏI�X�V���Ԃ̎擾
 * 
 */
qint64 pGIKObrowserParser::getLastUpdate(int aIndex)
{
    // �ۂ��M�R�͊Ǘ����Ă��Ȃ�...0����������B
    return (0);
}

/*!
 *   dat�t�@�C������M����
 * 
 */
bool pGIKObrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(entry File)", arNick + " | " + arFile, QMessageBox::Ok);

    // �t�@�C�����ړ�������
    datFileUtil util;
    util.moveDatFile(arSrc, isSame, arDst, true);

    // �Ǘ������X�V����
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/*!
 *   dat�t�@�C�����R�s�[����
 * 
 */
bool pGIKObrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(copy File)", arNick + " | " + arFile + " - ", QMessageBox::Ok);

    // �t�@�C�����R�s�[����
    datFileUtil util;
    util.copyDatFile(arSrc, isSame, arDst, true);

    // �Ǘ������X�V����
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/*!
 *   �Ǘ������X�V����
 * 
 */
bool pGIKObrowserParser::updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // �t�@�C�������擾����
    datFileUtil datFileUtility;
    QString path = getReceiveDatFileName(arNick, arFile);
    QString target = "/" + arFile;
    path.replace(target, "");
    QString title = datFileUtility.pickupDatFileTitle(path, arFile);
    int    nofMsg = datFileUtility.checkResCount(path, arFile);

    pGIKOacquiredParser *parser = 0;
    int index = -1;
    int limit = mAcquiredNickName.size();
    for (int loop = 0; loop < limit; loop++)
    {
        if (mAcquiredNickName[loop] == arNick)
        {
            // ����ł����I
            parser = mAcquiredParser[loop];
            if (parser != 0)
            {
                index = parser->exists(arFile);

            }
            if (index >= 0)
            {
                // �o�^�ς�...�X�V����
                parser->setCurrentMessage(index, aReadPoint);
                parser->setNofMessage(index, nofMsg);
            }
            break;
        }
    }

    if (index < 0)
    {
        if (parser == 0)
        {
            // acquired.txt��ǉ�����
            path   = getReceiveDatFileName(arNick, arFile);
            target = "dat/" + arFile;
            path.replace(target, "acquired.txt");
            parser = new pGIKOacquiredParser();
            parser->parseFile(path);
            mAcquiredNickName.append(arNick);
            mAcquiredFileName.append(path);
            mAcquiredParser.append(parser);
        }

        // �A�C�e����ǉ��o�^
        parser->appendData(arFile, title, nofMsg, aReadPoint);
    }
    return (true);
}

/*!
 *   ���Ƃ��܂����s���� (acquired.txt ���o�͂���...)
 * 
 */
bool pGIKObrowserParser::finalize(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = 0; loop < limit; loop++)
   {
        mAcquiredParser[loop]->outputFile();
   }
   return (true);
}

/*!
 *   pGIKOacquiredParser���N���A����
 * 
 */
void pGIKObrowserParser::clearAcquiredParser(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = limit; loop > 0; loop--)
   {
        delete mAcquiredParser[loop];
   }    
   mAcquiredParser.clear();
   
   return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
