/**
 *   bbstable.html��͕�...
 *
 */
#include <QMap>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QMessageBox>
#include "bbsTableParser.h"

/**
 *   �R���X�g���N�^
 *
 */
bbsTableParser::bbsTableParser(QWidget *parent) : QWidget(parent), mParsed(false)
{
    mUrlMap.clear();
}

/**
 *    �f�X�g���N�^
 *
 */
bbsTableParser::~bbsTableParser()
{
    mUrlMap.clear();
}

/**
 *    bbstable.html �̉�͂����s����
 *
 */
bool bbsTableParser::prepare(QString &arFileName)
{
    if (QFile::exists(arFileName) != true)
    {
        // �t�@�C�������݂��Ȃ��ꍇ�A�G���[�I������
        return (false);
    }

    QFile bbsTableFile(arFileName);
    if (!bbsTableFile.open(QIODevice::ReadOnly))
    {
        return (false);
    }
    QByteArray data = bbsTableFile.readAll();
    bbsTableFile.close();
    if ((data.isNull() == true)||(data.isEmpty() == true))
    {
        return (false);
    }

    mUrlMap.clear();
    mBbsDataMap.clear();
    mParsed = false;

    int length = data.length();
    for (int index = 0; index < length; index++)
    {
        int startPos = data.indexOf("HREF=http://", index);
        if (startPos < 0)
        {
            // �f�[�^��͏I���A���[�v�𔲂���
            break;
        }
        int endDataPos = data.indexOf("</A>", startPos);
        int endPos   = data.indexOf(">", startPos);
        if (endPos < 0)
        {
            // �f�[�^��͏I���A���[�v�𔲂���
            break;
        }
        startPos = startPos + 5; // 'HREF='���A�ǂݏo���ꏊ��i�߂�
        int len = endPos - startPos;
        if (len <= 0)
        {
            break;
        }

        // �f�[�^�̈�𒊏o����
        int dataLen = endDataPos - startPos;
        QByteArray bbsDataArray = data.mid(startPos, dataLen);

        // URL�𒊏o����...
        QByteArray urlData = data.mid(startPos, len);
        QString    url(urlData);

        QString  boardNick = url.section("/", 3, 3);
        if (boardNick.isEmpty() != true)
        {
            // ��Nick���m�肵�AMAP�Ɋi�[����
            boardNick = boardNick;
            mUrlMap.insert(boardNick, url);
            mBbsDataMap.insert(boardNick, bbsDataArray);
        }
        index = endPos;
    }
    mParsed = true;
    return (true);
}

/**
 *    ��͍ς݂̔�����������
 *
 */
int bbsTableParser::numberOfBoards()
{
    return (mUrlMap.count());
}

/**
 *    ��URL����������
 *
 */
QString bbsTableParser::getUrl(QString &arNick)
{
    QMap<QString, QString>::const_iterator it = mUrlMap.find(arNick);
    if (it != mUrlMap.end())
    {
        return (it.value());
    }
    return ("");
}

/**
 *    �̏�����������
 *
 */
QByteArray bbsTableParser::getData(QString &arNick)
{
    QMap<QString, QByteArray>::const_iterator it = mBbsDataMap.find(arNick);
    if (it != mBbsDataMap.end())
    {
        return (it.value());
    }
    return ("");
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
