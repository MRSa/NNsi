/*
 *  $Id: ThreadInfo.h,v 1.1 2005/06/26 14:33:56 mrsa Exp $
 *
 *    ƒXƒŒî•ñ‚ğ•Û‚·‚éƒNƒ‰ƒX...
 *
 */
#pragma once

#include <string>

class ThreadInfo
{
public:
	ThreadInfo(void);
	virtual ~ThreadInfo(void);

public:
	void setKey  (std::string &arKey);
	void setNick (std::string &arNick);
	void setTitle(std::string &arTitle);

	void getKey  (std::string &arKey);
	void getNick (std::string &arNick);
	void getTitle(std::string &arTitle);

private:
	std::string mKey;
	std::string mBoardNick;
	std::string mThreadTitle;
};
