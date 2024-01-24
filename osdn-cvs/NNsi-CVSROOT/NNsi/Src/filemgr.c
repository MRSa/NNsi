/*============================================================================*
 *
 *  $Id: filemgr.c,v 1.9 2004/03/08 13:57:32 mrsa Exp $
 *
 *  FILE: 
 *     fileMgr.c
 *
 *  Description: 
 *     ファイルマネージャ
 *===========================================================================*/
#define FILEMGR_C
#include "local.h"

static void SetFullPathFileName_NNsh(Char *file, Char *fullPath, UInt16 size);
static Err openFile_VFSFile(Char *fileName, UInt16 fileMode, NNshFileRef *ref);
static Err openFile_FileStream(Char *name, UInt16 fileMode, NNshFileRef *ref);

/*==========================================================================*/
/*  CheckFileSpace_NNsh() : ファイル空き容量の確認                          */
/*                                                                          */
/*==========================================================================*/
Err CheckFileSpace_NNsh(UInt16 fileMode, UInt32 size)
{
    Err    ret = errNone;
    UInt32 useSize, totalSize;

    if (((fileMode & NNSH_FILEMODE_TEMPFILE) == 0)&&
        (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0))
    {
        // VFSの空き容量を確認する
        ret = VFSVolumeSize(NNshGlobal->vfsVol, &useSize, &totalSize);
        if (ret == errNone)
        {
            if ((size + FREE_SPACE_MARGIN) > (totalSize - useSize))
            {
                // 空き容量不足、エラー応答する
                NNsh_ErrorMessage(ALTID_ERROR, MSG_VFSSPACE_INSUFFICIENT,
                                  "", (totalSize - useSize));
                return (~errNone);
            }
        }
    }
    else
    {
        // Palm本体内の空きスペースは確認しない。
    }
    return (ret);
}

/*==========================================================================*/
/*  closeFile_NNsh() : ファイルクローズ                                     */
/*                                                                          */
/*==========================================================================*/
Err CloseFile_NNsh(NNshFileRef *fileRef)
{
    Err ret = errNone;

    if (fileRef->fileLocation == NNSH_VFS_ENABLE)
    {
        ret = VFSFileClose(fileRef->vfsRef);
        NNsh_DebugMessage(ALTID_INFO, "VFS file close ", "", ret);
    }
    else
    {
        ret = FileClose(fileRef->streamRef);
        NNsh_DebugMessage(ALTID_INFO, "stream file close ", "", ret);
    }

    // ファイルを使えないようにする。
    fileRef->streamRef = 0;
    fileRef->vfsRef    = 0;
    return (ret);
}

/*==========================================================================*/
/*  openFile_NNsh() : ファイルオープン                                      */
/*                                                                          */
/*==========================================================================*/
Err OpenFile_NNsh(Char *fileName, UInt16 fileMode, NNshFileRef *fileRef)
{
    Err ret;
    MemSet(fileRef, sizeof(NNshFileRef), 0x00);

    // ファイルオープン先の確認
    if (((fileMode & NNSH_FILEMODE_TEMPFILE) == 0)&&
        (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0))
    {
        // VFS ファイルオープン要求
        NNsh_DebugMessage(ALTID_INFO, "VFS file Open:", fileName, 0);
        fileRef->fileLocation = NNSH_VFS_ENABLE;
        ret = openFile_VFSFile(fileName, fileMode, fileRef);
    }
    else
    {
        // stream ファイルオープン要求
        NNsh_DebugMessage(ALTID_INFO, "Stream file Open :", fileName, 0);
        fileRef->fileLocation = NNSH_VFS_DISABLE;
        ret = openFile_FileStream(fileName, fileMode, fileRef);
    }
    return (ret);
}

/*==========================================================================*/
/*  GetFileSize_NNsh() : ファイルサイズの取得                               */
/*                                                                          */
/*==========================================================================*/
Err GetFileSize_NNsh(NNshFileRef *fileRef, UInt32 *fileSize)
{
    Int32 size;
    Err   ret;

    *fileSize = 0;

    if (fileRef->fileLocation == NNSH_VFS_ENABLE)
    {
        ret =  VFSFileSize(fileRef->vfsRef, fileSize);
    }
    else
    {
        size = 0;
        if (FileTell(fileRef->streamRef, &size, &ret) != -1)
        {
            *fileSize = size;
        }
        else
        {
            ret = errNone;
        }
    }
    return (ret);
}

/*==========================================================================*/
/*  ReadFile_NNsh() : ファイルからデータの読み出し                          */
/*                                                                          */
/*==========================================================================*/
Err ReadFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size, 
                  void *ptr, UInt32 *readSize)
{
    Err ret = errNone;

    *readSize = 0;

    if (fileRef->fileLocation == NNSH_VFS_ENABLE)
    {
        if (fileRef->vfsRef == 0)
        {
            return (~errNone);
        }
        (void) VFSFileSeek(fileRef->vfsRef, vfsOriginBeginning, offset);
        ret = VFSFileRead(fileRef->vfsRef, size, ptr, readSize);
    }
    else
    {
        if (fileRef->streamRef == 0)
        {
            return (~errNone);
        }
        (void) FileSeek(fileRef->streamRef, offset, fileOriginBeginning);
        *readSize = FileRead(fileRef->streamRef, ptr, 1, size, &ret);
    }
    return (ret);
}

/*--------------------------------------------------------------------------*/
/*  SetFullPathFileName_NNsh() : ファイル名称をフルパス指定に拡張           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static void SetFullPathFileName_NNsh(Char *file, Char *fullPath, UInt16 size)
{
    MemSet (fullPath, size, 0x00);
    if (*file != '/')
    {
        // 相対パス指定だった場合には、ベースディレクトリを頭につける
        StrCopy(fullPath, DATAFILE_PREFIX);
        StrCat (fullPath, file);
    }
    else
    {
        // 絶対パスが指定されていた場合には、そのままコピー
        StrCopy(fullPath, file);
    }
    return;    
}

/*--------------------------------------------------------------------------*/
/*  openFile_VFSFile() : ファイルオープン(VFS File用)                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Err openFile_VFSFile(Char *fileName, UInt16 fileMode, NNshFileRef *ref)
{
    Err     ret;
    UInt16  mode;
    Char    fullPath[MAXLENGTH_FILENAME];

    // ファイルオープンモードのチェック
    switch (fileMode)
    {
      case NNSH_FILEMODE_READONLY:
      case (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE):
        mode = vfsModeRead;
        break;

      case NNSH_FILEMODE_CREATE:
      case (NNSH_FILEMODE_CREATE|NNSH_FILEMODE_TEMPFILE):
        // 新規作成指示の場合には、ファイルを消してからオープンする
        (void) DeleteFile_NNsh(fileName, NNSH_VFS_ENABLE);
        mode = vfsModeReadWrite;
        break;

      case NNSH_FILEMODE_APPEND:
      case (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE):
      case NNSH_FILEMODE_READWRITE:
      case (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE):
      default:
        mode = vfsModeReadWrite;
        break;
    }

    // FULL PATH指定に拡張
    SetFullPathFileName_NNsh(fileName, fullPath, sizeof(fullPath));
    ret = VFSFileOpen(NNshGlobal->vfsVol, fullPath, mode, &(ref->vfsRef));
    if (ret == vfsErrFileNotFound)
    {
        // ファイルが存在しなかった場合は、新規作成してもう一回
        (void) CreateFile_NNsh(fullPath);
        ret = VFSFileOpen(NNshGlobal->vfsVol, fullPath, mode, &(ref->vfsRef));
    }
    if (ret != errNone)
    {
        ref->vfsRef = 0;
    }
    return (ret);
}

/*--------------------------------------------------------------------------*/
/*  openFile_FileStream() : ファイルオープン(File Stream用)                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Err openFile_FileStream(Char *name, UInt16 fileMode, NNshFileRef *ref)
{
    Err    err;
    UInt32 openMode;

    // ファイルオープンモードのチェック
    switch (fileMode)
    {
      case NNSH_FILEMODE_READONLY:
      case (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE):
        openMode = fileModeReadOnly;
        break;

      case NNSH_FILEMODE_APPEND:
      case (NNSH_FILEMODE_APPEND|NNSH_FILEMODE_TEMPFILE):
        openMode = fileModeAppend;
        break;

      case NNSH_FILEMODE_CREATE:
      case (NNSH_FILEMODE_CREATE|NNSH_FILEMODE_TEMPFILE):
      case NNSH_FILEMODE_READWRITE:
      case (NNSH_FILEMODE_READWRITE|NNSH_FILEMODE_TEMPFILE):
      default:
        openMode = fileModeReadWrite;
        break;
    }

    ref->streamRef = FileOpen(0, name, 0, 0, openMode, &err);
    if (ref->streamRef == 0)
    {
        return (err);
    }
    return (errNone);
}

/*==========================================================================*/
/*  DeleteFile_NNsh() : ファイルデリート                                    */
/*                                                                          */
/*==========================================================================*/
Err DeleteFile_NNsh(Char *fileName, UInt16 location)
{
    Err  ret;
    Char fullPath[MAXLENGTH_FILENAME];

    if ((location != NNSH_VFS_DISABLE)&&
        (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0))
    {
        // VFS上のファイルを削除する
        SetFullPathFileName_NNsh(fileName, fullPath, sizeof(fullPath));
        ret = VFSFileDelete(NNshGlobal->vfsVol, fullPath);
    }
    else
    {
        // File Stream上のファイルを削除する
        ret = FileDelete(0, fileName);
    }
    return (ret);
}

/*==========================================================================*/
/*  createFile_NNsh() : ファイル作成 (VFS Only)                             */
/*                                                                          */
/*==========================================================================*/
Err CreateFile_NNsh(Char *fileName)
{
    Err  ret = errNone;
    Char fullPath[MAXLENGTH_FILENAME];

    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS上にファイルを作成する
        SetFullPathFileName_NNsh(fileName, fullPath, sizeof(fullPath));
        ret = VFSFileCreate(NNshGlobal->vfsVol, fullPath);
    }
    return (ret);
}

/*==========================================================================*/
/*  renameFile_NNsh() : ファイルリネーム(VFS Only)                          */
/*                                                                          */
/*==========================================================================*/
Err RenameFile_NNsh(Char *oldFileName, Char *newFileName)
{
    Err  ret = errNone;
    Char oldFullPath[MAXLENGTH_FILENAME];
    Char newFullPath[MAXLENGTH_FILENAME];

    if ((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE)
    {
        // VFS上のファイル名を変更する
        SetFullPathFileName_NNsh(oldFileName,oldFullPath,sizeof(oldFullPath));
        SetFullPathFileName_NNsh(newFileName,newFullPath,sizeof(newFullPath));
        ret = VFSFileRename(NNshGlobal->vfsVol, oldFullPath, newFullPath);
    }
    return (ret);
}

/*==========================================================================*/
/*  CreateDir_NNsh() : ディレクトリの作成(VFS Only)                         */
/*                                                                          */
/*==========================================================================*/
Err CreateDir_NNsh(Char *dirName)
{
    Err  ret = errNone;
    Char fullPath[MAXLENGTH_FILENAME];

    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS上にディレクトリを作成する
        SetFullPathFileName_NNsh(dirName, fullPath, sizeof(fullPath));
        ret = VFSDirCreate(NNshGlobal->vfsVol, fullPath);
    }
    return (ret);
}


/*==========================================================================*/
/*  CopyFile_NNsh() : ファイルのコピー(VFS Only)                            */
/*                                                                          */
/*==========================================================================*/
Err CopyFile_NNsh(Char *destFile, Char *sourceFile, UInt16 location)
{
    NNshFileRef srcRef, destRef;
    UInt16      limit, fileModeR, fileModeW;
    UInt32      offsetW, offsetR, readSize, dummy;
    Err         ret = errNone;
    Char       *buffer;

    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) == 0)
    {
        location = NNSH_COPY_PALM_TO_PALM;      
    }

    // コピータイプにあわせてファイルモードを変更する
    switch (location)
    {
      case NNSH_COPY_PALM_TO_VFS:
        // Palm本体からVFSへコピーする場合
        fileModeR =  NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
        fileModeW =  NNSH_FILEMODE_CREATE;
        break;

      case NNSH_COPY_VFS_TO_VFS:
        // VFSからVFSへコピーする場合
        fileModeR = NNSH_FILEMODE_READONLY;
        fileModeW = NNSH_FILEMODE_CREATE;
        break;

      case NNSH_COPY_VFS_TO_PALM:
        // VFSからPalm本体へコピーする場合
        fileModeR = NNSH_FILEMODE_READONLY;
        fileModeW = NNSH_FILEMODE_CREATE | NNSH_FILEMODE_TEMPFILE;
        break;

      case NNSH_COPY_PALM_TO_PALM:
      default:
        // Palm本体からPalm本体へコピーする場合
        fileModeR = NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
        fileModeW = NNSH_FILEMODE_CREATE   | NNSH_FILEMODE_TEMPFILE;
        break;        
    }

    // 送り元ファイルのオープン
    ret = OpenFile_NNsh(sourceFile, fileModeR, &srcRef);
    if (ret != errNone)
    {
        return (ret);
    }

    // 送り先ファイルのオープン
    ret = OpenFile_NNsh(destFile, fileModeW, &destRef);
    if (ret != errNone)
    {
        return (ret);
    }

    // コピー用ワーク領域を確保する
    limit = (NNshGlobal->NNsiParam)->bufferSize - (NNshGlobal->NNsiParam)->bufferSize % 2;
    do
    {
        limit  = limit - 8;
        buffer = (Char *) MEMALLOC_PTR(limit + 4);
    } while (buffer == NULL);

    offsetW = 0;
    offsetR = 0;
    ret     = errNone;

    // 正常に読み込めたときは、ファイルをコピーする
    while (ret == errNone)
    {
        readSize = 0;
        MemSet(buffer, limit, 0x00);
        ret  = ReadFile_NNsh (&srcRef, offsetR, limit, buffer, &readSize);
        offsetR = offsetR + readSize;
        if ((readSize % 2) != 0)
        {
            // 書き込みサイズを偶数に補正する(対CLIE用の保険)
            if (((fileModeW & NNSH_FILEMODE_TEMPFILE) == 0)&&
                (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_WORKAROUND) != 0))
            {
                buffer[readSize] = ' ';
                readSize++;
            }
        }
        (void) WriteFile_NNsh(&destRef,offsetW, readSize, buffer, &dummy);
        offsetW = offsetW + readSize;
    }

    // ワーク領域を開放する
    MEMFREE_PTR(buffer);

    // ファイルクローズ
    CloseFile_NNsh(&srcRef);
    CloseFile_NNsh(&destRef);

    // ファイルの最後までコピーした時はerrNoneで応答する
    if ((ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        ret = errNone;
    }

    return (ret);
}

/*==========================================================================*/
/*  WriteFile_NNsh() : ファイルへデータの書き出し                           */
/*                                                                          */
/*==========================================================================*/
Err WriteFile_NNsh(NNshFileRef *fileRef, UInt32 offset, UInt32 size,
                                                void *ptr, UInt32 *writeSize)
{
    Err  ret = errNone;

    *writeSize = 0;

    if (fileRef->fileLocation == NNSH_VFS_ENABLE)
    {
        if (fileRef->vfsRef == 0)
        {
            return (~errNone);
        }

        //  一部機種にVFSFileWrite問題が存在するため、ファイル書き出し時に
        // その条件に合致した場合、WARNINGを表示する。(対策はしない。)
        if ((offset % 2) == 1)
        {
            NNsh_DebugMessage(ALTID_WARN, "OFFSET is odd number! ","", offset);
        }
        if ((size % 2) == 1)
        {
            NNsh_DebugMessage(ALTID_WARN, "SIZE is odd number! ", "", size);
        }
        (void) VFSFileSeek (fileRef->vfsRef, vfsOriginBeginning, offset);
        ret  = VFSFileWrite(fileRef->vfsRef, size, ptr, writeSize);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "VFSFileWrite() ", "ret:", ret);
            *writeSize = 0;
        }
    }
    else
    {
        if (fileRef->streamRef == 0)
        {
            return (~errNone);
        }
        (void) FileSeek(fileRef->streamRef, offset, fileOriginBeginning);
        *writeSize = FileWrite(fileRef->streamRef, ptr, 1, size, &ret);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "FileWrite() ", " ret:", ret);
            *writeSize = 0;
        }
    }
    return (ret);
}

/*==========================================================================*/
/*  appendFile() : ファイルの末尾に書き出し                                 */
/*                                                                          */
/*==========================================================================*/
Err AppendFile_NNsh(NNshFileRef *fileRef, UInt32 size, 
                    void *ptr, UInt32 *writeSize)
{
    Err  ret = errNone;

    *writeSize = 0;
    if (fileRef->fileLocation == NNSH_VFS_ENABLE)
    {
        if (fileRef->vfsRef == 0)
        {
            return (~errNone);
        }

        //  一部機種にVFSFileWrite問題が存在するため、ファイル書き出し時に
        // その条件に合致した場合、WARNINGを表示する。(対策はしない。)
        if ((size % 2) == 1)
        {
            NNsh_DebugMessage(ALTID_WARN, "SIZE is odd number! ", "", size);
        }

        (void) VFSFileSeek (fileRef->vfsRef, vfsOriginEnd, 0);
        ret  = VFSFileWrite(fileRef->vfsRef, size, ptr, writeSize);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "VFSFileWrite() ", "ret:", ret);
            *writeSize = 0;
        }
    }
    else
    {
        if (fileRef->streamRef == 0)
        {
            return (~errNone);
        }
        // (void) FileSeek (fileRef->streamRef, 0, fileOriginEnd);
        *writeSize = FileWrite(fileRef->streamRef, ptr, 1, size, &ret);
        if (ret != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "FileWrite() ", " ret:", ret);
            *writeSize = 0;
        }
    }
    return (ret);
}

/*==========================================================================*/
/*  appendFileAsURLEncode() : ファイルの末尾に書き出し(URLエンコード実施)   */
/*                                                                          */
/*==========================================================================*/
Err AppendFileAsURLEncode_NNsh(NNshFileRef *fileRef, UInt32  size,
                               void        *ptr,     UInt32 *writeSize)
{
    Char   *data, *buffer, *locP, logBuf[MINIBUF];
    UInt32  current, num, limit;
    Err     ret;

    // データコンバート後に格納する、データ格納領域を用意する
    buffer  = NULL;
    limit   = size * 6 + 8;
    do
    {
        limit  = limit - 8;
        buffer = (Char *) MEMALLOC_PTR(limit);
    } while (buffer == NULL);
    NNsh_DebugMessage(ALTID_INFO,"ALLOCATED URLENCODE BUFFER"," size:", limit);
    MemSet(buffer, limit, 0x00);

    current  = 0;
    data     = buffer;
    locP     = (Char *) ptr;

    while ((data <= (buffer + limit - 6))&&(current < size))
    {
        // スペースは＋に変換
        if (*locP == ' ')
        {
            *data = '+';
            data++;
            locP++;
            current++;
            continue;
        }

        // 改行は、CR + LFに変換
        if (*locP == '\n')
        {
            StrCopy(data, "%0D%0A");
            data = data + 6; // 6 == StrLen("%0D%0A");
            locP++;
            current++;
            continue;
        }

        // そのまま出力(その１)
        if ((*locP == '.')||(*locP == '_')||(*locP == '-')||(*locP == '*'))
        {
            *data = *locP;
            data++;
            locP++;
            current++;
            continue;
        }

        // そのまま出力(その２)
        if (((*locP >= '0')&&(*locP <= '9'))||
            ((*locP >= 'A')&&(*locP <= 'Z'))||
            ((*locP >= 'a')&&(*locP <= 'z')))
        {
            *data = *locP;
            data++;
            locP++;
            current++;
            continue;
        }

        // 上の条件のどれにも当てはまらない場合には、コンバートする。
        *data = '%';
        data++;
        
        num = (UInt32) *locP;
        MemSet(logBuf, sizeof(logBuf), 0x00);
        StrIToH(logBuf, num);

        // 数値の下２桁が欲しいので...
        StrCat(data, &logBuf[6]);
        data = data + 2; // 2 == StrLen(data);
        locP++;
        current++;
    }

    // ファイルへ書き込み(AppendFile_NNsh()を使用する)
    ret = AppendFile_NNsh(fileRef, (data - buffer), buffer, writeSize);

    MEMFREE_PTR(buffer);
    return (ret);
}

/*==========================================================================*/
/*  BackupDatabaseToVFS : データベースをVFSにバックアップする               */
/*                                                                          */
/*==========================================================================*/
Err BackupDatabaseToVFS_NNsh(Char *dbName)
{
    LocalID dbId;
    Char    fileName[MAXLENGTH_FILENAME];

    // ファイル名領域を初期化する
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, dbName);
    StrCat (fileName, ".pdb");

    // 存在するデータベースを検索
    dbId = DmFindDatabase(0, dbName);
    if (dbId == 0)
    {
        return (~errNone);
    }

    // (存在している)ファイルを削除し、VFSファイルに出力する
    (void) VFSFileDelete(NNshGlobal->vfsVol, fileName);
    return (VFSExportDatabaseToFile(NNshGlobal->vfsVol, fileName, 0, dbId));
}

/*==========================================================================*/
/*  RestoreDatabaseFromVFS : VFSからデータベースを復旧する                  */
/*                                                                          */
/*==========================================================================*/
Err RestoreDatabaseFromVFS_NNsh(Char *dbName)
{
    Err     ret;
    LocalID dbId;
    UInt16  cardNo;
    Char    fileName[MAXLENGTH_FILENAME];

    // ファイル名領域を初期化する
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, dbName);
    StrCat (fileName, ".pdb");

    // データベースが存在するか検索
    dbId = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // 存在していた場合、DBを削除する
        (void) DmDeleteDatabase(0, dbId);
        dbId = 0;
    }

    // VFSファイルからデータベースをインポートする
    ret = VFSImportDatabaseFromFile(NNshGlobal->vfsVol, 
                                    fileName, &cardNo, &dbId);
    return (ret);
}
