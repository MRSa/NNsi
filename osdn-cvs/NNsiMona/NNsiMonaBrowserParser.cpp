/*
 *  $Id: NNsiMonaBrowserParser.cpp,v 1.5 2005/06/26 14:33:56 mrsa Exp $
 *
 *    ギコナビのスレインデックスを解析するクラス
 *
 */
#include "NNsiMonaBrowserParser.h"
#include "gikoNaviIndexParser.h"

/*
 *   コンストラクタ
 *
 *
 */
NNsiMonaBrowserParser::NNsiMonaBrowserParser(void)
{
	mBBSparser.clear();
	mFavoriteBoard.clear();
	mFavoriteThread.clear();
	mFavoriteThreadURL.clear();
}

/*
 *   デストラクタ
 *
 *
 */
NNsiMonaBrowserParser::~NNsiMonaBrowserParser(void)
{
	mBBSparser.clear();
	mFavoriteBoard.clear();
	mFavoriteThread.clear();
	mFavoriteThreadURL.clear();
}


/*
 *   ベースパスの設定...
 *
 *
 */
void NNsiMonaBrowserParser::setBrowserLocation(char *apBrowserFileLocation)
{
	mBasePath = apBrowserFileLocation;
	return;
}

/*
 *   設定パスの取得...
 *
 *
 */
void NNsiMonaBrowserParser::getBrowserConfigLocation(std::string &arBrowserConfigLocation)
{
	arBrowserConfigLocation = mConfigPath;
	return;
}

/*
 *   設定パスの設定...
 *
 *
 */
void NNsiMonaBrowserParser::setBrowserConfigLocation(char *apBrowserConfigLocation)
{
	mConfigPath = apBrowserConfigLocation;
	return;
}

/*
 *   ベースパスの取得...
 *
 *
 */
void NNsiMonaBrowserParser::getBasePath(std::string &arpBasePath)
{
	arpBasePath = mBasePath;
	return;
}

/*
 *   解析の準備を実施する
 *
 *
 */
void NNsiMonaBrowserParser::prepare(void)
{
	// ギコナビ用の解析を実施...
	prepareGikoNavi();

	// ギコナビ用のお気に入り板リスト・スレッドリストを作成する
	prepareFavoriteGikoNavi();

	return;
}

/*
 *   解析の準備を実施する (ギコナビ用)
 *
 *
 */
void NNsiMonaBrowserParser::prepareGikoNavi(void)
{
	HANDLE           hDir;
	WIN32_FIND_DATA  w32fd;

	char bottom = *(mBasePath.c_str() + strlen(mBasePath.c_str()) - 1);
	if (bottom != '\\')
	{
		mBasePath = mBasePath + "\\2ch\\";
	}
	else
	{
		mBasePath = mBasePath + "2ch\\";
	}

	std::string path = mBasePath + "*.*";

	hDir = FindFirstFile(path.c_str(), &w32fd);
	if (hDir != INVALID_HANDLE_VALUE)
	{
		if (w32fd.cFileName[0] != '.')
		{
			// マップに登録...
			gikoNaviIndexParser indexParser;
			std::string folder = mBasePath + w32fd.cFileName + "\\Folder.idx";
			std::string bbs = w32fd.cFileName;
			bbs = bbs + "/";
			indexParser.setBoardNickName(bbs);
			indexParser.setIndexFileName(folder);
			mBBSparser.insert(pair<std::string,gikoNaviIndexParser>(bbs, indexParser));
		}
		while (FindNextFile(hDir, &w32fd))
		{
			if (w32fd.cFileName[0] != '.')
			{
				// マップに登録...
				gikoNaviIndexParser indexParser;
				std::string folder = mBasePath + w32fd.cFileName + "\\Folder.idx";
				std::string bbs = w32fd.cFileName;
				bbs = bbs + "/";
				indexParser.setBoardNickName(bbs);
				indexParser.setIndexFileName(folder);
				mBBSparser.insert(pair<std::string,gikoNaviIndexParser>(bbs, indexParser));
			}
		}
	}

	// インデックスファイルデータを読み込んでメモリに展開する
	std::map<std::string,gikoNaviIndexParser>::iterator it;
	for (it = mBBSparser.begin(); it != mBBSparser.end(); it++)
	{
		gikoNaviIndexParser *parser = &(it->second);
		parser->parseIndex();
	}

	return;
}

/*!
 *   ファイル名からインデックスデータ情報クラスを取得する
 *
 */
bool NNsiMonaBrowserParser::getIndexRecord(std::string &arBoardNick, std::string &arFileName, threadIndexRecord *&arpIndexRecord)
{
	std::map<std::string,gikoNaviIndexParser>::iterator ite;
	ite = mBBSparser.find(arBoardNick);
	if (ite == mBBSparser.end())
	{
		return (false);
	}

	gikoNaviIndexParser *parser = &(ite->second);
	return (parser->getIndexRecord(arFileName, arpIndexRecord));
}

/*
 *  インデックスレコードを追加登録する
 *
 */
bool NNsiMonaBrowserParser::entryIndexRecord(std::string &arBoardNick, threadIndexRecord *&arpIndexRecord)
{
	std::map<std::string,gikoNaviIndexParser>::iterator ite;
	ite = mBBSparser.find(arBoardNick);
	if (ite == mBBSparser.end())
	{
		return (false);
	}

	gikoNaviIndexParser *parser = &(ite->second);
	return (parser->entryIndexRecord(arpIndexRecord));
}

/*
 * ログのセパレータを取得する
 *
 */
void NNsiMonaBrowserParser::getSeparatorData(char *apSeparator)
{
	apSeparator[0] = '\x0d';
	apSeparator[1] = '\x0a';
	apSeparator[2] = '\0';

	return;
}

/*
 *  インデックスファイルをすべて更新する... 
 *
 *
 */
bool NNsiMonaBrowserParser::updateIndexRecords(void)
{
	std::map<std::string,gikoNaviIndexParser>::iterator ite;
	for (ite = mBBSparser.begin(); ite != mBBSparser.end(); ite++)
	{
		gikoNaviIndexParser *parser = &(ite->second);
		parser->outputIndexFile();
	}
	return (true);
}

/*
 *  お気に入り板リスト・スレリストを作成する
 *
 *
 */
bool NNsiMonaBrowserParser::prepareFavoriteGikoNavi(void)
{
	// お気に入りのファイル名を設定する
	std::string favoritePath = mConfigPath + "\\config\\Favorite.xml";

	// ファイルをオープンする (読み)
	HANDLE hFile = CreateFile(favoritePath.c_str(),
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
		prepareFavoriteBoardGikoNavi(buffer, readFileSize);
		prepareFavoriteThreadGikoNavi(buffer, readFileSize);
	}
	delete[] buffer;

#if 0
	// デバッグ用...お気に入り板一覧を表示する
	debugBoardList();
#endif

	return (false);
}

/*
 *  お気に入り板リストを作成する
 *
 *
 */
bool NNsiMonaBrowserParser::prepareFavoriteBoardGikoNavi(char *apBuffer, long aSize)
{
	//MessageBox(NULL, "NNsiMonaBrowserParser::prepareFavoriteBoardGikoNavi()", "Information", MB_OK);
	mFavoriteBoard.clear();

	char *ptr = NULL;
	char *parsePtr = apBuffer;
	char buffer[1024];
	while (parsePtr < (apBuffer + aSize))
	{
		// 
		ptr = strstr(parsePtr, "favtype=\"board\"");
		if (ptr == NULL)
		{
			break;
		}
		ptr = ptr + sizeof("favtype=\"board\"") - 1;
		char *urlPtr = strstr(ptr, "http://");
		if (urlPtr == NULL)
		{
			break;
		}

		// URLを切り出す...
		memset(buffer, 0x00, sizeof(buffer));
		char *cpyPtr  = buffer;
		char *keyPtr  = buffer + sizeof("http://");
		char *prevKey = buffer + sizeof("http://");
		while ((*urlPtr != '\"')&&(*urlPtr != '\0'))
		{
			*cpyPtr = *urlPtr;
			if (*cpyPtr == '/')
			{
				keyPtr  = prevKey;
				prevKey = (cpyPtr + 1);
			}
			cpyPtr++;
			urlPtr++;
		}
		parsePtr = urlPtr;

		// favorite bbs mapに格納する
		std::string urlString = buffer;
		std::string keyString = keyPtr;
		mFavoriteBoard.insert(pair<std::string,std::string>(keyString, urlString));
	}
	return (false);
}

/*
 *  お気に入りスレッドリストを作成する (GikoNavi用)
 *
 */
bool NNsiMonaBrowserParser::prepareFavoriteThreadGikoNavi(char *apBuffer, long aSize)
{
	//MessageBox(NULL, "NNsiMonaBrowserParser::prepareFavoriteThreadGikoNavi()", "Information", MB_OK);
	mFavoriteBoard.clear();

	char *ptr = NULL;
	char *parsePtr = apBuffer;
	char buffer[1024], title[1024], key[1024];
	while (parsePtr < (apBuffer + aSize))
	{
		// 
		ptr = strstr(parsePtr, "favtype=\"thread\"");
		if (ptr == NULL)
		{
			break;
		}
		ptr = ptr + sizeof("favtype=\"thread\"") - 1;
		char *urlPtr = strstr(ptr, "http://");
		if (urlPtr == NULL)
		{
			break;
		}

		// URLを切り出す...
		memset(buffer, 0x00, sizeof(buffer));
		char *cpyPtr    = buffer;
		char *keyPtr    = buffer + sizeof("http://");
		char *prevKey   = keyPtr;
		char *prepreKey = keyPtr;
		while ((*urlPtr != '\"')&&(*urlPtr != '\0'))
		{
			*cpyPtr = *urlPtr;
			if (*cpyPtr == '/')
			{
				keyPtr    = prevKey;
				prevKey   = prepreKey;
				prepreKey = (cpyPtr + 1);
			}
			cpyPtr++;
			urlPtr++;
		}

		// titleを切り出す
		char *titlePtr = strstr(ptr, "title=\"");
		if (titlePtr == NULL)
		{
			break;
		}
		titlePtr = titlePtr + sizeof("title=\"") - 1;

		memset(title, 0x00, sizeof(title));
		cpyPtr  = title;
		while ((*titlePtr != '\"')&&(*titlePtr != '\0'))
		{
			*cpyPtr = *titlePtr;
			cpyPtr++;
			titlePtr++;
		}

		// 次のデータの先頭へ移動...
		if (titlePtr < prepreKey)
		{
			parsePtr = prepreKey;
		}
		else
		{
			parsePtr = titlePtr;
		}


		// お気に入りとして認識するのは、２ちゃんねるのみ...
		if ((strstr(buffer, "2ch.net") != NULL)||(strstr(buffer, "bbspink.com") != NULL))
		{
			// keyを抜き出す
			memset (key, 0x00, sizeof(key));
			memmove(key, keyPtr, (prepreKey - keyPtr));
        
			char *keySepa = key;
			while (*keySepa != '\0')
			{
				if (*keySepa == '/')
				{
					keySepa++;
					break;
				}
				keySepa++;
			}

			// ログファイル名を抜き出す
			char *datSepa = keySepa;
			while (*datSepa != '\0')
			{
				if (*datSepa == '/')
				{
					*datSepa = '.';
					datSepa++;
					*datSepa = 'd';
					datSepa++;
					*datSepa = 'a';
					datSepa++;
					*datSepa = 't';
					datSepa++;
					*datSepa = '\0';
					datSepa++;
					break;
				}
				datSepa++;
			}

			// favorite thread mapに格納する
			std::string keyString   = keySepa;
			*keySepa = '\0';
			std::string titleString = title;
			std::string bbsNick     = key;
			mFavoriteThread.insert   (pair<std::string,std::string>(keyString, titleString));
			mFavoriteThreadURL.insert(pair<std::string,std::string>(keyString, bbsNick));

#if 0
			// デバッグ表示...
			memset(buffer, 0x00, sizeof(buffer));
			sprintf(buffer, 
				    "NNsiMonaBrowserParser::prepareFavoriteThreadGikoNavi()\n"
					"KEY : %s [%s]\nTITLE : %s\n",
				    keyString.c_str(), 
					bbsNick.c_str(), 
					titleString.c_str());

			MessageBox(NULL, buffer, "Information", MB_OK);
#endif
		}
	}
	return (false);
}

/*!
 * 
 *
 */
void NNsiMonaBrowserParser::debugBoardList(void)
{
	std::map<std::string,std::string>::iterator ite;
	for (ite = mFavoriteBoard.begin(); ite != mFavoriteBoard.end(); ite++)
	{
		char buffer[1024];
		sprintf(buffer, "key: '%s' (URL: %s)", (ite->first).c_str(), (ite->second).c_str());
		MessageBox(NULL, buffer, "Information", MB_OK);
	}
}

/*
 *  追加するスレ情報を取得する
 *
 */
long NNsiMonaBrowserParser::getThreadRecordToAppend(std::map<std::string,std::string> &arThreadMap, std::list<ThreadInfo> &arAppendList)
{
	return (0);
}
