/*!
 *   a2BCbbsTableTrim
 * 
 *   �����F
 *       a2B���g��bbstable.html�Ɂua2BC�v�J�e�S���A��URL��ǉ�����
 * 
 *     �� a2BC�J�e�S���ɒǉ������URL�́A45���ő�Ƃ��A����𒴂���ꍇ�ɂ́A
 *       ��(a2BC-XX�AXX���C���N�������g����)�̃J�e�S��������ɒǉ�����B
 *       (�����̋��E�l�́AmaxBbsDatasInCategory�Őݒ肷��B)
 * 
 *     �� �{�N���X�ŉ��H�����s�̖����ɂ́A"<!--a2BC-->" �Ƃ����������ǉ�����B
 * 
 */
#include <QList>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QTextCodecPlugin>
#include <QMessageBox>
#include "a2BCbbsTableTrim.h"
#include "a2BcConstants.h"
#include "a2BCfavoriteBbsData.h"
#include "bbsTableParser.h"

/*!
 *  �R���X�g���N�^
 * 
 */
a2BCbbsTableTrim::a2BCbbsTableTrim(QWidget *parent) : QWidget(parent)
{
    // ���ɉ������Ȃ�
}

/*!
 *  �f�X�g���N�^
 * 
 */
a2BCbbsTableTrim::~a2BCbbsTableTrim()
{
    // ���ɉ������Ȃ�
}

/*!
 *  �f�[�^�̃��Z�b�g
 * 
 */
void a2BCbbsTableTrim::resetTrimmer()
{
    // �ǉ�����f�[�^������
    mBbsTableAppend.clear();
}

/*!
 *  �ꗗ�f�[�^�̉��H����
 *    (true : ����OK�A false : �������s)
 */
bool a2BCbbsTableTrim::prepareTrimmer(QString &arFileName)
{
    mBbsTableFileName = "";
    if (QFile::exists(arFileName) != true)
    {
        // �t�@�C�������݂��Ȃ��ꍇ�A�G���[��������
        return (false);
    }

    mBbsTableFileName = arFileName;

    QFile bbsTableFile(arFileName);
    if (!bbsTableFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // �t�@�C���̃I�[�v�����s�A�A�A�I������
        return (false);
    }

    // ���H�O�t�@�C�������ɂ������ꍇ�폜����
    if (QFile::exists(arFileName + ".back") == true)
    {
        if (QFile::remove(arFileName + ".back") == false)
        {
            // ���H�O�t�@�C���������s...�I������
            bbsTableFile.close();
            return (false);
        }
    }

    // ���H�O�t�@�C���̃I�[�v��
    QFile bbsTableBakFile(arFileName + ".back");
    if (!bbsTableBakFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        bbsTableFile.close();
        return (false);
    }

    // �ǉ����ꂽ�f�[�^���N���A����
    clearLogBoardData();

    // a2BC�����H�����s�𔲂����s�����H�O�t�@�C���ɏo�͂���
    QTextStream in(&bbsTableFile);
    QTextStream out(&bbsTableBakFile);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.contains("<!--a2BC-->") != true)
        {
            // �f�[�^���o�͂���
            out << line << endl;
        }
        else
        {
            if (line.contains("http://") == true)
            {
                // �f�[�^�s���L������
                appendBoardUrlData(line);
            }
        }
    }
    bbsTableBakFile.close();
    bbsTableFile.close();

    return (true);
}

void a2BCbbsTableTrim::clearLogBoardData()
{
    mItemModel.clear();

    int limit = mFavoriteBbsData.size();
    for (int loop = limit - 1; loop >= 0; loop--)
    {
        delete mFavoriteBbsData[loop];
    }
    mFavoriteBbsData.clear();
}

void a2BCbbsTableTrim::appendBoardUrlData(const QString &lineData)
{
    QString boardName(lineData.section('>',1,1));
    QString boardUrl(lineData.section('>',0,0));
    QString sortKey = "";

    boardName.replace(QRegExp("</[Aa]"),"");
    boardUrl.replace(QRegExp("< *[Aa] +[Hh][Rr][Ee][Ff]="),"");

    // �f�[�^���i�[����
    entryBbsData(boardName, boardUrl, sortKey);

}

/*!
 *  �f�[�^���P���i�[����
 * 
 */
void a2BCbbsTableTrim::entryBbsData(QString &boardName, QString &boardUrl, QString &sortKey)
{
    a2BCfavoriteBbsData *bbsData = new a2BCfavoriteBbsData();
    bbsData->setData(boardName, boardUrl, sortKey);    
    mFavoriteBbsData.append(bbsData);

    // �f�[�^������
    QList<QStandardItem *> itemList;
    itemList << bbsData->getName() << bbsData->getUrl() << bbsData->getSortKey();
    mItemModel.appendRow(itemList);
}

/*!
 *  bbstable.html�̉��H�������C��
 * 
 */
bool a2BCbbsTableTrim::outputTrimmer(void)
{
    // ���H�O�t�@�C���̃I�[�v��
    QFile bbsTableBakFile(mBbsTableFileName + ".back");
    if (!bbsTableBakFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (false);
    }

    // �o�̓t�@�C���̃I�[�v��
    QFile bbsTableFile(mBbsTableFileName);
    if (!bbsTableFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        bbsTableBakFile.close();
        return (false);
    }
 
    // �e�L�X�g�X�g���[���̏���(�e�L�X�g�R�[�f�b�N�̐ݒ���s��)
    QTextStream out(&bbsTableFile);
    QTextStream in(&bbsTableBakFile);
    QTextCodec *codec = setCodec(out);

    // a2BC�J�e�S���f�[�^�̃t�@�C���o��
    outputA2BCcategory(out, codec);

    // ���H�O�f�[�^�t�@�C���̓��e�R�s�[
    out.setCodec(QTextCodec::codecForLocale());
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (line.contains("<!--a2BC-->") != true)
        {
            // �ǂݍ��񂾂��̂����̂܂܃f�[�^�o�͂���
            out << line << endl;
        }
    }
    bbsTableBakFile.close();
    bbsTableFile.close();
    
    // ���H�O�t�@�C���̍폜
    QFile::remove(mBbsTableFileName + ".back");
    return (true);
}

/*!
 *  �f�[�^�̒ǉ�
 * 
 */
void a2BCbbsTableTrim::appendTrimmer(QByteArray &arData)
{
    mBbsTableAppend.append(arData);
    return;
}

/*!
 *   codec�̏���...
 * 
 */
QTextCodec *a2BCbbsTableTrim::setCodec(QTextStream &arOutputStream)
{
    // codec�N���X�����܂���...
    QTextCodec  *codec = QTextCodec::codecForName("Shift_JIS");
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("MS_Kanji");
    }
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("sjis");
    }
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("SJIS");
    }
    if (codec == 0)
    {
        codec = QTextCodec::codecForName("System");
    }

    // �o��stream�ɃR�[�f�b�N��ݒ肷��B
    if (codec == 0)
    {
        arOutputStream.setCodec(QTextCodec::codecForLocale());
        return (QTextCodec::codecForLocale());
    }
    arOutputStream.setCodec(codec);
    return (codec);
}

/*!
 *   a2BC�J�e�S���̏o��
 * 
 * 
 */
void a2BCbbsTableTrim::outputA2BCcategory(QTextStream &arOutputStream, QTextCodec *codec)
{
    // codec == 0�̂Ƃ��ɂ́Aa2B�J�e�S���͏o�͂��Ȃ��悤�ɂ���
    if (codec == 0)
    {
        outputA2Bcategory_noCodec(arOutputStream);
        return;
    }

    // 1��a2BC�J�e�S���ɓ������̍ő�l (���̒l�𒴂���ƕ�������)
    int maxBbsDatasInCategory = 45;

    // �O����...
    arOutputStream << "<!--a2BC-->" << endl;

    // �J�e�S���f�[�^�o�͂̃��C��
    int count = 0;
    for (QVector<QByteArray>::ConstIterator it = mBbsTableAppend.begin(); it != mBbsTableAppend.end(); it++)
    {
        ///// �����ŃJ�e�S���� (�ya2B-xx�z) ���o�͂���
        if ((count % maxBbsDatasInCategory) == 0)
        {
            arOutputStream << codec->toUnicode("�y<B>a2B");

            // "-xx"�����̏o��(�K�v�ȏꍇ�ɂ�...
            if ((count / maxBbsDatasInCategory) != 0)
            {
                QString num;
                num.setNum(count / maxBbsDatasInCategory);
                arOutputStream << "-" << num;
            }
            arOutputStream << codec->toUnicode("</B>�z") << "<!--a2BC-->" << endl;
        }

        // ��(��URL)��1�����o�͂���
        arOutputStream << "<A HREF=" << codec->toUnicode(*it) << "</A>" << "<!--a2BC-->" << endl;
        count++;
    }

    // �㏈��...
    arOutputStream << codec->toUnicode("�y<B>-----</B>�z") << "<!--a2BC-->" << endl;
    arOutputStream << "<!--a2BC-->" << endl;
    
    return;
}

/*!
 *   a2BC�J�e�S���̏o��(�R�[�f�b�N���g�p��)
 * 
 * 
 */
void a2BCbbsTableTrim::outputA2Bcategory_noCodec(QTextStream &arOutputStream)
{
    // 1��a2BC�J�e�S���ɓ������̍ő�l (���̒l�𒴂���ƕ�������)
    int maxBbsDatasInCategory = 45;

    // �O����...
    arOutputStream << "<!--a2BC--><!--(NO CODEC)-->" << endl;

    // �J�e�S���f�[�^�o�͂̃��C��
    int count = 0;
    for (QVector<QByteArray>::ConstIterator it = mBbsTableAppend.begin(); it != mBbsTableAppend.end(); it++)
    {
        ///// �����ŃJ�e�S���� (�ya2B-xx�z) ���o�͂���
        if ((count % maxBbsDatasInCategory) == 0)
        {
            arOutputStream << "�y<B>a2B";

            // "-xx"�����̏o��(�K�v�ȏꍇ�ɂ�...
            if ((count / maxBbsDatasInCategory) != 0)
            {
                QString num;
                num.setNum(count / maxBbsDatasInCategory);
                arOutputStream << "-" << num;
            }
            arOutputStream << "</B>�z" << "<!--a2BC-->" << endl;
        }

        // ��(��URL)��1�����o�͂���
        arOutputStream << "<A HREF=" << (*it) << "</A>" << "<!--a2BC-->" << endl;
        count++;
    }

    // �㏈��...
    arOutputStream << "�y<B>-----</B>�z" << "<!--a2BC-->" << endl;
    arOutputStream << "<!--a2BC--><!--(NO CODEC)-->" << endl;

    return;
}

/**
 *   a2B�̃��O�f�B���N�g��������F������
 * 
 * 
 */
bool a2BCbbsTableTrim::readA2BLogBoards(const QString &arFileName)
{
    bbsTableParser bbsTable;
    QString fileName = arFileName;

    // �ꗗ�f�[�^�̓ǂݍ���...
    bool readyBbs = bbsTable.prepare(fileName);
    if (readyBbs == false)
    {
        // �ꗗ�̉�͎��s...�I������
        return (false);
    }

    // �����p�f�B���N�g�������m�肷��
    QString baseDirectory = fileName;
    baseDirectory.replace(QRegExp("bbstable.html$"), "");

    // ���O�i�[�f�B���N�g���̈ꗗ���擾����
    QDir baseDir(baseDirectory);
    QFileInfoList list = baseDir.entryInfoList();
    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir() == true)
        {
            QString dirName = fileInfo.fileName();
            if (dirName.startsWith(".") != true)
            {
                // ���O�f�B���N�g�������o�A�A�A
                QString    url  = bbsTable.getUrl(dirName);
                QByteArray data = bbsTable.getData(dirName);

                // codec�N���X�����܂���...
                QTextCodec  *codec = QTextCodec::codecForName("Shift_JIS");
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("MS_Kanji");
                }
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("sjis");
                }
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("SJIS");
                }
                if (codec == 0)
                {
                    codec = QTextCodec::codecForName("System");
                }
                if (url.isEmpty() != true)
                {
                    QString lineData;
                    if (codec != 0)
                    {
                        lineData = "<A HREF=" + codec->toUnicode(data) + "</A>";                        
                    }
                    else
                    {
                        lineData = "<A HREF=" + data + "</A>";
                    }
                    appendBoardUrlData(lineData);
                }
            }
        }
    }    
    return (true);
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
