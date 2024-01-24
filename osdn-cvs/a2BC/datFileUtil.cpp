/**
 *   datFileUtil
 * 
 *    [����]
 * 
 * 
 */
#include <QFile>
#include <QIODevice>
#include <QTextCodec>
#include <QTextStream>
#include <QMessageBox>
#include "datFileUtil.h"

/**
 *  �R���X�g���N�^
 * 
 */
datFileUtil::datFileUtil()
{
    //
}

/**
 *  �f�X�g���N�^
 * 
 */
datFileUtil::~datFileUtil()
{
    //
}

/**
 *  dat�t�@�C������X���^�C�g�������o��
 *   (dat�t�@�C���̂P�s�ڂ� "<>"�ȍ~�Ƀt�@�C�������i�[����Ă���A���̕����𒊏o����)
 * 
 */
QString datFileUtil::pickupDatFileTitle(const QString &arPath, const QString &arFileName)
{
    QFile datFile(arPath + "/" + arFileName);
    if (!datFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // �t�@�C�����I�[�v�����邱�Ƃ��ł��Ȃ�����...�I������
        return (arFileName);
    }

    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    if (codec == 0)
    {
        // codec���擾�ł��Ȃ�����...�I������
        return (arFileName);
    }

    QTextStream in(&datFile);
    in.setCodec(codec);
    QString line = in.readLine();
    datFile.close();
    if (line.isEmpty())
    {
        // 1�s�ڂ�ǂݏo�����Ƃ��ł��Ȃ�����...�I������
        return (arFileName);
    }

    int     pos = line.lastIndexOf("<>");
    if (pos < 0)
    {
        // �t�@�C���t�H�[�}�b�g���Ⴄ...�H �^�C�g���̎擾���ł��Ȃ������̂ŏI������
        return (arFileName);
    }
    return (line.mid((pos + 2)));
}

/**
 *  dat�t�@�C���Ɋ܂܂�Ă��郌�X������������ (dat�t�@�C���̍s���𐔂��邾��...)
 * 
 */
int datFileUtil::checkResCount(const QString &arPath, const QString &arFileName)
{
    QFile datFile(arPath + "/" + arFileName);
    if (!datFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // �t�@�C�����I�[�v�����邱�Ƃ��ł��Ȃ�����...�I������
        return (0);
    }

    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    if (codec == 0)
    {
        // codec���擾�ł��Ȃ�����...�I������
        return (0);
    }

    QTextStream in(&datFile);
    in.setCodec(codec);
    int count = 0;
    while (1)
    {
        QString line = in.readLine();
        if (line.isNull() == true)
        {
            break;
        }
        count++;
    }
    datFile.close();
    return (count);
}

/**
 *  dat�t�@�C���̈ړ������s����
 * 
 * 
 */
bool datFileUtil::moveDatFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst)
{
    bool ret = false;
    if (isSameSrc == isSameDst)
    {
        // �t�@�C�����ړ�������
        ret = QFile::rename(arSrcFile, arDstFile);
/**
        if (ret == false)
        {
            QMessageBox::critical(0, "Error(move File)", arSrcFile + " -> " + arDstFile + " ", QMessageBox::Ok, QMessageBox::Cancel);
        }
**/
    }
    else
    {
        // �t�@�C�����ړ�����
        ret = copyFile(arSrcFile, isSameSrc, arDstFile, isSameDst, true);
    }
    return (ret);
}

/**
 *  dat�t�@�C���̃R�s�[�����s����
 * 
 */
bool datFileUtil::copyDatFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst)
{
    bool ret = false;
    if (isSameSrc == isSameDst)
    {
        // �t�@�C�����R�s�[����
        ret = QFile::copy(arSrcFile, arDstFile);
/**
        if (ret == false)
        {
            QMessageBox::critical(0, "Error(copy File)", arSrcFile + " -> " + arDstFile + " ", QMessageBox::Ok, QMessageBox::Cancel);
        }
**/
    }
    else
    {
        // �t�@�C�����R�s�[
        ret = copyFile(arSrcFile, isSameSrc, arDstFile, isSameDst, false);
    }
    return (ret);
}

/**
 * 
 * 
 */
bool datFileUtil::copyFile(const QString &arSrcFile, bool isSameSrc, const QString &arDstFile, bool isSameDst, bool isRemoveSrcFile)
{

    // �ǂݍ��ރt�@�C���̏���...
    QFile inData(arSrcFile);
    bool ret = inData.open(QIODevice::ReadOnly | QIODevice::Text);
//    bool ret = inData.open(QIODevice::ReadOnly);
    if (ret == false)
    {
        return (false);
    }

    qint64 fileSize = inData.size();
    char *data = new char[fileSize + 2];

    // �����o���t�@�C���̏���...
    if (QFile::exists(arDstFile) == true)
    {
        // �R�s�[��t�@�C�����������ꍇ�͍폜
        QFile::remove(arDstFile);
    }
    QFile outData(arDstFile);
    ret = outData.open(QFile::WriteOnly | QFile::Truncate);
    if (ret == false)
    {
        inData.close();
        delete[] data;
        return (false);
    }

    qint64 readSize = inData.readLine(data, fileSize);
    while (readSize > 0)
    {
        if (data[readSize -1] == '\n')
        {
            data[readSize - 1] = '\0';
            readSize--;
        }
        if (data[readSize - 1] == '\r')
        {
            data[readSize - 1] = '\0';
            readSize--;
        }
       
        if (isSameDst == true)
        {
            readSize++;
            data[readSize - 1] = '\n';
        }
        else
        {
            readSize++;
            data[readSize - 1] = '\r';
            readSize++;
            data[readSize - 1] = '\n';
        }
        outData.write(data, readSize);
        readSize = inData.readLine(data, fileSize);        
    }

    /// �㏈��...
    outData.close();
    inData.close();

    if (isRemoveSrcFile == true)
    {
        // �R�s�[��t�@�C�����������ꍇ�͍폜
        QFile::remove(arDstFile);
    }
    delete[] data;
    return (true);
}

/**
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
