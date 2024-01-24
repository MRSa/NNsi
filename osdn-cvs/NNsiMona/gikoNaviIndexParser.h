/*
 *  $Id: gikoNaviIndexParser.h,v 1.1 2005/05/22 09:35:02 mrsa Exp $
 *
 *    ギコナビのスレインデックスを解析するクラス
 *
 */
#pragma once
#include <string>
#include <map>
#include <list>

#include "I2chBrowserIndexFileParser.h"

using namespace std;

class gikoNaviIndexParser : public I2chBrowserIndexFileParser
{
public:
	// コンストラクタ/デストラクタ
	gikoNaviIndexParser(void);
	virtual ~gikoNaviIndexParser(void);

	// setters...
	void setBoardNickName(std::string &arBoardNick);
	void setIndexFileName(std::string &arIndexFileName);
	void setBoardCategory(std::string &arBoardCategory);
	void setBoardName    (std::string &arBoardName);
	void setBoardURL     (std::string &arBoardURL);
	void setBoardType    (int          aBoardType);

	// getters...
	void getBoardNickName(std::string &arBoardNick);
	void getIndexFileName(std::string &arIndexFileName);
	void getBoardCategory(std::string &arBoardCategory);
	void getBoardName    (std::string &arBoardName);
	void getBoardURL     (std::string &arBoardURL);
	void getBoardType    (int         &arBoardType);

public:
	// インデックスファイルの解析
	bool parseIndex(void);

	// インデックスファイルを出力する
	bool outputIndexFile(void);

	// 解析したスレ数を応答する
	int  getThreadCount();

	// インデックスレコードを取得する
	bool getIndexRecord(std::string &arFileName, threadIndexRecord *&arpIndexRecord);

	// インデックスデータ情報クラスを追加する
	bool entryIndexRecord(threadIndexRecord *&arpIndexRecord);

private:
	// インデックスファイルを解析する
	BOOL parseIndexFile(char *apBuffer, const DWORD aDataSize, int &arThreadCount);

	// レコードをスレインデックス格納領域に登録する
	void entryThreadRecord(char *apBuffer, DWORD aDataSize);

	// レコードのインデックスを作成する
	void createRecordIndex(void);

private:
	// ボードニックネーム
	std::string  mBoardNick;

	// インデックスファイル名
	std::string  mIndexFileName;

	// 板カテゴリ
	std::string  mBoardCategory;

	// 板名称
	std::string  mBoardName;

	// 板URL
	std::string  mBoardURL;

	// 板タイプ
	int          mBoardType;

	// 解析完了したかどうか
	bool         mParsed;

	// スレデータベース格納領域
	map<std::string,threadIndexRecord> mThreadDatabase;

	// スレデータベースのインデックス領域
	map<DWORD,threadIndexRecord*>   mDatabaseIndex;
};
