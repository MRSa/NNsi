/*============================================================================*
 *  FILE: 
 *     nnshmain.c
 *
 *  Description: 
 *     Main routines for NNsh. 
 *
 *===========================================================================*/
#define THRMNG_C
#include "local.h"

static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR);
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS);
static Err create_BBS_INDEX(Char **bdLst, UInt16 *cnt);

static Boolean set_SearchTitleString     (void);
static Boolean search_NextTitle          (Int16 direction);
static Boolean display_message           (void);
static Boolean get_all_message           (void);
static Boolean update_message            (void);
static Boolean info_message              (void);
static Boolean update_getall_message     (void);
static Boolean redraw_Thread_List        (UInt16 bbsIndex, UInt16 threadIndex);
static Boolean delete_notreceived_message(void);
static Boolean delete_message            (void);
static Boolean get_subject_txt           (void);
static Boolean popEvt_ThreadMain         (EventType *event);
static Boolean keyDownEvt_ThreadMain     (EventType *event);
static Boolean ctlSelEvt_ThreadMain      (EventType *event);
static Boolean menuEvt_ThreadMain        (EventType *event);

extern void    Initialize_NNshSetting(FormType *frm);
extern void    Effect_NNshSetting    (FormType *frm);
extern Boolean Handler_NNshSetting   (EventType *event);
extern Boolean Handler_JumpSelection(EventType *event);

/*=========================================================================*/
/*   Function :   Hander_EditAction                                        */
/*                                         �ҏW�����p�̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_EditAction(EventType *event)
{
    // ���݂̃t�H�[�����擾
    switch (event->eType)
    { 
      // ���j���[�I��
      case menuEvent:
        return (NNsh_MenuEvt_Edit(event));
        break;

      case ctlSelectEvent:
      default: 
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   open_menu                                                */
/*                                                 NNsi�Ǝ��̃��j���[�\��  */
/*-------------------------------------------------------------------------*/
static Boolean open_menu(void)
{
    FormType         *frm;
    ControlType      *ctlP;
    EventType        *dummyEvent;
    Char             *ptr, listPtr[BUFSIZE];
    UInt16            btnId;

    // ���X�g�̍쐬
    MemSet(listPtr, BUFSIZE, 0x00);
    ptr = listPtr;

    // BBS��I��
    StrCopy(ptr, NNSH_JUMPMSG_OPENBBS);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENBBS);

    // ���j���[���J��
    StrCopy(ptr, NNSH_JUMPMSG_OPENMENU);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENMENU);

    // �V���m�F
    StrCopy(ptr, NNSH_JUMPMSG_OPENNEW);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENNEW);

    // �ꗗ�擾
    StrCopy(ptr, NNSH_JUMPMSG_OPENLIST);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENLIST);

    // �X���擾
    StrCopy(ptr, NNSH_JUMPMSG_OPENGET);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENGET);

    // �X�����
    StrCopy(ptr, NNSH_JUMPMSG_OPENINFO);
    ptr = ptr + sizeof(NNSH_JUMPMSG_OPENINFO);

    // �I���E�B���h�E��\������
    btnId = NNshWinSelectionWindow(listPtr, NNSH_JUMPSEL_NUMLIST);
    if (btnId != BTNID_JUMPCANCEL)
    {
        // �I�����X�g�ԍ����擾
        switch (NNshGlobal->jumpSelection)
        {
          case NNSH_JUMPSEL_OPENBBS:
            // BBS�ꗗ��I��(�|�b�v�A�b�v)
            frm  = FrmGetActiveForm();
            ctlP = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,POPTRID_BBSINFO));
            CtlHitControl(ctlP);
            break;

          case NNSH_JUMPSEL_OPENMENU:
            // ���j���[���J��
            // (�_�~�[�ō��㕔����penDownEvent�𐶐�)
            dummyEvent = &(NNshGlobal->dummyEvent);
            MemSet(dummyEvent, sizeof(EventType), 0x00);
            dummyEvent->eType    = penDownEvent;
            dummyEvent->penDown  = true;
            dummyEvent->tapCount = 1;
            dummyEvent->screenX  = 5;
            dummyEvent->screenY  = 5;
            EvtAddEventToQueue(dummyEvent);
            break;

          case NNSH_JUMPSEL_OPENNEW:
            // �V���m�F
            (void) update_newarrival_message();
            break;

          case NNSH_JUMPSEL_OPENLIST:
            // �ꗗ�X�V
            (void) get_subject_txt();
            break;

          case NNSH_JUMPSEL_OPENGET:
            // ���b�Z�[�W�擾(�����擾/�S���擾�͎������f)
            (void) update_getall_message();
            break;

          case NNSH_JUMPSEL_OPENINFO:
            // �X�����\��
            (void) info_message();
            break;

          default:
            // �������Ȃ�
            break;
        }
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function : NNshMain_Close                                             */
/*                                                      �ꗗ��ʂ̃N���[�Y */
/*-------------------------------------------------------------------------*/
static void NNshMain_Close(UInt16 nextFormID)
{
    // �X���ꗗ�̗̈�͈�U�J������(��ʂ��J�����Ƃ��ɕK���č\�z���邽��)
    if (NNshGlobal->threadTitles != NULL)
    {
        MemHandleUnlock(NNshGlobal->msgTTLStringH);
        NNshGlobal->threadTitles = NULL;
    }
    MEMFREE_HANDLE(NNshGlobal->msgTTLStringH);

    // �ʂ̉�ʂ��J��
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   restoreDatabaseToVFS                                     */
/*                                           NNsi��DB��VFS���烊�X�g�A���� */
/*-------------------------------------------------------------------------*/
static Err restoreDatabaseToVFS(void)
{
    Err      ret;
    FileRef  fileRef;
    Char     fileName[MAXLENGTH_FILENAME];
    UInt32   dummy;
    UInt16   butId;

    // VFS�̃T�|�[�g�L�����m�F����B
    dummy = 0;
    ret = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &dummy);
    if (ret != errNone)
    {
        // VFS��T�|�[�g�̏ꍇ�ɂ́A���I���B
        NNsh_DebugMessage(ALTID_INFO, "Not Support VFS :", "", ret);
        return (~errNone);
    }

    // VFS���g�p�ł���Ƃ��ABackup���ꂽDB�����邩(FILE��OPEN����)�m�F
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, DBNAME_SUBJECT);
    StrCat (fileName, ".pdb");
    if (VFSFileOpen(NNshGlobal->vfsVol, fileName, vfsModeRead, &fileRef) !=
        errNone)
    {
        // BACKUP���ꂽ�t�@�C�����Ȃ��̂ŏI������
        NNsh_DebugMessage(ALTID_INFO, "Backup DB files do not exist", "", 0);
        return (~errNone);
    }
    // File Open����(== DB������)�A���X�g�A���邩�m�F����
    VFSFileClose(fileRef);
    butId = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_RESTORE, "", 0);
    if (butId != 0)
    {
        // Cancel���I�����ꂽ���߁A���X�g�A���Ȃ��B
        NNsh_DebugMessage(ALTID_INFO, "DB-Restore is canceled.", "", 0);
        return (~errNone);
    }

    // "���X�g�A��"�\��
    Show_BusyForm(MSG_DBRESTORE_BUSY);

    // DB���X�g�A���C��
    (void) RestoreDatabaseFromVFS_NNsh(DBNAME_SETTING);
    (void) RestoreDatabaseFromVFS_NNsh(DBNAME_DIRINDEX);
    ret  = RestoreDatabaseFromVFS_NNsh(DBNAME_BBSLIST);
    ret  = (ret)|(RestoreDatabaseFromVFS_NNsh(DBNAME_SUBJECT));

    // "���X�g�A��"�\���̍폜
    Hide_BusyForm();

    // DB�̃��X�g�A���ʕ\��
    if (ret == errNone)
    {
        // ���X�g�A����
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_OK, "", 0);
    }
    else
    {
        // ���X�g�A���s
        NNsh_InformMessage(ALTID_INFO, MSG_DBRESTORE_NG, "", 0);
    }
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   copy_to_readOnly                                         */
/*                                  ���O�t�@�C�����Q�Ɛ�p���O�ɃR�s�[���� */
/*-------------------------------------------------------------------------*/
static Boolean copy_to_readOnly(void)
{
    Err                  ret;
    UInt16               selM, selBBS, butID;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;
    Char                *fileName, *orgName, *area;

    // �{�@�\�́AVFS��p�ł���|�\������(VFS�ȊO�͏������s��Ȃ�)
    if ((NNshParam->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        NNsh_ErrorMessage(ALTID_WARN, MSG_ONLYVFS_FEATURE, "", 0);
        return (false);
    }

    // �o�b�t�@�̊m��
    fileName = MemPtrNew(MAXLENGTH_FILENAME);
    if (fileName == NULL)
    {
        return (false);
    }
    orgName  = MemPtrNew(MAXLENGTH_FILENAME);
    if (orgName == NULL)
    {
        MemPtrFree(fileName);
        return (false);
    }

    // �X�������擾����
    ret = get_message_Info(&mesInfo, &selM, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }
    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        //  �X�����擾�̂��߁A�R�s�[�͍s��Ȃ�
        NNsh_InformMessage(ALTID_WARN, MSG_NOTGET_MESSAGE,
                           mesInfo.threadTitle, 0);
        ret = ~errNone;
        goto FUNC_END;
    }

    // �R�s�[��f�B���N�g�����쐬����
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);

    // ���O�x�[�X�f�B���N�g����t������
    area = NULL;
    if (NNshGlobal->logPrefixH != 0)
    {
        area = MemHandleLock(NNshGlobal->logPrefixH);
    }
    if (area == NULL)
    {
        StrCopy(fileName, LOGDATAFILE_PREFIX);
    }
    else
    {
        StrCopy(fileName, area);
        MemHandleUnlock(NNshGlobal->logPrefixH);
    }
    StrCat (fileName, mesInfo.boardNick);
    fileName[StrLen(fileName) - 1] = '\0';
    (void) CreateDir_NNsh(fileName);

    // �R�s�[��t�@�C�������쐬����
    StrCat (fileName, "/");
    StrCat (fileName, mesInfo.threadFileName);

    // �R�s�[���t�@�C�������쐬����
    MemSet (orgName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(orgName, mesInfo.boardNick);
    StrCat (orgName, mesInfo.threadFileName);

    // �t�@�C����{���ɃR�s�[���邩���m�F����
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_COPY,
                                mesInfo.threadTitle, 0);
    if (butID != 0)
    {
        // Cancel��I��(�������Ȃ�)
        ret = ~errNone;
        goto FUNC_END;
    }

    // �R�s�[�̎��{
    NNsh_DebugMessage(ALTID_INFO, "Dest :",   fileName, 0);
    NNsh_DebugMessage(ALTID_INFO, "Source :", orgName, 0);
    Show_BusyForm(MSG_COPY_IN_PROGRESS);

    if (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        // Palm�{�̂���VFS�̎Q�Ɛ�p���O�փR�s�[����
        ret = CopyFile_NNsh(fileName, orgName, NNSH_VFS_DISABLE);
    }
    else
    {
        // VFS�̃��O����VFS�̎Q�Ɛ�p���O�փR�s�[����
        ret = CopyFile_NNsh(fileName, orgName, NNSH_VFS_ENABLE);
    }
    Hide_BusyForm();
    if (ret != errNone)
    {
        // �t�@�C���R�s�[�Ɏ��s�����B
        (void) DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);

        // �R�s�[�Ɏ��s�����|�\������
        NNsh_ErrorMessage(ALTID_ERROR, MSG_FAILURE_COPY, fileName, ret);
        goto FUNC_END;
    }

    // �R�s�[�ɐ����A�Q�Ɛ�p���O�ɕ\������Ƃ��ɂ�scan�K�v���x��
    NNsh_InformMessage(ALTID_INFO, MSG_INFORM_COPYEND, "", 0);

FUNC_END:
    MemPtrFree(orgName);
    MemPtrFree(fileName);
    if (ret != errNone)
    {
        return (false);
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_MessageFromMesNum                                    */
/*                                            �X���ԍ��w��̃��b�Z�[�W�擾 */
/*-------------------------------------------------------------------------*/
static Boolean get_MessageFromMesNum(void)
{
    Boolean              rep;
    Err                  ret;
    UInt16               btnId, index, dataIndex;
    UInt32               size;
    DmOpenRef            dbRef;
    Char                *url, *ptr, *nPtr;
    FormType            *prevFrm, *diagFrm;
    NNshBoardDatabase   bbsData;
    NNshSubjectDatabase subjDb;

    // ���[�N�o�b�t�@�̊m�ۂƏ�����
    url     = MemPtrNew(BUFSIZE);
    if (url == NULL)
    {
        return (false);
    }
    index = 0;
    MemSet(&subjDb,  sizeof(NNshSubjectDatabase), 0x00);
    MemSet(&bbsData, sizeof(NNshBoardDatabase),   0x00);
    MemSet(url,     BUFSIZE,                      0x00);

    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if ((FtrGet(sysFtrCreator, sysFtrNumROMVersion, &size) == errNone)
        &&(size < 0x03503000))
    {
        FrmEraseForm(prevFrm);
    }
    diagFrm  = FrmInitForm(FRMID_GETMESSAGE_NUMBER);
    if (diagFrm == NULL)
    {
        rep = false;
        goto FUNC_END;
    }

    // �V���N�\�����ő剻
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(diagFrm);
    }
    HandEraMoveDialog(diagFrm);
    FrmSetActiveForm(diagFrm);

    // �J�[�\������͗��Ɉړ�����
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_BBS_NICK));

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    // �_�C�A���O�̃I�[�v���A������̓��͂�҂�
    btnId = FrmDoDialog(diagFrm);

    // Window����(���͂��ꂽ)��������擾����
    NNshWinGetFieldText(diagFrm, FLDID_BBS_NICK, url, BUFSIZE);

    // �_�C�A���O�����
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // �V���N�\�����ŏ���
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    FrmDrawForm(prevFrm);

    // �����ꂽ�{�^���̃`�F�b�N
    if (btnId == BTNID_GET_CANCEL)
    {
        // Cancel�{�^���������ꂽ�Ƃ��ɂ́A���������I������B
        rep = false;
        goto FUNC_END;
    }

    // ���̓f�[�^�����Nick��؂�o��
    ptr  = url;
    nPtr = subjDb.boardNick;
    while (*ptr != '/')
    { 
        *nPtr = *ptr;
        ptr++;
        nPtr++;
        if (*ptr == '\0')
        {
            // �f�[�^�擾�G���[�A�I������
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_GETFAILURE, "", 0);
            rep = false;
            goto FUNC_END;
        }
    }
    *nPtr = '/';    // ��Nick�̍Ō�� '/'��t�����Ă���

    // �X���ԍ���؂�o��
    ptr++;
    nPtr = subjDb.threadFileName;
    while ((*ptr != '/')&&(*ptr != '\0'))
    {
        *nPtr = *ptr;
	nPtr++;
        ptr++;
    }

    // BBS�ꗗ�ǂݍ��ݒ��̃��b�Z�[�W��\������
    Show_BusyForm(MSG_READ_BBSLIST_WAIT);

    // ���̎擾
    ret = Get_BBS_Database(subjDb.boardNick, &bbsData, &index);
    if (ret != errNone)
    {
        // ���擾�Ɏ��s�A�G���[�\�����ďI��
        NNsh_ErrorMessage(ALTID_ERROR, subjDb.boardNick,
                          MSG_WRONG_BOARDNICK, ret);
        Hide_BusyForm();
        rep = false;
        goto FUNC_END;
    }

    // ���b�Z�[�W�̋L��
    MemSet (url, BUFSIZE, 0x00);
    StrCopy(url, bbsData.boardName);
    StrCat (url, MSG_CONFIRM_GETTHREAD);
    StrCat (url, subjDb.threadFileName);
    StrCat (url, ")");

    // ���b�Z�[�W�̍폜
    Hide_BusyForm();

    // �{���ɃX�����擾���邩�m�F���s��
    btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, url, "", 0);
    if (btnId != 0)
    {
        // �擾���L�����Z������
        NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
        rep = false;
        goto FUNC_END;
    }

    // �X�����擾���̃��b�Z�[�W��\������
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    // ���b�Z�[�W�擾��̍쐬
    MemSet (url, BUFSIZE, 0x00);
    StrCopy(url, bbsData.boardURL);
    switch (bbsData.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂�BBS����f�[�^�擾(HTML��ǂ�)
        StrCat (url, "bbs/read.pl?BBS=");
        StrCat (url, bbsData.boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, subjDb.threadFileName);

        StrCat (subjDb.threadFileName, ".cgi");   // �g���q������

        //////////////////////////////////////////////////////////////////////
        // ���݁A�܂��a�a�r�ɂ͂��̋@�\�͎g�p�ł��Ȃ��|��\�����ďI������B
        NNsh_InformMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI,"",0);
        rep = false;
        goto FUNC_END;
        //////////////////////////////////////////////////////////////////////
        break;

      case NNSH_BBSTYPE_2ch:
      default:
        // 2ch����f�[�^�擾(dat��ǂ�)
        StrCat (subjDb.threadFileName, ".dat");   // �g���q������

        StrCat (url, "dat/");
        StrCat (url, subjDb.threadFileName);
        break;
    }

    // �X���f�[�^���擾�ς݂��m�F����B
    if (check_same_thread(bbsData.threadCnt, &subjDb, &subjDb, &dataIndex)
                                                                    == errNone)
    {
        Hide_BusyForm();      // ���b�Z�[�W�̍폜

        // �擾�ς݂Ɣ����A�����擾�ɐ؂�ւ���
        ret = Get_PartMessage(url, bbsData.boardNick, &subjDb, 
                              convertListIndexToMsgIndex(dataIndex));

        // �擾�I���̕\��(DEBUG�I�v�V�����L�����̂�)
        NNsh_InformMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                           subjDb.threadTitle, ret);

        // �擪��\������
        rep = redraw_Thread_List(NNshParam->lastBBS, 0);
        goto FUNC_END;
    }

    ////////////  ���b�Z�[�W�S���擾�̏���  ///////////

    /////////////////////////////////////////////////////////////////////////
    // �f�[�^�x�[�X�ɃX���f�[�^��(��)�ǉ����� ���X���^�C�g���͌�Œǉ����遦
    /////////////////////////////////////////////////////////////////////////
    StrCopy(subjDb.threadTitle, "(Unknown)");

    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // ���b�Z�[�W�̍폜
        Hide_BusyForm();

        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "OpenDatabase_NNsh()", "", 0);
        rep = false;
        goto FUNC_END;
    }
    EntryRecord_NNsh(dbRef, sizeof(NNshSubjectDatabase), &subjDb);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    /** (VFS���)�f�B���N�g�����쐬���� **/
    (void) CreateDir_NNsh(bbsData.boardNick);
    
    // ���b�Z�[�W�̍폜
    Hide_BusyForm();

    // ���O�擾����ыL�^����
    ret = Get_AllMessage(url, bbsData.boardNick, subjDb.threadFileName,
                         0, (UInt16) bbsData.bbsType);

    if (ret != errNone)
    {
        // �X���t�@�C������ �ϐ�url�ɏ����o���A(�����O��)�X���t�@�C�����폜
        MemSet (url, BUFSIZE, 0x00);
        StrCopy(url, bbsData.boardNick);
        StrCat (url, subjDb.threadFileName);
        if ((NNshParam->useVFS & NNSH_VFS_ENABLE) == 0)
        {
            // Palm�{�̏�̃��O���폜����
            ret = DeleteFile_NNsh(url, NNSH_VFS_DISABLE);
        }
        else
        {
            // VFS��̃��O���폜����
            ret = DeleteFile_NNsh(url, NNSH_VFS_ENABLE);
        }

        // �G���[����(���ǉ������f�[�^�x�[�X�f�[�^���폜����)
        (void) OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
        DeleteRecordIDX_NNsh(dbRef, 0);
        CloseDatabase_NNsh(dbRef);
        NNsh_DebugMessage(ALTID_ERROR, "Get_AllMessage()", "", ret);
        rep = false;
        goto FUNC_END;
    }

    //////////////////�X���^�C�g���擾����/////////////////////////

    // �X�����擾���̃��b�Z�[�W��\������
    Show_BusyForm(MSG_MESSAGE_INFO_WAIT);

    // �X�������擾����
    (void) Get_Subject_Database(0, &subjDb);

    // �t�@�C���T�C�Y���ꎞ�I�ɋL��
    size = subjDb.fileSize;

    // �X���t�@�C������ �ϐ�url�ɏ����o��
    MemSet (url, BUFSIZE, 0x00);
    StrCopy(url, bbsData.boardNick);
    StrCat (url, subjDb.threadFileName);

    // �t�@�C������X�������擾����
    ret = SetThreadInformation(url, (NNSH_FILEMODE_READONLY), &subjDb);
    Hide_BusyForm();  // ���b�Z�[�W�̍폜
    if (ret != errNone)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "SetThreadInformation()\nFileName:",
                          subjDb.threadFileName, ret);
        // ���^�[�������ɏ������p������
    }

    // �t�@�C���T�C�Y�������߂�
    subjDb.fileSize = size;

    // ���R�[�h�̓o�^
    ret = update_subject_database(0, &subjDb);
    if (ret != errNone)
    {
        // �G���[����
        NNsh_DebugMessage(ALTID_ERROR, "update_subject_database()", "", ret);
        rep = false;
        goto FUNC_END;
    }

    // �擾�I���̕\��
    NNsh_InformMessage(ALTID_INFO,MSG_GET_MESSAGE_END,subjDb.threadTitle,ret);

    // ���VISIBLE�ł͂Ȃ��ꍇ�AVISIBLE�ɂ��邩�m�F����
    btnId = 1;
    if (bbsData.state != NNSH_BBSSTATE_VISIBLE)
    {
        btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, bbsData.boardName,
                                    MSG_CONFIRM_SETVISIBLE, 0);
        if (btnId == 0)
        {
            // BBS��VISIBLE�ɕύX����iCONFIRMATION�� OK��I�������ꍇ�j
            bbsData.state = NNSH_BBSSTATE_VISIBLE;
        }
    }

    // �擾�X�������J�E���g�A�b�v����
    dataIndex = bbsData.threadCnt;
    bbsData.threadCnt = NNSH_BBSLIST_AGAIN;

    // BBS���̍X�V
    Update_BBS_Database(index, &bbsData);

    // �g�pBBS�ꗗ���X�V���ꂽ�Ƃ�
    if (btnId == 0)
    {
        // BBS�ꗗ��������
        MEMFREE_PTR(NNshGlobal->bbsTitles);

        // BBS�ꗗ�̍X�V
        (void) create_BBS_INDEX(&(NNshGlobal->bbsTitles), &index);

        // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B
        NNshWinSetListItems(prevFrm, LSTID_BBSINFO,
                            NNshGlobal->bbsTitles, index,
                            NNshParam->lastBBS, &(NNshGlobal->bbsTitleH));
    }

    // �X���ꗗ&BBS�ꗗ���ĕ\������
    rep = redraw_Thread_List(NNshParam->lastBBS, (dataIndex + 1));

FUNC_END:
    // �֐��̏I���A
    MEMFREE_PTR(url);
    return (rep);
}

/*-------------------------------------------------------------------------*/
/*   Function :   set_SearchTitleString                                    */
/*                                                �X���^�C����������̐ݒ� */
/*-------------------------------------------------------------------------*/
static Boolean set_SearchTitleString(void)
{
    Boolean   ret = false;
    FormType *prevFrm, *diagFrm;
    Char     *ptr = NULL;
    UInt16    btnId;
    UInt32    romVersion;

    // ����������̈�̎擾
    if (NNshGlobal->searchTitleH == 0)
    {
        NNshGlobal->searchTitleH = MemHandleNew(BUFSIZE);
        if (NNshGlobal->searchTitleH == 0)
        {
            return (false);
        }
        ptr = MemHandleLock(NNshGlobal->searchTitleH);
        if (ptr != NULL)
        {
            MemSet(ptr, BUFSIZE, 0x00);
        }
    }
    else
    {
        ptr = MemHandleLock(NNshGlobal->searchTitleH);
    }
    if (ptr == NULL)
    {
        // �̈�擾���s
        MemHandleFree(NNshGlobal->searchTitleH);
        NNshGlobal->searchTitleH = 0;
        return (false);
    }

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    prevFrm = FrmGetActiveForm();

    if ((FtrGet(sysFtrCreator,sysFtrNumROMVersion,&romVersion) == errNone)
        &&(romVersion < 0x03503000))
    {
        FrmEraseForm(prevFrm);
    }
    diagFrm  = FrmInitForm(FRMID_SEARCHTITLE);
    if (diagFrm == NULL)
    {
        MemHandleUnlock(NNshGlobal->searchTitleH);
        return (false);
    }

    // �V���N�\�����ő剻
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        VgaFormModify(diagFrm, vgaFormModify160To240);
        SilkMaximizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMaximize(diagFrm);
    }
    HandEraMoveDialog(diagFrm);
    FrmSetActiveForm(diagFrm);

    // ���݂̌����ݒ�p�����[�^���E�B���h�E�ɔ��f������
    SetControlValue(diagFrm, CHKID_CASELESS,
                                  &(NNshParam->searchCaseless));

    // ���݂̌���������ݒ���E�B���h�E�ɔ��f������
    NNshWinSetFieldText(diagFrm, FLDID_SEARCHTITLE, ptr, StrLen(ptr));
    FrmSetFocus(diagFrm, FrmGetObjectIndex(diagFrm, FLDID_SEARCHTITLE));

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_EditAction);

    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_SEARCHTITLE_OK)
    {
        // OK�{�^���������ꂽ�Ƃ��ɂ́A������������擾����
        NNshWinGetFieldText(diagFrm, FLDID_SEARCHTITLE, ptr, BUFSIZE);

        // ���݂̌����ݒ�p�����[�^���p�����[�^�ɔ��f������
        UpdateParameter(diagFrm, CHKID_CASELESS,
                             &(NNshParam->searchCaseless));
        ret = true;
        NNsh_DebugMessage(ALTID_INFO,"String:", ptr, 0);
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    // �V���N�\�����ŏ���
#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        SilkMinimizeWindow();
    }
    else
#endif // #ifdef USE_HANDERA
    {
        NNshSilkMinimize(prevFrm);
    }
    FrmDrawForm(prevFrm);
    MemHandleUnlock(NNshGlobal->searchTitleH);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   search_NextTitle                                         */
/*                                                        �X���^�C�������� */
/*-------------------------------------------------------------------------*/
static Boolean search_NextTitle(Int16 direction)
{
    Err                  ret;
    Char                 msg[BUFSIZE], match1[BUFSIZE], match2[MAX_THREADNAME];
    Char                *ptr;
    UInt16               selMES, selBBS, index, loop, end;
    Int16                step;
    NNshSubjectDatabase  mesInfo;
    NNshBoardDatabase    bbsInfo;

    // check if title string is already set or not.
    if (NNshGlobal->searchTitleH == 0)
    {
        return (false);
    }

    // ������������擾
    ptr = MemHandleLock(NNshGlobal->searchTitleH);
    if (ptr == NULL)
    {
        return (false);
    }

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // �X�������̎��{
    MemSet (msg, sizeof(msg), 0x00);
    StrCopy(msg, MSG_SEARCHING_TITLE);
    StrCat (msg, ptr);
    Show_BusyForm(msg);

    if (direction == NNSH_SEARCH_FORWARD)
    {
        // �t�H���[�h�����w��
        end  = NNshGlobal->threadCount;
        step = 1;
    }
    else
    {
        // �o�b�N���[�h�����w��
        end  =  0;
        step = -1;
    }

    // �������ɕϊ�
    MemSet(match1, sizeof(match1), 0x00);
    if (NNshParam->searchCaseless != 0)
    {
        (void) StrToLower(match1, ptr);
    }
    else
    {
        (void) StrCopy(match1, ptr);
    }

    // �����̎�����
    for (loop = selMES + step; loop != end; loop = loop + step)
    {
        index = convertListIndexToMsgIndex(loop);
        ret = Get_Subject_Database(index, &mesInfo);
        if (ret != errNone)
        {
            // ���R�[�h�̎擾�Ɏ��s
            continue;
        }

        //  �w�肳�ꂽ������ƃ}�b�`���邩�m�F
        MemSet(match2, sizeof(match2), 0x00);
        if (NNshParam->searchCaseless != 0)
        {
            // �X���^�C���������ɕϊ�
            (void) StrToLower(match2, mesInfo.threadTitle);
        }
        else
        {
            (void) StrCopy(match2, mesInfo.threadTitle);
        }

        if (StrStr(match2, match1) != NULL)
        {
            Hide_BusyForm();

            // �X���ꗗ�̕\���ӏ���ύX
            (void) redraw_Thread_List(selBBS, loop);
            MemHandleUnlock(NNshGlobal->searchTitleH);
            return (true);
        }
    }
    Hide_BusyForm();

FUNC_END:
    // �����Ɏ��s����
    NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ptr, 0);
    MemHandleUnlock(NNshGlobal->searchTitleH);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   create_BBS_INDEX                                         */
/*                                                      BBS�ꗗ�̍쐬����  */
/*-------------------------------------------------------------------------*/
static Err create_BBS_INDEX(Char **bdLst, UInt16 *cnt)
{
    Err                   ret = errNone;
    DmOpenRef             bbsRef;
    UInt16                nofBBS, tempSize, lp, useCnt, *idxP;
    Char                 *ptr;
    NNshBoardDatabase     tmpBd;
    NNshBBSRelation      *relPtr;

    // �^����ꂽ *bdLst �� NULL�łȂ���΁A(BBS�ꗗ�\�z�ς݂ƍl��)�������Ȃ�
    if (*bdLst != NULL)
    {
        *cnt = NNshGlobal->useBBS;
        NNsh_DebugMessage(ALTID_INFO,"Already created BBS-name-LIST","",*cnt);
        return (errNone);
    }

    // "BBS�ꗗ�쐬��"�̕\��
    Show_BusyForm(MSG_READ_BBS_WAIT);

    // BBS�f�[�^�x�[�X�̃I�[�v��
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
    GetDBCount_NNsh(bbsRef, &nofBBS);
    if (nofBBS == 0)
    {
        // "�쐬��"�\���̍폜
        Hide_BusyForm();
        CloseDatabase_NNsh(bbsRef);

        // VFS����c�a�����X�g�A(�����\�Ȃ�)
        ret = restoreDatabaseToVFS();
        if (ret != errNone)
        {
            // BBS�f�[�^�x�[�X�����݂��Ȃ������ꍇ�A�x����\������
            NNsh_ErrorMessage(ALTID_WARN,
                              MSG_BOARDDB_NONE1, MSG_BOARDDB_NONE2, 0);
        }
        return (~errNone);
    }

    // �g�p��BBS�����J�E���g����
    useCnt = 0;
    for (lp = 0; lp < nofBBS; lp++)
    {
        (void) GetRecord_NNsh(bbsRef, lp, sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            useCnt++;
        } 
    }
    if (useCnt == 0)
    {
        // "�쐬��"�\���̍폜
        Hide_BusyForm();
        CloseDatabase_NNsh(bbsRef);

        // �g�p����f�[�^�x�[�X���ݒ肳��Ă��Ȃ������ꍇ�A�x����\������
        NNsh_ErrorMessage(ALTID_WARN,
                          MSG_USEBOARD_WARN1, MSG_USEBOARD_WARN2, 0);
        return (~errNone);
    }

    // BBS�^�C�g���C���f�b�N�X�p�̗̈���m�ۂ���
    if (NNshGlobal->boardIdxH != 0)
    {
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;
    }
    tempSize      = sizeof(NNshBBSRelation) +
                       sizeof(UInt16) * (useCnt + NNSH_NOF_SPECIAL_BBS);
    NNshGlobal->boardIdxH = MemHandleNew(tempSize);
    if (tempSize == 0)
    {
       // "�쐬��"�\���̍폜
       Hide_BusyForm();

       // �̈�m�ۃG���[
       ret = ~errNone;
       NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                          " size:", tempSize);

       // �f�[�^�x�[�X�̃N���[�Y
       CloseDatabase_NNsh(bbsRef);
       return (~errNone);
    }
    relPtr = MemHandleLock(NNshGlobal->boardIdxH);
    idxP   = &(relPtr->bbsIndex);

    // BBS�^�C�g���ꗗ�p�̗̈���m�ۂ���
    tempSize = sizeof(Char) *
                     (useCnt + NNSH_NOF_SPECIAL_BBS) * (LIMIT_BOARDNAME + 1);
    *bdLst   = (Char *) MemPtrNew(tempSize);
    if (*bdLst == NULL)
    {
        MemHandleUnlock(NNshGlobal->boardIdxH);
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;

        // "�쐬��"�\���̍폜
        Hide_BusyForm();

        // �̈�m�ۃG���[
        ret = ~errNone;
        NNsh_InformMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                           " size:", tempSize);

        // �f�[�^�x�[�X�̃N���[�Y
        CloseDatabase_NNsh(bbsRef);
        return (~errNone);
    }
    MemSet(*bdLst, tempSize, 0x00);
    ptr = *bdLst;

    // BBS�ꗗ�̐擪�Ɂu���C�ɓ���v�u�擾�ςݑS�āv�u���ǂ���v��ǉ�����
    StrCopy(ptr, FAVORITE_THREAD_NAME);
    ptr   = ptr + sizeof(FAVORITE_THREAD_NAME); // Terminator���܂߂čl���Ă�
    *cnt  = 1;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, GETALL_THREAD_NAME);
    ptr   = ptr + sizeof(GETALL_THREAD_NAME); // Terminator���܂߂čl���Ă�
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, NOTREAD_THREAD_NAME);
    ptr   = ptr + sizeof(NOTREAD_THREAD_NAME); // Terminator���܂߂čl���Ă�
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    // (�g�p����)BBS�^�C�g���ꗗ�ƃC���f�b�N�X���쐬����
    for (lp = 0; lp < nofBBS; lp++)
    {
        MemSet(&tmpBd, sizeof(tmpBd), 0x00);
        (void) GetRecord_NNsh(bbsRef, lp, sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            // ���̂��R�s�[����
            StrNCopy(ptr, tmpBd.boardName, LIMIT_BOARDNAME);
            ptr = ptr + StrLen(ptr) + 1;   // 1 ��'\0'�̈Ӗ��A(���X�g�Ŏg�p)
            (*cnt)++;
            *idxP = lp;
            idxP++;
	}
    }
    NNshGlobal->useBBS = *cnt;
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // "�쐬��"�\���̍폜
    Hide_BusyForm();

    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(bbsRef);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_info                                         */
/*                                                   ���b�Z�[�W���̎擾  */
/*-------------------------------------------------------------------------*/
static Err get_subject_info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR)
{
    Err        ret;
    UInt16     index;
    FormType  *frm;
    ListType  *lstP;

    // ���b�Z�[�W���̏�����
    *selTHR = 0;

    if (NNshGlobal->threadIdxH == 0)
    {
        // �X���C���f�b�N�X�̎擾���ł��Ȃ�
        NNsh_DebugMessage(ALTID_ERROR, 
                          "the thread-index is not allocated.", "", 0);
        return (~errNone);
    }

    // �I������Ă���X���̃��X�g�ԍ����擾����
    frm  = FrmGetActiveForm();
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
    *selTHR = LstGetSelection(lstP);
    NNshParam->openThreadIndex = *selTHR;

    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(*selTHR);
    NNshParam->openMsgIndex    = index;

    // �X�������擾����
    ret = Get_Subject_Database(index, mesInfo);
    if (ret != errNone)
    {
        // ���R�[�h�̎擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
        MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_message_Info                                         */
/*                                                   ���b�Z�[�W���̎擾  */
/*-------------------------------------------------------------------------*/
static Err get_message_Info(NNshSubjectDatabase *mesInfo,  UInt16 *selTHR, 
                            NNshBoardDatabase   *bbsInfo,  UInt16 *selBBS)
{
    Err        ret;
    FormType  *frm;
    ListType  *lstP;
    UInt16     dummy;

    *selBBS = 0;

    // �X�������擾����
    ret = get_subject_info(mesInfo, selTHR);
    if (ret != errNone)
    {
        return (~errNone);
    }

    // �I������Ă���BBS�̃��X�g�ԍ����擾����
    frm     = FrmGetActiveForm();
    lstP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
    *selBBS = LstGetSelection(lstP);

    // �I�����ꂽBBS�̃��X�g�ԍ��ŁABBS�̃��[�h�𔻒f����
    if ((*selBBS == NNSH_SELBBS_FAVORITE)||
        (*selBBS == NNSH_SELBBS_GETALL)||
        (*selBBS == NNSH_SELBBS_NOTREAD))
    {
        // ���C�ɓ���/�X���S�擾/���ǂ���Mode�́AboardNick���g����BBS�����擾
        ret = Get_BBS_Database(mesInfo->boardNick, bbsInfo, &dummy);
    }
    else
    {
        // BBS�̃��X�g�ԍ����ABBS�����擾����
        ret = Get_BBS_Info(*selBBS, bbsInfo);
    }

    // �X�����ЂƂ����݂��Ȃ��{�[�h���ǂ����m�F
    if (bbsInfo->threadCnt == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_THREAD, "", 0);
        return (~errNone - 10);
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   display_message                                          */
/*                                                        ���b�Z�[�W�̕\�� */
/*-------------------------------------------------------------------------*/
static Boolean display_message(void)
{
    Err                  ret;
    UInt16               selM, butID, selBBS;
    NNshSubjectDatabase  subjInfo;
    NNshBoardDatabase    bbsInfo;

    // �X�������擾����
    ret = get_message_Info(&subjInfo, &selM, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �X�����擾�̂Ƃ��A���擾�̎|���x�����A�X�����擾���邩�m�F����B
    if (subjInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_NOT_YET_SUBJECT, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancel��I��(�������Ȃ�)
            return (true);
        }
        // �X�����擾����
        if (get_all_message() == true)
        {
            return (false);
        }
        // �X���f�[�^�擾�ł����Ƃ��́A��ʂ�\���ɑJ�ڂ���B
    }

    // BBS��(�Ō��)�Q�Ƃ����Ƃ���ɕύX����(���C�ɓ���Ή�)
    NNshParam->lastBBS = selBBS;

    // �Q�Ɖ�ʂ��I�[�v������
    NNshMain_Close(FRMID_MESSAGE);

    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_all_message                                          */
/*                                                     ���b�Z�[�W�S���擾  */
/*-------------------------------------------------------------------------*/
static Boolean get_all_message(void)
{
    Boolean             err;
    Err                 ret;
    Char                url[BUFSIZE], *ptr;
    UInt16              butID, selMES, selBBS, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (true);
    }

    // OFFLINE�X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    // �����X�����X�����X�g�̏�ʂɂ��邩�`�F�b�N����
    if (check_same_thread(selMES, &mesInfo, NULL, &butID) == errNone)
    {
        //  �����X�����������ꍇ�ADB�̕s������h�~���邽�߁A�x����\������
        // �f�[�^�̎擾���s��Ȃ��B
        NNsh_InformMessage(ALTID_WARN, MSG_THREADLIST_ALREADY,
                           mesInfo.threadTitle, 0);
        return (true);
    }

    if (mesInfo.state != NNSH_SUBJSTATUS_NOT_YET)
    {
        // ���Ɏ擾�ς݁A���b�Z�[�W���ŏ�����擾���邪�A�{���ɂ������̊m�F
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_GET_ALREADY, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // �L�����Z������
            NNsh_InformMessage(ALTID_INFO, MSG_ABORT_INFO, "", 0);
            return (true);
        }
    }
    ////////////  ���b�Z�[�W�S���擾�̏���  ///////////

    /** (VFS���)�f�B���N�g�����쐬���� **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // ���b�Z�[�W�擾��̍쐬
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsInfo.boardURL);
    switch (bbsInfo.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂�BBS����f�[�^�擾(HTML��ǂ�)
        StrCat (url, "bbs/read.pl?BBS=");
        StrCat (url, mesInfo.boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        } 
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo.threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileName�̊g���q���폜����
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';
        NNsh_DebugMessage(ALTID_INFO, "MACHI BBS URL:", url, 0);
        break;

      case NNSH_BBSTYPE_2ch:
      default:
        // 2ch����f�[�^�擾(dat��ǂ�)
        StrCat (url, "dat/");
        StrCat (url, mesInfo.threadFileName);
        break;
    }

    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(selMES);

    // ���O�擾����ыL�^����
    ret = Get_AllMessage(url, bbsInfo.boardNick, mesInfo.threadFileName,
                         index, (UInt16) bbsInfo.bbsType);

    // �擾�I���̕\��(DEBUG�I�v�V�����L�����̂�)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_MESSAGE_END,
                      mesInfo.threadTitle, ret);

    // �X�����X�g�̍X�V
    err = redraw_Thread_List(selBBS, selMES);

    // NNsi�ݒ�ŁA�u�擾��X����\���v�Ƀ`�F�b�N�������Ă����ꍇ�́A��ʕ\��
    if ((ret == errNone)&&(NNshParam->openAutomatic != 0))
    {
        // �X����\������B
        display_message();
        err = true;
    }
    return (err);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_message                                           */
/*                                                     ���b�Z�[�W�����擾  */
/*-------------------------------------------------------------------------*/
static Boolean update_message(void)
{
    Boolean             err;
    Err                 ret;
    Char                url[BUFSIZE], *ptr;
    UInt16              butID, selBBS, selMES, index;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // OFFLINE�X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (StrCompare(mesInfo.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    //  Palm�{�̂Ƀ��O�����邪�AVFS���g�p����ݒ�ɂȂ��Ă����ꍇ�ɂ́A
    // �X�V�ł��Ȃ��|�\������
    if ((((NNshParam->useVFS & NNSH_VFS_ENABLE) != 0)&&
        (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM))||
        (((NNshParam->useVFS & NNSH_VFS_ENABLE) == 0)&&
         (mesInfo.msgState == FILEMGR_STATE_OPENED_VFS)))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE_LOG,
                           mesInfo.threadTitle, 0);
        return (false);
    }

    if (bbsInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        //  �܂�BBS�ɑ΂��č����擾��I���������A���݃T�|�[�g�Ȃ�
        // �����擾���邩���ɑS�擾���邩�m�F���s��
        butID = NNsh_ConfirmMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI,
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancel��I��(�������Ȃ�)
            return (true);
        }
        // �X����(�S��)�擾����
        return (get_all_message());
    }

    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // �����擾��I���������A�܂��擾���Ă��Ȃ��B(�S�擾���邩�m�F����)
        butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_NOT_YET_SUBJECT, 
                                    MSG_CONFIRM_GETALLMSG, 0);
        if (butID != 0)
        {
            // Cancel��I��(�������Ȃ�)
            return (true);
        }
        // �X����(�S��)�擾����
        return (get_all_message());
    }

    ////////////  ���b�Z�[�W�����擾�̏���  ///////////

    /** (VFS���)�f�B���N�g�����쐬���� **/
    (void) CreateDir_NNsh(mesInfo.boardNick);

    // ���b�Z�[�W�擾��̍쐬
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsInfo.boardURL);
    switch (bbsInfo.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂�BBS����f�[�^�擾(HTML��ǂ�)
        StrCat (url, "bbs/read.pl?BBS=");
        StrCat (url, mesInfo.boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        }
        StrCat (url, "&KEY=");
        StrCat (url, mesInfo.threadFileName);
        ptr = &url[StrLen(url) - 1];
        // threadFileName�̊g���q���폜����
        while (*ptr != '.')
        {
            ptr--;
        }
        *ptr = '\0';
        NNsh_DebugMessage(ALTID_INFO, "MACHI BBS URL:", url, 0);
        break;

      case NNSH_BBSTYPE_2ch:
      default:
        // 2ch����f�[�^�擾(dat��ǂ�)
        StrCat (url, "dat/");
        StrCat (url, mesInfo.threadFileName);
        break;
    }

    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(selMES);

    // ���O�̎擾����ыL�^(�ǋL)����
    ret = Get_PartMessage(url, bbsInfo.boardNick, &mesInfo, index);

    // �擾�I���̕\��(DEBUG�I�v�V�����L�����̂�)
    NNsh_DebugMessage(ALTID_INFO, MSG_GET_PARTMESSAGE_END,
                      mesInfo.threadTitle, ret);

    // �X�����X�g�̍X�V
    err = redraw_Thread_List(selBBS, selMES);

    // NNsi�ݒ�ŁA�u�擾��X����\���v�Ƀ`�F�b�N�������Ă����ꍇ�́A��ʕ\��
    if ((ret == errNone)&&(NNshParam->openAutomatic != 0))
    {
        // �X����\������B
        display_message();
        err = true;
    }

    return (err);
}

/*-------------------------------------------------------------------------*/
/*   Function :   info_message                                             */
/*                                                    ���b�Z�[�W�̏��\�� */
/*-------------------------------------------------------------------------*/
Boolean info_message(void)
{
    Err                 ret;
    Char                logBuf[BIGBUF], *ptr;
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �X�����b�Z�[�W�����\�z����
    MemSet (logBuf, sizeof(logBuf), 0x00);
    StrCopy(logBuf, mesInfo.threadTitle);
    StrCat (logBuf, "\n");
    if (bbsInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // �܂��a�a�r�̂t�q�k���o�͂���
        StrCat (logBuf, bbsInfo.boardURL);
        StrCat (logBuf, "bbs/read.pl?BBS=");
        StrCat (logBuf, mesInfo.boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (logBuf[StrLen(logBuf) - 1] == '/')
        {
            logBuf[StrLen(logBuf) - 1] = '\0';
        } 
        StrCat (logBuf, "&KEY=");
        StrCat (logBuf, mesInfo.threadFileName);
    }
    else
    {
        // 2ch�̂t�q�k���o�͂���
        StrCat (logBuf, bbsInfo.boardURL);

        // 2ch��URL�ɂ́A�{�[�h�����܂܂�Ă���̂ŁA���̕��������B
        ptr = logBuf + StrLen(logBuf) - 2;
        while (*ptr != '/')
        {
            ptr--;
        }
        *ptr = '\0';
        StrCat (logBuf, "/test/read.cgi/");
        StrCat (logBuf, bbsInfo.boardNick);
        StrCat (logBuf, mesInfo.threadFileName);
    }
    // ������".dat" �������� ".cgi " �͍폜
    ptr = logBuf + StrLen(logBuf) - 1;
    while (*ptr != '.')
    {
        ptr--;
    }
    *ptr = '\0';

    StrCat (logBuf, "\n\n[");

    // ���b�Z�[�W�̋L�^�ӏ����o�͂���
    switch (mesInfo.msgState)
    {
      case FILEMGR_STATE_OPENED_VFS:
        StrCat(logBuf, "VFS");
        break;
      case FILEMGR_STATE_OPENED_STREAM:
        StrCat(logBuf, "Palm");
        break;
      default:
        StrCat(logBuf, "???");
        break;
    }
    if ((mesInfo.msgAttribute & NNSH_MSGATTR_FAVOR) == NNSH_MSGATTR_FAVOR)
    {
        StrCat(logBuf, "<<CHK>>");
    }
    StrCat (logBuf, "] size:");
    NUMCATI(logBuf, mesInfo.fileSize);
    StrCat (logBuf, "(st:");
    NUMCATI(logBuf, mesInfo.state);
    StrCat (logBuf, ", msg#:");
    NUMCATI(logBuf, mesInfo.currentLoc);
    StrCat (logBuf, ")");

    // �X�����b�Z�[�W����\������
    NNsh_ErrorMessage(ALTID_INFO, logBuf, "", 0);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   launch_WebBrowser                                        */
/*                                                  WebBrowser�ŃX�����J�� */
/*-------------------------------------------------------------------------*/
Boolean launch_WebBrowser(void)
{
    Err                 ret;
    Char                logBuf[BIGBUF], *ptr;
    UInt16              selBBS, selTHR;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selTHR, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �X�����b�Z�[�W��URL���\�z����
    MemSet (logBuf, sizeof(logBuf), 0x00);
    if (bbsInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // �܂��a�a�r�̂t�q�k���o�͂���
        StrCat (logBuf, bbsInfo.boardURL);
        StrCat (logBuf, "bbs/read.pl?BBS=");
        StrCat (logBuf, mesInfo.boardNick);
        // boardNick�̖����ɂ��� '/'���폜
        if (logBuf[StrLen(logBuf) - 1] == '/')
        {
            logBuf[StrLen(logBuf) - 1] = '\0';
        } 
        StrCat (logBuf, "&KEY=");
        StrCat (logBuf, mesInfo.threadFileName);

    }
    else
    {
        // 2ch�̂t�q�k���o�͂���
        StrCat (logBuf, bbsInfo.boardURL);

        // 2ch��URL�ɂ́A�{�[�h�����܂܂�Ă���̂ŁA���̕��������B
        ptr = logBuf + StrLen(logBuf) - 2;
        while (*ptr != '/')
        {
            ptr--;
        }
        *ptr = '\0';
        StrCat (logBuf, "/test/read.cgi/");
        StrCat (logBuf, bbsInfo.boardNick);
        StrCat (logBuf, mesInfo.threadFileName);
    }
    // ������".dat" �������� ".cgi " �͍폜
    ptr = logBuf + StrLen(logBuf) - 1;
    while (*ptr != '.')
    {
        ptr--;
    }
    *ptr = '\0';

    // WebBrowser�ŊJ���Ƃ��A�ŐV�T�O�����J���悤�ɂ���
    if (NNshParam->browseMesNum != 0)
    {
        if (mesInfo.bbsType == NNSH_BBSTYPE_MACHIBBS)
        {
            StrCat(logBuf, "&LAST=");
        }
        else
        {
            StrCat(logBuf, "/l");
        }
        NUMCATI(logBuf, NNshParam->browseMesNum);
    }

    // �\������X��URL���f�o�b�O�\��
    NNsh_DebugMessage(ALTID_INFO, "OPEN URL :", logBuf, 0);

    // NetFront���N������(NNsi�I����ɋN��)
    (void) WebBrowserCommand(NNshGlobal->browserCreator, false, 0, 
                             NNshGlobal->browserLaunchCode, logBuf, NULL);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_newarrival_message                                */
/*                                                  �V�����b�Z�[�W�擾���� */
/*-------------------------------------------------------------------------*/
static Boolean update_newarrival_message(void)
{
    Err                  ret;
    Boolean              penState;
    UInt16               selMES, selBBS, index, loop, savedParam, count, idx;
    UInt16               x, y;
    Char                *url, *bbsLbl;
    FormType            *frm;
    ListType            *lstP;
    ControlType         *ctlP;
    NNshBoardDatabase   *bbsInfo;
    NNshSubjectDatabase *mesInfo;

    // ���[�N�̈�̊m��(3��)
    url = MemPtrNew(BUFSIZE);
    if (url == NULL)
    {
        return (false);
    }
    bbsInfo = MemPtrNew(sizeof(NNshBoardDatabase));
    if (bbsInfo == NULL)
    {
        MemPtrFree(url);
        return (false);
    }
    mesInfo = MemPtrNew(sizeof(NNshSubjectDatabase));
    if (bbsInfo == NULL)
    {
        MemPtrFree(url);
        MemPtrFree(bbsInfo);
        return (false);
    }

    // �m�ۂ������[�N�̈������������
    MemSet(url,     BUFSIZE,                     0x00);
    MemSet(bbsInfo, sizeof(NNshBoardDatabase),   0x00);
    MemSet(mesInfo, sizeof(NNshSubjectDatabase), 0x00);

    // ���݂̃��b�Z�[�W�����擾����
    ret = get_message_Info(mesInfo, &selMES, bbsInfo, &selBBS);
    if (ret != errNone)
    {
        goto FUNC_END;
    }

    // OFFLINE�X�����I������Ă����ꍇ�́A�X�V�s��\��
    if (selBBS == NNSH_SELBBS_OFFLINE)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           mesInfo->threadTitle, 0);
        goto FUNC_END;
    }

    ////////////  ���b�Z�[�W�����擾�̏���  ///////////

    // �V�����b�Z�[�W�m�F�������́A�m�F���b�Z�[�W���u�\�����Ȃ��v�ݒ�ɂ���B
    savedParam = NNshParam->confirmationDisable;
    NNshParam->confirmationDisable = 1;

    // "�X��������"��\������
    Show_BusyForm(MSG_SEARCHING_TITLE);

    // ���݂̈ꗗ�X�������擾����
    Get_Thread_Count(selBBS, &count);
    for (loop = 0; loop < count; loop++)
    {
        index = convertListIndexToMsgIndex(loop);
        ret = Get_Subject_Database(index, mesInfo);
        if (ret != errNone)
        {
            // ���R�[�h�̎擾�Ɏ��s
            goto NEXT_STEP;
        }

        //   �P�x�ł����b�Z�[�W�擾�ς݂̃X���ł��A���b�Z�[�W���̏����
        // ���B���Ă��Ȃ��X���ŁA�܂��a�a�r�łȂ�������A�����擾�����{����
        if (mesInfo->state != NNSH_SUBJSTATUS_NOT_YET)
        {
            if ((mesInfo->maxLoc < NNSH_MESSAGE_LIMIT)&&
                (mesInfo->bbsType != NNSH_BBSTYPE_MACHIBBS))
            {
                //  ���C�ɓ���/�擾�ςݑS��/���ǂ��肾������A
                // �a�a�r�����擾����
                if ((selBBS == NNSH_SELBBS_FAVORITE)||
                    (selBBS == NNSH_SELBBS_GETALL)||
                    (selBBS == NNSH_SELBBS_NOTREAD))
                {
                    idx = 0;
                    ret = Get_BBS_Database(mesInfo->boardNick, bbsInfo, &idx);
                    if (ret != errNone)
                    {
                        // ���R�[�h�̎擾�Ɏ��s
                        NNsh_DebugMessage(ALTID_WARN, "BBSINFO Get Fail Nick:",
                                          mesInfo->boardNick, 0);
                        goto NEXT_STEP;
                    }
                    if (idx == 0)
                    {
                        // OFFLINE�X���Ȃ̂ŁA�V���m�F�͍s��Ȃ��B
                        goto NEXT_STEP;
                    }
                }

                // ���b�Z�[�W�擾��̍쐬
                MemSet (url, BUFSIZE, 0x00);
                StrCopy(url, bbsInfo->boardURL);
                StrCat (url, "dat/");
                StrCat (url, mesInfo->threadFileName);
 
                // �ꎞ�I��BUSY�E�B���h�E����������
                Hide_BusyForm();

                // ���O�̎擾����ыL�^(�ǋL)����
                ret = Get_PartMessage(url, bbsInfo->boardNick, mesInfo, index);

                // BUSY�E�B���h�E�𕜊�������
                Show_BusyForm(MSG_SEARCHING_TITLE);

                //  ���O�擾���������~���ꂽ or �v���I�G���[�������ɂ�
                // �V�����b�Z�[�W�̊m�F�𒆎~����B
                if (ret == ~errNone)
                {
                    // �����𒆎~����|�A��ʕ\������
                    NNsh_InformMessage(ALTID_INFO, MSG_ABORT_MESSAGE, "", 0);
                    break;
                }
            }
        }
NEXT_STEP:
        // �y���̏�Ԃ��E��(��ʃ^�b�v����Ă��邩�m�F����)
        EvtGetPen(&x, &y, &penState);
        if (penState == true)
        {
            // �y�����_�E������Ă�����A���~���邩�m�F���s��
            if (NNsh_ErrorMessage(ALTID_CONFIRM,
                                  MSG_CONFIRM_ABORT_GET, "", 0) == 0)
            {
                // OK�������ꂽ�Abreak����
                break;
            }
        }
    }
    Hide_BusyForm();

    // �m�F���b�Z�[�W�ȗ��̃p�����[�^�����ɖ߂�
    NNshParam->confirmationDisable = savedParam;

    // �u�V���m�F�v��ɖ��ǈꗗ��\������
    if (NNshParam->autoOpenNotRead != 0)
    {
        // �I������Ă���BBS�̃��X�g�ԍ���ύX����
        selBBS = NNSH_SELBBS_NOTREAD;

        frm    = FrmGetActiveForm();
        lstP   = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
        LstSetSelection(lstP, selBBS);

        // BBS�\���^�u��ύX����
        NNshParam->lastBBS = selBBS;
        bbsLbl = NNshGlobal->bbsTitles;
        for (x = 0; x < NNshParam->lastBBS; x++)
        {
            bbsLbl = bbsLbl + StrLen(bbsLbl) + 1;
        }
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_BBSINFO));
        CtlSetLabel(ctlP, bbsLbl);
    }

    // ���X�g�����X�V����
    ret = redraw_Thread_List(selBBS, 0);

FUNC_END:
    // �֐��̏I��
    MemPtrFree(url);
    MemPtrFree(bbsInfo);
    MemPtrFree(mesInfo);

    // �G���[�����������Ƃ��́A�����؂炸�ɏI������
    if (ret != errNone)
    {
        return (false);
    }
    if (NNshParam->disconnArrivalEnd != 0)
    {
        // �u�V���m�F�v�I�����ɉ���ؒf
        savedParam = NNshParam->confirmationDisable;
        NNshParam->confirmationDisable = 1;
        NNshNet_LineHangup();
        NNshParam->confirmationDisable = savedParam;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   update_getall_message                                    */
/*                                     ���b�Z�[�W�擾����("�X�V" �{�^���p) */
/*-------------------------------------------------------------------------*/
static Boolean update_getall_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���b�Z�[�W��Ԃ��擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // ���b�Z�[�W��Ԃɂ���āA�A�N�V������ύX����
    if (mesInfo.state == NNSH_SUBJSTATUS_NOT_YET)
    {
        // ���擾�Ȃ�S���擾
        return (get_all_message());
    }

    // �擾�ς݂Ȃ獷���擾
    return (update_message());
}

/*-------------------------------------------------------------------------*/
/*   Function :   redraw_Thread_list                                       */
/*                                                      �X���ꗗ�\���̍X�V */
/*-------------------------------------------------------------------------*/
static Boolean redraw_Thread_List(UInt16 bbsIndex, UInt16 threadIndex)
{
    Err               ret;
    FormType         *frm;
    UInt32            dataSize, margin;
    UInt16            cnt, threadCnt;

    // �X�����X�g�̃^�b�v����������
    NNshGlobal->tappedPrev = -1;
    frm = FrmGetActiveForm();

    // �m�ۍςݗ̈���J������B
    if (NNshGlobal->threadTitles != NULL)
    {
        MemHandleUnlock(NNshGlobal->msgTTLStringH);
        NNshGlobal->threadTitles = NULL;
    }
    MEMFREE_HANDLE(NNshGlobal->msgTTLStringH);

    // ��񃊃X�g����������
    NNshWinSetListItems(frm, LSTID_SELECTION_THREAD, NNshGlobal->dummyBuffer,
                        1, 0, &(NNshGlobal->threadTitleH));
    NNshWinClearList(frm, LSTID_SELECTION_THREAD);

    // "�X���ꗗ�쐬��" �̕\��
    Show_BusyForm(MSG_READ_THREAD_WAIT);

    // �Y������X���b�h�������߂�B
    ret = Get_Thread_Count(bbsIndex, &threadCnt);
    if (ret == errNone)
    {
        //   �X��������A���X�g�̈�̊m�ۃT�C�Y�����߂�(8�́A�^�C�g���̑O��
        // �\������MSG�������̃o�C�g��)
        dataSize = (NNshParam->titleListSize + 8) * (threadCnt + 1);
        margin   = MARGIN;
        if (dataSize > (NNSH_WORKBUF_MAX - margin))
        {
            dataSize = NNSH_WORKBUF_MAX - margin;
	}
        NNshGlobal->msgTTLStringH = MemHandleNew(dataSize + margin);
        if (NNshGlobal->msgTTLStringH != 0)
        {
            NNshGlobal->threadTitles =
                                      MemHandleLock(NNshGlobal->msgTTLStringH);
            if (NNshGlobal->threadTitles != NULL)
            {
                // �X���ꗗ���쐬����
                MemSet(NNshGlobal->threadTitles, dataSize + margin, 0x00);
                (void) Create_SubjectList(bbsIndex, threadCnt,
                                     NNshGlobal->threadTitles, dataSize, &cnt);
                // ���̃X�e�b�v��
                goto NEXT_STEP;
            }
        }
        // ���X�g�̈�̊m�ۂɎ��s����(���̎|��\������)
        NNsh_DebugMessage(ALTID_WARN, MSG_SUBJECT_LISTFAIL, " SIZE:",dataSize);
    }

NEXT_STEP:
    // �X�����̎擾�Ɏ��s�����ꍇ�ɂ́A�_�~�[�̃��X�g���쐬����
    if (cnt == 0)
    {
        // ���X�g�̈悪�m�ۍς݂������ꍇ�ɂ́A��U�J������B
        if (NNshGlobal->threadTitles != NULL)
        {
            MemHandleUnlock(NNshGlobal->msgTTLStringH);
            NNshGlobal->threadTitles = NULL;
        }
        MEMFREE_HANDLE(NNshGlobal->msgTTLStringH);
        NNshGlobal->msgTTLStringH = MemHandleNew(MAX_THREADNAME + MARGIN);
        if (NNshGlobal->msgTTLStringH != 0)
        {
            NNshGlobal->threadTitles =
                                      MemHandleLock(NNshGlobal->msgTTLStringH);
            if (NNshGlobal->threadTitles != NULL)
            {
                MemSet (NNshGlobal->threadTitles,
                        (MAX_THREADNAME + MARGIN), 0x00);
                StrCopy(NNshGlobal->threadTitles, MSG_SUBJECT_DEFAULT);
                cnt = 1;
            }
        }

        // �C���f�b�N�X�̏�����
        if (NNshGlobal->threadIdxH != 0)
        {
            MemHandleFree(NNshGlobal->threadIdxH);
            NNshGlobal->threadIdxH = 0;
        }
    }

    // "�X���ꗗ�쐬��"�̕\���폜
    Hide_BusyForm();

    // �X���ꗗ��\������
    if ((frm != NULL)&&(NNshGlobal->threadTitles != NULL))
    {
        NNshWinSetListItems(frm, LSTID_SELECTION_THREAD,
                            NNshGlobal->threadTitles, cnt, threadIndex,
                            &(NNshGlobal->threadTitleH));
        NNshGlobal->threadCount = cnt;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_notreceived_message                               */
/*                                                    ����M�X�����폜���� */
/*-------------------------------------------------------------------------*/
Boolean delete_notreceived_message(void)
{
    Err                   ret;
    UInt16                selMES, selBBS, cnt;
    FormType             *frm;
    ListType             *lstP;
    NNshSubjectDatabase   mesInfo;
    NNshBoardDatabase     bbsInfo;

    // �I������Ă���BBS�̃��X�g�ԍ����擾����
    frm    = FrmGetActiveForm();
    lstP   = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
    selBBS = LstGetSelection(lstP);

    // �u���C�ɓ���v�ꗗ�́A�폜�ł��Ȃ��B
    if ((selBBS == NNSH_SELBBS_FAVORITE)||
        (selBBS == NNSH_SELBBS_GETALL)||
        (selBBS == NNSH_SELBBS_NOTREAD))
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOTDELETE_FAVOR,"", 0);
        return (false);    
    }

    // �X�����̎擾
    ret = get_subject_info(&mesInfo, &selMES);
    if (ret != errNone)
    {
        return (false);
    }

    // ���擾�̃X�����폜����
    (void) DeleteSubjectList(mesInfo.boardNick, NNSH_SUBJSTATUS_NOT_YET, &cnt);

    // ���ݕێ�����X������BBS-DB�ɔ��f������
    Get_BBS_Info   (selBBS, &bbsInfo);
    bbsInfo.threadCnt = cnt;
    Update_BBS_Info(selBBS, &bbsInfo);

    // �X���������݂̈ʒu��菬�����ꍇ�ɂ́A�����̃X���ɃJ�[�\�������킹��
    if (selMES >= cnt)
    {
        selMES = cnt - 1;    
    }

    // ���X�g�����X�V����
    return (redraw_Thread_List(selBBS, selMES));
}

/*-------------------------------------------------------------------------*/
/*   Function :   delete_message                                           */
/*                                                           �X���폜����  */
/*-------------------------------------------------------------------------*/
Boolean delete_message(void)
{
    Err                 ret;
    UInt16              selMES, selBBS, butID, index;
    Char                fileName[MAXLENGTH_FILENAME], *area;
    DmOpenRef           dbRef;
    NNshSubjectDatabase mesInfo;
    NNshBoardDatabase   bbsInfo;

    // ���b�Z�[�W�����擾����
    ret = get_message_Info(&mesInfo, &selMES, &bbsInfo, &selBBS);
    if (ret != errNone)
    {
        return (false);
    }

    // �X����{���ɍ폜���邩�m�F����
    butID = NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_DELETE_MSGCONFIRM, 
                                mesInfo.threadTitle, 0);
    if (butID != 0)
    {
        // �폜�L�����Z��
        return (false);
    }

    // �X�����DB�̃I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    if (dbRef == 0)
    {
        // �G���[����
        return (~errNone);
    }

    // �X���t�@�C��(�{�̂̃��O�t�@�C��)���폜
    MemSet (fileName, sizeof(fileName), 0x00);
    if (selBBS != NNSH_SELBBS_OFFLINE)
    {
        // OFFLINE���O�̂Ƃ��́AboardNick��t�����Ȃ�
        StrCopy(fileName, mesInfo.boardNick);
    }
    else
    {
        // OFFLINE�X����VFS�Ɋi�[����Ă���ꍇ�ɂ́A�f�B���N�g����t������B
        if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ���O�x�[�X�f�B���N�g�����擾����
            if ((NNshGlobal->logPrefixH == 0)||
                ((area = MemHandleLock(NNshGlobal->logPrefixH)) == NULL))
            {
                StrCopy(fileName, LOGDATAFILE_PREFIX);
            }
            else
            {
                StrCopy(fileName, area);
                MemHandleUnlock(NNshGlobal->logPrefixH);
            }

            GetSubDirectoryName(mesInfo.dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, mesInfo.threadFileName);

    if (mesInfo.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        // Palm�{�̏�̃��O���폜����
        ret = DeleteFile_NNsh(fileName, NNSH_VFS_DISABLE);
    }
    else
    {
        // VFS��̃��O���폜����
        ret = DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
    }
    if((ret != errNone)&&(ret != fileErrNotFound)&&(ret != vfsErrFileNotFound))
    {
        NNsh_InformMessage(ALTID_ERROR, "File Delete: ", fileName, ret);
    }

    // ���X�g�ԍ�����X��index�ɕϊ�����
    index = convertListIndexToMsgIndex(selMES);

    // �X���C���f�b�N�X�t�@�C�����폜���ăN���[�Y
    (void) DeleteRecordIDX_NNsh(dbRef, index);
    CloseDatabase_NNsh(dbRef);

    // �X���ꗗDB���X�V�������Ƃɂ���
    NNshGlobal->updateDatabaseInfo = 
                ((NNshGlobal->updateDatabaseInfo) | (NNSH_DB_UPDATE_SUBJECT));

    // ���ݕێ�����X������BBS-DB�ɔ��f������
    Get_BBS_Info   (selBBS, &bbsInfo);
    bbsInfo.threadCnt--;
    Update_BBS_Info(selBBS, &bbsInfo);

    // �J�[�\�����P�O�̃��b�Z�[�W�Ɉړ�������B
    if (selMES != 0)
    {
        selMES--;    
    }

    return (redraw_Thread_List(selBBS, selMES));
}

/*-------------------------------------------------------------------------*/
/*   Function :   get_subject_txt                                          */
/*                                      �X�����ꗗ(subject.txt)���擾����  */
/*-------------------------------------------------------------------------*/
static Boolean get_subject_txt(void)
{
    Err       ret;
    UInt16    selBBS;
    FormType *frm;
    ListType *lstP;

    frm  = FrmGetActiveForm();
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO)); 
    selBBS  = LstGetSelection(lstP);

    switch (selBBS)
    {
      case NNSH_SELBBS_FAVORITE:
      case NNSH_SELBBS_GETALL:
      case NNSH_SELBBS_NOTREAD:
        // "���C�ɓ���"�A"�擾�ςݑS��"�A"���ǂ���"�\�����ɂ́A�󂯕t���Ȃ��B
        return (false);
        break;

      case NNSH_SELBBS_OFFLINE:
        // �I�t���C���X���̌������s��
        create_offline_database();
        redraw_Thread_List(selBBS, 0);
        FrmDrawForm(frm);
        break;

      default:
        // �X���ꗗ���擾����
        ret = NNsh_GetSubjectList(selBBS);
        if (ret == errNone)
        {  
            // �X���ꗗ�̎擾�ɐ��������Ƃ��́A�X���ꗗ���ĕ\������
            redraw_Thread_List(selBBS, 0);
            FrmDrawForm(frm);
        }
        break;
    }
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   popEvt_ThreadMain                                        */
/*                                          ���X�g�I���C�x���g�������[�`�� */
/*-------------------------------------------------------------------------*/
static Boolean popEvt_ThreadMain(EventType *event)
{
    switch (event->data.popSelect.listID)
    {
      case LSTID_BBSINFO:
        // LSTID_BBSINFO��popSelectEvent�������Ȃ�A�X�������X�V����
        NNshParam->lastBBS = event->data.popSelect.selection;
        return (redraw_Thread_List(NNshParam->lastBBS, 0));
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   keyDownEvt_ThreadMain                                    */
/*                                                        �L�[���͎��̏��� */
/*-------------------------------------------------------------------------*/
static Boolean keyDownEvt_ThreadMain(EventType *event)
{
    Boolean   ret = false;
    UInt16    keyCode;
    Int16     curr, max, itms;
    FormType *frm;
    ListType *lstP;

    // ���݂̃��X�g�I�u�W�F�N�g�����擾
    frm  = FrmGetActiveForm();
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
    curr = LstGetSelection(lstP);
    max  = LstGetNumberOfItems(lstP);
    itms = LstGetVisibleItems(lstP);

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // �u���v(�W���O�_�C������)�����������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
        // ���X�g�P��
        if (curr < (max - 1))
        {
            curr++;
            LstSetSelection(lstP, curr);
            ret = true;
	}
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
        // ���X�g�P��
        if (curr > 0)
        {
            curr--;
            LstSetSelection(lstP, curr);
            ret = true;
        }
        break;

      case chrCapital_T:  // T�L�[�̓���
      case chrSmall_T:
        // ���X�g�̐擪�ֈړ�
        curr = 0;
        LstSetSelection(lstP, curr);
        LstSetTopItem  (lstP, curr);
        ret = true;
        break;

      case chrCapital_B:    // B�L�[�̓���
      case chrSmall_B:
        curr = max - 1;
        LstSetSelection(lstP, curr);
        LstSetTopItem  (lstP, curr);
        ret = true;
        break;

      // �W���O�_�C����������(��)�̏���(1�y�[�W��)
      case vchrJogPushedUp:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        /** not break!!  **/

      case chrRecordSeparator:   // �J�[�\���L�[(��)�������̏���
      case chrCapital_P:         // P�L�[�̓���
      case chrSmall_P:           // p�L�[�̓���
        curr = (curr < (itms - 1)) ? 0 : curr - (itms - 1);
        LstSetSelection(lstP, curr);
        LstSetTopItem  (lstP, curr);
        ret = true;
        break;

      // �W���O�_�C����������(��)�̏���(1�y�[�W��)
      case vchrJogPushedDown:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        /** not break!!  **/

      case chrUnitSeparator:  // �J�[�\���L�[(��)�������̏���
      case chrCapital_N:      // N�L�[�̓���
      case chrSmall_N:        // n�L�[�̓���
        curr = ((max - 1) < (curr + itms)) ? (max - 1) : (curr + itms);
        LstSetSelection(lstP, curr);
        break;

      // JOG Push�I�����̏���(���X�g�I��)
      case vchrJogRelease:
#ifdef USE_CLIE
        if (NNshGlobal->tempStatus == NNSH_TEMPTYPE_CLEARSTATE)
        {
            // �X���Q��
            return (display_message());
        }
        // �ꎞ��Ԃ��N���A����
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
#endif
        break;

      // �o�b�N�{�^��/ESC�L�[/BS�L�[�̏���
      case vchrJogBack:
      case chrBackspace:
      case chrEscape:
        if (NNshParam->jogBackBtnAsMenu != 0)
        {
            // ���j���[���J��
            return (open_menu());
        }
        else
        {
            // �X�����\��
            info_message();
        }
        break;

      // HandEra JOG Push�I��/Enter�L�[���͎��̏���(���X�g�I��)
      case chrCarriageReturn:
      case chrLineFeed:
        // �X���Q��
        return (display_message());
        break;

      // �X�y�[�X�L�[����
      case chrSpace:
        // �V��MSG�m�F
        return (update_newarrival_message());

      // Q�L�[����
      case chrCapital_Q:
      case chrSmall_Q:
        NNshNet_LineHangup();
        break;

      default:
        // ���̑�
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_ThreadMain                                     */
/*                                                      �{�^���������̏��� */
/*-------------------------------------------------------------------------*/
static Boolean ctlSelEvt_ThreadMain(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_MSGCHK:
        // �V��MSG�m�F
        return (update_newarrival_message());
        break;

      case BTNID_OPEN_THREAD:
        // �X���Q��
        return (display_message());
        break;

      case BTNID_UPDATE_THREAD:
        // ���b�Z�[�W�擾(�����擾/�S���擾�͎������f)
        return (update_getall_message());
        break;

      case BTNID_LIST_THREAD:
        // �V�K�X���擾(SUBJECT.TXT�擾)
        return (get_subject_txt());
        break;

      case BTNID_DELETE_THREAD:
        // �X���폜
        return (delete_message());
        break;

      case BTNID_DISCONNECT:
        // ����ؒf
        NNshNet_LineHangup();
        break;

      case CHKID_SELECT_GETMODE:
        // �X�������擾/�S���擾�̐؂�ւ�(�p�����[�^�ɔ��f������)
        UpdateParameter(FrmGetActiveForm(),
                        CHKID_SELECT_GETMODE, &(NNshParam->enablePartGet));
        if (NNshParam->enablePartGet == 0)
        {
            // �X���ꊇ�擾���[�h�ɐ؂�ւ������Ƃ�ʒm����
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_ALLGET_MODE, "", 0);
        }
        else
        {
            // �X�������擾���[�h�ɐ؂�ւ������Ƃ�ʒm����
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_PARTGET_MODE, "", 0);
        }
        break;
  
      default:
        // ���̑�
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   lstSelEvt_ThreadMain                                     */
/*                                          ���X�g�I���C�x���g�������[�`�� */
/*-------------------------------------------------------------------------*/
static Boolean lstSelEvt_ThreadMain(EventType *event)
{
    if (event->data.lstSelect.listID == LSTID_SELECTION_THREAD)
    {
        if (event->data.lstSelect.selection == NNshGlobal->tappedPrev)
        {
            // �_�u���^�b�v�����o(���̏ꍇ�̓X���Q��)
            return (display_message());
        }
        else
        {
            NNshGlobal->tappedPrev = event->data.lstSelect.selection;
        }
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Handler_ThreadMain                                       */
/*                                          �X���Ǘ���ʗp�C�x���g�n���h�� */
/*=========================================================================*/
Boolean Handler_ThreadMain(EventType *event)
{
    Boolean   ret;
#ifdef USE_HANDERA
    FormType *frm;
#endif

    ret = false;
    switch (event->eType)
    {
      case keyDownEvent:
        return (keyDownEvt_ThreadMain(event));
        break;

      case fldEnterEvent:
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_ThreadMain(event));
        break;

      case frmOpenEvent:
        break;

      case popSelectEvent:
        return (popEvt_ThreadMain(event));
        break;

      case lstSelectEvent:
        return (lstSelEvt_ThreadMain(event));
        break;

      case lstEnterEvent:
        break;

      case lstExitEvent:
        break;

      case menuEvent:
        return (menuEvt_ThreadMain(event));
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        break;
#endif  // #ifdef USE_HANDERA

      case menuOpenEvent:
        break;

      case sclRepeatEvent:
        break;

      case sclExitEvent:
        break;

      default:
        break;
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   OpenForm_ThreadMain                                      */
/*                                         �X���Ǘ���ʂ��J�����Ƃ��̏���  */
/*=========================================================================*/
void OpenForm_ThreadMain(FormType *frm)
{
    Err          ret;
    UInt16       lp;
    Char        *bbsLbl;
    ControlType *ctlP;

    // ����I�[�v�����A�I�t���C���X���̌���
    if ((NNshParam->offChkLaunch != 0)&&(NNshGlobal->bbsTitles == NULL))
    {
        create_offline_database();
    }

    // BBS�ꗗ�̍쐬(�ꗗ���Ȃ���΁ABBS�ꗗ���擾����悤����)
    ret = create_BBS_INDEX(&(NNshGlobal->bbsTitles), &lp);
    if (ret != errNone)
    {
        // �쐬���s�A�t�H�[�����͍X�V���Ȃ�
        return;
    }

    // NNshParam->lastBBS(�O��I�����Ă���BBS��)���ςȂƂ�(�ꗗ�̐擪�ɂ���)
    if (lp < NNshParam->lastBBS)
    {
        NNshParam->lastBBS = 0;
    }

    // BBS�ꗗ�̃��X�g��Ԃ𔽉f����B
    NNshWinSetListItems(frm, LSTID_BBSINFO, NNshGlobal->bbsTitles, lp,
                        NNshParam->lastBBS, &(NNshGlobal->bbsTitleH));

    // �|�b�v�A�b�v�g���K�̃��x����(���X�g��Ԃɍ��킹)�X�V����
    bbsLbl = NNshGlobal->bbsTitles;
    for (lp = 0; lp < NNshParam->lastBBS; lp++)
    {
        bbsLbl = bbsLbl + StrLen(bbsLbl) + 1;
    }
    if (bbsLbl != NULL)
    {
        ctlP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, POPTRID_BBSINFO));
        CtlSetLabel(ctlP, bbsLbl);
    }

    // �X�������擾/�S���擾�̐ݒ�l�𔽉f������
    SetControlValue(frm, CHKID_SELECT_GETMODE, &(NNshParam->enablePartGet));
                        
    // �X���ꗗ�̕\�����X�V
    redraw_Thread_List(NNshParam->lastBBS, NNshParam->openThreadIndex);

    // �J�����t�H�[��ID��ݒ肷��(������ݒ蒆�ɂ́AlastFrmID�͕ύX���Ȃ�)
    if ((NNshParam->useBookmark != 0)&&(NNshParam->bookMsgNumber != 0))
    {
        // ������ݒ蒆�Ȃ̂ŁA�������Ȃ�
    }
    else
    {
        NNshParam->lastFrmID = FRMID_THREAD;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   menuEvt_ThreadMain                                       */
/*                                      �X���Ǘ���ʗp���j���[�������[�`�� */
/*-------------------------------------------------------------------------*/
static Boolean menuEvt_ThreadMain(EventType *event)
{
    Boolean             err = false;
    Int16               max;
    FormType           *frm;
    ListType           *lstP;
    Char               *buffer;
#ifdef USE_GLUE
    UInt16              selM, selB;
#endif
#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // �X�N���[�����[�h
    VgaRotateModeType   handEraRotateMode;     // ��]���[�h
#endif

    frm  = FrmGetActiveForm();
    switch (event->data.menu.itemID)
    {
      // �o�[�W�����̕\��(������������ FrmCustomAlert() ���Ăяo��)
      case MNUID_SHOW_VERSION:
        buffer = MemPtrNew(BUFSIZE);
        if (buffer == NULL)
        {
            return (false);
        }
        MemSet (buffer, BUFSIZE, 0x00);
        StrCopy(buffer, "NNsi\n");
        StrCat (buffer, SOFT_VERSION);
        StrCat (buffer, "\n");
        StrCat (buffer, SOFT_REVISION);
        StrCat (buffer, "\n(");
        StrCat (buffer, SOFT_DATE);
#ifdef SMALL
        StrCat (buffer, ", OS31NNsi.prc");
#endif
#ifdef STD
        StrCat (buffer, ", NNsiSTD.prc");
#endif
#ifdef OFFLINE_DEBUG
        StrCat (buffer, "[OFFLINE DEBUG]");
#endif
        FrmCustomAlert(ALTID_INFO, buffer, ")", "");
        MemPtrFree(buffer);
        buffer = NULL;
        err    = true;
        break;

        // NNsi�ݒ��ʂ��J��
      case MNUID_CONFIG_NNSH:
        NNshMain_Close(FRMID_CONFIG_NNSH);
        break;

        // NNsi�ݒ�-2��ʂ��J��
      case MNUID_NNSI_SETTING2:
        NNshMain_Close(FRMID_NNSI_SETTING2);
        break;

        // NNsi�ݒ�-3��ʂ��J��
      case MNUID_NNSI_SETTING3:
        NNshMain_Close(FRMID_NNSI_SETTING3);
        break;

        // NNsi�ݒ�-4��ʂ��J��
      case MNUID_NNSI_SETTING4:
        NNshMain_Close(FRMID_NNSI_SETTING4);
        break;

        // ����ؒf
      case MNUID_NET_DISCONN:
        NNshNet_LineHangup();
        break;

        // �V�����b�Z�[�W�m�F
      case MNUID_UPDATE_CHECK:
        err = update_newarrival_message();
        break;

        // ���X�V(�擾)
      case MNUID_UPDATE_BBS:
        err = GetBBSList(NNshParam->bbs_URL);
        if (err == false)
        {
            break;
        }
        /** not break; (�����Ďg�p�I�����s��) **/

        // �g�p�I����ʂ��J��
      case MNUID_SELECT_BBS:
        NNshMain_Close(FRMID_MANAGEBBS);
        break;

        // �V�K�X���擾(SUBJECT.TXT�擾)
      case MNUID_GET_NEWMESSAGE:
        err = get_subject_txt();
        break;

        // �X�����
      case MNUID_INFO_MESSAGE:
        err = info_message();
        break;

        // ���擾�X���폜
      case MNUID_DEL_THREADLIST:
        err = delete_notreceived_message();
        break;

        // �X���폜
      case MNUID_DELETE_MESSAGE:
        err = delete_message();
        break;

        // ���b�Z�[�W�S���擾(�Ď擾)
      case MNUID_GET_ALLMESSAGE:
        err = get_all_message();
        break;

        // ���b�Z�[�W�����擾
      case MNUID_UPDATE_MESSAGE:
        err = update_message();
        break;

        // �X���Q��
      case MNUID_OPEN_MESSAGE:
        err = display_message();
        break;

        // �t�H���g�ύX
      case MNUID_FONT:
        NNshParam->currentFont = FontSelect(NNshParam->currentFont);
#ifdef USE_GLUE
        LstGlueSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,
                            LSTID_SELECTION_THREAD)), NNshParam->currentFont);

        // �t�H���g�ύX��̕\���́A���X�g���č쐬���ĕ\��������
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
        selM = LstGetSelection(lstP);
        lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_BBSINFO));
        selB = LstGetSelection(lstP);
        redraw_Thread_List(selB, selM);
#endif
        break;

#ifdef USE_HANDERA
        // HandEra ��ʉ�] //
      case MNUID_HANDERA_ROTATE:
        if(NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
            VgaSetScreenMode(screenMode1To1,
                             VgaRotateSelect(handEraRotateMode));
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        else
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;
#endif  // #ifdef USE_HANDERA

      case MNUID_GET_MESSAGENUM:
        // �X���Ԏw��̃��b�Z�[�W�擾
        err = get_MessageFromMesNum();
        break;

      case MNUID_SEARCH_TITLE:
        // �^�C�g������(������̐ݒ�)
        err = set_SearchTitleString();
        if (err != true)
        {
            // ���������񂪐ݒ肳��Ȃ�����(�I������) 
            break;
        }
        // not break! (�����ăX���^�C���������s!)
      case MNUID_SEARCH_NEXT:
        // �X���^�C��������(�����)
        err = search_NextTitle(NNSH_SEARCH_FORWARD);
        break;

      case MNUID_SEARCH_PREV:
        // �X���^�C��������(�O����)
        err = search_NextTitle(NNSH_SEARCH_BACKWARD);
        break;

      case MNUID_MOVE_TOP:
        // �X���^�C�g���̐擪�ֈړ�
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
        LstSetSelection(lstP, 0);
        LstSetTopItem  (lstP, 0);
        break;

      case MNUID_MOVE_BOTTOM:
        // �X���^�C�g���̐擪�ֈړ�
        lstP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm,LSTID_SELECTION_THREAD));
        max  = LstGetNumberOfItems(lstP);
        LstSetSelection(lstP, max - 1);
        LstSetTopItem  (lstP, max - 1);
        break;

      case MNUID_COPY_READONLY:
        // �X�����Q�Ɛ�p���O�փR�s�[����
        err = copy_to_readOnly();
        if ((err == true)&&(NNshParam->copyDelReadOnly != 0))
        {
            // �R�s�[�����������Ƃ��A�R�s�[���t�@�C�����폜����
            err = delete_message();
        }
        break;

      case MNUID_OS5_LAUNCH_WEB:
        if (NNshGlobal->browserCreator != 0)
        {
            // WebBrowser�ŊJ��
            launch_WebBrowser();
        }
        else
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      case MNUID_BACKUP_DATABASE:
        if (((NNshParam->useVFS) & (NNSH_VFS_ENABLE)) == NNSH_VFS_ENABLE)
        {
            // DB��VFS�Ƀo�b�N�A�b�v����
            BackupDBtoVFS(NNSH_ENABLE);
        }
        else
        {
             // "���݃T�|�[�g���Ă��܂���" �\�����s��
             NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;

      default:
        // ���̑�(���肦�Ȃ�)
        break;
    }
    return (err);
}
