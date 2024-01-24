#/*
 *  $Id: janeIndexParser.cpp,v 1.1 2005/06/15 15:13:58 mrsa Exp $
 *
 *    Janeのスレインデックスを解析するクラス
 *
 */
#include <windows.h>
#include "janeIndexParser.h"

/*
 *  コンストラクタ
 *
 *
 */
janeIndexParser::janeIndexParser(void) : mParsed(false)
{
	// データを初期化する...
	mThreadDatabase.clear();
	mDatabaseIndex.clear();
}


/*
 *  デストラクタ
 *
 *
 */
janeIndexParser::~janeIndexParser(void)
{
    // とりあえず何もしない
}

/*
 *  ボードニックネームを設定する
 *
 */
void janeIndexParser::setBoardNickName(std::string &arBoardNick)
{
	mBoardNick = arBoardNick;
	return;
}

/*
 *  インデックスファイル名を指定する
 *
 */
void janeIndexParser::setIndexFileName(std::string &arIndexFileName)
{
	mIndexFileName = arIndexFileName;
	return;
}

/*
 *  板カテゴリを設定する
 *
 */
void janeIndexParser::setBoardCategory(std::string &arBoardCategory)
{
	mBoardCategory = arBoardCategory;
	return;
}

/*
 *  板名称を設定する
 *
 */
void janeIndexParser::setBoardName    (std::string &arBoardName)
{
	mBoardName = arBoardName;
	return;
}

/*
 *  板URLを設定する
 *
 */
void janeIndexParser::setBoardURL     (std::string &arBoardURL)
{
	mBoardURL = arBoardURL;
	return;
}

/*
 *  板タイプを設定する
 *
 *
 */
void janeIndexParser::setBoardType(int aBoardType)
{
	mBoardType = aBoardType;
	return;
}

/*
 *  設定しているボードニックネームを取得する
 *
 */
void janeIndexParser::getBoardNickName(std::string &arBoardNick)
{
	arBoardNick = mBoardNick;

	return;
}

/*
 *  設定しているインデックスファイル名を応答する
 *
 */
void janeIndexParser::getIndexFileName(std::string &arIndexFileName)
{
	arIndexFileName = mIndexFileName;

	return;
}

/*
 *  設定している板カテゴリ名を応答する
 *
 */
void janeIndexParser::getBoardCategory(std::string &arBoardCategory)
{
	arBoardCategory = mBoardCategory;
	return;
}

/*
 *  設定している板名称を応答する
 *
 */
void janeIndexParser::getBoardName(std::string &arBoardName)
{
	arBoardName = mBoardName;
	return;
}

/*
 *  設定している板URLを応答する
 *
 */
void janeIndexParser::getBoardURL     (std::string &arBoardURL)
{
	arBoardURL = mBoardURL;
	return;
}

/*
 *  設定している板タイプを応答する
 *
 */
void janeIndexParser::getBoardType    (int         &arBoardType)
{
	arBoardType = mBoardType;
	return;
}


/*
 *  スレ解析用にインデックスファイルを解析する
 *
 */
bool janeIndexParser::parseIndex(void)
{

	// インデックスファイル名が指定されていないか？
	if (mIndexFileName.length() == 0)
	{
		return (false);
	}

	// 解析状態を初期化する
	mThreadDatabase.clear();
	mDatabaseIndex.clear();
	mParsed = false;

	// ファイルをオープンする (読み)
	HANDLE hFile = CreateFile(mIndexFileName.c_str(),
		                      GENERIC_READ, 
		                      FILE_SHARE_READ,
							  NULL, 
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// ファイルオープンに失敗。。。終了する
		return (false);
	}

	// ファイルサイズを取得する
	DWORD fileSize = GetFileSize(hFile, NULL);

	char *buffer = new char[fileSize + 100];
	if (buffer == NULL)
	{
		// バッファを確保するのに失敗、終了する
		CloseHandle(hFile);
		return (false);
	}
	memset(buffer, 0x00, (fileSize + 100));

	// ファイルデータを読み込む
	DWORD readFileSize = 0;
	BOOL ret = ReadFile(hFile, buffer, fileSize, &readFileSize, NULL);
	CloseHandle(hFile);

	// ファイルを読み込んだ結果がちゃんとバッファに格納できたら...
	if (ret == TRUE)
	{
		// 読み込んだファイルのインデックス解析を実施する
		int threadCount = 0;
		ret = parseIndexFile(buffer, readFileSize, threadCount);
	}
	delete[] buffer;
	return (mParsed);
}


/*
 *   インデックスファイルの解析を実施する
 *
 *
 */
BOOL janeIndexParser::parseIndexFile(char *apBuffer, const DWORD aDataSize, int &arThreadCount)
{
	DWORD parsedSize = 0;
	char *ptr;

	// スレカウントをリセットする
	arThreadCount = 0;

	// ヘッダ分読み飛ばす...
	ptr = strstr(apBuffer, "\x0d\x0a");
	if (ptr == NULL)
	{
		// ヘッダが見つからなかった、、、終了する
		mParsed = false;
		return (FALSE);
	}
	ptr = ptr + sizeof("\x0d\x0a") - 1;

    // スレの解析を実行する
	parsedSize = static_cast<DWORD>(ptr - apBuffer);
	while (parsedSize < aDataSize)
	{
		char *nextPtr;
		nextPtr = strstr(ptr, "\x0d\x0a");
		if (nextPtr == NULL)
		{
			break;
		}

		// レコードデータサイズを求める
		DWORD dataSize = 0;
		dataSize = static_cast<DWORD>((nextPtr + sizeof("\x0d\x0a") - 1) - ptr);

		// スレデータをインデックス格納領域に登録
		entryThreadRecord(ptr, dataSize);
		arThreadCount++;

		parsedSize = parsedSize + ((nextPtr + sizeof("\x0d\x0a") - 1) - ptr);
		ptr = nextPtr + sizeof("\x0d\x0a") - 1;
	}

	// レコードインデックスを生成する
	createRecordIndex();

	mParsed = true;
	return (TRUE);
}

/*
 *   現在抱えているスレ数を取得する
 *
 *
 */
int janeIndexParser::getThreadCount(void)
{
	//return (mThreadDatabase.size());
	return (static_cast<int>(mDatabaseIndex.size()));
}

/*
 *   レコードをスレインデックス格納領域に登録する
 *
 *
 */
void janeIndexParser::entryThreadRecord(char *apBuffer, DWORD aDataSize)
{
	char              debugTemp[512];
	unsigned int      num = 0;
	threadIndexRecord indexRecord;
	char *buffer = new char[aDataSize + 100];
	if (buffer == NULL)
	{
		return;
	}
	memset(buffer, 0x00, (aDataSize + 100));
	memmove(buffer, apBuffer, aDataSize);

	// データを分解する
	char *ptr1 = buffer;
    char *ptr2 = NULL;

	// レス番号の切り出し
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
    *ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "THREAD NUMBER : %s (%d)", ptr1, num);
	indexRecord.setCurrentNumber(num);

	// datファイル名の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	sprintf(debugTemp, "DAT FILE NAME : %s", ptr1);
	indexRecord.setFileName(ptr1);

	// スレタイトルの切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	sprintf(debugTemp, "THREAD TITLE : %s", ptr1);
	indexRecord.setThreadTitle(ptr1);

	// スレッドカウント（ローカル）の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Count : %d", num);
	indexRecord.setResCount(num);

	// スレッドサイズの切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Thread Size : %d", num);
	indexRecord.setFileSize(num);

	// スレッドを取得した日時（巡回日時）の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Round Date : %d (0x%x)", num, num);
	indexRecord.setRoundDate(num);

	// スレッドが更新されている日時（サーバ側日時）の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Last Modified : %d (0x%x)", num, num);
	indexRecord.setLastModified(num);

	// ココまで読んだ番号の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Read Res Number : %d (0x%x)", num, num);
	indexRecord.setReadResNumber(num);

	// ココから新規受信の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "new receive res : %d (0x%x)", num, num);
	indexRecord.setNewReceivedRes(num);

	// 予約(ゼロ固定)の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "reserved : %d (0x%x)", num, num);
	indexRecord.setReserved(num);

	// 未読フラグの切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "read/unread : %d (0x%x)", num, num);
	indexRecord.setReadUnread(num);

	// スクロール位置の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "scrollTop : %d (0x%x)", num, num);
	indexRecord.setScrollTop(num);

	// スレッドカウント（サーバ）の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "allResCount : %d (0x%x)", num, num);
	indexRecord.setAllResCount(num);

	// スレッド新着数の切り出し
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "newResCount : %d (0x%x)", num, num);
	indexRecord.setNewResCount(num);

	// アイテムの上げ下げの切り出し
	ptr1 = ptr2 + 1;
	////////// 注意: 末尾の項目なので、切り出しは不要 /////////
	//ptr2 = strstr(ptr1, "\x01");
	//if (ptr2 == NULL)
	//{
	//	delete[] buffer;
	//	return;
	//}
	//*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "ageSage : %d (0x%x)", num, num);
	indexRecord.setAgeSage(num);

	delete[] buffer;

	// スレインデックスをMAPに格納する
	std::string key;
	indexRecord.getFileName(key);
	mThreadDatabase.insert(pair<std::string, threadIndexRecord>(key, indexRecord));
	return;
}

/*
 *  インデックスデータ情報クラスを追加する
 *
 */
bool janeIndexParser::entryIndexRecord(threadIndexRecord *&arpIndexRecord)
{
	map<std::string, threadIndexRecord>::iterator ite;
	std::string key;
	arpIndexRecord->getFileName(key);

	ite = mThreadDatabase.find(key);
	if (ite != mThreadDatabase.end())
	{
		// 既に登録済み、、、
		return (false);
	}

	// インデックスデータ情報クラスを追加登録する...
	mThreadDatabase.insert(pair<std::string, threadIndexRecord>(key, *arpIndexRecord));
	return (true);
}

// インデックスレコードを取得する
bool janeIndexParser::getIndexRecord(std::string &arFileName, threadIndexRecord *&arpIndexRecord)
{
	map<std::string,threadIndexRecord>::iterator ite;
	ite = mThreadDatabase.find(arFileName);
	if (ite == mThreadDatabase.end())
	{
		// 該当データなし...
		return (false);
	}
	arpIndexRecord = &(ite->second);

	return (true);
}

/*
 *  レコードのインデックスを作成する
 *
 */
void janeIndexParser::createRecordIndex(void)
{
	size_t dataCount = mThreadDatabase.size();

	map<std::string,threadIndexRecord>::iterator it;
	for (it = mThreadDatabase.begin(); it != mThreadDatabase.end(); it++)
	{
		threadIndexRecord *data;
		data = &(it->second);
		DWORD indexNumber = 0;
		data->getCurrentNumber(indexNumber);
		mDatabaseIndex.insert(pair<DWORD, threadIndexRecord*>(indexNumber, &(it->second)));
	}
	return;
}

/*
 *  レコードのインデックスを出力する
 *
 */
bool janeIndexParser::outputIndexFile(void)
{
	// インデックスファイル名が指定されていないか？
	if (mIndexFileName.length() == 0)
	{
		return (false);
	}
	char buffer[8192];

	// バックアップファイルを作成...
	sprintf(buffer, "%s.bak", mIndexFileName.c_str());
	CopyFile(mIndexFileName.c_str(), buffer, FALSE);

	// ファイルをオープンする (書き込み用)
	HANDLE hFile = CreateFile(mIndexFileName.c_str(),
		                      GENERIC_WRITE, 
		                      FILE_SHARE_WRITE,
							  NULL, 
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// ファイルオープンに失敗。。。終了する
		return (false);
	}
	SetEndOfFile(hFile);

	// ヘッダ部分を出力する
	sprintf(buffer, "1.01\x0d\x0a");
	DWORD writeSize = 0;
	DWORD length    = 0;
	length = static_cast<DWORD>(strlen(buffer));
	(void) WriteFile(hFile, buffer, length, &writeSize, NULL);

	// １レコードづつ出力する
	int recordCount = static_cast<int>(mDatabaseIndex.size());
	for (int loop = 1; loop <= recordCount; loop++)
	{
		map<DWORD,threadIndexRecord*>::iterator it;
		it = mDatabaseIndex.find(loop);
		if (it != mDatabaseIndex.end())
		{
			std::string fileName;
			std::string threadTitle;
			DWORD       resCount       = 0;
			DWORD       fileSize       = 0;
			DWORD       roundDate      = 0;
			DWORD       lastUpdate     = 0;
			DWORD       readResNumber  = 0;
			DWORD       newReceivedRes = 0;
			DWORD       reserved       = 0;
			DWORD       readUnread     = 0;
			DWORD       scrollTop      = 0;
			DWORD       allResCount    = 0;
			DWORD       newResCount    = 0;
			DWORD       ageSage        = 0;

			threadIndexRecord *record;
			record = it->second;

			// レコードのデータを取得する
			record->getFileName(fileName);
			record->getThreadTitle(threadTitle);
			record->getResCount(resCount);
			record->getFileSize(fileSize);
			record->getRoundDate(roundDate);
			record->getLastModified(lastUpdate);
			record->getReadResNumber(readResNumber);
			record->getNewReceivedRes(newReceivedRes);
			record->getReserved(reserved);
			record->getReadUnread(readUnread);
			record->getScrollTop(scrollTop);
			record->getAllResCount(allResCount);
			record->getNewResCount(newResCount);
			record->getAgeSage(ageSage);

			// レコードを出力する
			memset(buffer, 0x00, sizeof(buffer));
			sprintf(buffer, "%X\x01%s\x01%s\x01%X\x01%X\x01%X\x01%X\x01%X\x01"
				    "%X\x01%X\x01%X\x01%X\x01%X\x01%X\x01%X\x0d\x0a",
					loop, fileName.c_str(), threadTitle.c_str(), resCount, fileSize,
					roundDate,lastUpdate,readResNumber,newReceivedRes,reserved,
					readUnread,scrollTop,allResCount,newResCount,ageSage);

			writeSize = static_cast<DWORD>(strlen(buffer));
			length    = static_cast<DWORD>(strlen(buffer));
			(void) WriteFile(hFile, buffer, length, &writeSize, NULL);
		}
	}
	CloseHandle(hFile);

	return (true);
}
