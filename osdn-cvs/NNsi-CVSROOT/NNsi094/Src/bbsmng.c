/*============================================================================*
 *  FILE: 
 *     bbsmng.c
 *
 *  Description: 
 *     BBS list manage form functions for NNsh.
 *
 *===========================================================================*/
#define BBSMNG_C
#include "local.h"

// BBS�擾�֐��Q
static UInt16  getBoardURL    (Char *url, Char *nick, Char *top, Char *bottom);
static void    parseBoard     (DmOpenRef dbRef, Char *top, UInt32 *readSize);
static void    updateBoardList(void);

// �g�pBBS�I���֐��Q
static void    closeForm_SelectBBS  (void);
static void    setBBSListItem       (void);
static void    setAllBBS_Selection  (UInt16     value);
static Boolean ctlSelEvt_SelectBBS  (EventType *event);
static Int16   ctlRepEvt_SelectBBS  (EventType *event);
static Int16   keyDownEvt_SelectBBS (EventType *event);
static void    set_Selection_Info   (UInt16     pageNum);
static void    display_BBS_Selection(UInt16     pageNum);

///////////////////////////////////////////////////////////////////////////////
//////////   BBS���擾�p�֐��Q
///////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------*/
/*   Function :   getBoardURL                                              */
/*                                                                         */
/*                                              ���̂��ǂ����̔��f�Ǝ擾 */
/*-------------------------------------------------------------------------*/
static UInt16 getBoardURL(Char *url, Char *nick, Char *top, Char *bottom)
{
    Char   *ptr, *ptr2, *ptr3, *chk;
    
    ptr  = StrStr(top, "http://");
    ptr2 = ptr + 7; // 7 == StrLen("http://");

#ifdef USE_STRSTR
    ptr2 = StrStr(ptr2, "/");
#else
    while ((*ptr2 != '/')&&(*ptr2 != '\0'))
    {
        ptr2++;
    }
#endif
    ptr2 = ptr2 + 1; // 1 == StrLen("/");
#ifdef USE_STRSTR
    ptr3 = StrStr(ptr2, "/");
#else
    ptr3 = ptr2;
    while ((*ptr3 != '/')&&(*ptr3 != '\0'))
    {
        ptr3++;
    }
#endif
    ptr3 = ptr3 + 1; // 1 == StrLen("/");
#ifdef USE_STRSTR
    if ((ptr3 == NULL)||(bottom < ptr3))
#else
    if ((bottom < ptr3)||(ptr3 == '\0'))
#endif
    {
        // "http://�قɂ���/�قւق�/" �`���ł͂Ȃ��̂ŁA�ł͂Ȃ��Ɣ��f
        return (NNSH_BBSTYPE_ERROR);
    }

    // 2�����˂�̃{�[�h���ABBSPINK�̃{�[�h�ȊO�͔Ƃ݂͂Ȃ��Ȃ�
    chk = StrStr(top, "2ch.net");
    if ((chk != NULL)&&(chk < bottom))
    {
#if 0   // ���܂�ɂ���͂Ɏ��Ԃ������肷����̂ł͂���
        chk = StrStr(top, "www.2ch.net");
        if ((chk != NULL)&&(chk < bottom))
        {
            // �������A"www.2ch.net" ���ƁA�Ƃ݂͂Ȃ��Ȃ��悤�ɂ���
            return (NNSH_BBSTYPE_ERROR);
        }
#endif
        StrNCopy(url,  ptr,  (ptr3 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_2ch);
    }
    chk = StrStr(top, "bbspink.com");
    if ((chk != NULL)&&(chk < bottom))
    {
        StrNCopy(url,  ptr,  (ptr3 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_2ch);
    }

    // �܂�BBS�̃{�[�h�����v�ɂ��Ă݂�B�B�B
    chk = StrStr(top, "machibbs.com");
    if ((chk != NULL)&&(chk < bottom))
    {
        // �܂�BBS��URL�́A"http://(�z�X�g��)/" �����ɂ���B
        StrNCopy(url,  ptr,  (ptr2 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_MACHIBBS);
    }

    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   parseBoard                                               */
/*                                  ��M�f�[�^�̍\�����߂���єꗗ�̍X�V */
/*-------------------------------------------------------------------------*/
static void parseBoard(DmOpenRef dbRef, Char *top, UInt32 *readSize)
{
    Char             *data, *ptr, *chkToken;
    UInt32            parseSize;
    UInt16            state, index, ret;
    NNshBoardDatabase bbsData;

    // �f�[�^�̏�����
    index     = 0;
    ret       = NNSH_BBSTYPE_ERROR;
    state     = HTTP_ANCHORSTATE_NORMAL;
    chkToken  = "<A";
    MemSet(&bbsData, sizeof(NNshBoardDatabase), 0x00);

    parseSize = 0;
    ptr       = top;
    while (parseSize < *readSize)
    {
        data = StrStr(ptr, chkToken);
        if (data == NULL)
        {
            // �g�[�N����������Ȃ�����(�r���܂Ńp�[�X����)
            if (state != HTTP_ANCHORSTATE_NORMAL)
            {
                // �O��p�[�X�����擪����������
                *readSize = parseSize;
            }
            return;
        }
        // �g�[�N�������I ���݂̃X�e�[�g�ɂ��A�����𕪊򂷂�
        switch (state)
        {
          case HTTP_ANCHORSTATE_ANCHOR:
            // URL���R�s�[
            ret      = getBoardURL(bbsData.boardURL, bbsData.boardNick, 
                                   ptr, data);
            chkToken = "</A>";
            ptr      = data + 1;  // 1 == StrLen(">");
            state    = HTTP_ANCHORSTATE_LABEL;
            break;

          case HTTP_ANCHORSTATE_LABEL:
            StrNCopy(bbsData.boardName, ptr, (data - ptr));
            switch (ret)
            {
              case NNSH_BBSTYPE_MACHIBBS:
                // �܂� BBS�̔������ꍇ
                StrCat(bbsData.boardName, NNSH_MACHINAME_SUFFIX);
                /** not break! **/
              case NNSH_BBSTYPE_2ch:
                // 2ch�̔� or �܂� BBS�̔������ꍇ
                bbsData.threadCnt = NNSH_BBSLIST_AGAIN;
                bbsData.state     = (UInt8) NNSH_BBSSTATE_INVISIBLE;
                bbsData.bbsType   = (UInt8) ret;
                {
                    // �{�[�h�f�[�^���c�a�ɓo�^����B
                    (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase),
                                            &bbsData);
                }
                ret = NNSH_BBSTYPE_ERROR;
                break;

              default:
                // BBS�Ƃ͉������Ȃ�
                break;
            }

            // ���̔���T��
            state     = HTTP_ANCHORSTATE_NORMAL;
            chkToken  = "<A";
            MemSet(&bbsData, sizeof(NNshBoardDatabase), 0x00);
            ptr       = data + 4; // 4 == StrLen("</A>");
            parseSize = ptr - top;
            break;

          case HTTP_ANCHORSTATE_NORMAL:
          default:
            state     = HTTP_ANCHORSTATE_ANCHOR;
            chkToken  = ">";
            ptr       = data + 2; // 2 == StrLen("<A");
            break;
        }
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   updateBoardList                                          */
/*                                                        �{�[�h�ꗗ�̍X�V */
/*-------------------------------------------------------------------------*/
static void updateBoardList(void)
{
    Err                   ret;
    Char                 *ptr, boardNick[MAX_NICKNAME], *saveBBSInfo, *bufPtr;
    UInt16                bufSize, cnt, index, useBBSCnt, lp;
    UInt32                fileSize, offset, readSize;
    DmOpenRef             dbRef;
    NNshFileRef           fileRef;
    NNshBoardDatabase     tmpBd;

    // �ꎞ�̈�̊m��
    bufSize = NNshParam->bufferSize + MARGIN;
    bufPtr     = NULL;
    while (bufPtr == NULL)
    {
        bufPtr  = (Char *) MemPtrNew(bufSize);
        bufSize = bufSize - 12;
    }
    MemSet(bufPtr, bufSize, 0x00);

    // �g�p��BBS���𐔂��āA���݂̐ݒ��ޔ�����B
    Show_BusyForm(MSG_BBSBACKUP_INFO);
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);

    // �g�p���ɐݒ肳�ꂽBBS���𐔂���
    useBBSCnt = 0;
    for (index = cnt; index != 0; index--)
    {
        MemSet(&tmpBd, sizeof(tmpBd), 0x00);
        GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            useBBSCnt++;
        }
    }

    // �ݒ肳��Ă���BBS�����L������
    saveBBSInfo = MemPtrNew((sizeof(Char) * MAX_NICKNAME) * (useBBSCnt + 1));
    if (saveBBSInfo == NULL)
    {
        // �������A�������s��(BBS�g�p���͋L�^���Ȃ�)
        useBBSCnt = 0;
    }
    else
    {
        // �̈揉����
        MemSet(saveBBSInfo,
               (sizeof(Char) * MAX_NICKNAME) * (useBBSCnt + 1),0x00);
    }

    // �a�a�r�ꗗ��S���폜����B
    SetMsg_BusyForm(MSG_BBSDELETE_INFO);
    lp  = 0;
    ptr = saveBBSInfo;
    for (index = cnt; index != 0; index--)
    {
        GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
        if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            // BBS�g�p��񂪂���΁A���̏����o���Ă���
            if (lp < useBBSCnt)
            {
                StrCopy(ptr, tmpBd.boardNick);
                lp++;
                ptr = ptr + MAX_NICKNAME;
            }
        }
        DeleteRecordIDX_NNsh(dbRef, (index - 1));
    }
    useBBSCnt = lp;

    // �܂��A�I�t���C���pBBS����(�擪��)�o�^���Ă���
    MemSet (&tmpBd, sizeof(NNshBoardDatabase), 0x00);
    StrCopy(tmpBd.boardNick, OFFLINE_THREAD_NICK);
    StrCopy(tmpBd.boardURL,  OFFLINE_THREAD_URL);
    StrCopy(tmpBd.boardName, OFFLINE_THREAD_NAME);
    tmpBd.state     = NNSH_BBSSTATE_VISIBLE;
    tmpBd.threadCnt = NNSH_BBSLIST_AGAIN;
    (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);

    // �t�@�C�������M�f�[�^��ǂݏo���Ȃ���{�[�h�ꗗ���X�V����
    SetMsg_BusyForm(MSG_BBSPARSE_INFO);
    OpenFile_NNsh(FILE_RECVMSG, 
                  (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                  &fileRef);
    GetFileSize_NNsh(&fileRef, &fileSize);

    // (�f�o�b�O���b�Z�[�W�\��)
    NNsh_DebugMessage(ALTID_INFO, FILE_RECVMSG, " fileSize:", fileSize);
    
    // ��M�f�[�^�����ׂēǂݏo���A���̒�����BBS���̕����𒊏o�A�o�^
    offset = 0;
    while (offset < fileSize)
    {
        readSize = 0;
        ret = ReadFile_NNsh(&fileRef, offset, bufSize, bufPtr, &readSize);
        if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
        {        
            NNsh_DebugMessage(ALTID_ERROR, 
                              "ReadFile_NNsh() ", FILE_RECVMSG, ret);
            break;
        }

        // BBS���̒��o�����DB�ɓo�^���郁�C������
        parseBoard(dbRef, bufPtr, &readSize);
        offset = offset + readSize;
    }
    if (bufPtr != NULL)
    {
        MemPtrFree(bufPtr);
    }
    CloseFile_NNsh(&fileRef);
    SetMsg_BusyForm(MSG_BBSSORT_INFO);

    // �o�^����BBS���𐮗񂷂�(Quick Sort���g�p)
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_CHAR, 0);

    // �_�u����BBS�����폜����
    //  (BBS���͐��񂵂Ă��邽�߁A�_�u����BBS���͘A�����Ă���͂�)
    SetMsg_BusyForm(MSG_BBSDUPCHK_INFO);
    GetDBCount_NNsh(dbRef, &cnt);
    if (cnt != 0)
    {
        MemSet(&tmpBd, sizeof(tmpBd), 0x00);
        GetRecord_NNsh(dbRef, (cnt - 1), sizeof(tmpBd), &tmpBd);
        MemSet (boardNick, sizeof(boardNick), 0x00);
        StrCopy(boardNick, tmpBd.boardNick);
        for (index = (cnt - 1) ; index != 0; index--)
        {
            MemSet(&tmpBd, sizeof(tmpBd), 0x00);
            GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
            if (StrCompare(boardNick, tmpBd.boardNick) == 0)
            {
                // INDEX�̑傫���ق����폜����
                DeleteRecordIDX_NNsh(dbRef, index);
            }
            else
            {
                MemSet (boardNick, sizeof(boardNick), 0x00);
                StrCopy(boardNick, tmpBd.boardNick);
            }
        }
    }

    //  �ꎞ�I�ɋL�����Ă�����BBS�g�p�ݒ�𔽉f������(���\���Ԃ����邩���H)
    SetMsg_BusyForm(MSG_BBSRESTORE_INFO);
    GetDBCount_NNsh(dbRef, &cnt);
    for (index = cnt; index != 0; index--)
    {
        GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
        for (ptr = saveBBSInfo, lp = 0;
             lp < useBBSCnt;
             lp++, ptr = ptr + MAX_NICKNAME)
        {
	    if (StrCompare(tmpBd.boardNick, ptr) == 0)
            {
                tmpBd.state = NNSH_BBSSTATE_VISIBLE;
                break;
            }
        }
        // BBS��Ԃ�(�K��)�X�V����
        UpdateRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
    }

    // ���Ƃ��܂�
    if (saveBBSInfo != NULL)
    {
        MemPtrFree(saveBBSInfo);
    }
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm();
    return;
}

/*=========================================================================*/
/*   Function :   GetBBSList                                               */
/*                                            Internet���ꗗ���擾���� */
/*=========================================================================*/
Boolean GetBBSList(Char *url)
{
    Err                ret;
    UInt16             buttonID, cnt;
    DmOpenRef          dbRef;
    NNshBoardDatabase  tmpBd;

    // �a�a�r�擾�J�n���m�F����
    buttonID = NNsh_ConfirmMessage(ALTID_CONFIRM, url, MSG_BBSURL_INFO, 0);
    if (buttonID != 0)
    {
        // �L�����Z���{�^����������Ă����ꍇ�ABBS�ꗗ���擾�����ɏI������
        //   �������A���̃^�C�~���O��BBS�ꗗ���O���������ꍇ�ɂ́AOFFLINE
        // �p�̃X�������ǉ�����B
        goto SET_DUMMY_BBS;
    }

    // �z�X�g����{�[�h�ꗗ�f�[�^���擾
    ret = NNshHttp_comm(HTTP_SENDTYPE_GET, url, NULL, NULL,
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    if (ret != errNone)
    {
        // �ʐM�G���[����
        if (ret == netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "", ret);
        }
        // �G���[�I������
        goto SET_DUMMY_BBS;
    }

    // ��M�����{�[�h�ꗗ�f�[�^��ϊ����A���݋L�^����Ă�����̂ƒu��������B
    updateBoardList();
    return (true);

SET_DUMMY_BBS:
    // BBS�ꗗ���O���������ꍇ�ɂ́AOFFLINE�p�̃X�������ǉ����鏈��
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);
    if (cnt == 0)
    {
        // �I�t���C�����O�pBBS����o�^
        MemSet (&tmpBd, sizeof(NNshBoardDatabase), 0x00);
        StrCopy(tmpBd.boardNick, OFFLINE_THREAD_NICK);
        StrCopy(tmpBd.boardURL,  OFFLINE_THREAD_URL);
        StrCopy(tmpBd.boardName, OFFLINE_THREAD_NAME);
        tmpBd.bbsType   = NNSH_BBSTYPE_2ch;
        tmpBd.state     = NNSH_BBSSTATE_VISIBLE;
        tmpBd.threadCnt = NNSH_BBSLIST_AGAIN;
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);

        // BBS�ꗗ��ʂ�����������
        if ((NNshGlobal->bbsTitles) != NULL)
        {
            MemPtrFree((NNshGlobal->bbsTitles));
            (NNshGlobal->bbsTitles) = NULL;
        }
        CloseDatabase_NNsh(dbRef);
        return (true);
    }
    CloseDatabase_NNsh(dbRef);
    return (false);
}

///////////////////////////////////////////////////////////////////////////////
//////////   �g�pBBS�I���֘A
///////////////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------------*/
/*   Function :   CloseForm_SelectBBS                                      */
/*                                        ��ʕ\���Ŏg�p�����ꎞ�̈�̊J�� */
/*-------------------------------------------------------------------------*/
static void closeForm_SelectBBS(void)
{
    // �g�����f�[�^�̂��Ƃ��܂�
    MemPtrFree((NNshGlobal->tempInfo));
    (NNshGlobal->tempInfo) = NULL;

    // BBS�ꗗ��ʂ�����������
    if ((NNshGlobal->bbsTitles) != NULL)
    {
        MemPtrFree((NNshGlobal->bbsTitles));
        (NNshGlobal->bbsTitles) = NULL;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   setBBSListItem                                           */
/*                                   �I������BBS�����g�p����ݒ�ɔ��f���� */
/*-------------------------------------------------------------------------*/
static void setBBSListItem(void)
{
    Err                   ret;
    UInt16                index, cnt, useCnt;
    DmOpenRef             dbRef;
    NNshBoardDatabase     bbsData;

    // �a�a�r�ꗗ���J���A���݂̓o�^BBS�����擾����
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);

    // BBS�����X�V����
    useCnt = 0;
    for (index = 0; index < cnt; index++)
    {
        // BBS�����擾����
        ret =  GetRecord_NNsh(dbRef, index, sizeof(bbsData), &bbsData);
        if (ret != errNone)
        {
            // �����ŃG���[�͂��肦�Ȃ��͂��Ȃ񂾂�...
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh()", "", ret);
            continue;
        }
        if (((NNshGlobal->tempInfo)->checkBoxState[index] == 
                                          NNSH_BBSSTATE_VISIBLE)||(index == 0))
        {
            bbsData.state = NNSH_BBSSTATE_VISIBLE;
            useCnt++;
        } 
        else
        {
            bbsData.state = NNSH_BBSSTATE_INVISIBLE;
        } 
        UpdateRecord_NNsh(dbRef, index, sizeof(bbsData), &bbsData);
    }

    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(dbRef);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   setAllBBS_Selection                                      */
/*                                            �{�[�h�ꗗ�I�������ꊇ�X�V */
/*-------------------------------------------------------------------------*/
static void setAllBBS_Selection(UInt16 value)
{
    FormType  *frm;
    TableType *tableP;
    UInt16     row, nofRow;

    // ���ׂđI�� �������� ��������
    for (row = 0; row < (NNshGlobal->tempInfo)->nofRecords; row++)
    {
        (NNshGlobal->tempInfo)->checkBoxState[row] = value;
    }

    // ��ʏ�̃I�u�W�F�N�g�ɐݒ�𔽉f������
    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
    nofRow = TblGetNumberOfRows(tableP);
    for (row = 0; row < nofRow; row++)
    {
        TblSetItemInt(tableP, row, 1, value);
    }

    //�e�[�u���̕\����Ԃ��X�V����
    TblEraseTable(tableP);
    TblDrawTable (tableP);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ctlSelEvt_SelectBBS                                        */
/*                                                �{�^���������ꂽ���̏��� */
/*-------------------------------------------------------------------------*/
static Boolean ctlSelEvt_SelectBBS(EventType *event)
{
    Boolean ret = true;
    Err     err;

    switch (event->data.ctlSelect.controlID)
    {
      // �u���ׂđI���v�{�^��
      case BTNID_SELECT_ALLBBS:
        setAllBBS_Selection(NNSH_BBSSTATE_VISIBLE);
        break;

      // �u���ׂĉ����v�{�^��
      case BTNID_DESELECT_ALLBBS:
        setAllBBS_Selection(NNSH_BBSSTATE_INVISIBLE);
        break;

      // �u�ݒ�v�{�^��
      case BTNID_SELECT_BBS:
        // ���݂̐ݒ���u�g�p��Ԋi�[���[�N�̈�v�ɔ��f������
        set_Selection_Info((NNshGlobal->tempInfo)->currentPage);
        // �g�pBBS�ݒ��DB�ɔ��f����
        Show_BusyForm(MSG_BBSLISTSETTING_INFO);
        setBBSListItem();

        // �g�pBBS�ݒ�DB��VFS�Ƀo�b�N�A�b�v����
        if ((NNshParam->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            err = BackupDatabaseToVFS_NNsh(DBNAME_BBSLIST);
            if (err != errNone)
            {
                // �G���[�\��
                NNsh_DebugMessage(ALTID_ERROR, "BackupDatabaseToVFS()",
                                  DBNAME_BBSLIST, err);
            }
        }
        Hide_BusyForm();

        // �t�H�[���Ŏg�p�����ꎞ�̈�̊J��
        closeForm_SelectBBS();

        // �X���ꗗ��ʂɖ߂�
        FrmGotoForm(FRMID_THREAD);
        break;

      // ����ȊO�̃{�^��(���肦�Ȃ�)
      default:
        ret = false;
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : ctlRepEvt_SelectBBS                                        */
/*                                        ���s�[�g�{�^���������ꂽ���̏��� */
/*-------------------------------------------------------------------------*/
static Int16 ctlRepEvt_SelectBBS(EventType *event)
{
    Int16 command = 0;

    switch (event->data.ctlRepeat.controlID)
    {
      case RPTID_MANAGEBBS_UP:
        // �ナ�s�[�g�{�^�� : �O�̃y�[�W��\��
        command = -1;
        break;

      case RPTID_MANAGEBBS_DOWN:
        // �����s�[�g�{�^�� : ���̃y�[�W��\��
        command = 1;
        break;

      default:
        // ����ȊO�̃{�^��
        break;
    }
    return (command);
}

/*-------------------------------------------------------------------------*/
/*   Function : keyDownEvt_SelectBBS                                       */
/*                                                  �L�[���͂��ꂽ���̏��� */
/*-------------------------------------------------------------------------*/
static Int16 keyDownEvt_SelectBBS(EventType *event)
{
    Int16  command = 0;
    UInt16 keyCode;

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
        // B�L�[����
      case chrCapital_B:
      case chrSmall_B:
        // ������
        command = (NNshGlobal->tempInfo)->maxPage
                                  - ((NNshGlobal->tempInfo)->currentPage + 1);
        break;

        // T�L�[����
      case chrCapital_T:
      case chrSmall_T:
        // �擪��
        command = (NNshGlobal->tempInfo)->currentPage * (-1);
        break;


      // �u��v�����������̏���(�W���O�_�C������) : �O�̃y�[�W��\��
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case chrCapital_P:  // P�L�[�̓���
      case chrSmall_P:
        command = -1;
        break;

      // �u���v�����������̏���(�W���O�_�C������) : ���̃y�[�W��\��
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case chrCapital_N:  // N�L�[�̓���
      case chrSmall_N:
        command = 1;
        break;

      // �W���O�_�C����������(��)�̏���
      case vchrJogPushedDown:
      // �J�[�\���L�[(��)�������̏���
      case chrUnitSeparator:
        if (((NNshGlobal->tempInfo)->maxPage - 
                                     (NNshGlobal->tempInfo)->currentPage) < 5)
        {
            command = (NNshGlobal->tempInfo)->maxPage
                                  - ((NNshGlobal->tempInfo)->currentPage + 1);
        }
        else
        {
            command = 5;
        }
        break;

        // �W���O�_�C����������(��)�̏���
      case vchrJogPushedUp:
      // �J�[�\���L�[(��)�������̏���
      case chrRecordSeparator:
        if ((NNshGlobal->tempInfo)->currentPage < 5)
        {
            command = (NNshGlobal->tempInfo)->currentPage * (-1);
        }
        else
        {
            command = -5;
        }
        break;

      // Q�L�[����
      case chrCapital_Q:
      case chrSmall_Q:
        // ����ؒf
        NNshNet_LineHangup();
        break;

      // �㉺�ȊO�̏���(�������Ȃ�)
      case vchrJogRelease:
      case vchrJogBack:
      default:
        break;
    }
    return (command);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Set_BBS_Selection                                        */
/*                                        �\�����̃{�[�h�ꗗ�I�������擾 */
/*-------------------------------------------------------------------------*/
static void set_Selection_Info(UInt16 pageNum)
{
    FormType  *frm;
    TableType *tableP;
    UInt16     nofRow, lp, startItem;

    // �t�H�[���̏����擾����
    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));

    // �擾����擪��INDEX�ԍ������肷��
    nofRow    = (NNshGlobal->tempInfo)->nofRow;
    startItem = pageNum * (nofRow - 1);

    // �\������Ă���f�[�^���u�g�p��Ԋi�[���[�N�̈�v�ɔ��f������
    for (lp = 0; lp < nofRow; lp++)
    {
        // �f�[�^�̍ő吔�𒴂������ǂ����̃`�F�b�N
        if ((startItem + lp) >= (NNshGlobal->tempInfo)->nofRecords)
        {
            return;
        }
        (NNshGlobal->tempInfo)->checkBoxState[startItem + lp]
                                               = TblGetItemInt(tableP, lp, 1);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   display_BBS_Selection                                    */
/*                                  �{�[�h�ꗗ�I������(�y�[�W�P�ʂ�)�\�� */
/*-------------------------------------------------------------------------*/
static void display_BBS_Selection(UInt16 pageNum)
{
    FormType         *frm;
    TableType        *tableP;
    Char             *namePtr;
    UInt16           *chkPtr, nofRow, lp, startItem;
    DmOpenRef         dbRef;
    NNshBoardDatabase tmpBd;

    // �t�H�[���̏����擾����
    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
    nofRow = TblGetNumberOfRows(tableP);

    // ���ݕ\�����̃y�[�W����(��ʉE���)�\������
    MemSet ((NNshGlobal->tempInfo)->PageLabel, BBSSEL_PAGEINFO, 0x00);
    NUMCATI((NNshGlobal->tempInfo)->PageLabel, (pageNum + 1));
    StrCat ((NNshGlobal->tempInfo)->PageLabel, " / ");
    NUMCATI((NNshGlobal->tempInfo)->PageLabel, 
            (NNshGlobal->tempInfo)->maxPage);
    NNshWinSetFieldText(frm, FLDID_PAGENUM, (NNshGlobal->tempInfo)->PageLabel,
			StrLen((NNshGlobal->tempInfo)->PageLabel));

    // �擾����擪��INDEX�ԍ������肷��
    startItem = pageNum * (nofRow - 1);

    // ��ʂɕ\����������x�����쐬���Ȃ���A��ʂ�ݒ肷��
    MemSet((NNshGlobal->tempInfo)->BBSName_Temp, 
           (NNshGlobal->tempInfo)->BBSName_size, 0x00);
    namePtr = (NNshGlobal->tempInfo)->BBSName_Temp;
    chkPtr  = (NNshGlobal->tempInfo)->checkBoxState;
    for (lp = 0; lp < nofRow; lp++)
    {
        // �f�[�^�̍ő吔�𒴂������ǂ����̃`�F�b�N
        if ((startItem + lp) < (NNshGlobal->tempInfo)->nofRecords)
        {
            // �����x����namePtr�ɃR�s�[����(�� + boardNick��\������)
            MemSet(&tmpBd, sizeof(tmpBd), 0x00);
            OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
            GetRecord_NNsh(dbRef, (lp + startItem), sizeof(tmpBd), &tmpBd);
            CloseDatabase_NNsh(dbRef);
            StrCopy(namePtr, tmpBd.boardName);
            StrCat (namePtr, "(");
            StrCat (namePtr, tmpBd.boardNick);
            namePtr[StrLen(namePtr) - 1] = ')';

            // ��̍쐬
            TblSetRowData(tableP, lp, (UInt32) (lp + 1));

            // �f�[�^�Z�b�g (���̂̐ݒ�)
            TblSetItemStyle(tableP, lp, 0, labelTableItem);
            TblSetItemPtr  (tableP, lp, 0, namePtr);

            // �f�[�^�Z�b�g (�`�F�b�N�{�b�N�X)
            TblSetItemStyle(tableP, lp, 1, checkboxTableItem);
            TblSetItemInt (tableP, lp, 1, (UInt16) chkPtr[lp + startItem]);

            // �����x���i�[�̈�����ւ����߂�
            namePtr = namePtr + StrLen(namePtr) + 1;

            TblSetRowUsable(tableP, lp, true);
        }
        else
        {
            TblSetRowUsable(tableP, lp, false);
        }
    }
    
    // �e�[�u���̃f�[�^��\���ł���悤�ɂ���
    TblSetColumnUsable(tableP, 0, true);
    TblSetColumnUsable(tableP, 1, true);

    // ��ʂ̍ĕ\��
    TblEraseTable(tableP);
    TblDrawTable (tableP);
    return;
}

/*=========================================================================*/
/*   Function : Handler_SelectBBS                                          */
/*                                           �a�a�r�Ǘ��̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_SelectBBS(EventType *event)
{
    Int16  addPage = 0;

    switch (event->eType)
    {
      case ctlSelectEvent:
        // �{�^���������ꂽ�Ƃ��̏���
        ctlSelEvt_SelectBBS(event);
        break;

      case ctlRepeatEvent:
        // ���s�[�g�{�^���������ꂽ�Ƃ��̏���
        addPage = ctlRepEvt_SelectBBS(event);
	break;

      case keyDownEvent:
        // �L�[���͂��ꂽ�Ƃ��̏���
        addPage = keyDownEvt_SelectBBS(event);
	break;

      case menuEvent:
        // ����ؒf
        if (event->data.menu.itemID == MNUID_NET_DISCONN)
        {
            NNshNet_LineHangup();
        }
        return (false);
	break;

      default:
        // ��L�ȊO�̃C�x���g
	break;
    }

    // �y�[�W�̈ړ�����
    if ((addPage != 0)&&
        (((NNshGlobal->tempInfo)->currentPage + addPage) >= 0)&&
        (((NNshGlobal->tempInfo)->currentPage +addPage) <
                                              (NNshGlobal->tempInfo)->maxPage))
    {
        // ���݂̐ݒ���u�g�p��Ԋi�[���[�N�̈�v�ɔ��f������
        set_Selection_Info((NNshGlobal->tempInfo)->currentPage);

        // �\������y�[�W�ԍ����X�V����
        (NNshGlobal->tempInfo)->currentPage =
                                  (NNshGlobal->tempInfo)->currentPage +addPage;

        //////////  �X���I����ʂ��X�V����  /////////
        display_BBS_Selection((NNshGlobal->tempInfo)->currentPage);
        return (true);
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_SelectBBS                                       */
/*                                       �g�p����BBS����I�������ʂ��J�� */
/*=========================================================================*/
void OpenForm_SelectBBS(FormType *frm)
{
    Err                    ret;
    DmOpenRef              dbBBSRef;
    UInt16                 bbsCnt,lp, nofRow;
    UInt32                 bufSize;
    NNshBoardDatabase      tmpBd;
    TableType             *tableP;

    // �g�pBBS�I����ʗp�o�b�t�@�̈�̊m��
    if ((NNshGlobal->tempInfo) != NULL)
    {
        MemPtrFree((NNshGlobal->tempInfo));
    }
    (NNshGlobal->tempInfo) = MemPtrNew(sizeof(NNshBBS_Temp_Info));
    if ((NNshGlobal->tempInfo) == NULL)
    {
        // �̈�m�ۃG���[�A���̈ꗗ��ʂɖ߂�
        FrmGotoForm(FRMID_THREAD);
        return;
    }
    MemSet((NNshGlobal->tempInfo), sizeof(NNshBBS_Temp_Info), 0x00);

    // �񐔂��擾����
    tableP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TBLID_BBS_SELECTION));
    nofRow = TblGetNumberOfRows(tableP);

    // BUSY�E�B���h�E��\������
    Show_BusyForm(MSG_READ_BBSLIST_WAIT);

    // �o�^����Ă�������擾
    OpenDatabase_NNsh (DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbBBSRef);
    GetDBCount_NNsh   (dbBBSRef, &bbsCnt);

    // �g�p��Ԋi�[���[�N�̈�̊m��(�m�ۍς݂������ꍇ�ɂ͉�����Ă���o�^)
    if ((NNshGlobal->tempInfo)->checkBoxState != NULL)
    {
        MemPtrFree((NNshGlobal->tempInfo)->checkBoxState);
        (NNshGlobal->tempInfo)->checkBoxState = NULL;
    }
    bufSize = (bbsCnt + 1) * sizeof(UInt16) + 8;
    (NNshGlobal->tempInfo)->checkBoxState = (UInt16 *) MemPtrNew(bufSize);
    if ((NNshGlobal->tempInfo)->checkBoxState == NULL)
    {
        //  �o�b�t�@�̈�m�ۃG���[
        NNsh_InformMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"size:",bufSize);
        goto ERROR_END;
    }
    MemSet((NNshGlobal->tempInfo)->checkBoxState, bufSize, 0x00);

    // BBS�̎g�p����Ԃ��ꎞ�̈�ɔ��f������
    for (lp = 0; lp < bbsCnt; lp++)
    {
        ret = GetRecord_NNsh(dbBBSRef, lp, sizeof(tmpBd), &tmpBd);
        if (ret == errNone)
        {
            (NNshGlobal->tempInfo)->checkBoxState[lp] = tmpBd.state;
        }
    }

    // ���\���̈�̊m��(�m�ۍς݂������ꍇ�ɂ͉�����Ă���ēx�m�ۂ���)
    if ((NNshGlobal->tempInfo)->BBSName_Temp != NULL)
    {
        MemPtrFree((NNshGlobal->tempInfo)->BBSName_Temp);
        (NNshGlobal->tempInfo)->BBSName_Temp = NULL;
    }
    bufSize = (nofRow + 1) * 
                    (MAX_BOARDNAME + MAX_NICKNAME + 3 + 1) * sizeof(Char) + 8;
    (NNshGlobal->tempInfo)->BBSName_Temp = (Char *) MemPtrNew(bufSize);
    if ((NNshGlobal->tempInfo)->BBSName_Temp == NULL)
    {
        //  �o�b�t�@�̈�m�ۃG���[
        NNsh_InformMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"Size:",bufSize);

        // �`�F�b�N��Ԃ̗̈���J��
        MemPtrFree((NNshGlobal->tempInfo)->checkBoxState);
        (NNshGlobal->tempInfo)->checkBoxState = NULL;

        goto ERROR_END;
    }
    (NNshGlobal->tempInfo)->BBSName_size = bufSize;
    MemSet((NNshGlobal->tempInfo)->BBSName_Temp, bufSize, 0x00);

    // BBS�I����ʍő�y�[�W�������߂�
    (NNshGlobal->tempInfo)->maxPage     = (bbsCnt / (nofRow - 1));
    if ((bbsCnt % (nofRow - 1)) > 1)
    {
        //  �X�N���[�����l������ƁA1�y�[�W�Ɏ���(nofRow - 1)��BBS����
        // �\���ł��邪�A�ŏ��̃y�[�W������ nofRow�\���ł��邽�߁A����
        // �������l�����Ă��܂�y�[�W�����߂�B
        (NNshGlobal->tempInfo)->maxPage++;
    }
    (NNshGlobal->tempInfo)->currentPage = 0;
    (NNshGlobal->tempInfo)->nofRecords  = bbsCnt;
    (NNshGlobal->tempInfo)->nofRow      = nofRow;

    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(dbBBSRef);

    // BUSY�E�B���h�E����������
    Hide_BusyForm();

    //////////  �X���I����ʂ�\������  /////////
    display_BBS_Selection((NNshGlobal->tempInfo)->currentPage);

    // BBS�^�C�g�����̓N���A����
    if ((NNshGlobal->bbsTitles) != NULL)
    {
        MemPtrFree((NNshGlobal->bbsTitles));
        (NNshGlobal->bbsTitles) = NULL;
    }
    return;

ERROR_END:
    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(dbBBSRef);

    // BUSY�E�B���h�E����������
    Hide_BusyForm();

    // �f�[�^���N���A����
    closeForm_SelectBBS();

    // �X���ꗗ��ʂɖ߂�
    FrmGotoForm(FRMID_THREAD);
    return;
}

/*=========================================================================*/
/*   Function :   Get_BBS_Info                                             */
/*                                     ���X�g�I��ԍ����BBS�����擾���� */
/*=========================================================================*/
Err Get_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo)
{
    Err              ret;
    DmOpenRef        bbsRef;
    UInt16          *bbsIdx, index;
    NNshBBSRelation *relPtr;

#if 0    // ���̊֐����Ăяo���O�ɂ��̃G���[�͔���ς݁A�A�A�̂͂��B

    // BBS�ꗗ���u���C�ɓ���v�u�擾�ς݃X���S�āv�u���ǂ���v�̎��̓G���[
    if ((selBBS == NNSH_SELBBS_FAVORITE)||
        (selBBS == NNSH_SELBBS_GETALL)||
        (selBBS == NNSH_SELBBS_NOTREAD))
    {    
        return (~errNone - 3);
    }

#endif

    // BBS�̃C���f�b�N�X���擾����
    if (NNshGlobal->boardIdxH == 0)
    {
        // �C���f�b�N�X�擾���s
        return (~errNone);
    }
    relPtr = MemHandleLock(NNshGlobal->boardIdxH);
    if (relPtr == NULL)
    {
        // �C���f�b�N�X�擾���s
        return (~errNone);
    }
    bbsIdx = &(relPtr->bbsIndex);
    index  = bbsIdx[selBBS];
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // �f�[�^�x�[�X���I�[�v�����ABBS�����擾����
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
    ret = GetRecord_NNsh(bbsRef, index, sizeof(NNshBoardDatabase), bbsInfo);
    CloseDatabase_NNsh(bbsRef);
    if (ret != errNone)
    {
        // ���R�[�h�̎擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Info()", "", ret);
        MemSet(bbsInfo, sizeof(NNshBoardDatabase),   0x00);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   Update_BBS_Info                                          */
/*                                     ���X�g�I��ԍ�����BBS�����X�V���� */
/*=========================================================================*/
Err Update_BBS_Info(UInt16 selBBS, NNshBoardDatabase *bbsInfo)
{
    UInt16          *bbsIdx, index;
    NNshBBSRelation *relPtr;
    
    // BBS�ꗗ���u���C�ɓ���v�u�擾�ς݃X���S�āv�u���ǂ���v�̎��̓G���[
    if ((selBBS == NNSH_SELBBS_FAVORITE)||
        (selBBS == NNSH_SELBBS_GETALL)||
        (selBBS == NNSH_SELBBS_NOTREAD))
    {    
        return (~errNone - 3);
    }

    // BBS�̃C���f�b�N�X���擾����
    if (NNshGlobal->boardIdxH == 0)
    {
        // �C���f�b�N�X�擾���s
        return (~errNone);
    }
    relPtr = MemHandleLock(NNshGlobal->boardIdxH);
    if (relPtr == NULL)
    {
        // �C���f�b�N�X�擾���s
        return (~errNone);
    }
    bbsIdx = &(relPtr->bbsIndex);
    index  = bbsIdx[selBBS];
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // BBS�̃f�[�^�x�[�X�Ƀf�[�^��o�^����
    return (Update_BBS_Database(index, bbsInfo));
}

/*=========================================================================*/
/*   Function : Get_BBS_Database                                           */
/*                                      (boardNick����BBS�����擾����)   */
/*=========================================================================*/
Err Get_BBS_Database(Char *boardNick, NNshBoardDatabase *bbs, UInt16 *index)
{
    Err       ret;
    DmOpenRef dbRef;

    *index = 0;

    // BBS�����f�[�^�x�[�X����擾
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);

    ret = SearchRecord_NNsh(dbRef, boardNick, NNSH_KEYTYPE_CHAR, 0,
                            sizeof(NNshBoardDatabase), bbs, index);

    CloseDatabase_NNsh(dbRef);

    return (ret);
}

/*=========================================================================*/
/*   Function : Update_BBS_Database                                        */
/*                                          (index����BBS�����L�^����)   */
/*=========================================================================*/
Err Update_BBS_Database(UInt16 index, NNshBoardDatabase *bbs)
{
    Err       ret;
    DmOpenRef bbsRef;

    // �f�[�^�x�[�X���I�[�v�����ABBS�����X�V����
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
    ret = UpdateRecord_NNsh(bbsRef, index, sizeof(NNshBoardDatabase), bbs);
    CloseDatabase_NNsh(bbsRef);

    return (ret);
}
