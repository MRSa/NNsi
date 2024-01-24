/*============================================================================*
 *
 *  $Id: bbsmng.c,v 1.53 2006/05/08 15:18:43 mrsa Exp $
 *
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
static void    display_BBS_Selection(UInt16     pageNum, Int16 selection);

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

    // �܂�BBS�̃{�[�h�����v�ɂ��Ă݂�B�B�B(�VURL)
    chk = StrStr(top, "machi.to");
    if ((chk != NULL)&&(chk < bottom))
    {
        // �܂�BBS��URL�́A"http://(�z�X�g��)/" �����ɂ���B
        StrNCopy(url,  ptr,  (ptr2 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_MACHIBBS);
    }

    // �܂�BBS�̃{�[�h�����v�ɂ��Ă݂�B�B�B(��URL)
    chk = StrStr(top, "machibbs.com");
    if ((chk != NULL)&&(chk < bottom))
    {
        // �܂�BBS��URL�́A"http://(�z�X�g��)/" �����ɂ���B
        StrNCopy(url,  ptr,  (ptr2 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_MACHIBBS);
    }

    if ((NNshGlobal->NNsiParam)->notCheckBBSURL != 0)
    {
        // ��URL�̗L���`�F�b�N�����Ȃ��ꍇ(�Ƃ肠����)
        StrNCopy(url,  ptr,  (ptr3 - ptr));
        StrNCopy(nick, ptr2, (ptr3 - ptr2));
        return (NNSH_BBSTYPE_2ch);
#if 0
        {
            // �܂�BBS��URL�́A"http://(�z�X�g��)/" �����ɂ���B
            StrNCopy(url,  ptr,  (ptr2 - ptr));
            StrNCopy(nick, ptr2, (ptr3 - ptr2));
            return (NNSH_BBSTYPE_MACHIBBS);
        }
#endif
    }

    return (~errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function :   parseBoard                                               */
/*                                  ��M�f�[�^�̍\�����߂���єꗗ�̍X�V */
/*-------------------------------------------------------------------------*/
static void parseBoard(DmOpenRef dbRef, Char *top, UInt32 *readSize)
{
    Err               err;
    Char             *data, *ptr, *chkToken;
    UInt32            parseSize;
    UInt16            state, index, ret;
    NNshBoardDatabase bbsData, bbsData2;

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
            if ((data - ptr) < (MAX_BOARDNAME - sizeof(NNSH_MACHINAME_SUFFIX)))
            {
                StrNCopy(bbsData.boardName, ptr, (data - ptr));
            }
            else
            {
                StrNCopy(bbsData.boardName, 
                         ptr, 
                         (MAX_BOARDNAME - sizeof(NNSH_MACHINAME_SUFFIX)));
            }
            switch (ret)
            {
              case NNSH_BBSTYPE_SHITARABA:
                // ������΂������ꍇ
                StrCat(bbsData.boardName, NNSH_SHITARABA_SUFFIX);
                break;

              case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
              case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
                // �������@JBBS�������ꍇ
                StrCat(bbsData.boardName, NNSH_SHITARABAJBBS_SUFFIX);
                break;

              case NNSH_BBSTYPE_MACHIBBS:
                // �܂�BBS�������ꍇ
                StrCat(bbsData.boardName, NNSH_MACHINAME_SUFFIX);
                break;

              case NNSH_BBSTYPE_2ch:
              case NNSH_BBSTYPE_2ch_EUC:
              case NNSH_BBSTYPE_OTHERBBS:
              case NNSH_BBSTYPE_OTHERBBS_2:
                // 2ch�������ꍇ
                break;

              default:
                // BBS�Ƃ͉������Ȃ�
                goto NEXT_BBS;
                break;
            }

            // BBS�f�[�^��DB�ɓo�^����
            bbsData.state     = (UInt8) NNSH_BBSSTATE_INVISIBLE;
            bbsData.bbsType   = (UInt8) ret;

            if ((NNshGlobal->NNsiParam)->bbsOverwrite == 0)
            {
                // �{�[�h�f�[�^���c�a�ɓo�^����B
                (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase),
                                        &bbsData);
            }
            else
            {
                // �{�[�h�f�[�^�����łɓo�^�ς݂��`�F�b�N����
                err = SearchRecord_NNsh(dbRef, bbsData.boardNick,
                                        NNSH_KEYTYPE_CHAR, 0,
                                        sizeof(NNshBoardDatabase),
                                        &bbsData2, &index);
                if (err != errNone)
                {
                    // �f�[�^�����炸�A�V�K�o�^����
                    (void) EntryRecord_NNsh(dbRef, 
                                            sizeof(NNshBoardDatabase),
                                            &bbsData);
                }
                else
                {
                    // �����f�[�^�����A�����f�[�^���㏑������
                    bbsData.state   = bbsData2.state;
                    bbsData.bbsType = bbsData2.bbsType;
                    (void) UpdateRecord_NNsh(dbRef, index,
                                             sizeof(NNshBoardDatabase),
                                             &bbsData);
                }
            }
            ret = NNSH_BBSTYPE_ERROR;

NEXT_BBS:
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
    bufSize = (NNshGlobal->NNsiParam)->bufferSize + MARGIN;
    bufPtr     = NULL;
    while (bufPtr == NULL)
    {
        bufPtr  = (Char *) MEMALLOC_PTR(bufSize);
        bufSize = bufSize - 12;
    }
    MemSet(bufPtr, bufSize, 0x00);

    // �g�p��BBS���𐔂��āA���݂̐ݒ��ޔ�����B
    Show_BusyForm(MSG_BBSBACKUP_INFO);
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    GetDBCount_NNsh(dbRef, &cnt);

    useBBSCnt   = 0;
    saveBBSInfo = NULL;

    if ((NNshGlobal->NNsiParam)->bbsOverwrite == 0)
    { 
        // �g�p���ɐݒ肳�ꂽBBS���𐔂���
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
        saveBBSInfo = 
                MEMALLOC_PTR((sizeof(Char) * (MAX_NICKNAME + 1)) * (useBBSCnt + 1));
        if (saveBBSInfo == NULL)
        {
            // �������A�������s��(BBS�g�p���͋L�^���Ȃ�)
            useBBSCnt = 0;
        }
        else
        {
            // �̈揉����
            MemSet(saveBBSInfo,
                   (sizeof(Char) * MAX_NICKNAME + 1) * (useBBSCnt + 1),0x00);
        }

        // �a�a�r�ꗗ��S���폜����B
        SetMsg_BusyForm(MSG_BBSDELETE_INFO);
        ptr = saveBBSInfo;
        lp  = 0;
        for (index = cnt; index != 0; index--)
        {
            GetRecord_NNsh(dbRef, (index - 1), sizeof(tmpBd), &tmpBd);
            if ((tmpBd.state & NNSH_BBSSTATE_VISIBLE)== NNSH_BBSSTATE_VISIBLE)
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
    }

    // ���R�[�h�������m�F����
    GetDBCount_NNsh(dbRef, &cnt);
    if (cnt == 0)
    {
        // ���R�[�h���P�����Ȃ������ꍇ�A�I�t���C���pBBS����(�擪��)�o�^���Ă���
        MemSet (&tmpBd, sizeof(NNshBoardDatabase), 0x00);
        StrCopy(tmpBd.boardNick, OFFLINE_THREAD_NICK);
        StrCopy(tmpBd.boardURL,  OFFLINE_THREAD_URL);
        StrCopy(tmpBd.boardName, OFFLINE_THREAD_NAME);
        tmpBd.state     = NNSH_BBSSTATE_VISIBLE;
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);
    }

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
        if (readSize == 0)
        {
            // �p�[�X���s...
            NNsh_DebugMessage(ALTID_INFO, "readSize == 0", " ret:", ret);
            break;
        }
    }
    MEMFREE_PTR(bufPtr);
    CloseFile_NNsh(&fileRef);
    SetMsg_BusyForm(MSG_BBSSORT_INFO);

    // �o�^����BBS���𐮗񂷂�(Quick Sort���g�p)
    QsortRecord_NNsh(dbRef, NNSH_KEYTYPE_CHAR, 0);

    // �_�u����BBS��(boardNick)���폜����
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

    //    BBS�ꗗ���㏑�����Ȃ��ꍇ�ɂ́A�ꎞ�I�ɋL�����Ă�����
    //  BBS�g�p�ݒ�𔽉f������(���\���Ԃ����邩���H)
    if ((NNshGlobal->NNsiParam)->bbsOverwrite == 0)
    {
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
    }

    // ���Ƃ��܂�
    MEMFREE_PTR(saveBBSInfo);
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(true);
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
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
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
        (void) EntryRecord_NNsh(dbRef, sizeof(NNshBoardDatabase), &tmpBd);

        // BBS�ꗗ��ʂ�����������
        if (NNshGlobal->boardIdxH != 0)
        {
            MemHandleFree(NNshGlobal->boardIdxH);
            NNshGlobal->boardIdxH = 0;
        }
        MEMFREE_PTR((NNshGlobal->bbsTitles));
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
/*   Function :   searchBBSTitle                                           */
/*                                                 BBS�^�C�g���������s���� */
/*-------------------------------------------------------------------------*/
static Err searchBBSTitle(Char *title, UInt16 direction, UInt16 *recNum)
{
    Err                ret;
    Int16              step;
    UInt16             end, loop;
    Char               string[BUFSIZE];
    Char               match1[MAX_BOARDNAME], match2[MAX_BOARDNAME];
    DmOpenRef          dbRef;
    MemHandle          bdH;
    NNshBoardDatabase *tmpBd;

    // "������"�̕\��
    MemSet (string, 0x00, sizeof(string));
    StrCopy(string, MSG_SEARCHING_TITLE);
    StrCat (string, title);
    Show_BusyForm(string);

    // �啶������������ʂ��Ȃ��ꍇ�ɂ́A�������ɕϊ�
    MemSet(match1, sizeof(match1), 0x00);
    if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
    {
        (void) StrToLower(match1, title);
    }
    else
    {
        (void) StrCopy(match1, title);
    }

    // BBS�����f�[�^�x�[�X����擾
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);

    switch (direction)
    {
      case NNSH_SEARCH_ALL:
        // �擪����S����
        *recNum = 0;
        GetDBCount_NNsh(dbRef, &end);
        end--;
        step = 1;
        break;

      case NNSH_SEARCH_FORWARD:
        // �t�H���[�h�����w��
        GetDBCount_NNsh(dbRef, &end);
        end--;
        step = 1;
        break;

      case NNSH_SEARCH_BACKWARD: 
      default:
        // �o�b�N���[�h�����w��
        end  = 0;
        step = -1;
        break;
    }
    loop = *recNum;

    // �����̎�����
    do {
        // GetRecord_NNsh()���Ń[���N���A���Ă��邽�ߕs�v
        // MemSet(&tmpBd, sizeof(NNshBoardDatabase), 0x00);
        GetRecordReadOnly_NNsh(dbRef,(loop + step), &bdH, (void **)  &tmpBd);

        //  �w�肳�ꂽ������ƃ}�b�`���邩�m�F
        MemSet(match2, sizeof(match2), 0x00);
        if ((NNshGlobal->NNsiParam)->searchCaseless != 0)
        {
            // �X���^�C���������ɕϊ�
            (void) StrToLower(match2, tmpBd->boardName);
        }
        else
        {
            (void) StrCopy(match2, tmpBd->boardName);
        }
        ReleaseRecordReadOnly_NNsh(dbRef, bdH);

        loop = loop + step;
        if (StrStr(match2, match1) != NULL)
        {
            // ���������I
            ret = errNone;
            goto FUNC_END;
        }
    } while (loop != end);

    // �����Ɏ��s�A������Ȃ�����
    ret = ~errNone;

FUNC_END:
    *recNum = loop;
    CloseDatabase_NNsh(dbRef);
    Hide_BusyForm(false);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   entryBBSRecord                                           */
/*                                                           BBS�̌ʓo�^ */
/*-------------------------------------------------------------------------*/
static void entryBBSRecord(UInt16 recNum)
{
    Err                err;
    Boolean            ret = false;
    FormType          *prevFrm, *diagFrm;
    UInt16             btnId, item;
    ListType          *lstP;
    NNshBoardDatabase  bbsData;
    Char               msg[BIGBUF], nickBak[MAX_NICKNAME + MARGIN];
    DmOpenRef          bbsRef;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // PalmOS v3.5�ȉ��Ȃ�A�O�̃t�H�[������������B
    if (NNshGlobal->palmOSVersion < 0x03503000)
    {
        FrmEraseForm(prevFrm);
    }

    // �_�C�A���O�t�H�[�����A�N�e�B�u�ɂ���
    diagFrm  = FrmInitForm(FRMID_BOARDINPUT);
    if (diagFrm == NULL)
    {
        return;
    }
    FrmSetActiveForm(diagFrm);

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

    if (recNum != NNSH_NOTENTRY_BBS)
    {
        // ���R�[�h�f�[�^���擾
        MemSet(&bbsData, sizeof(bbsData), 0x00);
        OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
        err = 
           GetRecord_NNsh(bbsRef, recNum, sizeof(NNshBoardDatabase), &bbsData);
        CloseDatabase_NNsh(bbsRef);
        if (err == errNone)
        {
            // �f�[�^���擾�ł����Ƃ��́A���̎擾���e����ʂɔ��f

            // BBS�^�C�v
            switch (bbsData.bbsType)
            {
              case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
                // ������΁�JBBS
                item = 7;
                break;

              case NNSH_BBSTYPE_2ch_EUC:
                // 2ch(EUC)
                item = 6;
                break;

              case NNSH_BBSTYPE_OTHERBBS_2:
                // 2ch�݊�(2)
                item = 5;
                break;

              case NNSH_BBSTYPE_OTHERBBS:
                // 2ch�݊�
                item = 4;
                break;
              
              case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
                // ������΁�JBBS
                item = 3;
                break;

              case NNSH_BBSTYPE_SHITARABA:
                // �������
                item = 2;
                break;

              case NNSH_BBSTYPE_MACHIBBS:
                // �܂�BBS
                item = 1;
                break;

              case NNSH_BBSTYPE_2ch:
              default:
                // �Q�����˂�
                item = 0;
                break;
            }
            NNshWinSetPopItems(diagFrm,POPTRID_BBSINPUT, LSTID_BBSINPUT, item);

            // BBS��
            NNshWinSetFieldText(diagFrm, FLDID_BBSINPUT_NAME, false,
                                bbsData.boardName, (MAX_BOARDNAME));
            // URL
            NNshWinSetFieldText(diagFrm, FLDID_BBSINPUT_URL, false,
                                bbsData.boardURL, (MAX_URL));
            // BBS Nick
            NNshWinSetFieldText(diagFrm, FLDID_BBSINPUT_NICK, true,
                                bbsData.boardNick, (MAX_NICKNAME));
            MemSet (nickBak, sizeof(nickBak), 0x00);
            StrCopy(nickBak, bbsData.boardNick);

            if (((bbsData.state) & NNSH_BBSSTATE_SUSPEND) == NNSH_BBSSTATE_SUSPEND)
            {
                // �擾��~���̏�Ԑݒ�
                item = 1;
                SetControlValue(diagFrm, CHKID_BBS_STOP, &item);
            }

            // �W���X�����x���̐ݒ�
            item = 1;
            switch((bbsData.state) & NNSH_BBSSTATE_LEVELMASK)
            {
              case NNSH_BBSSTATE_FAVOR_L1:
                // ���C�ɓ��背�x��(��)
                SetControlValue(diagFrm, PBTNID_FAVOR_L1, &item);
                break;

              case NNSH_BBSSTATE_FAVOR_L2:
                // ���C�ɓ��背�x��(����)
                SetControlValue(diagFrm, PBTNID_FAVOR_L2, &item);
                break;

              case NNSH_BBSSTATE_FAVOR_L3:
                // ���C�ɓ��背�x��(��)
                SetControlValue(diagFrm, PBTNID_FAVOR_L3, &item);
                break;

              case NNSH_BBSSTATE_FAVOR_L4:
                // ���C�ɓ��背�x��(����)
                SetControlValue(diagFrm, PBTNID_FAVOR_L4, &item);
                break;

              case NNSH_BBSSTATE_FAVOR:
                // ���C�ɓ��背�x��(��)
                SetControlValue(diagFrm, PBTNID_FAVOR_L5, &item);
                break;

              default:
                // ���C�ɓ���ł͂Ȃ�
                SetControlValue(diagFrm, PBTNID_FAVOR_L0, &item);
                break;
            }
        }
    }

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_DialogAction);

    // ���̓E�B���h�E���J��
    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_DIALOG_OK)
    {
        // OK�{�^���������ꂽ�Ƃ��A�o�^��������쐬����
        MemSet(&bbsData, sizeof(bbsData), 0x00);

        // BBS���擾
        NNshWinGetFieldText(diagFrm, FLDID_BBSINPUT_NAME, bbsData.boardName,
                            (MAX_BOARDNAME - 1));
        // URL�擾
        NNshWinGetFieldText(diagFrm, FLDID_BBSINPUT_URL, bbsData.boardURL,
                            (MAX_URL - 1));
        // Nick�擾
        NNshWinGetFieldText(diagFrm, FLDID_BBSINPUT_NICK, bbsData.boardNick,
                            (MAX_NICKNAME - 1));

        // �X�����x�������BBS���g�p����ݒ���X�V����
        item = 0;
        UpdateParameter(diagFrm, PBTNID_FAVOR_L0, &item);
        if (item != 0)
        {
            // ���C�ɓ���ݒ肳��Ă��Ȃ�
            bbsData.state = NNSH_BBSSTATE_VISIBLE;
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L1, &item);
        if (item != 0)
        {
            // ���C�ɓ���ݒ�(��)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L1);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L2, &item);
        if (item != 0)
        {
            // ���C�ɓ���ݒ�(����)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L2);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L3, &item);
        if (item != 0)
        {
            // ���C�ɓ���ݒ�(��)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L3);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L4, &item);
        if (item != 0)
        {
            // ���C�ɓ���ݒ�(����)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR_L4);
            goto DELETE_DIALOG;
        }
        UpdateParameter(diagFrm, PBTNID_FAVOR_L5, &item);
        if (item != 0)
        {
            // ���C�ɓ���ݒ�(��)
            bbsData.state = (NNSH_BBSSTATE_VISIBLE)|(NNSH_BBSSTATE_FAVOR);
            goto DELETE_DIALOG;
        }

DELETE_DIALOG:
        // �擾��~���̏��
        UpdateParameter(diagFrm, CHKID_BBS_STOP, &item);
        if (item != 0)
        {
            // �擾��~����Ԃ̐ݒ�
            bbsData.state = (bbsData.state)|(NNSH_BBSSTATE_SUSPEND);
        }

        // BBS�^�C�v�̐ݒ�
        lstP = FrmGetObjectPtr(diagFrm,
                               FrmGetObjectIndex(diagFrm, LSTID_BBSINPUT));
        item = LstGetSelection(lstP);
        switch (item)
        {
          case 7:
            // �������@JBBS�^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_SHITARABAJBBS_OLD;
            break;

          case 6:
            // 2ch(EUC)�^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_2ch_EUC;
            break;

          case 5:
            // 2ch�݊��^�C�v(Part2)
            bbsData.bbsType = NNSH_BBSTYPE_OTHERBBS_2;
            break;

          case 4:
            // 2ch�݊��^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_OTHERBBS;
            break;
            
          case 3:
            // �������@JBBS�^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_SHITARABAJBBS_RAW;
            break;

          case 2:
            // ������΃^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_SHITARABA;
            break;

          case 1:
            // �܂��a�a�r�^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_MACHIBBS;
            break;

          case 0:
          default:
            // 2ch�^�C�v
            bbsData.bbsType = NNSH_BBSTYPE_2ch;
            break;
        }
        MemSet (msg, sizeof(msg), 0x00);
        StrCopy(msg, bbsData.boardName);
        StrCat (msg, "(");
        StrCat (msg, bbsData.boardURL);
        StrCat (msg, ", Nick:");
        StrCat (msg, bbsData.boardNick);
        StrCat (msg, ") type:");
        NUMCATI(msg, (UInt16) bbsData.bbsType);
        btnId = NNsh_ConfirmMessage(ALTID_CONFIRM, 
                                    NNSH_CONFIRMMSG_ENTRYBBS, msg, 0);
        if (btnId == 0)
        {
            /////////////////////////////////////////////////////
            // BBS�f�[�^�x�[�X�ɓo�^/�X�V����
            ////////////////////////////////////////////////////
            if (recNum == NNSH_NOTENTRY_BBS)
            {
                OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);

                EntryRecord_NNsh(bbsRef,sizeof(NNshBoardDatabase),&bbsData);

                // �o�^����BBS���𐮗񂷂�(Quick Sort���g�p)
                QsortRecord_NNsh(bbsRef, NNSH_KEYTYPE_CHAR, 0);

                CloseDatabase_NNsh(bbsRef);

                // �o�^�������Ƃ�ʒm�A�ċN����v������
                NNsh_InformMessage(ALTID_WARN, 
                                   bbsData.boardName, NNSH_MSGINFO_DBENTRY, 0);
            }
            else
            {

                // BBS�����X�V����
                err = Update_BBS_Database(recNum, &bbsData);

                // �{�[�h�j�b�N�l�[����ύX�������ǂ����m�F����
                if (StrCompare(nickBak, bbsData.boardNick) != 0)
                {
                    // �{�[�h�j�b�N�l�[����ύX�����ꍇ�ɂ́A�\�[�g����
                    OpenDatabase_NNsh(DBNAME_BBSLIST, 
                                      DBVERSION_BBSLIST, &bbsRef);
                    // �o�^����BBS���𐮗񂷂�(Quick Sort���g�p)
                    QsortRecord_NNsh(bbsRef, NNSH_KEYTYPE_CHAR, 0);

                    CloseDatabase_NNsh(bbsRef);
                }

                // �X�V�������Ƃ�ʒm����
                NNsh_InformMessage(ALTID_WARN, bbsData.boardName, 
                                   NNSH_MSGINFO_DBUPDATE, err);
            }
        }
    }
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

#ifdef USE_HANDERA
    if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
    {
        FrmEraseForm(prevFrm);

        // �V���N�\�����ŏ���
        NNshSilkMinimize(prevFrm);

        FrmDrawForm(prevFrm);
    }
    else
#endif
    {
        // �V���N�\�����ŏ���
        NNshSilkMinimize(prevFrm);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   CloseForm_SelectBBS                                      */
/*                                        ��ʕ\���Ŏg�p�����ꎞ�̈�̊J�� */
/*-------------------------------------------------------------------------*/
static void closeForm_SelectBBS(void)
{
    // �g�����f�[�^�̂��Ƃ��܂�
    MEMFREE_PTR((NNshGlobal->tempInfo)->checkBoxState);
    MEMFREE_PTR((NNshGlobal->tempInfo)->BBSName_Temp);
    MEMFREE_PTR((NNshGlobal->tempInfo));

    // BBS�ꗗ��ʂ�����������
    if (NNshGlobal->boardIdxH != 0)
    {
        // MemHandleUnlock(NNshGlobal->boardIdxH);  // �K�v?
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;
    }
    MEMFREE_PTR((NNshGlobal->bbsTitles));
    NNshGlobal->bbsTitles = 0;  // �O�̂���...
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
            bbsData.state = ((NNSH_BBSSTATE_VISIBLEMASK)&(bbsData.state))|(NNSH_BBSSTATE_VISIBLE);
            useCnt++;
        } 
        else
        {
            bbsData.state = ((NNSH_BBSSTATE_VISIBLEMASK)&(bbsData.state))|(NNSH_BBSSTATE_INVISIBLE);
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
/*   Function : detail_SelectBBS                                           */
/*                                                       BBS�ڍׂ̕\������ */
/*-------------------------------------------------------------------------*/
static Boolean detail_SelectBBS(void)
{
    FormType    *frm;
    TableType   *tableP;
    UInt16       row, col, nofRow, recNum;

    frm    = FrmGetActiveForm();
    tableP = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,TBLID_BBS_SELECTION));
                                 
    nofRow = TblGetNumberOfRows(tableP);
    if (TblGetSelection(tableP, &row, &col) == true)
    {
        // �擾���郌�R�[�h�ԍ������肷��
        recNum = ((NNshGlobal->tempInfo)->currentPage * 
                  ((NNshGlobal->tempInfo)->nofRow - 1)) + row;
        entryBBSRecord(recNum);

        //////////  �X���I����ʂ��X�V����  /////////
        display_BBS_Selection((NNshGlobal->tempInfo)->currentPage,
                              NNSH_ITEM_LASTITEM);
        return (true);
    }
    // �e�[�u�����I������Ă��Ȃ�����(�x���\��)
    NNsh_WarningMessage(ALTID_WARN, NNSH_MSGINFO_NOTSELECTBBS, "", 0);

    return (false);
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

      // �u�ڍׁv�{�^��
      case BTNID_DETAIL_BBS:
        return (detail_SelectBBS());
        break;

      // �u�ݒ�v�{�^��
      case BTNID_SELECT_BBS:
        // ���݂̐ݒ���u�g�p��Ԋi�[���[�N�̈�v�ɔ��f������
        set_Selection_Info((NNshGlobal->tempInfo)->currentPage);
        // �g�pBBS�ݒ��DB�ɔ��f����
        Show_BusyForm(MSG_BBSLISTSETTING_INFO);
        setBBSListItem();

        // �g�pBBS�ݒ�DB��VFS�Ƀo�b�N�A�b�v����
        if (((NNshGlobal->NNsiParam)->useVFS & (NNSH_VFS_DBBACKUP)) != 0)
        {
            err = BackupDatabaseToVFS_NNsh(DBNAME_BBSLIST);
            if (err != errNone)
            {
                // �G���[�\��
                NNsh_DebugMessage(ALTID_ERROR, "BackupDatabaseToVFS()",
                                  DBNAME_BBSLIST, err);
            }
        }
        Hide_BusyForm(true);

        // �t�H�[���Ŏg�p�����ꎞ�̈�̊J��
        closeForm_SelectBBS();

        // �X���ꗗ��ʂɖ߂�
        FrmGotoForm(NNshGlobal->backFormId);
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

    //tungsten T 5way navigator (281����A���ӁI)
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
      case vchrThumbWheelUp:
      case vchrRockerUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case chrUpArrow:
      case chrCapital_P:  // P�L�[�̓���
      case chrSmall_P:
        command = -1;
        break;

      // �u���v�����������̏���(�W���O�_�C������) : ���̃y�[�W��\��
      case vchrPageDown:
      case vchrJogDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case chrDownArrow:
      case chrCapital_N:  // N�L�[�̓���
      case chrSmall_N:
        command = 1;
        break;

      // �W���O�_�C����������(��)�̏���
      // �J�[�\���L�[�E�i5way �E�j�̏���
      // �J�[�\���L�[(��)�������̏���
      case vchrJogPushedDown:
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
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
      // �J�[�\���L�[�i���j�E5way���̏��� 
      // �J�[�\���L�[(��)�������̏���
      case vchrJogPushedUp:
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
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
static void display_BBS_Selection(UInt16 pageNum, Int16 selection)
{
    FormType          *frm;
    TableType         *tableP;
    Char              *namePtr;
    UInt16            *chkPtr, nofRow, lp, startItem;
    DmOpenRef          dbRef;
    MemHandle          bdH;
    NNshBoardDatabase *tmpBd;

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
    NNshWinSetFieldText(frm, FLDID_PAGENUM, true,
                        (NNshGlobal->tempInfo)->PageLabel,
                        StrLen((NNshGlobal->tempInfo)->PageLabel));

    // �擾����擪��INDEX�ԍ������肷��
    startItem = pageNum * (nofRow - 1);

    // ��ʂɕ\����������x�����쐬���Ȃ���A��ʂ�ݒ肷��
    MemSet((NNshGlobal->tempInfo)->BBSName_Temp, 
           (NNshGlobal->tempInfo)->BBSName_size, 0x00);
    namePtr = (NNshGlobal->tempInfo)->BBSName_Temp;
    chkPtr  = (NNshGlobal->tempInfo)->checkBoxState;
    OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &dbRef);
    for (lp = 0; lp < nofRow; lp++)
    {
        // �f�[�^�̍ő吔�𒴂������ǂ����̃`�F�b�N
        if ((startItem + lp) < (NNshGlobal->tempInfo)->nofRecords)
        {
            // �����x����namePtr�ɃR�s�[����(�� + boardNick��\������)
            (void) GetRecordReadOnly_NNsh(dbRef, (lp + startItem), &bdH, 
                                          (void **) &tmpBd);
            StrCopy(namePtr, tmpBd->boardName);
            StrCat (namePtr, "(");
            StrCat (namePtr, tmpBd->boardNick);
            ReleaseRecordReadOnly_NNsh(dbRef, bdH);
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
    CloseDatabase_NNsh(dbRef);
    
    // �e�[�u���̃f�[�^��\���ł���悤�ɂ���
    TblSetColumnUsable(tableP, 0, true);
    TblSetColumnUsable(tableP, 1, true);

    // ��ʂ̍ĕ\��
    TblEraseTable(tableP);
    TblDrawTable (tableP);

    // ���������e�[�u����I������
    if (selection < nofRow)
    {
        TblSelectItem(tableP, selection, 0);
    }
    return;
}

/*=========================================================================*/
/*   Function : Handler_SelectBBS                                          */
/*                                           �a�a�r�Ǘ��̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_SelectBBS(EventType *event)
{
    Int16        addPage = 0;
    UInt16       nofRow, recNum  = 0;
    Char        *ttl;
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
    FormType    *frm;
#endif

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

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        // HandEra/DIA�p��ʃT�C�Y�ύX�C�x���g
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            FrmEraseForm(frm);
            HandEraResizeForm(frm);
            FrmDrawForm(frm);

            //////////  �X���I����ʂ��X�V����  /////////
            display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                  NNSH_ITEM_LASTITEM);
        } 
        break;
#endif  // #ifdef USE_HANDERA

#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            // HandEra/DIA�p��ʃT�C�Y�ύX�C�x���g
            frm = FrmGetActiveForm();
            // FrmEraseForm(frm);
            if (HandEraResizeForm(frm) == true)
            {
                // ��ʂ̃��T�C�Y���s��ꂽ�Ƃ������X�V
                FrmDrawForm(frm);

                //////////  �X���I����ʂ��X�V����  /////////
                display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                      NNSH_ITEM_LASTITEM);
            }
        }
        break;
#endif

      case menuEvent:
        switch (event->data.menu.itemID)
        {
          case MNUID_NET_DISCONN:
            // ����ؒf
            NNshNet_LineHangup();
            break;

          case MNUID_SEARCH_TITLE:
            ttl = ((NNshGlobal->tempInfo)->searchName);
            MemSet(ttl, MAX_BOARDNAME, 0x00);
            // �^�C�g������
            // (��������̓E�B���h�E���J���A�f�[�^���l��)
            if (DataInputDialog(NNSH_INPUTWIN_SEARCH, ttl, MAX_BOARDNAME,
                                NNSH_DIALOG_USE_SEARCH, NULL) != true)
            {
                break;
            }
            addPage = -1;
            recNum = 0;
            // not break;  �����Č�����...

          case MNUID_SEARCH_NEXT:
            // ���������s����(������)
            if (addPage == 0)
            {
                // �\�����Ă���y�[�W�̈�ԉ����猟��
                recNum =  (((NNshGlobal->tempInfo)->currentPage + 1) * 
                           ((NNshGlobal->tempInfo)->nofRow - 1));
                if (recNum >= (NNshGlobal->tempInfo)->nofRecords)
                {
                    // ���R�[�h�����I�[�o�t���[�����ꍇ�͐擪����
                    recNum = 0;
                }
            }
            ttl = ((NNshGlobal->tempInfo)->searchName);
            if (searchBBSTitle(ttl, NNSH_SEARCH_FORWARD, &recNum) == errNone)
            {
                // (�^�C�g������)
                // ���݂̐ݒ���u�g�p��Ԋi�[���[�N�̈�v�ɔ��f������
                set_Selection_Info((NNshGlobal->tempInfo)->currentPage);

                // �\������y�[�W�ԍ����X�V����
                (NNshGlobal->tempInfo)->currentPage =
                               recNum / (((NNshGlobal->tempInfo)->nofRow) - 1);
                nofRow =   recNum % (((NNshGlobal->tempInfo)->nofRow) - 1);

                //////////  �X���I����ʂ��X�V����(�\���y�[�W�̈ړ�) /////
                display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                      nofRow);
                return (true);
            }
            NNsh_InformMessage(ALTID_WARN, MSG_NOTFOUND_TITLE, ttl, 0);

            // ��ʂ��ĕ\������
            display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                                  (NNshGlobal->tempInfo)->nofRow);
            break;

          case MNUID_ENTRY_BBS:
            // ���݂̐ݒ���u�g�p��Ԋi�[���[�N�̈�v�ɔ��f������
            set_Selection_Info((NNshGlobal->tempInfo)->currentPage);

            // �g�pBBS�ݒ��DB�ɔ��f����
            Show_BusyForm(MSG_BBSLISTSETTING_INFO);
            setBBSListItem();
            Hide_BusyForm(true);

            // BBS�̌ʓo�^��A��ʂ����
            entryBBSRecord(NNSH_NOTENTRY_BBS);
            closeForm_SelectBBS();
            FrmGotoForm(NNshGlobal->backFormId);
            break;

          case MNUID_UPDATE_BBS:
            // BBS���ڍ�
            return (detail_SelectBBS());
            break;

          default:
            // ��L�ȊO 
            break;
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
        display_BBS_Selection((NNshGlobal->tempInfo)->currentPage, 
                              NNSH_ITEM_LASTITEM);
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
        // �g�p��Ԋi�[���[�N�̈�̊m��(�m�ۍς݂������ꍇ�ɂ͉�����Ă���o�^)
        MEMFREE_PTR((NNshGlobal->tempInfo)->checkBoxState);
        MEMFREE_PTR((NNshGlobal->tempInfo));
    }
    (NNshGlobal->tempInfo) = MEMALLOC_PTR(sizeof(NNshBBS_Temp_Info));
    if ((NNshGlobal->tempInfo) == NULL)
    {
        // �̈�m�ۃG���[�A���̈ꗗ��ʂɖ߂�
        FrmGotoForm(NNshGlobal->backFormId);
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

    bufSize = (bbsCnt + 1) * sizeof(UInt16) + 8;
    (NNshGlobal->tempInfo)->checkBoxState = (UInt16 *) MEMALLOC_PTR(bufSize);
    if ((NNshGlobal->tempInfo)->checkBoxState == NULL)
    {
        //  �o�b�t�@�̈�m�ۃG���[
        NNsh_DebugMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"size:",bufSize);
        goto ERROR_END;
    }
    MemSet((NNshGlobal->tempInfo)->checkBoxState, bufSize, 0x00);

    // BBS�̎g�p����Ԃ��ꎞ�̈�ɔ��f������
    for (lp = 0; lp < bbsCnt; lp++)
    {
        ret = GetRecord_NNsh(dbBBSRef, lp, sizeof(tmpBd), &tmpBd);
        if (ret == errNone)
        {
            (NNshGlobal->tempInfo)->checkBoxState[lp] = ((tmpBd.state)&(NNSH_BBSSTATE_VISIBLE));
        }
    }

    // ���\���̈�̊m��(�m�ۍς݂������ꍇ�ɂ͉�����Ă���ēx�m�ۂ���)
    MEMFREE_PTR((NNshGlobal->tempInfo)->BBSName_Temp);
    bufSize = (nofRow + 1) * 
                    (MAX_BOARDNAME + MAX_NICKNAME + 3 + 1) * sizeof(Char) + 8;
    (NNshGlobal->tempInfo)->BBSName_Temp = (Char *) MEMALLOC_PTR(bufSize);
    if ((NNshGlobal->tempInfo)->BBSName_Temp == NULL)
    {
        //  �o�b�t�@�̈�m�ۃG���[
        NNsh_DebugMessage(ALTID_ERROR,MSG_MEMORY_ALLOC_FAIL,"Size:",bufSize);

        // �`�F�b�N��Ԃ̗̈���J��
        MEMFREE_PTR((NNshGlobal->tempInfo)->checkBoxState);
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
    Hide_BusyForm(true);

    //////////  �X���I����ʂ�\������  /////////
    display_BBS_Selection((NNshGlobal->tempInfo)->currentPage,
                           NNSH_ITEM_LASTITEM);

    // BBS�^�C�g�����̓N���A����
    if (NNshGlobal->boardIdxH != 0)
    {
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;
    }
    MEMFREE_PTR((NNshGlobal->bbsTitles));
    return;

ERROR_END:
    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(dbBBSRef);

    // BUSY�E�B���h�E����������
    Hide_BusyForm(true);

    // �f�[�^���N���A����
    closeForm_SelectBBS();

    // �X���ꗗ��ʂɖ߂�
    FrmGotoForm(NNshGlobal->backFormId);
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
        (selBBS == NNSH_SELBBS_NOTREAD)||
        (selBBS == NNSH_SELBBS_CUSTOM1)||
        (selBBS == NNSH_SELBBS_CUSTOM2)||
        (selBBS == NNSH_SELBBS_CUSTOM3)||
        (selBBS == NNSH_SELBBS_CUSTOM4)||
        (selBBS == NNSH_SELBBS_CUSTOM5))
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
    
    // BBS�ꗗ������{�[�h�̎��̓G���[
    if (selBBS <= NNSH_SELBBS_CUSTOM2)
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

/*=========================================================================*/
/*   Function :   Create_BBS_INDEX                                         */
/*                                                      BBS�ꗗ�̍쐬����  */
/*=========================================================================*/
Err Create_BBS_INDEX(Char **bdLst, UInt16 *cnt)
{
    Err                   ret = errNone;
    DmOpenRef             bbsRef;
    UInt16                nofBBS, tempSize, lp, useCnt, *idxP;
    Char                 *ptr;
    MemHandle             bdH;
    NNshBoardDatabase    *tmpBd;
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
        CloseDatabase_NNsh(bbsRef);

        // VFS����c�a�����X�g�A(�����\�Ȃ�)
        ret = RestoreDBtoVFS(true);
        if (ret != errNone)
        {
            // BBS�f�[�^�x�[�X�����݂��Ȃ������ꍇ�A�x����\������
            NNsh_WarningMessage(ALTID_WARN,
                                MSG_BOARDDB_NONE1, MSG_BOARDDB_NONE2, 0);
        }
#ifdef ENTRY_INITIAL_BBSDATA_AUTOMATICALLY
        // �u�Q�ƃ��O�v���a�a�r�f�[�^�Ƃ��ēo�^����
        OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
        tmpBd = MEMALLOC_PTR(sizeof(NNshBoardDatabase) + MARGIN);
        if (tmpBd == NULL)
        {
            // �������m�ێ��s...�I������
            return (~errNone);
        }
        MemSet(tmpBd, (sizeof(NNshBoardDatabase) + MARGIN), 0x00);
        StrCopy(tmpBd->boardNick, OFFLINE_THREAD_NICK);
        StrCopy(tmpBd->boardURL,  OFFLINE_THREAD_URL);
        StrCopy(tmpBd->boardName, OFFLINE_THREAD_NAME);
        tmpBd->state = NNSH_BBSSTATE_VISIBLE;
        (void) EntryRecord_NNsh(bbsRef, sizeof(NNshBoardDatabase), tmpBd);
        MEMFREE_PTR(tmpBd);
        CloseDatabase_NNsh(bbsRef);

        // �ēx�J���Ȃ���
        OpenDatabase_NNsh(DBNAME_BBSLIST, DBVERSION_BBSLIST, &bbsRef);
        GetDBCount_NNsh(bbsRef, &nofBBS);
        if (nofBBS == 0)
        {
            // ����͂��肦�Ȃ��͂�����...
            return (~errNone);
        }
#else  // #ifdef ENTRY_INITIAL_BBSDATA_AUTOMATICALLY
        return (~errNone);
#endif  // #ifdef ENTRY_INITIAL_BBSDATA_AUTOMATICALLY
    }

    // �g�p��BBS�����J�E���g����
    useCnt = 0;
    for (lp = 0; lp < nofBBS; lp++)
    {
        (void) GetRecordReadOnly_NNsh(bbsRef, lp, &bdH, (void **) &tmpBd);
        if ((tmpBd->state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            useCnt++;
        } 
        ReleaseRecordReadOnly_NNsh(bbsRef, bdH);
    }
    if (useCnt == 0)
    {
        // "�쐬��"�\���̍폜
        Hide_BusyForm(false);
        CloseDatabase_NNsh(bbsRef);

        // �g�p����f�[�^�x�[�X���ݒ肳��Ă��Ȃ������ꍇ�A�x����\������
        NNsh_WarningMessage(ALTID_WARN,
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
       Hide_BusyForm(false);

       // �̈�m�ۃG���[
       ret = ~errNone;
       NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
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
    *bdLst   = (Char *) MEMALLOC_PTR(tempSize);
    if (*bdLst == NULL)
    {
        MemHandleUnlock(NNshGlobal->boardIdxH);
        MemHandleFree(NNshGlobal->boardIdxH);
        NNshGlobal->boardIdxH = 0;

        // "�쐬��"�\���̍폜
        Hide_BusyForm(false);

        // �̈�m�ۃG���[
        ret = ~errNone;
        NNsh_DebugMessage(ALTID_ERROR, MSG_MEMORY_ALLOC_FAIL,
                          " size:", tempSize);

        // �f�[�^�x�[�X�̃N���[�Y
        CloseDatabase_NNsh(bbsRef);
        return (~errNone);
    }
    MemSet(*bdLst, tempSize, 0x00);
    ptr = *bdLst;
    *cnt  = 0;

    // BBS�ꗗ�̐擪�Ɂu���ǂ���v�u�擾�ςݑS�āv�u���C�ɓ���v��ǉ�����
    StrCopy(ptr, NOTREAD_THREAD_NAME);
    ptr   = ptr + sizeof(NOTREAD_THREAD_NAME); // Terminator���܂߂čl���Ă�
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, GETALL_THREAD_NAME);
    ptr   = ptr + sizeof(GETALL_THREAD_NAME); // Terminator���܂߂čl���Ă�
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, FAVORITE_THREAD_NAME);
    ptr   = ptr + sizeof(FAVORITE_THREAD_NAME); // Terminator���܂߂čl���Ă�
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, CUSTOM1_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom1.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, CUSTOM2_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom2.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, CUSTOM3_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom3.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, CUSTOM4_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom4.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    StrCopy(ptr, CUSTOM5_THREAD_HEAD);
    StrCat (ptr, (NNshGlobal->NNsiParam)->custom5.tabName);
    ptr   = ptr + StrLen(ptr) + 1;
    (*cnt)++;
    *idxP = 0;                       // �_�~�[�f�[�^(���̒l�ɂ͉��l�Ȃ�)
    idxP++;

    // (�g�p����)BBS�^�C�g���ꗗ�ƃC���f�b�N�X���쐬����
    for (lp = 0; lp < nofBBS; lp++)
    {
        (void) GetRecordReadOnly_NNsh(bbsRef, lp, &bdH, (void **) &tmpBd);
        if ((tmpBd->state & NNSH_BBSSTATE_VISIBLE) == NNSH_BBSSTATE_VISIBLE)
        {
            // ���̂��R�s�[����
            StrNCopy(ptr, tmpBd->boardName, LIMIT_BOARDNAME);
            ptr = ptr + StrLen(ptr) + 1;   // 1 ��'\0'�̈Ӗ��A(���X�g�Ŏg�p)
            (*cnt)++;
            *idxP = lp;
            idxP++;
        }
        ReleaseRecordReadOnly_NNsh(bbsRef, bdH);
    }
    NNshGlobal->useBBS = *cnt;
    MemHandleUnlock(NNshGlobal->boardIdxH);

    // "�쐬��"�\���̍폜
    Hide_BusyForm(false);

    // �f�[�^�x�[�X�̃N���[�Y
    CloseDatabase_NNsh(bbsRef);
    return (ret);
}
