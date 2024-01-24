/*
 *  $Id: janeBrowserParser.h,v 1.2 2005/06/19 14:40:32 mrsa Exp $
 *
 *    jane�̃X���C���f�b�N�X����͂���N���X
 *
 */
#pragma once

#include <map>
#include "I2chBrowserIndexFileParser.h"
#include "I2chBrowserParser.h"
#include "janeIndexParser.h"

class janeBrowserParser : public I2chBrowserParser
{
public:
	// �R���X�g���N�^
	janeBrowserParser(void);

	// �f�X�g���N�^
	virtual ~janeBrowserParser(void);

	// �x�[�X�p�X�̎擾
	void getBasePath(std::string &arpBasePath);

	// �x�[�X�p�X�̐ݒ�
	void setBrowserLocation(char *apBrowserFileLocation);

	// �ݒ�p�X�̎擾
	void getBrowserConfigLocation(std::string &arBrowserConfigLocation);

	// �ݒ�p�X�̐ݒ�
	void setBrowserConfigLocation(char *apBrowserConfigLocation);

	// ��͂̏��������s����
	void prepare(void);

	// �C���f�b�N�X���R�[�h���擾����
	bool getIndexRecord(std::string &arBoardNick, std::string &arFileName, threadIndexRecord *&arpIndexRecord);

	// �C���f�b�N�X���R�[�h��ǉ��o�^����
	bool entryIndexRecord(std::string &arBoardNick, threadIndexRecord *&arpIndexRecord);

	// ���O�̃Z�p���[�^���擾����
	void getSeparatorData(char *apSeparator);

	// �C���f�b�N�X�������ׂčX�V����
	bool updateIndexRecords(void);

private:
	// ��͂̏��������s����(Jane�p)
	void prepareJane(void);

private:
	std::string                                      mBasePath;
	std::string										 mConfigPath;
	std::map<std::string,janeIndexParser>            mBBSparser;
};
