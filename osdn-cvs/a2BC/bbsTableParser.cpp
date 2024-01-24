/**
 *   bbstable.html解析部...
 *
 */
#include <QMap>
#include <QFile>
#include <QString>
#include <QByteArray>
#include <QMessageBox>
#include "bbsTableParser.h"

/**
 *   コンストラクタ
 *
 */
bbsTableParser::bbsTableParser(QWidget *parent) : QWidget(parent), mParsed(false)
{
    mUrlMap.clear();
}

/**
 *    デストラクタ
 *
 */
bbsTableParser::~bbsTableParser()
{
    mUrlMap.clear();
}

/**
 *    bbstable.html の解析を実行する
 *
 */
bool bbsTableParser::prepare(QString &arFileName)
{
    if (QFile::exists(arFileName) != true)
    {
        // ファイルが存在しない場合、エラー終了する
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
            // データ解析終了、ループを抜ける
            break;
        }
        int endDataPos = data.indexOf("</A>", startPos);
        int endPos   = data.indexOf(">", startPos);
        if (endPos < 0)
        {
            // データ解析終了、ループを抜ける
            break;
        }
        startPos = startPos + 5; // 'HREF='分、読み出す場所を進める
        int len = endPos - startPos;
        if (len <= 0)
        {
            break;
        }

        // データ領域を抽出する
        int dataLen = endDataPos - startPos;
        QByteArray bbsDataArray = data.mid(startPos, dataLen);

        // URLを抽出する...
        QByteArray urlData = data.mid(startPos, len);
        QString    url(urlData);

        QString  boardNick = url.section("/", 3, 3);
        if (boardNick.isEmpty() != true)
        {
            // 板Nickを確定し、MAPに格納する
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
 *    解析済みの板数を応答する
 *
 */
int bbsTableParser::numberOfBoards()
{
    return (mUrlMap.count());
}

/**
 *    板のURLを応答する
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
 *    板の情報を応答する
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
