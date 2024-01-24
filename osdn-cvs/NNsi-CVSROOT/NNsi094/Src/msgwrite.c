/*============================================================================*
 *  FILE: 
 *     msgwrite.c
 *
 *  Description: 
 *     Message write functions for NNsh. 
 *
 *===========================================================================*/
#define MSGWRITE_C
#include "local.h"

static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command);
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType);
static Err threadWrite_Message(void);

/*-------------------------------------------------------------------------*/
/*   Function : returnToMessageView                                        */
/*                                                          �Q�Ɖ�ʂɖ߂� */
/*-------------------------------------------------------------------------*/
static void returnToMessageView(void)
{
    FrmEraseForm(FrmGetActiveForm());
    FrmGotoForm(FRMID_MESSAGE);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : saveWritingMessage                                         */
/*                                            �ҏW���̃��b�Z�[�W��ۑ����� */
/*-------------------------------------------------------------------------*/
static void saveWritingMessage(void)
{
    Err          ret;
    Char        *buffer;
    UInt32       writeSize;
    FormType    *frm;
    NNshFileRef  fileRef;

    // �ꎞ�o�b�t�@�̈���m��
    buffer = MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MemPtrNew() ", " size:",
                          NNshParam->bufferSize + MARGIN);
        return;
    }
    MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);

    // �ҏW�����b�Z�[�W���擾����
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm,FLDID_WRITE_MESSAGE,buffer,NNshParam->bufferSize);
    if (StrLen(buffer) == 0)
    {
        // �ҏW���������Ȃ��ꍇ�͏I������
        goto FUNC_END;

    }

    // �ҏW��������ۑ����邩�m�F����
    if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_SAVEMSG, "", 0) != 0)
    {
        // Cancel�{�^���������ꂽ�A�I������
        goto FUNC_END;
    }

    // �ҏW��������(Palm�f�o�C�X����)�t�@�C��(write.txt)�ɏ�������
    ret = OpenFile_NNsh(FILE_WRITEMSG,
                        (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
        goto FUNC_END;
    }
    ret = AppendFile_NNsh(&fileRef, StrLen(buffer), buffer, &writeSize);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", ret);
    }
    ret = CloseFile_NNsh (&fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "CloseFile() ", " ret:", ret);
    }
    else
    {
        NNsh_DebugMessage(ALTID_INFO, "Save Message", " bytes:", writeSize);
    }

FUNC_END:
    MemPtrFree(buffer);
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : menuEvt_WriteMessage                                       */
/*                                            ���j���[���I�����ꂽ���̏��� */
/*-------------------------------------------------------------------------*/
static Boolean menuEvt_WriteMessage(EventType *event)
{
    switch (event->data.menu.itemID)
    {
        // ����ؒf
      case MNUID_NET_DISCONN:
        NNshNet_LineHangup();
        break;

        // �������ݎw��
      case MNUID_WRITE_START:
        if (threadWrite_Message() != errNone)
        {
            // �������ݎ��s(�ڑ����s)���A�������݉�ʂɖ߂�B
            return (false);
            break;
	}
        returnToMessageView();
        break;

        // �������ݒ��~ �ق�(�X���Q�Ɖ�ʂɖ߂�)
      case MNUID_WRITE_ABORT:
        // �ҏW�����b�Z�[�W���o�b�N�A�b�v
        saveWritingMessage();
        returnToMessageView();
        break;

        // �ҏW���j���[�Q
      case MNUID_EDIT_UNDO:
      case MNUID_EDIT_CUT:
      case MNUID_EDIT_COPY:
      case MNUID_EDIT_PASTE:
      case MNUID_SELECT_ALL:
        (void) NNsh_MenuEvt_Edit(event);
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : checkCheckBoxUpdate                                        */
/*                                  �`�F�b�N�{�b�N�X���X�V���ꂽ�Ƃ��̏��� */
/*-------------------------------------------------------------------------*/
static void checkCheckBoxUpdate(UInt16 chkId)
{
    Char         data[BUFSIZE];
    UInt16       fieldId, *chkBox;
    FormType    *frm;
    frm = FrmGetActiveForm();

    MemSet(data, BUFSIZE, 0x00);
    switch (chkId)
    {
      case CHKID_HANDLENAME:
        StrNCopy(data, NNshParam->handleName, BUFSIZE - 1);
        fieldId = FLDID_WRITE_NAME;
        chkBox  = &(NNshParam->useFixedHandle);
        break;

      case CHKID_SAGE:
      default:
        StrCopy(data, "sage");
        fieldId = FLDID_WRITE_EMAIL;
        chkBox  = &(NNshParam->writeAlwaysSage);
        break;
    }

    // �`�F�b�N�{�b�N�X�Ƀ`�F�b�N�������Ă����ꍇ
    if (CtlGetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,chkId))) != 0)
    {
        // �f�[�^�ݒ�
        NNshWinSetFieldText(frm, fieldId, data, StrLen(data));
        *chkBox = 1;
    }
    else
    {
        // �f�[�^�N���A
        MemSet(data, BUFSIZE, 0x00);
        NNshWinSetFieldText(frm, fieldId, data, 2);
        *chkBox = 0;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : selEvt_Control_WriteMessage                                */
/*                                               �{�^���������ꂽ���̏���  */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_Control_WriteMessage(EventType *event)
{
    Err ret;
    switch (event->data.ctlSelect.controlID)
    {
        // �������݃{�^��(�������݌�A�X���Q�Ɖ�ʂɖ߂�)
      case BTNID_WRITE_MESSAGE:
        ret = threadWrite_Message();
        if (ret != errNone)
        {
            // �������ݎ��s(�ڑ����s)���A�������݉�ʂɖ߂�B
            return (false);
            break;
	}
        returnToMessageView();
        break;

        // Clear�{�^��(�������ݗ̈���N���A����)
      case BTNID_WRITEMSG_CLR:
        NNshWinSetFieldText(FrmGetActiveForm(),
                            FLDID_WRITE_MESSAGE,"", 0);
        break;

        // Cancel�{�^�� �ق�(�X���Q�Ɖ�ʂɖ߂�)
      case BTNID_WRITE_CANCEL:
        // �ҏW�����b�Z�[�W���o�b�N�A�b�v
        saveWritingMessage();
        returnToMessageView();
        break;

        // ���O��/Email�����̃`�F�b�N���X�V���ꂽ�Ƃ�
      case CHKID_HANDLENAME:
      case CHKID_SAGE:
        checkCheckBoxUpdate(event->data.ctlSelect.controlID);
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 �L�[�������ꂽ���̏���  */
/*                                (�W���O�A�V�X�g�n�e�e���̃W���O�����ǉ�) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_WriteMessage(EventType *event)
{
    Err       err;
    FormType *frm;
    Boolean   ret = false;
    UInt16    keyCode;

    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // �u���v(�W���O�_�C������)�����������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winDown); 
        ret = true;
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winUp); 
        ret = true;
        break;

      // CLIE��Ctrl+Enter �������� Jog���������Ƃ�
      case vchrJogPush:
        err = threadWrite_Message();
        if (err != errNone)
        {
            // �������ݎ��s(�ڑ����s)���A�������݉�ʂɖ߂�B
            break;
	}
        returnToMessageView();
        ret = true;
        break;
 
      // Tab�L�[���������Ƃ��̏���
      case chrHorizontalTabulation:
        // �t�H�[�J�X�����b�Z�[�W�ɂ��Ă�
        frm = FrmGetActiveForm();
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
        ret = true;
        break;

      // ���̑��̃{�^���Q
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case vchrJogBack:
      default:
        break;
    }
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : sclRepEvt_WriteMessage                                     */
/*                                       �X�N���[���o�[�X�V�C�x���g�̏���  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_WriteMessage(EventType *event)
{
    Int16   lines;

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
      (void) NNshWinViewPageScroll(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE,
                                   -lines, winUp);
    }
    else if (lines > 0)
    {
      (void) NNshWinViewPageScroll(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE,
                                   lines, winDown);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : fldChgEvt_WriteMessage                                     */
/*                                                     �X�N���[���o�[�X�V  */
/*-------------------------------------------------------------------------*/
static Boolean fldChgEvt_WriteMessage(EventType *event)
{
    NNshWinViewUpdateScrollBar(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE);
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :   Hander_WriteMessage                                      */
/*                                                       �C�x���g�n���h��  */
/*-------------------------------------------------------------------------*/
Boolean Handler_WriteMessage(EventType *event)
{
    Boolean   ret = false;
#ifdef USE_HANDERA
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        // �L�[����
        return (selEvt_KeyDown_WriteMessage(event));
        break;

      case ctlSelectEvent:
        // �{�^������
        return (selEvt_Control_WriteMessage(event));
        break;

      case fldChangedEvent:
        // �t�B�[���h�ɕ�������
        return (fldChgEvt_WriteMessage(event));
        break;

      case sclRepeatEvent:
        // �X�N���[���o�[�X�V
        return (sclRepEvt_WriteMessage(event));
        break;

      case menuEvent:
        // ���j���[�I��
        return (menuEvt_WriteMessage(event));
       break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
	if(NNshGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
	    HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
	break;
#endif // #ifdef USE_HANDERA

      case fldEnterEvent:
      case fldHeightChangedEvent:
      case frmOpenEvent:
      case frmUpdateEvent:
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
/*   Function : OpenForm_WriteMessage                                      */
/*                                                    �������ݎw�����̏��� */
/*=========================================================================*/
void OpenForm_WriteMessage(FormType *frm)
{
    Err                  ret;
    Char                 msg[BUFSIZE], *buffer;
    UInt32               fileSize, readSize;
    NNshFileRef          fileRef;
    NNshSubjectDatabase  subjDB;
#if 0
    FieldAttrType        attrP;
    FieldType           *fldP;

    // �A���_�[���C���̕\���ݒ�
    MemSet(&attrP, sizeof(attrP), 0x00);
    fldP  = FrmGetObjectPtr(frm, 
                            FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    FldGetAttributes(fldP, &attrP);
    if (NNshParam->disableUnderline != 0)
    {
        attrP.underlined = noUnderline;
    }
    else
    {
        attrP.underlined = grayUnderline;
    }
    FldSetAttributes(fldP, &attrP);
#endif

    // FILE_WRITEMSG�����݂��邩�`�F�b�N����
    ret = OpenFile_NNsh(FILE_WRITEMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret == errNone)
    {
        // ��M���b�Z�[�W�T�C�Y���擾
        GetFileSize_NNsh(&fileRef, &fileSize);
        if (fileSize != 0)
        {
            // �ꎞ�o�b�t�@�̈���m��
            buffer = MemPtrNew(NNshParam->bufferSize + MARGIN);
            if (buffer != NULL)
            {
                MemSet(buffer, (NNshParam->bufferSize + MARGIN), 0x00);
                // �t�@�C������f�[�^��ǂݏo��
                ret = ReadFile_NNsh(&fileRef, 0, NNshParam->bufferSize,
                                    buffer, &readSize);
                if ((ret == errNone)||(ret == fileErrEOF))
                {
                    NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE,
                                        buffer, StrLen(buffer));
                    ret = errNone;
                }
                MemPtrFree(buffer);
            }
            else
            {
                NNsh_DebugMessage(ALTID_ERROR, "Memory Alloc. ", "", ret);
                ret = ~errNone;
            }
        }
        else
        {
            NNsh_DebugMessage(ALTID_ERROR, "file size is 0. ", "", ret);
            ret = ~errNone;
        }
        CloseFile_NNsh(&fileRef);
    }
    // �ۑ����Ă������t�@�C�����폜����
    (void) DeleteFile_NNsh(FILE_WRITEMSG, NNSH_VFS_DISABLE);

    // ���X�ԍ����t�B�[���h�̐擪�ɒǉ�����B
    if ((ret != errNone)&&(NNshParam->insertReplyNum != 0)&&
        (Get_Subject_Database(NNshParam->openMsgIndex, &subjDB) == errNone))
    {
        MemSet(msg, sizeof(msg), 0x00);
        StrCopy(msg, ">>");
        NUMCATI(msg, subjDB.currentLoc);
        StrCat (msg, "\n");
        NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE,msg, StrLen(msg));
    }

    // ���sage��������(�p�����[�^�ݒ莞)
    if (NNshParam->writeAlwaysSage != 0)
    { 
        NNshWinSetFieldText(frm, FLDID_WRITE_EMAIL,"sage", StrLen("sage"));

        // �`�F�b�N�{�b�N�X���ݒ肷��
        CtlSetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CHKID_SAGE)),1);
    }

    // �R�e�n���@�\���g�p����(�p�����[�^�ݒ莞)
    if ((NNshParam->useFixedHandle != 0)&&(NNshParam->handleName[0] != '\0'))
    {
        NNshWinSetFieldText(frm, FLDID_WRITE_NAME, NNshParam->handleName,
                            StrLen(NNshParam->handleName));

        // �`�F�b�N�{�b�N�X���ݒ肷��
        CtlSetValue(FrmGetObjectPtr(frm,
                                  FrmGetObjectIndex(frm,CHKID_HANDLENAME)),1);
    }

    // �X�N���[���o�[���X�V
    NNshWinViewUpdateScrollBar(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE);

    // �t�H�[�J�X�����b�Z�[�W�t�B�[���h�ɐݒ�(�p�����[�^���ݒ肳��ĂȂ��ꍇ)
    if (NNshParam->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkWriteReply                                          */
/*                                                                         */
/*             �������݌�̉����m�F(�ݒ�v��Cookie or �������݌��� �𒊏o) */
/*-------------------------------------------------------------------------*/
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command)
{
     NNshFileRef  fileRef;
     UInt32       readSize;
     Char         *start, *end, *ptr;
     Err          ret;

     // ��M���b�Z�[�W���J���āA�T�[�o����̉�����ǂݏo��
     MemSet(buffer, bufSize, 0x00);
     ret = OpenFile_NNsh(FILE_RECVMSG,
                         (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                         &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Open HTTP Reply Message ", "", ret);
        return (NULL);
    }
    (void) ReadFile_NNsh(&fileRef, 0, bufSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);

    if (command == NNSH_WRITECHK_REPLY)
    {
        // �T�[�o����̉����ɂ��āA�^�C�g�������̂ݒ��o����B(�b��H)
        ptr = buffer;
        start = StrStr(ptr, "<title>");
        if (start == NULL)
        {
            start = StrStr(ptr, "<TITLE>");
        }
        if (start != NULL)
        {
            start = start + StrLen("<TITLE>");
        }
        else
        {
            start = ptr;
        }
        end   = StrStr(start, "</title>");
        if (end == NULL)
        {
            end = StrStr(start, "</TITLE>");
        }
        if (end != NULL)
        {
            *end = '\0';
        }
    }
    else
    {
        // �������ݎw�� Cookie�̈ʒu��T��
        ptr   = buffer;
        start = StrStr(ptr, "Set-Cookie: ");
        if (start == NULL)
        {
            return (NULL);
        }

        // �������ݎw��Cookie�̍Ō����T��
        end = start;
        while ((end <= start + bufSize)&&(*end != ';'))
        {
            end++;
        }
        *end = '\0';
        NNsh_DebugMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);
    }
    return (start);
}

/*-------------------------------------------------------------------------*/
/*   Function : threadWrite_Message                                        */
/*                                                           �������ݏ���  */
/*-------------------------------------------------------------------------*/
static Err threadWrite_Message(void)
{
    UInt16               index;
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    Err                  ret;
    Char                 subBuf[BIGBUF], msgID[MAX_THREADFILENAME];
    Char                 spId[BUFSIZE], *url, *buffer, *ptr, *start;
    FormType            *frm;

    // ���b�Z�[�W�����f�[�^�x�[�X����擾
    ret = Get_Subject_Database(NNshParam->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "get_subject_database()", "", ret);
        return (ret);
    }

    // BBS�����f�[�^�x�[�X����擾
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);

    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        return (ret);
    }

    // �������݂��邩���m�F���郁�b�Z�[�W
    if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_WRITEMSG,
                            subjDB.threadTitle, 0) != 0)
    {
        return (~errNone);
    }

    // �f�[�^���M�p���[�N�o�b�t�@�̊l��
    buffer = (Char *) MemPtrNew(NNshParam->bufferSize + MARGIN);
    if (buffer == NULL)
    {
        // �̈�̊m�ێ��s�I
        NNsh_InformMessage(ALTID_ERROR, "SEND AREA MemPtrNew()",
                           " size:", NNshParam->bufferSize + MARGIN);
        return (~errNone);
    }
    MemSet (buffer, (NNshParam->bufferSize + MARGIN), 0x00);
    MemSet (subBuf, sizeof(subBuf), 0x00);
    StrCopy(subBuf, subjDB.boardNick);
    subBuf[StrLen(subjDB.boardNick) - 1] = '\0';

    MemSet (msgID, sizeof(msgID), 0x00);
    StrCopy(msgID, subjDB.threadFileName);
    ptr = StrStr (msgID, ".");
    if (ptr != NULL)
    {
        *ptr = '\0';
    }
    frm = FrmGetActiveForm();
    // ���M���郁�b�Z�[�W��send.txt�֏o�͂���
    create_outputMessageFile(frm, buffer, NNshParam->bufferSize,
                             subBuf, msgID, (UInt16) subjDB.bbsType);
    MemPtrFree(buffer);

    //�@���M��URL�̍쐬
    MemSet(subBuf, sizeof(subBuf), 0x00);
    url = subBuf;
    StrCopy(url, bbsData.boardURL);
    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // �܂�BBS�̏ꍇ
        StrCat(url, URL_PREFIX_MACHIBBS);
        StrCat(url, bbsData.boardNick);
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        }
        StrCat(url, "&KEY=");
        StrCat(url, msgID); 
    }
    else
    {
        // 2ch�̏ꍇ
        StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
        StrCat (url, bbsData.boardNick);
        StrCat (url, msgID);
        StrCat (url,  "/");
    }

    // ���b�Z�[�W�𑗐M
    NNsh_DebugMessage(ALTID_INFO, "Send URL(1)>", url, 0);
    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_MACHIBBS, url,
                            NULL, "&submit=%8F%91%82%AB%8D%9E%82%DE",
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    }
    else
    {
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url,
                            NULL, "&submit=%8F%91%82%AB%8D%9E%82%DE",
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    }
    if (ret != errNone)
    {
        if (ret != netErrTimeout)
        {
            // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_ERROR_HTTP_COMM, "(pre-Write)", ret);
        }
        else
        {
            // �^�C���A�E�g����
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_OCCUR_TIMEOUT, "(pre-Write)", ret);
        }
        return (ret);
    }

    /////////////////////////////////////////////////////////////////////////
    //   ��M����Cookie���g�p���ď������݂��s��Ȃ��ꍇ�A�܂��z�X�g����
    // �u�������݂܂����B�v�Ɖ������������ꍇ�ɂ́ACookie���g�p�����������݂�
    // �s��Ȃ��悤�ɂ���
    // (�܂�BBS�̏ꍇ��Cookie�g�p�̏������݂��s��Ȃ��悤�ɂ���)
    /////////////////////////////////////////////////////////////////////////

    // �T�[�o����̉��������
    MemSet(subBuf, sizeof(subBuf), 0x00);
    start = checkWriteReply(subBuf, sizeof(subBuf), NNSH_WRITECHK_REPLY);
    if ((NNshParam->useCookieWrite == 0)||
        (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (StrCompare(start, NNSH_SYMBOL_WRITE_DONE) == 0))
    {
        // �X�����b�Z�[�W�̍đ��M�͍s��Ȃ��A�����擾�`�F�b�N��
        goto WRITE_NEXT;
    }

    // Cookie���g�p���ď������݃��b�Z�[�W���đ��M 

    // �T�[�o����̉�������́ASPID���擾����
    MemSet(subBuf, sizeof(subBuf), 0x00);
    ptr = checkWriteReply(subBuf, sizeof(subBuf), NNSH_WRITECHK_SPID);
    if (ptr == NULL)
    {
        // �T�[�o����̉������ُ킾����
        return (~errNone);
    }

    // SPID���擾����
    MemSet(spId, sizeof(spId), 0x00);
    StrNCopy(spId, ptr, sizeof(spId) - 1);

    //�@���M��URL�̍쐬
    MemSet(subBuf, sizeof(subBuf), 0x00);
    url = subBuf;
    StrCopy(url, bbsData.boardURL);
    StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
    StrCat (url, bbsData.boardNick);
    StrCat (url, msgID);
    StrCat (url,  "/");

    // (cookie���Đݒ肵��)���b�Z�[�W���M
    NNsh_DebugMessage(ALTID_INFO, "Send URL(2)>", url, 0);
    ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url,
                        spId,
                        "&submit=%91S%90%D3%94C%82%F0%95"
                        "%89%82%A4%82%B1%82%C6%82%F0%8F%B3"
                        "%91%F8%82%B5%82%C4%8F%91%82%AB%8D"
                        "%9E%82%DE",
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT);
    if (ret != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_ERROR_HTTP_COMM, "(Write)", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_OCCUR_TIMEOUT, "(Write)", ret);
        }
        return (ret);
    }
    // �T�[�o����̉��������
    MemSet(subBuf, sizeof(subBuf), 0x00);
    start = checkWriteReply(subBuf, sizeof(subBuf), NNSH_WRITECHK_REPLY);

WRITE_NEXT:

    // ������͂̌��ʂ�\������
    NNsh_InformMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);

    // �������ݐ悪�܂�BBS���ǂ����`�F�b�N����
    if (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        // �܂�BBS�ɏ������񂾂Ƃ��́A�X�������擾���ł��Ȃ��̂Ő܂�Ԃ��B
        NNsh_InformMessage(ALTID_INFO, MSG_NOTSUPPORT_MACHI2, "", 0);
        return (errNone);
    }

    // �X�������擾���s��
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsData.boardURL);
    StrCat (url, "dat/");
    StrCat (url, subjDB.threadFileName);
    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           NNshParam->openMsgIndex);
    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : create_outputMessageFile                                   */
/*                                                �������݃��b�Z�[�W�̏o�� */
/*-------------------------------------------------------------------------*/
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType)
{
    Err          ret;
    Char        *ptr;
    Int16        timeZone, dayLight;
    UInt32       dummy;
    NNshFileRef  sendRef;

    // �o�͗p�t�@�C���̏���
    (void) DeleteFile_NNsh(FILE_SENDMSG, NNSH_VFS_DISABLE);
    ret = OpenFile_NNsh(FILE_SENDMSG, 
                        (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE),
                        &sendRef);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_ERROR, "OpenFile_NNsh() ", FILE_SENDMSG, ret);
        return (ret);
    }

    // key�����̏o��
    MemSet (buffer, bufSize, 0x00);
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCopy(buffer, "KEY=");
    }
    else
    {
        StrCopy(buffer, "key=");
    }
    StrCat (buffer, key);
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCat (buffer, "&NAME=");
    }
    else
    {
        StrCat (buffer, "&FROM=");
    }
    ptr = buffer;
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);

    // ���O���̏o��
    NNshWinGetFieldText(diagFrm, FLDID_WRITE_NAME, buffer, bufSize);
    ptr = buffer;
    if (StrLen(ptr) != 0)
    {
        (void) AppendFileAsURLEncode_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    }

    // E-Mail���̏o��
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        ptr = "&MAIL=";
    }
    else
    {
        ptr = "&mail=";
    }
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    NNshWinGetFieldText(diagFrm, FLDID_WRITE_EMAIL, buffer, bufSize);
    ptr = buffer;
    if (StrLen(ptr) != 0)
    {
        (void) AppendFileAsURLEncode_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    }

    // ���b�Z�[�W���̏o��
    ptr = "&MESSAGE=";
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    NNshWinGetFieldText(diagFrm, FLDID_WRITE_MESSAGE, buffer, bufSize);
    ptr = buffer;
    if (StrLen(ptr) != 0)
    {
        (void) AppendFileAsURLEncode_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    }

    // BBS������ь��ݎ����̏o��
    MemSet (buffer, bufSize, 0x00);
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCopy(buffer, "&BBS=");
    }
    else
    {
        StrCopy(buffer, "&bbs=");
    }
    StrCat (buffer, boardNick);

    // ������ݒ肷��(����A2ch�̎d�l�ɂ��킹�ĕύX����\�肠��)
    if (bbsType == NNSH_BBSTYPE_MACHIBBS)
    {
        StrCat (buffer, "&TIME=");
    }
    else
    {
        StrCat (buffer, "&time=");
    }

    // PalmOS version 4�ȏ�̂Ƃ��́A������UTC�ɕϊ����Ă���v�Z����
    ret = FtrGet(sysFtrCreator, sysFtrNumROMVersion, &dummy);
    if ((ret == errNone)&&(dummy >= 0x04003000))
    {
        timeZone = PrefGetPreference(prefTimeZone);
        dayLight = PrefGetPreference(prefDaylightSavingAdjustment);
        dummy    = TimUTCToTimeZone(TimGetSeconds(), timeZone, dayLight);
        NUMCATI(buffer, dummy - TIME_CONVERT_1904_1970_UTC);
    }
    else
    {
        NUMCATI(buffer, TimGetSeconds() - TIME_CONVERT_1904_1970);
    }
    (void) AppendFile_NNsh(&sendRef, StrLen(buffer), buffer, &dummy);

    // �t�@�C���N���[�Y
    CloseFile_NNsh(&sendRef);

    return (errNone);
}
