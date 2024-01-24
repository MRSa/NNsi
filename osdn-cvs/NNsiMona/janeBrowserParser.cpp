/*
 *  $Id: janeBrowserParser.cpp,v 1.2 2005/06/19 14:40:32 mrsa Exp $
 *
 *    Jane�̃X���C���f�b�N�X����͂���N���X
 *
 */
#include "janeBrowserParser.h"

/*
 *   �R���X�g���N�^
 *
 *
 */
janeBrowserParser::janeBrowserParser(void)
{
	mBBSparser.clear();
}

/*
 *   �f�X�g���N�^
 *
 *
 */
janeBrowserParser::~janeBrowserParser(void)
{

}


/*
 *   �x�[�X�p�X�̐ݒ�...
 *
 *
 */
void janeBrowserParser::setBrowserLocation(char *apBrowserFileLocation)
{
	mBasePath = apBrowserFileLocation;
	return;
}

/*
 *   �x�[�X�p�X�̎擾...
 *
 *
 */
void janeBrowserParser::getBasePath(std::string &arpBasePath)
{
	arpBasePath = mBasePath;
	return;
}

/*
 *   �ݒ�p�X�̎擾...
 *
 *
 */
void janeBrowserParser::getBrowserConfigLocation(std::string &arBrowserConfigLocation)
{
	arBrowserConfigLocation = mConfigPath;
	return;
}

/*
 *   �ݒ�p�X�̐ݒ�...
 *
 *
 */
void janeBrowserParser::setBrowserConfigLocation(char *apBrowserConfigLocation)
{
	mConfigPath = apBrowserConfigLocation;
	return;
}

/*
 *   ��͂̏��������{����
 *
 *
 */
void janeBrowserParser::prepare(void)
{
	// Jane�p�̉�͂����{...
	prepareJane();

	return;
}

/*
 *   ��͂̏��������{���� (Jane�p)
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
			// �}�b�v�ɓo�^...
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
				// �}�b�v�ɓo�^...
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

	// �C���f�b�N�X�t�@�C���f�[�^��ǂݍ���Ń������ɓW�J����
	std::map<std::string,janeIndexParser>::iterator it;
	for (it = mBBSparser.begin(); it != mBBSparser.end(); it++)
	{
		janeIndexParser *parser = &(it->second);
		parser->parseIndex();
	}

	return;
}

/*!
 *   �t�@�C��������C���f�b�N�X�f�[�^���N���X���擾����
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
 *  �C���f�b�N�X���R�[�h��ǉ��o�^����
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
 * ���O�̃Z�p���[�^���擾����
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
 *  �C���f�b�N�X�t�@�C�������ׂčX�V����... 
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
