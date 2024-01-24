#/*
 *  $Id: janeIndexParser.cpp,v 1.1 2005/06/15 15:13:58 mrsa Exp $
 *
 *    Jane�̃X���C���f�b�N�X����͂���N���X
 *
 */
#include <windows.h>
#include "janeIndexParser.h"

/*
 *  �R���X�g���N�^
 *
 *
 */
janeIndexParser::janeIndexParser(void) : mParsed(false)
{
	// �f�[�^������������...
	mThreadDatabase.clear();
	mDatabaseIndex.clear();
}


/*
 *  �f�X�g���N�^
 *
 *
 */
janeIndexParser::~janeIndexParser(void)
{
    // �Ƃ肠�����������Ȃ�
}

/*
 *  �{�[�h�j�b�N�l�[����ݒ肷��
 *
 */
void janeIndexParser::setBoardNickName(std::string &arBoardNick)
{
	mBoardNick = arBoardNick;
	return;
}

/*
 *  �C���f�b�N�X�t�@�C�������w�肷��
 *
 */
void janeIndexParser::setIndexFileName(std::string &arIndexFileName)
{
	mIndexFileName = arIndexFileName;
	return;
}

/*
 *  �J�e�S����ݒ肷��
 *
 */
void janeIndexParser::setBoardCategory(std::string &arBoardCategory)
{
	mBoardCategory = arBoardCategory;
	return;
}

/*
 *  ���̂�ݒ肷��
 *
 */
void janeIndexParser::setBoardName    (std::string &arBoardName)
{
	mBoardName = arBoardName;
	return;
}

/*
 *  ��URL��ݒ肷��
 *
 */
void janeIndexParser::setBoardURL     (std::string &arBoardURL)
{
	mBoardURL = arBoardURL;
	return;
}

/*
 *  �^�C�v��ݒ肷��
 *
 *
 */
void janeIndexParser::setBoardType(int aBoardType)
{
	mBoardType = aBoardType;
	return;
}

/*
 *  �ݒ肵�Ă���{�[�h�j�b�N�l�[�����擾����
 *
 */
void janeIndexParser::getBoardNickName(std::string &arBoardNick)
{
	arBoardNick = mBoardNick;

	return;
}

/*
 *  �ݒ肵�Ă���C���f�b�N�X�t�@�C��������������
 *
 */
void janeIndexParser::getIndexFileName(std::string &arIndexFileName)
{
	arIndexFileName = mIndexFileName;

	return;
}

/*
 *  �ݒ肵�Ă���J�e�S��������������
 *
 */
void janeIndexParser::getBoardCategory(std::string &arBoardCategory)
{
	arBoardCategory = mBoardCategory;
	return;
}

/*
 *  �ݒ肵�Ă�����̂���������
 *
 */
void janeIndexParser::getBoardName(std::string &arBoardName)
{
	arBoardName = mBoardName;
	return;
}

/*
 *  �ݒ肵�Ă����URL����������
 *
 */
void janeIndexParser::getBoardURL     (std::string &arBoardURL)
{
	arBoardURL = mBoardURL;
	return;
}

/*
 *  �ݒ肵�Ă���^�C�v����������
 *
 */
void janeIndexParser::getBoardType    (int         &arBoardType)
{
	arBoardType = mBoardType;
	return;
}


/*
 *  �X����͗p�ɃC���f�b�N�X�t�@�C������͂���
 *
 */
bool janeIndexParser::parseIndex(void)
{

	// �C���f�b�N�X�t�@�C�������w�肳��Ă��Ȃ����H
	if (mIndexFileName.length() == 0)
	{
		return (false);
	}

	// ��͏�Ԃ�����������
	mThreadDatabase.clear();
	mDatabaseIndex.clear();
	mParsed = false;

	// �t�@�C�����I�[�v������ (�ǂ�)
	HANDLE hFile = CreateFile(mIndexFileName.c_str(),
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
		int threadCount = 0;
		ret = parseIndexFile(buffer, readFileSize, threadCount);
	}
	delete[] buffer;
	return (mParsed);
}


/*
 *   �C���f�b�N�X�t�@�C���̉�͂����{����
 *
 *
 */
BOOL janeIndexParser::parseIndexFile(char *apBuffer, const DWORD aDataSize, int &arThreadCount)
{
	DWORD parsedSize = 0;
	char *ptr;

	// �X���J�E���g�����Z�b�g����
	arThreadCount = 0;

	// �w�b�_���ǂݔ�΂�...
	ptr = strstr(apBuffer, "\x0d\x0a");
	if (ptr == NULL)
	{
		// �w�b�_��������Ȃ������A�A�A�I������
		mParsed = false;
		return (FALSE);
	}
	ptr = ptr + sizeof("\x0d\x0a") - 1;

    // �X���̉�͂����s����
	parsedSize = static_cast<DWORD>(ptr - apBuffer);
	while (parsedSize < aDataSize)
	{
		char *nextPtr;
		nextPtr = strstr(ptr, "\x0d\x0a");
		if (nextPtr == NULL)
		{
			break;
		}

		// ���R�[�h�f�[�^�T�C�Y�����߂�
		DWORD dataSize = 0;
		dataSize = static_cast<DWORD>((nextPtr + sizeof("\x0d\x0a") - 1) - ptr);

		// �X���f�[�^���C���f�b�N�X�i�[�̈�ɓo�^
		entryThreadRecord(ptr, dataSize);
		arThreadCount++;

		parsedSize = parsedSize + ((nextPtr + sizeof("\x0d\x0a") - 1) - ptr);
		ptr = nextPtr + sizeof("\x0d\x0a") - 1;
	}

	// ���R�[�h�C���f�b�N�X�𐶐�����
	createRecordIndex();

	mParsed = true;
	return (TRUE);
}

/*
 *   ���ݕ����Ă���X�������擾����
 *
 *
 */
int janeIndexParser::getThreadCount(void)
{
	//return (mThreadDatabase.size());
	return (static_cast<int>(mDatabaseIndex.size()));
}

/*
 *   ���R�[�h���X���C���f�b�N�X�i�[�̈�ɓo�^����
 *
 *
 */
void janeIndexParser::entryThreadRecord(char *apBuffer, DWORD aDataSize)
{
	char              debugTemp[512];
	unsigned int      num = 0;
	threadIndexRecord indexRecord;
	char *buffer = new char[aDataSize + 100];
	if (buffer == NULL)
	{
		return;
	}
	memset(buffer, 0x00, (aDataSize + 100));
	memmove(buffer, apBuffer, aDataSize);

	// �f�[�^�𕪉�����
	char *ptr1 = buffer;
    char *ptr2 = NULL;

	// ���X�ԍ��̐؂�o��
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
    *ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "THREAD NUMBER : %s (%d)", ptr1, num);
	indexRecord.setCurrentNumber(num);

	// dat�t�@�C�����̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	sprintf(debugTemp, "DAT FILE NAME : %s", ptr1);
	indexRecord.setFileName(ptr1);

	// �X���^�C�g���̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	sprintf(debugTemp, "THREAD TITLE : %s", ptr1);
	indexRecord.setThreadTitle(ptr1);

	// �X���b�h�J�E���g�i���[�J���j�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Count : %d", num);
	indexRecord.setResCount(num);

	// �X���b�h�T�C�Y�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Thread Size : %d", num);
	indexRecord.setFileSize(num);

	// �X���b�h���擾���������i��������j�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Round Date : %d (0x%x)", num, num);
	indexRecord.setRoundDate(num);

	// �X���b�h���X�V����Ă�������i�T�[�o�������j�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Last Modified : %d (0x%x)", num, num);
	indexRecord.setLastModified(num);

	// �R�R�܂œǂ񂾔ԍ��̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "Read Res Number : %d (0x%x)", num, num);
	indexRecord.setReadResNumber(num);

	// �R�R����V�K��M�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "new receive res : %d (0x%x)", num, num);
	indexRecord.setNewReceivedRes(num);

	// �\��(�[���Œ�)�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "reserved : %d (0x%x)", num, num);
	indexRecord.setReserved(num);

	// ���ǃt���O�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "read/unread : %d (0x%x)", num, num);
	indexRecord.setReadUnread(num);

	// �X�N���[���ʒu�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "scrollTop : %d (0x%x)", num, num);
	indexRecord.setScrollTop(num);

	// �X���b�h�J�E���g�i�T�[�o�j�̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "allResCount : %d (0x%x)", num, num);
	indexRecord.setAllResCount(num);

	// �X���b�h�V�����̐؂�o��
	ptr1 = ptr2 + 1;
	ptr2 = strstr(ptr1, "\x01");
	if (ptr2 == NULL)
	{
		delete[] buffer;
		return;
	}
	*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "newResCount : %d (0x%x)", num, num);
	indexRecord.setNewResCount(num);

	// �A�C�e���̏グ�����̐؂�o��
	ptr1 = ptr2 + 1;
	////////// ����: �����̍��ڂȂ̂ŁA�؂�o���͕s�v /////////
	//ptr2 = strstr(ptr1, "\x01");
	//if (ptr2 == NULL)
	//{
	//	delete[] buffer;
	//	return;
	//}
	//*ptr2 = '\0';
	num = strtoul(ptr1, NULL, 16);
	sprintf(debugTemp, "ageSage : %d (0x%x)", num, num);
	indexRecord.setAgeSage(num);

	delete[] buffer;

	// �X���C���f�b�N�X��MAP�Ɋi�[����
	std::string key;
	indexRecord.getFileName(key);
	mThreadDatabase.insert(pair<std::string, threadIndexRecord>(key, indexRecord));
	return;
}

/*
 *  �C���f�b�N�X�f�[�^���N���X��ǉ�����
 *
 */
bool janeIndexParser::entryIndexRecord(threadIndexRecord *&arpIndexRecord)
{
	map<std::string, threadIndexRecord>::iterator ite;
	std::string key;
	arpIndexRecord->getFileName(key);

	ite = mThreadDatabase.find(key);
	if (ite != mThreadDatabase.end())
	{
		// ���ɓo�^�ς݁A�A�A
		return (false);
	}

	// �C���f�b�N�X�f�[�^���N���X��ǉ��o�^����...
	mThreadDatabase.insert(pair<std::string, threadIndexRecord>(key, *arpIndexRecord));
	return (true);
}

// �C���f�b�N�X���R�[�h���擾����
bool janeIndexParser::getIndexRecord(std::string &arFileName, threadIndexRecord *&arpIndexRecord)
{
	map<std::string,threadIndexRecord>::iterator ite;
	ite = mThreadDatabase.find(arFileName);
	if (ite == mThreadDatabase.end())
	{
		// �Y���f�[�^�Ȃ�...
		return (false);
	}
	arpIndexRecord = &(ite->second);

	return (true);
}

/*
 *  ���R�[�h�̃C���f�b�N�X���쐬����
 *
 */
void janeIndexParser::createRecordIndex(void)
{
	size_t dataCount = mThreadDatabase.size();

	map<std::string,threadIndexRecord>::iterator it;
	for (it = mThreadDatabase.begin(); it != mThreadDatabase.end(); it++)
	{
		threadIndexRecord *data;
		data = &(it->second);
		DWORD indexNumber = 0;
		data->getCurrentNumber(indexNumber);
		mDatabaseIndex.insert(pair<DWORD, threadIndexRecord*>(indexNumber, &(it->second)));
	}
	return;
}

/*
 *  ���R�[�h�̃C���f�b�N�X���o�͂���
 *
 */
bool janeIndexParser::outputIndexFile(void)
{
	// �C���f�b�N�X�t�@�C�������w�肳��Ă��Ȃ����H
	if (mIndexFileName.length() == 0)
	{
		return (false);
	}
	char buffer[8192];

	// �o�b�N�A�b�v�t�@�C�����쐬...
	sprintf(buffer, "%s.bak", mIndexFileName.c_str());
	CopyFile(mIndexFileName.c_str(), buffer, FALSE);

	// �t�@�C�����I�[�v������ (�������ݗp)
	HANDLE hFile = CreateFile(mIndexFileName.c_str(),
		                      GENERIC_WRITE, 
		                      FILE_SHARE_WRITE,
							  NULL, 
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// �t�@�C���I�[�v���Ɏ��s�B�B�B�I������
		return (false);
	}
	SetEndOfFile(hFile);

	// �w�b�_�������o�͂���
	sprintf(buffer, "1.01\x0d\x0a");
	DWORD writeSize = 0;
	DWORD length    = 0;
	length = static_cast<DWORD>(strlen(buffer));
	(void) WriteFile(hFile, buffer, length, &writeSize, NULL);

	// �P���R�[�h�Âo�͂���
	int recordCount = static_cast<int>(mDatabaseIndex.size());
	for (int loop = 1; loop <= recordCount; loop++)
	{
		map<DWORD,threadIndexRecord*>::iterator it;
		it = mDatabaseIndex.find(loop);
		if (it != mDatabaseIndex.end())
		{
			std::string fileName;
			std::string threadTitle;
			DWORD       resCount       = 0;
			DWORD       fileSize       = 0;
			DWORD       roundDate      = 0;
			DWORD       lastUpdate     = 0;
			DWORD       readResNumber  = 0;
			DWORD       newReceivedRes = 0;
			DWORD       reserved       = 0;
			DWORD       readUnread     = 0;
			DWORD       scrollTop      = 0;
			DWORD       allResCount    = 0;
			DWORD       newResCount    = 0;
			DWORD       ageSage        = 0;

			threadIndexRecord *record;
			record = it->second;

			// ���R�[�h�̃f�[�^���擾����
			record->getFileName(fileName);
			record->getThreadTitle(threadTitle);
			record->getResCount(resCount);
			record->getFileSize(fileSize);
			record->getRoundDate(roundDate);
			record->getLastModified(lastUpdate);
			record->getReadResNumber(readResNumber);
			record->getNewReceivedRes(newReceivedRes);
			record->getReserved(reserved);
			record->getReadUnread(readUnread);
			record->getScrollTop(scrollTop);
			record->getAllResCount(allResCount);
			record->getNewResCount(newResCount);
			record->getAgeSage(ageSage);

			// ���R�[�h���o�͂���
			memset(buffer, 0x00, sizeof(buffer));
			sprintf(buffer, "%X\x01%s\x01%s\x01%X\x01%X\x01%X\x01%X\x01%X\x01"
				    "%X\x01%X\x01%X\x01%X\x01%X\x01%X\x01%X\x0d\x0a",
					loop, fileName.c_str(), threadTitle.c_str(), resCount, fileSize,
					roundDate,lastUpdate,readResNumber,newReceivedRes,reserved,
					readUnread,scrollTop,allResCount,newResCount,ageSage);

			writeSize = static_cast<DWORD>(strlen(buffer));
			length    = static_cast<DWORD>(strlen(buffer));
			(void) WriteFile(hFile, buffer, length, &writeSize, NULL);
		}
	}
	CloseHandle(hFile);

	return (true);
}
