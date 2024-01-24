/*
 *  $Id: janeBrowserParser.cpp,v 1.2 2005/06/19 14:40:32 mrsa Exp $
 *
 *    Janeのスレインデックスを解析するクラス
 *
 */
#include "janeBrowserParser.h"

/*
 *   コンストラクタ
 *
 *
 */
janeBrowserParser::janeBrowserParser(void)
{
	mBBSparser.clear();
}

/*
 *   デストラクタ
 *
 *
 */
janeBrowserParser::~janeBrowserParser(void)
{

}


/*
 *   ベースパスの設定...
 *
 *
 */
void janeBrowserParser::setBrowserLocation(char *apBrowserFileLocation)
{
	mBasePath = apBrowserFileLocation;
	return;
}

/*
 *   ベースパスの取得...
 *
 *
 */
void janeBrowserParser::getBasePath(std::string &arpBasePath)
{
	arpBasePath = mBasePath;
	return;
}

/*
 *   設定パスの取得...
 *
 *
 */
void janeBrowserParser::getBrowserConfigLocation(std::string &arBrowserConfigLocation)
{
	arBrowserConfigLocation = mConfigPath;
	return;
}

/*
 *   設定パスの設定...
 *
 *
 */
void janeBrowserParser::setBrowserConfigLocation(char *apBrowserConfigLocation)
{
	mConfigPath = apBrowserConfigLocation;
	return;
}

/*
 *   解析の準備を実施する
 *
 *
 */
void janeBrowserParser::prepare(void)
{
	// Jane用の解析を実施...
	prepareJane();

	return;
}

/*
 *   解析の準備を実施する (Jane用)
 *
 *
 */
void janeBrowserParser::prepareJane(void)
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
			janeIndexParser indexParser;
			std::string folder = mBasePath + w32fd.cFileName + "\\Folder.idx";
			std::string bbs = w32fd.cFileName;
			bbs = bbs + "/";
			indexParser.setBoardNickName(bbs);
			indexParser.setIndexFileName(folder);
			mBBSparser.insert(pair<std::string,janeIndexParser>(bbs, indexParser));
		}
		while (FindNextFile(hDir, &w32fd))
		{
			if (w32fd.cFileName[0] != '.')
			{
				// マップに登録...
				janeIndexParser indexParser;
				std::string folder = mBasePath + w32fd.cFileName + "\\Folder.idx";
				std::string bbs = w32fd.cFileName;
				bbs = bbs + "/";
				indexParser.setBoardNickName(bbs);
				indexParser.setIndexFileName(folder);
				mBBSparser.insert(pair<std::string,janeIndexParser>(bbs, indexParser));
			}
		}
	}

	// インデックスファイルデータを読み込んでメモリに展開する
	std::map<std::string,janeIndexParser>::iterator it;
	for (it = mBBSparser.begin(); it != mBBSparser.end(); it++)
	{
		janeIndexParser *parser = &(it->second);
		parser->parseIndex();
	}

	return;
}

/*!
 *   ファイル名からインデックスデータ情報クラスを取得する
 *
 */
bool janeBrowserParser::getIndexRecord(std::string &arBoardNick, std::string &arFileName, threadIndexRecord *&arpIndexRecord)
{
	std::map<std::string,janeIndexParser>::iterator ite;
	ite = mBBSparser.find(arBoardNick);
	if (ite == mBBSparser.end())
	{
		return (false);
	}

	janeIndexParser *parser = &(ite->second);
	return (parser->getIndexRecord(arFileName, arpIndexRecord));
}

/*
 *  インデックスレコードを追加登録する
 *
 */
bool janeBrowserParser::entryIndexRecord(std::string &arBoardNick, threadIndexRecord *&arpIndexRecord)
{
	std::map<std::string,janeIndexParser>::iterator ite;
	ite = mBBSparser.find(arBoardNick);
	if (ite == mBBSparser.end())
	{
		return (false);
	}

	janeIndexParser *parser = &(ite->second);
	return (parser->entryIndexRecord(arpIndexRecord));
}

/*
 * ログのセパレータを取得する
 *
 */
void janeBrowserParser::getSeparatorData(char *apSeparator)
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
bool janeBrowserParser::updateIndexRecords(void)
{
	std::map<std::string,janeIndexParser>::iterator ite;
	for (ite = mBBSparser.begin(); ite != mBBSparser.end(); ite++)
	{
		janeIndexParser *parser = &(ite->second);
		parser->outputIndexFile();
	}
	return (true);
}
