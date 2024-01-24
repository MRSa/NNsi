/*
 *  $Id: ThreadInfo.cpp,v 1.1 2005/06/26 14:33:56 mrsa Exp $
 *
 *    ƒXƒŒî•ñ‚ğ•Û‚·‚éƒNƒ‰ƒX...
 *
 */
#include "ThreadInfo.h"

ThreadInfo::ThreadInfo(void) : mKey(0), mBoardNick(0), mThreadTitle(0)
{

}

ThreadInfo::~ThreadInfo(void)
{

}

void ThreadInfo::setKey  (std::string &arKey)
{
	mKey = arKey;
}

void ThreadInfo::setNick (std::string &arNick)
{
	mBoardNick = arNick;
}

void ThreadInfo::setTitle(std::string &arTitle)
{
	mThreadTitle = arTitle;
}

void ThreadInfo::getKey  (std::string &arKey)
{
	arKey = mKey;
}

void ThreadInfo::getNick (std::string &arNick)
{
	arNick = mBoardNick;
}

void ThreadInfo::getTitle(std::string &arTitle)
{
	arTitle = mThreadTitle;
}
