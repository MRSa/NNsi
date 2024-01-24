/*
 *  $Id: I2chBrowserIndexFileParser.h,v 1.2 2005/06/05 15:58:03 mrsa Exp $
 *
 *    スレインデックス解析用インタフェースクラス
 *
 */
#pragma once
#include <string>
#include "threadIndexRecord.h"

class I2chBrowserIndexFileParser
{
public:
	I2chBrowserIndexFileParser() {}
	virtual ~I2chBrowserIndexFileParser() {}

public:
	// setters...
	virtual void setBoardNickName(std::string &arBoardNick) = 0;
	virtual void setIndexFileName(std::string &arIndexFileName) = 0;
	virtual void setBoardCategory(std::string &arBoardCategory) = 0;
	virtual void setBoardName    (std::string &arBoardName) = 0;
	virtual void setBoardURL     (std::string &arBoardURL) = 0;
	virtual void setBoardType    (int          aBoardType) = 0;

	// getters...
	virtual void getBoardNickName(std::string &arBoardNick) = 0;
	virtual void getIndexFileName(std::string &arIndexFileName) = 0;
	virtual void getBoardCategory(std::string &arBoardCategory) = 0;
	virtual void getBoardName    (std::string &arBoardName) = 0;
	virtual void getBoardURL     (std::string &arBoardURL) = 0;
	virtual void getBoardType    (int         &arBoardType) = 0;

public:
	// インデックスファイルの解析
	virtual bool parseIndex(void) = 0;

	// インデックスファイルを出力する
	virtual bool outputIndexFile(void) = 0;

	// 解析したスレ数を応答する
	virtual int  getThreadCount() = 0;

	// ファイル名からインデックスデータ情報クラスを取得する
	virtual bool getIndexRecord(std::string &arFileName, threadIndexRecord *&arpIndexRecord) = 0;

	// インデックスデータ情報クラスを追加する
	virtual bool entryIndexRecord(threadIndexRecord *&arpIndexRecord) = 0;

};
