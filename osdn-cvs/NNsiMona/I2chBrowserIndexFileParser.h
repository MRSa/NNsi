/*
 *  $Id: I2chBrowserIndexFileParser.h,v 1.2 2005/06/05 15:58:03 mrsa Exp $
 *
 *    �X���C���f�b�N�X��͗p�C���^�t�F�[�X�N���X
 *
 */
#pragma once
#include <string>
#include "threadIndexRecord.h"

class I2chBrowserIndexFileParser
{
public:
	I2chBrowserIndexFileParser() {}
	virtual ~I2chBrowserIndexFileParser() {}

public:
	// setters...
	virtual void setBoardNickName(std::string &arBoardNick) = 0;
	virtual void setIndexFileName(std::string &arIndexFileName) = 0;
	virtual void setBoardCategory(std::string &arBoardCategory) = 0;
	virtual void setBoardName    (std::string &arBoardName) = 0;
	virtual void setBoardURL     (std::string &arBoardURL) = 0;
	virtual void setBoardType    (int          aBoardType) = 0;

	// getters...
	virtual void getBoardNickName(std::string &arBoardNick) = 0;
	virtual void getIndexFileName(std::string &arIndexFileName) = 0;
	virtual void getBoardCategory(std::string &arBoardCategory) = 0;
	virtual void getBoardName    (std::string &arBoardName) = 0;
	virtual void getBoardURL     (std::string &arBoardURL) = 0;
	virtual void getBoardType    (int         &arBoardType) = 0;

public:
	// �C���f�b�N�X�t�@�C���̉��
	virtual bool parseIndex(void) = 0;

	// �C���f�b�N�X�t�@�C�����o�͂���
	virtual bool outputIndexFile(void) = 0;

	// ��͂����X��������������
	virtual int  getThreadCount() = 0;

	// �t�@�C��������C���f�b�N�X�f�[�^���N���X���擾����
	virtual bool getIndexRecord(std::string &arFileName, threadIndexRecord *&arpIndexRecord) = 0;

	// �C���f�b�N�X�f�[�^���N���X��ǉ�����
	virtual bool entryIndexRecord(threadIndexRecord *&arpIndexRecord) = 0;

};
