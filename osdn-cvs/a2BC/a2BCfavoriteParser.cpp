/*!
 *   a2BCfavoriteParser
 * 
 *   説明：
 *       a2Bの「お気に入り管理ファイル」の管理
 *
 */
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "a2BCfavoriteParser.h"
#include "a2BcConstants.h"

/*!
 *  コンストラクタ
 * 
 */
a2BCfavoriteParser::a2BCfavoriteParser(QWidget *parent) : QWidget(parent)
{
    // なにもしない
}

/*!
 *  デストラクタ
 * 
 */
a2BCfavoriteParser::~a2BCfavoriteParser()
{
    // なにもしない
}

/*!
 *  ファイルを読み出し、解析する
 * 
 * 
 */
int a2BCfavoriteParser::parseIndexFile(const QString &arFileName)
{
    if (QFile::exists(arFileName) != true)
    {
        // ファイルが存在しない場合、何もしない
        return (0);
    }

    QFile indexFile(arFileName);
    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return (0);
    }
        
    QTextStream in(&indexFile);
    int count = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        if (process_line(arFileName, line) == true)
        {
            count++;
        }
    }
    indexFile.close();

    return (count);
}

/*!
 *   管理ファイルの１行を読み出し解析する
 * 
 */
bool a2BCfavoriteParser::process_line(const QString &arFileName, QString &arLineData)
{
    if (arLineData.isEmpty() == true)
    {
        return (false);
    }
 
    QString datFileName = arLineData.section(",", 0, 0);
    QString maxNumStr   = arLineData.section(",", 1, 1);
    QString curNumStr   = arLineData.section(",", 2, 2);
    QString statusStr   = arLineData.section(",", 3, 3);
    QString levelStr    = arLineData.section(",", 4, 4);
    QString dateTimeStr = arLineData.section(",", 5, 5);
    QString dummy2Str   = arLineData.section(",", 6, 6);
    QString dummy3Str   = arLineData.section(",", 7, 7);
    QString urlStr      = arLineData.section(",", 8, 8);
    QString nickStr     = arLineData.section(",", 9, 9);
    QString optionStr   = arLineData.section(",", 10);
    
    if (datFileName.contains(".dat") == false)
    {
        return (false);
    }


/**
    mIndexFileName.append(arFileName);
    mDatFileName.append(datFileName);
    mNofMessage.append(maxNumStr.toInt());
    mCurrentMessage.append(curNumStr.toInt());
    mDatFileStatus.append(statusStr.toInt());
    mFavoriteLevel.append(levelStr.toInt());
    mLastUpdate.append(dateTimeStr.toULongLong());
    mDummy2.append(dummy2Str.toInt());
    mDummy3.append(dummy3Str.toInt());
    mUrlData.append(urlStr);
    mNickData.append(nickStr);
    mOptionData.append(optionStr);      
    mUsable.append(0);
**/

    // データの追加...
    appendData(arFileName, datFileName, maxNumStr.toInt(), curNumStr.toInt(), statusStr.toInt(), levelStr.toInt(), dateTimeStr.toULongLong(), dummy2Str.toInt(), dummy3Str.toInt(), urlStr, nickStr, optionStr, 0);

    return (true);
}

/*!
 *  データの追加
 * 
 */
void a2BCfavoriteParser::appendData(const QString &arFileName, QString &arDatFileName, int maxMsg, int curMsg, int status, int level, quint64 lastUpdate, int dummy2, int dummy3, QString &arUrl, QString &arNick, QString &arOption, int usable)
{
    QString fileName = arFileName;
    a2BCfavoriteData *data = new a2BCfavoriteData(fileName, arDatFileName, maxMsg, curMsg, status, level, lastUpdate, dummy2, dummy3, arUrl, arNick, arOption, usable);

    // データの追加...
    mFavoriteData.append(data);
}

/*!
 *  管理ファイルを出力する
 * 
 */
void a2BCfavoriteParser::outputIndexFile(bool aRemoveError, bool aRemoveOver)
{
    QTextStream *out = 0;
    QFile *indexFile = 0;
    QString fileName = "";
    int limit = mFavoriteData.size();
    for (int index = 0; index < limit; index++)
    {
        a2BCfavoriteData *data = mFavoriteData[index];
        if (data == 0)
        {
            continue;
        }

        quint64 updateTime = data->getLastUpdate();
        QString indexFileName = data->getIndexFileName();
        if (indexFileName != fileName)
        {
            // ファイルオープン...
            fileName = indexFileName;
            
            if (out != 0)
            {
                *out << ";[EOF]" << endl;
                out->flush();
                delete out;
                out = 0;
            }
            
            if (indexFile != 0)
            {
                indexFile->close();
                delete indexFile;
                indexFile = 0;
            }

            // 古いファイルがあった場合、リネームする
            if (QFile::exists(fileName) == true)
            {
                QFile::remove(fileName + ".bak");
                QFile::rename(fileName, fileName + ".bak");
                QFile::remove(fileName);
            }
            indexFile = new QFile(fileName);
            if (!indexFile->open(QIODevice::WriteOnly | QIODevice::Text))
            {
                delete indexFile;
                indexFile = 0;
                fileName = "";
                continue;
            }
            out = new QTextStream(indexFile);
            if (out == 0)
            {
                delete indexFile;
                indexFile = 0;
                fileName = "";
                continue;
            }
            *out << "; FileName, Max, Current, Status, Level, lastAccess, 0, 0, Url, Nick, (Title)" << endl;

            int fileStatus = data->getFileStatus();
            int isUsable   = data->getUsable();
            int nofMsg     = data->getNofMessage();
            if ((aRemoveError == true)&&(fileStatus == A2BCSTATUS_GETERROR))
            {
                // 出力しない
//                (*out) << data->getDatFileName() << "," << data->getNofMessage() << "," << data->getCurrentMessage() << "," << data->getFileStatus() << "," << data->getFavoriteLevel() << "," << updateTime << "," << data->getDummy2() << "," << data->getDummy3() << "," << data->getUrl() << "," << data->getNickName() << "," << data->getOption() << endl;
//              (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mFavoriteLevel[index] << "," << updateTime << "," << mDummy2[index] << "," << mDummy3[index] << "," << mUrlData[index] << "," << mNickData[index] << "," << mOptionData[index] << endl;
            }
            else if ((aRemoveOver == true)&&(nofMsg > 1000))
            {
                // 出力しない
//                (*out) << data->getDatFileName() << "," << data->getNofMessage() << "," << data->getCurrentMessage() << "," << data->getFileStatus() << "," << data->getFavoriteLevel() << "," << updateTime << "," << data->getDummy2() << "," << data->getDummy3() << "," << data->getUrl() << "," << data->getNickName() << "," << data->getOption() << endl;
//              (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mFavoriteLevel[index] << "," << updateTime << "," << mDummy2[index] << "," << mDummy3[index] << "," << mUrlData[index] << "," << mNickData[index] << "," << mOptionData[index] << endl;
            }
            else // if ((aRemoveError == false)&&(aRemoveOver == false))
            {
                if (isUsable > 0)
                {
                    (*out) << data->getDatFileName() << "," << data->getNofMessage() << "," << data->getCurrentMessage() << "," << data->getFileStatus() << "," << data->getFavoriteLevel() << "," << updateTime << "," << data->getDummy2() << "," << data->getDummy3() << "," << data->getUrl() << "," << data->getNickName() << "," << data->getOption() << endl;
                }
            }
        }
        else
        {
            if (out != 0)
            {
                int fileStatus = data->getFileStatus();
                int isUsable   = data->getUsable();
                int nofMsg     = data->getNofMessage();
                if ((aRemoveError == true)&&(fileStatus != A2BCSTATUS_GETERROR)&&(isUsable > 0))
                {
                    (*out) << data->getDatFileName() << "," << data->getNofMessage() << "," << data->getCurrentMessage() << "," << data->getFileStatus() << "," << data->getFavoriteLevel() << "," << updateTime << "," << data->getDummy2() << "," << data->getDummy3() << "," << data->getUrl() << "," << data->getNickName() << "," << data->getOption() << endl;
//                  (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mFavoriteLevel[index] << "," << updateTime << "," << mDummy2[index] << "," << mDummy3[index] << "," << mUrlData[index] << "," << mNickData[index] << "," << mOptionData[index] << endl;
                }
                else if ((aRemoveOver == true)&&(nofMsg <= 1000)&&(isUsable > 0))
                {
                    (*out) << data->getDatFileName() << "," << data->getNofMessage() << "," << data->getCurrentMessage() << "," << data->getFileStatus() << "," << data->getFavoriteLevel() << "," << updateTime << "," << data->getDummy2() << "," << data->getDummy3() << "," << data->getUrl() << "," << data->getNickName() << "," << data->getOption() << endl;
//                  (*out) << mDatFileName[index] << "," << mNofMessage[index] << "," << mCurrentMessage[index] << "," << mDatFileStatus[index] << "," << mFavoriteLevel[index] << "," << updateTime << "," << mDummy2[index] << "," << mDummy3[index] << "," << mUrlData[index] << "," << mNickData[index] << "," << mOptionData[index] << endl;
                }
                else // if ((aRemoveError == false)&&(aRemoveOver == false))
                {
                    (*out) << data->getDatFileName() << "," << data->getNofMessage() << "," << data->getCurrentMessage() << "," << data->getFileStatus() << "," << data->getFavoriteLevel() << "," << updateTime << "," << data->getDummy2() << "," << data->getDummy3() << "," << data->getUrl() << "," << data->getNickName() << "," << data->getOption() << endl;
                }
            }
        }
    }
    if (out != 0)
    {
        *out << ";[EOF]" << endl;
        out->flush();
        delete out;
        out = 0;
    }
    if (indexFile != 0)
    {
           indexFile->close();
           delete indexFile;
           indexFile = 0;
    }
    clear();
    return;
}


/*!
 *  管理データをクリアする
 * 
 */
void a2BCfavoriteParser::clear()
{
    int limit = mFavoriteData.size();
    for (int loop = limit - 1; loop >= 0; loop--)
    {
        delete mFavoriteData[loop];
    }
    mFavoriteData.clear();
/**
    mIndexFileName.clear();
    mDatFileName.clear();
    mUrlData.clear();
    mNickData.clear();
    mOptionData.clear();
    
    mDatFileStatus.clear();
    mNofMessage.clear();
    mCurrentMessage.clear();
    mFavoriteLevel.clear();
    mLastUpdate.clear();
    mDummy2.clear();
    mDummy3.clear();
    mUsable.clear();
**/
    return;
}

/*!
 * 
 * 
 * 
 */
QString a2BCfavoriteParser::getIndexFileName(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return ("");
    }
//    return (mIndexFileName[index]);
    return ((mFavoriteData[index])->getIndexFileName());
}

/*!
 * 
 * 
 * 
 */
QString a2BCfavoriteParser::getFileName(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return ("");
    }
//    return (mDatFileName[index]);
    return ((mFavoriteData[index])->getDatFileName());
}

/*!
 * 
 * 
 * 
 */
QString a2BCfavoriteParser::getOptionData(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return ("");
    }
//    return (mOptionData[index]);
    return ((mFavoriteData[index])->getOption());
}

/*!
 * 
 * 
 * 
 */
QString a2BCfavoriteParser::getUrl(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return ("");
    }
//    return (mUrlData[index]);
    return ((mFavoriteData[index])->getUrl());
}

/*!
 * 
 * 
 * 
 */
int a2BCfavoriteParser::getNofMessage(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return (0);
    }
//    return (mNofMessage[index]);
    return ((mFavoriteData[index])->getNofMessage());
}

/*!
 * 
 * 
 * 
 */
int a2BCfavoriteParser::getCurrentMessage(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return (0);
    }
//    return (mCurrentMessage[index]);
    return ((mFavoriteData[index])->getCurrentMessage());
}

/*!
 * 
 * 
 * 
 */
int a2BCfavoriteParser::getFavoriteLevel(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return (0);
    }
//    return (mFavoriteLevel[index]);
    return ((mFavoriteData[index])->getFavoriteLevel());
}

/*!
 * 
 * 
 * 
 */
quint64 a2BCfavoriteParser::getLastUpdate(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return (0);
    }
    return ((mFavoriteData[index])->getLastUpdate());
//    return (mLastUpdate[index]);
}

/*!
 * 
 * 
 * 
 */
void a2BCfavoriteParser::setLastUpdate(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return;
    }
    QDateTime datetime = QDateTime::currentDateTime();
    quint64 currentTime = (quint64) datetime.toTime_t();
    currentTime = currentTime * 1000;
    (mFavoriteData[index])->setLastUpdate(currentTime);
//    mLastUpdate[index] = currentTime;
    return;
}

/*!
 * 
 * 
 * 
 */
int a2BCfavoriteParser::getFileStatus(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return (0);
    }
    return ((mFavoriteData[index])->getFileStatus());
}

/*!
 * 
 * 
 * 
 */
void a2BCfavoriteParser::setFileStatus(int index, int status)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return;
    }
    int datStatus = (mFavoriteData[index])->getFileStatus();
    if (status == A2BCSTATUS_UNKNOWN)
    {
        if ((datStatus != A2BCSTATUS_GETERROR)&&
            (datStatus != A2BCSTATUS_NOTYET)&&
            (datStatus != A2BCSTATUS_RESERVE))
        {
            // エラー以外の時には状態を更新しない
            return;
        }
        status = A2BCSTATUS_NEW;
    }  
//    mDatFileStatus[index] = status;
    (mFavoriteData[index])->setFileStatus(status);

    setLastUpdate(index);
    return;
}

/*!
 * 
 * 
 * 
 */
void a2BCfavoriteParser::setNofMessage(int index, int number)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return;
    }
    if (number <= 0)
    {
        number = 1;
    }
    if (number > 1000)
    {
        number = 1001;
    }    
//    mNofMessage[index] = number;
    (mFavoriteData[index])->setNofMessage(number);

    return;
}

/*!
 * 
 * 
 * 
 */
void a2BCfavoriteParser::setCurrentMessage(int index, int number)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return;
    }
    if (number <= 0)
    {
        number = 1;
    }
    if (number > 1000)
    {
        number = 1001;
    }
//    mCurrentMessage[index] = number;
    (mFavoriteData[index])->setCurrentMessage(number);
    return;
}

/*!
 * 
 * 
 */
int a2BCfavoriteParser::exists(QString &arNick, QString &arDatFileName)
{
    int limit = mFavoriteData.size();
    for (int index = 0; index < limit; index++)
    {
        if (((mFavoriteData[index])->getDatFileName() == arDatFileName)&&(((mFavoriteData[index])->getNickName()).contains(arNick) == true))
//        if ((mDatFileName[index] == arDatFileName)&&(mNickData[index].contains(arNick) == true))
        {
            if ((mFavoriteData[index])->getUsable() < 0)
//            if (mUsable[index] < 0)
            {
                return (-10);
            }
            return (index);
        }
    }
    return (-1);
}

/*!
 * 
 * 
 */
void  a2BCfavoriteParser::updateUrl(const QString &arBoardNick, const QString &arBoardUrl)
{
    int limit = mFavoriteData.size();
    for (int index = 0; index < limit; index++)
    {
        if (((mFavoriteData[index])->getNickName()).contains(arBoardNick) == true)
//        if (mNickData[index].contains(arBoardNick) == true)
        {
            QString url = arBoardUrl;
            (mFavoriteData[index])->setUrl(url);
//            mUrlData[index] = arBoardUrl;
        }
    }
    return;    
}

/*!
 * 
 * 
 * 
 */
void a2BCfavoriteParser::remove(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return;
    }
    (mFavoriteData[index])->setUsable(-1);
//    mUsable[index] = -1;

    return;
}

/*!
 *  お気に入り情報のオブジェクトを応答する
 * 
 */
a2BCfavoriteData *a2BCfavoriteParser::getFavoriteObject(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return (0);
    }
    return (mFavoriteData[index]);
}

/*!
 * 
 * 
 * 
 */
void a2BCfavoriteParser::markIt(int index)
{
    if ((index >= mFavoriteData.size())||(index < 0))
    {
        return;
    }
    (mFavoriteData[index])->setUsable(1);
//    mUsable[index] = 1;

    return;
}

/*!
 * 
 * 
 */
int a2BCfavoriteParser::count(void)
{
    return (mFavoriteData.size());
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
