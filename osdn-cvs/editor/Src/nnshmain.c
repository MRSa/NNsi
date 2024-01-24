/*============================================================================*
 *  FILE: 
 *     nnshset.c
 *
 *  Description: 
 *     NNsh parameter setting functions for NNsh.
 *
 *===========================================================================*/
#define NNSHMAIN_C
#include "local.h"

extern Boolean LaunchResource_NNsh(UInt32 type, UInt32 creator, DmResType resType, DmResID resID);
extern Boolean CheckInstalledResource_NNsh(UInt32 type, UInt32 creator);

extern Boolean Handler_NNshSetting(EventType *event);
extern Err OpenForm_NNshSetting(FormType *frm);

extern void PrepareDialog_NNsh(UInt16 formID, FormType **diagFrm, Boolean silk);
extern void PrologueDialog_NNsh(FormType *prevFrm, FormType *diagFrm);

extern void NNshWinSetPopItems(FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item);

extern void StrNCopySJtoJIS(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine, Boolean isFtr);
extern void StrNCopySJtoEUC(Char *dst, Char *src, UInt32 size, UInt16 newLine, Boolean isFtr);
extern void StrNCopySJtoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine, Boolean isFtr);
extern void StrNCopyEUCtoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine);
extern void StrNCopySJtoSJLF(Char *dst, Char *src, UInt32 size, UInt16 newLine);
extern void StrNCopyJIStoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine);

static void setFileListItems(FormType *frm, Char **fnameP, UInt16 loc);

/*=========================================================================*/
/*   Function :   checkFieldEnterAvailable                                 */
/*                        �t�B�[���h�ɕ����f�[�^���i�[�ł��邩�`�F�b�N���� */
/*                         (�ő啶���񒷊i�[����Ă���̈�Ƀf�[�^�����   */
/*                          ���悤�Ƃ����ꍇ�AFatal Error���o��̂�}�~)   */
/*                         �� �Ԃ�l��true�Ȃ���͉Afalse�Ȃ���͕s�� �� */
/*=========================================================================*/
Boolean checkFieldEnterAvailable(UInt16 fldId, EventType *event)
{
    FormType  *frmP;
    FieldType *fldP;
    UInt32     len, maxChar;

    // �t�B�[���h�̃|�C���^���擾
    frmP = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, fldId));

    // �t�B�[���h�̃e�L�X�g�f�[�^�����擾
    maxChar  = FldGetMaxChars(fldP); 
    len      = FldGetTextLength(fldP);
    if (len >= maxChar)
    {
        // ���͉\�f�[�^�T�C�Y�𒴂��Ă����ꍇ�A�����������Ȃ��悤�ɂ���
        return (false);
    }
    return (true);
}

/*=========================================================================*/
/*   Function :   DataSaveToFile                                           */
/*                                                            �f�[�^�̕ۊ� */
/*=========================================================================*/
Boolean DataSaveToFile(FormType *frm, Char *data, Char *fileName, UInt16 loc, UInt16 kanji, UInt16 newLine)
{
    Err         ret;
    Boolean     isFtrMem;
    Char       *buffer, *ptr, tempFile[MAXLENGTH_FILENAME + MARGIN];
    UInt16      fileMode, tempMode;
    UInt32      size, writeSize, len, bufSize;
    NNshFileRef fileRef;
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // �g���q(.txt)�����Ă��Ȃ���Εt����
    if (StrStr(fileName, DATAFILE_SUFFIX) == NULL)
    {
        StrCat(fileName, DATAFILE_SUFFIX);
    }

    if (NNsh_ConfirmMessage(ALTID_CONFIRM, fileName, MSG_WARN_OUTPUT, loc) != 0)
    {
        // �L�^�𒆎~����A�A�A
        NNsh_InformMessage(ALTID_INFO, MSG_SAVE_ABORT, "", 0);
        return (false);
    }

    // �t�@�C�������݂��邩�m�F����
    if (loc == 0)
    {
        // Palm��
        fileMode = NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
        tempMode = NNSH_COPY_PALM_TO_PALM;
    }
    else
    {
        // VFS
        fileMode = NNSH_FILEMODE_READONLY;
        tempMode = NNSH_COPY_VFS_TO_VFS;
    }
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret == errNone)
    {
        ret = GetFileSize_NNsh(&fileRef, &bufSize);
        CloseFile_NNsh(&fileRef);
        if ((ret == errNone)&&(bufSize != 0))
        {
            //  �t�@�C�������݂��āA�o�b�N�A�b�v�t�@�C���łȂ��ꍇ�A
            // ���炩���߃o�b�N�A�b�v�t�@�C�����쐬����
            if (StrCompare(fileName, FILE_TEMPFILE) != 0)
            {
                MemSet(tempFile, sizeof(tempFile), 0x00);
                StrCopy(tempFile, fileName);
                StrCat(tempFile, BACKFILE_SUFFIX);
                CopyFile_NNsh(tempFile, fileName, tempMode);
            }
        }
    }

    // �L�^�t���O��ݒ肷��
    if (loc == 0)
    {
        // Palm���ɋL�^����
        fileMode = NNSH_FILEMODE_CREATE | NNSH_FILEMODE_TEMPFILE;
    }
    else
    {
        // VFS�ɋL�^����
        fileMode = NNSH_FILEMODE_CREATE;
    }

    // �f�[�^�o�b�t�@���m�ۂ���
    size = StrLen(data);
    size = size * 2 + MARGIN + 2;
    if (size > NNsiParam->bufferSize)
    {
        // Feature���������g�p����
        ret = FtrPtrNew(SOFT_CREATOR_ID, NNSH_WORKBUF_FTR, size, (void **) &buffer);
        if (ret != errNone)
        {
            buffer = NULL;
        }
        isFtrMem = true;
    }
    else
    {
        // �ʏ탁�������g�p����
        buffer = MemPtrNew_NNsh(size);
        isFtrMem = false;
     }
    if (buffer == NULL)
    {
        // �L�^�𒆎~����
        NNsh_InformMessage(ALTID_INFO, MSG_SAVE_ABORT, MSG_CANNOT_ALLOC, 0);
        return (false);
    }

    // BUSY�t�H�[����\��
    Show_BusyForm("Saving...");

    // �������N���A
    if (isFtrMem == true)
    {
        DmSet(buffer, 0, size, 0x00);
    }
    else
    {
        MemSet(buffer, size, 0x00);
    }

    // �����R�[�h�{���s�R�[�h���l�����ăR�s�[����
    switch (kanji)
    {
      case NNSH_KANJI_EUC:
        // EUC�R�[�h
        StrNCopySJtoEUC(buffer, data, size, newLine, isFtrMem);
        break;

      case NNSH_KANJI_JIS:
        // JIS�R�[�h
        StrNCopySJtoJIS(buffer, data, size, newLine, isFtrMem);
        break;

      case NNSH_KANJI_UTF8:
        // UTF-8(Unicode)�R�[�h
        StrNCopySJtoUTF8(buffer, data, size, newLine, isFtrMem);
        break;

      case NNSH_KANJI_SHIFTJIS:
      default:
        // SHIFT JIS�R�[�h
        StrNCopySJtoSJ(buffer, data, size, newLine, isFtrMem);
        break;
    }

    // �t�@�C���I�[�v��
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_ErrorMessage(ALTID_ERROR, "File Open Error :", fileName, ret);
        goto FUNC_END;
    }

    // �t�@�C���ɏo�͂��A�N���[�Y����
    size = StrLen(buffer);
    if ((size % 2) != 0)
    {
        // �������ރf�[�^�T�C�Y�������ɒ�������
        size++;
    }

    len = 0;
    bufSize = size;
    ptr = buffer;
    while (bufSize != 0)
    {
        if ((NNsiParam->debugMessageON == 0)&&(bufSize > NNSH_WORKBUF_MAX))
        {
            size = NNSH_WORKBUF_MAX;
        }
        else
        {
            size = bufSize;
        }
        ret = WriteFile_NNsh(&fileRef, len, size, ptr, &writeSize);
        if (ret != errNone)
        {
            NNsh_ErrorMessage(ALTID_ERROR, "File Write Error :", fileName, ret);
            goto FUNC_END;
        }
        len = len + size;
        ptr = ptr + size;
        bufSize = bufSize - size;        
    }
    CloseFile_NNsh(&fileRef);    

    // BUSY�t�H�[�����B��
    Hide_BusyForm(true);

FUNC_END:
    // ���Ǝn���A�A�A
    if ((isFtrMem == true)&&(buffer != NULL))
    {
        FtrPtrFree(SOFT_CREATOR_ID, NNSH_WORKBUF_FTR);
    }
    else
    {
        MEMFREE_PTR(buffer);
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   DataLoadFromFile                                         */
/*                                                        �f�[�^�̓ǂݏo�� */
/*=========================================================================*/
Boolean DataLoadFromFile(FormType *frm, UInt16 fldId, Char *fileName, UInt16 loc, UInt16 kanji, UInt16 newLine)
{
    Err         ret;
    UInt16      fileMode; 
    UInt32      size, readSize;
    NNshFileRef fileRef;
    MemHandle   txtH, oldTxtH;
    FieldType  *fldP;
    Char       *txtP, *temp;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);
    
    // BUSY�t�H�[����\��
    Show_BusyForm("Loading...");

    // �ǂݏo���t���O��ݒ肷��
    if (loc == 0)
    {
        // Palm���ɋL�^����
        fileMode = NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
    }
    else
    {
        // VFS�ɋL�^����
        fileMode = NNSH_FILEMODE_READONLY;
    }

	// ���A�v������N������A�t�@�C�������w�肳��Ă��Ȃ��ꍇ...
	if ((fileName[StrLen(fileName) - 1] == '/')&&
        (NNsiGlobal->autoOpenFile != FILEMGR_STATE_DISABLED))
    {
        // ���������I������
        return (false);
    }

    // �t�@�C���I�[�v��
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_WARN, "File Open Error :", fileName, ret);
        return (false);
    }

    // �t�@�C���T�C�Y���m�F����
    readSize = 0;
    ret = GetFileSize_NNsh(&fileRef, &readSize);
    if (ret != errNone)
    {
        // �G���[�����A�I������
        NNsh_ErrorMessage(ALTID_ERROR, "unknown file size  :", fileName, ret);
        CloseFile_NNsh(&fileRef);
        return (false);
    }

    if (readSize > NNsiParam->bufferSize)
    {
        // �t�@�C���T�C�Y���傫�������A�ŏ��̕����̂ݓǂݏo���x����\������
        NNsh_WarningMessage(ALTID_WARN, fileName, " is too big, data is truncated.", readSize);
        readSize = NNsiParam->bufferSize;
    }

    // �ꎞ�I�ǂݍ��ݗ̈���m�ۂ���
    temp = MemPtrNew_NNsh(readSize + MARGIN);
    if (temp == NULL)
    {
        // �G���[�����A�I������
        NNsh_ErrorMessage(ALTID_ERROR, "buffer alloc. error  :", "", readSize);
        CloseFile_NNsh(&fileRef);
        return (false);
    }
    MemSet(temp, (readSize + MARGIN), 0x00);

    // �t�@�C������ǂݏo���ăN���[�Y����
    (void) ReadFile_NNsh(&fileRef, 0, readSize, temp, &size);
    (void) CloseFile_NNsh(&fileRef);

    // �t�B�[���h�ݒ�p�o�b�t�@�̗̈���m�ۂ���    
    readSize = NNsiParam->bufferSize;    
    txtH = MemHandleNew(readSize + MARGIN);
    if (txtH == 0)
    {
        // �̈�m�ێ��s,,,
        MEMFREE_PTR(temp);
        NNsh_ErrorMessage(ALTID_ERROR, "Memory allocation error ", " size :", readSize);
        CloseFile_NNsh(&fileRef);
        return (false);
    }

    txtP = (Char *) MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // �̈�̊m�ۂɎ��s
        MEMFREE_PTR(temp);
        MemHandleFree(txtH);
        NNsh_ErrorMessage(ALTID_ERROR, "Memory lock error ", " size :", readSize);
        CloseFile_NNsh(&fileRef);
        return (false);
    }
    MemSet(txtP, (readSize + MARGIN), 0x00);
    
    // �����R�[�h�ɂ��킹�āA�f�[�^��ϊ����A�t�B�[���h�ɔ��f������
    switch (kanji)
    {
      case NNSH_KANJI_EUC:
        // EUC�R�[�h
        StrNCopyEUCtoSJ(txtP, temp, readSize, newLine);
        break;

      case NNSH_KANJI_JIS:
        // JIS�R�[�h
        StrNCopyJIStoSJ(txtP, temp, readSize, newLine);
        break;

      case NNSH_KANJI_UTF8:
        // UTF-8(Unicode)�R�[�h
        StrNCopyUTF8toSJ(txtP, temp, readSize, newLine);
        break;


      case NNSH_KANJI_SHIFTJIS:
      default:
        // SHIFT JIS�R�[�h(���̂܂�)
        StrNCopySJtoSJLF(txtP, temp, readSize, newLine);
        break;
    }
    MEMFREE_PTR(temp);
    MemHandleUnlock(txtH);

    // BUSY�t�H�[�����B��
    Hide_BusyForm(true);
    
    // �t�B�[���h�̃I�u�W�F�N�g�|�C���^���擾
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldId));

    // �t�B�[���h�̃e�L�X�g�n���h�������ւ��A�Â��̂��폜����
    oldTxtH = FldGetTextHandle(fldP);
    FldSetTextHandle(fldP, NULL);
    FldSetTextHandle(fldP, txtH);
    if (oldTxtH != 0)
    {
        (void) MemHandleFree(oldTxtH);
    }

    // �t�B�[���h�̈ʒu���m�肷��
    

    // FrmDrawForm(frm);
    return (false);
}

/*=========================================================================*/
/*   Function :   Handler_NNshFiler                                        */
/*                                   �C�x���g�n���h���i�t�@�C��OPEN/CLOSE) */
/*=========================================================================*/
Boolean Handler_NNshFiler(EventType *event)
{
    Char    **fnameP, *txtP;
    UInt16    keyCode, cnt, max;
    FormType *frm;
    ListType *lstP;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);
    
    frm = FrmGetActiveForm();
    switch (event->eType)
    { 
      case keyDownEvent:
        // �L�[����
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          // �u���v(�W���O�_�C������)�����������̏���
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrNextField:
          case vchrThumbWheelDown:
          case chrDownArrow:
          case vchrPageDown:
            // �������Ȃ�
            lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
            max  = LstGetNumberOfItems(lstP);
            cnt  = LstGetSelection(lstP);
            if (cnt == noListSelection)
            {
                cnt = 0;
            }
            else if (cnt < (max - 1))
            {
                cnt++;
            }
            LstSetSelection(lstP, cnt);
            txtP = LstGetSelectionText(lstP, cnt);
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP, StrLen(txtP));
            break;

          // �u��v(�W���O�_�C������)�����������̏���
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
          case chrUpArrow:
          case vchrPageUp:
            // �������Ȃ�
            lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
            cnt  = LstGetSelection(lstP);
            if (cnt == noListSelection)
            {
                cnt = 0;
            }
            else if (cnt > 0)
            {
                cnt--;
            }
            LstSetSelection(lstP, cnt);
            txtP = LstGetSelectionText(lstP, cnt);
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP, StrLen(txtP));
            break;

          case vchrJogRelease:
          case vchrThumbWheelPush:
          case chrCarriageReturn:
          case chrLineFeed:
          case vchrJogLeft:
            // ���s�R�[�h�����͂��ꂽ�AOK�{�^�������������Ƃɂ���
            if ((StrLen(NNsiParam->fileName) == 0)||(NNsiParam->fileName[0] == ' '))
            {
                //  �t�@�C���������͂���Ă��Ȃ��A���̂Ƃ��͑I������Ă���
                // ���X�g�̃t�@�C�������g�p����
                lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm,LSTID_FILESELECTION));
                cnt = LstGetSelection(lstP);
                if (cnt == noListSelection)
                {
                    cnt = 0;
                }
                txtP = LstGetSelectionText(lstP, cnt);
                if (*txtP != ' ')
                {
                    StrCopy(NNsiParam->fileName, txtP);
                }
            }
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_OK)));
            break;

          case vchrJogBack:
          case chrEscape:
          case vchrThumbWheelBack:
          case vchrJogRight:
            // ESC�R�[�h�����͂��ꂽ�ACANCEL�{�^�������������Ƃɂ���
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_CANCEL)));
            break;

          case chrLeftArrow:         // �J�[�\���L�[��(5way��)
          case chrRightArrow:         // �J�[�\���L�[�E(5way�E)
          default:
            // �������Ȃ�
            break;
        }        
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ
        break;

      case popSelectEvent:
        // �|�b�v�A�b�v���I�����ꂽ
        if (event->data.popSelect.listID == LSTID_FILELOC)
        {
            // �t�@�C���ʒu���X�V�����A�t�@�C�����X�g���X�V����
            MemHandleUnlock(NNsiGlobal->fileListH);
            fnameP = MemHandleLock(NNsiGlobal->fileListH);
            for (cnt = 0; cnt < (MAX_FILELIST + MARGIN); cnt++)
            {
                txtP = (Char *) fnameP[cnt];
                MEMFREE_PTR(txtP);
            }
            MemSet(fnameP, sizeof(Char *) * (MAX_FILELIST + MARGIN), 0x00);
            setFileListItems(frm, fnameP, event->data.popSelect.selection);
            return (false);
        }
        break;

      case lstSelectEvent:
        // ���X�g��I��
        // (�t�@�C�������ɑI�������A�C�e����\������)
        MemHandleUnlock(NNsiGlobal->fileListH);
        fnameP = MemHandleLock(NNsiGlobal->fileListH);
        txtP = (Char *) fnameP[event->data.lstSelect.selection];
        if (*txtP != ' ')
        {
            MemSet(NNsiParam->fileName, (MAXLENGTH_FILENAME + MARGIN), 0x00);
            StrCopy(NNsiParam->fileName, txtP);        
            NNshWinSetFieldText(frm, FLDID_FILENAME, true, txtP,
                                StrLen(txtP));
        }
        break;

      case menuEvent:
      case sclRepeatEvent:
      case fldChangedEvent:
      default:
        // �������Ȃ� 
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   EffectData_NNshFiler                                     */
/*                                        �t�H�[������ݒ肳�ꂽ�����擾 */
/*=========================================================================*/
void EffectData_NNshFiler(FormType *frm, Char *fileName, UInt16 *loc, UInt16 *kanji, UInt16 *newLine)
{
    MemHandle  txtH;
    FieldType *fldP;
    Char      *txtP;
    
    // �t�@�C�������擾
    fldP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_FILENAME));
    txtH = FldGetTextHandle(fldP);
    if (txtH != 0)
    {
        txtP = MemHandleLock(txtH);
        if ((txtP != NULL)&&(*txtP != '\0'))
        {
            StrCopy(fileName, txtP);
        }
        MemHandleUnlock(txtH);
    }

    // �t�@�C���̒u���ꏊ
    *loc = LstGetSelection(FrmGetObjectPtr(frm, 
                                        FrmGetObjectIndex(frm, LSTID_FILELOC)));

    // �����R�[�h
    *kanji = LstGetSelection(FrmGetObjectPtr(frm, 
                                          FrmGetObjectIndex(frm, LSTID_KANJI)));

    // ���s�R�[�h
    *newLine = LstGetSelection(FrmGetObjectPtr(frm, 
                                        FrmGetObjectIndex(frm, LSTID_LINESEP)));

    return; 
}

/*-------------------------------------------------------------------------*/
/*   Function : setFileListItems                                           */
/*                                                �t�@�C���ꗗ�̃��X�g�ݒ� */
/*-------------------------------------------------------------------------*/
static void setFileListItems(FormType *frm, Char **fnameP, UInt16 loc)
{
    Err                  ret, err;
    UInt16               cardNo, cnt;
    LocalID              dbLocalID;
    DmSearchStateType    state;
    Char                 fileName[MINIBUF + MINIBUF + MARGIN], **ptr;
    FileInfoType         info;
    UInt32               dirIterator;
    FileRef              dirRef;
    ListType            *lstP;
    NNshWorkingInfo     *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // VFS�̎g�p�ݒ�ɂ��킹�ăt���O��ݒ�A�t�@�C�������ƁA�ꗗ�\��
    ptr = fnameP;
    cnt = 0;
    if (loc == NNSH_VFS_ENABLE)
    {
        // �f�B���N�g���̃I�[�v��
        MemSet(fileName, sizeof(fileName), 0x00);
        StrCopy(fileName, NNsiGlobal->dirName);
        ret = VFSFileOpen(NNsiGlobal->vfsVol, fileName, vfsModeRead, &dirRef);
        if (ret == errNone)
        {
            MemSet(&info, sizeof(info), 0x00);
            info.nameP      = fileName;
            info.nameBufLen = MINIBUF + MINIBUF;
            dirIterator     = vfsIteratorStart;

            while (dirIterator != vfsIteratorStop)
            {
                MemSet(fileName, sizeof(fileName), 0x00);

                if (VFSDirEntryEnumerate(dirRef, &dirIterator, &info) != errNone)
                {
                    // �f�B���N�g���ǂݏo���Ɏ��s�A������
                    break;
                }
                // �t�@�C�������̊m�F(�K�v�ł����)
                if ((info.attributes & vfsFileAttrDirectory) == vfsFileAttrDirectory)
                {
                    // �f�B���N�g���͖�������
                    continue;
                }
                if ((info.attributes & vfsFileAttrHidden) == vfsFileAttrHidden)
                {
                    // �B���t�@�C���͖�������
                    continue;
                }
                if ((info.attributes & vfsFileAttrSystem) == vfsFileAttrSystem)
                {
                    // �V�X�e���t�@�C���͖�������
                    continue;
                }
                if ((info.attributes & vfsFileAttrVolumeLabel) == vfsFileAttrVolumeLabel)
                {
                    // �{�����[�����x���͖�������
                    continue;
                }

                // �t�@�C���������X�g�A�C�e���ɃR�s�[
                if ((fnameP != NULL)&&(cnt < MAX_FILELIST))
                {
                    *ptr = MemPtrNew_NNsh(StrLen(fileName) + MARGIN);
                    if (*ptr != NULL)
                    {
                        MemSet(*ptr, (StrLen(fileName) + MARGIN), 0x00);
                        StrCopy(*ptr, fileName);
                        cnt++;
                        ptr++;
                    }
                }
            }
        }
        // �f�B���N�g�����N���[�Y
        VFSFileClose(dirRef);
    }
    else
    {
        // Palm�{�̓��ɂ���t�@�C���̈ꗗ���擾
        ret = DmGetNextDatabaseByTypeCreator(true, &state, '.txt', 'FLDB',
                                             false, &cardNo, &dbLocalID);
        while ((ret == errNone)&&(dbLocalID != 0))
        {
            // �t�@�C�������擾
            MemSet(fileName, sizeof(fileName), 0x00);
            err = DmDatabaseInfo(cardNo, dbLocalID, fileName,
                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL);
            if (err == errNone)
            {
                // �擾�����t�@�C���������X�g�A�C�e���ɃR�s�[
                if ((fnameP != NULL)&&(cnt < MAX_FILELIST))
                {
                    *ptr = MemPtrNew_NNsh(StrLen(fileName) + MARGIN);
                    if (*ptr != NULL)
                    {
                        MemSet(*ptr, (StrLen(fileName) + MARGIN), 0x00);
                        StrCopy(*ptr, fileName);
                        cnt++;
                        ptr++;
                    }
                }
            }
            // ���ڈȍ~�̃f�[�^�擾
            ret = DmGetNextDatabaseByTypeCreator(false, &state, '.txt', 'FLDB',
                                                 false, &cardNo, &dbLocalID);
        }
    }

    // �t�@�C�����P�����Ȃ��ꍇ�ɂ́A�X�y�[�X��\������
    if (cnt == 0)
    {
        *ptr = MemPtrNew_NNsh(StrLen("  ") + MARGIN);
        StrCopy(*ptr, "  ");
        cnt++;
        ptr++;
    }

    // �t�@�C���ꗗ�����X�g�ݒ�(�������A���I���ɂ���)
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FILESELECTION));   
    LstSetListChoices(lstP, fnameP, cnt);
    LstSetTopItem    (lstP, 0);
    LstSetSelection  (lstP, noListSelection);
    LstDrawList(lstP);

    return;
}


/*=========================================================================*/
/*   Function : OpenForm_NNshFiler                                         */
/*                                        �ݒ荀��(���[�_���t�H�[��)�̕\�� */
/*=========================================================================*/
Err OpenForm_NNshFiler(FormType *frm, UInt16 command, Char **fnameP)
{
	Char *ptr;
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    switch (command)
    {
      case BTNID_DATA_SAVE:
        // �f�[�^�̕ۊ�
         FrmCopyTitle(frm, "Save To ...");
        break;

      case BTNID_DATA_LOAD:
      default:
        // �f�[�^�̓ǂݏo��
         FrmCopyTitle(frm, "Read From ...");
        break;
    }

    // VFS���T�|�[�g���Ă��Ȃ��Ƃ��́A�\�����Ȃ�
    if (NNsiParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        // (�ݒ荀�ڎ��̂���ʂɕ\�����Ȃ�)
        FrmHideObject(frm, FrmGetObjectIndex(frm, LBLID_FILELOC));
        FrmHideObject(frm, FrmGetObjectIndex(frm, POPTRID_FILELOC));
        
        FrmHideObject(frm, FrmGetObjectIndex(frm, LBLID_KANJI));
        FrmHideObject(frm, FrmGetObjectIndex(frm, POPTRID_KANJI));

        FrmHideObject(frm, FrmGetObjectIndex(frm, LBLID_LINESEP));
        FrmHideObject(frm, FrmGetObjectIndex(frm, POPTRID_LINESEP));

        // �t�@�C�������X�g��ݒ肷��
        setFileListItems(frm, fnameP, NNSH_VFS_DISABLE);
        NNshWinSetPopItems(frm, POPTRID_FILELOC, LSTID_FILELOC, NNSH_VFS_DISABLE);        
    }
    else
    {
        // �t�@�C�������X�g��ݒ肷��
        setFileListItems(frm, fnameP, NNsiParam->fileLocation);
        NNshWinSetPopItems(frm, POPTRID_FILELOC, LSTID_FILELOC, NNsiParam->fileLocation);

        // �O��I�����������R�[�h��ݒ�
        NNshWinSetPopItems(frm, POPTRID_KANJI, LSTID_KANJI, NNsiParam->kanjiCode);

        // �O��I���������s�R�[�h��ݒ�
        NNshWinSetPopItems(frm, POPTRID_LINESEP, LSTID_LINESEP, NNsiParam->newLineCode);
    }

    // �t�@�C�������͗��ɑO����͂����t�@�C���������A�t�H�[�J�X��ݒ肷��
    if (StrLen(NNsiParam->fileName) != 0)
    {
		// �t�@�C���������\������悤��������
        ptr = NNsiParam->fileName + StrLen(NNsiParam->fileName) - 1;
		if (*ptr == '/')
		{
			// �t�@�C�������Ȃ������ꍇ�AKnEd.txt �𖖔��ɕt������
		    StrCat(NNsiParam->fileName, "KnEd.txt");
	        ptr = NNsiParam->fileName + StrLen(NNsiParam->fileName) - 1;
		}
        while (ptr > NNsiParam->fileName)
        {
            if (*ptr == '/')
            {
				ptr++;
				break;
            }
            ptr--;
        }
        NNshWinSetFieldText(frm, FLDID_FILENAME, true, ptr, StrLen(ptr));
    }
    FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_FILENAME));

    return (errNone);
}

/*=========================================================================*/
/*   Function :   NNsh_SetConfig                                           */
/*                                        �ݒ荀��(���[�_���t�H�[��)�̕\�� */
/*=========================================================================*/
Boolean NNsh_SetConfig(void)
{
    Boolean   ret;
    FormType *prevFrm, *diagFrm;

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �_�C�A���O�\���̏���
    PrepareDialog_NNsh(FRMID_CONFIG_NNSH, &diagFrm, false);
    if (diagFrm == NULL)
    {
        // �\���̏������s
        return (false);
    }

    // �ݒ菉���l�̔��f
    OpenForm_NNshSetting(diagFrm);
    
    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_NNshSetting);

    // ��ʕ\��
    (void) FrmDoDialog(diagFrm);
    
    // �\����ʂ����ɖ߂�
    PrologueDialog_NNsh(prevFrm, diagFrm);

    return (ret);
}

/*=========================================================================*/
/*   Function :   NNsh_ClearData                                           */
/*                                                         �f�[�^�̃N���A  */
/*=========================================================================*/
Boolean NNsh_ClearData(void)
{
    NNshSavedPref *NNsiParam;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    if (NNsh_ConfirmMessage(ALTID_CONFIRM, "Clear OK?", "", 0) == 0)
    {
        NNshWinSetFieldText(FrmGetActiveForm(), FLDID_SCREEN,
                                          true, "", NNsiParam->bufferSize);
        NNshWinViewUpdateScrollBar(FLDID_SCREEN, SCLID_SCREEN);

        // ���[�N�t�@�C�����폜����
        (void) DeleteFile_NNsh(FILE_TEMPFILE, NNSH_VFS_DISABLE);
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_FileData                                            */
/*                                                           �f�[�^�̕ۊ�  */
/*=========================================================================*/
Boolean NNsh_FileData(UInt16 command)
{
    Boolean   ret;
    Char     *fileName, *txtP, **area;
    UInt16    btnId, *loc, *kanji, *newLine, size, cnt;
    FormType *prevFrm, *diagFrm;
    MemHandle  txtH;
    FieldType *fldP;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);
    
	// ���A�v���P�[�V��������Ăяo����Ă����Ƃ��ɂ́Akaniedit���I��������
	if ((command != BTNID_DATA_SAVE)&&
	    (NNsiGlobal->autoOpenFile != FILEMGR_STATE_DISABLED))
	{
        // NNsi�I��
        MemSet(&(NNsiGlobal->dummyEvent), sizeof(EventType), 0x00);
        (NNsiGlobal->dummyEvent).eType = appStopEvent;
        EvtAddEventToQueue(&(NNsiGlobal->dummyEvent));
		return (false);
	}

    // �t�@�C�������O���[�o���̈悩��Ƃ��Ă���
    fileName = NNsiParam->fileName;
    kanji    = &(NNsiParam->kanjiCode);
    newLine  = &(NNsiParam->newLineCode);
    loc      = &(NNsiParam->fileLocation);    

    // ��ʂ�����������
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // �t�@�C�����X�g���N���A����
    MEMFREE_HANDLE(NNsiGlobal->fileListH);
    area = NULL;
    size = sizeof(Char *) * (MAX_FILELIST + MARGIN);
    NNsiGlobal->fileListH = MemHandleNew(size);
    if (NNsiGlobal->fileListH != 0)
    {
        area = MemHandleLock(NNsiGlobal->fileListH);
        MemSet(area, size, 0x00);
    }

    // �_�C�A���O�\���̏���
    PrepareDialog_NNsh(FRMID_FILE_SAVELOAD, &diagFrm, false);
    if (diagFrm == NULL)
    {
        // �\���̏������s
        return (false);
    }

    // �ݒ菉���l�̔��f
    OpenForm_NNshFiler(diagFrm, command, area);
    
    // �C�x���g�n���h���̐ݒ�
    FrmSetEventHandler(diagFrm, Handler_NNshFiler);

    // ��ʕ\��
    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_FILE_OK)
    {
        // �t�@�C�������w�肵�A�ۊǏ��������{
        EffectData_NNshFiler(diagFrm, fileName, loc, kanji, newLine);
    }   
    
    // �\����ʂ����ɖ߂�
    PrologueDialog_NNsh(prevFrm, diagFrm);
    FrmDrawForm(FrmGetActiveForm());

    if (area != NULL)
    {
        // �t�@�C�����X�g���N���A����
        for (cnt = 0; cnt < (MAX_FILELIST + MARGIN); cnt++)
        {
            txtP = (Char *) area[cnt];
            MEMFREE_PTR(txtP);
        }
        MemSet(area, sizeof(Char *) * (MAX_FILELIST + MARGIN), 0x00);
    }
    if (NNsiGlobal->fileListH != 0)
    {
        MemHandleUnlock(NNsiGlobal->fileListH);
        MEMFREE_HANDLE (NNsiGlobal->fileListH);
    }

    // �������ւ̕���
    if (btnId == BTNID_FILE_OK)
    {
        // �ۑ� or �ǂݍ��ݏ����̎��s        
        switch (command)
        {
          case BTNID_DATA_SAVE:
            // �o�͂���f�[�^�̃|�C���^���擾
            fldP = FrmGetObjectPtr(prevFrm, FrmGetObjectIndex(prevFrm, FLDID_SCREEN));
            txtH = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                txtP = MemHandleLock(txtH);
                if ((txtP != NULL))
                {
                    if (*txtP != '\0')
                    {
                        // �f�[�^�̕ۊ�
                        (void) DataSaveToFile(prevFrm, txtP, fileName, *loc, *kanji, *newLine);
                    }
                    else
                    {
                        // ���[�N�t�@�C�����폜����
                        (void) DeleteFile_NNsh(FILE_TEMPFILE, NNSH_VFS_DISABLE);
                    }
                }
                MemHandleUnlock(txtH);
            }
            else
            {
                NNsh_InformMessage(ALTID_WARN, "data nothing to write.", "", 0);
            }
            return (false);
            break;

         case BTNID_DATA_LOAD:
         default:
            // �f�[�^�̓ǂݏo��
            (void) DataLoadFromFile(prevFrm, FLDID_SCREEN, fileName, *loc, *kanji, *newLine);
            NNshWinViewUpdateScrollBar(FLDID_SCREEN, SCLID_SCREEN);
            FrmDrawForm(FrmGetActiveForm());
            return (false);
            break;
        }
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_MainSclEvt                                          */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainSclEvt(EventType *event)
{
    Int16   lines;

    lines = event->data.sclExit.newValue - event->data.sclExit.value;
    if (lines < 0)
    {
      (void) NNshWinViewPageScroll(FLDID_SCREEN, SCLID_SCREEN, -lines, winUp);
    }
    else if (lines > 0)
    {
      (void) NNshWinViewPageScroll(FLDID_SCREEN, SCLID_SCREEN, lines, winDown);
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_MainKeyEvt                                          */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainKeyEvt(EventType *event)
{
    // �L�[�R�[�h��ϊ��A
    switch (KeyConvertFiveWayToJogChara(event))
    {
      // �u���v(�W���O�_�C������)�����������̏���
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
        NNshWinViewPageScroll(FLDID_SCREEN, SCLID_SCREEN, 0, winDown); 
        return (true);
        break;

      // �u��v(�W���O�_�C������)�����������̏���
      case vchrPageUp:
      case vchrJogUp:
      case vchrTrgJogUp:
      case vchrThumbWheelUp:
      case vchrRockerUp:
        NNshWinViewPageScroll(FLDID_SCREEN, SCLID_SCREEN, 0, winUp); 
        return (true);
        break;

      case vchrJogBack:
      case vchrThumbWheelBack:
        // �W���O��Back�{�^���������ꂽ�Ƃ�(Load)
        return (NNsh_FileData(BTNID_DATA_LOAD));
        break;

      case vchrJogRelease:
      case vchrThumbWheelPush:
        // �W���O��Push�{�^���������ꂽ�Ƃ�(Save)
        return (NNsh_FileData(BTNID_DATA_SAVE));
        break;

      case vchrJogPush:
      case chrHorizontalTabulation:
      case vchrJogPushedUp:
      case vchrJogPushedDown:
      case chrLeftArrow:
      case chrRightArrow:
      default:
        break;
    }
    return (false);

}

/*=========================================================================*/
/*   Function :   NNsh_MainBtnEvt                                         */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainBtnEvt(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
        // Clear�{�^��(�������ݗ̈���N���A����)
      case BTNID_FIELD_CLEAR:
        return (NNsh_ClearData());
        break;

        // 'AA...' �{�^���������ꂽ�Ƃ�
      case BTNID_INSERT_AA:
        // AADA���N������
        LaunchResource_NNsh('DAcc','moAA','code',1000);
        break;

      case BTNID_DATA_SAVE:
      case BTNID_DATA_LOAD:
        // �f�[�^�̕ۊ�/�ǂݏo��
        return (NNsh_FileData(event->data.ctlSelect.controlID));
        break;

      default:
        // ��L�ȊO
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_MainMenuEvt                                         */
/*                                       ���C���t�H�[���̃C�x���g�n���h��  */
/*=========================================================================*/
Boolean NNsh_MainMenuEvt(EventType *event)
{
    NNshSavedPref   *NNsiParam;
	UInt16           savedParam;
    Boolean          ret;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    switch (event->data.menu.itemID)
    {
      case MNUID_NNSH_CONFIG:
        // NNsh�ݒ��ʂ��J��
        return (NNsh_SetConfig());
        break;
 
      case MNUID_NNSH_SAVE:
        // �f�[�^�ۑ�
        return (NNsh_FileData(BTNID_DATA_SAVE));
        break;

      case MNUID_NNSH_LOAD:
        // �f�[�^(����)�ǂݏo��
		savedParam = NNsiGlobal->autoOpenFile;
		NNsiGlobal->autoOpenFile = FILEMGR_STATE_DISABLED;
        ret = NNsh_FileData(BTNID_DATA_LOAD);
		NNsiGlobal->autoOpenFile = savedParam;
		return (ret);
        break;

      case MNUID_NNSH_CLEAR:
        // �\���f�[�^�N���A
        return (NNsh_ClearData());
        break;

      case MNUID_VERSION:
        // �o�[�W�����̕\��
        ShowVersion_NNsh();
        break;

      case MNUID_NNSH_FONTSET:
         // �t�H���g�ύX
         NNsiParam->currentFont = FontSelect(NNsiParam->currentFont);
         NNsh_InformMessage(ALTID_INFO, MSG_AFFECTS_NEXT_TIME, "", 0);
         break;

      case MNUID_EDIT_UNDO:
      case MNUID_EDIT_CUT:
      case MNUID_EDIT_COPY:
      case MNUID_EDIT_PASTE:
      case MNUID_SELECT_ALL:
        // �ҏW���j���[
        return (NNsh_MenuEvt_Edit(event));
        break;

      default:
        // �������Ȃ�
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Hander_MainForm                                          */
/*                                     NNsi�ݒ�n��(����)�C�x���g�n���h��  */
/*=========================================================================*/
Boolean Handler_MainForm(EventType *event)
{
    Boolean   handled;
    FormType *frm;

    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    switch (event->eType)
    { 
      case keyDownEvent:
        // �L�[����
        handled = NNsh_MainKeyEvt(event);
        if (handled == false)
        {
#if 0
            // �f�[�^�̓��͂��\���ǂ����`�F�b�N����
            if (checkFieldEnterAvailable(FLDID_SCREEN, event) == false)
            {
                // �f�[�^�̈�I�[�o�t���[�A��������������
                return (true);
            }
#endif
        }
        return (handled);
        break;

      case menuEvent:
        // ���j���[��I��
        return (NNsh_MainMenuEvt(event));
        break;

      case ctlSelectEvent:
        // �{�^���������ꂽ
        return (NNsh_MainBtnEvt(event));
        break;

      case sclRepeatEvent:
        // �X�N���[���o�[�X�V
        return (NNsh_MainSclEvt(event));
        break;

      case fldChangedEvent:
        // �t�B�[���h�ɕ�������
        NNshWinViewUpdateScrollBar(FLDID_SCREEN, SCLID_SCREEN);
        break;

#ifdef USE_HANDERA
      case displayExtentChangedEvent :
        if(NNsiGlobal->device == NNSH_DEVICE_HANDERA)
        {
            frm = FrmGetActiveForm();
            HandEraResizeForm(frm);
            FrmDrawForm(frm);
        }
        break;
#endif // #ifdef USE_HANDERA
#ifdef USE_PIN_DIA
      case winDisplayChangedEvent:
        if ((NNsiGlobal->device == NNSH_DEVICE_DIASUPPORT)||
            (NNsiGlobal->device == NNSH_DEVICE_GARMIN))
        {
            frm = FrmGetActiveForm();
            if (HandEraResizeForm(frm) == true)
            {
                // ���T�C�Y��̕`��
                FrmDrawForm(frm);
            }
        }
        break;
#endif
      case fldEnterEvent:
        // �t�B�[���h�Ƀf�[�^�����͂��ꂽ��
        if (checkFieldEnterAvailable(FLDID_SCREEN, event) == false)
        {
            // �f�[�^�̈�I�[�o�t���[�A��������������
            return (true);
        }
        break;

      case penDownEvent:
      default:
        return (false);
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   OpenForm_MainForm                                        */
/*                                                      MAIN�t�H�[�����J�� */
/*=========================================================================*/
Err OpenForm_MainForm(FormType *frm)
{
    UInt16       savedData;
    Char         buf[MARGIN];
    FieldType   *fldP;
    ControlType *ctlObj;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // �t�B�[���h�̃I�u�W�F�N�g�|�C���^���擾
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_SCREEN));

    // �O��̃f�[�^/�w�肳�ꂽ�t�@�C����ǂݍ���
    savedData = NNsiParam->confirmationDisable;
    NNsiParam->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // �t�@�C�����J���ݒ�
    switch (NNsiGlobal->autoOpenFile)
    {
      case FILEMGR_STATE_OPENED_STREAM:
        // FileStream
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 0, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS:
        // Palm�̃e�L�X�g
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS_DOS:
        // ��ʓI��DOS�̃t�@�C��
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_CRLF);
        break;

      case FILEMGR_STATE_OPENED_VFS_MAC:
        // MAC(?)�̃t�@�C��
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_CR);
        break;

      case FILEMGR_STATE_OPENED_VFS_JIS:
        // JIS�t�@�C��
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_JIS, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS_EUC:
        // EUC�t�@�C��
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_EUC, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS_UTF8:
        // UTF-8�t�@�C��
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_UTF8, NNSH_LINECODE_CRLF);
        break;

      case FILEMGR_STATE_OPENED_STREAM_READONLY:
      case FILEMGR_STATE_OPENED_VFS_READONLY:
      case FILEMGR_STATE_DISABLED:
      default:
        // 
        if (NNsiParam->autoRestore != 0)
        {
            DataLoadFromFile(frm,FLDID_SCREEN, FILE_TEMPFILE, 0, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
        }
        else
        {
            // �_�~�[�̃f�[�^�o�b�t�@��\��t����
            MemSet(buf, sizeof(buf), 0x00);
            NNshWinSetFieldText(frm, FLDID_SCREEN, false, buf, NNsiParam->bufferSize);
        }
        break;
    }
    NNsiParam->confirmationDisable = savedData;

    // �ő�l�� UInt16 �̃T�C�Y�Ɏ��܂邩�ǂ������`�F�b�N
    if (NNsiParam->bufferSize > 0xffe0)
    {
        savedData = 0xffe0;
    }
    else
    {
        savedData = NNsiParam->bufferSize;
    }
    FldSetMaxChars(fldP, savedData);

    // AADA���C���X�g�[������Ă��邩�m�F����
    if (CheckInstalledResource_NNsh('DAcc', 'moAA') == false)
    {
        // AADA���C���X�g�[������Ă��Ȃ��ꍇ�ɂ́AAA�}���{�^�����B��
        ctlObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_INSERT_AA));
        CtlSetEnabled(ctlObj, false);
        CtlSetUsable (ctlObj, false);
        FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_INSERT_AA));
        FrmEraseForm(frm);
        FrmDrawForm (frm);
    }

	// ���A�v���P�[�V��������N������Ă����ꍇ�ɂ́ARead�{�^����End�{�^����
    if (NNsiGlobal->autoOpenFile != FILEMGR_STATE_DISABLED)
    {
	    // Read�{�^���̃��x����End�ɕύX����
    	ctlObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_DATA_LOAD));
	    CtlSetLabel(ctlObj, "End");
    }

    // �t�H�[�J�X�����b�Z�[�W�t�B�[���h�ɐݒ�(�p�����[�^���ݒ肳��ĂȂ��ꍇ)
    if (NNsiParam->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_SCREEN));
        FldSetInsPtPosition(fldP, NNsiParam->bufferSize);
    }

    // �X�N���[���o�[���X�V
    NNshWinViewUpdateScrollBar(FLDID_SCREEN, SCLID_SCREEN);

    return (errNone);
}
