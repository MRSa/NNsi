#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMessageBox>
#include "janeIndexParser.h"
#include "a2BcConstants.h"

const int process_limit_line = 15;

/*!
 *  �R���X�g���N�^
 * 
 */
janeIndexParser::janeIndexParser()
{
    // �������Ȃ�...
}

/*!
 *  �f�X�g���N�^
 * 
 */
janeIndexParser::~janeIndexParser()
{
    // �������Ȃ�...
}

/**
 * 
 *  ���������N���A����
 * 
 */
void janeIndexParser::prepare(void)
{
    clearVariables();
    return;
}

/*!
 *  �t�@�C������͂���
 * 
 * 
 */
bool janeIndexParser::parseFile(const QString &arPath, const QString &arIdxFileName)
{
    QString fileName = arPath + "/" + arIdxFileName;
//    QMessageBox::information(0, "Info(parseFile)",  fileName, QMessageBox::Ok);

    if (QFile::exists(fileName) != true)
    {
        // �t�@�C�������݂��Ȃ��ꍇ�A�������Ȃ�
        return (false);
    }

    QFile indexFile(fileName);
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }

    // �t�@�C�����ƃp�X���L������
    QString path = arPath;
    process_line(-2, path);
    QString name = arIdxFileName;
    process_line(-1, name);
    
    int count = 0;
    QTextStream in(&indexFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        process_line(count, line);
        count++;
    }
    for (;count < process_limit_line; count++)
    {
        // ���܂�̗̈�i�ƃ_�~�[�f�[�^�j���X�V����
        QString line = "";
        process_line(count, line);
    }
    indexFile.close();
    
    return (true);
}

/*!
 *   �C���f�b�N�X�t�@�C���̂P�s����͂���
 * 
 */
void janeIndexParser::process_line(const int count, QString &arLineData)
{
    switch (count)
    {
          case -2:
            // �t�@�C���p�X���L�^����
            mFilePath.append(arLineData);
            break;

          case -1:
            // Idx�t�@�C�������L�^����
            mIdxFileName.append(arLineData);
            break;

          case 0:
            // �X���^�C�g�����i�[����
            mTitle.append(arLineData);
            break;

          case 1:
            // �ŏI�X�V�����i�[����
            mLastUpdate.append(arLineData);
            break;

          case 2:
            // ���X�i�[�����i�[����
            mNofMsg.append(arLineData);
            break;

          case 3:
            // ���݂̃��X�ԍ����i�[����
            mCurMsg.append(arLineData);
            break;

          case 4:
            // �_�~�[�f�[�^�i�H�j���i�[����
            mDummy1.append(arLineData);
            break;

          case 5:
            // URL���i�[����
            mUrl.append(arLineData);
            decideBoardNick(arLineData);
            break;

          case 6:
            // �_�~�[�f�[�^�i�O�j���i�[����
            mDummy2.append(arLineData);
            break;

          case 7:
            // �_�~�[�f�[�^�i�O�j���i�[����
            mDummy3.append(arLineData);
            break;

          case 8:
            // �_�~�[�f�[�^�i�H�j���i�[����
            mDummy4.append(arLineData);
            break;

          case 9:
            // �_�~�[�f�[�^�i�H�j���i�[����
            mDummy5.append(arLineData);
            break;

          case 10:
            // �_�~�[�f�[�^�i�O�j���i�[����
            mDummy6.append(arLineData);
            break;

          case 11:
            // �H�H�H���i�[����
            mUpdate.append(arLineData);
            break;

          case 12:
            // �_�~�[�f�[�^�i�O�j���i�[����
            mDummy7.append(arLineData);
            break;

          case 13:
            // �_�~�[�f�[�^�i�H�j���i�[����
            mDummy8.append(arLineData);
            break;

          case 14:
            // �_�~�[�f�[�^�i�H�j���i�[����
            mDummy9.append(arLineData);
            break;

          default:
            // 
            break;
    }
    return;
}

/*!
 *   ��Nick�𒊏o����
 * 
 */
void janeIndexParser::decideBoardNick(QString &arUrl)
{
    int pos = arUrl.lastIndexOf("/");
    
    QString nick = arUrl.mid((pos + 1)) + "/";
    mNick.append(nick);

    return;
}


/*!
 *   �C���f�b�N�X�t�@�C�����o�͂���
 * 
 */
void janeIndexParser::outputFile(void)
{
    int limit = mIdxFileName.size();
    for (int loop = 0; loop < limit; loop++)
    {
        outputFileMain(loop);
    }
    clearVariables();
    return;
}

/*!
 *   �C���f�b�N�X�t�@�C�����o�͂���
 * 
 */
void janeIndexParser::outputFileMain(int index)
{
//  QMessageBox::information(0, "Info(output file)", mIdxFileName[index], QMessageBox::Ok);

    QTextStream *out = 0;
    QString fileName = mFilePath[index] + "/" + mIdxFileName[index];

    // �Â��t�@�C�����������ꍇ�A���l�[������
    if (QFile::exists(fileName) == true)
    {
        QFile::remove(fileName + ".bak");
        QFile::rename(fileName, fileName + ".bak");
    }
    QFile indexFile(fileName);
    if (!indexFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }

    out = new QTextStream(&indexFile);
    if (out == 0)
    {
        return;
    }

    *out << mTitle[index] << endl;
    *out << mLastUpdate[index] << endl;
    *out << mNofMsg[index] << endl;
    *out << mCurMsg[index] << endl;
    *out << mDummy1[index] << endl;
    *out << mUrl[index] << endl;
    *out << mDummy2[index] << endl;
    *out << mDummy3[index] << endl;
    *out << mDummy4[index] << endl;
    *out << mDummy5[index] << endl;
    *out << mDummy6[index] << endl;
    *out << mUpdate[index] << endl;
    *out << mDummy7[index] << endl;
    *out << mDummy8[index] << endl;
    *out << mDummy9[index] << endl;

    out->flush();
    delete out;
    out = 0;
    QFile::remove(fileName + ".bak");
    return;
}

/*!
 *   �����f�[�^������������
 * 
 */
void janeIndexParser::clearVariables(void)
{
    mIdxFileName.clear();
    mTitle.clear();
    mLastUpdate.clear();
    mNofMsg.clear();
    mCurMsg.clear();
    mDummy1.clear();
    mUrl.clear();
    mDummy2.clear();
    mDummy3.clear();
    mDummy4.clear();
    mDummy5.clear();
    mDummy6.clear();
    mUpdate.clear();
    mDummy7.clear();
    mDummy8.clear();
    mDummy9.clear();
    mNick.clear();

    return;
}

/*!
 *  �t�@�C�����o�^����Ă��邩�ǂ����m�F����
 * 
 * 
 */
int janeIndexParser::exists(const QString &arFileName)
{
    for (int lp = 0; lp < mIdxFileName.size(); lp++)
    {
        QString target = mIdxFileName[lp];
        target.replace("idx", "dat");
        if (target == arFileName)
        {
            return (lp);
        }
    }
    return (-1);
}

/*!
 * 
 * 
 * 
 */
int janeIndexParser::count(void)
{
    return (mIdxFileName.size());
}

/*!
 * 
 * 
 * 
 */
int janeIndexParser::getNofMessage(int index)
{
    if ((index < 0)||(index >= mNofMsg.size()))
    {
        return (1);
    }
    QString nofMsg = mNofMsg[index];
    return (nofMsg.toInt());
}

/*!
 * 
 * 
 * 
 */
int janeIndexParser::getCurrentMessage(int index)
{
    if ((index < 0)||(index >= mCurMsg.size()))
    {
        return (1);
    }
    QString curMsg = mCurMsg[index];
    return (curMsg.toInt());
}

/*!
 * 
 * 
 * 
 */
QString janeIndexParser::getTitle(int index)
{
    if ((index < 0)||(index >= mTitle.size()))
    {
        return ("");
    }
    return (mTitle[index]);
}

/*!
 * 
 * 
 * 
 */
QString janeIndexParser::getDatFileName(int index)
{
    if ((index < 0)||(index >= mIdxFileName.size()))
    {
        return ("");
    }
    QString target = mIdxFileName[index];
    target.replace("idx", "dat");
    return (target);
}

/*!
 * 
 * 
 * 
 */
QString janeIndexParser::getNickName(int index)
{
    if ((index < 0)||(index >= mNick.size()))
    {
        return ("");
    }
    return (mNick[index]);
}

/*!
 * 
 * 
 * 
 */QString janeIndexParser::getLastUpdate(int index)
{
    if ((index < 0)||(index >= mLastUpdate.size()))
    {
        return ("");
    }
    return (mLastUpdate[index]);
}


/*!
 * 
 * 
 * 
 */
void janeIndexParser::setNofMessage(int index, int status)
{
    if ((index < 0)||(index >= mNofMsg.size()))
    {
        return;
    }
    QString num;
    num.setNum(status);
    mNofMsg[index] = num;    
    return;    
}

/*!
 * 
 * 
 * 
 */
void janeIndexParser::setCurrentMessage(int index, int status)
{
    if ((index < 0)||(index >= mCurMsg.size()))
    {
        return;
    }

    QString num;
    num.setNum(status);
    mCurMsg[index] = num;
    return;    
}

/*!
 * 
 * 
 * 
 */
void janeIndexParser::setTitle(int index, QString &arTitleName)
{
    if ((index < 0)||(index >= mTitle.size()))
    {
        return;
    }
    mTitle[index] = arTitleName;
    return;
}

/*!
 *  �X���f�[�^��o�^����
 * 
 */
bool janeIndexParser::appendData(QString &arNick, QString &arUrl, QString &arFilePath, QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage)
{
    QString nofMsg, curMsg, threadNumber, datFileName;
    nofMsg.setNum(aNofMessage);
    curMsg.setNum(aCurrentMessage);
    
    threadNumber = arDatFileName;
    threadNumber.replace(".dat", "");
    datFileName = threadNumber + ".idx";
    
    mFilePath.append(arFilePath);
    mIdxFileName.append(datFileName);
    mTitle.append(arTitle);
    mLastUpdate.append("");
    mNofMsg.append(nofMsg);
    mCurMsg.append(curMsg);
    mDummy1.append("");
    mUrl.append(arUrl);
    mDummy2.append("0");
    mDummy3.append("0");
    mDummy4.append("");
    mDummy5.append("");
    mDummy6.append("0");
    mUpdate.append(threadNumber);
    mDummy7.append("0");
    mDummy8.append("");
    mDummy9.append("");
    mNick.append(arNick);
    
    return (true);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
