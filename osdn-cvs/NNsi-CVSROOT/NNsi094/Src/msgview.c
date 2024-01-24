/*============================================================================*
 *  FILE: 
 *     msgview.c
 *
 *  Description: 
 *     Message(Thread) Viewer for NNsh. 
 *
 *===========================================================================*/
#define MSGVIEW_C
#include "local.h"
#include "Clipboard.h"

static Err PartReceiveMessage_View(void);

/*-------------------------------------------------------------------------*/
/*   Function : jumpPrevious                                               */
/*                                                �P�O�̃��b�Z�[�W�֖߂� */
/*-------------------------------------------------------------------------*/
static void jumpPrevious(void)
{
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;
 
    // 1��ł��W�����v���Ă���Ζ߂�
    if (idxP->prevMsg[idxP->histIdx] != 0)
    {
        idxP->openMsg = idxP->prevMsg[idxP->histIdx];
        idxP->histIdx = (idxP->histIdx != 0) ?
                                       (idxP->histIdx - 1) : (PREVBUF - 1);
    }
    (void) displayMessage(NNshGlobal->msgIndex);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : jumpEdge                                                   */
/*                                             ���b�Z�[�W�̈ړ�(�擪/����) */
/*-------------------------------------------------------------------------*/
static void jumpEdge(Boolean loc)
{
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;

    idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
    idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
    if (loc == false)
    {
        // �X���̐擪��
        idxP->openMsg    = 1;
    }
    else
    {
        // �X���̖�����
        idxP->openMsg    = idxP->nofMsg;
    }
    (void) displayMessage(NNshGlobal->msgIndex);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : parseMessage                                               */
/*                                                  (���b�Z�[�W�̐��`����) */
/*-------------------------------------------------------------------------*/
static void parseMessage(Char *buf, Char *source, UInt32 size,
                         UInt16 *nofJmp, UInt16 *jmpBuf)
{
    UInt32 dataStatus, jmp;
    Char *ptr, *dst, *num;

    dataStatus = MSGSTATUS_NAME;

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    dst  = buf;
    ptr  = source;
    while (ptr < (source + size))
    {
        if (*ptr == '&')
	{
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&
		(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                if ((*ptr <= '9')&&(*ptr >= '0'))
                {
                    // �X���Ԃ��擾
                    num = ptr;
                    jmp = 0;
                    while ((*num <= '9')&&(*num >= '0'))
                    {
                        jmp = jmp * 10 + (*num - '0');
                        num++;
                    }
                    if ((*nofJmp < JUMPBUF)&&(jmp != 0))
                    {
                        // �X���Ԃ��o�b�t�@�ɒǉ�
                        jmpBuf[*nofJmp] = jmp;
                        (*nofJmp)++;
                    }
                }
                continue;
            }
            // "&lt;" �� '<' �ɒu��
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&
		(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" �� '"' �ɒu��
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
		(*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&
		(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" �� '    ' �ɒu��
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
		(*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&
		(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" �� '&' �ɒu��
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
		(*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                // �M�R�̂����ەϊ��΍�... (��������)
                if ((*(ptr + 5) == 'g')&&(*(ptr + 6) == 't')&&
                    (*(ptr + 7) == ';'))
                {
                    *dst++ = '>';
                    ptr = ptr + 8;    // StrLen(TAG_AMP)+ 'g' + 't' + ';';
                    if ((*ptr <= '9')&&(*ptr >= '0'))
                    {
                        // �X���Ԃ��擾
                        num = ptr;
                        jmp = 0;
                        while ((jmp < NNSH_MESSAGE_LIMIT)&&
                               (*num <= '9')&&(*num >= '0'))
                        {
                            jmp = jmp * 10 + (*num - '0');
                            num++;
                        }
                        if ((*nofJmp < JUMPBUF)&&(jmp != 0))
                        {
                            // �X���Ԃ��o�b�t�@�ɒǉ�
                            jmpBuf[*nofJmp] = jmp;
                            (*nofJmp)++;
                        }
                    }
                    continue;
                }
                // �M�R�̂����ەϊ��΍�... (�����܂�)

                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }
            // ����͂��肦�Ȃ��͂�����...�ꉞ�B
            *dst++ = '&';
            ptr++;
            continue;
        }
        if (*ptr == '<')
	{
            //  "<>" �́A�Z�p���[�^(��Ԃɂ���ĕς��)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                switch (dataStatus)
                {
                  case MSGSTATUS_NAME:
                    // ���O���̋�؂�
                    *dst++ = ' ';
                    *dst++ = ' ';
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_EMAIL;
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail���̋�؂�
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_DATE;
                    *dst++ = '\n';
                    break;

                  case MSGSTATUS_DATE:
                    // �N�����E���Ԃ���тh�c���̋�؂�
                    *dst++ = '\n';
                    *dst++ = '\n';
                    dataStatus = MSGSTATUS_NORMAL;
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // ���b�Z�[�W�̏I�[
                    *dst++ = ' ';
                    *dst++ = '\0';
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" �́A���s�ɒu��
            if ((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&
		(*(ptr + 3) == '>'))
            {
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }
            // �^�O�͓ǂݔ�΂�
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            *dst++ = ' ';
            continue;
        }

        // �I�[�����𖳎�����悤�ɕύX����
        if (*ptr != '\0')
        {
            // ���ʂ̈ꕶ���]��
            *dst++ = *ptr;
        }
	ptr++;
    }
    *dst++ = '\0';
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : changeFont                                                 */
/*                                                          �t�H���g�̕ύX */
/*-------------------------------------------------------------------------*/
static void changeFont(void)
{
    FormType  *frm;

    frm = FrmGetActiveForm();

    // �t�H���gID�̑I��
    if (NNshParam->useSonyTinyFont != 0)
    {
        // SONY�n�C���]�t�H���g��ύX����
        NNshParam->sonyHRFont = FontSelect(NNshParam->sonyHRFont);

        // �t�B�[���h�̕`��(�n�C���]���)
        NNsi_HRFldDrawField(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, FLDID_MESSAGE)));
        // �X�N���[���o�[�̍X�V
        NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
    }
    else
    {
        // �ʏ�t�H���g��ύX����
        NNshParam->currentFont = FontSelect(NNshParam->currentFont);

        // �t�B�[���h�̕`��(�ʏ���)
        FldSetFont(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE)),
                                   NNshParam->currentFont);

        // �X�N���[���o�[���X�V���āA�ʏ�̉�ʕ`����s��
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);
        FrmDrawForm(frm);
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : ReadMessage_Close                                          */
/*                                                      �Q�Ɖ�ʂ̃N���[�Y */
/*-------------------------------------------------------------------------*/
static void ReadMessage_Close(UInt16 nextFormID)
{
    NNshMessageIndex *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;
    if (idxP != NULL)
    {
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }

    // �n�C���]�p�f�[�^�����l���N���A����
    NNshGlobal->totalLine   = 0;
    NNshGlobal->nofPage     = 0;
    NNshGlobal->currentPage = 0;
    NNshGlobal->prevHRLines = 0;

    // �X���I����ʂ��J��
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(nextFormID);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessageSub                                          */
/*                                                   ���b�Z�[�W�\�����C��  */
/*-------------------------------------------------------------------------*/
static void displayMessageSub(UInt16 bbsType, Char *topP, UInt32 mesNum,
                              UInt32 offset, UInt32 size)
{
    FormType    *frm;
    MemHandle    oldTxtH, txtH;
    Char        *buf, *ptr;
    UInt32       msgSize;
    void        *fldP;
#ifdef USE_ARMLET
    UInt32       armArgs[7], nofJump, lp, jmpMsg[JUMPBUF];
    MemHandle    armH;
    void        *armP;
#endif
    NNshMessageIndex *idxP;
    idxP = NNshGlobal->msgIndex;

    // �\���o�b�t�@���쐬����(MARGIN * 3�̕����ŃX���^�C�g���������J�o�[)
    txtH = MemHandleNew(size + (MARGIN * 3));
    if (txtH == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_SUBJECT_LISTFAIL, " size:", size);
        return;
    }
    buf  = MemHandleLock(txtH);
    if (buf == NULL)
    {
        MemHandleFree(txtH);
        NNsh_InformMessage(ALTID_WARN, "MemHandleLock()", " ", 0);
        return;
    }

    // ���b�Z�[�W�̐��`����
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        ptr = StrStr((topP + offset), NNSH_MACHITOKEN_MSGSTART);
        if (ptr == NULL)
        {
            ptr = topP + offset;
        }
        else
        {
            ptr = ptr + StrLen(NNSH_MACHITOKEN_MSGSTART);
        }
        // ���b�Z�[�W�T�C�Y�̌������킹...
        msgSize = size - (ptr - (topP + offset)) - 2;
    }
    else
    {
        ptr     = (topP + offset);
        msgSize = size;
    }

    // �\���̈�����������A�擪�Ƀ��b�Z�[�W�ԍ���\��
    MemSet(buf, msgSize, 0x00);
    if (NNshParam->boldMessageNum != 0)
    {
        StrCopy(buf, "�y");
        NUMCATI(buf, mesNum);
        StrCat (buf, "�z");
    }
    else
    {
        StrCopy(buf, "[");
        NUMCATI(buf, mesNum);
        StrCat (buf, "] ");
    }

    // �W�����v�Q�Ɛ���������
    idxP->nofJump = 0;

// ARMlet���g�p���ă��b�Z�[�W�̉�͂��s���B(�ł��A�Ȃ񂩖��ʂȏ�����...)
#ifdef USE_ARMLET
    // ARMlet�̊֐��|�C���^���擾����
    if (NNshParam->useARMlet != 0)
    {
        armH = DmGetResource('armc', RSCID_parseMessage_ARM);
        if (armH != 0)
        {
            armP = MemHandleLock(armH);
            if (armP != NULL)
            {
                // ARMlet���g�p���ẴC���f�b�N�X��͏���
                MemSet(armArgs, sizeof(armArgs), 0x00);
                armArgs[0] = (UInt32) (buf + StrLen(buf));
                armArgs[1] = (UInt32) ptr;
                armArgs[2] = (UInt32) msgSize;
                armArgs[3] = (UInt32) &(nofJump);
                armArgs[4] = (UInt32) &(jmpMsg[0]);
                (void) PceNativeCall(armP, armArgs);
                MemHandleUnlock(armH);
                idxP->nofJump = (UInt16) nofJump;

                // ����ȏ����A��������(68k��)����Ă��X�|�C�������Ȃ�...
                for (lp = 0; lp < idxP->nofJump; lp++)
                {
                    idxP->jumpMsg[lp] = (UInt16) jmpMsg[lp];
                }
            }
            else
            {
                // (�W����)�C���f�b�N�X��͏���
                parseMessage((buf + StrLen(buf)), ptr, msgSize,
                             &(idxP->nofJump), &(idxP->jumpMsg[0]));
            }
            DmReleaseResource(armH);
        }
        else
        {
            // (�W����)�C���f�b�N�X��͏���
            parseMessage((buf + StrLen(buf)), ptr, msgSize,
                         &(idxP->nofJump), &(idxP->jumpMsg[0]));
        }
    }
    else
#endif
    {
        // (�W����)�C���f�b�N�X��͏���
        parseMessage((buf + StrLen(buf)), ptr, msgSize,
                     &(idxP->nofJump), &(idxP->jumpMsg[0]));
    }
    (void) MemHandleUnlock(txtH);

    // �t�B�[���h�̍X�V
    frm     = FrmGetActiveForm();
    fldP    = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    oldTxtH = FldGetTextHandle(fldP);
    FldSetTextHandle(fldP, NULL);
    FldSetTextHandle(fldP, txtH);
    if (oldTxtH != 0)
    {
        (void) MemHandleFree(oldTxtH);
    }

    // �t�B�[���h�̕\��
    if (NNshParam->useSonyTinyFont != 0)
    {
        // �t�B�[���h�̕\���n�ϐ����N���A����
        NNshGlobal->totalLine   = 0;
        NNshGlobal->nofPage     = 0;
        NNshGlobal->currentPage = 0;
        NNshGlobal->prevHRLines = 0;

        // �t�B�[���h�̕`��(�n�C���]���)
        NNsi_HRFldDrawField(fldP);

        // �X�N���[���o�[�̍X�V
        NNsi_HRSclUpdate(FrmGetObjectPtr(frm, 
                                     FrmGetObjectIndex(frm, SCLID_MESSAGE)));
    }
    else
    {
        // �t�B�[���h�̕`��(�ʏ���)
        FldDrawField(fldP);

        // �X�N���[���o�[�̕\���X�V
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

        FrmDrawForm(frm);
    }

#if 0
    // �W�����v�Q�Ɛ��������炠�邩��\��������(���̃f�o�b�O�\��)
    if (idxP->nofJump != 0)
    {
        if ((buf = MemPtrNew(BUFSIZE)) != NULL)
        {
            MemSet(buf, BUFSIZE, 0x00);
            StrCopy(buf, "nofJump:");
            NUMCATI(buf, idxP->nofJump);
            StrCat (buf, "\n----\n");
            for (msgSize = 0; msgSize < idxP->nofJump; msgSize++)
            {
                StrCat(buf, "  >>");
                NUMCATI(buf, idxP->jumpMsg[msgSize]);
            }
            StrCat (buf, "\n");
            NNsh_DebugMessage(ALTID_INFO, buf, "", 0);
            MemPtrFree(buf);
        }
    }
#endif

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : displayMessage                                             */
/*                                                   ���b�Z�[�W��\������  */
/*-------------------------------------------------------------------------*/
static Err displayMessage(NNshMessageIndex *idxP)
{
    Err          ret = errNone;
    UInt16       msg;
    UInt32       readSize, offset, setMsg, backMsg;

    msg    = idxP->openMsg;
    offset = 0;

    //  �w�肳�ꂽ���b�Z�[�W�ԍ����Ó����`�F�b�N����B
    if ((msg < 1)||(msg > idxP->nofMsg))
    {
        return (~errNone);
    }

    // �f�[�^�͍��ǂݍ��܂�Ă���̈�ɂ��邩�`�F�b�N����B(�������킹...)
    if ((idxP->msgOffset[msg - 1]  < idxP->fileOffset)||
        (idxP->msgOffset[msg] - 1  > idxP->fileOffset + idxP->fileReadSize))
    {
        // ���݁A�ǂݍ��܂�Ă���o�b�t�@�ɂ̓f�[�^���Ȃ��ꍇ�B�B�B
        if (idxP->msgOffset[msg] < NNshParam->bufferSize)
        {
            // �t�@�C���̐擪����ǂݍ���
            offset = 0;
	}
        else if (idxP->msgOffset[msg - 1] >
                                     (idxP->fileSize - NNshParam->bufferSize))
        {
            // �t�@�C���̖�����ǂݍ���
            offset = idxP->fileSize - NNshParam->bufferSize;
	}
        else
        {
            // �߂郁�b�Z�[�W�������肷��B
            backMsg = NNshParam->bufferSize / 1200;

            // (�C���f�b�N�X�̐擪 - backMsg)����ǂݍ���
            setMsg = ((msg - backMsg) < 1) ? 1: (msg - backMsg);
	    offset = idxP->msgOffset[setMsg] - 10;  // 10�̓}�[�W��(�Ӗ��Ȃ�)
            if ((offset + NNshParam->bufferSize) < idxP->msgOffset[msg + 1])
            {
                // backMsg���b�Z�[�W�O���ǂݍ��ݔ͈͊O�������ꍇ�͂��̏ꏊ����
                offset = idxP->msgOffset[msg] - 10; // 10�̓}�[�W��(�Ӗ��Ȃ�)
            }
        }

        // �u�҂āv�\�� (hideBusyForm()�܂�)
        Show_BusyForm(MSG_READ_MESSAGE_WAIT);

        // �f�[�^���o�b�t�@�ɓǂݍ��ށB
        ret = ReadFile_NNsh(&(idxP->fileRef), offset, NNshParam->bufferSize,
                            &(idxP->buffer[0]), &readSize);
        if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
        {
            idxP->fileOffset   = offset;
            idxP->fileReadSize = readSize;
        }
        Hide_BusyForm();
    }
    if ((ret == errNone)||(ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        // ���b�Z�[�W��\������B
        offset = (idxP->msgOffset[msg - 1] - (idxP->fileOffset));

        displayMessageSub(idxP->bbsType, idxP->buffer, msg, offset,
                          idxP->msgOffset[msg] - idxP->msgOffset[msg - 1]);
    }

    // �ǂ񂾃��b�Z�[�W�ԍ�(�Ƃ��C�ɓ������)�L������
    MarkMessageIndex(NNshParam->openMsgIndex, NNSH_MSGATTR_NOTSPECIFY);

    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessageNext                                            */
/*                                           �ЂƂ��̃��b�Z�[�W��\��  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessageNext(Boolean chkScroll)
{
    Boolean           ret = false;
    NNshMessageIndex *idxP;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    // ���b�Z�[�W�̃X�N���[���`�F�b�N
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winDown); 
    }
    if ((ret != true)&&(idxP->openMsg < idxP->nofMsg))
    {
         // ���b�Z�[�W�ԍ����ЂƂ��₵�A��ʕ\���B
         (idxP->openMsg)++;
         (void) displayMessage(NNshGlobal->msgIndex);
         ret = true;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : moveMessagePrev                                            */
/*                                             �ЂƂO�̃��b�Z�[�W��\��  */
/*-------------------------------------------------------------------------*/
static Boolean moveMessagePrev(Boolean chkScroll)
{
    Boolean ret = false;
    NNshMessageIndex *idxP;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    // ���b�Z�[�W�̃X�N���[���`�F�b�N
    if (chkScroll == true)
    {
        ret = NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0,  winUp);
    }
    if ((ret != true)&&(idxP->openMsg > 1))
    {
         // ���b�Z�[�W�ԍ����ЂƂ��炵�A��ʕ\���B
         (idxP->openMsg)--;
         (void) displayMessage(NNshGlobal->msgIndex);
         ret = true;
    }
    return (ret);
}
/*-------------------------------------------------------------------------*/
/*   Function : jumpMessage                                                */
/*                                                (���b�Z�[�W�W�����v����) */
/*-------------------------------------------------------------------------*/
static Boolean jumpMessage(FormType *frm, NNshMessageIndex *idxP)
{
    UInt16     tempMsg, num, btnId;
    FieldType *fldP;
    Char      *numP;
    MemHandle  txtH;

    // ���͂��ꂽ���l���擾����
    tempMsg = idxP->openMsg;
    fldP    = FrmGetObjectPtr(frm, 
                              FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));
    txtH = FldGetTextHandle(fldP);
    if (txtH != NULL)
    {
        numP = MemHandleLock(txtH);
        num = StrAToI(numP);

        // ���l���Ó��Ȃ��̂��`�F�b�N
        if ((num > 0)&&(num <= idxP->nofMsg))
        {
            tempMsg = num;
        }
        MemHandleUnlock(txtH);
    }

    // �������������t�B�[���h�ɓ��͂���Ă��Ȃ������ꍇ
    if (tempMsg == idxP->openMsg)
    {
        //  Go�{�^���������ꂽ�Ƃ��A���������͂���Ă��Ȃ������ꍇ�A
        //  ���߂����X���ԍ������X�g�I�u�W�F�N�g�Ƃ��ĕ\������B
        idxP->jumpListTitles = 
                            MemPtrNew((idxP->nofJump + 2) *  NNSH_JUMPMSG_LEN);
        if (idxP != NULL)
        {
            // ���X�g�ɕ\������^�C�g�����X�g���쐬����
            MemSet(idxP->jumpListTitles, 0x00,
                                    ((idxP->nofJump + 2) *  NNSH_JUMPMSG_LEN));
            numP = idxP->jumpListTitles;
            if (NNshParam->addReturnToList != 0)
            {
                // ���X�g�̐擪�Ɂu�ꗗ�֖߂�v��ǉ�
                StrCopy(numP, NNSH_JUMPMSG_TO_LIST);
                numP = numP + StrLen(numP) + 1;
            }

            // ���X�g�Ɂu�P�߂�v��ǉ�
            StrCopy(numP, NNSH_JUMPMSG_BACK);
            numP = numP + StrLen(numP) + 1;

            for (num = 0; num < idxP->nofJump; num++)
            {
                StrCopy(numP, NNSH_JUMPMSG_HEAD);
                NUMCATI(numP, idxP->jumpMsg[num]);
                numP = numP + StrLen(numP) + 1;
            }
            num = idxP->nofJump + 1;
            if (NNshParam->addReturnToList != 0)
            {
                num++;
            }

            // �I���E�B���h�E��\������
            btnId = NNshWinSelectionWindow(idxP->jumpListTitles, num);

            if (btnId != BTNID_JUMPCANCEL)
            {
                // �X���ꗗ�ɖ߂邪�I�����ꂽ�Ƃ�
                if ((NNshParam->addReturnToList != 0)&&
                    (NNshGlobal->jumpSelection == NNSH_JUMPSEL_TO_LIST))
                {
                    // �X���I����ʂɖ߂�
                    ReadMessage_Close(FRMID_THREAD);
                    return (false);
                }
                if (((NNshParam->addReturnToList != 0)&&
                    (NNshGlobal->jumpSelection == NNSH_JUMPSEL_BACK_USELIST))||
                    ((NNshParam->addReturnToList == 0)&&
                    (NNshGlobal->jumpSelection == NNSH_JUMPSEL_BACK)))
                {
                    // �P�O�ɔ�񂾃��b�Z�[�W�ɖ߂�
                    jumpPrevious();
                    return (false);
                }

                // ���X�g����W�����v���b�Z�[�W�̏ꏊ�����߂�
                (NNshGlobal->jumpSelection)--;
                if (NNshParam->addReturnToList != 0)
                {
                    (NNshGlobal->jumpSelection)--;
                }
                tempMsg = idxP->jumpMsg[NNshGlobal->jumpSelection];
            }
            else
            {
                // ���X�g�I���L�����Z��(�������Ȃ�)
                tempMsg = idxP->openMsg;
            }
            MemPtrFree(idxP->jumpListTitles);
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "Jump LIST Create Failure.", "", 0);
        }
    }

    // �����t�B�[���h���N���A����
    NNshWinSetFieldText(frm, FLDID_MESSAGE_NUMBER, "", 0); 

    // ���b�Z�[�W���ړ����ăW�����v�A���b�Z�[�W��\������
    if (tempMsg != idxP->openMsg)
    {
        idxP->histIdx = 
                  (idxP->histIdx < (PREVBUF - 1)) ? (idxP->histIdx + 1) : 0;
        idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
        idxP->openMsg = tempMsg;

        // ���b�Z�[�W�\��
        (void) displayMessage(idxP);
        return (true);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectScrollEvent                                          */
/*                                       �X�N���[���o�[�X�V�C�x���g�̏���  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_DispMessage(EventType *event)
{
    Int16   lines;

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
      (void)
           NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winUp);
    }
    else
    {
      (void)
           NNshWinViewPageScroll(FLDID_MESSAGE, SCLID_MESSAGE, 0, winDown);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 �L�[�������ꂽ���̏���  */
/*                                (�W���O�A�V�X�g�n�e�e���̃W���O�����ǉ�) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_DispMessage(EventType *event)
{
    Boolean           ret = false;
    FormType         *frm;
    FieldType        *fld;
    NNshMessageIndex *idxP;
    UInt16            keyCode;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    // �C���f�b�N�X���Ȃ��ꍇ�ɂ́A������return����
    if (NNshGlobal->msgIndex == NULL)
    {
        return (false);
    }

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // �u���v(�W���O�_�C������)�����������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
        (void) moveMessageNext(true);
        ret = true;
        break;

      case chrCapital_N:  // N�L�[�̓���
      case chrSmall_N:
        // �ЂƂ��̃��b�Z�[�W�Ɉړ����� 
        (void) moveMessageNext(false);
        ret = true;
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
        (void) moveMessagePrev(true);
        ret = true;
        break;

      case chrCapital_P:  // P�L�[�̓���
      case chrSmall_P:
        // �ЂƂO�̃��b�Z�[�W�Ɉړ����� 
        (void) moveMessagePrev(false);
        ret = true;
        break;

      // �W���O�_�C����������(��)�̏���
      case vchrJogPushedUp:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGUP;
#endif
        /** not break!!  **/

      // �J�[�\���L�[(��)�������̏���
      case chrRecordSeparator:
      case chrCapital_R:  // R�L�[�̓���
      case chrSmall_R:
        ret = true;
        // �����񂵂łP�ÂÏグ��H
        if (NNshParam->notCursor != 0)
        {
            // ���̃��b�Z�[�W��\�����ďI��
            (void) moveMessagePrev(false);
            break;
        }
        if (idxP->openMsg > 1)
        {
            // ���b�Z�[�W�ԍ���10���炵�A��ʕ\���B
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg = (idxP->openMsg > 11) ? (idxP->openMsg - 10) : 1;
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        break;

      // �W���O�_�C����������(��)�̏���
      case vchrJogPushedDown:
        // �ꎞ��Ԃ�(��]����)�ݒ肷��
#ifdef USE_CLIE
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_PUSHEDJOGDOWN;
#endif
        /** not break!!  **/

      // �J�[�\���L�[(��)�������̏���
      case chrUnitSeparator:
      case chrCapital_F:  // F�L�[�̓���
      case chrSmall_F:
        ret = true;
        // �����񂵂łP�ÂÉ�����H
        if (NNshParam->notCursor != 0)
        {
            // ���̃��b�Z�[�W��\�����ďI��
            (void) moveMessageNext(false);
            break;
        }

        if (idxP->openMsg < idxP->nofMsg)
        {
            // ���b�Z�[�W�ԍ���10���₵�A��ʕ\���B
            idxP->histIdx = (idxP->histIdx < (PREVBUF - 1)) ?
                                                 (idxP->histIdx + 1) : 0;
            idxP->prevMsg[idxP->histIdx] = idxP->openMsg;
            idxP->openMsg    = (idxP->openMsg < idxP->nofMsg - 10) ?
                                          (idxP->openMsg + 10) : idxP->nofMsg;
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        break;

      // JOG Push���̏���(��萳�m�ɂ̓{�^���𗣂����Ƃ�������)
      case vchrJogRelease:
#ifdef USE_CLIE
        ret = true;
        if (NNshGlobal->tempStatus == NNSH_TEMPTYPE_CLEARSTATE)
        {
            // �X�������擾
            (void) PartReceiveMessage_View();
        }
        // �ꎞ��Ԃ��N���A����
        NNshGlobal->tempStatus = NNSH_TEMPTYPE_CLEARSTATE;
#endif
        break;

      // Sony �o�b�N�{�^��/HandEra �o�b�N�{�^��/ESC�L�[/X�L�[�̏���
      case vchrJogBack:
      case chrEscape:
      case chrCapital_X:  // X�L�[�̓���
      case chrSmall_X:
        if (NNshParam->jogBackBtnAsGo != 0)
        {
            // Go�{�^���Ɠ����ȏ����ɂ���
            jumpMessage(FrmGetActiveForm(), NNshGlobal->msgIndex);
            return (true);
        }
        // �X���I����ʂɖ߂�
        ReadMessage_Close(FRMID_THREAD);
        return(true);
        break;

      //  HandEra Jog-Push/Enter�L�[���������Ƃ��̏���
      case chrCarriageReturn:
      case chrLineFeed:
        // (�����擾)
        PartReceiveMessage_View();
        return (true);
        break;

      // ��O�̃W�����v�����X�ɖ߂�(Back�{�^���Ɠ���)
      case chrCapital_Z:  // Z�L�[�̓���
      case chrSmall_Z:
        jumpPrevious();
        return (true);
        break;

      // L�L�[����
      case chrCapital_L:
      case chrSmall_L:
        ret = true;
        // �t�H���g�ύX
        changeFont();
        break;

      // D�L�[����
      case chrCapital_D:
      case chrSmall_D:
        if (NNshParam->useSonyTinyFont == 0)
        {
            // �n�C���]���[�h�֕ύX
            NNshParam->useSonyTinyFont = 1;

            // ��ʂ�`�悷��
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        else
        {
            // �ʏ탂�[�h�֕ύX
            NNshParam->useSonyTinyFont = 0;
            frm  = FrmGetActiveForm();

            // �t�B�[���h�̈���g�p�ł���悤�ɕύX����
            fld = FrmGetObjectPtr(frm,FrmGetObjectIndex(frm, FLDID_MESSAGE));
            FldSetUsable(fld, true);

            // �X�N���[���o�[�̍X�V
            NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

            // �t�H�[���������ĕ\�����Ȃ���
            FrmEraseForm(frm);
            FrmDrawForm(frm);
        }
        ret = true;
        break;

      // Q�L�[����
      case chrCapital_Q:
      case chrSmall_Q:
        ret = true;
        // ����ؒf
        NNshNet_LineHangup();
        break;

        // B�L�[����
      case chrCapital_B:
      case chrSmall_B:
        ret = true;
        // �X���̖�����
        jumpEdge(true);
        break;

        // T�L�[����
      case chrCapital_T:
      case chrSmall_T:
        // �X���̐擪��
        ret = true;
        jumpEdge(false);
        break;

        // �X�y�[�X�L�[�EG�L�[�̓���(���b�Z�[�WJUMP)
      case chrSpace:
      case chrCapital_G:
      case chrSmall_G:
        ret = true;
        jumpMessage(FrmGetActiveForm(), NNshGlobal->msgIndex);
        break;

      case chrCapital_W:  // W�L�[�̓���
      case chrSmall_W:
        // �X���ɏ�������(OFFLINE�X���ɂ͏������݂����Ȃ�)
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
            ret = true;
        }
        break;

      default:
        // �L�[�R�[�h��\������
        NNsh_DebugMessage(ALTID_INFO, "Pushed a key  code:", "",
                          event->data.keyDown.chr);
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function :   ctlSelEvt_DispMessage                                    */
/*                                                      �{�^���������̏��� */
/*-------------------------------------------------------------------------*/
Boolean ctlSelEvt_DispMessage(EventType *event)
{
    UInt16       num;
    FormType    *frm;
    ControlType *fvP;
    NNshMessageIndex *idxP;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    // �C���f�b�N�X���Ȃ��ꍇ�ɂ́A������return����
    if (idxP == NULL)
    {
        return (false);
    }

    // ���݃A�N�e�B�u�ȃt�H�[�����擾����
    frm = FrmGetActiveForm();

    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_TOP:
        // �X���̐擪��
        jumpEdge(false);
        break;

        // �O�{�^�� 
      case BTNID_PREV_MESSAGE:
        moveMessagePrev(false);
        break;

        // ���{�^�� 
      case BTNID_NEXT_MESSAGE:
        moveMessageNext(false);
        break;

        // �����{�^��(�X���̖�����)
      case BTNID_BOTTOM:
        jumpEdge(true);
        break;

        // �߂�{�^�� 
      case BTNID_BACK_MESSAGE:
        jumpPrevious();
        break;

        // GO�{�^��
      case BTNID_JUMP:
        jumpMessage(frm, NNshGlobal->msgIndex);
        break;

        // �����{�^��
      case BTNID_WRITE_MESSAGE:
        // �X���ɏ�������(OFFLINE�X���ɂ͏������݂����Ȃ�)
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
        }
        break;

        // ������{�^��
      case BTNID_BOOKMARK:
        if (NNshParam->useBookmark != 0)
        {
            NNshParam->lastFrmID     = FRMID_MESSAGE;
            NNshParam->bookMsgIndex  = NNshParam->openMsgIndex;
            NNshParam->bookMsgNumber = idxP->openMsg;
            NNsh_InformMessage(ALTID_INFO, MSG_SET_BOOKMARK, "", 0);
        }
        break;

      case SELTRID_THREAD_TITLE:  
        // �^�C�g���{�^��(�ꗗ��ʂɖ߂�)
        ReadMessage_Close(FRMID_THREAD);
        break;

      case CHKID_FAVOR_THREAD:
        // ���C�ɓ���̐ݒ�����擾���A�L��
        fvP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, CHKID_FAVOR_THREAD));
        num = CtlGetValue(fvP);
        MarkMessageIndex(NNshParam->openMsgIndex, num);
        if (num == 0)
        {
            // "���C�ɓ���ݒ�"�����������|�\������
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_NOTFAVORITE, "", 0);
        }
        else
        {
            // "���C�ɓ���ݒ�"�����|�\������
            NNsh_InformMessage(ALTID_INFO, MSG_CHANGE_FAVORITE, "", 0);
        }
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   MenuEvt_DispMessage                                      */
/*                                                    ���j���[�I�����̏��� */
/*-------------------------------------------------------------------------*/
Boolean menuEvt_DispMessage(EventType *event)
{
    Char       logMsg[BUFSIZE * 2];
    FormType  *frm;
    FieldType *fld;
    Char      *urlP, *ptr, *dest;
    MemHandle  memH;
    UInt16     len;
#ifdef USE_HANDERA
    VgaScreenModeType   handEraScreenMode;     // �X�N���[�����[�h
    VgaRotateModeType   handEraRotateMode;     // ��]���[�h
#endif
    NNshMessageIndex *idxP;
    UInt16            frmId;

    // index��ϐ��ɓ����
    idxP = NNshGlobal->msgIndex;

    switch (event->data.menu.itemID)
    {
      case MNUID_THREAD_SELECTION:
        // �X���I����ʂɖ߂�
        ReadMessage_Close(FRMID_THREAD);
        break;

      case MNUID_THREAD_INFO:
        // �X������\������B 
        MemSet (logMsg, sizeof(logMsg), 0x00);
        StrCopy(logMsg, "TITLE: ");
        StrCat (logMsg, idxP->threadTitle);
        StrCat (logMsg, "\n\nfileSize: ");
        NUMCATI(logMsg, idxP->fileSize);
        StrCat (logMsg, "\ncurrent/Max\n");
        NUMCATI(logMsg, idxP->openMsg);
        StrCat (logMsg, "/");
        NUMCATI(logMsg, idxP->nofMsg);
        StrCat (logMsg, "\nfOffset:");
        NUMCATI(logMsg, idxP->fileOffset);
        StrCat (logMsg, " fRSize:");
        NUMCATI(logMsg, idxP->fileReadSize);
        NNsh_ErrorMessage(ALTID_INFO, "", logMsg, 0);
        break;

      case MNUID_TOP_MESSAGE:
        // �X���̐擪�ɃW�����v
        jumpEdge(false);
        break;

      case MNUID_BOTTOM_MESSAGE:
        // �X���̖����ɃW�����v
        jumpEdge(true);
        break;

      case MNUID_WRITE_MESSAGE:
        // �X���ɏ�������(OFFLINE�X���ɂ͏������݂����Ȃ�)
        if (StrCompare(idxP->boardNick, OFFLINE_THREAD_NICK) != 0)
        {
            ReadMessage_Close(FRMID_WRITE_MESSAGE);
        }
        break;

      case MNUID_GET_PART:
        // �X�������擾
        (void) PartReceiveMessage_View();
        break;

      case MNUID_NET_DISCONN:
        // ����ؒf
        NNshNet_LineHangup();
        break;

      case MNUID_FONT_MSGVIEW:
        // �t�H���g�ύX
        changeFont();
        break;

#ifdef USE_HANDERA
      case MNUID_HANDERA_ROTATE_MSGVIEW:
        // HandEra ��ʉ�] //
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            VgaGetScreenMode(&handEraScreenMode, &handEraRotateMode);
            VgaSetScreenMode(screenMode1To1, 
                             VgaRotateSelect(handEraRotateMode));

            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
	}
        else
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
        }
        break;
#endif // #ifdef USE_HANDERA

      case MNUID_CLIE_DRAW_MODE: 
        if (NNshParam->useSonyTinyFont == 0)
        {
            // �n�C���]���[�h�֕ύX
            NNshParam->useSonyTinyFont = 1;

            // ��ʂ�`�悷��
            (void) displayMessage(NNshGlobal->msgIndex);
        }
        else
        {
            // �ʏ탂�[�h�֕ύX
            NNshParam->useSonyTinyFont = 0;
            frm  = FrmGetActiveForm();

            // �t�B�[���h�̈���g�p�ł���悤�ɕύX����
            fld = FrmGetObjectPtr(frm, 
                                  FrmGetObjectIndex(frm, FLDID_MESSAGE));
            FldSetUsable(fld, true);

            // �X�N���[���o�[�̍X�V
            NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);

            // �t�H�[���������ĕ\�����Ȃ���
            FrmEraseForm(frm);
            FrmDrawForm(frm);
        }
        break;

      case MNUID_OS5_LAUNCH_WEB:
        if (NNshGlobal->browserCreator == 0)
        {
            // "���݃T�|�[�g���Ă��܂���" �\�����s��
            NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
            break;
        }

        // �I�𕶎�����g���āANetFront�ŊJ��
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId == noFocus)
        {        
           // �t�H�[�J�X���擾�ł��Ȃ��A�I������
            break;
        }
        if (FrmGetObjectType(frm, frmId) == frmFieldObj)
        {
            // �I�����Ă��镶�������U�N���b�v�{�[�h�ɃR�s�[���A�擾
            MemSet (logMsg, sizeof(logMsg), 0x00);
            FldCopy(FrmGetObjectPtr(frm, frmId));
            memH = ClipboardGetItem(clipboardText, &len);
            if ((len != 0)&&(memH != 0))
            {
                urlP = MemHandleLock(memH);
                if (urlP != NULL)
                {
                    ptr = StrStr(urlP, "tp://");
                    if (ptr != NULL)
                    {
                        // URL���擾("http://"������)
                        ptr = ptr + StrLen("tp://");
                        StrCopy(logMsg, "http://");
                        dest = &logMsg[StrLen(logMsg)];

                        // �N���b�v�{�[�h����URL�̕�����(ASCII)�����𒊏o����
                        while ((*ptr > ' ')&&(*ptr < 0x7f))
                        {
                            *dest = *ptr;
                            dest++;
                            ptr++;
                        }
                    }
                    MemHandleUnlock(memH);                
                }
            }
            if (logMsg[0] != 0x00)
            {
                // NetFront���w�肳�ꂽURL�ŋN�����邩�m�F����
                if (NNsh_ConfirmMessage(ALTID_CONFIRM, logMsg,
                                        MSG_WEBLAUNCH_CONFIRM, 0) == 0)
                {
                    // NetFront���N������(NNsi�I����ɋN��)
                    (void) WebBrowserCommand(NNshGlobal->browserCreator,
                                             false, 0, 
                                             NNshGlobal->browserLaunchCode,
                                             logMsg, NULL);
                }
            }
        }
        break;

      case MNUID_EDIT_COPY:
        // �R�s�[
        frm   = FrmGetActiveForm();
        frmId = FrmGetFocus(frm);
        if (frmId == noFocus)
        {        
           // �t�H�[�J�X���擾�ł��Ȃ��A�I������
            break;
        }
        if (FrmGetObjectType(frm, frmId) == frmFieldObj)
        {
            FldCopy(FrmGetObjectPtr(frm, frmId));
            if (NNshParam->useSonyTinyFont != 0)
            {
                // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
                NNsi_HRFldDrawField(FrmGetObjectPtr(frm, frmId));
            }
        }
        break;

      case MNUID_SELECT_ALL:
        // ������̑S�I��
        frm = FrmGetActiveForm();
        fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        FldSetSelection(fld, 0, FldGetTextLength(fld));
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
        if (NNshParam->useSonyTinyFont != 0)
        {
            // �n�C���]�t�H���g�ݒ蒆�̎��ɂ́A�t�B�[���h�̍ĕ`����s��
            NNsi_HRFldDrawField(fld);
        }
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_NNshMessage                                       */
/*                                                       �C�x���g�n���h��  */
/*-------------------------------------------------------------------------*/
Boolean Handler_NNshMessage(EventType *event)
{
    Boolean   ret = false;
#ifdef USE_HANDERA
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        return (selEvt_KeyDown_DispMessage(event));
        break;

      case ctlSelectEvent:
        return (ctlSelEvt_DispMessage(event));
        break;

      case fldChangedEvent:
        NNshWinViewUpdateScrollBar(FLDID_MESSAGE, SCLID_MESSAGE);
        return (false);
        break;

      case menuEvent:
        return (menuEvt_DispMessage(event));
        break;

      case sclRepeatEvent:
        return (sclRepEvt_DispMessage(event));
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent:
        if (NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        break;
#endif

      case menuOpenEvent:
      case fldEnterEvent:
      case frmOpenEvent:
      case popSelectEvent:
      case lstSelectEvent:
      case lstEnterEvent:
      case lstExitEvent:
      case sclExitEvent:
      default:
       break;
    }
    return (ret);
}

/*=========================================================================*/
/*   Function : OpenForm_NNshMessage                                       */
/*                              (�X���b�h�V�K�ǂݏo���F���b�Z�[�W�\���ȊO) */
/*=========================================================================*/
Err OpenForm_NNshMessage(FormType *frm)
{
    Err                  ret;
    UInt16               bufferSize, fileMode;
    Char                 title   [NNSH_MSGTITLE_BUFSIZE];
    Char                 fileName[MAXLENGTH_FILENAME], *area;
    NNshSubjectDatabase  subjDB;
    ControlType         *selP;
    FieldAttrType        attr;
    FieldType           *fldP;
    NNshMessageIndex    *idxP;

    // ������@�\���g���E�g��Ȃ��ł�����ݒ�{�^����\�����邩�؂�ւ���
    if (NNshParam->useBookmark != 0)
    {
        FrmShowObject(frm, FrmGetObjectIndex(frm, BTNID_BOOKMARK));
    }
    else
    {
        FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_BOOKMARK));
    }

    // �X�������܂��擾
    ret = Get_Subject_Database(NNshParam->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // subject�f�[�^�x�[�X����f�[�^�擾�Ɏ��s
        NNsh_DebugMessage(ALTID_ERROR, "Get_Subject_Database()", "", ret);
        goto FUNC_END;
    }

    // �ꎞ�o�b�t�@�̈悪���łɊm�ۂ���Ă����������A�ēx�Ƃ�Ȃ����B
    if (NNshGlobal->msgIndex != NULL)
    {
        idxP = NNshGlobal->msgIndex;
        CloseFile_NNsh(&(idxP->fileRef));
        MEMFREE_PTR(NNshGlobal->msgIndex);
    }
    bufferSize = sizeof(NNshMessageIndex) + NNshParam->bufferSize + MARGIN * 2;
    NNshGlobal->msgIndex = MemPtrNew(bufferSize);
    if (NNshGlobal->msgIndex == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>MemPtrNew(MsgIndex)",
                       " size:", bufferSize);
        ret = ~errNone;
        goto FUNC_END;
    }
    NNsh_DebugMessage(ALTID_INFO, "MemPtrNew(MsgIndex)"," size:",bufferSize);
    MemSet(NNshGlobal->msgIndex, bufferSize, 0x00);

    // BBS�^�C�v�����[�N�̈�ɃR�s�[
    idxP = NNshGlobal->msgIndex;
    idxP->bbsType = (UInt16) subjDB.bbsType;

    // �^�C�g���ƃ{�[�h�j�b�N�l�[�������[�N�̈�ɃR�s�[
    StrNCopy(idxP->threadTitle, subjDB.threadTitle, MAX_THREADNAME - 1);
    StrNCopy(idxP->boardNick,   subjDB.boardNick,   MAX_NICKNAME - 1);

    // �ǂݍ��ރt�@�C���̏�����
    MemSet (fileName, sizeof(fileName), 0x00);

    // OFFLINE�X���̂Ƃ��́AboardNick��t�����Ȃ��悤�ɂ���
    if (StrCompare(subjDB.boardNick, OFFLINE_THREAD_NICK) != 0)
    {
        StrCopy(fileName, subjDB.boardNick);
    }
    else
    {
        // OFFLINE�X����VFS�Ɋi�[����Ă���ꍇ�ɂ́A�f�B���N�g����t������B
        if ((NNshParam->useVFS & NNSH_VFS_USEOFFLINE) != 0)
        {
            // ���O�x�[�X�f�B���N�g����t������
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

            // ����ɃT�u�f�B���N�g�����������ĕt������
            GetSubDirectoryName(subjDB.dirIndex, &fileName[StrLen(fileName)]);
        }
    }
    StrCat (fileName, subjDB.threadFileName);

    // �t�@�C���̂��肩�ŁA�J�����[�h��؂�ւ���
    if (subjDB.msgState == FILEMGR_STATE_OPENED_STREAM)
    {
        fileMode = ((NNSH_FILEMODE_TEMPFILE)|(NNSH_FILEMODE_READONLY));
    }
    else
    {
        fileMode = (NNSH_FILEMODE_READONLY);
    }
    ret = OpenFile_NNsh(fileName,fileMode,&(idxP->fileRef));
    if (ret != errNone)
    {
        MemPtrFree(idxP);
        idxP = NULL;
        NNsh_DebugMessage(ALTID_ERROR, "Message File Open :", fileName, ret);
        goto FUNC_END;
    }
    GetFileSize_NNsh(&(idxP->fileRef), &(idxP->fileSize));

    //  ���b�Z�[�W�X���b�h�̃C���f�b�N�X���쐬����
    ret = CreateMessageThreadIndex(idxP, &(idxP->nofMsg), idxP->msgOffset,
                                   (UInt16) subjDB.bbsType);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "ERR>index creation failure", "", ret);
        goto FUNC_END;
    }

    // �A���_�[���C���̕\���ݒ�
    MemSet(&attr, sizeof(FieldAttrType), 0x00);
    fldP  = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE));
    FldGetAttributes(fldP, &attr);
    if (NNshParam->disableUnderline != 0)
    {
        attr.underlined = noUnderline;
    }
    else
    {
        attr.underlined = grayUnderline;
    }
    FldSetAttributes(fldP, &attr);

    // �t�H�[���^�C�g���̍X�V(���b�Z�[�W�����i�[)
    StrCopy(title, "1:");
    NUMCATI(title, idxP->nofMsg);
    FrmCopyTitle(frm, title);

    // �X���^�C�g���̕\��
    selP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SELTRID_THREAD_TITLE));
    if (selP != NULL)
    {
        CtlSetLabel(selP, idxP->threadTitle);
    }

    // �ǂݏo���ӏ��̊m�F(�ُ�Ȃ�擪����)
    if ((subjDB.currentLoc < 1)||(subjDB.currentLoc > idxP->nofMsg))
    {
        idxP->openMsg = 1;
    }
    else
    {
        idxP->openMsg = subjDB.currentLoc;
    }
    idxP->prevMsg[0] = idxP->openMsg;
    idxP->histIdx    = 0;

    // �Ƃ肠�����t�@�C���̐擪����f�[�^��ǂݍ���ł���
    switch (ret)
    {
        case errNone:
        case vfsErrFileEOF:
        case fileErrEOF:
          // read OK, create Index!
          ret = errNone;
          break;

        default:
          goto FUNC_END;
          break;
    }

    // �����肪�ݒ肳��Ă���ꍇ�́A���̒l��߂��B
    if ((NNshParam->useBookmark != 0)&&(NNshParam->bookMsgNumber != 0))
    {
        if (NNshParam->bookMsgIndex == NNshParam->openMsgIndex)
        {
            (NNshGlobal->msgIndex)->openMsg = NNshParam->bookMsgNumber;
        }
    }

    // �u���C�ɓ���v�ݒ�Ȃ�A�`�F�b�N�{�b�N�X�Ƀ`�F�b�N�𔽉f
    if ((subjDB.msgAttribute & NNSH_MSGATTR_FAVOR) == NNSH_MSGATTR_FAVOR)
    {
        selP = FrmGetObjectPtr(frm,
                               FrmGetObjectIndex(frm, CHKID_FAVOR_THREAD));
        CtlSetValue(selP, (UInt16) 1);
    }

    // �t�H�[�J�X�����b�Z�[�W�ԍ��t�B�[���h�ɐݒ�
    FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_MESSAGE_NUMBER));

    // ��ʕ\��
    (void) displayMessage(NNshGlobal->msgIndex);
    NNshParam->lastFrmID = FRMID_MESSAGE;
    return (errNone);

FUNC_END:
    // �G���[�I������
    ReadMessage_Close(FRMID_THREAD);
    return (ret);
}

/*=========================================================================*/
/*   Function : GetSubDirectoryName                                        */
/*                                                  �T�u�f�B���N�g���̎擾 */
/*=========================================================================*/
Err GetSubDirectoryName(UInt16 index, Char *dirName)
{
    DmOpenRef              dirRef;
    UInt16                 size, nextIndex;
    Int16                  depth, saveDepth;
    Err                    ret;
    Char                  *buffer;
    NNshDirectoryDatabase  dirDb;

    if (index == 0)
    {
        // �w��INDEX���ُ�
        NNsh_DebugMessage(ALTID_INFO,
                          "There is not any sub-directory.", "", index);
        return (~errNone - 2);
    }

    OpenDatabase_NNsh(DBNAME_DIRINDEX, DBVERSION_DIRINDEX, &dirRef);

    // �T�u�f�B���N�g���g�b�v���擾����
    MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
    ret = SearchRecord_NNsh(dirRef, &index, NNSH_KEYTYPE_UINT16, 0,
                            sizeof(NNshDirectoryDatabase), &dirDb, &saveDepth);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_INFO, "SearchRecord_NNsh() ret:", "",   ret);
        NNsh_DebugMessage(ALTID_INFO, "SearchRecord_NNsh() index:", "", index);

        GetDBCount_NNsh(dirRef, &size);
        for (nextIndex = 0; nextIndex < size; nextIndex++)
        {
            MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
            ret = GetRecord_NNsh(dirRef, nextIndex,
                                 sizeof(NNshDirectoryDatabase), &dirDb);
            NNsh_DebugMessage(ALTID_INFO, "GetRecord_NNsh() ret:", "", ret);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " INDEX:", nextIndex);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " dirIdx:", dirDb.dirIndex);
            NNsh_DebugMessage(ALTID_INFO, dirDb.dirName, " prevdx:", dirDb.previousIndex);
        }

        // DB���R�[�h�m�ۃG���[
        CloseDatabase_NNsh(dirRef);
        return (ret);
    }
    NNsh_DebugMessage(ALTID_INFO, "depth:", dirDb.dirName, dirDb.depth);

    // �ꎞ�I�ɗ̈���m�ۂ���
    size = (dirDb.depth + 1) * sizeof(Char) * MAX_DIRNAME + MARGIN;
    buffer = MemPtrNew(size);
    if (buffer == NULL)
    {
        // �̈�m�ۃG���[
        CloseDatabase_NNsh(dirRef);
        return (~errNone);
    }
    MemSet(buffer, size, 0x00);

    // �T�u�f�B���N�g����dirDB���猟��
    saveDepth = dirDb.depth - 1;
    for (depth = saveDepth; depth >= 0; depth--)
    {
        NNsh_DebugMessage(ALTID_INFO, "copy ", dirDb.dirName, depth);

        // �f�B���N�g�������R�s�[����
        StrNCopy(&buffer[depth * sizeof(Char) * MAX_DIRNAME],
                 dirDb.dirName, (MAX_DIRNAME - 1));
        if (dirDb.previousIndex == 0)
        {
            // �����I��
            break;
        }

        // ���f�B���N�g��������
        nextIndex = dirDb.previousIndex;
        MemSet(&dirDb, sizeof(NNshDirectoryDatabase), 0x00);
        ret = SearchRecord_NNsh(dirRef, &(nextIndex), NNSH_KEYTYPE_UINT16, 0,
                                sizeof(NNshDirectoryDatabase), &dirDb, &index);
        if (ret != errNone)
        {
            // �������s�A���ɂ�����
            NNsh_DebugMessage(ALTID_ERROR, "GetRecord_NNsh()", "", ret);
            break;
	}
    }

    // �f�B���N�g�������R�s�[
    for (depth = 0; depth <= saveDepth; depth++)
    {
        StrCat(dirName, &buffer[depth * sizeof(Char) * MAX_DIRNAME]);
    }
    NNsh_DebugMessage(ALTID_INFO, "SubDirectory:", dirName, depth);

    // ���Ƃ��܂�
    MemPtrFree(buffer);
    CloseDatabase_NNsh(dirRef);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : PartReceiveMessage_View                                    */
/*                                                            �X�������擾 */
/*-------------------------------------------------------------------------*/
static Err PartReceiveMessage_View(void)
{
    Err                  ret;
    Char                 url[BUFSIZE];
    UInt16               index;
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    NNshMessageIndex    *idxP;

    // ���b�Z�[�W�C���f�b�N�X
    idxP = NNshGlobal->msgIndex;

    // ���b�Z�[�W�����f�[�^�x�[�X����擾
    ret = Get_Subject_Database(NNshParam->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "get_subject_database()", "", ret);
        return (ret);
    }

    // OFFLINE�X�����I�����ꂽ�ꍇ�A�X�V�s��\��
    if (StrCompare(subjDB.boardNick, OFFLINE_THREAD_NICK) == 0)
    {
        NNsh_InformMessage(ALTID_WARN, MSG_CANNOT_UPDATE,
                           idxP->threadTitle, 0);
        return (~errNone);
    }
    
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        return (ret);
    }

    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // �܂�BBS�ɑ΂��č����擾��I���������A���݃T�|�[�g�Ȃ�(�Ƃ肠����?)
        NNsh_ErrorMessage(ALTID_WARN, MSG_NOTSUPPORT_MACHI, "", 0);
        return (~errNone);
    }

    // ���ݎQ�ƒ��̃t�@�C������U�N���[�Y���Ă���
    CloseFile_NNsh(&(idxP->fileRef));

    // �X�������擾���s��
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsData.boardURL);
    StrCat (url, "dat/");
    StrCat (url, subjDB.threadFileName);

    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           NNshParam->openMsgIndex);

    // �\�����Ȃ���
    return (OpenForm_NNshMessage(FrmGetActiveForm()));
}
