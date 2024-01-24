/*
 *  $Id: NNsiMonaBrowserParser.h,v 1.5 2005/06/26 14:33:56 mrsa Exp $
 *
 *    �M�R�i�r�̃X���C���f�b�N�X����͂���N���X
 *
 */
#pragma once

#include <map>
#include "I2chBrowserIndexFileParser.h"
#include "I2chBrowserParser.h"
#include "gikoNaviIndexParser.h"

class NNsiMonaBrowserParser : public I2chBrowserParser
{
public:
	// �R���X�g���N�^
	NNsiMonaBrowserParser(void);

	// �f�X�g���N�^
	virtual ~NNsiMonaBrowserParser(void);

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

public:
	// �ǉ�����X�������擾����
	long getThreadRecordToAppend(std::map<std::string,std::string> &arThreadMap, std::list<ThreadInfo> &arAppendList);

private:
	// ��͂̏��������s����(�M�R�i�r�p)
	void prepareGikoNavi(void);

	// ���C�ɓ�����X�g�E�X���b�h���X�g���쐬����(�M�R�i�r�p)
	bool NNsiMonaBrowserParser::prepareFavoriteGikoNavi(void);

	// ���C�ɓ�����X�g���쐬����(�M�R�i�r�p)
	bool prepareFavoriteBoardGikoNavi(char *apBuffer, long aSize);

	// ���C�ɓ���X���b�h���X�g���쐬����(�M�R�i�r�p)
	bool prepareFavoriteThreadGikoNavi(char *apBuffer, long aSize);

	// �f�o�b�O�p�{�[�h���X�g�̕\��
	void debugBoardList(void);

private:
	std::string                                      mBasePath;
	std::string										 mConfigPath;
	std::map<std::string,gikoNaviIndexParser>        mBBSparser;
	std::map<std::string,std::string>                mFavoriteBoard;
	std::map<std::string,std::string>                mFavoriteThread;
	std::map<std::string,std::string>                mFavoriteThreadURL;

};
