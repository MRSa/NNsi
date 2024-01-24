#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QMessageBox>
#include "GikoNaviIdxParser.h"

/**
 *  コンストラクタ
 */
GikoNaviIdxParser::GikoNaviIdxParser(QString &arNick, QWidget *parent)
 : QWidget(parent), mNick(arNick + "/"), mIndexFileName("")
{
    mIndexFileData.clear();    
}

/**
 *  デストラクタ
 */
GikoNaviIdxParser::~GikoNaviIdxParser()
{
    
}

/**
 *  ファイルの解析 (arDatFileNameList には、 | で区切って、 (NickName)|(datFileName)|(未読位置)|(最終更新時間) を応答する
 */
int GikoNaviIdxParser::parseFile(const QString &arFileName, QStringList &arDatFileNameList)
{  
    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    //QMessageBox::information(0, "GikoNaviIdxParser::parseFile()", "FileName : " + arFileName);
    mIndexFileName = arFileName;

    // インデックスファイル名が指定されていないか？
    if ((QFile::exists(arFileName) == false)||(codec == 0))
    {
        return (0);
    }

    // ファイルをオープン
    QFile indexFile(arFileName);
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (-1);
    }
 
    int count = 0;
    int lineCount = 0;
    QTextCodec::setCodecForCStrings(codec);
    QTextStream in(&indexFile);
    in.setCodec(codec);
    while (!in.atEnd())
    {
        QString line = (in.readLine()).trimmed();
        mIndexFileData << line;
        bool ret = process_line(arFileName, lineCount, line, arDatFileNameList);
        if (ret == true)
        {
            count++;
        }
        lineCount++;
    }
    indexFile.close();
    
    return (count);
}

/**
 *  ファイルの出力
 */
void GikoNaviIdxParser::outputFile(void)
{
    // コーデックの取得
    QTextCodec  *codec = QTextCodec::codecForName("Shift-JIS");
    if (codec == 0)
    {
        return;
    }

    // 古いファイルがあった場合、リネームする
    if (QFile::exists(mIndexFileName) == true)
    {
        QFile::remove(mIndexFileName + ".bak");
        QFile::rename(mIndexFileName, mIndexFileName + ".bak");
    }

    // ファイルをオープン
    QFile indexFile(mIndexFileName);
    if (!indexFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream outFile(&indexFile);
    outFile.setCodec(codec);
    QTextCodec::setCodecForCStrings(codec);

    outFile << "1.01" << endl;
    for (int index = 0; index < mIndexFileData.size(); index++)
    {
        if ((index == 0)&&((mIndexFileData.at(0)).toAscii() == "1.01"))
        {
            continue;
        }
        outFile << (mIndexFileData.at(index)).toAscii() << endl;
    }

    outFile.flush();
    indexFile.close();
    
    return;
}

/**
 * 
 */
 int GikoNaviIdxParser::exists(const QString &arFileName)
{
//    QMessageBox::information(0, "GikoNaviIdxParser::exists()", "FileName : " + arFileName);
    for (int index = 0; index < mIndexFileData.size(); index++)
    {
        QStringList splitData = mIndexFileData.at(index).split("\x01");
        if (splitData.size() < 2)
        {
            continue;
        }
        if (splitData.at(1) == arFileName)
        {
//            QMessageBox::information(0, "GikoNaviIdxParser::exists()", splitData.at(2) + " (" + arFileName + ") at " + QString::number(index));
            return (index);
        }
    }
//    QMessageBox::information(0, "GikoNaviIdxParser::exists()", "NOT MATCH  (" + arFileName + ")");
    return (-1);
}

/**
 *   未読位置とサイズを更新する
 * 
 */
 void GikoNaviIdxParser::updateReadPoint(int index, int currentMessage, int nofMessage, qint64 aLastUpdate)
{
    QStringList splitData = mIndexFileData.at(index).split("\x01");
    splitData.replace(3, (QString::number(nofMessage, 16)).toUpper());
    splitData.replace(12, (QString::number(nofMessage, 16)).toUpper());
    splitData.replace(8, (QString::number(currentMessage, 16)).toUpper());
    splitData.replace(11, (QString::number(currentMessage, 16)).toUpper());
    splitData.replace(5, (QString::number(aLastUpdate, 16)).toUpper());
    splitData.replace(6, (QString::number(aLastUpdate, 16)).toUpper());
    mIndexFileData.replace(index, splitData.join("\x01"));

    // 
    //QMessageBox::information(0, "UPDATE", " :: " + QString::number(currentMessage) + "/" + QString::number(nofMessage));
}

/**
 * 
 * 
 */
 bool GikoNaviIdxParser::appendData(QString &arDatFileName, QString &arTitle, int aNofMessage, int aCurrentMessage, qint64 aLastUpdate)
{
    int number = mIndexFileData.size();
    //QString data = "0x" + QString::number(number, 16);
    //QMessageBox::information(0, "APPEND", arTitle + " (" + arDatFileName + ")" + ", " + data + " :: " + QString::number(aCurrentMessage) + "/" + QString::number(aNofMessage));

/*
                    loop, 
                    fileName.c_str(), 
                    threadTitle.c_str(), 
                    resCount, 
                    fileSize,
                    roundDate,
                    lastUpdate,
                    readResNumber,
                    newReceivedRes,
                    reserved,
                    readUnread,
                    scrollTop,
                    allResCount,
                    newResCount,
                    ageSage);
*/
    number++;
    QString fileData = "";
    fileData = fileData + (QString::number(number, 16)).toUpper() + "\x01";  // count
    fileData = fileData + arDatFileName + "\x01";  // fileName
    fileData = fileData + arTitle + "\x01";  // threadTitle
    fileData = fileData + (QString::number(aNofMessage, 16)).toUpper() + "\x01";  // resCount
    fileData = fileData + "0" + "\x01";  // fileSize
    fileData = fileData + (QString::number(aLastUpdate, 16)).toUpper() + "\x01";  // roundDate
    fileData = fileData + (QString::number(aLastUpdate, 16)).toUpper() + "\x01";  // lastUpdate
    fileData = fileData + "FFFFFFFF" + "\x01"; // readResNumber
    fileData = fileData + (QString::number(aCurrentMessage, 16)).toUpper() + "\x01"; // newReceivedRes
    fileData = fileData + "0" + "\x01"; // reserved
    fileData = fileData + "0" + "\x01"; // readUnread
    fileData = fileData + (QString::number(aCurrentMessage, 16)).toUpper() + "\x01"; // scrollTop
    fileData = fileData + (QString::number(aNofMessage, 16)).toUpper() + "\x01";  // allResCount
    fileData = fileData + "0" + "\x01"; // newResCount
    fileData = fileData + "3";  // ageSage

    mIndexFileData << fileData;
    return (true);
}

/**
 * 
 */
bool GikoNaviIdxParser::process_line(const QString &arFileName, int count, QString &arLineData, QStringList &arDatFileNameList)
{
    if (count == 0)
    {
        // バージョン番号なので飛ばす。。。
        return (false);
    }
    QStringList lineData = arLineData.split("\x01");

    // datファイルが取得済みかどうか確認する
    QString res = "0x" + (lineData.at(3));
    if (res.toUInt(0, 16) != 0)
    {
        QString update = "0x" + lineData.at(6);
        // (NickName)|(datFileName)|(未読位置)|(最終更新時間) 
        QString dataLine = mNick + "|" + lineData.at(1) + "|" +  QString::number(res.toInt(0, 16)) + "|" + QString::number(update.toLongLong(0, 16)) + "0";
        // QMessageBox::information(0, "COUNT :" + QString::number(arDatFileNameList.size()), "DATALINE : " + dataLine);
        arDatFileNameList << dataLine;

        return (true);
    }
    return (false);
}

/**
 * 
 */
void GikoNaviIdxParser::reset()
{
    
}
