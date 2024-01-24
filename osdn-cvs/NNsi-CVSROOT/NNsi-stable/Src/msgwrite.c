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

static void setBBSNameAndTime(UInt16 bbsType, Char *boardNick,
                              Char *buffer, UInt16 bufSize);
static void setFormParameters(Char *nick, Char *buffer, UInt16 bufSize,
                            Char *tempArea, UInt16 areaSize, Char *defSubmit);
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command, UInt16 bbsType, Err *ret);
static Err create_outputMessageFile(FormType *diagFrm, Char *buffer,
                                    UInt32 bufSize, Char *boardNick, 
                                    Char *key, UInt16 bbsType);
static Err threadWrite_Message(void);


/*-------------------------------------------------------------------------*/
/*   Function : setBBSNameAndTime                                          */
/*                                               BBS���ƌ��ݎ�����ݒ肷�� */
/*-------------------------------------------------------------------------*/
static void setBBSNameAndTime(UInt16 bbsType, Char *boardNick,
                              Char *buffer, UInt16 bufSize)
{
    Int16    timeZone, dayLight;
    UInt32   dummy;

    // BBS���̏o��
    MemSet (buffer, bufSize, 0x00);
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        StrCopy(buffer, "&BBS=");
    }
    else
    {
        StrCopy(buffer, "&bbs=");
    }
    StrCat (buffer, boardNick);

    // BBS�ꗗ�̖����� '/' �����Ă�����O���B
    if (buffer[StrLen(buffer) - 1] == '/')
    {
        buffer[StrLen(buffer) - 1] = '\0';
    }

    // ���ݎ����̏o��
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        StrCat (buffer, "&TIME=");
    }
    else
    {
        StrCat (buffer, "&time=");
    }

    // PalmOS version 4�ȏ�̂Ƃ��́A������UTC�ɕϊ����Ă���v�Z����
    if (NNshGlobal->palmOSVersion >= 0x04003000)
    {
        timeZone = PrefGetPreference(prefTimeZone);
        dayLight = PrefGetPreference(prefDaylightSavingAdjustment);
        dummy    = TimTimeZoneToUTC(TimGetSeconds(), timeZone, dayLight);
        NUMCATI(buffer, dummy - TIME_CONVERT_1904_1970_UTC);
    }
    else
    {
        NUMCATI(buffer, TimGetSeconds() - TIME_CONVERT_1904_1970);
    }

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : setFormParameters                                          */
/*                                        �t�H�[������p�����[�^���擾���� */
/*-------------------------------------------------------------------------*/
static void setFormParameters(Char *nick, Char *buffer,   UInt16 bufSize,
                              Char *tempArea, UInt16 areaSize, Char *defSubmit)
{
    Err          ret;
    Char        *ptr, *data, numBuf[16];
    NNshFileRef  fileRef;
    UInt32       readSize, fileSize, offset, num;

    // ��M�����t�H�[���̃p�����[�^����͂���
    ret = OpenFile_NNsh(FILE_RECVMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Read File Open :", FILE_RECVMSG, ret);
        goto USE_DEFAULT;
    }

    // ��M���b�Z�[�W�T�C�Y���擾
    GetFileSize_NNsh(&fileRef, &fileSize);

    // �t�@�C���̓ǂݏo���ʒu��ݒ肷��
    if (fileSize <= bufSize)
    {
        offset = 0;
    }
    else
    {
        offset = fileSize - bufSize;
    }
    ret = ReadFile_NNsh(&fileRef, offset, bufSize, buffer, &readSize);
    CloseFile_NNsh(&fileRef);
    if ((ret != errNone)&&(ret != vfsErrFileEOF)&&(ret != fileErrEOF))
    {        
        NNsh_InformMessage(ALTID_ERROR, "ReadFile_NNsh :",  FILE_RECVMSG, ret);
        goto USE_DEFAULT;
    }

    // �p�����[�^����������o��
    ptr  = buffer;
    data = &(tempArea[StrLen(tempArea)]);
    while (ptr < (buffer + bufSize))
    {
        // �擪��T��
        ptr = StrStr(ptr, "<input type=hidden name=");
        if (ptr == NULL)
        {
            // �p�����[�^��������Ȃ������A���[�v�𔲂���
            break;
        }
        ptr = ptr + StrLen("<input type=hidden name=");
        *data++ = '&';
        while ((*ptr != ' ')&&(*ptr != '>'))
        {
            *data++ = *ptr;
            ptr++;
        }
        // '=' ���o��܂œǂݔ�΂�
        while ((*ptr != '=')&&(*ptr != '>'))
        {
            ptr++;
        }
        *data++ = *ptr;
        ptr++;

        //   value�̐擪��'"'(�_�u���N�I�[�e�[�V����)�������ꍇ��
        // �p�����[�^�Ƃ��ĔF�������Ȃ�(�{���ɂ���Ȃ�ŗǂ��̂��H)
        // ...�悭�Ȃ�����...(���[�[���Ahana����[��...)
        if (*ptr == '"')
        {
            do {
                *data ='\0';
                data--;
            } while (*data != '&');
            continue;
        }
        
        // �擾�����p�����[�^�l���R�s�[����
        while ((*ptr != ' ')&&(*ptr != '>')&&(*ptr != '"'))
        {
            *data++ = *ptr;
            ptr++;
        }
    }
    *data = '\0';

    if (*tempArea == '\0')
    {
        // �p�����[�^���o���s�A�W���̃p�����[�^���i�[����
        goto USE_DEFAULT;
    }
    StrCat(tempArea, "&submit=");

    // submit�{�^���̃o�����[���擾
    ptr = StrStr(buffer, "<input type=submit value=");
    if (ptr == NULL)
    {
        //  �p�����[�^��������Ȃ������A�f�t�H���g��submit�{�^���̒l��
        // �R�s�[���ă��[�v�𔲂���
        StrCat(tempArea, defSubmit);
    }
    else
    {
        //////// submit�{�^���̓��e��(URL�G���R�[�h����)���f������ //////
        // (data�́A�R�s�[��f�[�^�̐擪 �Aptr�̓R�s�[���o�b�t�@)
        data = tempArea + StrLen(tempArea);
        ptr = ptr + StrLen("<input type=submit value=");
        if (*ptr == '"')
        {
            // �_�u���N�I�[�e�[�V�����Ȃ�P�����߂�
            ptr++;
        }
        // �擾�����p�����[�^�l���R�s�[����
        while ((ptr < (buffer + bufSize))&&
               ((*ptr != ' ')&&(*ptr != '>')&&(*ptr != '"')))
        {
            if ((*ptr == '.')||(*ptr == '_')||
                (*ptr == '-')||(*ptr == '*')||
                ((*ptr >= '0')&&(*ptr <= '9'))||
                ((*ptr >= 'A')&&(*ptr <= 'Z'))||
                ((*ptr >= 'a')&&(*ptr <= 'z')))
            {
                //  ���̂܂܃R�s�[����
                *data++ = *ptr;
                ptr++;
            }
            else
            {
                // URL�G���R�[�h����(0x88 => '%88' �ɕϊ�����)
                *data = '%';
                data++;
                *data = '\0';
                num = (UInt32) *ptr;
                MemSet(numBuf, sizeof(numBuf), 0x00);
                StrIToH(numBuf, num);

                // ���l�̉��Q�����~�����̂�...
                StrCat(data, &numBuf[6]);
                data = data + 2; // 2 == StrLen(data);
                ptr++;
            }
        }
    }
    *data = '\0';

    NNsh_DebugMessage(ALTID_INFO, "Param :",  tempArea, 0);
    return;

USE_DEFAULT:
    // �W����BBS���Ǝ��Ԃ��擾����
    MemSet(tempArea, areaSize, 0x00);
    setBBSNameAndTime(NNSH_BBSTYPE_2ch, nick, tempArea, areaSize);

    NNsh_DebugMessage(ALTID_INFO, "Param(Default):",  tempArea, 0);
    return;
}

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

/*=========================================================================*/
/*   Function : SaveWritingMessage                                         */
/*                                            �ҏW���̃��b�Z�[�W��ۑ����� */
/*=========================================================================*/
void SaveWritingMessage(void)
{
    Err          ret;
    Char        *buffer, *ptr;
    UInt32       writeSize;
    FormType    *frm;
    NNshFileRef  fileRef;

    // �ꎞ�o�b�t�@�̈���m��
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR() ", " size:",
                          (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        return;
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // �ҏW�����b�Z�[�W���擾����
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm,FLDID_WRITE_MESSAGE,buffer,(NNshGlobal->NNsiParam)->writeBufferSize);
    if (StrLen(buffer) == 0)
    {
        // �ҏW���������Ȃ��ꍇ�͏I������
        MEMFREE_PTR(buffer);
        return;
    }
    // ���X�ԍ��̂ݓ��͂���Ă����ꍇ�́A�ۑ����Ȃ����߂̃`�F�b�N
    if ((buffer[0] == '>')&&(buffer[1] == '>'))
    {
        // �Q�Ɣԍ����擪�ɋL������Ă����ꍇ
        ptr = buffer + 2;   // 2�́AStrLen(">>") �̈Ӗ�
        while ((*ptr >= '0')&&(*ptr <= '9'))
        {        
            ptr++;
        }
        if (*ptr == '\n')
        {
            ptr++;
            if (StrLen(ptr) == 0)
            {
                // ���X�ԍ��̂݋L������Ă����A�ۑ������I������
                MEMFREE_PTR(buffer);
                return;
            }
        }
    }

    // �������݉�ʏI�������X�������ŕۑ���OFF�̂Ƃ�
    if ((NNshGlobal->NNsiParam)->writeMessageAutoSave == 0)
    {
        // �ҏW��������ۑ����邩�m�F����
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_SAVEMSG, "", 0) != 0)
        {
            // Cancel�{�^���������ꂽ�A�ۑ������ɏI������
            MEMFREE_PTR(buffer);
            return;
        }
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

    // �ҏW�������̖��O����(Palm�f�o�C�X����)�t�@�C��(writeName.txt)�ɏ�������
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // �ҏW�����b�Z�[�W���擾����
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm, FLDID_WRITE_NAME, buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
    if (StrLen(buffer) == 0)
    {
        // �ҏW���������Ȃ��ꍇ�͏I������
        goto FUNC_END;

    }

    ret = OpenFile_NNsh(FILE_WRITENAME,
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

    // �ҏW��������Mail����(Palm�f�o�C�X����)�t�@�C��(writeMail.txt)�ɏ�������
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // �ҏW�����b�Z�[�W���擾����
    frm = FrmGetActiveForm();
    NNshWinGetFieldText(frm, FLDID_WRITE_EMAIL, buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
    if (StrLen(buffer) == 0)
    {
        // �ҏW���������Ȃ��ꍇ�͏I������
        goto FUNC_END;

    }
    ret = OpenFile_NNsh(FILE_WRITEMAIL,
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
    MEMFREE_PTR(buffer);
    return;
}

/*=========================================================================*/
/*   Function : Handler_WriteConfigulation                                 */
/*                                                       �C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_WriteConfigulation(EventType *event)
{
    // ���݂̃t�H�[�����擾
    switch (event->eType)
    {
      case ctlRepeatEvent:
        // ���s�[�g�{�^�����������Ƃ��̏���...
        // command = event->data.ctlRepeat.controlID;
        break;

      case ctlSelectEvent:
        // �{�^�����������Ƃ��̏���...
        // command = event->data.ctlSelect.controlID;
        break;

      case keyDownEvent:
        // �L�[�R�[�h�ɂ���ď����𕪊򂳂���
        switch (KeyConvertFiveWayToJogChara(event))
        {
          default:
            // ��L�ȊO�̃L�[�ł͉������Ȃ�
            return (false);
            break;
        }
        break;

      case menuEvent:
      default: 
        // �������Ȃ�
        return (false);
        break;
    }

    return (false);
}


/*-------------------------------------------------------------------------*/
/*   Function : openWriteConfigulation                                     */
/*                                          �������ݗp�ݒ荀�ڂ̕\���Ɛݒ� */
/*-------------------------------------------------------------------------*/
static void openWriteConfigulation(void)
{
    Boolean       ret = false;
    UInt16        btnId, sequenceType2, useBe2chLogin;
    FormType     *prevFrm, *diagFrm;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �_�C�A���O�\������
    PrepareDialog_NNsh(FRMID_SETTING_WRITE_DIALOG, &diagFrm, false);
    if (diagFrm == NULL)
    {
        return;
    }
    HandEraMoveDialog(diagFrm);

    // ��x��ʂ�`�悷��(���v����...)
    FrmDrawForm(diagFrm);

    // �f�[�^�̕\���ݒ�
    useBe2chLogin = (NNshGlobal->NNsiParam)->useBe2chInfo;
    sequenceType2 = (NNshGlobal->NNsiParam)->writeSequence2;
    SetControlValue(diagFrm, CHKID_USE_BE2chLOGIN,  &useBe2chLogin);
    SetControlValue(diagFrm, CHKID_WRITE_SEQUENCE2, &sequenceType2);

    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_WriteConfigulation);

    // �_�C�A���O�̕\��
    btnId = FrmDoDialog(diagFrm);
    
    // �_�C�A���O�̕\���f�[�^���t�H�[������擾����
    useBe2chLogin = 0;
    sequenceType2 = 0;
    UpdateParameter(diagFrm, CHKID_USE_BE2chLOGIN,  &useBe2chLogin);
    UpdateParameter(diagFrm, CHKID_WRITE_SEQUENCE2, &sequenceType2);

    // �_�C�A���O�\���̌�Еt��
    PrologueDialog_NNsh(prevFrm, diagFrm, false);

    // �R�}���h
    switch (btnId)
    {
      case BTNID_LOGIN_OYSTER:
#ifdef USE_SSL
        // '��'���O�C�����������{...
        ProceedOysterLogin();
#else
        // "���݃T�|�[�g���Ă��܂���" �\�����s��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOT_SUPPORT, "", 0);
#endif        
        break;
      
      case BTNID_LOGIN_Be2ch:
        // Be��2ch���O�C�����������{...
        ProceedBe2chLogin();
        break;

      case BTNID_DIALOG_OK:
        // OK�{�^���������ꂽ�Ƃ�...(�f�[�^�𔽉f������)
        (NNshGlobal->NNsiParam)->useBe2chInfo   = useBe2chLogin;
        (NNshGlobal->NNsiParam)->writeSequence2 = sequenceType2;
        break;      

      case BTNID_DIALOG_CANCEL:
      default:
        // Cancel�{�^���������ꂽ�Ƃ�...(�������Ȃ�)
        break;
    }
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
        SaveWritingMessage();
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

      case MNUID_NET_CONFIG:
        // �L�[A (�l�b�g���[�N�ݒ�)
        // �l�b�g���[�N�ݒ���J��
        // NNshGlobal->work1 = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenNetworkPreferences();
        break;

#if 0
      case MNUID_BT_ON:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        NNshGlobal->work1 = (NNshGlobal->NNsiParam)->lastFrmID;
        NNsiParam->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;

      case MNUID_BT_OFF:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;
#endif

      case MNUID_SHOW_DEVICEINFO:
        // �o�[�W�������̕\��
        ShowDeviceInfo_NNsh();
        break;

      case MNUID_PREVIEW:
        // �������ݓ��e�̃v���r���[
        previewWritingForm_NNsh();
        break;

      default:
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : previewWritingForm_NNsh                                    */
/*                                �������݃f�[�^�̃v���r���[�@�\(a2DA�g�p) */
/*-------------------------------------------------------------------------*/
static void previewWritingForm_NNsh(void)
{
    UInt16       ret;
    UInt32       size;
    Char         logMsg[BUFSIZE * 2], *txtP;
    FormType    *frm;
    FieldType   *fld;
    MemHandle    txtH;
    NNshFileRef  fileRef;

    // �t�B�[���h����A�f�[�^���o��
    frm = FrmGetActiveForm();
    fld = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    txtH    = FldGetTextHandle(fld);
    if (txtH == 0)
    {
        // �t�B�[���h�̃t�@�C���n���h���擾���s�A�A�A�I������
        return;
    }
    txtP = MemHandleLock(txtH);
    if (StrLen(txtP) == 0)
    {
        MemHandleUnlock(txtH);
        return;
    }

    // �t�@�C���Ƀf�[�^���o�͂���
    ret = OpenFile_NNsh(FILE_SENDMSG,
                        (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret == errNone)
    {
        (void) WriteFile_NNsh(&fileRef, 0, StrLen(txtP), txtP, &size);
        CloseFile_NNsh(&fileRef);
    }
    else
    {
        NNsh_DebugMessage(ALTID_ERROR, "OpenFile_NNsh() ", " ret:", ret);
    }
    MemHandleUnlock(txtH);

    // �N���b�v�{�[�h�ɁAa2DA�ɓn���f�[�^�ɂ��Ă̎w�����i�[����B
    MemSet (logMsg, sizeof(logMsg), 0x00);
    StrCopy(logMsg, A2DA_NNSIEXT_AADATASTART);
    StrCat (logMsg, A2DA_NNSIEXT_NNSICREATOR);
    StrCat (logMsg, A2DA_NNSIEXT_SRC_STREAM);
    StrCat (logMsg, A2DA_NNSIEXT_DATANAME);
    StrCat (logMsg, FILE_SENDMSG);
    StrCat (logMsg, A2DA_NNSIEXT_ENDDATANAME);
    StrCat (logMsg, A2DA_NNSIEXT_ENDDATA);
    ClipboardAddItem(clipboardText, logMsg, (StrLen(logMsg) + 1));

    // a2DA���N������
    (void) LaunchResource_NNsh('DAcc','a2DA','code',1000);

    return;
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
        StrNCopy(data, (NNshGlobal->NNsiParam)->handleName, BUFSIZE - 1);
        fieldId = FLDID_WRITE_NAME;
        chkBox  = &((NNshGlobal->NNsiParam)->useFixedHandle);
        break;

      case CHKID_SAGE:
      default:
        StrCopy(data, "sage");
        fieldId = FLDID_WRITE_EMAIL;
        chkBox  = &((NNshGlobal->NNsiParam)->writeAlwaysSage);
        break;
    }

    // �`�F�b�N�{�b�N�X�Ƀ`�F�b�N�������Ă����ꍇ
    if (CtlGetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,chkId))) == 0)
    {
        // �f�[�^�N���A
        MemSet(data, BUFSIZE, 0x00);
        *chkBox = 0;
    }
    else
    {
        // �f�[�^�ݒ�
        *chkBox = 1;
    }
    NNshWinSetFieldText(frm, fieldId, true, data, BUFSIZE);
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
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_MSGCLEAR, "", 0) == 0)
        {
            NNshWinSetFieldText(FrmGetActiveForm(), FLDID_WRITE_MESSAGE,
                                true, "", (NNshGlobal->NNsiParam)->writeBufferSize);
        }
        break;

        // Cancel�{�^�� �ق�(�X���Q�Ɖ�ʂɖ߂�)
      case BTNID_WRITE_CANCEL:
        // �ҏW�����b�Z�[�W���o�b�N�A�b�v
        SaveWritingMessage();
        returnToMessageView();
        break;

        // 'AA...' �{�^���������ꂽ�Ƃ�
      case BTNID_WRITE_AA:
        // AADA���N������
        LaunchResource_NNsh('DAcc','moAA','code',1000);
        break;

        // ���O��/Email�����̃`�F�b�N���X�V���ꂽ�Ƃ�
      case CHKID_HANDLENAME:
      case CHKID_SAGE:
        checkCheckBoxUpdate(event->data.ctlSelect.controlID);
        break;

      case SELID_SETTING:
        // �E��̃Z���N�^�g���K�������ꂽ...
        openWriteConfigulation();
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

    //tungsten T 5way navigator (281����A���ӁI)
    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // �u���v(�W���O�_�C������)�����������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winDown); 
        ret = true;
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
        NNshWinViewPageScroll(FLDID_WRITE_MESSAGE,
                              SCLID_WRITE_MESSAGE, 0, winUp); 
        ret = true;
        break;

      // CLIE��Ctrl+Enter �������� Jog���������Ƃ�
      case vchrJogPush:
        if ((NNshGlobal->NNsiParam)->writeJogPushDisable != 0)
        {
            // �������ݎ�PUSH�{�^���𖳌���ON�Ȃ疳���ɂ���
            break;
        }
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

#if 0
      case chrCapital_A:
      case chrSmall_A:
        // �L�[A (�l�b�g���[�N�ݒ�)
        // �l�b�g���[�N�ݒ���J��
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenNetworkPreferences();
        ret = true;
        break;

      case chrCapital_Y:
      case chrSmall_Y:
        // Bluetooth ON
#ifdef USE_BT_CONTROL
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;

      case chrCapital_I:
      case chrSmall_I:
        // Bluetooth OFF
#ifdef USE_BT_CONTROL
        (NNshGlobal->NNsiParam)->backupFormId = (NNshGlobal->NNsiParam)->lastFrmID;
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_WRITE_MESSAGE;
        OpenBluetoothPreferences(true);
        ret = true;
#endif  // #ifdef USE_BT_CONTROL
        break;

#endif

      // ���̑��̃{�^���Q
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
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
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
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
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
            }
        }
        break;
#endif

      case fldHeightChangedEvent:
      case fldEnterEvent:
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
    Err                  ret, subRet, dbRet;
    Char                 msg[BUFSIZE], *buffer;
    Char                 title[NNSH_WRITELBL_MAX + 1];
    UInt32               fileSize, readSize;
    UInt16               alwaysSage, useFixedHandle, titleLen;
    NNshFileRef          fileRef;
    NNshSubjectDatabase  subjDB;
    FieldAttrType        attr;
    FieldType           *fldP;

    // �iBt�؂�ւ���l�b�g�ݒ�ɂ��j�ꎞ�I�ɉ�ʂ���Ă����ꍇ�ɂ́A�����߂�
    if ((NNshGlobal->NNsiParam)->lastFrmID == NNSH_FRMID_WRITE_MESSAGE)
    {
        (NNshGlobal->NNsiParam)->lastFrmID = NNSH_FRMID_MESSAGE;
    }

    // �t�H�[���^�C�g���̐ݒ�
    MemSet (title, sizeof(title), 0x00);
    StrCopy(title, NNSH_WRITELBL_HEAD);
    dbRet = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (dbRet == errNone)
    {
        // �������ݐ�X�������t�H�[���^�C�g���ɐݒ肷��
        StrCat(title, ":");
        titleLen = (sizeof(NNSH_WRITELBL_HEAD)); // ":"��ǉ������̂�sizeof()
        StrNCopy(&title[titleLen],
                 subjDB.threadTitle,
                 (NNSH_WRITELBL_MAX - titleLen));
    }
    FrmCopyTitle(frm, title);

    alwaysSage     = (NNshGlobal->NNsiParam)->writeAlwaysSage;
    useFixedHandle = (NNshGlobal->NNsiParam)->useFixedHandle;

    // AADA���C���X�g�[������Ă��邩�m�F����
    if (CheckInstalledResource_NNsh('DAcc', 'moAA') == false)
    {
        // AADA���C���X�g�[������Ă��Ȃ��ꍇ�ɂ́AAA�}���{�^������������
        CtlSetUsable(FrmGetObjectPtr(frm, 
                               FrmGetObjectIndex(frm, BTNID_WRITE_AA)), false);
    }

    // �`�F�b�N�{�b�N�X�ݒ�(�n���h����)
    CtlSetValue(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,CHKID_HANDLENAME)),
                useFixedHandle);

    // �`�F�b�N�{�b�N�X�ݒ�(E-Mail��)
    CtlSetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CHKID_SAGE)),
                alwaysSage);

    // �A���_�[���C���̕\���ݒ�
    MemSet(&attr, sizeof(FieldAttrType), 0x00);
    fldP  = FrmGetObjectPtr(frm, 
                            FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
    if ((NNshGlobal->NNsiParam)->disableUnderlineWrite != 0)
    {
        FldGetAttributes(fldP, &attr);
        attr.underlined = noUnderline;
        FldSetAttributes(fldP, &attr);
    }

    // FILE_WRITEMSG�����݂��邩�`�F�b�N����
    fileSize = 0;
    ret = OpenFile_NNsh(FILE_WRITEMSG,
                        (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                        &fileRef);
    if (ret != errNone)
    {
        goto SET_FIELD;
    }

    // ��M���b�Z�[�W�T�C�Y���擾
    GetFileSize_NNsh(&fileRef, &fileSize);

    if (fileSize != 0)
    {
        // �ꎞ�o�b�t�@�̈���m��
        buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        if (buffer != NULL)
        {
            MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
            // �t�@�C������f�[�^��ǂݏo��
            ret = ReadFile_NNsh(&fileRef, 0, (NNshGlobal->NNsiParam)->writeBufferSize,
                                buffer, &readSize);
            if ((ret == errNone)||(ret == fileErrEOF))
            {
                NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE, false,
                                    buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
                ret = errNone;
            }
            MEMFREE_PTR(buffer);
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

    ///// NAME���𕜋�
    MemSet(msg, sizeof(msg), 0x00);
    subRet = OpenFile_NNsh(FILE_WRITENAME,
                           (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                           &fileRef);
    if (subRet == errNone)
    {
        subRet = ReadFile_NNsh(&fileRef, 0, sizeof(msg), msg, &readSize);
        if ((subRet == errNone)||(subRet == fileErrEOF))
        {
            NNshWinSetFieldText(frm, FLDID_WRITE_NAME, true,msg, BUFSIZE);
        }
    }
    CloseFile_NNsh(&fileRef);

    ///// E-MAIL���𕜋�
    MemSet(msg, sizeof(msg), 0x00);
    subRet = OpenFile_NNsh(FILE_WRITEMAIL,
                           (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                           &fileRef);
    if (subRet == errNone)
    {
        subRet = ReadFile_NNsh(&fileRef, 0, sizeof(msg), msg, &readSize);
        if ((subRet == errNone)||(subRet == fileErrEOF))
        {
            NNshWinSetFieldText(frm, FLDID_WRITE_EMAIL, true,msg, BUFSIZE);
            alwaysSage = 0;
        }
    }
    CloseFile_NNsh(&fileRef);

SET_FIELD:
    // �ۑ����Ă������t�@�C�����폜����
    (void) DeleteFile_NNsh(FILE_WRITEMSG, NNSH_VFS_DISABLE);
    (void) DeleteFile_NNsh(FILE_WRITENAME,NNSH_VFS_DISABLE);
    (void) DeleteFile_NNsh(FILE_WRITEMAIL,NNSH_VFS_DISABLE);

    // ���sage��������(�p�����[�^�ݒ莞)
    if (alwaysSage != 0)
    { 
        NNshWinSetFieldText(frm, FLDID_WRITE_EMAIL, true,
                            "sage", BUFSIZE);
    }

    // �R�e�n���@�\���g�p����(�p�����[�^�ݒ莞)
    if ((useFixedHandle != 0)&&((NNshGlobal->NNsiParam)->handleName[0] != '\0'))
    {
        NNshWinSetFieldText(frm, FLDID_WRITE_NAME, true, 
                            (NNshGlobal->NNsiParam)->handleName,
                            BUFSIZE);
    }

    // ���X�ԍ����t�B�[���h�̐擪�ɒǉ�����B
    if ((ret != errNone)&&((NNshGlobal->NNsiParam)->insertReplyNum != 0)&&(dbRet == errNone))
    {
        MemSet(msg, sizeof(msg), 0x00);
        StrCopy(msg, ">>");
        NUMCATI(msg, subjDB.currentLoc);
        StrCat (msg, "\n");
        NNshWinSetFieldText(frm, FLDID_WRITE_MESSAGE,false, msg,
                                                   (NNshGlobal->NNsiParam)->writeBufferSize);
    }
    // �t�B�[���h�̍ő�T�C�Y��ݒ肷��
    FldSetMaxChars(fldP, (NNshGlobal->NNsiParam)->writeBufferSize);

    // �t�H�[�J�X�����b�Z�[�W�t�B�[���h�ɐݒ�(�p�����[�^���ݒ肳��ĂȂ��ꍇ)
    if ((NNshGlobal->NNsiParam)->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_WRITE_MESSAGE));
        FldSetInsPtPosition(fldP, (NNshGlobal->NNsiParam)->writeBufferSize);

        // FEP��ON�ɂ���(PalmOS 4.0�ȏ�̏ꍇ)
        if (NNshGlobal->palmOSVersion > 0x04000000)
        {
            (void) TsmSetFepMode(NULL, tsmFepModeDefault);
        }
    }    

    // �X�N���[���o�[���X�V
    NNshWinViewUpdateScrollBar(FLDID_WRITE_MESSAGE, SCLID_WRITE_MESSAGE);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkWriteReply                                          */
/*                                                                         */
/*             �������݌�̉����m�F(�ݒ�v��Cookie or �������݌��� �𒊏o) */
/*-------------------------------------------------------------------------*/
static Char *checkWriteReply(Char *buffer, UInt32 bufSize, UInt32 command, UInt16 bbsType, Err *ret)
{
     NNshFileRef  fileRef;
     UInt32       readSize;
     Char         *start, *end, *ptr, *buf;

    // ��M���b�Z�[�W���J���āA�T�[�o����̉�����ǂݏo��
    MemSet(buffer, bufSize, 0x00);
    *ret = OpenFile_NNsh(FILE_RECVMSG,
                         (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE),
                         &fileRef);
    if (*ret != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "Open HTTP Reply Message ", "", *ret);
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

        if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
            (bbsType == NNSH_BBSTYPE_2ch_EUC))
        {
            // EUC��SJ�ɕϊ�
            buf = MEMALLOC_PTR((StrLen(start) + MARGIN) * 2);
            if (buf != NULL)
            {
                MemSet(buf, ((StrLen(start) + MARGIN) * 2), 0x00);
                StrCopySJ(buf, start);
                StrCopy(start, buf);
            }
        }
        else
        {
            buf = NULL;
        }
        // �^�C�g�����G���[���ǂ����m�F����
        ptr = StrStr(start, NNSH_SYMBOL_WRITE_NG);
        if (ptr != NULL)
        {
            // �G���[�ݒ�
            *ret = ~errNone;
            
            // �G���[���������Ă����A�ڍ׏���T��(�{�������當������擾)
            ptr = StrStr((end + 1), NNSH_SYMBOL_WRITE_NG);
            if (ptr != NULL)
            {
                // �������镶����̐擪��ύX����
                start = ptr;

                // ������̍Ō���擾����
                end = StrStr(start, "</");
                if (end != NULL)
                {
                    *end = '\0';
                }

                if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
                    (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
                    (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
                    (bbsType == NNSH_BBSTYPE_2ch_EUC))
                {
                    // EUC��SJ�ɕϊ�
                    if (buf != NULL)
                    {
                        MEMFREE_PTR(buf);
                    }
                    buf = MEMALLOC_PTR((StrLen(start) + MARGIN) * 2);
                    if (buf != NULL)
                    {
                        MemSet(buf, ((StrLen(start) + MARGIN) * 2), 0x00);
                        StrCopySJ(buf, start);
                        StrCopy(start, buf);
                    }
                }
            }
            else
            {
                // �{���̉��...�{���̐擪��T��
                ptr = end + StrLen("</title>");
                while (*ptr != '\0')
                {
                    // �^�O�f�[�^�͓ǂݔ�΂�
                    if (*ptr == '<')
                    {
                        ptr++;
                        while (*ptr != '>')
                        {
                            if (*ptr == '\0')
                            {
                                // �����񂪏I�������甲����
                                break;
                            }
                            ptr++;
                        }
                        ptr++;
                        continue;
                    }
                    // ���s�R�[�h������
                    if (*ptr == '\x0a')
                    {
                        ptr++;
                        continue;
                    }
                    break;
                }
                // �擪�f�[�^�����������ꍇ�ɂ́A�擪�����̕�����̏ꏊ�ɂ���
                if (*ptr != '\0')
                {
                    start = ptr;
                }
                // ������̖�����(�Ă��Ɓ[��)�T���B
                while ((ptr < buffer + bufSize)&&(*ptr != '<'))
                {
                    ptr++;
                }
                *ptr = '\0';                                
            }
        }
        if (buf != NULL)
        {
            MEMFREE_PTR(buf);
        }
    }
    else
    {
        // �������ݎw�� Cookie�̈ʒu��T��
        start = pickupCookie(ret, buffer, bufSize);
        NNsh_DebugMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);
    }
    return (start);
}

/*-------------------------------------------------------------------------*/
/*   Function : pickupCookie                                               */
/*                                                   Cookie��؂�o������  */
/*-------------------------------------------------------------------------*/
static Char *pickupCookie(Err *ret, Char *start, UInt32 bufSize)
{
    Char *ptr, *cpyPtr, *top, *mid;

    top = start;
    ptr = StrStr(top, "Set-Cookie: ");
    if (ptr == NULL)
    {
        // Cookie��������Ȃ�����...�I������B
        *ret = ~errNone;
        return (NULL);        
    }
    top = ptr + StrLen("Set-");
    
    // �������ݎw��Cookie�̍Ō����T��
    cpyPtr = top;
    while ((cpyPtr <= start + bufSize)&&(*cpyPtr != ';'))
    {
        cpyPtr++;
    }
    mid = cpyPtr;
    while ((ptr = StrStr((mid + 1), "Set-Cookie: ")) != NULL)
    {
        *cpyPtr = ';';
        cpyPtr++;
        *cpyPtr = ' ';
        cpyPtr++;
        ptr = ptr + StrLen("Set-Cookie: ");
        while ((ptr <= start + bufSize)&&(*ptr != ';'))
        {
            *cpyPtr = *ptr;
            cpyPtr++;
            ptr++;
        }
        mid = ptr;
    }
    *cpyPtr = '\0';
    return (top);
}

/*-------------------------------------------------------------------------*/
/*   Function : threadWrite_Message                                        */
/*                                                           �������ݏ���  */
/*-------------------------------------------------------------------------*/
static Err threadWrite_Message(void)
{
    UInt32               dateTime;
    UInt16               index, subBufSize;
#ifdef PREVENT_DUPLICATE_POST
    UInt16               nofRetry;
#endif  // #ifdef PREVENT_DUPLICATE_POST
    NNshSubjectDatabase  subjDB;
    NNshBoardDatabase    bbsData;
    Err                  ret;
    Char                *subBuf, msgID[MAX_THREADFILENAME];
    Char                 spId[BUFSIZE], *param;
    Char                *url, *buffer, *ptr, *start;
    FormType            *frm;

    // (�T�u)�e���|�����̈���m��
    subBufSize = 3 * BIGBUF;
    subBuf = MEMALLOC_PTR(subBufSize + MARGIN);
    if (subBuf == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, "SUB AREA MEMALLOC_PTR()",
                           " size:", subBufSize + MARGIN);
        return (~errNone);
    }

    param = MEMALLOC_PTR(BIGBUF + BUFSIZE + MARGIN);
    if (param == NULL)
    {
        NNsh_InformMessage(ALTID_ERROR, "SUB AREA2 MEMALLOC_PTR()",
                           " size:", (BIGBUF + MARGIN));
        MEMFREE_PTR(subBuf);
        return (~errNone);
    }
    MemSet(param, (BIGBUF + BUFSIZE + MARGIN), 0x00);

#ifdef PREVENT_DUPLICATE_POST
    // ��d���e�h�~�΍�(�b��)
    nofRetry = (NNshGlobal->NNsiParam)->nofRetry;
    (NNshGlobal->NNsiParam)->nofRetry = 0;
#endif // #ifdef PREVENT_DUPLICATE_POST

    // ���b�Z�[�W�����f�[�^�x�[�X����擾
    ret = Get_Subject_Database((NNshGlobal->NNsiParam)->openMsgIndex, &subjDB);
    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_INFO, "get_subject_database()", "", ret);
        goto FUNC_END;
    }

    // BBS�����f�[�^�x�[�X����擾
    ret = Get_BBS_Database(subjDB.boardNick, &bbsData, &index);

    if (ret != errNone)
    {
        // �G���[����(�f�o�b�O�\��)
        NNsh_DebugMessage(ALTID_ERROR, "Get_BBS_Database() nick:", 
                          subjDB.boardNick, ret);
        goto FUNC_END;
    }

    // �������݂��邩���m�F���郁�b�Z�[�W
    if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_WRITEMSG,
                            subjDB.threadTitle, 0) != 0)
    {
        ret = ~errNone;
        goto FUNC_END;
    }

    // �f�[�^���M�p���[�N�o�b�t�@�̊l��
    buffer = (Char *) MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        // �̈�̊m�ێ��s�I
        NNsh_InformMessage(ALTID_ERROR, "SEND AREA MEMALLOC_PTR()",
                           " size:", (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        goto FUNC_END;
    }
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet (subBuf, (subBufSize + MARGIN), 0x00);
    StrCopy(subBuf, subjDB.boardNick);
    subBuf[StrLen(subjDB.boardNick) - 1] = '\0';

    MemSet (msgID, sizeof(msgID), 0x00);
    StrCopy(msgID, subjDB.threadFileName);
    ptr = StrStr (msgID, ".");
    if (ptr != NULL)
    {
        *ptr = '\0';
    }

    // ���M���郁�b�Z�[�W��send.txt�֏o�͂���
    frm = FrmGetActiveForm();
    create_outputMessageFile(frm, buffer,
                             ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN),
                             subBuf, msgID, (UInt16) subjDB.bbsType);

    //�@���M��URL(�Ƃ�����胊�t�@��)�Ǝ����̍쐬
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);
    MemSet(spId,   sizeof(spId),   0x00);
    url = spId;
    StrCopy(url, bbsData.boardURL);
    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        // �܂�BBS�̏ꍇ
        StrCat(url, URL_PREFIX_MACHIBBS);
        StrCat(url, bbsData.boardNick);
        if (url[StrLen(url) - 1] == '/')
        {
            url[StrLen(url) - 1] = '\0';
        }
        StrCat(url, "&KEY=");
        StrCat(url, msgID); 

        // BBS���ƌ��ݎ����A�{�^���̏o��
        setBBSNameAndTime(subjDB.bbsType,bbsData.boardNick,subBuf,subBufSize);
        StrCat(subBuf, "&submit=%8F%91%82%AB%8D%9E%82%DE");
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // ������΁��i�a�a�r�̂Ƃ�
        // �������ݐ�URL��V�d�l�ɂ��킹�ĕύX
        //  "http://jbbs.shitaraba.com/bbs/write.cgi/(category)/(nick)/(key)/"
        StrCopy(url, "http://jbbs.livedoor.jp/bbs/write.cgi");

        // URL����J�e�S�����𒊏o����( '/computer/' �݂����ȂƂ���)
        StrCopy(param, bbsData.boardURL);
        start = &param[StrLen(param) - 1];
        if (*start == '/')
        {
            *start = '\0';
        }
        while ((param < start)&&(*start != '/'))
        {
            start--;
        }
        StrCat (url, start);
        StrCat (url, "/");

        // nick��ǉ� (���Ƃ��� '351/')
        StrCat (url, bbsData.boardNick);

        // �X���ԍ���ǉ� ("1015506694/" �Ƃ�)
        StrCat (url, msgID); 
        StrCat (url, "/");

        // BBS���ƌ��ݎ����A�{�^���̏o��
        setBBSNameAndTime(subjDB.bbsType,bbsData.boardNick,subBuf,subBufSize);
        //StrCat(subBuf, "&KEY=");
        //StrCat(subBuf, msgID);
        start++;
        StrCat(subBuf, "&DIR=");
        StrCat(subBuf, start);
        StrCat(subBuf, "&submit=%BD%F1%A4%AD%B9%FE%A4%E0");
        break;


      case NNSH_BBSTYPE_SHITARABA:
        // ������΂̂Ƃ� 
        StrCopy(url, "http://www.shitaraba.com/");
        StrCat (url, URL_PREFIX_SHITARABA);
        StrCat (url, bbsData.boardNick);
        // ������ '/' �����Ă�����O���B
        if (buffer[StrLen(url) - 1] == '/')
        {
            buffer[StrLen(url) - 1] = '\0';
        }
        StrCat (url, "&key=");
        StrCat (url, msgID);

        // BBS���A�{�^���̏o��(�Ȃ񂩕�...)
        MemSet(subBuf, subBufSize, 0x00);
        StrCat (subBuf, "&bbs=");
        StrCat (subBuf, bbsData.boardNick);
        // ������ '/' �����Ă�����O���B
        if (subBuf[StrLen(subBuf) - 1] == '/')
        {
            subBuf[StrLen(subBuf) - 1] = '\0';
        }
        StrCat(subBuf, "&submit=%A4%AB%A4%AD%A4%B3%A4%81");
        break;

      case NNSH_BBSTYPE_OTHERBBS:
        // 2ch�݊�BBS�̏ꍇ�́ABBS���ƌ��ݎ����A�{�^���𐶐����ďo��
        setBBSNameAndTime(subjDB.bbsType, bbsData.boardNick,
                          subBuf, subBufSize);
        StrCat(subBuf, "&submit=%8F%91%82%AB%8D%9E%82%DE");
        break;

      case NNSH_BBSTYPE_OTHERBBS_2:
        // URL����BBS���𒊏o����
        start = &bbsData.boardURL[StrLen(bbsData.boardURL) - 1];
        if (*start == '/')
        {
            start--;
        }
        while ((bbsData.boardURL < start)&&(*start != '/'))
        {
            start--;
        }
        start++;
        MemSet (param, (BIGBUF + BUFSIZE + MARGIN), 0x00);
        StrCopy(param, start);
        
        // 2ch�݊�BBS�̏ꍇ�ABBS���́A��URL�̖�������擾����
        // (BBS���A���ݎ����A�{�^���𐶐����ďo��)
        setBBSNameAndTime(subjDB.bbsType, param,
                          subBuf, subBufSize);
        StrCat(subBuf, "&submit=%8F%91%82%AB%8D%9E%82%DE");
        break;

      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      default:
        // 2ch�̏ꍇ
        StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
        StrCat (url, bbsData.boardNick);
        StrCat (url, msgID);
        StrCat (url,  "/");

        // ���M�V�[�P���X�Q�̂Ƃ��A�t�H�[���̃p�����[�^����x�擾���ǉ�����B
        if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
        {
            // �������݃t�H�[�����擾����
            MemSet (param, (BIGBUF + BUFSIZE + MARGIN), 0x00);
            StrCopy(param, url);
            StrCat (param, "1n");
            NNsh_DebugMessage  (ALTID_INFO, "Get Param. (URL)", param, 0);
            ret = NNshHttp_comm(HTTP_SENDTYPE_GET_NOTMONA, param,
                                NULL, NULL, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                                NULL);
            if (ret != errNone)
            {
                // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
                if (ret != netErrTimeout)
                {
                    NNsh_InformMessage(ALTID_ERROR,
                                       MSG_ERROR_HTTP_COMM, "[FORM]", ret);
                }
                else
                {
                    // �ʐM�^�C���A�E�g����
                    NNsh_InformMessage(ALTID_ERROR,
                                       MSG_OCCUR_TIMEOUT, "[FORM]", ret);
                }
            }

            // ��M�����t�H�[������A�p�����[�^������؂�o��
            MemSet(subBuf, subBufSize, 0x00);
            setFormParameters(bbsData.boardNick,
                              buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                              subBuf, subBufSize, "%8F%91%82%AB%8D%9E%82%DE");
        }
        else
        {
            // BBS���ƌ��ݎ����A�{�^���̏o��
            setBBSNameAndTime(subjDB.bbsType, bbsData.boardNick,
                              subBuf, subBufSize);
            StrCat(subBuf, "&hana=mogera&submit=%8F%91%82%AB%8D%9E%82%DE");
        }
#ifdef USE_SSL
        // Oyster�̃Z�b�V����ID��ǋL����
        if ((NNshGlobal->connectedSessionId != NULL)&&
            (*(NNshGlobal->connectedSessionId) != '\0')) 
        {
            StrCat(subBuf, "&sid=");
            StrCatURLencode(subBuf, NNshGlobal->connectedSessionId);
        }
#endif
        break;
    }
    MEMFREE_PTR(buffer);

    // �������݃V�[�P���X�^�C�v�Q�̂Ƃ��́A������Ƒҋ@����
    if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
    {
        // �u�ҋ@���v��\��
        Show_BusyForm(MSG_INFO_WRITEWAIT);

        SysTaskDelay(NNSH_WRITE_DELAYTIME * SysTicksPerSecond());

        Hide_BusyForm(false);
    }

    // ���b�Z�[�W�𑗐M����
    NNsh_DebugMessage(ALTID_INFO, "Send(1) ", url, 0);
    NNsh_DebugMessage(ALTID_INFO, "Param:  ", subBuf, 0);
    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_MACHIBBS:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_MACHIBBS, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SHITARABAJBBS, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_SHITARABA:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SHITARABA, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_OTHERBBS, url,
                            NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                            NULL);
        break;

      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      default:
        if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
        {
            ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SEQUENCE2, url,
                                NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                                NULL);
        }
        else
        {
            ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url,
                                NULL, subBuf, HTTP_RANGE_OMIT, HTTP_RANGE_OMIT,
                                NULL);
        }
        break;
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
        goto FUNC_END;
    }

    /////////////////////////////////////////////////////////////////////////
    //   ��M����Cookie���g�p���ď������݂��s��Ȃ��ꍇ�A�܂��z�X�g����
    // �u�������݂܂����B�v�Ɖ������������ꍇ�ɂ́ACookie���g�p�����������݂�
    // �s��Ȃ��悤�ɂ���
    // (�܂�BBS�̏ꍇ��Cookie�g�p�̏������݂��s��Ȃ��悤�ɂ���)
    /////////////////////////////////////////////////////////////////////////

    // �T�[�o����̉��������
    MemSet(subBuf, subBufSize + MARGIN, 0x00);
    start = checkWriteReply(subBuf, subBufSize, NNSH_WRITECHK_REPLY,
                            subjDB.bbsType, &ret);
    if (((NNshGlobal->NNsiParam)->useCookieWrite == 0)||
        (subjDB.bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (subjDB.bbsType == NNSH_BBSTYPE_SHITARABA)||
        (subjDB.bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (subjDB.bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
        (subjDB.bbsType == NNSH_BBSTYPE_OTHERBBS_2)||
        (StrCompare(start, NNSH_SYMBOL_WRITE_DONE) == 0))
    {
        // �X�����b�Z�[�W�̍đ��M�͍s��Ȃ��A�����擾�`�F�b�N��
        goto WRITE_NEXT;
    }

    // Cookie���g�p���ď������݃��b�Z�[�W���đ��M����
    NNsh_DebugMessage(ALTID_INFO, "PICK UP COOKIE...", "", 0);

    // �f�[�^��͗p�̃o�b�t�@���m�ۂ���
    buffer = (Char *) MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        // �̈�̊m�ێ��s�I
        NNsh_InformMessage(ALTID_ERROR, "ANALYSIS AREA MEMALLOC_PTR()",
                           " size:", (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        ret = ~errNone;
        goto FUNC_END;
    }

    // ���ł����t�H�[���̃p�����[�^�f�[�^���擾
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet (param, (BIGBUF + BUFSIZE + MARGIN), 0x00);
    StrCopy(param, "&hana=mogera");
    setFormParameters(bbsData.boardNick, buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                      param, (BIGBUF + BUFSIZE),
                      "%91S%90%D3%94C%82%F0%95"
                      "%89%82%A4%82%B1%82%C6%82%F0%8F%B3"
                      "%91%F8%82%B5%82%C4%8F%91%82%AB%8D"
                      "%9E%82%DE");

    // �T�[�o����̉�������́ASPID���擾����
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet(spId, sizeof(spId), 0x00);
    ptr = checkWriteReply(buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                          NNSH_WRITECHK_SPID, subjDB.bbsType, &ret);
    if (ptr == NULL)
    {
        if ((NNshGlobal->NNsiParam)->writeSequence2 == 0)
        {
            // �T�[�o����̉������ُ킾����
            NNsh_ErrorMessage(ALTID_ERROR, MSG_WRITEREPLY_WRONG, url, 0);
            ret = ~errNone;
            goto FUNC_END;
        }
        // �������݃V�[�P���X���̂Q�̂Ƃ��ɂ́ACookie pick up �����s�����Ɏ��ɂ�����...
        StrCopy(spId, "Cookie: a=b");
    }
    else
    {
        // SPID���擾����
        while ((*ptr != 'C')&&(*ptr != '\0'))
        {
            ptr++;
        }
        StrNCopy(spId, ptr, sizeof(spId) - 1);
    }

    // param�Ɏ����������Ă����Ƃ��́A-45�����l�ɕϊ�����
    ptr = StrStr(param, "&time=");
    if (ptr != NULL)
    {
        ptr      = ptr + StrLen("&time=");
        dateTime = StrAToI(ptr);
        dateTime = (dateTime > NNSH_WRITE_DELAYSECOND) ? (dateTime - NNSH_WRITE_DELAYSECOND) : 1;
        StrIToA(ptr, dateTime);

        NNsh_DebugMessage(ALTID_INFO, ptr, ":", dateTime);
    }

#ifdef USE_SSL
    // Oyster�̃Z�b�V����ID��ǋL����
    if ((NNshGlobal->connectedSessionId != NULL)&&
        (*(NNshGlobal->connectedSessionId) != '\0')) 
    {
        StrCat(ptr, "&sid=");
        StrCatURLencode(ptr, NNshGlobal->connectedSessionId);
    }
#endif

    // ��͂����̈���J������
    MEMFREE_PTR(buffer);

    //�@���M��URL�̍쐬
    MemSet(subBuf, subBufSize + MARGIN, 0x00);
    url = subBuf;
    StrCopy(url, bbsData.boardURL);
    StrCopy(&url[StrLen(url) - StrLen(bbsData.boardNick)],  URL_PREFIX);
    StrCat (url, bbsData.boardNick);
    StrCat (url, msgID);
    StrCat (url,  "/");

    // (parameter��cookie���Đݒ肵��)���b�Z�[�W�𑗐M����
    NNsh_DebugMessage(ALTID_INFO, "Send(2) ", url, 0);

    // �������݃V�[�P���X�^�C�v�Q�̂Ƃ��́A�����ł�������Ƒҋ@����
    if ((NNshGlobal->NNsiParam)->writeSequence2 != 0)
    {
        // �u�ҋ@���v��\��
        Show_BusyForm(MSG_INFO_WRITEWAIT);

        SysTaskDelay(NNSH_WRITE_DELAYTIME * SysTicksPerSecond());

        Hide_BusyForm(false);

        ret = NNshHttp_comm(HTTP_SENDTYPE_POST_SEQUENCE2, url, spId, param, 
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }
    else
    {
        ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url, spId, param, 
                            HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    }
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
        goto FUNC_END;
    }

    // �T�[�o����̉��������
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);
    start = checkWriteReply(subBuf, subBufSize, NNSH_WRITECHK_REPLY,
                            subjDB.bbsType, &ret);

WRITE_NEXT:

    // �������ݐ悪�����擾�ł��邩�ǂ����`�F�b�N����
    MemSet (url, sizeof(url), 0x00);
    switch (subjDB.bbsType)
    {
      case NNSH_BBSTYPE_SHITARABAJBBS_OLD:
      case NNSH_BBSTYPE_MACHIBBS:
        //  �܂�BBS/������΁�JBBS�ɏ������񂾂Ƃ��́A�X�������擾��
        // �ł��Ȃ��̂ł����Ő܂�Ԃ�
        NNsh_InformMessage(ALTID_INFO, MSG_NOTSUPPORT_MACHI2, "", 0);

        ret = errNone;
        goto FUNC_END;
        break;

      case NNSH_BBSTYPE_SHITARABA:
        // ������΂́A������͂��s��Ȃ�
        ret    = errNone;
        break;

      case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
        // �������@JBBS�́A������͂��s��Ȃ�
        ret    = errNone;
        //goto FUNC_END;     // �Ȃ��������擾�����܂������Ȃ��B�B�B
        break;

#if 0
      case NNSH_BBSTYPE_2ch_EUC:
      case NNSH_BBSTYPE_2ch:
      case NNSH_BBSTYPE_OTHERBBS:
      case NNSH_BBSTYPE_OTHERBBS_2:
#endif
      default:
        // ������͂̌��ʂ�\������
        if (ret == errNone)
        {
            // ����ɏ������܂ꂽ�Ƃ�
            NNsh_InformMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, start, 0);
        }
        else
        {
            // �������݃G���[�̂Ƃ�
            NNsh_InformMessage(ALTID_WARN, MSG_RECV_SERVERRESPONSE, start, 0);
        }
        break;
    }

    // ���b�Z�[�W�擾��URL�̍쐬
    (void) CreateThreadURL(NNSH_DISABLE, url, (BUFSIZE), &bbsData, &subjDB);

    // �X�������擾���s��
    (void) Get_PartMessage(url, bbsData.boardNick, &subjDB, 
                           (NNshGlobal->NNsiParam)->openMsgIndex, NULL);

FUNC_END:
#ifdef PREVENT_DUPLICATE_POST
    // ��d���e�h�~�΍�
    (NNshGlobal->NNsiParam)->nofRetry = nofRetry;
#endif // #ifdef PREVENT_DUPLICATE_POST

    if (ret != errNone)
    {
        // ��ʂ��ĕ`�悷��
        NNsi_FrmDrawForm(FrmGetActiveForm(), true);
    }
    MEMFREE_PTR(param);
    MEMFREE_PTR(subBuf);
    return (ret);
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

    MemSet (buffer, bufSize, 0x00);
    if (key != NULL)
    {
        // key�����̏o��
        if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
            (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
        {
            StrCopy(buffer, "KEY=");
        }
        else
        {
            StrCopy(buffer, "key=");
        }
        StrCat (buffer, key);
    }
    else
    {
        // key�����w��̏ꍇ�ɂ̓X������(�^�C�g��������f�[�^�擾)    
        StrCopy(buffer, "&subject=");
        ptr = buffer;
        (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
        outputTextField(diagFrm, FLDID_NEWTHREAD_TITLE, buffer, bufSize, bbsType, &sendRef);
        MemSet (buffer, bufSize, 0x00);
    }

    // ���O���̏o��
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        StrCat (buffer, "&NAME=");
    }
    else
    {
        StrCat (buffer, "&FROM=");
    }
    ptr = buffer;
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    outputTextField(diagFrm, FLDID_WRITE_NAME, buffer, bufSize, bbsType, &sendRef);

    // E-Mail���̏o��
    if ((bbsType == NNSH_BBSTYPE_MACHIBBS)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW))
    {
        ptr = "&MAIL=";
    }
    else
    {
        ptr = "&mail=";
    }
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    outputTextField(diagFrm, FLDID_WRITE_EMAIL, buffer, bufSize, bbsType, &sendRef);

    // ���b�Z�[�W���̏o��
    ptr = "&MESSAGE=";
    (void) AppendFile_NNsh(&sendRef, StrLen(ptr), ptr, &dummy);
    if (key != NULL)
    {
        outputTextField(diagFrm, FLDID_WRITE_MESSAGE, buffer, bufSize, bbsType, &sendRef);
    }
    else
    {
        // �X�����Ă̏ꍇ
        outputTextField(diagFrm, FLDID_NEWTHREAD_MESSAGE, buffer, bufSize, bbsType, &sendRef);
    }
    
    // �t�@�C���N���[�Y
    CloseFile_NNsh(&sendRef);

    return (errNone);
}

/*-------------------------------------------------------------------------*/
/*   Function : outputTextField                                            */
/*                                                      �e�L�X�g�̈�̏o�� */
/*-------------------------------------------------------------------------*/
static void outputTextField(FormType *frm, UInt16 fldID, Char *buf, UInt16 size, UInt16 bbsType, NNshFileRef *fileRef)
{
    Char   *dat;
    UInt32  dum;
    UInt16  len;

    NNshWinGetFieldText(frm, fldID, buf, size);
    len = StrLen(buf);
    if (len == 0)
    {
        // �l���L������Ă��Ȃ�����
        return;
    }

    if ((bbsType == NNSH_BBSTYPE_SHITARABA)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_OLD)||
        (bbsType == NNSH_BBSTYPE_SHITARABAJBBS_RAW)||
        (bbsType == NNSH_BBSTYPE_2ch_EUC))
    {
        // ������΁A������΁��i�a�a�r�̂Ƃ��͊����R�[�h�ϊ�
        dat = MEMALLOC_PTR(len * 2);
        if (dat == NULL)
        {
            // �̈�m�ۂɎ��s�����̂ŁA���̂܂܏o��
            (void) AppendFileAsURLEncode_NNsh(fileRef, len, buf, &dum);
        }
        else
        {
            // EUC�����R�[�h�ɕϊ����ďo��
            MemSet(dat, (len * 2), 0x00);
            StrCopyEUC(dat, buf);
            (void) AppendFileAsURLEncode_NNsh(fileRef, StrLen(dat), dat, &dum);
                                                  
            MEMFREE_PTR(dat);
        }
    }
    else
    {
        // ���̂܂܏o��
        (void) AppendFileAsURLEncode_NNsh(fileRef, len, buf, &dum);
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////
/////       �X�����ĉ�ʌn�̏���
//////////////////////////////////////////////////////////////////////////////

/*=========================================================================*/
/*   Function : OpenForm_newThread                                         */
/*                                                      �X�����ĉ�ʂ��J�� */
/*=========================================================================*/
void OpenForm_newThread(FormType *frm)
{
#ifdef USE_NEWTHREAD_FEATURE
    Err                ret;
    UInt16             len, selBBS, alwaysSage, useFixedHandle;
    Char               title[NNSH_NEWTHREAD_MAX + MARGIN];
    NNshBoardDatabase  bbsData;

    // �X�����Ă�������擾����
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

    // BBS�����擾����
    MemSet(&bbsData, sizeof(bbsData), 0x00);
    ret = Get_BBS_Info(selBBS, &bbsData);
    if (ret != errNone)
    {
        // BBS���擾���s�A�X���ꗗ��ʂ��J��
        NNsh_DebugMessage(ALTID_ERROR, MSG_NOTAVAIL_BBS, "(Get_BBS_Info())", ret);
        FrmGotoForm(NNshGlobal->backFormId);
        return;
    }

    // �^�C�g�����R�s�[����
    MemSet (title, sizeof(title), 0x00);
    StrCopy(title, NNSH_NEWTHREAD_LBLHEAD);
    len = StrLen(title);
    StrNCopy(&title[len], bbsData.boardName, (NNSH_NEWTHREAD_MAX - len));
    FrmCopyTitle(frm, title);

    alwaysSage     = (NNshGlobal->NNsiParam)->writeAlwaysSage;
    useFixedHandle = (NNshGlobal->NNsiParam)->useFixedHandle;

    // �`�F�b�N�{�b�N�X�ݒ�(�n���h����)
    CtlSetValue(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,CHKID_HANDLENAME)), useFixedHandle);
    checkCheckBoxUpdate(CHKID_HANDLENAME);

    // �`�F�b�N�{�b�N�X�ݒ�(E-Mail��)
    CtlSetValue(FrmGetObjectPtr(frm,FrmGetObjectIndex(frm,CHKID_SAGE)), alwaysSage);
    checkCheckBoxUpdate(CHKID_SAGE);

    // �X�N���[���o�[���X�V
    NNshWinViewUpdateScrollBar(FLDID_NEWTHREAD_MESSAGE, SCLID_NEWTHREAD_MESSAGE);

    // �t�H�[�J�X���^�C�g���t�B�[���h�ɐݒ�(�p�����[�^���ݒ肳��ĂȂ��ꍇ)
    if ((NNshGlobal->NNsiParam)->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_NEWTHREAD_TITLE));
    }

    // FEP��ON�ɂ���(PalmOS 4.0�ȏ�̏ꍇ)
    if (NNshGlobal->palmOSVersion > 0x04000000)
    {
        (void) TsmSetFepMode(NULL, tsmFepModeDefault);
    }

    // nilEvent���s
    EvtWakeup();

#endif
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   Handler_newThread                                        */
/*                                         �X�����ĉ�ʂ̃C�x���g�n���h��  */
/*-------------------------------------------------------------------------*/
Boolean Handler_newThread(EventType *event)
{
#ifdef USE_NEWTHREAD_FEATURE

    Boolean   ret = false;
#if defined(USE_HANDERA) || defined(USE_PIN_DIA)
    FormType *frm;
#endif

    switch (event->eType)
    {
      case keyDownEvent:
        // �L�[����
        selEvt_KeyDown_newThread(event);
        break;

      case ctlSelectEvent:
        // �{�^������
        selEvt_Control_newThread(event);
        break;

      case fldChangedEvent:
        // �t�B�[���h�ɕ�������
       NNshWinViewUpdateScrollBar(FLDID_NEWTHREAD_MESSAGE, SCLID_NEWTHREAD_MESSAGE);
       break;

      case sclRepeatEvent:
        // �X�N���[���o�[�X�V
        sclRepEvt_newThread(event);
        break;

      case menuEvent:
        // ���j���[�I��
        NNsh_MenuEvt_Edit(event);
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
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNshGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNshGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                FrmDrawForm(frm);        
            }
        }
        break;
#endif

      case fldHeightChangedEvent:
      case fldEnterEvent:
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
#else
    return (false);
#endif
}

/////  �X�����ĉ�ʂ̃T�u�֐��Q
#ifdef USE_NEWTHREAD_FEATURE
/*-------------------------------------------------------------------------*/
/*   Function : sclRepEvt_newThread                                        */
/*                                       �X�N���[���o�[�X�V�C�x���g�̏���  */
/*-------------------------------------------------------------------------*/
static Boolean sclRepEvt_newThread(EventType *event)
{
    Int16   lines;

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
        (void) NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE,
                                      SCLID_NEWTHREAD_MESSAGE, -lines, winUp);
    }
    else if (lines > 0)
    {
        (void) NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE, 
                                      SCLID_NEWTHREAD_MESSAGE,lines, winDown);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selEvt_Control_newThread                                   */
/*                                           ���s�{�^���������ꂽ���̏���  */
/*-------------------------------------------------------------------------*/
static Err execute_newThread(FormType *frm)
{ 
    Err                ret = ~errNone;
    UInt16             selBBS, subBufSize;
#ifdef PREVENT_DUPLICATE_POST
    UInt16               nofRetry;
#endif  // #ifdef PREVENT_DUPLICATE_POST
    NNshBoardDatabase  bbsData;
    Char              *buffer, *subBuf, *ptr;
    Char               url[MAX_URL + MARGIN], *spId;

    // �ꎞ�o�b�t�@�̈���m��
    spId = MEMALLOC_PTR(BIGBUF + BUFSIZE + MARGIN);
    if (spId == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR() ", " size:",
                          BIGBUF + BUFSIZE + MARGIN);
        return (~errNone);
    }
    MemSet(spId, (BIGBUF + BUFSIZE + MARGIN), 0x00);

    // �ꎞ�o�b�t�@�̈���m��
    buffer = MEMALLOC_PTR((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
    if (buffer == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "MEMALLOC_PTR() ", " size:",
                          (NNshGlobal->NNsiParam)->writeBufferSize + MARGIN);
        MEMFREE_PTR(spId);
        return (~errNone);
    }
    MemSet(buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);

    // (�T�u)�e���|�����̈���m��
    subBufSize = 3 * BIGBUF;
    subBuf = MEMALLOC_PTR(subBufSize + MARGIN);
    if (subBuf == NULL)
    {
        NNsh_DebugMessage(ALTID_ERROR, "SUB AREA MEMALLOC_PTR()", " size:", 
                          subBufSize + MARGIN);
        MEMFREE_PTR(spId);
        MEMFREE_PTR(buffer);
        return (~errNone);
    }
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);

    // �X�����Ă�������擾����
    selBBS = (NNshGlobal->NNsiParam)->lastBBS;

#ifdef PREVENT_DUPLICATE_POST
    // ��d���e�h�~�΍�(�b��)
    nofRetry = (NNshGlobal->NNsiParam)->nofRetry;
    (NNshGlobal->NNsiParam)->nofRetry = 0;
#endif // #ifdef PREVENT_DUPLICATE_POST

    // BBS�����擾����
    MemSet(&bbsData, sizeof(bbsData), 0x00);
    ret = Get_BBS_Info(selBBS, &bbsData);
    if (ret != errNone)
    {
        // BBS���擾���s�A�X���ꗗ��ʂ��J��
        NNsh_InformMessage(ALTID_ERROR, MSG_NOTAVAIL_BBS, "(Get_BBS_Info())", ret);
        goto NOT_SUPPORT;
    }
 
    // subBuf�ɔ�Nick���i�[ 
    StrCopy(subBuf, bbsData.boardNick);
    subBuf[StrLen(bbsData.boardNick) - 1] = '\0';
 
    // �X�����ėp���M���b�Z�[�W���t�H�[������擾
    create_outputMessageFile(frm, buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN),
                             subBuf, NULL, (UInt16) bbsData.bbsType);

    // BBS���ƌ��ݎ����A�{�^���̏o��
    setBBSNameAndTime(bbsData.bbsType, bbsData.boardNick,
                      buffer, (NNshGlobal->NNsiParam)->writeBufferSize);
    StrCat(buffer, "&hana=mogera&submit=%90V%8BK%83X%83%8C%83b%83h%8D%EC%90%AC%89%E6%96%CA%82%D6");

#ifdef USE_SSL
        // Oyster�̃Z�b�V����ID��ǋL����
        if ((NNshGlobal->connectedSessionId != NULL)&&
            (*(NNshGlobal->connectedSessionId) != '\0')) 
        {
            StrCat(buffer, "&sid=");
            StrCatURLencode(buffer, NNshGlobal->connectedSessionId);
        }
#endif

    // 2ch�̔�URL�ɖ₢���킹...
    MemSet (url, sizeof(url), 0x00);
    StrCopy(url, bbsData.boardURL);
    ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url, NULL, buffer, 
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (ret != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR, MSG_ERROR_HTTP_COMM, "[#1]", ret);
        }
        else
        {
            // �ʐM�^�C���A�E�g����
            NNsh_InformMessage(ALTID_ERROR, MSG_OCCUR_TIMEOUT, "[#1]", ret);
        }

        // ��ʂ��ĕ`�悷��
        NNsi_FrmDrawForm(FrmGetActiveForm(), true);

        // �֐��𔲂���
        goto NOT_SUPPORT;
    }

    // ���ł����t�H�[���̃p�����[�^�f�[�^���擾
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    MemSet (subBuf, subBufSize, 0x00);
    StrCopy(subBuf, "&hana=mogera");
    setFormParameters(bbsData.boardNick, buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                      subBuf,  subBufSize,
                      "%91S%90%D3%94C%82%F0%95"
                      "%89%82%A4%82%B1%82%C6%82%F0%8F%B3"
                      "%91%F8%82%B5%82%C4%8F%91%82%AB%8D"
                      "%9E%82%DE");

    // �T�[�o����̉�������́ASPID���擾����
    MemSet (buffer, ((NNshGlobal->NNsiParam)->writeBufferSize + MARGIN), 0x00);
    ptr = checkWriteReply(buffer, (NNshGlobal->NNsiParam)->writeBufferSize,
                          NNSH_WRITECHK_SPID, bbsData.bbsType, &ret);
    if (ptr == NULL)
    {
        // �T�[�o����̉������ُ킾����
        NNsh_ErrorMessage(ALTID_ERROR, MSG_WRITEREPLY_WRONG, url, 0);
        ret = ~errNone;
        goto NOT_SUPPORT;
    }

    // SPID���擾����
    MemSet(spId, (BIGBUF + BUFSIZE + MARGIN), 0x00);
    StrNCopy(spId, ptr, (BIGBUF + BUFSIZE));

#ifdef USE_SSL
    // Oyster�̃Z�b�V����ID��ǋL����
    if ((NNshGlobal->connectedSessionId != NULL)&&
        (*(NNshGlobal->connectedSessionId) != '\0')) 
    {
        StrCat(spId, "&sid=");
        StrCatURLencode(spId, NNshGlobal->connectedSessionId);
    }
#endif

    // �X�����Ď��s(������)
    ret = NNshHttp_comm(HTTP_SENDTYPE_POST, url, spId, subBuf, 
                        HTTP_RANGE_OMIT, HTTP_RANGE_OMIT, NULL);
    if (ret != errNone)
    {
        // (�^�C���A�E�g�ȊO��)�ʐM�G���[����
        if (ret != netErrTimeout)
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_ERROR_HTTP_COMM, "(#2)", ret);
        }
        else
        {
            NNsh_InformMessage(ALTID_ERROR,
                               MSG_OCCUR_TIMEOUT, "(#2)", ret);
        }

        // ��ʂ��ĕ`�悷��
        NNsi_FrmDrawForm(FrmGetActiveForm(), true);

        goto NOT_SUPPORT;
    }

    // �T�[�o����̉��������
    MemSet(subBuf, (subBufSize + MARGIN), 0x00);
    ptr = checkWriteReply(subBuf, subBufSize, NNSH_WRITECHK_REPLY, bbsData.bbsType, &ret);

    // ������͂̌��ʂ�\������
    if (ret == errNone)
    {
        // ����ɃX�����Ăł����Ƃ�
        NNsh_InformMessage(ALTID_INFO, MSG_RECV_SERVERRESPONSE, ptr, 0);
    }
    else
    {
        // �X�����ĂɎ��s�����Ƃ�
        NNsh_InformMessage(ALTID_WARN, MSG_RECV_SERVERRESPONSE, ptr, 0);
    }

NOT_SUPPORT:
#ifdef PREVENT_DUPLICATE_POST
    // ��d���e�h�~�΍�
    (NNshGlobal->NNsiParam)->nofRetry = nofRetry;
#endif // #ifdef PREVENT_DUPLICATE_POST

    // �̈��������A�I������
    MEMFREE_PTR(subBuf);
    MEMFREE_PTR(buffer);
    MEMFREE_PTR(spId);
    return (ret);
}

/*-------------------------------------------------------------------------*/
/*   Function : selEvt_Control_newThread                                   */
/*                                               �{�^���������ꂽ���̏���  */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_Control_newThread(EventType *event)
{
    Err       ret;
    FormType *frm;
    
    frm = FrmGetActiveForm();
    switch (event->data.ctlSelect.controlID)
    {
      case BTNID_NEWTHREAD_EXECUTE:
        // "���s�{�^��"�������ꂽ�Ƃ��i�X�����Ă����s�j
        if (NNsh_ConfirmMessage(ALTID_CONFIRM, MSG_CONFIRM_NEWTHREAD, "", 0) == 0)
        {
            // �X�����Ă����s����
            ret = execute_newThread(frm);
            if (ret != errNone)
            {
                // �������ݎ��s(�ڑ����s)���A�X�����ĉ�ʂɖ߂�B
                return (false);
                break;
            }
            // ��ʂ��ĕ`�悵�A�ꗗ��ʂɖ߂�
            FrmEraseForm(frm);
            FrmGotoForm(NNshGlobal->backFormId);
            return (true);
        }
        break;

      case BTNID_NEWTHREAD_CLEAR:
        // Clear�{�^��(�^�C�g������я������ݗ̈���N���A����)
        if (NNsh_ConfirmMessage(ALTID_CONFIRM,MSG_CONFIRM_MSGCLEAR, "", 0) == 0)
        {
            // ���b�Z�[�W�̈���N���A
            NNshWinSetFieldText(frm, FLDID_NEWTHREAD_MESSAGE, true, "", 
                                (NNshGlobal->NNsiParam)->writeBufferSize);

            // �^�C�g�����N���A
            NNshWinSetFieldText(frm, FLDID_NEWTHREAD_TITLE, true, "", 
                                MAX_THREADNAME);

            // �X�N���[���o�[���X�V
            NNshWinViewUpdateScrollBar(FLDID_NEWTHREAD_MESSAGE,
                                       SCLID_NEWTHREAD_MESSAGE);
        }
        break;

      case BTNID_NEWTHREAD_CANCEL:
        // Cancel�{�^��(�ꗗ��ʂɖ߂�)
        FrmEraseForm(frm);
        FrmGotoForm(NNshGlobal->backFormId);
        break;

      case SELID_SETTING:
        // �E��̃Z���N�^�g���K�������ꂽ...
        openWriteConfigulation();
        break;

      case CHKID_HANDLENAME:
      case CHKID_SAGE:
        // ���O��/Email�����̃`�F�b�N���X�V���ꂽ�Ƃ�
        checkCheckBoxUpdate(event->data.ctlSelect.controlID);
        break;

      default:
        // �����ȊO(�Ȃɂ����Ȃ�)
        break;
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : selectKeyDownEvent                                         */
/*                                                 �L�[�������ꂽ���̏���  */
/*                                (�W���O�A�V�X�g�n�e�e���̃W���O�����ǉ�) */
/*-------------------------------------------------------------------------*/
static Boolean selEvt_KeyDown_newThread(EventType *event)
{
    Boolean   ret = false;
    FormType *frm;
    UInt16    keyCode;

    //tungsten T 5way navigator (281����A���ӁI)
    keyCode = KeyConvertFiveWayToJogChara(event);
    switch (keyCode)
    {
      // �u���v(�W���O�_�C������)�����������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrTapWaveSpecificKeyRDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
        NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE,
                              SCLID_NEWTHREAD_MESSAGE, 0, winDown); 
        ret = true;
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrTapWaveSpecificKeyRUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
        NNshWinViewPageScroll(FLDID_NEWTHREAD_MESSAGE,
                              SCLID_NEWTHREAD_MESSAGE, 0, winUp); 
        ret = true;
        break;
 
      // Tab�L�[���������Ƃ��̏���
      case chrHorizontalTabulation:
        // �t�H�[�J�X�����b�Z�[�W�ɂ��Ă�
        frm = FrmGetActiveForm();
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_NEWTHREAD_MESSAGE));
        ret = true;
        break;

      // ���̑��̃{�^���Q
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case chrLeftArrow:
      case vchrRockerLeft:
      case vchrJogLeft:
      case vchrTapWaveSpecificKeyRLeft:
      case chrRightArrow:
      case vchrRockerRight:
      case vchrJogRight:
      case vchrTapWaveSpecificKeyRRight:
      case vchrJogBack:
      default:
        break;
    }
    return (ret);
}

#endif // #ifdef USE_NEWTHREAD_FEATURE
