/*****************************************************************************
 *
 * Generic Conduit - CNNsiMonaSync Src File
 *
 ****************************************************************************/
#define ASSERT(f)          ((void)0)
#define TRACE0(sz)
#define TRACE(sz)
#include <crtdbg.h>
//#include <atlbase.h>
//#include <atlhttp.h>
#include <windows.h>
#include <wininet.h>

#include <string.h>
#include <stdio.h>
#include <time.h>
#ifdef METROWERKS_WIN
#include <wmem.h>
#else
#include <memory.h>
#endif
#include <sys/stat.h>
#include <TCHAR.H>

#include <syncmgr.h>
#include "NNsiMonaSync.h"
#include "NNsiMonaArchive.h"
#include "NNsiMonaBrowserParser.h"

extern HANDLE hLangInstance;

#define MAX_PROD_ID_TEXT	255

/**
  �R���X�g���N�^
**/
CNNsiMonaSync::CNNsiMonaSync(CSyncProperties& rProps, DWORD dwDatabaseFlags)
:  mDbPtr(0), mVfsFileRef(0), CSynchronizer(rProps, dwDatabaseFlags)
{
	mBBSmap.clear();
	memset(&mNNsiMonaPreferences, 0x00, sizeof(NNsiMonaPreferences));
	mNNsiMonaPreferences.waitTime = 800;
	mThreadNick.clear();
}

/**  
   �f�X�g���N�^

 **/
CNNsiMonaSync::~CNNsiMonaSync()
{
	if (mVfsFileRef != 0)
	{
		VFSFileClose(mVfsFileRef);
	}
	mThreadNick.clear();
}

/******************************************************************************
 *
 * Class:   CNNsiMonaSync
 *
 * Method:  SynchronizeAppInfo()
 *
 * Description: This method is used by slow/fast sync to synchronize the
 *              information contained in the AppInfo section on the
 *              Palm organizer.
 *
 * Parameter(s): None.
 *  
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaSync::SynchronizeAppInfo(void)
{
	long                retval = 0;
    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        if (retval == GEN_ERR_CATEGORIES_NOT_SUPPORTED)
            return 0;
        return retval;
    }

    retval = m_dbPC->PreAppInfoSync();
    if (retval) {
        if (retval == GEN_ERR_CATEGORIES_NOT_SUPPORTED)
            return 0;
        return retval;
    }
    retval = SynchronizeCategories();
    if (retval)
        return retval;



    // store the changes back into the app info area on the HH
    retval = m_dbHH->PostAppInfoSync();
    if (retval) {
        return retval;
    }

    // store the changes back into the app info area on the PC
    retval = m_dbPC->PostAppInfoSync();
    if (retval) {
        return retval;
    }
    return retval;
}

long CNNsiMonaSync::CopyAppInfoHHtoPC( void )
{
	MessageBox(NULL, "CNNsiMonaSync::CopyAppInfoHHtoPC(void)", "Information", MB_OK);

	
	long retval = 0;

    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        return retval;
    }
    ASSERT(m_dbHH->GetCatMgr());
    ASSERT(m_dbPC->GetCatMgr());

    CPCategoryMgr *m_pHHCatMgr = m_dbHH->GetCatMgr();
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->GetCatMgr();
    CPCategory *hhCat;

    // copy the categories
    hhCat = m_pHHCatMgr->FindFirst();
    while ((!retval) && (hhCat)) {
        retval = m_pPCCatMgr->Add(*hhCat);
        hhCat = m_pHHCatMgr->FindNext();
    }

	if (!retval) { //fix for categories not getting written to the Pc.
		retval = m_dbPC->PostAppInfoSync();
	}

    return retval;
}

long CNNsiMonaSync::CopyAppInfoPCtoHH( void )
{
	MessageBox(NULL, "CNNsiMonaSync::CopyAppInfoPCtoHH(void)", "Information", MB_OK);

	long retval = 0;

    retval = m_dbPC->PreAppInfoSync();
    if (retval) {
        return retval;
    }
    retval = m_dbHH->PreAppInfoSync();
    if (retval) {
        return retval;
    }
    ASSERT(m_dbHH->GetCatMgr());
    ASSERT(m_dbPC->GetCatMgr());

    CPCategoryMgr *m_pHHCatMgr = m_dbHH->GetCatMgr();
    CPCategoryMgr *m_pPCCatMgr = m_dbPC->GetCatMgr();
    CPCategory *pcCat;

    m_pHHCatMgr->Empty();

    // copy the categories
    pcCat = m_pPCCatMgr->FindFirst();
    while ((!retval) && (pcCat)) {
        retval = m_pHHCatMgr->Add(*pcCat);
        pcCat = m_pPCCatMgr->FindNext();
    }

	if (!retval) { //fix for categories not getting written to the hh.
		retval = m_dbHH->PostAppInfoSync();
	}

    return retval;

}


/******************************************************************************
 *
 * Class:   CNNsiMonaSync
 *
 * Method:  CreatePCManager()
 *
 * Description: This method creates the PC database manager. If it successfully creates
 *          the manager, it then opens the database.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaSync::CreatePCManager(void)
{
    DeletePCManager();

    m_dbPC = new CNNsiMonaPCMgr(m_pLog, 
                        m_dwDatabaseFlags,
                        m_remoteDB->m_Name, 
                        m_rSyncProperties.m_LocalName, 
                        m_rSyncProperties.m_PathName,
                        m_rSyncProperties.m_SyncType);
    if (!m_dbPC)
        return GEN_ERR_LOW_MEMORY;
    return m_dbPC->Open();
}

/******************************************************************************
 *
 * Class:   CNNsiMonaSync
 *
 * Method:  CreateBackupManager()
 *
 * Description: This method creates the backup database manager. If it successfully creates
 *          the manager, it then opens the database.
 *
 * Parameter(s): None.
 *
 * Return Value(s): 0 - no error
 *                  <0 - error code.
 *
 * Revision History:
 *
 * Date         Name        Description
 * ----------------------------------------------------------
 *  03/25/98    KRM         initial revision
 *
 *****************************************************************************/
long CNNsiMonaSync::CreateBackupManager(void)
{
    DeleteBackupManager();

    m_dbBackup = new CNNsiMonaBackupMgr(m_pLog, 
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                m_rSyncProperties.m_LocalName, 
                                m_rSyncProperties.m_PathName,
                                m_rSyncProperties.m_SyncType);
    if (!m_dbBackup)
        return GEN_ERR_LOW_MEMORY;
    return m_dbBackup->Open();
}

CPDbBaseMgr *CNNsiMonaSync::CreateArchiveManager(TCHAR *pFilename) 
{
    return (CPDbBaseMgr *)new CNNsiMonaArchive(m_pLog,
                                m_dwDatabaseFlags,
                                m_remoteDB->m_Name, 
                                pFilename, 
                                m_rSyncProperties.m_PathName);
}

/******************************************************************************
    HotSync�̎��s!!
	   �i�������J�X�^�}�C�Y����Ȃ�Generic Conduit���g���Ă���Ӗ���
         ���͂���܂�Ȃ������������A�A�A�B�j

 ******************************************************************************/
long CNNsiMonaSync::Perform(void)
{
    long retval = 0;
    long retval2 = 0;

	// �����^�C�v�̃`�F�b�N...
    if (m_rSyncProperties.m_SyncType > eProfileInstall)
	{
        // �����^�C�v�̒l���ُ�A�A�A�I������
		return (GEN_ERR_BAD_SYNC_TYPE);
	}

    if (m_rSyncProperties.m_SyncType == eDoNothing)
	{
        // �����^�C�v���u�������Ȃ��v�̂Ƃ��͏I������
		return (0);
    }

	// ��͂̃C���f�b�N�X�f�[�^����͂ł���悤�ɏ�������
	I2chBrowserParser *browserParser = new NNsiMonaBrowserParser;
	browserParser->setBrowserLocation(mNNsiMonaPreferences.browserFileLocation);
	browserParser->setBrowserConfigLocation(mNNsiMonaPreferences.browserConfigLocation);
	browserParser->prepare();
#if 0
	switch (mNNsiMonaPreferences.syncType)
	{
      case eDoNothing:
        // �������Ȃ�
      case eProfileInstall:
        // NNsi�̂��C�ɓ�����X�V����
      case eInstall:
        // NNsi�̃��O���X�V����
        break;
	  case eSlow:
        // ���O�̍X�V�ƃf�[�^�̓���
      case eFast:
        // �f�[�^�̓���
      case ePCtoHH:
        // PC�̃f�[�^��Palm�ɔ��f
      case eHHtoPC:
        // Palm�̃f�[�^��PC�ɔ��f
      default:
		break;
	}
#endif

    // �V�X�e�����̎擾 (SyncReadSystemInfo())
	//  (PalmOS�̃o�[�W�����ƃv���_�N�g�����擾����)
    m_SystemInfo.m_ProductIdText = (BYTE*) new char [MAX_PROD_ID_TEXT];
    if (m_SystemInfo.m_ProductIdText == NULL)
	{
        // �̈�̊m�ێ��s...
		delete browserParser;
		return (GEN_ERR_LOW_MEMORY);
	}
    m_SystemInfo.m_AllocedLen = (BYTE) MAX_PROD_ID_TEXT; 
	retval = SyncReadSystemInfo(m_SystemInfo);
    if (retval != 0)
	{
		// �V�X�e�����̎擾�Ɏ��s�����A�I������
		delete browserParser;
		return (retval);
	}

	// SyncRegisterConduit() �̌Ăяo��...
	//    (RegisterConduit()�͊��N���X�Ɏ����ς�)
	retval = RegisterConduit();
    if (retval != 0)
	{
		// �����J�n�G���[...
		delete browserParser;
		return (retval);
	}
	LogAddEntry("", slSyncStarted, false);

    //////////////////////  ����������������  //////////////////////

	// BBS�ꗗ�̃A�b�v���[�h (map�Ƀf�[�^���i�[����)
	uploadBBSdatabase();

	// Palm�{�̂̃��O�̂ݐV���m�F����ꍇ...
	//if ((mNNsiMonaPreferences.syncType == eInstall)||(mNNsiMonaPreferences.syncType == eProfileInstall))

	// �X���ꗗ�̃f�[�^�X�V�i�V���m�F�j�F�����͏���҂�����(ms)
    //     TODO: ���ꂼ��̏����ɂ���ăN���X������K�v�A���I�I (���t�@�N�^�����O�ӏ��I�j
	switch (mNNsiMonaPreferences.syncType)
	{
      case eHHtoPC:
		// Palm�̃��O���͂̂Q�����u���E�U�ɃR�s�[����
		updateAndCopyHHtoPC(browserParser);
        break;

	  case ePCtoHH:
	  case eSlow:
      case eFast:
		// ���O���͂̂Q�����u���E�U����Palm�ɃR�s�[����
		updateAndCopyPCtoHH(browserParser);
        break;

	  case eInstall:
      case eProfileInstall:
	  default:
        // Palm�̃��O���X�V or Palm�̂��C�ɓ��胍�O���X�V
		updateThreadInformation();
		break;
	}

	//////////////////////  �������������܂�  //////////////////////
	delete browserParser;

	// SyncUnRegisterConduit()�̌Ăяo��...
	//    (UnregisterConduit()�͊��N���X�Ɏ����ς�)
	LogAddEntry("", slSyncFinished, false);
    retval2 = UnregisterConduit((BOOL)(retval != 0));
    if (retval == 0)
	{
		// �����I���G���[...
		return (retval2);
	}
    return retval;
}

/*
 *    BBS�ꗗ�̃A�b�v���[�h...
 *
 */
void CNNsiMonaSync::uploadBBSdatabase(void)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead;

	// �f�[�^�x�[�X�̃I�[�v��
	ret = SyncOpenDB(DBNAME_BBSLIST, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// BBS�ꗗ�̃��R�[�h�����擾����
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// ���R�[�h�����̎擾�Ɏ��s�B�B�B�I������
		SyncCloseDB(rHandle);
		return;
	}

	// ���R�[�h�����ׂēǂݏo��
	for (WORD loop = 0; loop < recordCount; loop++)
	{
		// �f�[�^�P���ǂݏo���p�̏����쐬
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = loop;

		BYTE recordBuffer[sizeof(NNshBoardDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// �f�[�^�P���ǂݏo��...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// �f�[�^(���R�[�h)�̓ǂݏo���ɐ���...
			NNshBoardDatabase *dbPtr;
			dbPtr = reinterpret_cast<NNshBoardDatabase *>(recordBuffer);

			// BBS��URL��map�ɓ����...
			mBBSmap.insert(pair<string,string>(dbPtr->boardNick, dbPtr->boardURL));
        }
	}

	// �f�[�^�x�[�X�̃N���[�Y
	SyncCloseDB(rHandle);

	return;
}

/*
 *    �X���ꗗ�̃f�[�^�X�V�i�V���m�F�j...
 *
 */
void CNNsiMonaSync::updateThreadInformation(void)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead | eDbWrite;

	// �f�[�^�x�[�X�̃I�[�v��
	ret = SyncOpenDB(DBNAME_SUBJECT, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// �X���ꗗ�̃��R�[�h�����擾����
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// ���R�[�h�����̎擾�Ɏ��s�B�B�B�I������
		SyncCloseDB(rHandle);
		return;
	}

	// ���R�[�h�����ׂēǂݏo��
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// �f�[�^�P���ǂݏo���p�̏����쐬
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// �f�[�^�P���ǂݏo��...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// �f�[�^(���R�[�h)�̓ǂݏo���ɐ���...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// �ǂݍ��񂾃X���̏������O�o�͂���
			if (checkGetThreadStatus(mDbPtr) == TRUE)
			{
				// �V���m�F����X���������ꍇ...
                ret = getThreadLog(mDbPtr);
				if (ret == NNSH_NORMAL_END)
				{
					// �f�[�^������...
					SyncWriteRec(recordInfo);

					///// �擾�̂��߂�delay������... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
				if (ret == NNSH_RECEIVE_RESPONSE_NG)
				{
					///// �擾�̂��߂�delay������... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
			}
		}
		mDbPtr = 0;
	}

	// �f�[�^�x�[�X�̃N���[�Y
    SyncCloseDB(rHandle);
}

/*
      ���O���擾(�V���m�F)����...

 */
long CNNsiMonaSync::getThreadLog(NNshSubjectDatabase *dbPtr)
{

	// ��URL���擾����
	map<string,string>::iterator ite;
	ite = mBBSmap.find(dbPtr->boardNick);
	if (ite == mBBSmap.end())
	{
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "    BBS does not find... [%s]", dbPtr->boardNick);
		LogAddEntry(buffer, slWarning, false);
		return (NNSH_NOTFOUND_BBS);
	}

	// �^�C�v���Q�����˂�/�܂�BBS�ȊO�͐V���m�F���Ȃ�... (�Ƃ肠���������...)
	if ((dbPtr->bbsType != NNSH_BBSTYPE_2ch)&&(dbPtr->bbsType != NNSH_BBSTYPE_MACHIBBS))
	{
		return (NNSH_BBSTYPE_NG);
	}

	// ���O��VFS�Ɋi�[����Ă��Ȃ��ꍇ�ɂ́A���O�擾���Ȃ�...
    //  (�擾�\��X���̏ꍇ�ɂ́A���ɐi��...)
	if ((dbPtr->msgState != FILEMGR_STATE_OPENED_VFS)&&
		((dbPtr->msgAttribute & NNSH_MSGATTR_GETRESERVE) == 0))
	{
		//MessageBox(NULL, "NNSH_LOGGINGPLACE_WRONG", "Information", MB_OK);
		return (NNSH_LOGGINGPLACE_WRONG);
	}

	// �����^�C�v���u���C�ɓ��肾���V���m�F�v�������ꍇ...
	if (mNNsiMonaPreferences.syncType == eProfileInstall)
	{
		// ���C�ɓ���X�������V���m�F�����{����...
		if ((dbPtr->msgAttribute & ((NNSH_MSGATTR_FAVOR)|(NNSH_MSGATTR_FAVOR_L4)|(NNSH_MSGATTR_FAVOR_L3)|(NNSH_MSGATTR_FAVOR_L2)|(NNSH_MSGATTR_FAVOR_L1))) == 0)
		{
			return (NNSH_LOGGINGTYPE_NOTMATCHED);
		}
	}

    if (checkVFSVolumes(dbPtr->boardNick, dbPtr->threadFileName) == FALSE)
	{
		// VFS�̃f�[�^�����܂��݂���Ȃ�����
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "    Valid VFS Volume is nothing... [%s]", dbPtr->threadTitle);
		LogAddEntry(buffer, slText, false);
		return (NNSH_LOGGINGPLACE_WRONG_VFS);
	}

	// �擾����URL�𐶐�����... (���͂Q�����˂�̂�...)
	char url[256], *ptr;
	DWORD getBytes = 0;
    memset(url, 0x00, sizeof(url));
	switch (dbPtr->bbsType)
	{
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂�BBS�̎擾URL�𐶐�����
		strcpy(url, ite->second.c_str());
		strcat(url, "bbs/read.pl?BBS=");
        strcat(url, dbPtr->boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[strlen(url) - 1] == '/')
        {
            url[strlen(url) - 1] = '\0';
        } 
        strcat(url, "&KEY=");
        strcat(url, dbPtr->threadFileName);
        ptr = &url[strlen(url) - 1];
        // threadFileName�̊g���q���폜����
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';

		// �܂�BBS�͍����擾���Ȃ�...
		getBytes = 0;
        break;

	  case NNSH_BBSTYPE_2ch:
	  default:
        // 2�����˂�̎擾URL�𐶐�����
		sprintf(url, "%sdat/%s", ite->second.c_str(), dbPtr->threadFileName);

	    // �擾����X���̐擪�o�C�g���m�F����
	    if (dbPtr->fileSize != 0)
	    {
		    getBytes = SyncHHToHostDWord(dbPtr->fileSize) - 1;
	    }
		break;
	}

    // HTTP(�����擾)�����{����
	long returnValue = httpCommunication(url, getBytes);
	if (returnValue == NNSH_RECEIVE_RESPONSE_NG)
	{
		// �V���m�F����X���������ꍇ�A(�Ƃ肠����)���O�o�͂���...
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "        %s (%s)", dbPtr->threadTitle, url);
		LogAddEntry(buffer, slText, false);
	}

	return (returnValue);
}

/*
    http�ʐM�����{ (WinInet���g�p����)
 */
long CNNsiMonaSync::httpCommunication(char *url, int range)
{
	HINTERNET hSession = 0;
	hSession = InternetOpen("Monazilla/1.00  (NNsiMona 1.0)", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hSession == 0)
	{
        // internet�̃I�[�v���Ɏ��s...�I������
		LogAddEntry("   InternetOpen()", slWarning, false);
		return (NNSH_RECIEVE_ERROR);
	}

	// http�v���w�b�_�̒ǉ�
	char httpHeader[4096 + 1];
	memset(httpHeader, 0x00, sizeof(httpHeader));
	if (range == 0)
	{
		strcpy(httpHeader, "ACCEPT: text/html, */*\r\nACCEPT-ENCODING: identity\r\nConnection: close\r\nACCEPT-LANGUAGE: ja, en\r\nPragma: no-cache\r\n\r\n");
	}
	else
	{
		sprintf(httpHeader, "RANGE: bytes=%d-\r\nACCEPT: text/html, */*\r\nACCEPT-ENCODING: identity\r\nConnection: close\r\nACCEPT-LANGUAGE: ja, en\r\nPragma: no-cache\r\n\r\n", range);
	}

	// URL����f�[�^���擾����...
	HINTERNET hService = 0;
	hService = InternetOpenUrl(hSession, url, httpHeader, strlen(httpHeader), INTERNET_FLAG_NO_AUTO_REDIRECT, 0);
	if (hService == 0)
	{
		// �ڑ��Ɏ��s�A�A�A�I������
		InternetCloseHandle(hSession);
		LogAddEntry("   InternetOpenUrl()", slWarning, false);
		return (NNSH_RECIEVE_ERROR);
	}

	// HTTP�����w�b�_���m�F����...
	DWORD headerBufferLength;
	memset(httpHeader, 0x00, sizeof(httpHeader));
	headerBufferLength = sizeof(httpHeader) - 1;
	if (HttpQueryInfo(hService, HTTP_QUERY_RAW_HEADERS_CRLF, httpHeader, &headerBufferLength, NULL) != TRUE)
	{
		// �����w�b�_���擾�ł��Ȃ��A�A�A���肦�Ȃ��͂�����...�I������
		InternetCloseHandle(hService);
		InternetCloseHandle(hSession);
		LogAddEntry("   HttpQueryInfo()", slWarning, false);
		return (NNSH_RECIEVE_ERROR);
	}

	// ��M�f�[�^�̃f�[�^�o�C�g�����擾����...
	char *ptr;
	ptr = strstr(httpHeader, "Content-Length");
	if (ptr == NULL)
	{
		ptr = strstr(httpHeader, "CONTENT-LENGTH");
	}
	long contentLength = getContentLength(ptr);

	// �����R�[�h�̃`�F�b�N
	long returnValue;
	if ((httpHeader[9] == '2')&&(httpHeader[10] == '0')&&(httpHeader[11] == '0'))
    {
	    // �S���b�Z�[�W����M����...
		returnValue = receivedWholeMessage(hService, contentLength);
	}
    else if ((httpHeader[9] == '2')&&(httpHeader[10] == '0')&&(httpHeader[11] == '6'))
    {

		// �����擾�ɐ�������...
		returnValue = receivedPartMessage(hService, contentLength);
	}
	else
	{
        // �����R�[�h�ُ�...
		char message[80];
		sprintf(message, "    WRONG HTTP RESPONSE(HTTP/1.1 %c%c%c)", 
			    httpHeader[9], httpHeader[10], httpHeader[11]);
		LogAddEntry(message, slWarning, false);
	    returnValue = NNSH_RECEIVE_RESPONSE_NG;
	}

	// HTTP�̃Z�b�V�������N���[�Y����...
	InternetCloseHandle(hService);
	InternetCloseHandle(hSession);

	return (returnValue);
}

/*
    �S���擾�����s...
 */
long CNNsiMonaSync::receivedWholeMessage(HINTERNET hService, long contentLength)
{
	if ((mVfsFileRef == 0)||(mDbPtr == NULL))
	{
		// VFS�̏������ݏ������ł��Ă��Ȃ�...�I������
		return (NNSH_RECIEVE_ERROR);
	}

	// �t�@�C����擪���珑�����ނ悤�ɂ���...
	VFSFileSeek(mVfsFileRef, fsOriginBeginning, 0);

	// �t�@�C���T�C�Y�̋L��...
	DWORD originalFileSize = SyncHHToHostDWord(mDbPtr->fileSize);

	// ��M�f�[�^���i�[����o�b�t�@��p�ӂ���...
    char receiveBuffer[8192 + 8];
	DWORD totalReadBytes = 0;
	while (1)
	{
		// �擾�o�b�t�@���N���A����
		memset(receiveBuffer, 0, sizeof(receiveBuffer));
	
		// ��M�f�[�^��ǂݏo���A�A�A
		DWORD dwBytesRead = 8192;
	    BOOL ret = InternetReadFile(hService, receiveBuffer, 8192, &dwBytesRead);
		if ((ret == FALSE)||(dwBytesRead == 0))
		{
			// �f�[�^�擾�I��...���[�v�𔲂���
			break;
        }
		totalReadBytes = totalReadBytes + dwBytesRead;

		// VFS�t�@�C���Ƀ��O��append����
		UINT32 outputDataSize = 0;
		if ((dwBytesRead % 2) == 0)
		{
			// ������NULL�^�[�~�l�[�g
			receiveBuffer[dwBytesRead]     = '\0';
		
			// �␳�Ȃ�VFS�f�[�^�o��
			VFSFileWrite(mVfsFileRef, dwBytesRead, receiveBuffer, &outputDataSize);
		}
		else
		{
			// CLIE�p�␳�f�[�^���쐬...
			receiveBuffer[dwBytesRead]     = ' ';
			receiveBuffer[dwBytesRead + 1] = '\0';

			// �␳����VFS�f�[�^(�o�̓f�[�^�������o�C�g�ɕ␳����...)
			VFSFileWrite(mVfsFileRef, (dwBytesRead + 1), receiveBuffer, &outputDataSize);
		}
	}

#if 0
	// �o�̓t�@�C���T�C�Y�����O�o�͂���
	char buffer[400];
	memset(buffer, 0x00, sizeof(buffer));
	sprintf(buffer, "   Wrote %d bytes. (%s)", totalReadBytes, mDbPtr->threadTitle);
	LogAddEntry(buffer, slText, false);
#endif

	// �t�@�C���̃N���[�Y...
	VFSFileClose(mVfsFileRef);
	mVfsFileRef = 0;

	// �t�@�C���T�C�Y�̉��Z...
	mDbPtr->fileSize = SyncHostToHHDWord(totalReadBytes);

	// ��M�f�[�^���u�V�K�擾�v�ɕύX����...
	if (originalFileSize != totalReadBytes)
	{
		// �V�K�擾�ɂ���̂́A�t�@�C���T�C�Y���قȂ��Ă���Ƃ��̂�...
		mDbPtr->state = NNSH_SUBJSTATUS_NEW;
	}
	mDbPtr->msgState = FILEMGR_STATE_OPENED_VFS;
	mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&((NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE)));

	return (NNSH_NORMAL_END);
}

/*
    �����擾�����s...
 */
long CNNsiMonaSync::receivedPartMessage(HINTERNET hService, long contentLength)
{
	// ��M�f�[�^��ǂݏo���A�A�A
	char *receiveBuffer;

	// ��M�f�[�^���i�[����o�b�t�@��p�ӂ���...
	receiveBuffer = static_cast<char *>(malloc(contentLength + 100 + 1));
	if (receiveBuffer == NULL)
	{
		return (NNSH_MEMORY_ALLOCATION_FAILURE);
	}
	memset(receiveBuffer, 0x00, (contentLength + 100 + 1));

	// ��M�f�[�^���o�b�t�@�Ɋi�[����...
	DWORD dwBytesRead = contentLength + 100;
    InternetReadFile(hService, receiveBuffer, (contentLength + 100), &dwBytesRead);

	// �X�����̕ύX�����邩�ǂ����`�F�b�N����
	if ((dwBytesRead == 1)&&(receiveBuffer[0] == '\x0a'))
	{
		// �X���ύX�Ȃ��i�V�����X�Ȃ��j
		free(receiveBuffer);

		// VFS�t�@�C�����I�[�v�����Ă���ꍇ�ɂ̓N���[�Y
		if (mVfsFileRef != 0)
		{
			VFSFileClose(mVfsFileRef);
			mVfsFileRef = 0;
		}

		// �G���[�t���O�Ǝ擾�ۗ��t���O�𗎂Ƃ�...
		mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&((NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE)));
		return (NNSH_NORMAL_END);
	}
	receiveBuffer[dwBytesRead] = '\0';

	// ��M�f�[�^�ُ̈���`�F�b�N����...
	if (receiveBuffer[0] != '\x0a')
	{
		// ��M�f�[�^���ُ�A�A�A�I������
		return (NNSH_RECEIVE_DATA_WRONG);
	}
	receiveBuffer[0] = ' ';  // CLIE�␳�p�ɏ���...

	if ((mVfsFileRef != 0)&&(mDbPtr != NULL))
	{
		// VFS�t�@�C���Ƀ��O��append����
		UINT32 outputDataSize = 0;
		VFSFileSeek(mVfsFileRef, fsOriginEnd, 0);
		if ((dwBytesRead % 2) == 0)
		{
			// �␳�t��VFS�f�[�^�o��
			VFSFileWrite(mVfsFileRef, dwBytesRead, receiveBuffer, &outputDataSize);
		}
		else
		{
			// �␳�Ȃ�VFS�f�[�^�o�� (1�o�C�g�ڂɂ� '\x0a'�A�O���X�̋�؂肪�����Ă���)
			VFSFileWrite(mVfsFileRef, (dwBytesRead - 1), &receiveBuffer[1], &outputDataSize);
		}

		// �t�@�C���̃N���[�Y...
		VFSFileClose(mVfsFileRef);
		mVfsFileRef = 0;

		// �t�@�C���T�C�Y�̉��Z...
		DWORD fileSize = SyncHHToHostDWord(mDbPtr->fileSize);
		fileSize = fileSize + contentLength - 1;
		mDbPtr->fileSize = SyncHostToHHDWord(fileSize);

		// ��M�f�[�^���u�X�V�ς݁v�ɕύX����...
		mDbPtr->state = NNSH_SUBJSTATUS_UPDATE;
		mDbPtr->msgState = FILEMGR_STATE_OPENED_VFS;
	    mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&((NNSH_MSGATTR_NOTERROR)&(NNSH_MSGATTR_NOTRESERVE)));
	}

	// �m�ۂ����o�b�t�@���N���A����...
	free(receiveBuffer);

	return (NNSH_NORMAL_END);
}

/**
    Content-Length �ɋL�q���ꂽ���l���擾����
 **/
long CNNsiMonaSync::getContentLength(char *str)
{
	if (str == NULL)
	{
		return (0);
	}

	long length = 0;
	while ((*str < '0')||(*str > '9'))
	{
		if (*str < ' ')
		{
			break;
		}
		str++;
	}
	while ((*str >= '0')&&(*str <= '9'))
	{
		length = length * 10 + (*str - '0');
		str++;
	}
	return (length);
}

/***
   VFS�̃{�����[�����`�F�b�N����...
   (�I�[�v�����Ă��̃n���h����mVfsFileRef �ɓ���ĉ�������
 ***/
BOOL CNNsiMonaSync::checkVFSVolumes(char *boardNick, char *datFileName)
{
	// �t�@�C�����I�[�v�����Ă����ԂȂ�A��x�t�@�C�������
	if (mVfsFileRef != 0)
	{
		VFSFileClose(mVfsFileRef);
	}
	mVfsFileRef = 0;

	WORD  nofVolume = 0;
	WORD *volRefNumList;
	long ret = VFSVolumeEnumerate(&nofVolume, NULL);
	if ((ret != 0)||(nofVolume == 0))
	{
		// VFS���Ȃ��A�A�A�I������
		return (FALSE);
	}

	// VFS�{�����[���̊i�[�̈���m�ۂ���
	volRefNumList = new WORD[nofVolume];
	if (volRefNumList == NULL)
	{
		// �̈�m�ۂɎ��s...
		return (FALSE);
	}
	ret = VFSVolumeEnumerate(&nofVolume, volRefNumList);
	if ((ret != 0)||(nofVolume == 0))
	{
		// VFS���Ȃ��A�A�A�I������ (�������肦�Ȃ��͂��B�B�B)
		return (FALSE);
	}

	// �t�@�C�����𐶐�����...
	char openFileName[256];
	memset(openFileName, 0x00, sizeof(openFileName));
	sprintf(openFileName, "/Palm/Programs/NNsh/%s%s", boardNick, datFileName);
	// VFS�̃{�����[�����m�F����
	BOOL retValue = FALSE;
	for (int loop = 0; loop < nofVolume; loop++)
	{
		VolumeInfoType  volInfo;
		memset(&volInfo, 0x00, sizeof(volInfo));
		ret = VFSVolumeInfo(volRefNumList[loop], &volInfo);
		if (ret != 0)
		{
			// VFS�{�����[����񂪂Ƃ�Ȃ������A�A�A���̃{�����[����...
			continue;
		}
		if (volInfo.fsType != fsFilesystemType_VFAT)
		{
			// VFS�{�����[����VFAT�ł͂Ȃ������A�A�A���̃{�����[����...
			continue;
		}

		// �ړI�̃t�@�C�����I�[�v���ł��邩��������...
		FileRef fileRef = 0;
		ret = VFSFileOpen(volRefNumList[loop], openFileName, vfsModeReadWrite, &fileRef);
		if (ret == 0)
		{
			// dat�t�@�C����OPEN�ɐ��������ꍇ...
			mVfsFileRef = fileRef;
			retValue = TRUE;

			// �擾�\��t���O�𗎂Ƃ�...
			mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&(NNSH_MSGATTR_NOTRESERVE));

			break;
		}
		//if (ret != 0)
		{
			// �ړI�̃t�@�C�����I�[�v���ł��Ȃ������ꍇ�A�A�A
			if ((mDbPtr->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
			{
				// �擾�\��X���̏ꍇ�ɂ́A�t�@�C�����쐬����
				(void) VFSFileOpen(volRefNumList[loop], openFileName, (vfsModeCreate), &fileRef);
				VFSFileClose(fileRef);
				ret = VFSFileOpen(volRefNumList[loop], openFileName, vfsModeReadWrite, &fileRef);
				if (ret != 0)
				{
					continue;
				}
				// dat�t�@�C����OPEN�ɐ��������ꍇ...
				mVfsFileRef = fileRef;
				retValue = TRUE;

				// �擾�\��t���O�𗎂Ƃ�...
			    mDbPtr->msgAttribute = ((mDbPtr->msgAttribute)&(NNSH_MSGATTR_NOTRESERVE));

				break;
			}
			else
			{
				continue;
			}
		}
	}
	delete [] volRefNumList;
	return (retValue);
}

/*
	NNsiMona�̐ݒ�����擾����

 */
void CNNsiMonaSync::setPreferences(NNsiMonaPreferences *dataSource)
{
	memcpy(&mNNsiMonaPreferences, dataSource, sizeof(NNsiMonaPreferences));
}

/*
    �X���擾��Ԃ��m�F���A�擾����X���������ꍇ�ɂ�true��Ԃ�

 */
BOOL CNNsiMonaSync::checkGetThreadStatus(NNshSubjectDatabase *dbPtr)
{
	// �Q�Ɛ�p���O�������ꍇ�ɂ́A�������Ȃ�
	if (dbPtr->boardNick[0] == '!')
	{
        return (FALSE);
	}

	// �擾�\��X���������ꍇ�ɂ͍X�V����
	if ((dbPtr->msgAttribute & NNSH_MSGATTR_GETRESERVE) != 0)
	{
        return (TRUE);
	}


	WORD mesNum = 0;
    switch (dbPtr->state)
	{
      case NNSH_SUBJSTATUS_NEW:
	  case NNSH_SUBJSTATUS_UPDATE:
	  case NNSH_SUBJSTATUS_REMAIN:
      case NNSH_SUBJSTATUS_ALREADY:
		// �擾������...

		// ���X���̍ő�����o����...  
		mesNum = SyncHHToHostWord(dbPtr->maxLoc);
		if (mesNum > 1000)
		{
			// �X���̍ő�𒴂����A�擾���Ȃ��B
			char buffer[400];
            memset(buffer, 0x00, sizeof(buffer));
			sprintf(buffer, "   Res: %d  [%s%s]", mesNum, dbPtr->threadTitle, dbPtr->boardNick, dbPtr->threadFileName);
            LogAddEntry(buffer, slText, false);
			return (FALSE);
		}
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   MATCH %s  [%s%s]", dbPtr->threadTitle, dbPtr->boardNick, dbPtr->threadFileName);
		LogAddEntry(buffer, slText, false);
#endif
		return (TRUE);
		break;

	  default:
        // �擾���Ȃ����...
		break;
	}
	return (FALSE);
}


/*
 *    �V���m�F�����{���A����Palm�̃X�������͂̂Q�����u���E�U�ɃR�s�[����
 *     �� �R�s�[������e�F���O�t�@�C���A���ǃ|�C���g�A�擾���O�t�@�C���T�C�Y...
 */
void CNNsiMonaSync::updateAndCopyHHtoPC(I2chBrowserParser *apParser)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead | eDbWrite;

	// �f�[�^�x�[�X�̃I�[�v��
	ret = SyncOpenDB(DBNAME_SUBJECT, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// �X���ꗗ�̃��R�[�h�����擾����
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// ���R�[�h�����̎擾�Ɏ��s�B�B�B�I������
		SyncCloseDB(rHandle);
		return;
	}

	char logDataSeparator[32];
	memset(logDataSeparator, 0x00, sizeof(logDataSeparator));
	apParser->getSeparatorData(logDataSeparator);

	// ���R�[�h�����ׂēǂݏo��
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// �f�[�^�P���ǂݏo���p�̏����쐬
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// �f�[�^�P���ǂݏo��...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// �f�[�^(���R�[�h)�̓ǂݏo���ɐ���...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// �ǂݍ��񂾃X���̏������O�o�͂���
			if (checkGetThreadStatus(mDbPtr) == TRUE)
			{
				// �V���m�F����X���������ꍇ...
                ret = getThreadLog(mDbPtr);
				if (ret == NNSH_NORMAL_END)
				{
					// �f�[�^������...
					SyncWriteRec(recordInfo);

					///// �擾�̂��߂�delay������... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
				if (ret == NNSH_RECEIVE_RESPONSE_NG)
				{
					///// �擾�̂��߂�delay������... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
			}
			else
			{
				// �V���m�F���Ȃ��X���������ꍇ...
				//  (���O��VFS�ɂ���΁A���̃��O��PC�̃��O�ɏ㏑������)
			}

			// �C���f�b�N�X���R�[�h���擾����...
			std::string boardNick = mDbPtr->boardNick;
			std::string fileName  = mDbPtr->threadFileName;
			threadIndexRecord *indexRecord;

			long  resNumArray[65536];
			memset(resNumArray, 0x00, sizeof(resNumArray));

			// ���ݎ������擾����
			time_t unixTime = 0;
			time(&unixTime);
			//localtime(&unixTime);
			DWORD dataFileSize = SyncHHToHostDWord(mDbPtr->fileSize);
			DWORD currentUnixTime = unixTime;
			currentUnixTime = currentUnixTime + (9 * 60 * 60);  // 9���Ԏ��Ԃ�i�߂�...

			if ((mDbPtr->bbsType == NNSH_BBSTYPE_2ch)&&(mDbPtr->msgState == FILEMGR_STATE_OPENED_VFS))
			{
				// �����Ń��O���X�V����...
				if (checkVFSVolumes(mDbPtr->boardNick, mDbPtr->threadFileName) == TRUE)
				{
					// ���O��PC�ɃR�s�[����...
					UINT32 fileSize, readSize;
					if (VFSFileSize(mVfsFileRef, &fileSize) == 0)
					{
						char *buf = new char[fileSize + 100];
						if (buf != NULL)
						{
							memset(buf, 0x00, (fileSize + 100));
							if (VFSFileRead(mVfsFileRef, fileSize, buf, &readSize) == 0)
							{
								// �x�[�X�p�X�̐ݒ�...
								std::string basePath;
								apParser->getBasePath(basePath);
								int resNum = 0;

								// ���O���擾�ς݂������ꍇ...
								if (apParser->getIndexRecord(boardNick, fileName, indexRecord) == true)
								{
									// �����Ń��O�t�@�C�����R�s�[����...
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// ���X�ԍ������X�V����...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}

									// ���ǈʒu��ݒ�...
									DWORD curResNum = 0;
									indexRecord->getCurrentResNumber(curResNum);
									if (curResNum < locNum)
									{
										indexRecord->setCurrentResNumber(locNum);
									}
									indexRecord->setAllResCount(maxNum);  // �S���X�� (�T�[�o)
									indexRecord->setResCount(maxNum);     // �S���X�� (���[�J��)
									indexRecord->setFileSize(dataFileSize); // �t�@�C���T�C�Y
									indexRecord->setNewReceivedRes(newResNum);  // ��������V�K�擾
									indexRecord->setNewResCount(newNum);        // �V����
									indexRecord->setRoundDate(currentUnixTime);  // �擾�������L������
									//indexRecord->setScrollTop(resNumArray[locNum]);    // "�����܂œǂ�"�ԍ�
									indexRecord->setReadResNumber(locNum);          // "�����܂œǂ�"�ԍ�
									//indexRecord->setCurrentNumber(0);           // ���݂̔ł̃C���f�b�N�X�ʒu
									indexRecord->setCurrentResNumber(locNum);
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								else
								{
									// �X���^�C�g����ǉ����ă��O�t�@�C�����R�s�[����
									threadIndexRecord *indexRecord = new threadIndexRecord;

									indexRecord->getFileName(fileName);
									std::string threadTitle = mDbPtr->threadTitle;
									indexRecord->setThreadTitle(threadTitle);
									apParser->entryIndexRecord(boardNick, indexRecord);
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// ���X�ԍ������X�V����...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}
									indexRecord->setCurrentResNumber(locNum);
									indexRecord->setAllResCount(maxNum);  // �S���X�� (�T�[�o)
									indexRecord->setResCount(maxNum);     // �S���X�� (���[�J��)
									indexRecord->setFileSize(dataFileSize); // �t�@�C���T�C�Y
									indexRecord->setNewReceivedRes(newResNum);  // ��������V�K�擾
									indexRecord->setNewResCount(newNum);        // �V����
									indexRecord->setRoundDate(currentUnixTime); // �擾�������L������
									//indexRecord->setScrollTop(resNumArray[locNum]);    // �X�N���[���̐擪
									indexRecord->setReadResNumber(locNum);    // "�����܂œǂ�"�ԍ�
									//indexRecord->setCurrentNumber(0);         // ���݂̔ł̃C���f�b�N�X�ԍ�
									delete indexRecord;
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								delete [] buf;
							}
						}
						VFSFileClose(mVfsFileRef);
					}
				}

			}
		}
		mDbPtr = 0;
	}
	apParser->updateIndexRecords();

	// �f�[�^�x�[�X�̃N���[�Y
    SyncCloseDB(rHandle);
}


/*
 *    �V���m�F�����{���A����Palm�̃X�������͂̂Q�����u���E�U�ɃR�s�[����
 *     �� �R�s�[������e�F���O�t�@�C���A���ǃ|�C���g�A�擾���O�t�@�C���T�C�Y...
 */
void CNNsiMonaSync::updateAndCopyPCtoHH(I2chBrowserParser *apParser)
{
	long ret;
	BYTE rHandle;
	BYTE openMode;
	openMode = eDbRead | eDbWrite;

	// �f�[�^�x�[�X�̃I�[�v��
	ret = SyncOpenDB(DBNAME_SUBJECT, 0, rHandle, openMode);
	if (ret != 0)
	{
		return;
	}

	// �X���ꗗ�̃��R�[�h�����擾����
	WORD recordCount = 0;
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// ���R�[�h�����̎擾�Ɏ��s�B�B�B�I������
		SyncCloseDB(rHandle);
		return;
	}

	// �X������ǂݏo���O�ɏ���������...
	mThreadNick.clear();

	// ���R�[�h�����ׂēǂݏo��
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// �f�[�^�P���ǂݏo���p�̏����쐬
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// �f�[�^�P���ǂݏo��...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// �f�[�^(���R�[�h)�̓ǂݏo���ɐ���...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// 2ch�^�C�v�̃X���Ŏ擾�ς݂̂��̂����A�A�A
			if ((mDbPtr->bbsType == NNSH_BBSTYPE_2ch)&&(mDbPtr->msgState == FILEMGR_STATE_OPENED_VFS))
			{
				// �X�������i�[����
				std::string boardNick  = mDbPtr->boardNick;
				std::string datLogFile = mDbPtr->threadFileName;
				mThreadNick.insert(pair<std::string,std::string>(datLogFile, boardNick));
			}
		}
		mDbPtr = 0;
	}

	/////////// ���R�[�h���ǉ�����Ă��邩�ǂ����`�F�b�N  //////////
	std::list<ThreadInfo> appendRecordList;
	appendRecordList.clear();
	ret = apParser->getThreadRecordToAppend(mThreadNick, appendRecordList);
	if (ret != 0)
	{
		NNshSubjectDatabase subjDB;
		std::list<ThreadInfo>::iterator it;
		for (it = appendRecordList.begin(); it != appendRecordList.end(); it++)
		{
			memset(&subjDB, 0x00, sizeof(NNshSubjectDatabase));

			// �f�[�^�x�[�X�Ƀ��R�[�h��ǉ�����
			// apParser->getAppendThreadList(mThreadNick, (ret + 4), subjDBList);
		}
	}
	///////////////////////////////////

	char logDataSeparator[32];
	memset(logDataSeparator, 0x00, sizeof(logDataSeparator));
	apParser->getSeparatorData(logDataSeparator);

	// ���߂ă��R�[�h�������擾����...
	ret = SyncGetDBRecordCount(rHandle, recordCount);
	if (ret != 0)
	{
		// ���R�[�h�����̎擾�Ɏ��s�B�B�B�I������
		SyncCloseDB(rHandle);
		return;
	}

	// ���R�[�h�����ׂēǂݏo��
	for (WORD loop = recordCount; loop != 0; loop--)
	{
		// �f�[�^�P���ǂݏo���p�̏����쐬
		CRawRecordInfo  recordInfo;
		memset(&recordInfo, 0x00, sizeof(CRawRecordInfo));
		recordInfo.m_FileHandle = rHandle;
		recordInfo.m_RecIndex   = (loop - 1);

		BYTE recordBuffer[sizeof(NNshSubjectDatabase) + MARGIN];
		memset(recordBuffer, 0x00, sizeof(recordBuffer));

		recordInfo.m_pBytes = recordBuffer;
		recordInfo.m_TotalBytes = sizeof(recordBuffer);

		// �f�[�^�P���ǂݏo��...
		ret = SyncReadRecordByIndex(recordInfo);
		if (ret == 0)
		{
			// �f�[�^(���R�[�h)�̓ǂݏo���ɐ���...
			mDbPtr = reinterpret_cast<NNshSubjectDatabase *>(recordBuffer);

			// �ǂݍ��񂾃X���̏������O�o�͂���
			if (checkGetThreadStatus(mDbPtr) == TRUE)
			{
				// �V���m�F����X���������ꍇ...
                ret = getThreadLog(mDbPtr);
				if (ret == NNSH_NORMAL_END)
				{
					// �f�[�^������...
					SyncWriteRec(recordInfo);

					///// �擾�̂��߂�delay������... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
				if (ret == NNSH_RECEIVE_RESPONSE_NG)
				{
					///// �擾�̂��߂�delay������... /////
					if (mNNsiMonaPreferences.waitTime != 0)
					{
						Sleep(mNNsiMonaPreferences.waitTime);
					}
				}
			}
			else
			{
				// �V���m�F���Ȃ��X���������ꍇ...
				//  (���O��VFS�ɂ���΁A���̃��O��PC�̃��O�ɏ㏑������)
			}

			// �C���f�b�N�X���R�[�h���擾����...
			std::string boardNick = mDbPtr->boardNick;
			std::string fileName  = mDbPtr->threadFileName;
			threadIndexRecord *indexRecord;

			long  resNumArray[65536];
			memset(resNumArray, 0x00, sizeof(resNumArray));

			// ���ݎ������擾����
			time_t unixTime = 0;
			time(&unixTime);
			//localtime(&unixTime);
			DWORD dataFileSize = SyncHHToHostDWord(mDbPtr->fileSize);
			DWORD currentUnixTime = unixTime;
			currentUnixTime = currentUnixTime + (9 * 60 * 60);  // 9���Ԏ��Ԃ�i�߂�...

			if ((mDbPtr->bbsType == NNSH_BBSTYPE_2ch)&&(mDbPtr->msgState == FILEMGR_STATE_OPENED_VFS))
			{
				// �����Ń��O���X�V����...
				if (checkVFSVolumes(mDbPtr->boardNick, mDbPtr->threadFileName) == TRUE)
				{
					// ���O��PC�ɃR�s�[����...
					UINT32 fileSize, readSize;
					if (VFSFileSize(mVfsFileRef, &fileSize) == 0)
					{
						char *buf = new char[fileSize + 100];
						if (buf != NULL)
						{
							memset(buf, 0x00, (fileSize + 100));
							if (VFSFileRead(mVfsFileRef, fileSize, buf, &readSize) == 0)
							{
								// �x�[�X�p�X�̐ݒ�...
								std::string basePath;
								apParser->getBasePath(basePath);
								int resNum = 0;

								// ���O���擾�ς݂������ꍇ...
								if (apParser->getIndexRecord(boardNick, fileName, indexRecord) == true)
								{
									// �����Ń��O�t�@�C�����R�s�[����...
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// ���X�ԍ������X�V����...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}

									// ���ǈʒu��ݒ�...
									DWORD curResNum = 0;
									indexRecord->getCurrentResNumber(curResNum);
									if (curResNum < locNum)
									{
										indexRecord->setCurrentResNumber(locNum);
									}
									indexRecord->setAllResCount(maxNum);  // �S���X�� (�T�[�o)
									indexRecord->setResCount(maxNum);     // �S���X�� (���[�J��)
									indexRecord->setFileSize(dataFileSize); // �t�@�C���T�C�Y
									indexRecord->setNewReceivedRes(newResNum);  // ��������V�K�擾
									indexRecord->setNewResCount(newNum);        // �V����
									indexRecord->setRoundDate(currentUnixTime);  // �擾�������L������
									//indexRecord->setScrollTop(resNumArray[locNum]);    // "�����܂œǂ�"�ԍ�
									indexRecord->setReadResNumber(locNum);          // "�����܂œǂ�"�ԍ�
									//indexRecord->setCurrentNumber(0);           // ���݂̔ł̃C���f�b�N�X�ʒu
									indexRecord->setCurrentResNumber(locNum);
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								else
								{
									// �X���^�C�g����ǉ����ă��O�t�@�C�����R�s�[����
									threadIndexRecord *indexRecord = new threadIndexRecord;

									indexRecord->getFileName(fileName);
									std::string threadTitle = mDbPtr->threadTitle;
									indexRecord->setThreadTitle(threadTitle);
									apParser->entryIndexRecord(boardNick, indexRecord);
									resNum = copyLogDatFile(basePath, boardNick, logDataSeparator, indexRecord, buf, readSize, resNumArray, 65536);

									// ���X�ԍ������X�V����...
									DWORD locNum = SyncHHToHostWord(mDbPtr->currentLoc);
									DWORD maxNum = resNum;
									DWORD newNum = maxNum - locNum;
									if (maxNum < locNum)
									{
										newNum = 0;
									}
									DWORD newResNum = SyncHHToHostWord(mDbPtr->maxLoc) + 1;
									if (maxNum < newResNum)
									{
										newResNum = maxNum;
									}
									indexRecord->setCurrentResNumber(locNum);
									indexRecord->setAllResCount(maxNum);  // �S���X�� (�T�[�o)
									indexRecord->setResCount(maxNum);     // �S���X�� (���[�J��)
									indexRecord->setFileSize(dataFileSize); // �t�@�C���T�C�Y
									indexRecord->setNewReceivedRes(newResNum);  // ��������V�K�擾
									indexRecord->setNewResCount(newNum);        // �V����
									indexRecord->setRoundDate(currentUnixTime); // �擾�������L������
									//indexRecord->setScrollTop(resNumArray[locNum]);    // �X�N���[���̐擪
									indexRecord->setReadResNumber(locNum);    // "�����܂œǂ�"�ԍ�
									//indexRecord->setCurrentNumber(0);         // ���݂̔ł̃C���f�b�N�X�ԍ�
									delete indexRecord;
#if 0
		char buffer[400];
		memset(buffer, 0x00, sizeof(buffer));
		sprintf(buffer, "   %s (resNum:%d [%d])", mDbPtr->threadTitle, resNumArray[locNum], locNum);
		LogAddEntry(buffer, slText, false);
#endif
								}
								delete [] buf;
							}
						}
						VFSFileClose(mVfsFileRef);
					}
				}

			}
		}
		mDbPtr = 0;
	}
	apParser->updateIndexRecords();

	// �f�[�^�x�[�X�̃N���[�Y
    SyncCloseDB(rHandle);
}

/*
 *   ���O�t�@�C�����R�s�[����...
 *
 */
int CNNsiMonaSync::copyLogDatFile(std::string &aBasePath, std::string &aBoardNick, char *apSeparator, threadIndexRecord *apRecord, void *apBuffer, long aSize, long *linePtr, long resSize)
{
	int resNum = 0;
	char *ptr, *buf, *base;

	// boardNick...
	char boardNick[96];
	sprintf(boardNick, "%s", aBoardNick.c_str());
	boardNick[strlen(boardNick) - 1] = '\\';  // ������ / �� \ �ɒu������

	// �t�@�C����...
	std::string fileName;
	apRecord->getFileName(fileName);
	if (strlen(fileName.c_str()) < 1)
	{
		// �t�@�C�������擾�ł��Ȃ��ꍇ�ɂ̓��X���[���ŉ�������...
		return (0);
	}

    // full path��������
	char fullPath[1024];
	sprintf(fullPath, "%s%s%s", aBasePath.c_str(), boardNick, fileName.c_str());

	// �t�@�C�����I�[�v������ (�������ݗp)
	HANDLE hFile = CreateFile(fullPath,
		                      GENERIC_WRITE, 
		                      FILE_SHARE_WRITE,
							  NULL, 
							  CREATE_ALWAYS,
							  FILE_ATTRIBUTE_NORMAL, 
							  NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// �t�@�C���I�[�v���Ɏ��s�B�B�B�I������
		return (0);
	}
	SetEndOfFile(hFile);

	DWORD  lines = 0;
	base = static_cast<char *>(apBuffer);
	buf  = base;
	ptr  = buf;
	while (ptr < (base + aSize))
	{
        if (*ptr == '\x0a')
		{
			resNum++;
			if (resNum < resSize)
			{
                linePtr[resNum] = lines;
                linePtr[resNum + 1] = lines;
			}

			DWORD writeSize = 0;
			DWORD length    = 0;

			// ���X�i�{���j���o�͂���
			writeSize = static_cast<DWORD>(ptr - buf);
			length    = writeSize;
			(void) WriteFile(hFile, buf, length, &writeSize, NULL);

			// �Z�p���[�^���o�͂���
			writeSize = static_cast<DWORD>(strlen(apSeparator));
			length    = writeSize;
			(void) WriteFile(hFile, apSeparator, length, &writeSize, NULL);

            // �擪��ݒ肷��...
			buf = ptr + 1;
			lines++;
		}
		if ((*ptr == '<')&&(*(ptr + 1) == '>'))
		{
			lines++;
		}
		if ((*ptr == '<')&&
			((*(ptr + 1) == 'b')||(*(ptr + 1) == 'B'))&&
			((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R'))&&
			((*(ptr + 3) == '>')))
		{
			lines++;
		}
		ptr++;
	}
	// �t�@�C�����N���[�Y����
	CloseHandle(hFile);
	return (resNum);
}
