#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include "janeBrowserParser.h"
#include "bbsTableParser.h"
#include "datFileUtil.h"

/*!
 *  �R���X�g���N�^
 * 
 */
janeBrowserParser::janeBrowserParser()
 : mpBbsTableParser(0), mBbsTable(0), mLogPath(0)
{
    // �����ł͉������Ȃ�
}


/*!
 *  �f�X�g���N�^
 * 
 */
janeBrowserParser::~janeBrowserParser()
{
    // �����ł͉������Ȃ�
}

/*!
 *   �u���E�U�̖��̂���������
 * 
 */
QString janeBrowserParser::getBrowserName(void)
{
    QString name = "jane";
    return (name);
}

/*!
 *   �Ǘ��p�X�̎w��...
 * 
 */
void janeBrowserParser::setPath(QString &arPath)
{
    mLogPath = arPath;
}

/*!
 *   bbstable.html�̎w��...
 * 
 */
void janeBrowserParser::setBbsTable(bbsTableParser *apBbsTable, QString &arBbsTable)
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
bool janeBrowserParser::prepare(void)
{
    // �ϐ����N���A����
    clearVariables();

    // �ꗗ����������
    mBbsParser.prepare(mLogPath, "jane2ch.brd");

    bool ret = checkDirectory(mLogPath);
    return (ret);
}

/*!
 *   ��͂̏���(�f�B���N�g�����@��...)
 * 
 */    
bool janeBrowserParser::checkDirectory(QString &arPath)
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
                QString idxFile = fileName;
                idxFile.replace(QRegExp("dat$"), "idx");
                if (QFile::exists(filePath + "/" + idxFile) == true)
                {
                    // �Ǘ���dat�t�@�C��������
                    // (�C���f�b�N�X�t�@�C���̉�͂����s)
                    parseIndexFile(filePath, idxFile);
                }
            }
        }
    }
    return (true);
}

/*!
 *  jane�̖��ǈʒu�Ǘ��t�@�C���̓ǂݏo��...
 * 
 * 
 */
void janeBrowserParser::parseIndexFile(QString &arPath, QString &arFileName)
{
    // bbsmenu.idx �� bbsmenu.dat �͑����Ă���̂Ŕr������...
    if (arFileName == "bbsmenu.idx")
    {
        return;
    }
    mIndexParser.parseFile(arPath, arFileName);
    return;
}

/*!
 *  dat�t�@�C���̐�����������
 * 
 */
int janeBrowserParser::datFileCount(void)
{
    return (mIndexParser.count());
}

/*!
 *   �Ǘ��O��dat�t�@�C�����`�F�b�N����
 *    (jane�ł͓��ʑΉ����Ȃ��̂�0)
 * 
 */
int  janeBrowserParser::checkUnmanagedDatFile(void)
{
    return (0);
}

/*!
 *   �Ǘ��O��dat�t�@�C�������擾����
 *    (jane�ł͓��ʑΉ����Ȃ�)
 * 
 */
QString janeBrowserParser::getUnmanagedDatFileName(int aIndex)
{
    return ("");
}

/*!
 *   �Ǘ��O��dat��Nick�����擾����
 *    (jane�ł͓��ʑΉ����Ȃ�)
 * 
 */
QString janeBrowserParser::getUnmanagedDatNickName(int aIndex)
{
    return ("");
}

/*!
 *   dat�t�@�C����(full path)�𐶐�����
 * 
 */
QString janeBrowserParser::getFullPathDatFileName(QString &arNick, QString &arFileName)
{
    QString datName = mBbsParser.getPath(arNick) + arFileName;
    return (datName);
}

/*!
 *   dat�t�@�C����(full path)�𐶐�����
 * 
 */
QString janeBrowserParser::getReceiveDatFileName(QString &arNick, QString &arFileName)
{
    QString pathName = mBbsParser.getPath(arNick);

    QDir path(mLogPath);
    path.mkpath(pathName);

    QString fileName = arFileName;
    int pos = arFileName.indexOf(".");
    fileName = arFileName.left(pos);

    QString datName = pathName + fileName + ".dat";

    return (datName);
}

/*!
 *  dat�t�@�C�������擾����
 * 
 */
QString janeBrowserParser::getDatFileName(int aIndex)
{
    if ((aIndex < 0)||(aIndex >= mIndexParser.count()))
    {
        return ("");
    }
    return (mIndexParser.getDatFileName(aIndex));    
}

/*!
 *  ��Nick�����擾����
 * 
 */
QString janeBrowserParser::getBoardNick(int aIndex)
{
    if ((aIndex < 0)||(aIndex >=  mIndexParser.count()))
    {
        return ("");
    }
    return (mIndexParser.getNickName(aIndex));
}

/*!
 *  ���ǈʒu�̎擾
 * 
 */
int  janeBrowserParser::getReadPoint(int aIndex)
{
    if ((aIndex < 0)||(aIndex >=  mIndexParser.count()))
    {
        return (1);
    }
    return (mIndexParser.getCurrentMessage(aIndex));
}

/*!
 *  �ŏI�X�V���Ԃ̎擾
 * 
 */
qint64 janeBrowserParser::getLastUpdate(int aIndex)
{
    if ((aIndex < 0)||(aIndex >=  mIndexParser.count()))
    {
        return (0);
    }
    QString str = mIndexParser.getLastUpdate(aIndex);

//////////// TODO : ������ �� ���l�ϊ����K�v!!
    return (0);
}

/*
 *   subject.txt�̃t�@�C����(full path)���擾����
 * 
 */
QString janeBrowserParser::getSubjectTxtFilePath(QString &arNick)
{
    QString pathName = mBbsParser.getPath(arNick) + "subject.txt";
    return (pathName);
}

/*!
 *   dat�t�@�C������M����
 * 
 */
bool janeBrowserParser::receiveDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
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
bool janeBrowserParser::copyDatFile(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{
//    QMessageBox::information(0, "Info(copy File)", arSrc + " -> " + arDst + " ", QMessageBox::Ok);

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
bool janeBrowserParser::updateInformations(QString &arNick, QString &arFile, QString &arDst, QString &arSrc, bool isSame, int aReadPoint, qint64 aLastUpdate)
{

    // �t�@�C�������擾����
    datFileUtil datFileUtility;
    QString path = getReceiveDatFileName(arNick, arFile);
    QString target = "/" + arFile;
    path.replace(target, "");
    QString title = datFileUtility.pickupDatFileTitle(path, arFile);
    int    nofMsg = datFileUtility.checkResCount(path, arFile);
    int    curMsg = aReadPoint;
    if (curMsg == nofMsg)
    {
        curMsg--;
    }
    int index = mIndexParser.exists(arFile);
    if (index >= 0)
    {
        // �o�^�ς�...�X�V����
        mIndexParser.setCurrentMessage(index, curMsg);
        mIndexParser.setNofMessage(index, nofMsg);
    }

    QString nick = arNick;
    nick.replace("/", "");
    QString url = mpBbsTableParser->getUrl(nick);
    int pos = url.lastIndexOf("/");
    url = url.left(pos);

    QString pathName = mBbsParser.getPath(arNick);
    pos = pathName.lastIndexOf("/");
    pathName = pathName.left(pos);
    
    // �A�C�e����ǉ��o�^
    mIndexParser.appendData(arNick, url, pathName, arFile, title, nofMsg, curMsg);
    return (true);
}

/*!
 *   ���Ƃ��܂����s���� (idx�t�@�C�����o�͂���...)
 * 
 */
bool janeBrowserParser::finalize(void)
{
    mIndexParser.outputFile();
    clearVariables();
    return (true);
}

/*!
 *   �����ϐ������N���A����
 * 
 */
void janeBrowserParser::clearVariables(void)
{
    //
    //
    //   
    return;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
