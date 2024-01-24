#include <QString>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "GikoNaviBrowserParser.h"
#include "GikoNaviIdxParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/**
 *  �R���X�g���N�^
 * 
 */
GikoNaviBrowserParser::GikoNaviBrowserParser()
 : mpBbsTableParser(0), mBbsTable(0), mLogPath(0)
{
    // �����ł͉������Ȃ�
}


/**
 *  �f�X�g���N�^
 * 
 */
GikoNaviBrowserParser::~GikoNaviBrowserParser()
{
    // �����ł͉������Ȃ�
}

/**
 *   �u���E�U�̖��̂���������
 * 
 */
QString GikoNaviBrowserParser::getBrowserName(void)
{
    QString name = "GikoNavi";
    return (name);
}

/**
 *   �Ǘ��p�X�̎w��...
 * 
 */
void GikoNaviBrowserParser::setPath(QString &arPath)
{
    mLogPath = arPath;
    //QMessageBox::information(0, "LOG PATH", mLogPath, QMessageBox::Ok);
}

/**
 *   bbstable.html�̎w��...
 * 
 */
void GikoNaviBrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
{
    // BBS��̓N���X
    mpBbsTableParser = apBbsTable;

    // bbstable.html
    mBbsTable = arBbsTable;
}

/**
 *   ��͂̏���...
 * 
 */    
bool GikoNaviBrowserParser::prepare(void)
{
    clearAcquiredParser();

    bool ret = checkDirectory(mLogPath);
    return (ret);
}

/**
 *   ��͂̏���(�f�B���N�g�����@��...)
 * 
 */    
bool GikoNaviBrowserParser::checkDirectory(QString &arPath)
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
            // ���O�Ǘ��f�[�^�t�@�C��...
            if ( fileInfo.fileName() == "Folder.idx")
            {
                QString filePath = fileInfo.absolutePath();
                int pos = filePath.lastIndexOf("/");
                QString nickName = filePath.mid(pos + 1);
                QString idxFile = fileInfo.absolutePath() + "/Folder.idx";
                parseIndexFile(nickName, idxFile);
            }
        }
    }
    return (true);
}

/**
 *  �M�R�i�r�̖��ǈʒu�Ǘ��t�@�C���̓ǂݏo��...
 * 
 * 
 */
void GikoNaviBrowserParser::parseIndexFile(QString &arNickName, QString &arFileName)
{
    //QMessageBox::information(0, "Info(index File)", arNickName + " | " + arFileName, QMessageBox::Ok);
    GikoNaviIdxParser *parser = new GikoNaviIdxParser(arNickName);
    parser->parseFile(arFileName, mDatFileList);
    mAcquiredParser.append(parser);
    return;
}

/**
 *  �M�R�i�r���Ǘ����Ă���dat�t�@�C���̐�����������
 * 
 */
int GikoNaviBrowserParser::datFileCount(void)
{
    return (mDatFileList.size());
}

/**
 *   �Ǘ��O��dat�t�@�C�����`�F�b�N����
 * 
 */
int  GikoNaviBrowserParser::checkUnmanagedDatFile(void)
{
    return (0);
}

/**
 *   �Ǘ��O��dat�t�@�C�������擾����
 * 
 */
QString GikoNaviBrowserParser::getUnmanagedDatFileName(int aIndex)
{
    return ("");
}

/**
 *   �Ǘ��O��dat��Nick�����擾����
 * 
 */
QString GikoNaviBrowserParser::getUnmanagedDatNickName(int aIndex)
{
    return ("");
}

/**
 *   dat�t�@�C����(full path)�𐶐�����
 * 
 */
QString GikoNaviBrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    QString datName = mLogPath + "/2ch/" + arNick + arFileName;
    return (datName);
}

/*!
 *   dat�t�@�C����(full path)�𐶐�����
 * 
 */
QString GikoNaviBrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QDir path(mLogPath);
    path.mkpath(mLogPath + "/2ch/" + arNick);

    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QString datName = mLogPath + "/2ch/" + arNick + fileName + ".dat";
    return (datName);
}

/**
 *  dat�t�@�C�������擾����
 * 
 */
QString GikoNaviBrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return ("");
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    
    return (dataList.at(1));    
}

/**
 *  ��Nick�����擾����
 * 
 */
QString GikoNaviBrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return ("");
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    
    return (dataList.at(0));  
}

/**
 *  ���ǈʒu�̎擾
 * 
 */
int  GikoNaviBrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return (1);
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    int point = (dataList.at(2)).toInt();
    return (point);
}

/**
 *   subject.txt�̃t�@�C����(full path)���擾����
 * 
 */
QString GikoNaviBrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString datName = mLogPath + "/2ch/" + arNick + "subject.txt";
    return (datName);
}

/**
 *  �ŏI�X�V���Ԃ̎擾
 * 
 */
qint64 GikoNaviBrowserParser::getLastUpdate(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mDatFileList.size()))
    {
        return (0);
    }
    QString datFile = mDatFileList.at(aIndex);
    QStringList dataList = datFile.split("|");
    int point = (dataList.at(3)).toInt();
    return (point);
}

/**
 *   dat�t�@�C������M����
 * 
 */
bool GikoNaviBrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(entry File)", arNick + " | " + arFile, QMessageBox::Ok);

    // �t�@�C�����ړ�������
    datFileUtil util;
    util.moveDatFile(arSrc, isSame, arDst, false);

    // �Ǘ������X�V����
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/**
 *   dat�t�@�C�����R�s�[����
 * 
 */
bool GikoNaviBrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(copy File)", arNick + " | " + arFile + " - ", QMessageBox::Ok);

    // �t�@�C�����R�s�[����
    datFileUtil util;
    util.copyDatFile(arSrc, isSame, arDst, false);

    // �Ǘ������X�V����
    return (updateInformations(arNick, arFile, arDst, arSrc, isSame, aReadPoint, aLastUpdate));
}

/**
 *   �Ǘ������X�V����
 * 
 */
bool GikoNaviBrowserParser::updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
    // �t�@�C�������擾����
    datFileUtil datFileUtility;
    QString path = getReceiveDatFileName(arNick, arFile);
    QString target = "/" + arFile;
    path.replace(target, "");
    QString title = datFileUtility.pickupDatFileTitle(path, arFile);
    int    nofMsg = datFileUtility.checkResCount(path, arFile);

    QString boardNick = arNick;
    boardNick.replace("/", "");
    GikoNaviIdxParser *parser = 0;
    int index = -1;
    int limit = mAcquiredParser.size();

    for (int loop = 0; loop < limit; loop++)
    {
        GikoNaviIdxParser *checkParser = mAcquiredParser.at(loop);
        if (checkParser != 0)
        {
            QString myNick = checkParser->getMyNick();
            myNick.replace("/", "");
            if (myNick == boardNick)
            {
                index = checkParser->exists(arFile);
                parser = checkParser;
                if (index >= 0)
                {
                    // �o�^�ς�...�X�V����
                    checkParser->updateReadPoint(index, aReadPoint, nofMsg, aLastUpdate);
                }
                break;
            }   
        }
    }

    if (index < 0)
    {
        if (parser == 0)
        {
            // Folder.idx��ǉ�����
            QString indexPath = getReceiveDatFileName(arNick, arFile);
            indexPath.replace(arFile, "Folder.idx");
            parser = new GikoNaviIdxParser(boardNick);
            parser->parseFile(indexPath, mDatFileList);
            mAcquiredParser.append(parser);
        }

        // �A�C�e����ǉ��o�^
        if (parser != 0)
        {
            parser->appendData(arFile, title, nofMsg, aReadPoint, aLastUpdate);
        }
    }
    return (true);
}

/**
 *   ���Ƃ��܂����s���� (Folder.idx ���o�͂���...)
 * 
 */
bool GikoNaviBrowserParser::finalize(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = 0; loop < limit; loop++)
   {
        mAcquiredParser[loop]->outputFile();
   }
   return (true);
}

/**
 *   GikoNaviIdxParser���N���A����
 * 
 */
void GikoNaviBrowserParser::clearAcquiredParser(void)
{
   int limit = mAcquiredParser.size();
   for (int loop = limit; loop > 0; loop--)
   {
        delete mAcquiredParser[loop];
   }    
   mAcquiredParser.clear();
   mDatFileList.clear();
   return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2007- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
