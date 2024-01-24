/*
 *  $Id: I2chBrowserParser.h,v 1.3 2005/06/26 14:33:56 mrsa Exp $
 *
 *    �X���C���f�b�N�X����͂���N���X
 *
 */
#pragma once

#include <map>
#include <list>
#include "I2chBrowserIndexFileParser.h"
#include "ThreadInfo.h"

class I2chBrowserParser
{
public:
	// �R���X�g���N�^
	I2chBrowserParser(void) {}

	// �f�X�g���N�^
	virtual ~I2chBrowserParser(void) {}

public:
	// �x�[�X�p�X�̐ݒ�
	virtual void setBrowserLocation(char *apBrowserFileLocation) = 0;

	// �x�[�X�p�X�̎擾
	virtual void getBasePath(std::string &arpBasePath) = 0;

	// �u���E�U�ݒ�p�X�̎擾
	virtual void getBrowserConfigLocation(std::string &arBrowserConfigLocation) = 0;

	// �u���E�U�ݒ�p�X�̐ݒ�
	virtual void setBrowserConfigLocation(char *apBrowserConfigLocation) = 0;

public:
	// ��͂̏��������s����
	virtual void prepare(void) = 0;

	// �C���f�b�N�X���R�[�h���擾����
	virtual bool getIndexRecord(std::string &arBoardNick, std::string &arFileName, threadIndexRecord *&arpIndexRecord) = 0;

	// �C���f�b�N�X���R�[�h��ǉ��o�^����
	virtual bool entryIndexRecord(std::string &arBoardNick, threadIndexRecord *&arpIndexRecord) = 0;

	// ���O�̃Z�p���[�^���擾����
	virtual void getSeparatorData(char *apSeparator) = 0;

	// �C���f�b�N�X�������ׂčX�V����
	virtual bool updateIndexRecords(void) = 0;

public:
	// �ǉ�����X�������擾����
	virtual long getThreadRecordToAppend(std::map<std::string,std::string> &arThreadMap, std::list<ThreadInfo> &arAppendList) = 0;

};
