/*============================================================================*
 *  FILE: 
 *     msglist.c
 *
 *  Description: 
 *     Message(Thread) list drawings for NNsh. 
 *
 *===========================================================================*/
#define MSGLIST_C
#include "local.h"

/*-------------------------------------------------------------------------*/
/*   Function : CheckDispList                                              */
/*                                         ��ʂɕ\������/���Ȃ��𔻒肷�� */
/*-------------------------------------------------------------------------*/
Boolean CheckDispList(UInt16 bbsIndex, NNshSubjectDatabase *subjP,
                                                        NNshBoardDatabase *bdP)
{
    Char *ptr, *match1, *match2;
    Char  fileName[MAX_THREADFILENAME + MARGIN];

    // �\������/���Ȃ��̏�������(���ɂ���ďꍇ����)
    switch (bbsIndex)
    { 
      case NNSH_SELBBS_GETALL:
        // �擾�ςݑS�Ă�\��
        if ((subjP->state == NNSH_SUBJSTATUS_NOT_YET)||
            (((NNshGlobal->NNsiParam)->notListReadOnly != 0)&&
             ((subjP->boardNick[0]  == '!')&&(subjP->boardNick[1]  == 'G'))))
        {
            //  �u���擾�v���������͎Q�ƃ��O���u�Q�ƃ��O�͕\�����Ȃ��v��
            // �ꍇ�ɂ͕\�����Ȃ�
            return (false);
        }
        break;

      case NNSH_SELBBS_FAVORITE:
        // ���C�ɓ���w���\��
        if ((subjP->msgAttribute & NNSH_MSGATTR_FAVOR) < (NNshGlobal->NNsiParam)->displayFavorLevel)
        {
            // �u���C�ɓ���v�ł͂Ȃ��̂ŕ\�����Ȃ�
            return (false);
        }
        break;

      case NNSH_SELBBS_NOTREAD:
        // ���ǂ����\��
        if (((NNshGlobal->NNsiParam)->notListReadOnlyNew != 0)&&
             ((subjP->boardNick[0]  == '!')&&(subjP->boardNick[1]  == 'G')))
        {
            //�@�Q�ƃ��O���u���ǂ���ɂ̓`�F�b�N�Ȃ��v�������Ă����ꍇ�ɂ�
            // �\�����Ȃ�
            return (false);
        }        
        if ((subjP->state != NNSH_SUBJSTATUS_NEW)&&
            (subjP->state != NNSH_SUBJSTATUS_UPDATE)&&
            (subjP->state != NNSH_SUBJSTATUS_REMAIN))
        {
            // ���ǂ�����X���ł͂Ȃ��̂ŕ\�����Ȃ�
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM1:
        // ���[�U�ݒ�P�̏����m�F
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom1)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM2:
        // ���[�U�ݒ�Q�̏����m�F
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom2)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM3:
        // ���[�U�ݒ�R�̏����m�F
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom3)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM4:
        // ���[�U�ݒ�S�̏����m�F
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom4)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_CUSTOM5:
        // ���[�U�ݒ�T�̏����m�F
        if (CheckIfCustomTabIsValid(bdP->boardNick, subjP, &((NNshGlobal->NNsiParam)->custom5)) == false)
        {
            return (false);
        }
        break;

      case NNSH_SELBBS_OFFLINE:
        // �Q�ƃ��O
#ifdef USE_STRSTR
        if (StrCompare(subjP->boardNick, OFFLINE_THREAD_NICK) != 0)
#else
        if ((subjP->boardNick[0]  != '!')||
            (subjP->boardNick[1]  != 'G'))
#endif
        {
            // �X���̏������Ⴄ�̂ŕ\�����Ȃ�
            return (false);
        }
        if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_DIROFFLINE) != 0)
        {
            if (subjP->dirIndex != (NNshGlobal->NNsiParam)->readOnlySelection)
            {
                // �ۊǂ���Ă���f�B���N�g����������̂ŕ\�����Ȃ�
                return (false);
            }
        }
        if ((NNshGlobal->NNsiParam)->getLogSiteListMode != 0)
        {
            // �ꗗ��ʂ��Q�ƃ��O�ꗗ�T�C�g�ʕ\�����[�h�Ƃ��Ďg�p����ꍇ�B�B�B
            MemSet(fileName, sizeof(fileName), 0x00);
            StrCopy(fileName, FILE_LOGCHARGE_PREFIX);
            NUMCATI(fileName, (NNshGlobal->NNsiParam)->selectedTitleItemForGetLog);
            StrCat (fileName, "-");
            if (StrStr(subjP->threadFileName, fileName) == NULL)
            {
                // �Y���̃��O�ł͂Ȃ������A�A�A�I������
                return (false);
            }
        }
        break;

      default:
        // �u���C�ɓ���v�ł��u�擾�ςݑS�āv�ł��u���ǂ���v�ł��Ȃ��ꍇ
        if (StrCompare(subjP->boardNick, bdP->boardNick) != 0)
        {
            // �X���̏������Ⴄ�̂ŕ\�����Ȃ�
            return (false);
        }
        break;
    }

    if ((NNshGlobal->NNsiParam)->searchPickupMode != 0)
    {
        // �i���݌������[�h... 

        // ���������񂪊m�ۍς݂��m�F����
        if (NNshGlobal->searchTitleH == 0)
        {
            return (false);
        }

        // ���������񂪐ݒ�ς݂̏ꍇ�ɂ́A���̕�������擾����
        ptr = MemHandleLock(NNshGlobal->searchTitleH);
        if (ptr == NULL)
        {
            return (false);
        }
        if (*ptr == '\0')
        {
            MemHandleUnlock(NNshGlobal->searchTitleH);
            return (false);
        }

        // �^�C�g�����[�h�̌���
        if (StrStr(subjP->threadTitle, ptr) == NULL)
        {
            if ((NNshGlobal->NNsiParam)->searchCaseless == 0)
            {
                MemHandleUnlock(NNshGlobal->searchTitleH);
                return (false);
            }

            // �召�����ϊ����Ĕ�r...
            match1 = MEMALLOC_PTR(MAX_THREADNAME + MARGIN);
            match2 = MEMALLOC_PTR(StrLen(ptr) + MARGIN);
            (void) StrToLower(match1, subjP->threadTitle);
            (void) StrToLower(match2, ptr);
            if (StrStr(match1, match2) == NULL)
            {
                MEMFREE_PTR(match2);
                MEMFREE_PTR(match1);
                MemHandleUnlock(NNshGlobal->searchTitleH);
                return (false);
            }
            MEMFREE_PTR(match2);
            MEMFREE_PTR(match1);
        }
        MemHandleUnlock(NNshGlobal->searchTitleH);
    }

    // �\������X���Ɣ���
    return (true);
}                               

/*-------------------------------------------------------------------------*/
/*   Function : copyThreadTitleString                                      */
/*                                �X���^�C�g����������R�s�[(�f�[�^�ϊ��t) */
/*-------------------------------------------------------------------------*/
static void copyThreadTitleString(Char  *dest, Char *src, UInt16 length,
                                  UInt16 state, UInt16 msgNumber)
{
    UInt16  msgAttribute;
    UInt16 msgState, size;
    Char   *dst;

    // �G���[��Ԃ����������Ƃ��A���b�Z�[�W�̐擪�� '#'��\������
    dst = dest;
    if (((state >> 8)&(NNSH_MSGATTR_ERROR)) == NNSH_MSGATTR_ERROR)
    {
        *dst = '#';
        dst++;
    }

    // �擾�\�񒆂̂Ƃ��A���b�Z�[�W�̐擪�� '!'��\������
    if (((state >> 8)&(NNSH_MSGATTR_GETRESERVE)) == NNSH_MSGATTR_GETRESERVE)
    {
        *dst = '!';
        dst++;
    }

    // ���b�Z�[�W�̏�Ԃ��L���Ŕ��f
    msgState = (state & 0xff);
    switch (msgState)
    {
      case NNSH_SUBJSTATUS_NEW:
        /** �V�K�擾(�Ď擾) **/
        *dst = '*';
        break;

      case NNSH_SUBJSTATUS_UPDATE:
        /** ���b�Z�[�W�X�V **/
        *dst = '+';
        break;

      case NNSH_SUBJSTATUS_REMAIN:
        /** ���ǂ��� **/
        *dst = '-';
        break;

      case NNSH_SUBJSTATUS_ALREADY:
        /** ���b�Z�[�W�S���ǂ� **/
        *dst = ':';
        break;
#ifdef USE_MARK_SUBJSTATUS_OVER
      case NNSH_SUBJSTATUS_OVER:
        /** ���X���̍ő�𒴂��� **/
        *dst = '~';
#endif
      case NNSH_SUBJSTATUS_NOT_YET:
      default:                
        /** ���b�Z�[�W���擾 **/
        *dst = ' ';
        break;
    }
    dst++;

    // ���b�Z�[�W���̃R�s�[����
    if ((NNshGlobal->NNsiParam)->printNofMessage != 0)
    {
        switch (msgState)
        {
          case NNSH_SUBJSTATUS_UNKNOWN:
            // ���X������͂Ȃ��Ƃ�\������
            StrCat(dst, "[???]");
            break;
          case NNSH_SUBJSTATUS_NEW:
            // �V���Ɏ擾�������Ƃ�\������
            StrCat(dst, "[NEW]");
            break;
          case NNSH_SUBJSTATUS_UPDATE:
            // �X�V�����������Ƃ�\������
            StrCat(dst, "[New]");
            break;

          default:
            // �擾���b�Z�[�W����\������
            if (((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 2)||((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 3))
            {
                // �X�����x����\���ɒǉ�����
                msgAttribute = ((state >> 8)&(NNSH_MSGATTR_FAVOR));
                if (msgAttribute == NNSH_MSGATTR_FAVOR_L1)
                {
                    StrCat(dst, "[L.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR_L2)
                {
                    StrCat(dst, "[1.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR_L3)
                {
                    StrCat(dst, "[2.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR_L4)
                {
                    StrCat(dst, "[3.");
                }                
                else if (msgAttribute == NNSH_MSGATTR_FAVOR)
                {
                    StrCat(dst, "[H.");
                }                
                else
                {
                    StrCat(dst, "[");
                }                
            }
            else
            {
                StrCat(dst, "[");
            }
            (void) NUMCATI(dst, msgNumber);
            StrCat(dst, "]");
            break;
        }
    }
    size = StrLen(dest);
    dst  = dest + size;

    // �X���^�C�g���̃R�s�[
    StrNCopyHanZen(dst, src, (length - size - 1));
    return;
}

/*=========================================================================*/
/*   Function : ClearMsgTitleInfo                                          */
/*                                           �X���^�C�ꗗ�̊֘A�̈���J��  */
/*=========================================================================*/
void ClearMsgTitleInfo(void)
{
    MEMFREE_PTR(NNshGlobal->msgListIndex);
    MEMFREE_PTR(NNshGlobal->msgListStrings);
    MEMFREE_PTR(NNshGlobal->msgTitleIndex);
    MEMFREE_PTR(NNshGlobal->msgTitleStatus);
    MEMFREE_PTR(NNshGlobal->msgNumbers);

    NNshGlobal->nofTitleItems = 0;

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : AllocMsgTitleInfo                                          */
/*                                           �X���^�C�ꗗ�̊֘A�̈���m��  */
/*-------------------------------------------------------------------------*/
static Err AllocMsgTitleInfo(UInt16 nofItems)
{
    UInt16 allocSize;

    // �X���ꗗ���X�����R�[�h�ԍ��ϊ��e�[�u���̈�̏�����
    allocSize = sizeof(UInt16) * (nofItems + 2);
    NNshGlobal->msgListIndex = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgListIndex == NULL)
    {
        return (~errNone + 10);
    }
    MemSet(NNshGlobal->msgListIndex, allocSize, 0x00);

    // �X���^�C�g���̐擪�ʒu�i�[�̈�̏�����
    allocSize = sizeof(Char *) * (nofItems + 2);
    NNshGlobal->msgTitleIndex = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgTitleIndex == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        return (~errNone + 11);    
    }
    MemSet(NNshGlobal->msgTitleIndex, allocSize, 0x00);

    // �X���^�C�g���i�[�̈�̏�����
    allocSize = (LIMIT_TITLENAME_DEFAULT + 10 + MARGIN) * (nofItems + 1);
    NNshGlobal->msgListStrings = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgListStrings == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        MEMFREE_PTR(NNshGlobal->msgTitleIndex);
        return (~errNone + 12);
    }
    MemSet(NNshGlobal->msgListStrings, allocSize, 0x00);

    // �X���^�C�g����Ԋi�[�̈�̏�����
    allocSize = sizeof(UInt16) * (nofItems + 2);
    NNshGlobal->msgTitleStatus = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgTitleStatus == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        MEMFREE_PTR(NNshGlobal->msgTitleIndex);
        MEMFREE_PTR(NNshGlobal->msgListStrings);
        return (~errNone + 13);
    }
    MemSet(NNshGlobal->msgTitleStatus, allocSize, 0x00);

    // �X�����b�Z�[�W���i�[�̈�̏�����
    allocSize = sizeof(UInt16) * (nofItems + 2);
    NNshGlobal->msgNumbers = MEMALLOC_PTR(allocSize);
    if (NNshGlobal->msgNumbers == NULL)
    {
        MEMFREE_PTR(NNshGlobal->msgListIndex);
        MEMFREE_PTR(NNshGlobal->msgTitleIndex);
        MEMFREE_PTR(NNshGlobal->msgListStrings);
        MEMFREE_PTR(NNshGlobal->msgTitleStatus);
        return (~errNone + 14);
    }
    MemSet(NNshGlobal->msgNumbers, allocSize, 0x00);

    NNshGlobal->nofTitleItems = 0;

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   function :   create_TitleList                                         */
/*                                            ���b�Z�[�W�^�C�g���ꗗ���쐬 */
/*-------------------------------------------------------------------------*/
static Err create_TitleList(UInt16 bbsIndex, UInt16 areaCount,
                             Char  *buffer,   Int16  command)
{
    Err                  ret = ~errNone;
    Char                *ptr;
    Int16                stepCnt;
    UInt16               dataIndexMax, loop, startRec, endRec, tempIndex;
    DmOpenRef            dbRef;
    NNshSubjectDatabase *subjDb;
    NNshBoardDatabase    bbsData;
    MemHandle            msgH;

#ifdef DEBUG
    Char debugMsg[129];
#endif

    // ���v�^�C�g�����̏�����
    NNshGlobal->nofTitleItems  = 0;
    endRec                     = 0;
    ptr                        = buffer;

    switch (bbsIndex)
    {
      case NNSH_SELBBS_NOTREAD:     // ���ǂ���
      case NNSH_SELBBS_GETALL:      // �擾�ςݑS��
      case NNSH_SELBBS_FAVORITE:    // ���C�ɓ���X��
        // BBS���͎擾���Ȃ�(�����擾)
        break;

      case NNSH_SELBBS_CUSTOM1:     // CUSTOM1
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom1.boardNick, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM2:     // CUSTOM2
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom2.boardNick, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM3:     // CUSTOM3
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom3.boardNick, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM4:     // CUSTOM4
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom4.boardNick, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_CUSTOM5:     // CUSTOM5
        if (Get_BBS_Info((NNshGlobal->NNsiParam)->custom5.boardNick, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;

      case NNSH_SELBBS_OFFLINE:
        // �Q�ƃ��O
        if (Get_BBS_Info(bbsIndex, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;

      default:
        // ����BBS�ȊO�̂Ƃ�
        if (Get_BBS_Info(bbsIndex, &bbsData) != errNone)
        {
            // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
            goto RECORD_NOTHING;
        }
        break;
    }
    ret = errNone;

    // �X�����Ǘ�DB�I�[�v��
    OpenDatabase_NNsh(DBNAME_SUBJECT, DBVERSION_SUBJECT, &dbRef);
    GetDBCount_NNsh(dbRef, &dataIndexMax);

    // �P�����f�[�^���o�^����Ă��Ȃ������ꍇ
    if (dataIndexMax == 0)
    {
        // �u�ꗗ�Ȃ��v�ݒ蕔���փW�����v
        goto RECORD_NOTHING;
    }

    //////////////////// ���݂̕\����Ԃ��m�F���� ///////////////////////
    if (command == NNSH_STEP_UPDATE)
    {
        // �X���ꗗ���X�V����w���������ꍇ�ɂ́A��Ԃ�"���"�ɕύX����
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
    }

    switch ((NNshGlobal->NNsiParam)->titleDispState)
    {
      case NNSH_DISP_UPPERLIMIT:
      case NNSH_DISP_ALL:
        // �擪���猟������
        (NNshGlobal->NNsiParam)->startTitleRec = (dataIndexMax - 1);
        break;

      case NNSH_DISP_LOWERLIMIT:
        // �������猟������
        (NNshGlobal->NNsiParam)->endTitleRec = 0;
        break;

      default:
        // �������Ȃ�
        break;
    }

    //////////////////// �\���v�����m�F���� ///////////////////////
    switch (command)
    {
      case NNSH_STEP_PAGEUP:
        // 1�y�[�W"��"�ֈړ�������        (���R�[�h�ԍ��̑偨�������j
        stepCnt  = -1;
        startRec = (NNshGlobal->NNsiParam)->endTitleRec;
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
        break;

      case NNSH_STEP_PAGEDOWN:
        // 1�y�[�W"��"�ֈړ�������       �i���R�[�h�ԍ��̏����匟���j
        stepCnt  = 1;
        startRec = (NNshGlobal->NNsiParam)->startTitleRec;
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
        break;

      case NNSH_STEP_TO_BOTTOM:
        // �ꗗ�̖����ֈړ�������        �i���R�[�h�ԍ��̂O���猟���j
        stepCnt  = 1;
        startRec = 0;
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_LOWERLIMIT;
        break;

      case NNSH_STEP_REDRAW:
        // ���ݕ\�����̃y�[�W���ĕ`�悷��i�O��̕\�����R�[�h�ԍ��̐擪����j
        stepCnt  = -1;
        startRec = (NNshGlobal->NNsiParam)->startTitleRec;
        break;

      // case NNSH_STEP_UPDATE:
      // case NNSH_STEP_TO_TOP:
      default:
        // ���������̐擪����ĕ\������  �i���R�[�h�ԍ��̍ő傩�猟���j
        stepCnt  = -1;
        startRec = (dataIndexMax - 1);
        (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
        break;
    }

    //////////////// �\������X���̃��R�[�h�ԍ������ /////////////////////////
    for (loop = startRec + 1;
         ((loop > 0)&&(loop <= dataIndexMax));
         loop = loop + stepCnt)
    {
        // �\�����R�[�h���擾����
        GetRecordReadOnly_NNsh(dbRef, (loop - 1), &msgH, (void **) &subjDb);

        // �\������/���Ȃ��̏�������
        if (CheckDispList(bbsIndex, subjDb, &bbsData) == false)
        {
            // �\�����Ȃ��X���Ɣ���
            ReleaseRecordReadOnly_NNsh(dbRef, msgH);
            continue;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, msgH);

        if (NNshGlobal->nofTitleItems >= areaCount)
        {
            // ���X�g�\�����̕\���\�萔 + 1���I�������̂ŁA�����Ŕ�����
            endRec = NNshGlobal->msgListIndex[(NNshGlobal->nofTitleItems - 1)];
            break;                                          // ���[�v�𔲂���
        }

        // �\�����郌�R�[�h�ԍ����L�����Ă���
        NNshGlobal->msgListIndex[NNshGlobal->nofTitleItems] = (loop - 1);

        // ���R�[�h�ԍ����L�^����
        (NNshGlobal->nofTitleItems)++;

    }
    
    //////////  ���[�v�̏I�������������m�F����
    if (loop == 0)                   // �F �擪�܂Ō���
    {
        // ���R�[�h�̖�������擪�܂Ō����������ǂ����`�F�b�N����
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_UPPERLIMIT)||
             ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_ALL))
        {
            // �S�ĕ\��
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_ALL;
        }
        else
        {
            // ����
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_LOWERLIMIT;
        }
        endRec = loop;
    }
    else if (loop > dataIndexMax)    // �����܂Ō���
    {
        // ���R�[�h�̐擪���疖���܂Ō����������ǂ����`�F�b�N����
        if (((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_LOWERLIMIT)||
            ((NNshGlobal->NNsiParam)->titleDispState == NNSH_DISP_ALL))
        {
            // �S�ĕ\��
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_ALL;
        }
        else
        {
            // ���
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
        }
        endRec = (dataIndexMax - 1);
    }
    else   //   0 < loop <= dataIndexMax �̎�...
    {
        if (startRec == 0)
        {
            // ����
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_LOWERLIMIT;
        }
        else if (startRec == (dataIndexMax - 1))
        {
            // ���
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_UPPERLIMIT;
        }
        else
        {
            // �r���܂ŕ\��
            (NNshGlobal->NNsiParam)->titleDispState = NNSH_DISP_HALFWAY;
        }
    }

    //////// ���Y�^ ///////////////////////////////////////////////////////////
    //   ����A���ǂ��������ꍇ�ANNshGlobal->nofTitleItems��0�ł����ɓ��B    //
    // ���邱�Ƃ�����B(���ǂ���̍ŏI�y�[�W���Q�Ƃ��I������Ƃ��A�X�����폜 //
    // ���Ă��̃y�[�W�ɕ\������X�����Ȃ��Ȃ����ꍇ)                         //
    //  �u�Ȃ��v�ƕ\������邪�A���ۂɃ^�u��I�����čX�V����ƃX���͑��݂��� //
    // �̂ŁA�Ώ����K�v�B(���̊֐����Ăяo�����őΏ�����)                    //
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////// �X���ꗗ�f�[�^�̕��בւ� ////////////////////////////
    if (stepCnt > 0)
    {
        // ���R�[�h�ԍ��̓���ւ�(�~���ɐ��񂳂���)
        for (loop = 0; loop < (NNshGlobal->nofTitleItems / 2) ; loop++)
        {
            tempIndex = NNshGlobal->msgListIndex[loop];

            NNshGlobal->msgListIndex[loop] =
               NNshGlobal->msgListIndex[NNshGlobal->nofTitleItems - (loop + 1)];

            NNshGlobal->msgListIndex[NNshGlobal->nofTitleItems - (loop + 1)] =
                                                                      tempIndex;
        }
    }

    //////////////////// �J�n/�I�����R�[�h�ԍ��̊i�[ //////////////////////
    if (startRec < endRec)
    {
        // �J�n/�I���̃��R�[�h�ԍ����L�^����
        (NNshGlobal->NNsiParam)->startTitleRec = endRec;
        (NNshGlobal->NNsiParam)->endTitleRec   = startRec;
    }
    else
    {
        // �J�n/�I���̃��R�[�h�ԍ����L�^����
        (NNshGlobal->NNsiParam)->startTitleRec = startRec;
        (NNshGlobal->NNsiParam)->endTitleRec   = endRec;     
    }

#ifdef DEBUG
    ///////////////////////////////////////// DEBUG MESSAGE { /////////////////
    MemSet (debugMsg, sizeof(debugMsg), 0x00);
    StrCopy(debugMsg, "startRec:");
    NUMCATI(debugMsg, (NNshGlobal->NNsiParam)->startTitleRec);
    StrCat (debugMsg, " endRec:");
    NUMCATI(debugMsg, (NNshGlobal->NNsiParam)->endTitleRec);
    NNsh_DebugMessage(ALTID_INFO, debugMsg, " (NNshGlobal->NNsiParam)->titleDispState:", 
                      (NNshGlobal->NNsiParam)->titleDispState);
    ///////////////////////////////////////// } DEBUG MESSAGE /////////////////
#endif  // #ifdef DEBUG


    ////////////////////////  �X���ꗗ�f�[�^���i�[����  //////////////////////
    for (loop = 0; loop < NNshGlobal->nofTitleItems; loop++)
    {
        // �\�����R�[�h���擾
        GetRecordReadOnly_NNsh(dbRef, NNshGlobal->msgListIndex[loop],
                                                    &msgH, (void **) &subjDb);

        // ���b�Z�[�W�����K����I�[�o���Ă�����A���̐F�ɂ��Ă��܂�
        if ((subjDb->maxLoc > NNSH_MESSAGE_LIMIT)&&
            (subjDb->state == NNSH_SUBJSTATUS_ALREADY))
        {
            NNshGlobal->msgTitleStatus[loop] = 
                        ((NNSH_SUBJSTATUS_OVER)|((subjDb->msgAttribute << 8)));
        }
        else
        {
            NNshGlobal->msgTitleStatus[loop] = 
                               ((subjDb->state)|((subjDb->msgAttribute << 8)));
        }

        // �X���i�[�����L�^����
        if (((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 0)||((NNshGlobal->NNsiParam)->listMesNumIsNotReadNum == 2))
        {
            NNshGlobal->msgNumbers[loop] = subjDb->maxLoc;
        }
        else
        {
            // ���ǐ����i�[����
            NNshGlobal->msgNumbers[loop] = subjDb->maxLoc - subjDb->currentLoc;
        }

        // �X���^�C�g����ۑ�����
        copyThreadTitleString(ptr, subjDb->threadTitle,
                              (LIMIT_TITLENAME_DEFAULT + 10),
                              NNshGlobal->msgTitleStatus[loop],
                              NNshGlobal->msgNumbers[loop]);

        // �X���^�C�g���̐擪�|�C���^��ۑ����A���̊i�[�̈�ֈړ�������
        NNshGlobal->msgTitleIndex [loop] = ptr;
        ptr = ptr + StrLen(ptr) + 1;

        // ���R�[�h�̈���J������
        ReleaseRecordReadOnly_NNsh(dbRef, msgH);
    }

RECORD_NOTHING:
    // �f�[�^�x�[�X���N���[�Y����
    CloseDatabase_NNsh(dbRef);


    //////////////////// �Y������X�����P�����Ȃ������ꍇ /////////////////////
    if (NNshGlobal->nofTitleItems == 0)
    {
        // �u�ꗗ�Ȃ��v��ݒ肷��
        StrCopy(buffer, MSG_SUBJECT_DEFAULT);
        (NNshGlobal->NNsiParam)->titleDispState     = NNSH_DISP_NOTHING;
        NNshGlobal->msgListIndex  [0] = 0;
        NNshGlobal->msgTitleIndex [0] = buffer;
        NNshGlobal->msgNumbers    [0] = 0;
        NNshGlobal->msgTitleStatus[0] = 0;
        ret = (NNSH_ERRORCODE_THREADNOTHING);
    }
    return (ret);
}

/*=========================================================================*/
/*   Function :   Update_Thread_List                                       */
/*                                                      �X���ꗗ�\���̍X�V */
/*=========================================================================*/
Boolean Update_Thread_List(UInt16 bbsIdx, UInt16 selItem, UInt16 step)
{
    Err               ret;
    Char             *ptr;
    FormType         *frm;
    RectangleType     dimF;
    UInt16            fontID, nlines, fontHeight, threadStatus;

    // �A�N�e�B�u�ȃt�H�[���̃|�C���^�擾
    frm = FrmGetActiveForm();
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, GADID_MESSAGE_LIST), &dimF);

    // �`��̈�ƃt�H���g�̏��A�\���\�s�����X�V
    fontID = ((NNshGlobal->NNsiParam)->useSonyTinyFontTitle == 0) ?
                           (NNshGlobal->NNsiParam)->currentFont : (NNshGlobal->NNsiParam)->sonyHRFontTitle;

    // ��ʂ̑傫�����擾(�ݒ�)
    NNsi_UpdateRectangle(&dimF, (NNshGlobal->NNsiParam)->useSonyTinyFontTitle,
                         &fontID, &fontHeight, &nlines);


    // "�X���ꗗ�쐬��" �̕\��
    Show_BusyForm(MSG_READ_THREAD_WAIT);

    // �X���^�C�֘A�̈���J������
    ClearMsgTitleInfo();

    // �X���^�C�֘A�������̈���m�ۂ���
    if (AllocMsgTitleInfo(nlines) != errNone)
    {
        // �X���^�C�g���֘A�������̈�̊m�ۂɎ��s
        return (false);
    }

    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    /////       �X���^�C�g���̕�����쐬
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////
    ptr = NNshGlobal->msgListStrings;
    threadStatus = (NNshGlobal->NNsiParam)->titleDispState;
    ret = create_TitleList(bbsIdx, nlines, ptr, step);
    if (ret == NNSH_ERRORCODE_THREADNOTHING)
    {
        // �X���ɕ\��������̂��Ȃ��A�Ƃ��������A�A�A
        if ((step == NNSH_STEP_REDRAW)&&(threadStatus != NNSH_DISP_ALL)&&
            (threadStatus != NNSH_DISP_UPPERLIMIT))
        {
            //  �ĕ`��w���ŁA�X����Ԃ��u�S���\���v�Ɓu����v�łȂ������ꍇ
            // �X���^�C�g��������쐬�֐���������x�Ăяo��
            MemSet(ptr, StrLen(ptr), 0x00);
            selItem = NNSH_ITEM_LASTITEM;
            step    = NNSH_STEP_TO_BOTTOM;
            ret     = create_TitleList(bbsIdx, nlines, ptr, step);
        }
    }

    // �I���X�����w�肵���A�C�e���ԍ��ɐݒ肷��
    // (�\���\�s���𒴂��Ă�����A��ԉ��̃A�C�e���ԍ��ɕύX����)
    (NNshGlobal->NNsiParam)->selectedTitleItem = (selItem >= NNshGlobal->nofTitleItems) ?
                                     (NNshGlobal->nofTitleItems - 1): selItem;

    // "�X���ꗗ�쐬��"�̕\���폜
    Hide_BusyForm(false);

#ifdef USE_PIN_DIA
    if (NNshGlobal->updateHR == NNSH_UPDATE_DISPLAY)
    {
        // CLIE�ŃV���N�̃T�C�Y���ύX���ꂽ�Ƃ��ɂ͈�U�t�H�[��������
        FrmEraseForm(frm);
    }
#endif
    FrmDrawForm(frm);

    // �X���ꗗ�̕\��(�`�������)
    NNsi_WinDrawList((NNshGlobal->NNsiParam)->selectedTitleItem, NNshGlobal->msgTitleIndex,
                     nlines, &dimF, fontHeight);

    return (true);
}

#ifdef USE_COLOR
/*=========================================================================*/
/*   Function :   NNsi_SetTitleColor                                       */
/*                                                    �X���^�C�g���̐F�ݒ� */
/*=========================================================================*/
void NNsi_SetTitleColor(UInt16 status)
{
    IndexedColorType color;

    if (((status >> 8)&(NNSH_MSGATTR_ERROR)) == NNSH_MSGATTR_ERROR)
    {
        // �G���[�X��    
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorError;
        goto SET_COLOR;
    }

    // ���b�Z�[�W�̏�ԂŐF��ݒ�
    switch (status & 0xff)
    {
      case NNSH_SUBJSTATUS_NEW:
        /** �V�K�擾(�Ď擾) **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorNew;
        break;

      case NNSH_SUBJSTATUS_UPDATE:
        /** ���b�Z�[�W�X�V **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorUpdate;
        break;

      case NNSH_SUBJSTATUS_REMAIN:
        /** ���ǂ��� **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorRemain;
        break;

      case NNSH_SUBJSTATUS_ALREADY:
        /** ���b�Z�[�W�S���ǂ� **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorAlready;
        break;

      case NNSH_SUBJSTATUS_NOT_YET:
        /** ���b�Z�[�W���擾 **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorNotYet;
        break;

      case NNSH_SUBJSTATUS_OVER:
        /** 1000�� **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorOver;
        break;

      default:                
        /** ���̑�(���肦�Ȃ��H) **/
        color = (IndexedColorType) (NNshGlobal->NNsiParam)->colorUnknown;
        break;
    }

SET_COLOR:
    // �F��ݒ肷��
    WinSetForeColor(color);
    WinSetTextColor(color);
    return;
}
#endif

/*=========================================================================*/
/*   Function :   NNsi_WinDrawList                                         */
/*                                                    �X���ꗗ�̕\�����C�� */
/*=========================================================================*/
void NNsi_WinDrawList(UInt16 pointIndex, Char *titleIndex[], UInt16 listItems,
                      RectangleType *dimF, UInt16 height)
{
    UInt16        lp, len;
#ifdef USE_COLOR
    IndexedColorType savedIndex;
    RGBColorType     savedColor, drawColor;
#endif

    // �X���ꗗ�ɕ\�����镶���񂪂Ȃ������ꍇ�ɂ́A����������������
    if (NNshGlobal->msgTitleIndex == NULL)
    {
        return;
    }

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        ///////////////////////////////////////////////////////////////
        //  ���̐ݒ�ɂ��A�ꗗ���X�g�̗̈悾���w�i�F���ς��...
        // (�Ȃ񂩂�����Ɗi�D�����Ȃ��B�B�B)
        ///////////////////////////////////////////////////////////////

        // �w�i�F��ݒ肷��
        WinSetBackColor((IndexedColorType) (NNshGlobal->NNsiParam)->colorBackGround);
    }
#endif

    // �\���̈���N���A����
    NNsi_EraseRectangle(dimF);

#ifdef USE_HIGHDENSITY
    // ���𑜓x�̐ݒ�
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFontTitle);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

    // �X���^�C�g���̕`��
    for (lp = 0; lp < listItems; lp++)
    {
        if (NNshGlobal->msgTitleIndex[lp] == '\0')
        {
            // �^�C�g���C���f�b�N�X���Ȃ��A�I������
            break;
        }

#ifdef USE_COLOR
        // �t�H���g�J���[�̐ݒ�
        if ((NNshGlobal->NNsiParam)->useColor != 0)
        {
            // ������Ǝ��s�A�����͔�����...�����Ă�������...(^^;
            NNsi_SetTitleColor(NNshGlobal->msgTitleStatus[lp]);
        }
#endif

#ifdef USE_CLIE
        if (NNshGlobal->hrRef != 0)
        {
            // CLIE�n�C���]�`��
            HRWinDrawChars(NNshGlobal->hrRef, 
                           titleIndex[lp], StrLen(titleIndex[lp]),
                           dimF->topLeft.x, dimF->topLeft.y);
            if (pointIndex == lp)
            {
                // �J�[�\���ʒu�𔽓]������
                HRWinDrawInvertedChars(NNshGlobal->hrRef, 
                                       titleIndex[lp], StrLen(titleIndex[lp]),
                                       dimF->topLeft.x, dimF->topLeft.y);
            }
        }
        else
#endif
        {
            // �ʏ탂�[�h�`��
            len = FntWordWrap(titleIndex[lp], dimF->extent.x);
            WinDrawChars(titleIndex[lp],len,dimF->topLeft.x,dimF->topLeft.y);
            if (pointIndex == lp)
            {
                // �J�[�\���ʒu�𔽓]������
                WinDrawInvertedChars(titleIndex[lp], len,
                                     dimF->topLeft.x, dimF->topLeft.y);
            }
        }

        // ���̍s�ɕ`��ʒu���ړ�������
        dimF->topLeft.y = dimF->topLeft.y + height;
    }

#ifdef USE_HIGHDENSITY
    // ���𑜓x�̐ݒ������
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),
                      NNshGlobal->os5HighDensity);
        WinSetCoordinateSystem(kCoordinatesStandard);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif   // #ifdef USE_HIGHDENSITY

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // ���݂̐F��ۑ�����
        savedIndex = UIColorGetTableEntryIndex(UIObjectForeground);
        WinIndexToRGB(savedIndex, &savedColor);

        // �{�^���̐F��ϊ�����
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorButton,    &drawColor);
        UIColorSetTableEntry(UIObjectForeground, &drawColor);
    }
#endif

    // ���/�����̃{�^����\������
    // (�㉺�{�^����\��/��\���ɂ���)
    NNsh_UpDownButtonControl(FrmGetActiveForm(), (NNshGlobal->NNsiParam)->titleDispState, BTNID_LISTPREV, BTNID_LISTNEXT);

    if ((NNshGlobal->NNsiParam)->searchPickupMode != 0)
    {
        // �i���݌������[�h�̂Ƃ�...
        FrmShowObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), BTNID_SEARCHLIST));        
    }
    else
    {
        // �i���݌������[�h�ł͂Ȃ��Ƃ�...
        FrmHideObject(FrmGetActiveForm(), FrmGetObjectIndex(FrmGetActiveForm(), BTNID_SEARCHLIST));
    }

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // �{�^���̐F�����ɖ߂�
        UIColorSetTableEntry(UIObjectForeground, &savedColor);

        // �f�t�H���g�̔w�i�F��ݒ肷��
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));

        // �����ɂ��āA�f�t�H���g�̕W���F��ݒ肷��
        WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
    }   
#endif

    return;
}


/*=========================================================================*/
/*   Function :   NNsi_WinUpdateList                                       */
/*                                            ���X�g�f�[�^�̔��]�����X�V */
/*=========================================================================*/
void NNsi_WinUpdateList(UInt16 pointIndex, UInt16 prevIndex,
                        Char *titleIndex[], UInt16 listItems,
                        RectangleType *dimF, UInt16 height)
{
    UInt16         lp, len;
#ifdef USE_COLOR
    IndexedColorType savedIndex;
    RGBColorType     savedColor, drawColor;
#endif

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        ///////////////////////////////////////////////////////////////
        //  ���̐ݒ�ɂ��A�ꗗ���X�g�̗̈悾���w�i�F���ς��...
        // (�Ȃ񂩂�����Ɗi�D�����Ȃ��B�B�B)
        ///////////////////////////////////////////////////////////////

        // �w�i�F��ݒ肷��
        WinSetBackColor((IndexedColorType) (NNshGlobal->NNsiParam)->colorBackGround);
    }
#endif

#ifdef USE_HIGHDENSITY
    // ���𑜓x�̐ݒ�
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif

    // �X���^�C�g���̕`��
    for (lp = 0; lp < listItems; lp++)
    {
        if (NNshGlobal->msgTitleIndex[lp] == '\0')
        {
            // �^�C�g���C���f�b�N�X���Ȃ��A�I������
            break;
        }

        if (lp == prevIndex)
        {
#ifdef USE_COLOR
            // �t�H���g�J���[�̐ݒ�
            if ((NNshGlobal->NNsiParam)->useColor != 0)
            {
                // ������Ǝ��s�A�����͔�����...�����Ă�������...(^^;
                NNsi_SetTitleColor(NNshGlobal->msgTitleStatus[lp]);
            }
#endif
#ifdef USE_CLIE
            if (NNshGlobal->hrRef != 0)
            {
                // �J�[�\���ʒu�����ɖ߂�(CLIE�n�C���]�`��)
                HRWinDrawChars(NNshGlobal->hrRef, 
                               titleIndex[lp], StrLen(titleIndex[lp]),
                               dimF->topLeft.x, dimF->topLeft.y);
            }
            else
#endif
            {
                // �J�[�\���ʒu�����ɖ߂�(�ʏ탂�[�h�`��)
                len = FntWordWrap(titleIndex[lp], dimF->extent.x);
                WinDrawChars(titleIndex[lp], len,
                             dimF->topLeft.x, dimF->topLeft.y);
            }
        }

        if (lp == pointIndex)
        {
            // �t�H���g�J���[�̐ݒ�
#ifdef USE_COLOR
            if ((NNshGlobal->NNsiParam)->useColor != 0)
            {
                // ������Ǝ��s�A�����͔�����...�����Ă�������...(^^;
                NNsi_SetTitleColor(NNshGlobal->msgTitleStatus[lp]);
            }
#endif
#ifdef USE_CLIE
            if (NNshGlobal->hrRef != 0)
            {
                // �J�[�\���ʒu�𔽓]������(CLIE�n�C���]�`��)
                HRWinDrawInvertedChars(NNshGlobal->hrRef, 
                                       titleIndex[lp], StrLen(titleIndex[lp]),
                                       dimF->topLeft.x, dimF->topLeft.y);
            }
            else
#endif
            {
                // �J�[�\���ʒu�𔽓]������(�ʏ탂�[�h�`��)
                len = FntWordWrap(titleIndex[lp], dimF->extent.x);
                WinDrawInvertedChars(titleIndex[lp], len,
                                     dimF->topLeft.x, dimF->topLeft.y);
            }
        }
 
        // ���̍s�ɕ`��ʒu���ړ�������
        dimF->topLeft.y = dimF->topLeft.y + height;
    }

#ifdef USE_HIGHDENSITY
    // ���𑜓x�̐ݒ����
    switch (NNshGlobal->os5HighDensity)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),
                      NNshGlobal->os5HighDensity);
        WinSetCoordinateSystem(kCoordinatesStandard);
        // FntSetFont((NNshGlobal->NNsiParam)->sonyHRFont);
        break;

      case kDensityLow:
      default:
        break;
    }
#endif // #ifdef USE_HIGHDENSITY

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // ���݂̐F��ۑ�����
        savedIndex = UIColorGetTableEntryIndex(UIObjectForeground);
        WinIndexToRGB(savedIndex, &savedColor);

        // �{�^���̐F��ϊ�����
        WinIndexToRGB((NNshGlobal->NNsiParam)->colorButton,    &drawColor);
        UIColorSetTableEntry(UIObjectForeground, &drawColor);
    }
#endif

    // ���/�����̃{�^����\������
    // (�㉺�{�^����\��/��\���ɂ���)
    NNsh_UpDownButtonControl(FrmGetActiveForm(), (NNshGlobal->NNsiParam)->titleDispState, BTNID_LISTPREV, BTNID_LISTNEXT);

#ifdef USE_COLOR
    if ((NNshGlobal->NNsiParam)->useColor != 0)
    {
        // �{�^���̐F�����ɖ߂�
        UIColorSetTableEntry(UIObjectForeground, &savedColor);

        // �f�t�H���g�̔w�i�F��ݒ肷��
        WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));

        // �����ɂ��āA�f�t�H���g�̕W���F��ݒ肷��
        WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
        WinSetTextColor(UIColorGetTableEntryIndex(UIFieldText));
    }
#endif
    return;
}
