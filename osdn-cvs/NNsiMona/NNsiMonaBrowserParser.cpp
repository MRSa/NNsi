/*
 *  $Id: NNsiMonaBrowserParser.cpp,v 1.5 2005/06/26 14:33:56 mrsa Exp $
 *
 *    �M�R�i�r�̃X���C���f�b�N�X����͂���N���X
 *
 */
#include "NNsiMonaBrowserParser.h"
#include "gikoNaviIndexParser.h"

/*
 *   �R���X�g���N�^
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
 *   �f�X�g���N�^
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
 *   �x�[�X�p�X�̐ݒ�...
 *
 *
 */
void NNsiMonaBrowserParser::setBrowserLocation(char *apBrowserFileLocation)
{
	mBasePath = apBrowserFileLocation;
	return;
}

/*
 *   �ݒ�p�X�̎擾...
 *
 *
 */
void NNsiMonaBrowserParser::getBrowserConfigLocation(std::string &arBrowserConfigLocation)
{
	arBrowserConfigLocation = mConfigPath;
	return;
}

/*
 *   �ݒ�p�X�̐ݒ�...
 *
 *
 */
void NNsiMonaBrowserParser::setBrowserConfigLocation(char *apBrowserConfigLocation)
{
	mConfigPath = apBrowserConfigLocation;
	return;
}

/*
 *   �x�[�X�p�X�̎擾...
 *
 *
 */
void NNsiMonaBrowserParser::getBasePath(std::string &arpBasePath)
{
	arpBasePath = mBasePath;
	return;
}

/*
 *   ��͂̏��������{����
 *
 *
 */
void NNsiMonaBrowserParser::prepare(void)
{
	// �M�R�i�r�p�̉�͂����{...
	prepareGikoNavi();

	// �M�R�i�r�p�̂��C�ɓ�����X�g�E�X���b�h���X�g���쐬����
	prepareFavoriteGikoNavi();

	return;
}

/*
 *   ��͂̏��������{���� (�M�R�i�r�p)
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
			// �}�b�v�ɓo�^...
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
				// �}�b�v�ɓo�^...
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

	// �C���f�b�N�X�t�@�C���f�[�^��ǂݍ���Ń������ɓW�J����
	std::map<std::string,gikoNaviIndexParser>::iterator it;
	for (it = mBBSparser.begin(); it != mBBSparser.end(); it++)
	{
		gikoNaviIndexParser *parser = &(it->second);
		parser->parseIndex();
	}

	return;
}

/*!
 *   �t�@�C��������C���f�b�N�X�f�[�^���N���X���擾����
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
 *  �C���f�b�N�X���R�[�h��ǉ��o�^����
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
 * ���O�̃Z�p���[�^���擾����
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
 *  �C���f�b�N�X�t�@�C�������ׂčX�V����... 
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
 *  ���C�ɓ�����X�g�E�X�����X�g���쐬����
 *
 *
 */
bool NNsiMonaBrowserParser::prepareFavoriteGikoNavi(void)
{
	// ���C�ɓ���̃t�@�C������ݒ肷��
	std::string favoritePath = mConfigPath + "\\config\\Favorite.xml";

	// �t�@�C�����I�[�v������ (�ǂ�)
	HANDLE hFile = CreateFile(favoritePath.c_str(),
		                      GENERIC_READ, 
		                      FILE_SHARE_READ,
							  NULL, 
							  OPEN_EXISTING,
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// �t�@�C���I�[�v���Ɏ��s�B�B�B�I������
		return (false);
	}

	// �t�@�C���T�C�Y���擾����
	DWORD fileSize = GetFileSize(hFile, NULL);

	char *buffer = new char[fileSize + 100];
	if (buffer == NULL)
	{
		// �o�b�t�@���m�ۂ���̂Ɏ��s�A�I������
		CloseHandle(hFile);
		return (false);
	}
	memset(buffer, 0x00, (fileSize + 100));

	// �t�@�C���f�[�^��ǂݍ���
	DWORD readFileSize = 0;
	BOOL ret = ReadFile(hFile, buffer, fileSize, &readFileSize, NULL);
	CloseHandle(hFile);

	// �t�@�C����ǂݍ��񂾌��ʂ������ƃo�b�t�@�Ɋi�[�ł�����...
	if (ret == TRUE)
	{
		// �ǂݍ��񂾃t�@�C���̃C���f�b�N�X��͂����{����
		prepareFavoriteBoardGikoNavi(buffer, readFileSize);
		prepareFavoriteThreadGikoNavi(buffer, readFileSize);
	}
	delete[] buffer;

#if 0
	// �f�o�b�O�p...���C�ɓ���ꗗ��\������
	debugBoardList();
#endif

	return (false);
}

/*
 *  ���C�ɓ�����X�g���쐬����
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

		// URL��؂�o��...
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

		// favorite bbs map�Ɋi�[����
		std::string urlString = buffer;
		std::string keyString = keyPtr;
		mFavoriteBoard.insert(pair<std::string,std::string>(keyString, urlString));
	}
	return (false);
}

/*
 *  ���C�ɓ���X���b�h���X�g���쐬���� (GikoNavi�p)
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

		// URL��؂�o��...
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

		// title��؂�o��
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

		// ���̃f�[�^�̐擪�ֈړ�...
		if (titlePtr < prepreKey)
		{
			parsePtr = prepreKey;
		}
		else
		{
			parsePtr = titlePtr;
		}


		// ���C�ɓ���Ƃ��ĔF������̂́A�Q�����˂�̂�...
		if ((strstr(buffer, "2ch.net") != NULL)||(strstr(buffer, "bbspink.com") != NULL))
		{
			// key�𔲂��o��
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

			// ���O�t�@�C�����𔲂��o��
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

			// favorite thread map�Ɋi�[����
			std::string keyString   = keySepa;
			*keySepa = '\0';
			std::string titleString = title;
			std::string bbsNick     = key;
			mFavoriteThread.insert   (pair<std::string,std::string>(keyString, titleString));
			mFavoriteThreadURL.insert(pair<std::string,std::string>(keyString, bbsNick));

#if 0
			// �f�o�b�O�\��...
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
 *  �ǉ�����X�������擾����
 *
 */
long NNsiMonaBrowserParser::getThreadRecordToAppend(std::map<std::string,std::string> &arThreadMap, std::list<ThreadInfo> &arAppendList)
{
	return (0);
}
