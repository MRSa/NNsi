/*
 *  $Id: threadIndexRecord.cpp,v 1.2 2005/06/14 15:02:57 mrsa Exp $
 *
 *    スレインデックスのデータ...
 *     (共通のデータ保持クラス)
 *
 */
#include <windows.h>
#include "threadIndexRecord.h"

/*
 *   コンストラクタ
 *
 */
threadIndexRecord::threadIndexRecord(void)
{
	initializeSelf();
}

/*
 *   デストラクタ
 *
 */
threadIndexRecord::~threadIndexRecord(void)
{

}

/*
 *    クラスの初期化を実施する
 *
 */
void threadIndexRecord::initializeSelf(void)
{
	currentNumber  = 0;      // 現在の板での位置 
	fileName       = "";     // スレファイル名(*.dat)
	threadTitle    = "";     // スレタイトル
	resCount       = 0;      // 現在のレス数
	fileSize       = 0;      // スレファイルサイズ
	roundDate      = 0;      // 巡回時間
	lastModified   = 0;      // 最終更新時間
	readResNumber  = 0;      // ”ここまで読んだ”
	newReceivedRes = 0;      // 新着レス数
	reserved       = 0;      // 予約(ゼロ固定)
	readUnread     = 0;      // 読んだ/読んでいない
	scrollTop      = 0;      // スクロールトップ??
	allResCount    = 0;      // 現在の全レス数?
	newResCount    = 0;      // 新着レス数？
	ageSage        = 0;      // ageSage??
	currentResNumber = 0;    // 現在メッセージを読んでいる位置
	return;
}

/*
 *  setter:スレ番号を格納
 *
 */
void threadIndexRecord::setCurrentNumber(DWORD aNumber)
{
	currentNumber = aNumber;
	return;
}

/*
 *  setter:スレファイル名を格納
 *
 */
void threadIndexRecord::setFileName(char *apFileName)
{
	fileName = apFileName;
	return;
}

/*
 *  setter:スレファイル名を格納
 *
 */
void threadIndexRecord::setFileName(std::string &arFileName)
{
	fileName = arFileName;
	return;
}

/*
 *  setter:スレタイトルを格納
 *
 */
void threadIndexRecord::setThreadTitle(char *apThreadTitle)
{
	threadTitle = apThreadTitle;
	return;
}

/*
 *  setter:スレタイトルを格納
 *
 */
void threadIndexRecord::setThreadTitle(std::string &arThreadTitle)
{
	threadTitle = arThreadTitle;
	return;
}

/*
 *  setter
 *
 */
void threadIndexRecord::setResCount(DWORD aNumber)
{
	resCount = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setFileSize(DWORD aNumber)
{
	fileSize = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setRoundDate(DWORD aNumber)
{
	roundDate = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setLastModified(DWORD aNumber)
{
	lastModified = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setReadResNumber(DWORD aNumber)
{
	readResNumber = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setNewReceivedRes(DWORD aNumber)
{
	newReceivedRes = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setReserved(DWORD aNumber)
{
	reserved = aNumber;
	return;
}



/*
 *
 *
 */
void threadIndexRecord::setReadUnread(DWORD aNumber)
{
	readUnread = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setScrollTop(DWORD aNumber)
{
	scrollTop = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setAllResCount(DWORD aNumber)
{
	allResCount = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setNewResCount(DWORD aNumber)
{
	newResCount = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setAgeSage(DWORD aNumber)
{
	ageSage = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setCurrentResNumber(DWORD aNumber)
{
	currentResNumber = aNumber;
	return;
}

/**********/

/*
 *
 *
 */
void threadIndexRecord::getCurrentNumber(DWORD &arNumber)
{
	arNumber = currentNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getFileName(std::string &arFileName)
{
	arFileName = fileName;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getThreadTitle(std::string &arThreadTitle)
{
	arThreadTitle = threadTitle;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getResCount(DWORD &arNumber)
{
	arNumber = resCount;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getFileSize(DWORD &arNumber)
{
	arNumber = fileSize;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getRoundDate(DWORD &arNumber)
{
	arNumber = roundDate;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getLastModified(DWORD &arNumber)
{
	arNumber = lastModified;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getReadResNumber(DWORD &arNumber)
{
	arNumber = readResNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getNewReceivedRes(DWORD &arNumber)
{
	arNumber = newReceivedRes;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getReserved(DWORD &arNumber)
{
	arNumber = reserved;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getReadUnread(DWORD &arNumber)
{
	arNumber = readUnread;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getScrollTop(DWORD &arNumber)
{
	arNumber = scrollTop;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getAllResCount(DWORD &arNumber)
{
	arNumber = allResCount;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getNewResCount(DWORD &arNumber)
{
	arNumber = newResCount;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getAgeSage(DWORD &arNumber)
{
	arNumber = ageSage;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getCurrentResNumber(DWORD &arNumber)
{
	arNumber = currentResNumber;
	return;
}
