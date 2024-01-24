/*
 *  $Id: NNsiMonaBrowserParser.h,v 1.5 2005/06/26 14:33:56 mrsa Exp $
 *
 *    ギコナビのスレインデックスを解析するクラス
 *
 */
#pragma once

#include <map>
#include "I2chBrowserIndexFileParser.h"
#include "I2chBrowserParser.h"
#include "gikoNaviIndexParser.h"

class NNsiMonaBrowserParser : public I2chBrowserParser
{
public:
	// コンストラクタ
	NNsiMonaBrowserParser(void);

	// デストラクタ
	virtual ~NNsiMonaBrowserParser(void);

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

public:
	// 追加するスレ情報を取得する
	long getThreadRecordToAppend(std::map<std::string,std::string> &arThreadMap, std::list<ThreadInfo> &arAppendList);

private:
	// 解析の準備を実行する(ギコナビ用)
	void prepareGikoNavi(void);

	// お気に入り板リスト・スレッドリストを作成する(ギコナビ用)
	bool NNsiMonaBrowserParser::prepareFavoriteGikoNavi(void);

	// お気に入り板リストを作成する(ギコナビ用)
	bool prepareFavoriteBoardGikoNavi(char *apBuffer, long aSize);

	// お気に入りスレッドリストを作成する(ギコナビ用)
	bool prepareFavoriteThreadGikoNavi(char *apBuffer, long aSize);

	// デバッグ用ボードリストの表示
	void debugBoardList(void);

private:
	std::string                                      mBasePath;
	std::string										 mConfigPath;
	std::map<std::string,gikoNaviIndexParser>        mBBSparser;
	std::map<std::string,std::string>                mFavoriteBoard;
	std::map<std::string,std::string>                mFavoriteThread;
	std::map<std::string,std::string>                mFavoriteThreadURL;

};
