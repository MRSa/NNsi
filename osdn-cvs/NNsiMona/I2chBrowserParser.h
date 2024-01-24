/*
 *  $Id: I2chBrowserParser.h,v 1.3 2005/06/26 14:33:56 mrsa Exp $
 *
 *    スレインデックスを解析するクラス
 *
 */
#pragma once

#include <map>
#include <list>
#include "I2chBrowserIndexFileParser.h"
#include "ThreadInfo.h"

class I2chBrowserParser
{
public:
	// コンストラクタ
	I2chBrowserParser(void) {}

	// デストラクタ
	virtual ~I2chBrowserParser(void) {}

public:
	// ベースパスの設定
	virtual void setBrowserLocation(char *apBrowserFileLocation) = 0;

	// ベースパスの取得
	virtual void getBasePath(std::string &arpBasePath) = 0;

	// ブラウザ設定パスの取得
	virtual void getBrowserConfigLocation(std::string &arBrowserConfigLocation) = 0;

	// ブラウザ設定パスの設定
	virtual void setBrowserConfigLocation(char *apBrowserConfigLocation) = 0;

public:
	// 解析の準備を実行する
	virtual void prepare(void) = 0;

	// インデックスレコードを取得する
	virtual bool getIndexRecord(std::string &arBoardNick, std::string &arFileName, threadIndexRecord *&arpIndexRecord) = 0;

	// インデックスレコードを追加登録する
	virtual bool entryIndexRecord(std::string &arBoardNick, threadIndexRecord *&arpIndexRecord) = 0;

	// ログのセパレータを取得する
	virtual void getSeparatorData(char *apSeparator) = 0;

	// インデックス情報をすべて更新する
	virtual bool updateIndexRecords(void) = 0;

public:
	// 追加するスレ情報を取得する
	virtual long getThreadRecordToAppend(std::map<std::string,std::string> &arThreadMap, std::list<ThreadInfo> &arAppendList) = 0;

};
