/*
 *  $Id: janeIndexParser.h,v 1.1 2005/06/15 15:13:58 mrsa Exp $
 *
 *    Jane�̃X���C���f�b�N�X����͂���N���X
 *
 */
#pragma once
#include <string>
#include <map>
#include <list>

#include "I2chBrowserIndexFileParser.h"

using namespace std;

class janeIndexParser : public I2chBrowserIndexFileParser
{
public:
	janeIndexParser(void);
	virtual ~janeIndexParser(void);

public:
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
	// �C���f�b�N�X�t�@�C���̉��
	bool parseIndex(void);

	// �C���f�b�N�X�t�@�C�����o�͂���
	bool outputIndexFile(void);

	// ��͂����X��������������
	int  getThreadCount();

	// �C���f�b�N�X���R�[�h���擾����
	bool getIndexRecord(std::string &arFileName, threadIndexRecord *&arpIndexRecord);

	// �C���f�b�N�X�f�[�^���N���X��ǉ�����
	bool entryIndexRecord(threadIndexRecord *&arpIndexRecord);

private:
	// �C���f�b�N�X�t�@�C������͂���
	BOOL parseIndexFile(char *apBuffer, const DWORD aDataSize, int &arThreadCount);

	// ���R�[�h���X���C���f�b�N�X�i�[�̈�ɓo�^����
	void entryThreadRecord(char *apBuffer, DWORD aDataSize);

	// ���R�[�h�̃C���f�b�N�X���쐬����
	void createRecordIndex(void);

private:
	// �{�[�h�j�b�N�l�[��
	std::string  mBoardNick;

	// �C���f�b�N�X�t�@�C����
	std::string  mIndexFileName;

	// �J�e�S��
	std::string  mBoardCategory;

	// ����
	std::string  mBoardName;

	// ��URL
	std::string  mBoardURL;

	// �^�C�v
	int          mBoardType;

	// ��͊����������ǂ���
	bool         mParsed;

	// �X���f�[�^�x�[�X�i�[�̈�
	map<std::string,threadIndexRecord> mThreadDatabase;

	// �X���f�[�^�x�[�X�̃C���f�b�N�X�̈�
	map<DWORD,threadIndexRecord*>   mDatabaseIndex;

};
