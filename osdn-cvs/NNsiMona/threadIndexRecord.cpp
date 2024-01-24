/*
 *  $Id: threadIndexRecord.cpp,v 1.2 2005/06/14 15:02:57 mrsa Exp $
 *
 *    �X���C���f�b�N�X�̃f�[�^...
 *     (���ʂ̃f�[�^�ێ��N���X)
 *
 */
#include <windows.h>
#include "threadIndexRecord.h"

/*
 *   �R���X�g���N�^
 *
 */
threadIndexRecord::threadIndexRecord(void)
{
	initializeSelf();
}

/*
 *   �f�X�g���N�^
 *
 */
threadIndexRecord::~threadIndexRecord(void)
{

}

/*
 *    �N���X�̏����������{����
 *
 */
void threadIndexRecord::initializeSelf(void)
{
	currentNumber  = 0;      // ���݂̔ł̈ʒu 
	fileName       = "";     // �X���t�@�C����(*.dat)
	threadTitle    = "";     // �X���^�C�g��
	resCount       = 0;      // ���݂̃��X��
	fileSize       = 0;      // �X���t�@�C���T�C�Y
	roundDate      = 0;      // ���񎞊�
	lastModified   = 0;      // �ŏI�X�V����
	readResNumber  = 0;      // �h�����܂œǂ񂾁h
	newReceivedRes = 0;      // �V�����X��
	reserved       = 0;      // �\��(�[���Œ�)
	readUnread     = 0;      // �ǂ�/�ǂ�ł��Ȃ�
	scrollTop      = 0;      // �X�N���[���g�b�v??
	allResCount    = 0;      // ���݂̑S���X��?
	newResCount    = 0;      // �V�����X���H
	ageSage        = 0;      // ageSage??
	currentResNumber = 0;    // ���݃��b�Z�[�W��ǂ�ł���ʒu
	return;
}

/*
 *  setter:�X���ԍ����i�[
 *
 */
void threadIndexRecord::setCurrentNumber(DWORD aNumber)
{
	currentNumber = aNumber;
	return;
}

/*
 *  setter:�X���t�@�C�������i�[
 *
 */
void threadIndexRecord::setFileName(char *apFileName)
{
	fileName = apFileName;
	return;
}

/*
 *  setter:�X���t�@�C�������i�[
 *
 */
void threadIndexRecord::setFileName(std::string &arFileName)
{
	fileName = arFileName;
	return;
}

/*
 *  setter:�X���^�C�g�����i�[
 *
 */
void threadIndexRecord::setThreadTitle(char *apThreadTitle)
{
	threadTitle = apThreadTitle;
	return;
}

/*
 *  setter:�X���^�C�g�����i�[
 *
 */
void threadIndexRecord::setThreadTitle(std::string &arThreadTitle)
{
	threadTitle = arThreadTitle;
	return;
}

/*
 *  setter
 *
 */
void threadIndexRecord::setResCount(DWORD aNumber)
{
	resCount = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setFileSize(DWORD aNumber)
{
	fileSize = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setRoundDate(DWORD aNumber)
{
	roundDate = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setLastModified(DWORD aNumber)
{
	lastModified = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setReadResNumber(DWORD aNumber)
{
	readResNumber = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setNewReceivedRes(DWORD aNumber)
{
	newReceivedRes = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setReserved(DWORD aNumber)
{
	reserved = aNumber;
	return;
}



/*
 *
 *
 */
void threadIndexRecord::setReadUnread(DWORD aNumber)
{
	readUnread = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setScrollTop(DWORD aNumber)
{
	scrollTop = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setAllResCount(DWORD aNumber)
{
	allResCount = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setNewResCount(DWORD aNumber)
{
	newResCount = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setAgeSage(DWORD aNumber)
{
	ageSage = aNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::setCurrentResNumber(DWORD aNumber)
{
	currentResNumber = aNumber;
	return;
}

/**********/

/*
 *
 *
 */
void threadIndexRecord::getCurrentNumber(DWORD &arNumber)
{
	arNumber = currentNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getFileName(std::string &arFileName)
{
	arFileName = fileName;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getThreadTitle(std::string &arThreadTitle)
{
	arThreadTitle = threadTitle;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getResCount(DWORD &arNumber)
{
	arNumber = resCount;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getFileSize(DWORD &arNumber)
{
	arNumber = fileSize;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getRoundDate(DWORD &arNumber)
{
	arNumber = roundDate;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getLastModified(DWORD &arNumber)
{
	arNumber = lastModified;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getReadResNumber(DWORD &arNumber)
{
	arNumber = readResNumber;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getNewReceivedRes(DWORD &arNumber)
{
	arNumber = newReceivedRes;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getReserved(DWORD &arNumber)
{
	arNumber = reserved;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getReadUnread(DWORD &arNumber)
{
	arNumber = readUnread;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getScrollTop(DWORD &arNumber)
{
	arNumber = scrollTop;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getAllResCount(DWORD &arNumber)
{
	arNumber = allResCount;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getNewResCount(DWORD &arNumber)
{
	arNumber = newResCount;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getAgeSage(DWORD &arNumber)
{
	arNumber = ageSage;
	return;
}

/*
 *
 *
 */
void threadIndexRecord::getCurrentResNumber(DWORD &arNumber)
{
	arNumber = currentResNumber;
	return;
}
