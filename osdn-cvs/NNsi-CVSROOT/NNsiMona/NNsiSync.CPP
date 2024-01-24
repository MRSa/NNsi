///////////////////////////////////////////////////////
//// NNsiSync.CPP  : NNsi�p�̓����N���X
///////////////////////////////////////////////////////
#define STRICT 1
#define ASSERT(f)     ((void)0)
#define TRACE0(sz)
#define TRACE(sz)
#define HELP_FILE     "hotsync.hlp"

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <sys/stat.h>
#include <TCHAR.H>
#include <COMMCTRL.H>
#include <syncmgr.h>
#include "NNsiMona.h"
#include <logstrng.h>
#include <gensync.h>
#include <PALM_CMN.H>
#include <Pgenerr.h>
#include "resource.h"
#include "hhmgr.h"
#include "NNsiSync.H"
#include "NNsiHHMgr.h"

// NNsi�{�̂̒�`(V1.05 Revision001���)
#include "symbols.h"

// ���̃\�[�X�����������Ă����i�ł��i�D��������A�A�A���ƂŕύX����B�j
#define MAX_PROD_ID_TEXT    255

/******************************************************************************
 * Class:   NNsiCSynchronizer (�R���X�g���N�^)
 *
 *****************************************************************************/
NNsiCSynchronizer::NNsiCSynchronizer(HANDLE langInstance, CSyncProperties& rProps, DWORD dwDatabaseFlags) : m_hLangInstance(langInstance), CSynchronizer(rProps, dwDatabaseFlags = 0)
{
   TRACE0("NNsiCSynchronizer Constructor\n");
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::Perform() (HotSync�����̎��s)
 *
 *****************************************************************************/
long NNsiCSynchronizer::Perform()
{
    long retval  = 0;

    // HotSync�X�^�[�g�̃��O��f��
    NNsiDebugLog("HotSync started...", "", 0);

    if (m_rSyncProperties.m_SyncType > eProfileInstall)
    {
        // ���O��f��
        NNsiDebugLog("GEN_ERR_BAD_SYNC_TYPE", "", GEN_ERR_BAD_SYNC_TYPE);

        // SyncType�̐ݒ�l����������
        return (GEN_ERR_BAD_SYNC_TYPE);
    }

	// �������Ȃ��A�ɐݒ肳��Ă��邩���m�F
    if (m_rSyncProperties.m_SyncType == eDoNothing) 
    {
        // �u�������Ȃ��v���Ƃ����O�ɏo�͂���
		NNsiOutputInfoLog(IDS_DO_NOTHING, "", 0);
        return (0);
    }

    // Obtain System Information
    m_SystemInfo.m_ProductIdText = (BYTE*) new char [MAX_PROD_ID_TEXT];
    if (m_SystemInfo.m_ProductIdText == NULL)
    {
        // ���O��f��
        NNsiDebugLog("GEN_ERR_LOW_MEMORY", "", GEN_ERR_LOW_MEMORY);

        // �������m�ۂ̎��s
        return (GEN_ERR_LOW_MEMORY);
    }
    m_SystemInfo.m_AllocedLen = (BYTE) MAX_PROD_ID_TEXT; 

    retval = SyncReadSystemInfo(m_SystemInfo);
    if (retval != 0)
    {
        // ���O��f��
        NNsiDebugLog("fail SyncReadSystemInfo()", "", retval);

        // �V�X�e�����̓ǂݏo�����s
        return (retval);
    }

	// ���������J�n�O�̏���(NNsiSyncExt.cpp)
	retval = PrologueAllSynchronize();
	if (retval != 0)
	{
		NNsiDebugLog("fail PrologueAllSynchronize()", "", retval);
		return (retval);
	}

	// Register the conduit
    retval = RegisterConduit();
    if (retval != 0)
    {
        // ���O��f��
        NNsiDebugLog("fail RegisterConduit()", "", retval);

        // ���������J�n�̎��s
        return (retval);
    }

    //////////////////////////////////////////////////
	/////////    ���������̎��s(NNsi�p�ɕύX)    /////
    //////////////////////////////////////////////////
	long retval2 = 0;

	// BBS�ꗗ�c�a�̓����������{
	retval2      = ExecuteSynchronize(DBNAME_BBSLIST, DBVERSION_BBSLIST, DBSIZE_BBSLIST);
	if (retval2 != 0)
	{
		// BBS�ꗗ�c�a�̓����������s
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_BBSLIST)", "", retval2);
	}
	retval  = retval2;

	// �X���ꗗ�c�a�̓����������{
	retval2 = ExecuteSynchronize(DBNAME_SUBJECT, DBVERSION_SUBJECT, DBSIZE_SUBJECT);
    if (retval2 != 0)
	{
		// �X���ꗗ�c�a�̓����������s
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_SUBJECT)", "", retval2);
	}
	retval  = retval | retval2;

    // NNsi�ݒ�c�a�̓����������{
	retval2 = ExecuteSynchronize(DBNAME_SETTING, DBVERSION_SETTING, DBSIZE_SETTING);
	if (retval2 != 0)
	{
		// NNsi�ݒ�c�a�̓����������s
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_SETTING)", "", retval2);
	}
	retval  = retval | retval2;

	// �Q�Ɛ�p���O�p�f�B���N�g�����c�a�̓����������{
	retval2 = ExecuteSynchronize(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, DBSIZE_DIRINDEX);
	if (retval2 != 0)
	{
		// �Q�Ɛ�p���O�A�f�B���N�g�����c�a�̓����������s
		NNsiDebugLog("fail. ExecuteSynchronize(DBNAME_DIRINDEX)", "", retval2);
	}
	retval = retval | retval2;

    // Unregister the conduit
    retval2 = UnregisterConduit((BOOL)(retval != 0));

	// ���������J�n��̏���(NNsiSyncExt.cpp)
	long retval3 = 0;
	retval3      = EpilogueAllSynchronize();
	if (retval3 != 0)
	{
		NNsiDebugLog("fail EpilogueAllSynchronize()", "", retval3);
		return (retval3);
	}

	// �����R�[�h�̔���
    if (retval != 0)
    {
        // �ǂ�����HotSync�����s���Ă����ꍇ
        return (retval2);
    }
    return (retval);
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::NNsiDebugLog() (�f�o�b�O�p���O�o��)
 *
 *****************************************************************************/
void NNsiCSynchronizer::NNsiDebugLog(char *message, char *msg2, long value)
{
#ifdef _DEBUG
    char outputData[512];

    if (value != 0)
    {
        sprintf(outputData, "    %s %s 0x%x(%d)", message, msg2, value, value);
    }
    else
    {
        sprintf(outputData, "    %s %s", message, msg2);
    }

    // ���O��f��
    m_pLog->LogInfo(outputData);
#endif
    return;
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::NNsiOutputInfo() (���O���o��)
 *
 *****************************************************************************/
void NNsiCSynchronizer::NNsiOutputInfoLog(UINT strId, char *msg, long value)
{
    TCHAR szBuffer[330], buffer[300], hexBuffer[20];

	// ���\�[�X����e�L�X�g�f�[�^���擾����
    LoadString((HINSTANCE)m_hLangInstance, strId, buffer, sizeof(buffer));

	if (value != 0)
	{
		// ���l��16�i���ɕϊ�
		_itoa(value, hexBuffer, 16);

		// ���O�o�͕�������쐬
		wsprintf(szBuffer, "    %s %s 0x%s(%u)", buffer, msg, hexBuffer, value);
	}
	else
	{
		// ���O�o�͕�������쐬
		wsprintf(szBuffer, "    %s %s", buffer, msg);
	}

    // ���O��f��
    m_pLog->LogInfo(szBuffer);
    return;
}

/*****************************************************************************
 * Method:  PrepareRemoteDBInfo() Palm�f�o�C�X�̂c�a��������
 *
 *****************************************************************************/
long NNsiCSynchronizer::PrepareRemoteDBInfo(char *dbName, long version, long recSize)
{
    long retval = 0;

    if (m_remoteDB)
	{
		delete m_remoteDB;
	}
    m_remoteDB = new CDbList;
    if (m_remoteDB)
	{
        m_remoteDB->m_Creator   = m_rSyncProperties.m_Creator; 
        m_remoteDB->m_DbFlags   = eRecord; // todo
        m_remoteDB->m_DbType    = m_rSyncProperties.m_DbType; 
        strcpy(m_remoteDB->m_Name, dbName);
        m_remoteDB->m_Version   = 0;
        m_remoteDB->m_CardNum   = 0;
        m_remoteDB->m_ModNumber = 0;
        m_remoteDB->m_CreateDate= 0;
        m_remoteDB->m_ModDate   = 0;
        m_remoteDB->m_BackupDate= 0;
    }
	else
	{
		// �������s���G���[
        return (GEN_ERR_LOW_MEMORY);
    }

	// ���R�[�h�T�C�Y���i�[����
    m_remoteDBrecSize = recSize + DBSIZE_MARGIN;
    return (0);
}

/******************************************************************************
 *  Method:   NNsiCSynchronizer::Perform() (HotSync�����̎��s����)
 *
 *****************************************************************************/
long NNsiCSynchronizer::ExecuteSynchronize(char *dbName, long version, long recSize)
{
	long retval = 0;
	char localName[BUFSIZE];

	// �o�b�N�A�b�v����c�a����ݒ肷��
	memset(localName, 0x00, sizeof(localName));
	strcpy(localName, dbName);
	strcat(localName, ".dat");
	strcpy(m_rSyncProperties.m_LocalName, localName);
	NNsiDebugLog("Backup file name is ", m_rSyncProperties.m_LocalName, 0);

	// DB���������̑O����
	retval = PrologueSynchronize(dbName);
    if (retval != 0)
	{
        NNsiDebugLog("fail. PrologueSynchronize() ", dbName, retval);
		return (retval);
	}
	
	// ��������c�a�̂c�a������������
	retval = PrepareRemoteDBInfo(dbName, version, recSize);
	if (retval != 0)
	{
        NNsiDebugLog("fail. GetRemoteDBInfo() " , dbName, retval);
		return (retval);
	}

	// �������s�t���O��Ԃɂ���ď����𕪊�
    switch (m_rSyncProperties.m_SyncType)
    {
      case eFast:
		// ��������
		retval = PerformFastSync();
        if ((retval) && (retval == GEN_ERR_CHANGE_SYNC_MODE))
        {
            if (GetSyncMode() == eHHtoPC)
            {
                retval = CopyHHtoPC();

				// ���O��f��
		        NNsiDebugLog("SyncType:eFast CopyHHtoPC() ", dbName, retval);
            }
            else if (GetSyncMode() == ePCtoHH)
            {
                retval = CopyPCtoHH();

				// ���O��f��
		        NNsiDebugLog("SyncType:eFast CopyPCtoHH() ", dbName, retval);
            }
        }
        // ���O��f��
        NNsiDebugLog("SyncType:eFast ", dbName, retval);
        break;

      case eSlow:
		// �ᑬ����
        retval = PerformSlowSync();  
        if ((retval) && (retval == GEN_ERR_CHANGE_SYNC_MODE))
        {
            if (GetSyncMode() == eHHtoPC)
            {
                retval = CopyHHtoPC();
            }
            else if (GetSyncMode() == ePCtoHH)
            {
                retval = CopyPCtoHH();
            }
        }
        // ���O��f��
        NNsiDebugLog("SyncType:eSlow ", dbName, retval);
        break;

      case eHHtoPC:
      case eBackup:
		// Palm�̃f�[�^���o�b�ɕۊ�
        retval = CopyHHtoPC();
        // ���O��f��
        NNsiDebugLog("SyncType:eHHtoPC/eBackup ", dbName, retval);
        break;

      case eInstall:
      case ePCtoHH:
      case eProfileInstall:
		// PC�̃f�[�^��Palm�ɕۊ�
		retval = CopyPCtoHH();
        // ���O��f��
        NNsiDebugLog("SyncType:eInstall/ePCtoHH/eProfileInstall", dbName, retval);
        break;

      case eDoNothing:
      default:
        // ��L�ȊO�i"�T�|�[�g�O"�̃G���[)
        retval = GEN_ERR_SYNC_TYPE_NOT_SUPPORTED;

        // ���O��f��
        NNsiDebugLog("GEN_ERR_SYNC_TYPE_NOT_SUPPORTED", dbName, retval);
        break;
    }
    // �n���h���n�̍폜
    DeleteHHManager();
    DeletePCManager();
    DeleteBackupManager();
    CloseArchives();

	// DB���������̌㏈��
	retval = EpilogueSynchronize(dbName);
    if (retval != 0)
	{
        NNsiDebugLog("fail. EpilogueSynchronize()", dbName, retval);
		return (retval);
	}
    return (retval);
}

// Palm�Ɠ�������}�l�[�W���̐ݒ�
long NNsiCSynchronizer::CreateHHManager(void)
{
    DeleteHHManager();

    m_dbHH = new NNsiCHHMgr(m_pLog, 
                            m_dwDatabaseFlags,
                            m_remoteDB->m_Name,
                            m_remoteDB->m_Creator,
                            m_remoteDB->m_DbType,
                            m_remoteDB->m_DbFlags,
                            0,
					    	m_remoteDB->m_CardNum,
                            m_rSyncProperties.m_SyncType);

    if (!m_dbHH)
	{
        NNsiDebugLog("fail. CreateHHManager()", m_remoteDB->m_Name, 0);
        return (GEN_ERR_LOW_MEMORY);
	}
    NNsiDebugLog("CreateHHManager()", m_remoteDB->m_Name, 0);
    return (m_dbHH->Open());
}

long NNsiCSynchronizer::SynchronizeAppInfo(void)
{
	// AppInfo�͓��������Ȃ��B�B�B
    return (0);
}

long NNsiCSynchronizer::CopyAppInfoHHtoPC(void)
{
	// AppInfo�͓��������Ȃ��B�B�B
    return (0);
}

long NNsiCSynchronizer::CopyAppInfoPCtoHH(void)
{
    // AppInfo�͓��������Ȃ��B�B�B
	return (0);
}