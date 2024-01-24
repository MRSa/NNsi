/*
 *  $Id: janeBrowserParser.h,v 1.2 2005/06/19 14:40:32 mrsa Exp $
 *
 *    janeのスレインデックスを解析するクラス
 *
 */
#pragma once

#include <map>
#include "I2chBrowserIndexFileParser.h"
#include "I2chBrowserParser.h"
#include "janeIndexParser.h"

class janeBrowserParser : public I2chBrowserParser
{
public:
	// コンストラクタ
	janeBrowserParser(void);

	// デストラクタ
	virtual ~janeBrowserParser(void);

	// ベースパスの取得
	void getBasePath(std::string &arpBasePath);

	// ベースパスの設定
	void setBrowserLocation(char *apBrowserFileLocation);

	// 設定パスの取得
	void getBrowserConfigLocation(std::string &arBrowserConfigLocation);

	// 設定パスの設定
	void setBrowserConfigLocation(char *apBrowserConfigLocation);

	// 解析の準備を実行する
	void prepare(void);

	// インデックスレコードを取得する
	bool getIndexRecord(std::string &arBoardNick, std::string &arFileName, threadIndexRecord *&arpIndexRecord);

	// インデックスレコードを追加登録する
	bool entryIndexRecord(std::string &arBoardNick, threadIndexRecord *&arpIndexRecord);

	// ログのセパレータを取得する
	void getSeparatorData(char *apSeparator);

	// インデックス情報をすべて更新する
	bool updateIndexRecords(void);

private:
	// 解析の準備を実行する(Jane用)
	void prepareJane(void);

private:
	std::string                                      mBasePath;
	std::string										 mConfigPath;
	std::map<std::string,janeIndexParser>            mBBSparser;
};
