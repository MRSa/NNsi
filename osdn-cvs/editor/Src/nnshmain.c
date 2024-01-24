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
/*                        フィールドに文字データが格納できるかチェックする */
/*                         (最大文字列長格納されている領域にデータを入力   */
/*                          しようとした場合、Fatal Errorが出るのを抑止)   */
/*                         ※ 返り値がtrueなら入力可、falseなら入力不可 ※ */
/*=========================================================================*/
Boolean checkFieldEnterAvailable(UInt16 fldId, EventType *event)
{
    FormType  *frmP;
    FieldType *fldP;
    UInt32     len, maxChar;

    // フィールドのポインタを取得
    frmP = FrmGetActiveForm();
    fldP = FrmGetObjectPtr(frmP, FrmGetObjectIndex(frmP, fldId));

    // フィールドのテキストデータ長を取得
    maxChar  = FldGetMaxChars(fldP); 
    len      = FldGetTextLength(fldP);
    if (len >= maxChar)
    {
        // 入力可能データサイズを超えていた場合、何も処理しないようにする
        return (false);
    }
    return (true);
}

/*=========================================================================*/
/*   Function :   DataSaveToFile                                           */
/*                                                            データの保管 */
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

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // 拡張子(.txt)がついていなければ付ける
    if (StrStr(fileName, DATAFILE_SUFFIX) == NULL)
    {
        StrCat(fileName, DATAFILE_SUFFIX);
    }

    if (NNsh_ConfirmMessage(ALTID_CONFIRM, fileName, MSG_WARN_OUTPUT, loc) != 0)
    {
        // 記録を中止する、、、
        NNsh_InformMessage(ALTID_INFO, MSG_SAVE_ABORT, "", 0);
        return (false);
    }

    // ファイルが存在するか確認する
    if (loc == 0)
    {
        // Palm内
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
            //  ファイルが存在して、バックアップファイルでない場合、
            // あらかじめバックアップファイルを作成する
            if (StrCompare(fileName, FILE_TEMPFILE) != 0)
            {
                MemSet(tempFile, sizeof(tempFile), 0x00);
                StrCopy(tempFile, fileName);
                StrCat(tempFile, BACKFILE_SUFFIX);
                CopyFile_NNsh(tempFile, fileName, tempMode);
            }
        }
    }

    // 記録フラグを設定する
    if (loc == 0)
    {
        // Palm内に記録する
        fileMode = NNSH_FILEMODE_CREATE | NNSH_FILEMODE_TEMPFILE;
    }
    else
    {
        // VFSに記録する
        fileMode = NNSH_FILEMODE_CREATE;
    }

    // データバッファを確保する
    size = StrLen(data);
    size = size * 2 + MARGIN + 2;
    if (size > NNsiParam->bufferSize)
    {
        // Featureメモリを使用する
        ret = FtrPtrNew(SOFT_CREATOR_ID, NNSH_WORKBUF_FTR, size, (void **) &buffer);
        if (ret != errNone)
        {
            buffer = NULL;
        }
        isFtrMem = true;
    }
    else
    {
        // 通常メモリを使用する
        buffer = MemPtrNew_NNsh(size);
        isFtrMem = false;
     }
    if (buffer == NULL)
    {
        // 記録を中止する
        NNsh_InformMessage(ALTID_INFO, MSG_SAVE_ABORT, MSG_CANNOT_ALLOC, 0);
        return (false);
    }

    // BUSYフォームを表示
    Show_BusyForm("Saving...");

    // メモリクリア
    if (isFtrMem == true)
    {
        DmSet(buffer, 0, size, 0x00);
    }
    else
    {
        MemSet(buffer, size, 0x00);
    }

    // 漢字コード＋改行コードを考慮してコピーする
    switch (kanji)
    {
      case NNSH_KANJI_EUC:
        // EUCコード
        StrNCopySJtoEUC(buffer, data, size, newLine, isFtrMem);
        break;

      case NNSH_KANJI_JIS:
        // JISコード
        StrNCopySJtoJIS(buffer, data, size, newLine, isFtrMem);
        break;

      case NNSH_KANJI_UTF8:
        // UTF-8(Unicode)コード
        StrNCopySJtoUTF8(buffer, data, size, newLine, isFtrMem);
        break;

      case NNSH_KANJI_SHIFTJIS:
      default:
        // SHIFT JISコード
        StrNCopySJtoSJ(buffer, data, size, newLine, isFtrMem);
        break;
    }

    // ファイルオープン
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_ErrorMessage(ALTID_ERROR, "File Open Error :", fileName, ret);
        goto FUNC_END;
    }

    // ファイルに出力し、クローズする
    size = StrLen(buffer);
    if ((size % 2) != 0)
    {
        // 書き込むデータサイズを偶数に調整する
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

    // BUSYフォームを隠す
    Hide_BusyForm(true);

FUNC_END:
    // あと始末、、、
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
/*                                                        データの読み出し */
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

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);
    
    // BUSYフォームを表示
    Show_BusyForm("Loading...");

    // 読み出しフラグを設定する
    if (loc == 0)
    {
        // Palm内に記録する
        fileMode = NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
    }
    else
    {
        // VFSに記録する
        fileMode = NNSH_FILEMODE_READONLY;
    }

	// 他アプリから起動され、ファイル名が指定されていない場合...
	if ((fileName[StrLen(fileName) - 1] == '/')&&
        (NNsiGlobal->autoOpenFile != FILEMGR_STATE_DISABLED))
    {
        // 何もせず終了する
        return (false);
    }

    // ファイルオープン
    ret = OpenFile_NNsh(fileName, fileMode, &fileRef);
    if (ret != errNone)
    {
        NNsh_InformMessage(ALTID_WARN, "File Open Error :", fileName, ret);
        return (false);
    }

    // ファイルサイズを確認する
    readSize = 0;
    ret = GetFileSize_NNsh(&fileRef, &readSize);
    if (ret != errNone)
    {
        // エラー発生、終了する
        NNsh_ErrorMessage(ALTID_ERROR, "unknown file size  :", fileName, ret);
        CloseFile_NNsh(&fileRef);
        return (false);
    }

    if (readSize > NNsiParam->bufferSize)
    {
        // ファイルサイズが大きすぎた、最初の部分のみ読み出す警告を表示する
        NNsh_WarningMessage(ALTID_WARN, fileName, " is too big, data is truncated.", readSize);
        readSize = NNsiParam->bufferSize;
    }

    // 一時的読み込み領域を確保する
    temp = MemPtrNew_NNsh(readSize + MARGIN);
    if (temp == NULL)
    {
        // エラー発生、終了する
        NNsh_ErrorMessage(ALTID_ERROR, "buffer alloc. error  :", "", readSize);
        CloseFile_NNsh(&fileRef);
        return (false);
    }
    MemSet(temp, (readSize + MARGIN), 0x00);

    // ファイルから読み出してクローズする
    (void) ReadFile_NNsh(&fileRef, 0, readSize, temp, &size);
    (void) CloseFile_NNsh(&fileRef);

    // フィールド設定用バッファの領域を確保する    
    readSize = NNsiParam->bufferSize;    
    txtH = MemHandleNew(readSize + MARGIN);
    if (txtH == 0)
    {
        // 領域確保失敗,,,
        MEMFREE_PTR(temp);
        NNsh_ErrorMessage(ALTID_ERROR, "Memory allocation error ", " size :", readSize);
        CloseFile_NNsh(&fileRef);
        return (false);
    }

    txtP = (Char *) MemHandleLock(txtH);
    if (txtP == NULL)
    {
        // 領域の確保に失敗
        MEMFREE_PTR(temp);
        MemHandleFree(txtH);
        NNsh_ErrorMessage(ALTID_ERROR, "Memory lock error ", " size :", readSize);
        CloseFile_NNsh(&fileRef);
        return (false);
    }
    MemSet(txtP, (readSize + MARGIN), 0x00);
    
    // 漢字コードにあわせて、データを変換し、フィールドに反映させる
    switch (kanji)
    {
      case NNSH_KANJI_EUC:
        // EUCコード
        StrNCopyEUCtoSJ(txtP, temp, readSize, newLine);
        break;

      case NNSH_KANJI_JIS:
        // JISコード
        StrNCopyJIStoSJ(txtP, temp, readSize, newLine);
        break;

      case NNSH_KANJI_UTF8:
        // UTF-8(Unicode)コード
        StrNCopyUTF8toSJ(txtP, temp, readSize, newLine);
        break;


      case NNSH_KANJI_SHIFTJIS:
      default:
        // SHIFT JISコード(そのまま)
        StrNCopySJtoSJLF(txtP, temp, readSize, newLine);
        break;
    }
    MEMFREE_PTR(temp);
    MemHandleUnlock(txtH);

    // BUSYフォームを隠す
    Hide_BusyForm(true);
    
    // フィールドのオブジェクトポインタを取得
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, fldId));

    // フィールドのテキストハンドルを入れ替え、古いのを削除する
    oldTxtH = FldGetTextHandle(fldP);
    FldSetTextHandle(fldP, NULL);
    FldSetTextHandle(fldP, txtH);
    if (oldTxtH != 0)
    {
        (void) MemHandleFree(oldTxtH);
    }

    // フィールドの位置を確定する
    

    // FrmDrawForm(frm);
    return (false);
}

/*=========================================================================*/
/*   Function :   Handler_NNshFiler                                        */
/*                                   イベントハンドラ（ファイルOPEN/CLOSE) */
/*=========================================================================*/
Boolean Handler_NNshFiler(EventType *event)
{
    Char    **fnameP, *txtP;
    UInt16    keyCode, cnt, max;
    FormType *frm;
    ListType *lstP;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);
    
    frm = FrmGetActiveForm();
    switch (event->eType)
    { 
      case keyDownEvent:
        // キー入力
        keyCode = KeyConvertFiveWayToJogChara(event);
        switch (keyCode)
        {
          // 「下」(ジョグダイヤル下)を押した時の処理
          case vchrJogDown:
          case vchrTrgJogDown:
          case vchrNextField:
          case vchrThumbWheelDown:
          case chrDownArrow:
          case vchrPageDown:
            // 何もしない
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

          // 「上」(ジョグダイヤル上)を押した時の処理
          case vchrJogUp:
          case vchrTrgJogUp:
          case vchrPrevField:
          case vchrThumbWheelUp:
          case chrUpArrow:
          case vchrPageUp:
            // 何もしない
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
            // 改行コードが入力された、OKボタンを押したことにする
            if ((StrLen(NNsiParam->fileName) == 0)||(NNsiParam->fileName[0] == ' '))
            {
                //  ファイル名が入力されていない、このときは選択されている
                // リストのファイル名を使用する
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
            // ESCコードが入力された、CANCELボタンを押したことにする
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                      FrmGetObjectIndex(FrmGetActiveForm(),
                                                        BTNID_FILE_CANCEL)));
            break;

          case chrLeftArrow:         // カーソルキー左(5way左)
          case chrRightArrow:         // カーソルキー右(5way右)
          default:
            // 何もしない
            break;
        }        
        break;

      case ctlSelectEvent:
        // ボタンが押された
        break;

      case popSelectEvent:
        // ポップアップが選択された
        if (event->data.popSelect.listID == LSTID_FILELOC)
        {
            // ファイル位置を更新した、ファイルリストを更新する
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
        // リストを選択
        // (ファイル名欄に選択したアイテムを表示する)
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
        // 何もしない 
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   EffectData_NNshFiler                                     */
/*                                        フォームから設定された情報を取得 */
/*=========================================================================*/
void EffectData_NNshFiler(FormType *frm, Char *fileName, UInt16 *loc, UInt16 *kanji, UInt16 *newLine)
{
    MemHandle  txtH;
    FieldType *fldP;
    Char      *txtP;
    
    // ファイル名を取得
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

    // ファイルの置き場所
    *loc = LstGetSelection(FrmGetObjectPtr(frm, 
                                        FrmGetObjectIndex(frm, LSTID_FILELOC)));

    // 漢字コード
    *kanji = LstGetSelection(FrmGetObjectPtr(frm, 
                                          FrmGetObjectIndex(frm, LSTID_KANJI)));

    // 改行コード
    *newLine = LstGetSelection(FrmGetObjectPtr(frm, 
                                        FrmGetObjectIndex(frm, LSTID_LINESEP)));

    return; 
}

/*-------------------------------------------------------------------------*/
/*   Function : setFileListItems                                           */
/*                                                ファイル一覧のリスト設定 */
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

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // VFSの使用設定にあわせてフラグを設定、ファイル検索と、一覧表示
    ptr = fnameP;
    cnt = 0;
    if (loc == NNSH_VFS_ENABLE)
    {
        // ディレクトリのオープン
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
                    // ディレクトリ読み出しに失敗、抜ける
                    break;
                }
                // ファイル属性の確認(必要であれば)
                if ((info.attributes & vfsFileAttrDirectory) == vfsFileAttrDirectory)
                {
                    // ディレクトリは無視する
                    continue;
                }
                if ((info.attributes & vfsFileAttrHidden) == vfsFileAttrHidden)
                {
                    // 隠しファイルは無視する
                    continue;
                }
                if ((info.attributes & vfsFileAttrSystem) == vfsFileAttrSystem)
                {
                    // システムファイルは無視する
                    continue;
                }
                if ((info.attributes & vfsFileAttrVolumeLabel) == vfsFileAttrVolumeLabel)
                {
                    // ボリュームラベルは無視する
                    continue;
                }

                // ファイル名をリストアイテムにコピー
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
        // ディレクトリをクローズ
        VFSFileClose(dirRef);
    }
    else
    {
        // Palm本体内にあるファイルの一覧を取得
        ret = DmGetNextDatabaseByTypeCreator(true, &state, '.txt', 'FLDB',
                                             false, &cardNo, &dbLocalID);
        while ((ret == errNone)&&(dbLocalID != 0))
        {
            // ファイル名を取得
            MemSet(fileName, sizeof(fileName), 0x00);
            err = DmDatabaseInfo(cardNo, dbLocalID, fileName,
                                 NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                 NULL, NULL, NULL);
            if (err == errNone)
            {
                // 取得したファイル名をリストアイテムにコピー
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
            // 二回目以降のデータ取得
            ret = DmGetNextDatabaseByTypeCreator(false, &state, '.txt', 'FLDB',
                                                 false, &cardNo, &dbLocalID);
        }
    }

    // ファイルが１件もない場合には、スペースを表示する
    if (cnt == 0)
    {
        *ptr = MemPtrNew_NNsh(StrLen("  ") + MARGIN);
        StrCopy(*ptr, "  ");
        cnt++;
        ptr++;
    }

    // ファイル一覧をリスト設定(ただし、無選択にする)
    lstP = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, LSTID_FILESELECTION));   
    LstSetListChoices(lstP, fnameP, cnt);
    LstSetTopItem    (lstP, 0);
    LstSetSelection  (lstP, noListSelection);
    LstDrawList(lstP);

    return;
}


/*=========================================================================*/
/*   Function : OpenForm_NNshFiler                                         */
/*                                        設定項目(モーダルフォーム)の表示 */
/*=========================================================================*/
Err OpenForm_NNshFiler(FormType *frm, UInt16 command, Char **fnameP)
{
	Char *ptr;
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    switch (command)
    {
      case BTNID_DATA_SAVE:
        // データの保管
         FrmCopyTitle(frm, "Save To ...");
        break;

      case BTNID_DATA_LOAD:
      default:
        // データの読み出し
         FrmCopyTitle(frm, "Read From ...");
        break;
    }

    // VFSをサポートしていないときは、表示しない
    if (NNsiParam->useVFS == NNSH_NOTSUPPORT_VFS)
    {
        // (設定項目自体を画面に表示しない)
        FrmHideObject(frm, FrmGetObjectIndex(frm, LBLID_FILELOC));
        FrmHideObject(frm, FrmGetObjectIndex(frm, POPTRID_FILELOC));
        
        FrmHideObject(frm, FrmGetObjectIndex(frm, LBLID_KANJI));
        FrmHideObject(frm, FrmGetObjectIndex(frm, POPTRID_KANJI));

        FrmHideObject(frm, FrmGetObjectIndex(frm, LBLID_LINESEP));
        FrmHideObject(frm, FrmGetObjectIndex(frm, POPTRID_LINESEP));

        // ファイル名リストを設定する
        setFileListItems(frm, fnameP, NNSH_VFS_DISABLE);
        NNshWinSetPopItems(frm, POPTRID_FILELOC, LSTID_FILELOC, NNSH_VFS_DISABLE);        
    }
    else
    {
        // ファイル名リストを設定する
        setFileListItems(frm, fnameP, NNsiParam->fileLocation);
        NNshWinSetPopItems(frm, POPTRID_FILELOC, LSTID_FILELOC, NNsiParam->fileLocation);

        // 前回選択した漢字コードを設定
        NNshWinSetPopItems(frm, POPTRID_KANJI, LSTID_KANJI, NNsiParam->kanjiCode);

        // 前回選択した改行コードを設定
        NNshWinSetPopItems(frm, POPTRID_LINESEP, LSTID_LINESEP, NNsiParam->newLineCode);
    }

    // ファイル名入力欄に前回入力したファイル名を入れ、フォーカスを設定する
    if (StrLen(NNsiParam->fileName) != 0)
    {
		// ファイル名だけ表示するよう調整する
        ptr = NNsiParam->fileName + StrLen(NNsiParam->fileName) - 1;
		if (*ptr == '/')
		{
			// ファイル名がなかった場合、KnEd.txt を末尾に付加する
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
/*                                        設定項目(モーダルフォーム)の表示 */
/*=========================================================================*/
Boolean NNsh_SetConfig(void)
{
    Boolean   ret;
    FormType *prevFrm, *diagFrm;

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // ダイアログ表示の準備
    PrepareDialog_NNsh(FRMID_CONFIG_NNSH, &diagFrm, false);
    if (diagFrm == NULL)
    {
        // 表示の準備失敗
        return (false);
    }

    // 設定初期値の反映
    OpenForm_NNshSetting(diagFrm);
    
    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_NNshSetting);

    // 画面表示
    (void) FrmDoDialog(diagFrm);
    
    // 表示画面を元に戻す
    PrologueDialog_NNsh(prevFrm, diagFrm);

    return (ret);
}

/*=========================================================================*/
/*   Function :   NNsh_ClearData                                           */
/*                                                         データのクリア  */
/*=========================================================================*/
Boolean NNsh_ClearData(void)
{
    NNshSavedPref *NNsiParam;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    if (NNsh_ConfirmMessage(ALTID_CONFIRM, "Clear OK?", "", 0) == 0)
    {
        NNshWinSetFieldText(FrmGetActiveForm(), FLDID_SCREEN,
                                          true, "", NNsiParam->bufferSize);
        NNshWinViewUpdateScrollBar(FLDID_SCREEN, SCLID_SCREEN);

        // ワークファイルも削除する
        (void) DeleteFile_NNsh(FILE_TEMPFILE, NNSH_VFS_DISABLE);
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_FileData                                            */
/*                                                           データの保管  */
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

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);
    
	// 他アプリケーションから呼び出されていたときには、kanieditを終了させる
	if ((command != BTNID_DATA_SAVE)&&
	    (NNsiGlobal->autoOpenFile != FILEMGR_STATE_DISABLED))
	{
        // NNsi終了
        MemSet(&(NNsiGlobal->dummyEvent), sizeof(EventType), 0x00);
        (NNsiGlobal->dummyEvent).eType = appStopEvent;
        EvtAddEventToQueue(&(NNsiGlobal->dummyEvent));
		return (false);
	}

    // ファイル名をグローバル領域からとってくる
    fileName = NNsiParam->fileName;
    kanji    = &(NNsiParam->kanjiCode);
    newLine  = &(NNsiParam->newLineCode);
    loc      = &(NNsiParam->fileLocation);    

    // 画面を初期化する
    ret     = false;
    prevFrm = FrmGetActiveForm();

    // ファイルリストをクリアする
    MEMFREE_HANDLE(NNsiGlobal->fileListH);
    area = NULL;
    size = sizeof(Char *) * (MAX_FILELIST + MARGIN);
    NNsiGlobal->fileListH = MemHandleNew(size);
    if (NNsiGlobal->fileListH != 0)
    {
        area = MemHandleLock(NNsiGlobal->fileListH);
        MemSet(area, size, 0x00);
    }

    // ダイアログ表示の準備
    PrepareDialog_NNsh(FRMID_FILE_SAVELOAD, &diagFrm, false);
    if (diagFrm == NULL)
    {
        // 表示の準備失敗
        return (false);
    }

    // 設定初期値の反映
    OpenForm_NNshFiler(diagFrm, command, area);
    
    // イベントハンドラの設定
    FrmSetEventHandler(diagFrm, Handler_NNshFiler);

    // 画面表示
    btnId = FrmDoDialog(diagFrm);
    if (btnId == BTNID_FILE_OK)
    {
        // ファイル名を指定し、保管処理を実施
        EffectData_NNshFiler(diagFrm, fileName, loc, kanji, newLine);
    }   
    
    // 表示画面を元に戻す
    PrologueDialog_NNsh(prevFrm, diagFrm);
    FrmDrawForm(FrmGetActiveForm());

    if (area != NULL)
    {
        // ファイルリストをクリアする
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

    // 実処理への分岐
    if (btnId == BTNID_FILE_OK)
    {
        // 保存 or 読み込み処理の実行        
        switch (command)
        {
          case BTNID_DATA_SAVE:
            // 出力するデータのポインタを取得
            fldP = FrmGetObjectPtr(prevFrm, FrmGetObjectIndex(prevFrm, FLDID_SCREEN));
            txtH = FldGetTextHandle(fldP);
            if (txtH != 0)
            {
                txtP = MemHandleLock(txtH);
                if ((txtP != NULL))
                {
                    if (*txtP != '\0')
                    {
                        // データの保管
                        (void) DataSaveToFile(prevFrm, txtP, fileName, *loc, *kanji, *newLine);
                    }
                    else
                    {
                        // ワークファイルを削除する
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
            // データの読み出し
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
/*                                       メインフォームのイベントハンドラ  */
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
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_MainKeyEvt(EventType *event)
{
    // キーコードを変換、
    switch (KeyConvertFiveWayToJogChara(event))
    {
      // 「下」(ジョグダイヤル下)を押した時の処理
      case vchrPageDown:
      case vchrJogDown:
      case vchrTrgJogDown:
      case vchrThumbWheelDown:
      case vchrRockerDown:
        NNshWinViewPageScroll(FLDID_SCREEN, SCLID_SCREEN, 0, winDown); 
        return (true);
        break;

      // 「上」(ジョグダイヤル上)を押した時の処理
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
        // ジョグのBackボタンが押されたとき(Load)
        return (NNsh_FileData(BTNID_DATA_LOAD));
        break;

      case vchrJogRelease:
      case vchrThumbWheelPush:
        // ジョグのPushボタンが押されたとき(Save)
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
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_MainBtnEvt(EventType *event)
{
    switch (event->data.ctlSelect.controlID)
    {
        // Clearボタン(書き込み領域をクリアする)
      case BTNID_FIELD_CLEAR:
        return (NNsh_ClearData());
        break;

        // 'AA...' ボタンが押されたとき
      case BTNID_INSERT_AA:
        // AADAを起動する
        LaunchResource_NNsh('DAcc','moAA','code',1000);
        break;

      case BTNID_DATA_SAVE:
      case BTNID_DATA_LOAD:
        // データの保管/読み出し
        return (NNsh_FileData(event->data.ctlSelect.controlID));
        break;

      default:
        // 上記以外
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   NNsh_MainMenuEvt                                         */
/*                                       メインフォームのイベントハンドラ  */
/*=========================================================================*/
Boolean NNsh_MainMenuEvt(EventType *event)
{
    NNshSavedPref   *NNsiParam;
	UInt16           savedParam;
    Boolean          ret;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    switch (event->data.menu.itemID)
    {
      case MNUID_NNSH_CONFIG:
        // NNsh設定画面を開く
        return (NNsh_SetConfig());
        break;
 
      case MNUID_NNSH_SAVE:
        // データ保存
        return (NNsh_FileData(BTNID_DATA_SAVE));
        break;

      case MNUID_NNSH_LOAD:
        // データ(強制)読み出し
		savedParam = NNsiGlobal->autoOpenFile;
		NNsiGlobal->autoOpenFile = FILEMGR_STATE_DISABLED;
        ret = NNsh_FileData(BTNID_DATA_LOAD);
		NNsiGlobal->autoOpenFile = savedParam;
		return (ret);
        break;

      case MNUID_NNSH_CLEAR:
        // 表示データクリア
        return (NNsh_ClearData());
        break;

      case MNUID_VERSION:
        // バージョンの表示
        ShowVersion_NNsh();
        break;

      case MNUID_NNSH_FONTSET:
         // フォント変更
         NNsiParam->currentFont = FontSelect(NNsiParam->currentFont);
         NNsh_InformMessage(ALTID_INFO, MSG_AFFECTS_NEXT_TIME, "", 0);
         break;

      case MNUID_EDIT_UNDO:
      case MNUID_EDIT_CUT:
      case MNUID_EDIT_COPY:
      case MNUID_EDIT_PASTE:
      case MNUID_SELECT_ALL:
        // 編集メニュー
        return (NNsh_MenuEvt_Edit(event));
        break;

      default:
        // 何もしない
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function :   Hander_MainForm                                          */
/*                                     NNsi設定系の(総合)イベントハンドラ  */
/*=========================================================================*/
Boolean Handler_MainForm(EventType *event)
{
    Boolean   handled;
    FormType *frm;

    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    switch (event->eType)
    { 
      case keyDownEvent:
        // キー入力
        handled = NNsh_MainKeyEvt(event);
        if (handled == false)
        {
#if 0
            // データの入力が可能かどうかチェックする
            if (checkFieldEnterAvailable(FLDID_SCREEN, event) == false)
            {
                // データ領域オーバフロー、何もせず抜ける
                return (true);
            }
#endif
        }
        return (handled);
        break;

      case menuEvent:
        // メニューを選択
        return (NNsh_MainMenuEvt(event));
        break;

      case ctlSelectEvent:
        // ボタンが押された
        return (NNsh_MainBtnEvt(event));
        break;

      case sclRepeatEvent:
        // スクロールバー更新
        return (NNsh_MainSclEvt(event));
        break;

      case fldChangedEvent:
        // フィールドに文字入力
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
                // リサイズ後の描画
                FrmDrawForm(frm);
            }
        }
        break;
#endif
      case fldEnterEvent:
        // フィールドにデータが入力された時
        if (checkFieldEnterAvailable(FLDID_SCREEN, event) == false)
        {
            // データ領域オーバフロー、何もせず抜ける
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
/*                                                      MAINフォームを開く */
/*=========================================================================*/
Err OpenForm_MainForm(FormType *frm)
{
    UInt16       savedData;
    Char         buf[MARGIN];
    FieldType   *fldP;
    ControlType *ctlObj;
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // パラメータ領域の取得
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

    // フィールドのオブジェクトポインタを取得
    fldP = (FieldType *) FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, FLDID_SCREEN));

    // 前回のデータ/指定されたファイルを読み込む
    savedData = NNsiParam->confirmationDisable;
    NNsiParam->confirmationDisable = NNSH_OMITDIALOG_ALLOMIT;

    // ファイルを開く設定
    switch (NNsiGlobal->autoOpenFile)
    {
      case FILEMGR_STATE_OPENED_STREAM:
        // FileStream
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 0, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS:
        // Palmのテキスト
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS_DOS:
        // 一般的なDOSのファイル
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_CRLF);
        break;

      case FILEMGR_STATE_OPENED_VFS_MAC:
        // MAC(?)のファイル
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_SHIFTJIS, NNSH_LINECODE_CR);
        break;

      case FILEMGR_STATE_OPENED_VFS_JIS:
        // JISファイル
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_JIS, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS_EUC:
        // EUCファイル
        DataLoadFromFile(frm,FLDID_SCREEN, NNsiParam->fileName, 1, NNSH_KANJI_EUC, NNSH_LINECODE_LF);
        break;

      case FILEMGR_STATE_OPENED_VFS_UTF8:
        // UTF-8ファイル
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
            // ダミーのデータバッファを貼り付ける
            MemSet(buf, sizeof(buf), 0x00);
            NNshWinSetFieldText(frm, FLDID_SCREEN, false, buf, NNsiParam->bufferSize);
        }
        break;
    }
    NNsiParam->confirmationDisable = savedData;

    // 最大値が UInt16 のサイズに収まるかどうかをチェック
    if (NNsiParam->bufferSize > 0xffe0)
    {
        savedData = 0xffe0;
    }
    else
    {
        savedData = NNsiParam->bufferSize;
    }
    FldSetMaxChars(fldP, savedData);

    // AADAがインストールされているか確認する
    if (CheckInstalledResource_NNsh('DAcc', 'moAA') == false)
    {
        // AADAがインストールされていない場合には、AA挿入ボタンを隠す
        ctlObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_INSERT_AA));
        CtlSetEnabled(ctlObj, false);
        CtlSetUsable (ctlObj, false);
        FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_INSERT_AA));
        FrmEraseForm(frm);
        FrmDrawForm (frm);
    }

	// 他アプリケーションから起動されていた場合には、ReadボタンをEndボタンに
    if (NNsiGlobal->autoOpenFile != FILEMGR_STATE_DISABLED)
    {
	    // ReadボタンのラベルをEndに変更する
    	ctlObj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_DATA_LOAD));
	    CtlSetLabel(ctlObj, "End");
    }

    // フォーカスをメッセージフィールドに設定(パラメータが設定されてない場合)
    if (NNsiParam->notAutoFocus == 0)
    {
        FrmSetFocus(frm, FrmGetObjectIndex(frm, FLDID_SCREEN));
        FldSetInsPtPosition(fldP, NNsiParam->bufferSize);
    }

    // スクロールバーを更新
    NNshWinViewUpdateScrollBar(FLDID_SCREEN, SCLID_SCREEN);

    return (errNone);
}
