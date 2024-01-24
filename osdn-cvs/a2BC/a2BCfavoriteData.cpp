#include "a2BCfavoriteData.h"

/*!
 *  コンストラクタ
 * 
 * 
 */
a2BCfavoriteData::a2BCfavoriteData(QString &arIndexFileName, QString &arDatFileName, int maxMsg, int curMsg, int status, int level, quint64 lastUpdate, int dummy2, int dummy3, QString &arUrl, QString &arNick, QString &arOption, int usable, QWidget *parent)
 : QWidget(parent)
{
    mIndexFileName  = arIndexFileName;
    mDatFileName    = arDatFileName;
    mUrlData        = arUrl;
    mNickData       = arNick;
    mOptionData     = arOption;
    mDatFileStatus  = status;
    mNofMessage     = maxMsg;
    mCurrentMessage = curMsg;
    mFavoriteLevel  = level;
    mLastUpdate     = lastUpdate;
    mDummy2         = dummy2;
    mDummy3         = dummy3;
    mUsable         = usable;
}

/*!
 *   デストラクタ
 * 
 * 
 */
a2BCfavoriteData::~a2BCfavoriteData()
{

}

/*!
 * 
 * 
 */
QString  a2BCfavoriteData::getIndexFileName()
{
    return (mIndexFileName);
}

/*!
 * 
 * 
 */
QString  a2BCfavoriteData::getDatFileName()
{
    return (mDatFileName);
}

/*!
 * 
 * 
 */
QString  a2BCfavoriteData::getUrl()
{
    return (mUrlData);
}

/*!
 * 
 * 
 */
QString  a2BCfavoriteData::getNickName()
{
    return (mNickData);
}

/*!
 * 
 * 
 */
QString  a2BCfavoriteData::getOption()
{
    return (mOptionData);
}

/*!
 * 
 * 
 */
int a2BCfavoriteData::getFileStatus()
{
    return (mDatFileStatus);
}

/*!
 * 
 * 
 */
int     a2BCfavoriteData::getNofMessage()
{
    return (mNofMessage);
}

/*!
 * 
 * 
 */
int     a2BCfavoriteData::getCurrentMessage()
{
    return (mCurrentMessage);
}

/*!
 * 
 * 
 */
int     a2BCfavoriteData::getFavoriteLevel()
{
    return (mFavoriteLevel);
}

/*!
 * 
 * 
 */
quint64 a2BCfavoriteData::getLastUpdate()
{
    return (mLastUpdate);
}

/*!
 * 
 * 
 */
int     a2BCfavoriteData::getDummy2()
{
    return (mDummy2);
}

/*!
 * 
 * 
 */
int     a2BCfavoriteData::getDummy3()
{
    return (mDummy3);
}

/*!
 * 
 * 
 */
int     a2BCfavoriteData::getUsable()
{
    return (mUsable);
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setIndexFileName(QString &arIndexFileName)
{
    mIndexFileName = arIndexFileName;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setDatFileName(QString &arFileName)
{
    mDatFileName = arFileName;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setUrl(QString &arUrl)
{
    mUrlData = arUrl;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setNickName(QString &arNickName)
{
    mNickData = arNickName;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setOption(QString &arOption)
{
    mOptionData = arOption;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setFileStatus(int status)
{
    mDatFileStatus = status;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setNofMessage(int nofMessage)
{
    mNofMessage = nofMessage;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setCurrentMessage(int curMessage)
{
    mCurrentMessage = curMessage;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setFavoriteLevel(int level)
{
    mFavoriteLevel = level;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setLastUpdate(quint64 lastUpdate)
{
    mLastUpdate = lastUpdate;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setDummy2(int dummy2)
{
    mDummy2 = dummy2;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setDummy3(int dummy3)
{
    mDummy3 = dummy3;
}

/*!
 * 
 * 
 */
void a2BCfavoriteData::setUsable(int usable)
{
    mUsable = usable;
}

/*
 *  a2BC : a2B Connectivity
 *  Copyright (C) 2006- NNsi Project
 *  (see a2BC-src.txt for detail.)
 */
