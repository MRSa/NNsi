//////////////////////////////////////////////////////////////////////////////
//  $Id: nnda.c,v 1.31 2007/01/02 14:35:36 mrsa Exp $
//
// nnda.c   --- nnDA main
//
// (C) NNsi project, 2004, all rights reserved.
//
//////////////////////////////////////////////////////////////////////////////
#include <PalmOS.h>
#include "nnda-rsc-defines.h"
#include "nnda-defines.h"

static void setnnDA_DAData       (FormType *frm, nnDADataType *adtP);
static void setDisp_nnDA_DAData  (FormType *frm, nnDADataType *adtP);
static void resetDisp_nnDA_DAData(FormType *frm, nnDADataType *adtP);
static void endnnDA_DAData       (FormType *frm, nnDADataType *adtP);

static Boolean checkPnoJpegLibInstalled (UInt16 *jpegRef);
static Boolean checkJpegLibInstalled    (UInt16 *jpegRef);
static Boolean checkSonyJpegLibInstalled(UInt16 *jpegRef);

static void displayJpegUsingSonyJpegLib(FormType *frm, nnDADataType *adtP);
static void displayJpegUsingJpegLib    (FormType *frm, nnDADataType *adtP);
#ifdef USE_PNOJPEGLIB2
static void displayJpegUsingPnoJpegLib2(FormType *frm, nnDADataType *adtP);
#else //  #ifdef USE_PNOJPEGLIB2
static void displayJpegUsingPnoJpegLib1(FormType *frm, nnDADataType *adtP);
#endif // #ifdef USE_PNOJPEGLIB2

#ifdef USE_CLIE
static void startClieDisp_DAData(FormType *frm, nnDADataType *adtP);
static void endClieDisp_DAData  (FormType *frm, nnDADataType *adtP);
#endif // #ifdef USE_CLIE


// 表示データの取得関数群
static Char *getFileStreamData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP);
static Char *getVfsFileData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP);
static Char *getFileStreamHtmlData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP);
static Char *getMemoryHandleData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP);
static Char *getStringListData(nnDADataType *adtP);
static Char *getStringData    (UInt16 rscID, nnDADataType *adtP);
static Char *outputHexString(Char *data, UInt16 len, nnDADataType *adtP);
static Char *getJpegFileData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP);
static Char *openFileStreamForJPEG(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP);
static void postProcessJpegDisplay(nnDADataType *adtP);

// UTF8 -> Shift JIS変換テーブル (AMsoftさんのＤＢを使用する)
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId);

static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr);
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte);
static UInt16  Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L);

static void ParseMessage(Char *buf, Char *src, UInt32 size, UInt16 kanjiCode);

Boolean textCallBack(PrgCallbackDataPtr cbP);
static Boolean checkVFSdevice(UInt32 aMediaType, nnDADataType *adtP);

static void resizeDialogWindow(FormType *diagFrm, nnDADataType *adtP);
static Boolean createDialogWindow(FormType **diagFrm, nnDADataType *adtP);
static Boolean CheckClipboard(nnDADataType *adtP, Char **infoName);

static Char *copyClipboard(void);

#if 0
/*=========================================================================*/
/*   Function : MemPtrNew_NNsh                                             */
/*                                    メモリ確保関数(将来的に使うかも...)  */
/*=========================================================================*/
MemPtr MemPtrNew_NNsh(UInt32 size, nnDADataType *adtP)
{
    if (adtP->palmOSVersion > 0x04000000)
    {
        // over 64kBの領域を確保する(OS4.x以上)
        return (MemGluePtrNew(size));
    }
    // 通常のメモリ確保
    return (MemPtrNew(size));
}
#endif

/*-------------------------------------------------------------------------*/
/*   Function :  getFileStreamData                                         */
/*                                          fileStreamからデータを取得する */
/*-------------------------------------------------------------------------*/
static Char *getFileStreamData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    UInt32     creator, dataSize;
    UInt16     len, dispLimit;
    Char      *ptr, *ptr2;
    Err        err;
    FileHand   filRef;
    LocalID    dbId;

    dbId = 0;
    dispLimit = DISP_LIMIT;
    switch (adtP->editCommand)
    {
      case nnDA_EDITCOMMAND_UTF8:
        dbId = DmFindDatabase(0, "UnicodeToJis0208");
        if (dbId == 0)
        {
            StrNCopy(title, "Not exist:UTF8-DB", titleSize);
            return (NULL);
        }
        dispLimit = (DISP_LIMIT / 3);
        break;

      case nnDA_EDITCOMMAND_BINDATA:
        dispLimit = (DISP_LIMIT / 3);
        break;

      default:
        dispLimit = DISP_LIMIT;
        break;
    }
    
    // creatorIDの抽出
    ptr = (Char *) &creator;
    *ptr       = infoName[0];
    *(ptr + 1) = infoName[1];
    *(ptr + 2) = infoName[2];
    *(ptr + 3) = infoName[3];

    // ファイル名の長さを検出
    len      = 0;
    ptr      = infoName + 5;  // クリエータID + ":" 分、ポインタを進める
    while (*ptr != '<')
    {
        len++;
        ptr++;
    }
    if (len > nnDA_FILENAMESIZE)
    {
        len = nnDA_FILENAMESIZE;
    }
    StrNCopy(adtP->fileName, (infoName + 5), len);

    // ファイルをオープンし、データをコピーする
    filRef = FileOpen(0, adtP->fileName, 0, creator, fileModeReadOnly, &err);
    if (filRef == 0)
    {
        // ファイルオープンに失敗したら、エラーコードを表示する
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], err);
        goto FUNC_ABEND;
    }

    // ファイルサイズを取得
    if (FileTell(filRef, &dataSize, &err) == -1)
    {
        // ファイルサイズの取得に失敗、ファイル名だけを表示する
        StrNCopy(title, "FileTell", titleSize);
        FileClose(filRef);
        goto FUNC_ABEND;
    }

    len = dataSize;
    if (dataSize > dispLimit)
    {
        len = dispLimit;
    }
    StrNCopy(title, adtP->fileName, titleSize);

    ptr = MEMALLOC_PTR(len + BUF_MARGIN);
    if (ptr == NULL)
    {
        StrNCopy(title, "MEMALLOC_PTR()", titleSize);
        FileClose(filRef);
        goto FUNC_ABEND;
    }
    MemSet (ptr, (len + BUF_MARGIN), 0x00);
    StrCopy(ptr, "size:");
    StrIToA(&ptr[StrLen(ptr)], len);
    StrCat (ptr, "/");
    StrIToA(&ptr[StrLen(ptr)], dataSize);
    StrCat (ptr, "\n=====\n");
    FileRead(filRef, &ptr[StrLen(ptr)], 1, len, &err);
        
    switch (adtP->editCommand)
    {
      case nnDA_EDITCOMMAND_UTF8:
        ptr2 = MEMALLOC_PTR(len + BUF_MARGIN);
        if (ptr2 == NULL)
        {
            StrNCopy(title, "MEMALLOC_PTR()", titleSize);
            FileClose(filRef);
            MEMFREE_PTR(ptr);
            goto FUNC_ABEND;
        }
        MemSet(ptr2, (len + BUF_MARGIN), 0x00);
        StrNCopyUTF8toSJ(ptr2, ptr, len, dbId);

        // 変換後の文字列が作成できたら、領域を開放する
        MEMFREE_PTR(ptr);
        ptr = ptr2;
        break;

      case nnDA_EDITCOMMAND_BINDATA:
        ptr2 = outputHexString(ptr, len, adtP);
        if (ptr2 != NULL)
        {
            // バイナリ表示データが作成できたら、領域を開放する
            MEMFREE_PTR(ptr);
            ptr = ptr2;
        }
        break;

      default:
        // なにもしない...
        break;
    }
    FileClose(filRef);
    return (ptr);

FUNC_ABEND:
    ptr = MEMALLOC_PTR(StrLen(adtP->fileName) + 4);
    if ((ptr != NULL)&&(StrLen(adtP->fileName) != 0))
    {
        MemSet (ptr, (StrLen(adtP->fileName) + 4), 0x00);
        MemMove(ptr, adtP->fileName, StrLen(adtP->fileName));
    }
    return (ptr);
}

/*-------------------------------------------------------------------------*/
/*   Function :  getVfsFileData                                            */
/*                                            VFS fileからデータを取得する */
/*-------------------------------------------------------------------------*/
static Char *getVfsFileData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    Err     ret;
    Char   *ptr, *ptr2;
    Boolean result;
    UInt32  dataSize, len;

    // xRRRR:/...(fileName)...
    //   x: 読み込みファイルのありか(ファイルストリーム or VFSの指定)
    //       v : VFS (指定なし)
    //       c : VFS (CF)
    //       m : VFS (MS)
    //       s : VFS (SD)
    //       t : VFS (SmartMedia ... ToshibaのT)
    //       r : VFS (RD  : ramdisk)
    //       d : VFS (DoC : 内蔵デバイス)
    //   RRRR: 予約領域
    //   /...(fileName)...  ファイル名称
    adtP->vfsFileRef = 0;
    switch (infoName[0])
    {
      case 'c': 
        // VFS(CF)
        result = checkVFSdevice(ExpMediaType_CompactFlash, adtP);
        break;

      case 'm': 
        // VFS(MS)
        result = checkVFSdevice(ExpMediaType_MemoryStick, adtP);
        break;

      case 's': 
        // VFS(SD)
        result = checkVFSdevice(ExpMediaType_SecureDigital, adtP);
        if (result == false)
        {
            result = checkVFSdevice(ExpMediaType_MultiMediaCard, adtP);
        }
        break;

      case 't': 
        // VFS(SmartMedia)
        result = checkVFSdevice(ExpMediaType_SmartMedia, adtP);
        break;

      case 'r': 
        // VFS(RamDisk)
        result = checkVFSdevice(ExpMediaType_RAMDisk, adtP);
        break;

      case 'd': 
        // VFS(DoC)
        result = checkVFSdevice('Tffs', adtP);
        break;

      default:
        // VFS(any)
        result = checkVFSdevice(0, adtP);
        break;
    }
    if (result == false)
    {
        //  VFS未検出、終了する
        return (NULL);
    }

    // ファイル名を 変数ファイル名にコピー
    ptr  = adtP->fileName;
    ptr2 = &(infoName[6]);
    while ((*ptr2 != '<')&&(*ptr2 != '\0'))
    {
        *ptr = *ptr2;
        ptr++;
        ptr2++;
    }

    // ファイルをオープンする(VFS)
    ret = VFSFileOpen(adtP->vfsVol,adtP->fileName,vfsModeRead,&(adtP->vfsFileRef));
    if (ret != errNone)
    {
        // ファイルオープンに失敗したら、ファイル名だけ表示する
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], ret);
        adtP->vfsFileRef = 0;
        goto FUNC_ABEND;
    }

    // ファイルサイズを取得
    ret = VFSFileSize(adtP->vfsFileRef, &(dataSize));
    if (ret != errNone)
    {
        // ファイルサイズの取得に失敗、ファイル名だけを表示する
        StrNCopy(title, "VfsFileSize()", titleSize);
        VFSFileClose(adtP->vfsFileRef);
        adtP->vfsFileRef = 0;
        goto FUNC_ABEND;
    }

    len = dataSize;
    if (dataSize > DISP_LIMIT)
    {
        len = DISP_LIMIT;
    }
    StrNCopy(title, adtP->fileName, titleSize);

    ptr = MEMALLOC_PTR(len + BUF_MARGIN);
    if (ptr == NULL)
    {
        StrNCopy(title, "MEMALLOC_PTR()", titleSize);
        VFSFileClose(adtP->vfsFileRef);
        adtP->vfsFileRef = 0;
        goto FUNC_ABEND;
    }

    MemSet (ptr, (len + BUF_MARGIN), 0x00);
    StrCopy(ptr, "size:");
    StrIToA(&ptr[StrLen(ptr)], len);
    StrCat (ptr, "/");
    StrIToA(&ptr[StrLen(ptr)], dataSize);
    StrCat (ptr, "\n=====\n");
    VFSFileSeek(adtP->vfsFileRef, vfsOriginBeginning, 0);
    VFSFileRead(adtP->vfsFileRef, len, &ptr[StrLen(ptr)], &(dataSize));
    VFSFileClose(adtP->vfsFileRef);

    adtP->vfsFileRef = 0;
    return (ptr);

FUNC_ABEND:
    ptr = MEMALLOC_PTR(StrLen(adtP->fileName) + 4);
    if ((ptr != NULL)&&(StrLen(adtP->fileName) != 0))
    {
        MemSet (ptr, (StrLen(adtP->fileName) + 4), 0x00);
        MemMove(ptr, adtP->fileName, StrLen(adtP->fileName));
    }
    return (ptr);
}

/*--------------------------------------------------------------------------*/
/*  Function : outputHexString                                              */
/*                                             デバッグ用に数値へデータ変換 */
/*--------------------------------------------------------------------------*/
static Char *outputHexString(Char *data, UInt16 len, nnDADataType *adtP)
{
    UInt16   loop;
    UInt8   *dat;
    Char     buff[12], *buffer;

    buffer = MEMALLOC_PTR(len * 3 + MARGIN);
    if (buffer == NULL)
    {
         // 領域確保失敗、何も表示せず終了する
         return (NULL);
    }
    MemSet(buffer, (len * 3 + MARGIN), 0x00);

    // 数値データを画面表示
    dat = data;
    for (loop = 0; loop < len; loop++)
    {
        if ((loop % 8) == 0)
        {
            // データを８桁づつ区切る
            StrCat(buffer, "\n");            
        } 
        MemSet (buff, sizeof(buff), 0x00);
        StrIToH(buff, *dat);
        StrCat(buffer, &buff[6]);
        dat++;
    }
    return (buffer);
}

/*-------------------------------------------------------------------------*/
/*   Function :  getStringListData                                         */
/*                        一覧画面の操作ヘルプデータ(ストリング)を取得する */
/*-------------------------------------------------------------------------*/
static Char *getStringListData(nnDADataType *adtP)
{
    MemHandle  memH, memH2;
    UInt16     len;
    Char      *ptr, *ptr2, *data;

    // ストリングリソースを取得
    memH = DmGetResource('tSTR', HELPSTRING_LIST);
    if (memH == 0)
    {
        // リソースの取得失敗、NULLを応答する
        return (NULL);
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        // リソースの取得失敗、NULLを応答する
        return (NULL);
    }
    len = StrLen(ptr);
    
    // ストリングリソースを取得
    ptr2  = NULL;
    memH2 = DmGetResource('tSTR', HELPSTRING_LIST2);
    if (memH2 != 0)
    {
        ptr2 = MemHandleLock(memH2);
        if (ptr2 != NULL)
        {
            len = len + StrLen(ptr2);
        }
    }

    // データの格納領域を確保し、リソースデータをコピー
    // （ここで確保した領域は、nnDA終了時に解放する）
    data = MEMALLOC_PTR(len + MARGIN);
    if (data == NULL)
    {
        return (NULL);
    }
    MemSet (data, (len + MARGIN), 0x00);
    StrCopy(data, ptr);
    if (ptr2 != NULL)
    {
        StrCat(data, ptr2);
    }
    MemHandleUnlock(memH2);
    MemHandleUnlock(memH);

    return (data);
}

/*-------------------------------------------------------------------------*/
/*   Function :  getStringData                                             */
/*                                  操作ヘルプデータ(ストリング)を取得する */
/*-------------------------------------------------------------------------*/
static Char *getStringData(UInt16 rscID, nnDADataType *adtP)
{
    MemHandle  memH;
    UInt16     len;
    Char      *ptr, *data;

    // ストリングリソースを取得
    memH = DmGetResource('tSTR', rscID);
    if (memH == 0)
    {
        // リソースの取得失敗、NULLを応答する
        return (NULL);
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        // リソースの取得失敗、NULLを応答する
        return (NULL);
    }
    len = StrLen(ptr);

    // データの格納領域を確保し、リソースデータをコピー
    // （ここで確保した領域は、nnDA終了時に解放する）
    data = MEMALLOC_PTR(len + 4);
    if (data == NULL)
    {
        return (NULL);
    }
    MemSet (data, (len + 4), 0x00);
    StrCopy(data, ptr);

    MemHandleUnlock(memH);

    return (data);
}

/*-------------------------------------------------------------------------*/
/*   Function :  checkDisplayMode                                          */
/*                           画面モード(PalmOS5高解像度サポート)のチェック */
/*-------------------------------------------------------------------------*/
static void checkDisplayMode(nnDADataType *adtP)
{
    UInt32 version;
#ifdef USE_CLIE
    Err     ret;
    Boolean isColor;
    UInt32  width, height, depth;
#endif

    adtP->os5density = (UInt32) kDensityLow;

/**
    // Yomeru5 を利用しているか？
    if (FtrGet('Ymru', 5000, &version) == 0)
    {
        // Yomeru5を利用中、、、高解像度モードは使用しないことにする
        return;
    }
**/

    // 高解像度フィーチャーの取得
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &(version)) != 0)
    {
        // 高解像度フィーチャーのサポートなし
        return;
    }
    if (version < 4)
    {
        //　高解像度のサポートがない場合の処理
        //  (これで正しいんだろうか...マニュアルにはそう書いてあったが...)
#ifdef USE_CLIE
        // CLIE用高解像度がサポートされているかチェックする    
        // HRLIBの参照番号を取得する
        ret = SysLibFind(sonySysLibNameHR, &(adtP->hrRef));
        if (ret == sysErrLibNotFound)
        {
            // HRLIBが未ロードの場合、ロードできるかTryしてみる。
            ret = SysLibLoad('libr', sonySysFileCHRLib, &(adtP->hrRef));
        }
        if (ret != errNone)
        {
            // HRLIBは使用できない、標準解像度を使う
            adtP->hrRef = 0;
            return;
        }

        // HRLIBの使用宣言とHRLIBのバージョン番号取得
        HROpen(adtP->hrRef);
        HRGetAPIVersion(adtP->hrRef, &(adtP->hrVer));
        if (adtP->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
        {
            // 旧バージョンのハイレゾ...画面モードを確認する...
            ret = HRWinScreenMode(adtP->hrRef, winScreenModeGet, 
                                  &width, &height, &depth, &isColor);
            if ((ret != errNone)||(width != hrWidth))
            {
                // ハイレゾモードではないので、HRLIBは使用しない
                HRClose(adtP->hrRef);
                adtP->hrRef = 0;
                return;
            }
        }
#endif // #ifdef USE_CLIE
        return;
    }

    // Windowの解像度を取得する
    WinScreenGetAttribute(winScreenDensity, &(adtP->os5density));

    // とりあえず、標準解像度にしておく
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}

/*=========================================================================*/
/*   Function :  getInformation                                            */
/*                          与えられた情報から、表示すべき情報の設定を行う */
/*=========================================================================*/
Char *getInformation(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    // 与えられた情報が異常？
    if (infoName == NULL)
    {
        // 何もせず応答する。
        return (NULL);
    }

    ////////////////////// 表示タイトルと表示文字列の生成 //////////////////

    // ファイルからデータを取得する
    if (StrNCompare(infoName, nnDA_NNSIEXT_FILE, (sizeof(nnDA_NNSIEXT_FILE) - 1)) == 0)
    {
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE) - 1), title, titleSize, adtP));
    }

    // ファイルからデータを取得する(折り返し対応)
    if (StrNCompare(infoName, nnDA_NNSIEXT_FILE_FOLD, (sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // 折りたたみ対応指示
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1), title, titleSize, adtP));
    }

    // 一覧画面のヘルプデータを取得する
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPLIST, (sizeof(nnDA_NNSIEXT_HELPLIST) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_LIST, titleSize);
        return (getStringListData(adtP));
    }

    // 参照画面のヘルプデータを取得する
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPVIEW, (sizeof(nnDA_NNSIEXT_HELPVIEW) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_VIEW, titleSize);
        return (getStringData(HELPSTRING_VIEW, adtP));
    }

    // ユーザ設定タブ画面のヘルプデータを取得する
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPUSERTAB, (sizeof(nnDA_NNSIEXT_HELPUSERTAB) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_USERTAB, titleSize);
        return (getStringData(HELPSTRING_USERTAB, adtP));
    }

    // NNsi設定画面のヘルプデータを取得する
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPNNSISET, (sizeof(nnDA_NNSIEXT_HELPNNSISET) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_NNSISET, titleSize);
        return (getStringData(HELPSTRING_NNSISET, adtP));
    }

    // 参照ログ一覧画面のヘルプデータを取得する
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPGETLOG, (sizeof(nnDA_NNSIEXT_HELPGETLOG) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_GETLOG, titleSize);
        return (getStringData(HELPSTRING_GETLOG, adtP));
    }

    // ファイルからのデータ読み出し...
    if (StrNCompare(infoName, nnDA_NNSIEXT_FILE, (sizeof(nnDA_NNSIEXT_FILE) - 1)) == 0)
    {
        return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_FILE) - 1), title, titleSize, adtP));
    }

    // バイナリファイルからのデータ読み出し...
    if (StrNCompare(infoName, nnDA_NNSIEXT_BINFILE, (sizeof(nnDA_NNSIEXT_BINFILE) - 1)) == 0)
    {
        adtP->editCommand = nnDA_EDITCOMMAND_BINDATA;
        return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_BINFILE) - 1), title, titleSize, adtP));
    }

    // メモリハンドルからのデータ読み出し...
    if (StrNCompare(infoName, nnDA_NNSIEXT_VIEWMEM, (sizeof(nnDA_NNSIEXT_VIEWMEM) - 1)) == 0)
    {
        return (getMemoryHandleData(infoName + (sizeof(nnDA_NNSIEXT_VIEWMEM) - 1), title, titleSize, adtP));
    }

    // メモリハンドルからのデータ読み出し...(折りたたみ指示対応)
    if (StrNCompare(infoName, nnDA_NNSIEXT_VIEWMEM_FOLD, (sizeof(nnDA_NNSIEXT_VIEWMEM_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // 折りたたみ対応指示
        return (getMemoryHandleData(infoName + (sizeof(nnDA_NNSIEXT_VIEWMEM_FOLD) - 1), title, titleSize, adtP));
    }

    // HTMLファイルからのデータ読み出し...(折りたたみ指示対応)
    if (StrNCompare(infoName, nnDA_NNSIEXT_HTML_FOLD, (sizeof(nnDA_NNSIEXT_HTML_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // 折りたたみ対応指示
        adtP->editCommand    = nnDA_EDITCOMMAND_HTMLPARSE;
        return (getFileStreamHtmlData(infoName + (sizeof(nnDA_NNSIEXT_HTML_FOLD) - 1), title, titleSize, adtP));
    }

    // HTMLファイルからのデータ読み出し...
    if (StrNCompare(infoName, nnDA_NNSIEXT_HTMLFILE, (sizeof(nnDA_NNSIEXT_HTMLFILE) - 1)) == 0)
    {
        adtP->editCommand    = nnDA_EDITCOMMAND_HTMLPARSE;
        return (getFileStreamHtmlData(infoName + (sizeof(nnDA_NNSIEXT_HTMLFILE) - 1), title, titleSize, adtP));
    }

    // UTF8ファイルからのデータ読み出し
    if (StrNCompare(infoName, nnDA_NNSIEXT_UTF8FILE, (sizeof(nnDA_NNSIEXT_UTF8FILE) - 1)) == 0)
    {
        if (DmFindDatabase(0, "UnicodeToJis0208") != 0)
        {
            adtP->editCommand    = nnDA_EDITCOMMAND_UTF8;
            return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_UTF8FILE) - 1), title, titleSize, adtP));
        }
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1), title, titleSize, adtP));
    }

    // UTF8ファイルからのデータ読み出し(折りたたみ指示対応)
    if (StrNCompare(infoName, nnDA_NNSIEXT_UTF8FILEFOLD, (sizeof(nnDA_NNSIEXT_UTF8FILEFOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // 折りたたみ対応指示
        if (DmFindDatabase(0, "UnicodeToJis0208") != 0)
        {
            adtP->editCommand    = nnDA_EDITCOMMAND_UTF8;
            return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_UTF8FILEFOLD) - 1), title, titleSize, adtP));
        }
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1), title, titleSize, adtP));
    }

    // VFSファイルからのデータ読み出し
    if (StrNCompare(infoName, nnDA_NNSIEXT_VFSFILE, (sizeof(nnDA_NNSIEXT_VFSFILE) - 1)) == 0)
    {
        return (getVfsFileData(infoName + (sizeof(nnDA_NNSIEXT_VFSFILE) - 1), title, titleSize, adtP));
    }

    // VFSファイルからのデータ読み出し(折りたたみ指示対応)
    if (StrNCompare(infoName, nnDA_NNSIEXT_VFSFILE_FOLD, (sizeof(nnDA_NNSIEXT_VFSFILE_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // 折りたたみ対応指示
        return (getVfsFileData(infoName + (sizeof(nnDA_NNSIEXT_VFSFILE_FOLD) - 1), title, titleSize, adtP));
    }

    // JPEGファイルの表示を行う...
    if (StrNCompare(infoName, nnDA_NNSIEXT_SHOWJPEG, (sizeof(nnDA_NNSIEXT_SHOWJPEG) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_JPEGDISP;
        return (getJpegFileData((infoName + sizeof(nnDA_NNSIEXT_SHOWJPEG) - 1), title, titleSize, adtP));
    }

    // nnDA非サポート。。。
    return (NULL);
}

/*-------------------------------------------------------------------------*/
/*   Function :  nnDA_EventHandler                                         */
/*                                                   nnDA の event 処理    */
/*-------------------------------------------------------------------------*/
static Boolean nnDA_EventHandler(EventPtr evP)
{
    UInt16        btnId   = 0;
    UInt32        is5Navi = 0;
    Boolean       mov;
    nnDADataType *adtP = NULL;
    FormType     *frm;
    ControlType  *obj;

    // global pointerを取得する
    if (FtrGet(nnDA_Creator, ADT_FTRID, (UInt32 *) &adtP) != 0)
    {
        // グローバルポインタ取得失敗
        return (false);
    }

    // シフトインジケータを確認する
    frm = FrmGetActiveForm();
    obj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));
    mov = CtlEnabled(obj);

    // 5way-navを無効にする(PalmTX対応...)
    if (FtrGet(sysFtrCreator, sysFtrNumFiveWayNavVersion, &is5Navi) == errNone)
    {
        FrmSetNavState(FrmGetActiveForm(), kFrmNavStateFlagsInteractionMode);
    }

    switch (evP->eType)
    {
      case keyDownEvent:
        switch (evP->data.keyDown.chr)
        {
          // JOG PUSH/Zキーを押したときの処理
          case vchrJogRelease:
          case vchrThumbWheelPush:
          case chrCapital_Z:
          case chrSmall_Z:
            // 上下移動モード・左右移動モードの切り替えを実行
            btnId = BTNID_SHIFT;
            break;

          case vchrPageDown:
          case chrDownArrow:
          case vchrJogDown:
          case chrCapital_J:
          case chrSmall_J:
            // 下スクロール
            if (mov == true)
            {
                // 左右移動モード(右スクロール)
                btnId = BTNID_NEXT;
            }
            else
            {
                // 上下移動モード(下スクロール)
                btnId = BTNID_DOWN;
            }
            break;

          case vchrPageUp:
          case chrUpArrow:
          case vchrJogUp:
          case chrCapital_K:
          case chrSmall_K:
            if (mov == true)
            {
                // 左右移動モード(右スクロール)
                btnId = BTNID_PREV;
            }
            else
            {
                // 上下移動モード(上スクロール)
                btnId = BTNID_UP;
            }
            break;

          case vchrJogLeft:
          case chrLeftArrow:
          case vchrJogPushedUp:
          case chrCapital_H:
          case chrSmall_H:
          case vchrRockerLeft:
            // 左スクロール
            btnId = BTNID_PREV;
            break;

          case vchrJogRight:
          case chrRightArrow:
          case vchrJogPushedDown:
          case chrCapital_L:
          case chrSmall_L:
          case vchrRockerRight:
            // 右スクロール
            btnId = BTNID_NEXT;
            break;

          case vchrHard2:
          case vchrHard3:
          case chrCapital_Q:
          case chrSmall_Q:
          case vchrRockerCenter:
            // ＯＫを押したことにする
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_DA_OK)));
            return (true);
            break;
            
          case chrBackspace:
          case chrEscape:
            // About表示
            btnId = BTNID_DA_ABOUT;
            break;

          case vchrNavChange:
            // 5way navigator対応
            if ((evP->data.keyDown.keyCode) & navBitLeft)
            {
                // カーソルキー左と等価(左スクロール)
                btnId = BTNID_PREV;
                break;
            }
            if ((evP->data.keyDown.keyCode) & navBitRight)
            {
                // カーソルキー右と等価(右スクロール)
                btnId = BTNID_NEXT;
                break;
            }
            break;

          case chrCapital_F:
          case chrSmall_F:
            // フォントサイズの変更...
            adtP->fontId = FontSelect(adtP->fontId);
            FntSetFont(adtP->fontId);
            btnId = GADID_DA_VIEW;
            break;
           
          case chrSpace:
          case chrCarriageReturn:
          case chrLineFeed:
          default:
            // 何もしない
            return (false);
            break;
        }
        break;

      case ctlSelectEvent:
        // あとで処理する
        btnId = evP->data.ctlSelect.controlID;
        break;
        
      case ctlRepeatEvent:
        // あとで処理する
        btnId = evP->data.ctlRepeat.controlID;

      default:
        // 何もしない
        return (false);
        break;
    }

    // ボタンが押されたときの処理分岐
    switch (btnId)
    {
      case BTNID_DA_ABOUT:
        // About表示
        FrmAlert(ALERTID_DA_ABOUT);
        break;

      case BTNID_UP:
        // 上移動
        if (adtP->startY > 0)
        {
            adtP->startY = adtP->startY - (adtP->area.extent.y / 2);
        }
        break;      

      case BTNID_DOWN:
        // 下移動
        if (adtP->startY < adtP->maxY)
        {
            adtP->startY = adtP->startY + (adtP->area.extent.y / 2);
        }
        break;

      case BTNID_PREV:
        // 前(左移動)
        if (adtP->startX > 0)
        {
            adtP->startX = adtP->startX - (adtP->area.extent.x / 2);
        }
        break;

      case BTNID_NEXT:
        // 次(右移動)
        if (adtP->startX < adtP->maxX)
        {
            adtP->startX = adtP->startX + (adtP->area.extent.x / 2);
        }
        break;

      case BTNID_SHIFT:
        // シフト状態切り替え
        // ON/OFFが切り替わったら、"!"の表示を切り替える
        if (mov == true)
        {
            // 左右移動モード→上下移動モード(!を消す)
            CtlSetEnabled(obj, false);
            CtlSetUsable (obj, false);
            FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));                
        }
        else
        {
            // 上下移動モード→左右移動モード(!を表示する)
            CtlSetEnabled(obj, true);
            CtlSetUsable (obj, true);
            FrmShowObject(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));   
        }
        break;

      case GADID_DA_VIEW:
        // 画面を再描画する
        break;

      default:
        // 何もしない
        return (false);
        break;
    }

    // JPEG表示の場合には、なにもしない
    if (adtP->displayCommand == nnDA_DISPCOMMAND_JPEGDISP)
    {
        return (false);
    }

    // 画面を再描画(テキスト表示のとき)
    setDisp_nnDA_DAData  (FrmGetActiveForm(), adtP);
    setnnDA_DAData       (FrmGetActiveForm(), adtP);
    resetDisp_nnDA_DAData(FrmGetActiveForm(), adtP);

    return (true);
}

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  startClieDisp_DAData                                      */
/*                                              描画モード設定処理(CLIE用) */
/*-------------------------------------------------------------------------*/
static void startClieDisp_DAData(FormType *frm, nnDADataType *adtP)
{

    // drawしないと、Window上に領域が表示されない、、、
    FrmDrawForm(frm);

    // ガジェットの領域サイズを取得する
    FrmGetObjectBounds(frm,
                       FrmGetObjectIndex(frm, GADID_DA_VIEW),
                       &(adtP->dimF));

    // CLIE高解像度用に描画エリアを拡大する
    adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
    adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
    adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
    adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2;

    // 描画領域のクリア
    HRWinEraseRectangle(adtP->hrRef, &(adtP->area), 0);

    return;
}
#endif  // #ifdef USE_CLIE


#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  endClieDisp_DAData                                        */
/*                                                  画面描画後処理(CLIE用) */
/*-------------------------------------------------------------------------*/
static void endClieDisp_DAData(FormType *frm, nnDADataType *adtP)
{

    return;
}
#endif // #ifdef USE_CLIE


/*-------------------------------------------------------------------------*/
/*   Function :  WinDrawChar_NNsh                                          */
/*                                                            文字表示処理 */
/*-------------------------------------------------------------------------*/
static void WinDrawChar_NNsh(nnDADataType *adtP, WChar theChar, 
                               Coord x, Coord y)
{
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        HRWinDrawChar(adtP->hrRef, theChar, x, y);
    }
    else
#endif
    {
        WinDrawChar(theChar, x, y);
    }
    return;    
}

/*-------------------------------------------------------------------------*/
/*   Function :  setDisp_nnDA_DAData                                       */
/*                                            画面に文字列を貼り付ける処理 */
/*-------------------------------------------------------------------------*/
static void setDisp_nnDA_DAData(FormType *frm, nnDADataType *adtP)
{
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        startClieDisp_DAData(frm, adtP);  //  画面表示準備処理
        return;
    }
#endif

    // drawしないと、Window上に領域が表示されない、、、
    FrmDrawForm(frm);

    // ガジェットの領域サイズを取得する
    FrmGetObjectBounds(frm,FrmGetObjectIndex(frm, GADID_DA_VIEW),
                                                               &(adtP->dimF));

    // 解像度モードを確認し、領域をクリアする(areaに実解像度を格納)
    switch (adtP->os5density)
    {
      case kDensityDouble:
        // 320x320
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityTriple:
        // 480x480
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 3;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityQuadruple:
        // 640x640
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 4;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 4;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 4;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 4;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityOneAndAHalf:
        // 240x240
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2 / 3;
        adtP->area.extent.x   = adtP->dimF.extent.x  * 2 / 3;
        adtP->area.extent.y   = adtP->dimF.extent.y  * 2 / 3;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2 / 3;
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()),kDensityLow);
        WinSetCoordinateSystem(kCoordinatesNative);
        break;

      case kDensityLow:
      default:
        // 160x160
        adtP->area.topLeft.x  = adtP->dimF.topLeft.x;
        adtP->area.extent.x   = adtP->dimF.extent.x;
        adtP->area.extent.y   = adtP->dimF.extent.y;
        adtP->area.topLeft.y  = adtP->dimF.topLeft.y;
        break;
    }
    // 描画領域のクリア
    WinEraseRectangle(&(adtP->area), 0);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  resetDisp_nnDA_DAData                                     */
/*                                    画面に文字列を貼り付ける処理の後処理 */
/*-------------------------------------------------------------------------*/
static void resetDisp_nnDA_DAData(FormType *frm, nnDADataType *adtP)
{

    // 高解像度の設定を解除
    switch (adtP->os5density)
    {
      case kDensityDouble:
      case kDensityQuadruple:
      case kDensityTriple:
      case kDensityOneAndAHalf:
        BmpSetDensity(WinGetBitmap(WinGetDisplayWindow()), adtP->os5density);
        WinSetCoordinateSystem(kCoordinatesStandard);
        break;

      case kDensityLow:
      default:
        break;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  endnnDA_DAData                                            */
/*                                    画面に文字列を貼り付ける処理の後処理 */
/*-------------------------------------------------------------------------*/
static void endnnDA_DAData(FormType *frm, nnDADataType *adtP)
{
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        endClieDisp_DAData(frm, adtP);  //  画面表示準備処理
        return;
    }
#endif

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  setnnDA_DAData                                            */
/*                                            画面に文字列を貼り付ける処理 */
/*-------------------------------------------------------------------------*/
static void setnnDA_DAData(FormType *frm, nnDADataType *adtP)
{
    UInt16  letter;
    UInt8  *ptr, upper, lower;
    Coord   x, y, topX, marginX;

    ptr  = adtP->areaP;
    topX = 0;
    x    = 0;
    y    = 0;
    marginX = FntCharWidth('\x01');   // missing character symbolの幅を基準に
    while ((UInt8 *) ptr < adtP->areaP + adtP->length)
    {
        // 指定されたコードを表示する
        if ((*ptr == 0x0a)||(*ptr == 0x0d))
        {
            if ((*ptr == 0x0d)&&(*(ptr + 1) == 0x0a))
            {
                // 改行コードがＣＲ＋ＬＦだった場合
                ptr++;
            }

            // 改行コードのとき
            y = y + FntCharHeight();
            adtP->maxY = y;
            x = topX;
            ptr++;
            if (y >= adtP->startY + adtP->area.extent.y)
            {
                // 描画領域幅を超えた、終了する
                break;
            }
            continue;
        }

        // 折り返し表示を行う場合のチェック、、、(表示範囲を超えているか？)
        if ((adtP->displayCommand == nnDA_DISPCOMMAND_FOLD)&&
            (x >= adtP->startX + adtP->area.extent.x))
        {
            // 改行を実施する
            y = y + FntCharHeight();
            adtP->maxY = y;
            x = topX;
            if (y >= adtP->startY + adtP->area.extent.y)
            {
                // 描画領域幅を超えた、終了する
                break;
            }
            continue;
        }

        // 漢字 or ANK ?
        if ((*ptr <= 0x80)||((*ptr >= 0xa1)&&(*ptr <= 0xdf)))
        {            
            // シングルバイトの場合
            letter = *ptr;
            ptr++;
            if ((letter >= 0x20)&&(x >= adtP->startX)&&(y >= adtP->startY)&&
                (x < adtP->startX + adtP->area.extent.x)&&
                (y < adtP->startY + adtP->area.extent.y))
            {
                WinDrawChar_NNsh(adtP, letter, 
                                 (x - adtP->startX + adtP->area.topLeft.x),
                                 (y - adtP->startY + adtP->area.topLeft.y));
            }
#ifdef USE_REAL_WIDTH
            // 横幅を計算した値にする...
            x = x + FntCharWidth(letter);
#else
            x = x + marginX;
#endif
           adtP->maxX = x;
        }
        else
        {            
            // ダブルバイトの場合、JISコードに変換してから
            // 上位8ビット/下位8ビットを変数にコピーする
            upper   = *ptr;
            letter  = *ptr << 8;
            ptr++;
            lower   = *ptr;
            letter  = letter + *ptr;
            ptr++;
            if (upper <= 0x9f)
            {
                if (lower < 0x9f)
                {
                    upper = (upper << 1) - 0xe1;
                }
                else
                {
                    upper = (upper << 1) - 0xe0;
                }
            }
            else
            {
                if (lower < 0x9f)
                {
                    upper = ((upper - 0xb0) << 1) - 1;
                }
                else
                {
                    upper = ((upper - 0xb0) << 1);
                }
            }
            if (lower < 0x7f)
            {
                lower = lower - 0x1f;
            }
            else
            {
                if (lower < 0x9f)
                {
                    lower = lower - 0x20;
                }
                else
                {
                    lower = lower - 0x7e;
                }
            }

            if ((x >= adtP->startX)&&(y >= adtP->startY)&&
                (x < adtP->startX + adtP->area.extent.x)&&
                (y < adtP->startY + adtP->area.extent.y))
            {
                WinDrawChar_NNsh(adtP, letter, 
                                 (x - adtP->startX + adtP->area.topLeft.x),
                                 (y - adtP->startY + adtP->area.topLeft.y));
            }            
            letter = upper << 8;
            letter = lower + letter;
            x = x + (marginX + marginX);
            adtP->maxX = x;
        }
    }

    return;
}

/*=========================================================================*/
/*   Function :  nnDAMain                                                  */
/*                                                        DAデータ表示処理 */
/*=========================================================================*/
void nnDAMain(void)
{
    FontID        fontId;
    UInt16        result;
    Char         *infoName, *infoData;
    FormType     *diagFrm,  *prevFrm;
    nnDADataType *adtP;

    // 現在のフォームを取得
    prevFrm = FrmGetActiveForm();

    // 共有データの確保
    adtP = MEMALLOC_PTR(sizeof(nnDADataType) + MARGIN);
    if (adtP == NULL)
    {
        // 領域確保失敗、終了する
        return;
    }
    MemSet(adtP, (sizeof(nnDADataType) + MARGIN), 0x00);
    StrNCopy(adtP->title, nnDA_DEFTITLE, nnDA_TITLESIZE);
    infoData  = NULL;

    // global pointerをfeatureとしてセットする
    FtrSet(nnDA_Creator, ADT_FTRID, (UInt32) adtP);

    // OSバージョンの確認
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(adtP->palmOSVersion)) != errNone)
    {
        adtP->palmOSVersion = 0;
    }

    // フォントサイズを一時的に変更する
    fontId = FntGetFont();
    adtP->fontId = stdFont;
    FntSetFont(adtP->fontId);

    //  画面モードのチェック
    checkDisplayMode(adtP);

    //  ダイアログウィンドウの作成
    createDialogWindow(&diagFrm, adtP);

    // windowのリサイズ
    resizeDialogWindow(diagFrm, adtP);

    // Viewer Formのイベントハンドラ登録
    FrmSetEventHandler(diagFrm, nnDA_EventHandler);

    ////////////////////////////////////////////////////////////
    //  データの画面設定処理
    //   (クリップボードにあるデータを取得後表示する)
    infoData = NULL;
    if (CheckClipboard(adtP, &infoName) == true)
    {
        // 表示すべき情報の取得
        infoData = getInformation(infoName, adtP->title, nnDA_TITLESIZE, adtP);

        // 情報名領域の解放
        MEMFREE_PTR(infoName);
        infoName = NULL;

        // 表示すべき情報が取得できたか？
        if (infoData != NULL)
        {
            // 表示するデータの設定
            adtP->areaP  = infoData;
            adtP->length = StrLen(infoData);
        }
        else
        {
            if (adtP->displayCommand != nnDA_DISPCOMMAND_JPEGDISP)
            {
                // 情報が取得できなかったとき、、、
                goto DISPLAY_SCREEN;
            }
        }
    }
    else
    {
        // クリップボードの内容を表示文字列とする
        adtP->areaP = copyClipboard();
        if (adtP->areaP == NULL)
        {
            // 情報が取得できなかった、、、終了する
            goto DISPLAY_SCREEN;   
        }
    }

    setDisp_nnDA_DAData  (diagFrm, adtP);  //  画面表示準備処理
    if (adtP->displayCommand == nnDA_DISPCOMMAND_JPEGDISP)
    {
        // JPEG画面表示
        adtP->bitmapPP = NULL;
        adtP->bitmapPPV3 = NULL;
        adtP->jpegRef  = 0;

        // JPEGファイルの位置が指定されていなかった...
        if (adtP->sourceLocation == 0)
        {
            resetDisp_nnDA_DAData(diagFrm, adtP);  //  画面表示後処理

            FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", " a JPEG file", ".");
            goto DISPLAY_SCREEN;
        }

        // JpegLibが使用可能か確認する
        if (checkPnoJpegLibInstalled(&(adtP->jpegRef)) == true)
        {
            // JpegLibが使用可能、そっちを使用して画像を表示する
#ifdef USE_PNOJPEGLIB2
            displayJpegUsingPnoJpegLib2(FrmGetActiveForm(), adtP);
#else  // #ifdef USE_PNOJPEGLIB2
            displayJpegUsingPnoJpegLib1(FrmGetActiveForm(), adtP);
#endif  // #ifdef USE_PNOJPEGLIB2
        }
        else if (checkJpegLibInstalled(&(adtP->jpegRef)) == true)
        {
            // JpegLibが使用可能、そっちを使用して画像を表示する
            displayJpegUsingJpegLib(FrmGetActiveForm(), adtP);
        }
        else if (checkSonyJpegLibInstalled(&(adtP->jpegRef)) == true)
        {
            // Sony JpegLibが使用可能、これを利用して画像表示する
            displayJpegUsingSonyJpegLib(FrmGetActiveForm(), adtP);
        }
        else
        {
            resetDisp_nnDA_DAData(diagFrm, adtP);  //  画面表示後処理

            // サポートしている JPEGライブラリが見つからなかった...
            FrmCustomAlert(ALTID_ERROR, "Could not find ", "any JPEG library", ".");
            goto DISPLAY_SCREEN;
        }

        // 上下左右ボタンを押せないようにする...
        CtlSetUsable(FrmGetObjectPtr(FrmGetActiveForm(),
                     FrmGetObjectIndex(FrmGetActiveForm(), 
                     BTNID_NEXT)), false);

        CtlSetUsable(FrmGetObjectPtr(FrmGetActiveForm(),
                     FrmGetObjectIndex(FrmGetActiveForm(), 
                     BTNID_PREV)), false);

        CtlSetUsable(FrmGetObjectPtr(FrmGetActiveForm(),
                     FrmGetObjectIndex(FrmGetActiveForm(), 
                     BTNID_UP)), false);

        CtlSetUsable(FrmGetObjectPtr(FrmGetActiveForm(),
                     FrmGetObjectIndex(FrmGetActiveForm(), 
                     BTNID_DOWN)), false);
    }
    else
    {
        // 通常の文字列表示
        setnnDA_DAData       (FrmGetActiveForm(), adtP);
    }
    resetDisp_nnDA_DAData(diagFrm, adtP);  //  画面表示後処理

    ////////////////////////////////////////////////////////////
DISPLAY_SCREEN:
    // フォームのタイトルをコピーする
    FrmCopyTitle(diagFrm, adtP->title);

    // Dialog表示
    result = FrmDoDialog(diagFrm);
    if (result == BTNID_DA_OK)
    {
        // OKボタンが押された、、、けど、今回は必ずOKで抜けるはず
    }
    
    ////////////////////////////////////////////////////////////
    //  データの画面設定の後処理
    //
    endnnDA_DAData(diagFrm, adtP);
    //
    ////////////////////////////////////////////////////////////

    // JPEG表示の後処理(必要であれば)
    postProcessJpegDisplay(adtP);

    // フォントサイズを元に戻す
    FntSetFont(fontId);            

    // テンポラリデータバッファを解放する
    if (infoData != NULL)
    {
        MEMFREE_PTR(infoData);
    }

    // フォームとglobal pointerの後始末
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    FtrUnregister(nnDA_Creator, ADT_FTRID);
    MEMFREE_PTR(adtP);
}

/* --------------------------------------------------------------------------
 *   AMsoftさんのUTF8変換テーブルを利用して実現する
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     文字列のコピー(UTF8からSHIFT JISコードへの変換実施) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId)
{
    UInt8     buffer[2];
    UInt16    cnt, codes, ucode;
    Boolean   kanji;
    DmOpenRef dbRef;   

    // 変換テーブルがあるか確認する
    // dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        if (*src < 0x80)
        {
            // 半角モード...
            *dst = *src;
            dst++;
            cnt++;
            src++;
            continue;
        }

        ucode = ((*src & 0x0f) << 12);
        ucode = ucode | ((*(src + 1) & 0x3f) << 6);
        ucode = ucode | ((*(src + 2) & 0x3f));

        buffer[0] = ((ucode & 0xff00) >> 8);
        buffer[1] = ((ucode & 0x00ff));

        // 漢字コードの変換
        codes = Unicode11ToJisx0208(dbRef, buffer[0], buffer[1]);
        buffer[0] = ((codes & 0xff00) >> 8);
        buffer[1] = (codes & 0x00ff);
        ConvertJIStoSJ(dst, buffer);

        dst = dst + 2;
        src = src + 3;
        cnt = cnt + 2;
    }
    *dst = '\0';

    DmCloseDatabase(dbRef);
    return;
}

/* --------------------------------------------------------------------------
 *  Unicode11ToJisx0208()
 *                                                       (Unicode > JIS0208)
 * --------------------------------------------------------------------------*/
static UInt16 Unicode11ToJisx0208(DmOpenRef dbRef, UInt8 utf8H, UInt8 utf8L)
{
    MemHandle            recH;
    UInt16               num, col, code = 0x0000;
    UnicodeToJis0208Rec *rec;

    // データベースのレコードがなかったとき...
    if (dbRef == 0)
    {
        // エラー...
        return (0x2121);
    }
    num  = utf8H;

    // 現物あわせ､､､
    if (utf8L < 0x80)
    {
        num = (utf8H * 2);
        col = utf8L;
    }
    else
    {
        num = (utf8H * 2) + 1;
        col = utf8L - 0x80;
    }
    
    recH = DmQueryRecord(dbRef, num);
    if (recH == 0)
    {
        // エラー...
        return (0x2122);
    }
    rec = (UnicodeToJis0208Rec *) MemHandleLock(recH);
    if (rec->size > 0)
    {
        // データ有り
        code  = rec->table[col];
    }
    else
    {
        // データなし
        code = 0x2121;
    }
    MemHandleUnlock(recH);
    return (code);
}

/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS漢字コードをSHIFT JIS漢字コードに変換する */
/*=========================================================================*/
static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // 上位8ビット/下位8ビットを変数にコピーする
    upper = *ptr;
    lower = *(ptr + 1);

    // 奥村さんのC言語によるアルゴリズム辞典(p.110)のコードを使わせていただく
    if ((upper & 1) != 0)
    {
        if (lower < 0x60)
        {
            lower = lower + 0x1f;
        }
        else
        {
            lower = lower + 0x20;
        }
    }
    else
    {
        lower = lower + 0x7e;
    }   
    if (upper < 0x5f)
    {
        upper = (upper + 0xe1) >> 1;
    }
    else
    {
        upper = (((upper + 0x61) >> 1)|(0x80));
    }
    *dst = upper;
    dst++;
    *dst = lower;

    return (true);
}

/*=========================================================================*/
/*   Function : ConvertEUCtoSJ                                             */
/*                                  漢字コードをEUCからSHIFT JISに変換する */
/*=========================================================================*/
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte)
{
    UInt8 temp[2];

    *copyByte = 0;
    if (*ptr == 0x8e)
    {
        // 半角カナコード
        ptr++;
        *dst = *ptr;
        *copyByte = 1;
        return (true); 
    }
    if ((*ptr >= ((0x80)|(0x21)))&&(*ptr <= ((0x80)|(0x7e))))
    {
        // EUC漢字コードと判定、JIS漢字コードに一度変換してからSHIFT JISに変換
        temp[0] = ((*ptr)&(0x7f));
        ptr++;
        temp[1] = ((*ptr)&(0x7f));
        (void) ConvertJIStoSJ(dst, temp);
        *copyByte = 2;
        return (true);
    }
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function : ParseMessage                                               */
/*                                                  (メッセージの整形処理) */
/*-------------------------------------------------------------------------*/
static void ParseMessage(Char *buf, Char *src, UInt32 size, UInt16 kanjiCode)
{
    Boolean kanjiMode;
    UInt16  len;
    Char   *ptr, *dst, dataStatus;

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // 超遅い解釈ルーチンかも... (１文字づつパースする)
    dst  = buf;
    ptr  = src;

    while (ptr < (src + size))
    {
        // JIS漢字コードの変換ロジック
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS→SHIFT JISのコード変換
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x24')&&(*(ptr + 2) == '\x42'))
            {
                // 漢字モードに切り替え
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x28')&&(*(ptr + 2) == '\x42'))
            {
                // ANKモードに切り替え
                kanjiMode = false;
                ptr = ptr + 3;
                continue;
            }
            if (kanjiMode == true)
            {
                // JIS > SJIS 漢字変換
                ConvertJIStoSJ(dst, ptr);
                ptr = ptr + 2;
                dst = dst + 2;
                continue;
            }
        }
        if (*ptr == '&')
        {
            // "&gt;" を '>' に置換
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&lt;" を '<' に置換
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" を '"' に置換
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" を '    ' に置換
            if ((*(ptr + 1) == 'n')&&(*(ptr + 2) == 'b')&&
                (*(ptr + 3) == 's')&&(*(ptr + 4) == 'p')&&(*(ptr + 5) == ';'))
            {
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&amp;" を '&' に置換
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }
            // これはありえないはずだが...一応。
            *dst++ = '&';
            ptr++;
            continue;
        }
        if (*ptr == '<')
        {
            //  "<>" は、セパレータ(状態によって変わる)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                switch (dataStatus)
                {
                  case MSGSTATUS_NAME:
                    // 名前欄の区切り
                    *dst++ = ' ';
                    *dst++ = ' ';
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_EMAIL;
                    break;

                  case MSGSTATUS_EMAIL:
                    // e-mail欄の区切り
                    *dst++ = ' ';
                    dataStatus = MSGSTATUS_DATE;
                    *dst++ = '\n';
                    break;

                  case MSGSTATUS_DATE:
                    // 年月日・時間およびＩＤ欄の区切り
                    *dst++ = '\n';
                    *dst++ = '\n';
                    dataStatus = MSGSTATUS_NORMAL;
                    if (*ptr == ' ')
                    {
                        // スペースは読み飛ばす
                        ptr++;
                    }
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // メッセージの終端
                    *dst++ = ' ';
                    *dst++ = '\0';
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" は、改行に置換
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                // 行末と行頭のスペースを削ってみる場合
                if ((ptr > src)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    continue;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<p*>" は、改行2つに置換
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                while ((*ptr != '>')&&(ptr < (src + size)))
                {
                    ptr++;
                }
                ptr++;
                continue;
            }

            // <li>タグを改行コードと:に置換する
            if (((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'i')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'L')&&(*(ptr + 2) == 'I')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '･';
                ptr = ptr + 4;
                continue;
            }   

            //  "<hr>" は、改行 === 改行 に置換
            if (((*(ptr + 1) == 'h')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'H')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '=';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dt>"は、改行に置換
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "</tr>" および "</td>" は、改行に置換
            if (((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'r')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'R')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == 'd')&&(*(ptr + 4) == '>'))||
                ((*(ptr + 1) == '/')&&(*(ptr + 2) == 'T')&&(*(ptr + 3) == 'D')&&(*(ptr + 4) == '>')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<dd>" は、改行と空白２つに置換
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" は、改行 + 改行 に置換
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</h?>" は、改行 + 改行 に置換
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            // <tr>, <td> はスペース１つに変換
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                // continueはなし。。 (タグは読み飛ばすため)
            }
SKIP_TAG:
            // その他のタグは読み飛ばす
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            // *dst++ = ' ';   // タグは完全無視とする。
            continue;
        }
        // 漢字コードがEUCだった場合...
        if (kanjiCode == NNSH_KANJICODE_EUC)
        {
            if (ConvertEUCtoSJ((UInt8 *) dst, (UInt8 *) ptr, &len) == true)
            {
                // EUC > SHIFT JIS変換を実行した
                dst = dst + len;
                ptr = ptr + 2;
                continue;
            }
            if (*ptr != '\0')
            {
                // 普通の一文字転写
                *dst++ = *ptr;
            }
            ptr++;
            continue;
        }

        // スペースが連続していた場合、１つに減らす
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < (src + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL および 0x0a, 0x0d, 0x09(タブ) は無視する
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
        {
            // 普通の一文字転写
            *dst++ = *ptr;
        }
        ptr++;
    }
    *dst++ = '\0';
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  getMemoryHandleData                                       */
/*                                      メモリハンドルからデータを取得する */
/*-------------------------------------------------------------------------*/
static Char *getMemoryHandleData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    MemHandle  memH;
    Char      *ptr, *buf;
    Int16      times;
	Int32      size;

    // メモリハンドルを取得する
	ptr  = infoName;
	size  = 0;
    times = 1;
    if (*ptr == '-')
    {
        // 負の数...
        times = -1;
        ptr++;
    }    
	while ((*ptr >= '0')&&(*ptr <= '9'))
	{
		size = (size * 10) + (*ptr - '0');
		ptr++;
	}
	if (size == 0)
	{
        StrNCopy(title, "memH == 0", titleSize);
	    return (NULL);
	}
    size = size * times;
	memH = (MemHandle) size;

	// 漢字コードの決定
    switch (*ptr)
    {
      case 'e':
        // 漢字コードはEUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'E':
        // 漢字コードはEUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        break;

      case 'r':
        // 生データ表示
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'R':
        // 生データ表示
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        break;

      case 'j':
        // 漢字コードはJIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'J':
        // 漢字コードはJIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        break;

      case 's':
        // 漢字コードはSHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      default:
        // その他はSHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        break;
    }
    ptr++;
    ptr++;

    // Titleデータのコピー
    StrNCopy(title, ptr, titleSize);
    ptr = StrStr(title, "<");
    if (ptr != NULL)
    {
        *ptr = '\0';
    }

    // 表示データの取得
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        StrNCopy(title, "MemHandleLock()", titleSize);
        return (NULL);
    }
    size = StrLen(ptr);
    
    // 表示領域の確保
    buf = MEMALLOC_PTR((size * times) + BUF_MARGIN);
    if (buf == NULL)
    {
        StrNCopy(title, "MEMALLOC_PTR()", titleSize);
        MemHandleUnlock(memH);
        return (NULL);
    }
    MemSet (buf, ((size * times) + BUF_MARGIN), 0x00);

	// データの表示コードの確認...
	if (adtP->kanjiCode != nnDA_KANJICODE_RAW)
	{
        // 表示データの整形
        ParseMessage(buf, ptr, size, adtP->kanjiCode);
	}
	else
	{
		// 生データ表示
		StrNCopy(buf, ptr, size);
	}
	MemHandleUnlock(memH);
    return (buf);
}

/*-------------------------------------------------------------------------*/
/*   Function :  getFileStreamHtmlData                                     */
/*                                fileStreamからデータを取得する(HTML加工) */
/*-------------------------------------------------------------------------*/
static Char *getFileStreamHtmlData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    UInt16     len;
    Char      *fileName, *ptr, *buf;
    Err        err;
    FileHand   filRef;
    
    // creatorIDの抽出
    ptr = (Char *) &(adtP->creator);
    *ptr       = infoName[0];
    *(ptr + 1) = infoName[1];
    *(ptr + 2) = infoName[2];
    *(ptr + 3) = infoName[3];

    // ファイル名の長さを検出
    len      = 0;
    ptr      = infoName + 5;  // クリエータID + ":" 分、ポインタを進める

    // 漢字コードの取得
    switch (*ptr)
    {
      case 'e':
        // 漢字コードはEUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'E':
        // 漢字コードはEUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        break;

      case 'r':
        // 生データ表示
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'R':
        // 生データ表示
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        break;

      case 'j':
        // 漢字コードはJIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'J':
        // 漢字コードはJIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        break;

      case 's':
        // 漢字コードはSHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      default:
        // その他はSHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        break;
    }
    ptr++;
    ptr++;

    while (*ptr != '<')
    {
        len++;
        ptr++;
    }
    fileName = MEMALLOC_PTR(len + 4);
    if (fileName == NULL)
    {
        StrNCopy(title, "MEMALLOC_ptr", titleSize);
        return (NULL);
    }
    MemSet  (fileName, (len + 4), 0x00);
    StrNCopy(fileName, (infoName + 5 + 2), len);

    // ファイルをオープンし、データをコピーする
    filRef = FileOpen(0, fileName, 0, adtP->creator, fileModeReadOnly, &err);
    if (filRef == 0)
    {
        // ファイルオープンに失敗したら、エラーコードを表示する
        StrCopy(title, "err:0x");
        StrIToH(&title[StrLen(title)], err);
        return (fileName);
    }

    // ファイルサイズを取得
    if (FileTell(filRef, &(adtP->dataSize), &err) != -1)
    {
        // タイトルは空白に
        StrNCopy(title, " ", titleSize);

        ptr = MEMALLOC_PTR(adtP->dataSize + BUF_MARGIN);
        if (ptr == NULL)
        {
            StrNCopy(title, "MEMALLOC_ptr()", titleSize);
            FileClose(filRef);
            return (fileName);
        }
        MemSet (ptr, (adtP->dataSize + BUF_MARGIN), 0x00);

        FileRead(filRef, ptr, 1, adtP->dataSize, &err);
 
        if (adtP->kanjiCode != nnDA_KANJICODE_RAW)
        {
            len = (adtP->dataSize * adtP->times);
            if (len > DISP_LIMIT)
            {
                // 表示データサイズを丸める...
                len = DISP_LIMIT;
                adtP->dataSize = (adtP->dataSize * 2 / 3);
            }
            buf = MEMALLOC_PTR(len + BUF_MARGIN);
            if (buf != NULL)
            {
                MemSet(buf, (len + BUF_MARGIN), 0x00);

                // 表示データの整形
                ParseMessage(buf, ptr, adtP->dataSize, adtP->kanjiCode);

                // 読み込んだデータバッファは開放する
                MEMFREE_PTR(ptr);
                ptr = buf;
            }
        }
        MEMFREE_PTR(fileName);
    }
    else
    {
        // ファイルサイズの取得に失敗、ファイル名だけを表示する
        StrNCopy(title, "filetell", titleSize);
        ptr = fileName;
    }
    FileClose(filRef);
    return (ptr);
}

/*!
 *   Progress Dialog用のコールバック関数
 */
Boolean textCallBack(PrgCallbackDataPtr cbP)
{
    // 進捗をパーセント表示する
    StrPrintF(cbP->textP, "%d%%...", cbP->stage);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :  displayJpegUsingSonyJpegLib                               */
/*                                       JPEGファイル表示(SonyJpegLib使用) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingSonyJpegLib(FormType *frm, nnDADataType *adtP)
{
#ifdef USE_CLIE
#ifdef USE_SONY_JPEG
    Err          err;
    UInt32       apiVer;

    // SONY JPEGライブラリのオープン
    err = jpegUtilLibOpen(adtP->jpegRef);
    if (err != errNone)
    {
        // SONY JPEGライブラリのオープンに失敗、終了する
        FrmCustomAlert(ALTID_ERROR,
                       "Could not open",
                       "Sony JPEG library",
                       ".");
        return;
    }

    // SONY JPEGライブラリのバージョン確認 (2以上ならば、FileStream読み出し可)
    adtP->bitmapPP = 0;
    apiVer = jpegUtilLibGetLibAPIVersion(adtP->jpegRef);
    if ((apiVer < 2)&&(adtP->sourceLocation == nnDA_JPEGSOURCE_FILESTREAM))
    {
        // jpeg APIが非対応、SONY JPEGライブラリをクローズし終了する
        jpegUtilLibClose(adtP->jpegRef);
        FrmCustomAlert(ALTID_ERROR,
                       "Do not support a stream JPEG file",
                       "at Sony JPEG library",
                       ".");
        return;
    }

    // SONY JPEGライブラリを使用してJPEGファイルを画面描画する
    // (描画後、リソースをクローズする)
    switch (adtP->sourceLocation)
    {
      case nnDA_JPEGSOURCE_FILESTREAM:
        if (adtP->streamRef != 0)
        {
            // 画像を画面に描画する(ファイルストリームから)
            err = jpegUtilLibDecodeImageToWindowForFS(adtP->jpegRef,
                                                      adtP->streamRef,
                                                      jpegDecModeNormal,
                                                      &(adtP->area),
                                                      NULL);
            FileClose(adtP->streamRef);
            adtP->streamRef = 0;
        }
        else
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "File open failure", " > ");
        }
        break;

      case nnDA_JPEGSOURCE_VFS:
        if (adtP->vfsFileRef != 0)
        {
            // 画像を画面に描画する(VFSファイルから)
            err = jpegUtilLibDecodeImageToWindow(adtP->jpegRef,
                                                 adtP->vfsFileRef,
                                                 0,
                                                 jpegDecModeNormal,
                                                 &(adtP->area),
                                                 NULL);
            VFSFileClose(adtP->vfsFileRef);
            adtP->vfsFileRef = 0;
        }
        else
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "File open error", " > ");
        }
        break;

      case nnDA_JPEGSOURCE_MEMORY:
      default:
        // メモリ内部にあるJPEGデータを画面に表示する
#if 0
        if (adtP->jpegData != 0)
        {
            err = jpegUtilLibDecodeImageToWindow(adtP->jpegRef,
                                                 0,
                                                 adtP->jpegData,
                                                 jpegDecModeNormal,
                                                 &(adtP->area),
                                                 NULL);
            MEMFREE_PTR(adtP->jpegData);
        }
        else
#endif
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "not find a jpeg file.", " >");
        }
        break;
    }

    // SONY JPEGライブラリのクローズ
    jpegUtilLibClose(adtP->jpegRef);

    // 描画OKなら、表示する
    if ((err == errNone)&&(adtP->bitmapPP != 0))
    {
        HRWinDrawBitmap(adtP->hrRef,
                        adtP->bitmapPP, 
                        adtP->area.topLeft.x,
                        adtP->area.topLeft.y);
    }
#endif // #ifdef USE_SONY_JPEG
#endif // #ifdef USE_CLIE
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  displayJpegUsingJpegLib                                   */
/*                                         JPEGファイル表示(JpegLibを使用) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingJpegLib(FormType *frm, nnDADataType *adtP)
{
    Err          err;
    UInt16       nc;
    Coord        width, height;
    Char         textBuffer[20];

    // JpegLibのオープン
    err = JpegLibOpen(adtP->jpegRef, jpegLibCurrentVersion);
    if (err != errNone)
    {
        // JPEGライブラリが使用できない...(エラー終了する)...
        FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", "JPEG library", ".");
        return;
    }

    // 「待て」表示をする...
    MemSet (textBuffer, sizeof(textBuffer), 0x00);
    StrCopy(textBuffer, "Please Wait...");
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        HRWinDrawChars(adtP->hrRef, textBuffer, StrLen(textBuffer), adtP->area.topLeft.x, adtP->area.topLeft.y);
    }
    else
#endif
    {
        WinDrawChars(textBuffer, StrLen(textBuffer), adtP->area.topLeft.x, adtP->area.topLeft.y);
    }

    adtP->bitmapPP = 0;
    switch (adtP->sourceLocation)
    {
      case nnDA_JPEGSOURCE_FILESTREAM:
        if (adtP->streamRef != 0)
        {
            // 画像を画面に描画する(FileStreamから)
            JpegLibCreateDataFileStreaming(adtP->jpegRef, &(adtP->data), adtP->streamRef);
            JpegLibCreateDecOptions(adtP->jpegRef, &(adtP->dopt));
            JpegLibSetDecOptDestBitmap(adtP->jpegRef, &(adtP->dopt));
            JpegLibSetDecOptScale(adtP->jpegRef, &(adtP->dopt), 2);
            err = JpegLibRead(adtP->jpegRef, &(adtP->data), &(adtP->dopt), &width, &height, &nc, (void **)&(adtP->bitmapPP));
            FileClose(adtP->streamRef);
            adtP->streamRef = 0;
        }
        else
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "File open failure", " > ");
        }
        break;

      case nnDA_JPEGSOURCE_VFS:
        if (adtP->vfsFileRef != 0)
        {
            // 画像を画面に描画する(VFSファイルから)
            JpegLibCreateDataVFS(adtP->jpegRef, &(adtP->data), adtP->vfsFileRef);
            JpegLibCreateDecOptions(adtP->jpegRef, &(adtP->dopt));
            JpegLibSetDecOptDestBitmap(adtP->jpegRef, &(adtP->dopt));
            JpegLibSetDecOptScale(adtP->jpegRef, &(adtP->dopt), 2);
            err = JpegLibRead(adtP->jpegRef, &(adtP->data), &(adtP->dopt), &width, &height, &nc, (void **)&(adtP->bitmapPP));
            VFSFileClose(adtP->vfsFileRef);
            adtP->vfsFileRef = 0;
        }
        else
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "File open error", " > ");
        }
        break;

      case nnDA_JPEGSOURCE_MEMORY:
      default:
        // メモリ内部にあるJPEGデータを画面に表示する
#if 0
        if (adtP->jpegData != 0)
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "not support memory-data", " > ");
            MEMFREE_PTR(adtP->jpegData);
            adtP->jpegData = 0;
        }
        else
#endif
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "not find a jpeg file.", " >");
        }
        break;
    }

    // ライブラリのクローズ
    JpegLibClose(adtP->jpegRef, &nc);

    // 描画OKなら、Bitmapを表示する
    if ((err == errNone)&&(adtP->bitmapPP != 0))
    {
        //FrmCustomAlert(ALTID_ERROR, "READY ", " TO BITMAP...", ".");
        BmpGetDimensions(adtP->bitmapPP, &width, &height, NULL);

#ifdef USE_CLIE
        if (adtP->hrRef != 0)
        {
            HRWinDrawBitmap(adtP->hrRef,
                            adtP->bitmapPP, 
                            adtP->area.topLeft.x,
                            adtP->area.topLeft.y);
        }
        else
#endif  // #ifdef USE_CLIE
        {
            WinDrawBitmap(adtP->bitmapPP, 
                          adtP->area.topLeft.x,
                          adtP->area.topLeft.y);
        }
    }
    return;
}

#ifdef USE_PNOJPEGLIB2
/*-------------------------------------------------------------------------*/
/*   Function :  displayJpegUsingPnoJpegLib2                               */
/*                   JPEGファイルデータの表示を実施する(pnoJpegLib v2使用) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingPnoJpegLib2(FormType *frm, nnDADataType *adtP)
{
    Err           err;
    Coord         width, height;
    Char          textBuffer[20];

    // jpegLibのオープン
    err = pnoJpegOpen(adtP->jpegRef);
    if (err != errNone)
    {
        // pnoJPEGライブラリが使用できない...(エラー終了する)...
        FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", "pnoJpegLib2", ".");
        return;
    }

    // 「待て」表示をする...
    MemSet (textBuffer, sizeof(textBuffer), 0x00);
    StrCopy(textBuffer, "Please Wait...");
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        HRWinDrawChars(adtP->hrRef, textBuffer, StrLen(textBuffer), adtP->area.topLeft.x, adtP->area.topLeft.y);
    }
    else
#endif
    {
        WinDrawChars(textBuffer, StrLen(textBuffer), adtP->area.topLeft.x, adtP->area.topLeft.y);
    }
    adtP->bitmapPP = 0;

    // ワーク領域の確保
    err = pnoJpeg2Create(adtP->jpegRef, &(adtP->pnoData));
    if (err != errNone)
    {
        // pnoJPEGライブラリ用のワーク領域が確保できない...(エラー終了する)...
        FrmCustomAlert(ALTID_ERROR, "Could not alloc ", "pnoJpegLib2 ", "work area.");
        return;
    }

    switch (adtP->sourceLocation)
    {
      case nnDA_JPEGSOURCE_FILESTREAM:
        if (adtP->streamRef != 0)
        {
            // FileStreamからデータを読み出す...
            err = pnoJpeg2LoadFromFileStream(adtP->jpegRef, adtP->pnoData, adtP->streamRef);
        }
        else
        {
            // ファイルがオープンされていない...終了する　
            err = ~errNone;
        }
        break;

      case nnDA_JPEGSOURCE_VFS:
        // いったんVFSファイルをクローズする
        VFSFileClose(adtP->vfsFileRef);
        adtP->vfsFileRef = 0;

        // VFSファイルから読み出す
        err = pnoJpeg2LoadFromVFS(adtP->jpegRef, adtP->pnoData, adtP->vfsVol, adtP->fileName);
        break;


      case nnDA_JPEGSOURCE_MEMORY:
      default:
        // メモリ内部にあるJPEGデータを画面に表示する
#if 0
        if (adtP->jpegData != 0)
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "not support memory-data", " > ");
            MEMFREE_PTR(adtP->jpegData);
            adtP->jpegData = 0;
        }
        else
#endif
        {
            FrmCustomAlert(ALTID_ERROR, " < ", "not find any JPEG(pnoJpegLib2).", " >");
        }
        err = ~errNone;
        break;
    }

    if (err == errNone)
    {
#if 0
        //err = pnoJpeg2GetInfo(adtP->jpegRef, adtP->pnoData, &width, &height);
        //if (err != errNone)
        {
            width  = adtP->area.extent.x + 2;
            height = adtP->area.extent.y + 2;
            err = errNone;
        }
        if ((width <= adtP->area.extent.x)&&(height <= adtP->area.extent.y))
        {
            // 画像サイズはOK
            err = errNone;
        }
        else
        {
            err = ~errNone;
        }

        if (err != errNone)
        {
            width = width / 2;
            height = height / 2;
            if ((width <= adtP->area.extent.x)&&(height <= adtP->area.extent.y))
            {
                // 画像サイズを半分に
                err = pnoJpeg2SetScaleFactor(adtP->jpegRef, adtP->pnoData, 2);        
            }
            else
            {
                err = ~errNone;
            }
        }

        if (err != errNone)
        {
            width = width / 2;
            height = height / 2;
            if ((width <= adtP->area.extent.x)&&(height <= adtP->area.extent.y))
            {
                // 画像サイズを 1/4 に
                err = pnoJpeg2SetScaleFactor(adtP->jpegRef, adtP->pnoData, 4);        
            }
            else
            {
                err = ~errNone;
            }
        }

        if (err != errNone)
        {
            // 画像サイズを 1/8 に
            (void) pnoJpeg2SetScaleFactor(adtP->jpegRef, adtP->pnoData, 8);
            err = errNone;
        }
#endif
        // pnoJpeg2SetGrayScale(adtP->jpegRef, adtP->pnoData, true);
        err = pnoJpeg2SetMaxDimensions(adtP->jpegRef, adtP->pnoData, adtP->area.extent.x, adtP->area.extent.y);
        if (err == errNone)
        {
            err = pnoJpeg2Read(adtP->jpegRef, adtP->pnoData, &(adtP->bitmapPP));
        }
        //pnoJpeg2Bmp2DoubleDensity(adtP->jpegRef, adtP->bitmapPP, &(adtP->bitmapPPV3));
    }

    // pnoJpegLib2の確保領域を開放する
    pnoJpeg2Free(adtP->jpegRef, &(adtP->pnoData));
    adtP->pnoData = 0;

    // pnoJpeg2ライブラリをクローズする
    pnoJpegClose(adtP->jpegRef);
 
    // 描画OKなら、Bitmapを表示する
    if ((err == errNone)&&(adtP->bitmapPP != 0))
    {
        //FrmCustomAlert(ALTID_ERROR, "READY ", " TO BITMAP...", ".");
        BmpGetDimensions(adtP->bitmapPP, &width, &height, NULL);

#ifdef USE_CLIE
        if (adtP->hrRef != 0)
        {
            HRWinDrawBitmap(adtP->hrRef,
                            adtP->bitmapPP, 
                            adtP->area.topLeft.x,
                            adtP->area.topLeft.y);
        }
        else
#endif  // #ifdef USE_CLIE
        {
            WinDrawBitmap(adtP->bitmapPP, 
                          adtP->area.topLeft.x,
                          adtP->area.topLeft.y);
        }
    }
    return;
}
#else // #ifdef USE_PNOJPEGLIB2
/*-------------------------------------------------------------------------*/
/*   Function :  displayJpegUsingPnoJpegLib1                               */
/*                   JPEGファイルデータの表示を実施する(pnoJpegLib v1使用) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingPnoJpegLib1(FormType *frm, nnDADataType *adtP)
{
    Err          err;
    Coord        width, height;
    Char         textBuffer[20];

    // VFS形式以外は、pnoJpegLibではサポートしない (とりあえず)
    if (adtP->sourceLocation != nnDA_JPEGSOURCE_VFS)
    {
        // jpeg APIが非対応、JPEGライブラリをクローズし終了する
        FrmCustomAlert(ALTID_ERROR,
                       "Do not support a stream JPEG file",
                       "at pnoJpegLib",
                       ".");
        FileClose(adtP->streamRef);
        adtP->streamRef = 0;
        return;
    }

    VFSFileClose(adtP->vfsFileRef);
    adtP->vfsFileRef = 0;

    // jpegLibのオープン
    err = pnoJpegOpen(adtP->jpegRef);
    if (err != errNone)
    {
        // pnoJPEGライブラリが使用できない...(エラー終了する)...
        FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", "pnoJpegLib", ".");
        return;
    }

    // 「待て」表示をする...
    MemSet (textBuffer, sizeof(textBuffer), 0x00);
    StrCopy(textBuffer, "Please Wait...");
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        HRWinDrawChars(adtP->hrRef, textBuffer, StrLen(textBuffer), adtP->area.topLeft.x, adtP->area.topLeft.y);
    }
    else
#endif
    {
        WinDrawChars(textBuffer, StrLen(textBuffer), adtP->area.topLeft.x, adtP->area.topLeft.y);
    }
    adtP->bitmapPP = 0;

    (void) pnoJpegCreate(adtP->jpegRef, &(adtP->pnoData));
    err = pnoJpegLoadFromVFS(adtP->jpegRef, &(adtP->pnoData), adtP->vfsVol, adtP->fileName);
    if(err == errNone)
    {
         // pnoJpegSetGrayScale(adtP->jpegRef,   &(adtP->pnoData), true);
         pnoJpegSetScaleFactor(adtP->jpegRef, &(adtP->pnoData), 2);
         pnoJpegRead(adtP->jpegRef, &(adtP->pnoData), &(adtP->bitmapPP));
         pnoJpegBmp2DoubleDensity(adtP->jpegRef, &(adtP->bitmapPP));
    }

    // pnoJpegライブラリをクローズする
    pnoJpegClose(adtP->jpegRef);

    // 描画OKなら、Bitmapを表示する
    if ((err == errNone)&&(adtP->bitmapPP != 0))
    {
        //FrmCustomAlert(ALTID_ERROR, "READY ", " TO BITMAP...", ".");
        BmpGetDimensions(adtP->bitmapPP, &width, &height, NULL);

#ifdef USE_CLIE
        if (adtP->hrRef != 0)
        {
            HRWinDrawBitmap(adtP->hrRef,
                            adtP->bitmapPP, 
                            adtP->area.topLeft.x,
                            adtP->area.topLeft.y);
        }
        else
#endif  // #ifdef USE_CLIE
        {
            WinDrawBitmap(adtP->bitmapPP, 
                          adtP->area.topLeft.x,
                          adtP->area.topLeft.y);
        }
    }
    return;
}
#endif  // #ifdef USE_PNOJPEGLIB2

/*-------------------------------------------------------------------------*/
/*   Function :  getJpegFileData                                           */
/*                                  JPEGファイルデータのオープンを実施する */
/*-------------------------------------------------------------------------*/
static Char *getJpegFileData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    Err     ret;
    Char   *ptr, *ptr2;
    Boolean result;

    // SHOWJPEG:xRRRR:/...(fileName)...
    //   x: 読み込みファイルのありか(ファイルストリーム or VFSの指定)
    //       f : ファイルストリーム
    //       v : VFS (指定なし)
    //       c : VFS (CF)
    //       m : VFS (MS)
    //       s : VFS (SD)
    //       t : VFS (SmartMedia ... ToshibaのT)
    //       r : VFS (RD  : ramdisk)
    //       d : VFS (DoC : 内蔵デバイス)
    //   RRRR: 予約領域
    //   /...(fileName)...  ファイル名称
    adtP->streamRef  = 0;
    adtP->vfsFileRef = 0;
    adtP->bitmapPP   = 0; 
    switch (infoName[0])
    {
      case 'f': 
        // file stream
        return (openFileStreamForJPEG(infoName, title, titleSize, adtP));
        break;

      case 'v': 
        // VFS(any)
        result = checkVFSdevice(0, adtP);
        break;

      case 'c': 
        // VFS(CF)
        result = checkVFSdevice(ExpMediaType_CompactFlash, adtP);
        break;

      case 'm': 
        // VFS(MS)
        result = checkVFSdevice(ExpMediaType_MemoryStick, adtP);
        break;

      case 's': 
        // VFS(SD)
        result = checkVFSdevice(ExpMediaType_SecureDigital, adtP);
        if (result == false)
        {
            result = checkVFSdevice(ExpMediaType_MultiMediaCard, adtP);
        }
        break;

      case 't': 
        // VFS(SmartMedia)
        result = checkVFSdevice(ExpMediaType_SmartMedia, adtP);
        break;

      case 'r': 
        // VFS(RamDisk)
        result = checkVFSdevice(ExpMediaType_RAMDisk, adtP);
        break;

      case 'd': 
        // VFS(DoC)
        result = checkVFSdevice('Tffs', adtP);
        break;

      default:
        return (NULL);
        break;
    }
    if (result == false)
    {
        //  VFS未検出、終了する
        FrmCustomAlert(ALTID_ERROR, " not found proper VFS volume ", "", "");
        return (NULL);
    }

    // ファイル名を 変数ファイル名にコピー
    MemSet(adtP->fileName, (nnDA_FILENAMESIZE + 2), 0x00);
    ptr  = adtP->fileName;
    ptr2 = &(infoName[6]);
    while ((*ptr2 != '<')&&(*ptr2 != '\0')&&(ptr < (adtP->fileName + nnDA_FILENAMESIZE)))
    {
        *ptr = *ptr2;
        ptr++;
        ptr2++;
    }

    // ファイルをオープンする(VFS)
    ret = VFSFileOpen(adtP->vfsVol,adtP->fileName,vfsModeRead,&(adtP->vfsFileRef));
    if (ret != errNone)
    {
        // ファイルオープンに失敗したら、ファイル名だけ表示する
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], ret);
        adtP->vfsFileRef = 0;
        goto FUNC_ABEND;
    }

    // ファイルのオープン成功！（参照番号を構造体にコピーする)
    adtP->sourceLocation = nnDA_JPEGSOURCE_VFS;

   return (NULL);

FUNC_ABEND:
    ptr = MEMALLOC_PTR(StrLen(adtP->fileName) + 4);
    if ((ptr != NULL)&&(StrLen(adtP->fileName) != 0))
    {
        MemSet (ptr, (StrLen(adtP->fileName) + 4), 0x00);
        MemMove(ptr, adtP->fileName, StrLen(adtP->fileName));
    }
    return (ptr);
}

/*-------------------------------------------------------------------------*/
/*   Function :  postProcessJpegDisplay                                    */
/*                                          JPEGファイルデータの表示後処理 */
/*-------------------------------------------------------------------------*/
static void postProcessJpegDisplay(nnDADataType *adtP)
{
    if (adtP->vfsFileRef != 0)
    {
        VFSFileClose(adtP->vfsFileRef);
        adtP->vfsFileRef = 0;
    }    
    if (adtP->streamRef != 0)
    {
        FileClose(adtP->streamRef);
        adtP->streamRef = 0;
    }    
    if (adtP->bitmapPP != 0)
    {
        BmpDelete(adtP->bitmapPP);
        adtP->bitmapPP = 0;
    }
    if (adtP->bitmapPPV3 != 0)
    {
        BmpDelete((BitmapType *) adtP->bitmapPPV3);
        adtP->bitmapPPV3 = 0;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :   checkVFSdevice                                           */
/*                 VFSのサポートを確認(adtP->vfsVolにVFS volumeを格納する) */
/*-------------------------------------------------------------------------*/
static Boolean checkVFSdevice(UInt32 aMediaType, nnDADataType *adtP)
{
    Err err;

    // VFSのサポート有無を確認する。
    adtP->vfsVolumeIterator = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &(adtP->vfsVolumeIterator));
    if (err != errNone)
    {
        adtP->vfsVol = 0;
        return (false);
    }

    //  VFSが使用可能か確認する。
    adtP->vfsVolumeIterator = vfsIteratorStart;
    while (adtP->vfsVolumeIterator != vfsIteratorStop)
    {
        err = VFSVolumeEnumerate(&(adtP->vfsVol), &(adtP->vfsVolumeIterator));
        if (err != errNone)
        {
            adtP->vfsVol = 0;
	    return (false);
        }
        err = VFSVolumeInfo(adtP->vfsVol, &(adtP->volInfo));
        if (err != errNone)
        {
            adtP->vfsVol = 0;
            return (false);
        }
        if ((aMediaType == 0)||(aMediaType == adtP->volInfo.mediaType))
        {
            // ファイルシステムの確認
            if (adtP->volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // VFSデバイスを発見！
                return (true);                
            }
        }
    }
    // VFSデバイスが見つからなかった...
    adtP->vfsVol = 0;
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  openFileStreamForJPEG                                     */
/*                      JPEGファイルデータのオープンを実施する(FileStream) */
/*-------------------------------------------------------------------------*/
static Char *openFileStreamForJPEG(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    UInt32  creator;
    Char   *ptr, *ptr2;
    Err     err;

    ptr = (Char *) &creator;
    *ptr       = infoName[1];
    *(ptr + 1) = infoName[2];
    *(ptr + 2) = infoName[3];
    *(ptr + 3) = infoName[4];

    // ファイル名を取得する
    ptr  = adtP->fileName;
    ptr2 = &(infoName[6]);
    while ((*ptr2 != '<')&&(*ptr2 != '\0'))
    {
        *ptr = *ptr2;
        ptr++;
        ptr2++;
    }

    // ファイルをオープンする(fileStream)
    adtP->streamRef = FileOpen(0,adtP->fileName, 0, creator, fileModeReadOnly, &err);
    if (adtP->streamRef == 0)
    {
        // ファイルオープンに失敗したら、ファイル名だけ表示する
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], err);
        goto FUNC_ABEND;
    }
    adtP->sourceLocation = nnDA_JPEGSOURCE_FILESTREAM;
    return (NULL);

FUNC_ABEND:
    ptr = MEMALLOC_PTR(StrLen(adtP->fileName) + 4);
    if ((ptr != NULL)&&(StrLen(adtP->fileName) != 0))
    {
        MemSet (ptr, (StrLen(adtP->fileName) + 4), 0x00);
        MemMove(ptr, adtP->fileName, StrLen(adtP->fileName));
    }
    return (ptr);
}

/*-------------------------------------------------------------------------*/
/*   Function :  checkPnoJpegLibInstalled                                  */
/*                            pnoJpegLibがインストールされているか確認する */
/*-------------------------------------------------------------------------*/
static Boolean checkPnoJpegLibInstalled(UInt16 *jpegRef)
{
    Err err;

    // pnoJpegLibが使用可能か確認する
    err = SysLibFind(pnoJpegName, jpegRef);
    if (err != errNone) // not already loaded; do it now
    {
        err = SysLibLoad(pnoJpegTypeID, pnoJpegCreatorID, jpegRef);
    }
    if (err == errNone)
    {
        // pnoJpegLibがインストールされていた
        return (true);
    }
    // pnoJpegLibは未インストール、、
    return (false);    
}

/*-------------------------------------------------------------------------*/
/*   Function :  checkJpegLibInstalled                                     */
/*                               JpegLibがインストールされているか確認する */
/*-------------------------------------------------------------------------*/
static Boolean checkJpegLibInstalled(UInt16 *jpegRef)
{
    Err err;

    // JpegLibが使用可能か確認する
    err = SysLibFind(jpegLibName, jpegRef);
    if (err != errNone) // not already loaded; do it now
    {
        err = SysLibLoad(sysResTLibrary, jpegLibCreator, jpegRef);
    }
    if (err == errNone)
    {
        // JpegLibが使用可能、そっちを使用して画像を表示する
        return (true);
    }
    // JpegLibが使用できない...終了する
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  checkSonyJpegLibInstalled                                 */
/*                           SonyJpegLibがインストールされているか確認する */
/*-------------------------------------------------------------------------*/
static Boolean checkSonyJpegLibInstalled(UInt16 *jpegRef)
{
    Err err;

    err = SysLibFind(sonySysLibNameJpegUtil, jpegRef);
    if (err != errNone)
    {
        err = SysLibLoad(sonySysFileTJpegUtilLib,
                         sonySysFileCJpegUtilLib, jpegRef);
    }
    if (err == errNone)
    {
        // Sony JpegLibが使用可能、そっちを使用して画像を表示する
        return (true);
    }
    // Sony JpegLibが使用できない...終了する
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  resizeDialogWindow                                        */
/*                                        ダイアログウィンドウサイズの調整 */
/*-------------------------------------------------------------------------*/
static void resizeDialogWindow(FormType *diagFrm, nnDADataType *adtP)
{
    UInt16         objIndex;
    Coord          x, y, diffX, diffY;
    RectangleType  r;

    /* 全体のWindowサイズ取得 */
    WinGetDisplayExtent(&x, &y);

    /* dialogのサイズ取得 */
    FrmGetFormBounds(diagFrm, &r);

    /* ウィンドウの拡大サイズを求める */
    r.topLeft.x = 2;
    r.topLeft.y = 3;
    diffX       = r.extent.x;
    diffY       = r.extent.y;
    r.extent.y  = y - 6;
    r.extent.x  = x - 4;
    
    /* オブジェクトの移動サイズを求める */
    diffX = r.extent.x - diffX;
    diffY = r.extent.y - diffY;

    /* dialogの拡大 */
    WinSetWindowBounds(FrmGetWindowHandle(diagFrm), &r);

    /* OK ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_OK);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 上 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_UP);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 左 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_PREV);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 右 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_NEXT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* 下 ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DOWN);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* SHIFT ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_SHIFT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    // 初期値は、上下移動モードに設定
    CtlSetEnabled(FrmGetObjectPtr(diagFrm, objIndex), false);
    CtlSetUsable (FrmGetObjectPtr(diagFrm, objIndex), false);
    FrmHideObject(diagFrm, objIndex);                

    /* About ボタンの移動 */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_ABOUT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* gadgetの拡大 */
    objIndex = FrmGetObjectIndex(diagFrm, GADID_DA_VIEW);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    r.extent.x = r.extent.x + diffX;
    r.extent.y = r.extent.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  createDialogWindow                                        */
/*                                 DIAサポートのチェックとダイアログの生成 */
/*-------------------------------------------------------------------------*/
static Boolean createDialogWindow(FormType **diagFrm, nnDADataType *adtP)
{
#ifdef USE_DIA
    UInt32 diaVar;
    UInt16 objIndex, result;

    // DIAサポートがあるかどうか確認する
    result = FtrGet(pinCreator, pinFtrAPIVersion, &diaVar);
    if (result != errNone)
    {
        // DIA 非サポートデバイス、ダイアログサイズの拡大はしない
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // DIAサポートデバイス 、シルクが動的に動作可能か確認する
    result = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &diaVar);
    if ((result != errNone)|
        ((diaVar & grfFtrInputAreaFlagDynamic) == 0)||
        ((diaVar & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // エラーになる場合、もしくは動的シルク非サポートの場合、何もせず抜ける
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // 現在のシルク状態を記憶する
    objIndex = PINGetInputAreaState();

    // ダイアログのオープン
    *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
    FrmSetActiveForm(*diagFrm);

    // シルク制御を有効にする
    FrmSetDIAPolicyAttr(*diagFrm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // えらいベタ、、、ウィンドウサイズの最大値最小値を設定
    WinSetConstraintsSize(FrmGetWindowHandle(*diagFrm),
                          160, 225, 225, 160, 225, 225);

    // シルク領域を保存していたものにする
    PINSetInputAreaState(objIndex);

#else  // #ifdef USE_DIA
    // Viewer Formの初期化
    *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
    FrmSetActiveForm(*diagFrm);
#endif  // #ifdef USE_DIA

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckClipboard                                           */
/*                                                   クリップボードの確認  */
/*-------------------------------------------------------------------------*/
static Boolean CheckClipboard(nnDADataType *adtP, Char **infoName)
{
    // TODO ： クリップボードの連携をしない場合は、そのまま文字列を
    //         応答するように変更する必要がある。

    MemHandle  memH;
    UInt16     len;
    Char      *areaP, *ptr, *infoData;
    Boolean    result;
    
    result    = false;
    *infoName = NULL;

    len  = 0;
    memH = ClipboardGetItem(clipboardText, &len);
    if ((memH == 0)||(len == 0))
    {
        // クリップボードにデータがない、終了する
        return (false);
    }
    areaP = MemHandleLock(memH);
    if (areaP == NULL)
    {
        // クリップボードのポインタ取得失敗、終了する
        goto FUNC_END;
    }

    ///////////////////// NNsiとの連携処理 ここから
    if (StrNCompare(areaP, nnDA_NNSIEXT_VIEWSTART, StrLen(nnDA_NNSIEXT_VIEWSTART)) != 0)
    {
        // NNsiとの連携はしない、終了する
        goto FUNC_END;
    }

    // データ文字列の先頭を検索
    ptr = StrStr(areaP, nnDA_NNSIEXT_INFONAME);
    if ((ptr == NULL)||(ptr > (areaP + len)))
    {
        // データ文字列の先頭の検出に失敗、終了する
        goto FUNC_END;
    }

    // データの先頭に移動する
    ptr = ptr + StrLen(nnDA_NNSIEXT_INFONAME);

    // データから情報名を切り出す
    *infoName = MEMALLOC_PTR(BUFSIZE);
    if (*infoName == NULL)
    {
        // 格納領域の確保に失敗、終了する
        goto FUNC_END;
    }
    MemSet(*infoName, BUFSIZE, 0x00);
    infoData = *infoName;
    while ((*ptr != '<')&&
            (ptr < areaP + len)&&
            (infoData < (*infoName + BUFSIZE - 1)))
    {
        *infoData = *ptr;
        ptr++;
        infoData++;
    }
    *infoData = '\0';
    result = true;
    ///////////////////// NNsiとの連携処理 ここまで

FUNC_END:
    // クリップボード領域を開放する
    if (memH != 0)
    {
        MemHandleUnlock(memH);
    }
    return (result);
}

/*-------------------------------------------------------------------------*/
/*   Function :   copyClipboard                                            */
/*                                             クリップボードの内容コピー  */
/*-------------------------------------------------------------------------*/
static Char *copyClipboard(void)
{
    MemHandle memH;
    UInt16    len;
    Char     *ptr;

    // クリップボードに連携文字列がなかった...
    memH = ClipboardGetItem(clipboardText, &len);
    if ((memH == 0)||(len == 0))
    {
        // クリップボードにデータがない、終了する
        return (NULL);
    }

    // クリップボードのデータを描画用にコピーする
    ptr = MEMALLOC_PTR(len + 4);
    if (ptr != NULL)
    {
        MemSet (ptr, (len + 4), 0x00);
        MemMove(ptr, MemHandleLock(memH), len);
        MemHandleUnlock(memH);
    }
    return (ptr);
}
