/*============================================================================*
 *
 *  $Id: filemgr.c,v 1.9 2004/03/08 13:57:32 mrsa Exp $
 *
 *  FILE: 
 *     fileMgr.c
 *
 *  Description: 
 *     �t�@�C���}�l�[�W��
 *===========================================================================*/
#define FILEMGR_C
#include "local.h"

static void SetFullPathFileName_NNsh(Char *file, Char *fullPath, UInt16 size);
static Err openFile_VFSFile(Char *fileName, UInt16 fileMode, NNshFileRef *ref);
static Err openFile_FileStream(Char *name, UInt16 fileMode, NNshFileRef *ref);

/*==========================================================================*/
/*  CheckFileSpace_NNsh() : �t�@�C���󂫗e�ʂ̊m�F                          */
/*                                                                          */
/*==========================================================================*/
Err CheckFileSpace_NNsh(UInt16 fileMode, UInt32 size)
{
    Err    ret = errNone;
    UInt32 useSize, totalSize;

    if (((fileMode & NNSH_FILEMODE_TEMPFILE) == 0)&&
        (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0))
    {
        // VFS�̋󂫗e�ʂ��m�F����
        ret = VFSVolumeSize(NNshGlobal->vfsVol, &useSize, &totalSize);
        if (ret == errNone)
        {
            if ((size + FREE_SPACE_MARGIN) > (totalSize - useSize))
            {
                // �󂫗e�ʕs���A�G���[��������
                NNsh_ErrorMessage(ALTID_ERROR, MSG_VFSSPACE_INSUFFICIENT,
                                  "", (totalSize - useSize));
                return (~errNone);
            }
        }
    }
    else
    {
        // Palm�{�̓��̋󂫃X�y�[�X�͊m�F���Ȃ��B
    }
    return (ret);
}

/*==========================================================================*/
/*  closeFile_NNsh() : �t�@�C���N���[�Y                                     */
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

    // �t�@�C�����g���Ȃ��悤�ɂ���B
    fileRef->streamRef = 0;
    fileRef->vfsRef    = 0;
    return (ret);
}

/*==========================================================================*/
/*  openFile_NNsh() : �t�@�C���I�[�v��                                      */
/*                                                                          */
/*==========================================================================*/
Err OpenFile_NNsh(Char *fileName, UInt16 fileMode, NNshFileRef *fileRef)
{
    Err ret;
    MemSet(fileRef, sizeof(NNshFileRef), 0x00);

    // �t�@�C���I�[�v����̊m�F
    if (((fileMode & NNSH_FILEMODE_TEMPFILE) == 0)&&
        (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0))
    {
        // VFS �t�@�C���I�[�v���v��
        NNsh_DebugMessage(ALTID_INFO, "VFS file Open:", fileName, 0);
        fileRef->fileLocation = NNSH_VFS_ENABLE;
        ret = openFile_VFSFile(fileName, fileMode, fileRef);
    }
    else
    {
        // stream �t�@�C���I�[�v���v��
        NNsh_DebugMessage(ALTID_INFO, "Stream file Open :", fileName, 0);
        fileRef->fileLocation = NNSH_VFS_DISABLE;
        ret = openFile_FileStream(fileName, fileMode, fileRef);
    }
    return (ret);
}

/*==========================================================================*/
/*  GetFileSize_NNsh() : �t�@�C���T�C�Y�̎擾                               */
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
/*  ReadFile_NNsh() : �t�@�C������f�[�^�̓ǂݏo��                          */
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
/*  SetFullPathFileName_NNsh() : �t�@�C�����̂��t���p�X�w��Ɋg��           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static void SetFullPathFileName_NNsh(Char *file, Char *fullPath, UInt16 size)
{
    MemSet (fullPath, size, 0x00);
    if (*file != '/')
    {
        // ���΃p�X�w�肾�����ꍇ�ɂ́A�x�[�X�f�B���N�g���𓪂ɂ���
        StrCopy(fullPath, DATAFILE_PREFIX);
        StrCat (fullPath, file);
    }
    else
    {
        // ��΃p�X���w�肳��Ă����ꍇ�ɂ́A���̂܂܃R�s�[
        StrCopy(fullPath, file);
    }
    return;    
}

/*--------------------------------------------------------------------------*/
/*  openFile_VFSFile() : �t�@�C���I�[�v��(VFS File�p)                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Err openFile_VFSFile(Char *fileName, UInt16 fileMode, NNshFileRef *ref)
{
    Err     ret;
    UInt16  mode;
    Char    fullPath[MAXLENGTH_FILENAME];

    // �t�@�C���I�[�v�����[�h�̃`�F�b�N
    switch (fileMode)
    {
      case NNSH_FILEMODE_READONLY:
      case (NNSH_FILEMODE_READONLY|NNSH_FILEMODE_TEMPFILE):
        mode = vfsModeRead;
        break;

      case NNSH_FILEMODE_CREATE:
      case (NNSH_FILEMODE_CREATE|NNSH_FILEMODE_TEMPFILE):
        // �V�K�쐬�w���̏ꍇ�ɂ́A�t�@�C���������Ă���I�[�v������
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

    // FULL PATH�w��Ɋg��
    SetFullPathFileName_NNsh(fileName, fullPath, sizeof(fullPath));
    ret = VFSFileOpen(NNshGlobal->vfsVol, fullPath, mode, &(ref->vfsRef));
    if (ret == vfsErrFileNotFound)
    {
        // �t�@�C�������݂��Ȃ������ꍇ�́A�V�K�쐬���Ă������
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
/*  openFile_FileStream() : �t�@�C���I�[�v��(File Stream�p)                 */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Err openFile_FileStream(Char *name, UInt16 fileMode, NNshFileRef *ref)
{
    Err    err;
    UInt32 openMode;

    // �t�@�C���I�[�v�����[�h�̃`�F�b�N
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
/*  DeleteFile_NNsh() : �t�@�C���f���[�g                                    */
/*                                                                          */
/*==========================================================================*/
Err DeleteFile_NNsh(Char *fileName, UInt16 location)
{
    Err  ret;
    Char fullPath[MAXLENGTH_FILENAME];

    if ((location != NNSH_VFS_DISABLE)&&
        (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0))
    {
        // VFS��̃t�@�C�����폜����
        SetFullPathFileName_NNsh(fileName, fullPath, sizeof(fullPath));
        ret = VFSFileDelete(NNshGlobal->vfsVol, fullPath);
    }
    else
    {
        // File Stream��̃t�@�C�����폜����
        ret = FileDelete(0, fileName);
    }
    return (ret);
}

/*==========================================================================*/
/*  createFile_NNsh() : �t�@�C���쐬 (VFS Only)                             */
/*                                                                          */
/*==========================================================================*/
Err CreateFile_NNsh(Char *fileName)
{
    Err  ret = errNone;
    Char fullPath[MAXLENGTH_FILENAME];

    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS��Ƀt�@�C�����쐬����
        SetFullPathFileName_NNsh(fileName, fullPath, sizeof(fullPath));
        ret = VFSFileCreate(NNshGlobal->vfsVol, fullPath);
    }
    return (ret);
}

/*==========================================================================*/
/*  renameFile_NNsh() : �t�@�C�����l�[��(VFS Only)                          */
/*                                                                          */
/*==========================================================================*/
Err RenameFile_NNsh(Char *oldFileName, Char *newFileName)
{
    Err  ret = errNone;
    Char oldFullPath[MAXLENGTH_FILENAME];
    Char newFullPath[MAXLENGTH_FILENAME];

    if ((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE)
    {
        // VFS��̃t�@�C������ύX����
        SetFullPathFileName_NNsh(oldFileName,oldFullPath,sizeof(oldFullPath));
        SetFullPathFileName_NNsh(newFileName,newFullPath,sizeof(newFullPath));
        ret = VFSFileRename(NNshGlobal->vfsVol, oldFullPath, newFullPath);
    }
    return (ret);
}

/*==========================================================================*/
/*  CreateDir_NNsh() : �f�B���N�g���̍쐬(VFS Only)                         */
/*                                                                          */
/*==========================================================================*/
Err CreateDir_NNsh(Char *dirName)
{
    Err  ret = errNone;
    Char fullPath[MAXLENGTH_FILENAME];

    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS��Ƀf�B���N�g�����쐬����
        SetFullPathFileName_NNsh(dirName, fullPath, sizeof(fullPath));
        ret = VFSDirCreate(NNshGlobal->vfsVol, fullPath);
    }
    return (ret);
}


/*==========================================================================*/
/*  CopyFile_NNsh() : �t�@�C���̃R�s�[(VFS Only)                            */
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

    // �R�s�[�^�C�v�ɂ��킹�ăt�@�C�����[�h��ύX����
    switch (location)
    {
      case NNSH_COPY_PALM_TO_VFS:
        // Palm�{�̂���VFS�փR�s�[����ꍇ
        fileModeR =  NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
        fileModeW =  NNSH_FILEMODE_CREATE;
        break;

      case NNSH_COPY_VFS_TO_VFS:
        // VFS����VFS�փR�s�[����ꍇ
        fileModeR = NNSH_FILEMODE_READONLY;
        fileModeW = NNSH_FILEMODE_CREATE;
        break;

      case NNSH_COPY_VFS_TO_PALM:
        // VFS����Palm�{�̂փR�s�[����ꍇ
        fileModeR = NNSH_FILEMODE_READONLY;
        fileModeW = NNSH_FILEMODE_CREATE | NNSH_FILEMODE_TEMPFILE;
        break;

      case NNSH_COPY_PALM_TO_PALM:
      default:
        // Palm�{�̂���Palm�{�̂փR�s�[����ꍇ
        fileModeR = NNSH_FILEMODE_READONLY | NNSH_FILEMODE_TEMPFILE;
        fileModeW = NNSH_FILEMODE_CREATE   | NNSH_FILEMODE_TEMPFILE;
        break;        
    }

    // ���茳�t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(sourceFile, fileModeR, &srcRef);
    if (ret != errNone)
    {
        return (ret);
    }

    // �����t�@�C���̃I�[�v��
    ret = OpenFile_NNsh(destFile, fileModeW, &destRef);
    if (ret != errNone)
    {
        return (ret);
    }

    // �R�s�[�p���[�N�̈���m�ۂ���
    limit = (NNshGlobal->NNsiParam)->bufferSize - (NNshGlobal->NNsiParam)->bufferSize % 2;
    do
    {
        limit  = limit - 8;
        buffer = (Char *) MEMALLOC_PTR(limit + 4);
    } while (buffer == NULL);

    offsetW = 0;
    offsetR = 0;
    ret     = errNone;

    // ����ɓǂݍ��߂��Ƃ��́A�t�@�C�����R�s�[����
    while (ret == errNone)
    {
        readSize = 0;
        MemSet(buffer, limit, 0x00);
        ret  = ReadFile_NNsh (&srcRef, offsetR, limit, buffer, &readSize);
        offsetR = offsetR + readSize;
        if ((readSize % 2) != 0)
        {
            // �������݃T�C�Y�������ɕ␳����(��CLIE�p�̕ی�)
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

    // ���[�N�̈���J������
    MEMFREE_PTR(buffer);

    // �t�@�C���N���[�Y
    CloseFile_NNsh(&srcRef);
    CloseFile_NNsh(&destRef);

    // �t�@�C���̍Ō�܂ŃR�s�[��������errNone�ŉ�������
    if ((ret == vfsErrFileEOF)||(ret == fileErrEOF))
    {
        ret = errNone;
    }

    return (ret);
}

/*==========================================================================*/
/*  WriteFile_NNsh() : �t�@�C���փf�[�^�̏����o��                           */
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

        //  �ꕔ�@���VFSFileWrite��肪���݂��邽�߁A�t�@�C�������o������
        // ���̏����ɍ��v�����ꍇ�AWARNING��\������B(�΍�͂��Ȃ��B)
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
/*  appendFile() : �t�@�C���̖����ɏ����o��                                 */
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

        //  �ꕔ�@���VFSFileWrite��肪���݂��邽�߁A�t�@�C�������o������
        // ���̏����ɍ��v�����ꍇ�AWARNING��\������B(�΍�͂��Ȃ��B)
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
/*  appendFileAsURLEncode() : �t�@�C���̖����ɏ����o��(URL�G���R�[�h���{)   */
/*                                                                          */
/*==========================================================================*/
Err AppendFileAsURLEncode_NNsh(NNshFileRef *fileRef, UInt32  size,
                               void        *ptr,     UInt32 *writeSize)
{
    Char   *data, *buffer, *locP, logBuf[MINIBUF];
    UInt32  current, num, limit;
    Err     ret;

    // �f�[�^�R���o�[�g��Ɋi�[����A�f�[�^�i�[�̈��p�ӂ���
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
        // �X�y�[�X�́{�ɕϊ�
        if (*locP == ' ')
        {
            *data = '+';
            data++;
            locP++;
            current++;
            continue;
        }

        // ���s�́ACR + LF�ɕϊ�
        if (*locP == '\n')
        {
            StrCopy(data, "%0D%0A");
            data = data + 6; // 6 == StrLen("%0D%0A");
            locP++;
            current++;
            continue;
        }

        // ���̂܂܏o��(���̂P)
        if ((*locP == '.')||(*locP == '_')||(*locP == '-')||(*locP == '*'))
        {
            *data = *locP;
            data++;
            locP++;
            current++;
            continue;
        }

        // ���̂܂܏o��(���̂Q)
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

        // ��̏����̂ǂ�ɂ����Ă͂܂�Ȃ��ꍇ�ɂ́A�R���o�[�g����B
        *data = '%';
        data++;
        
        num = (UInt32) *locP;
        MemSet(logBuf, sizeof(logBuf), 0x00);
        StrIToH(logBuf, num);

        // ���l�̉��Q�����~�����̂�...
        StrCat(data, &logBuf[6]);
        data = data + 2; // 2 == StrLen(data);
        locP++;
        current++;
    }

    // �t�@�C���֏�������(AppendFile_NNsh()���g�p����)
    ret = AppendFile_NNsh(fileRef, (data - buffer), buffer, writeSize);

    MEMFREE_PTR(buffer);
    return (ret);
}

/*==========================================================================*/
/*  BackupDatabaseToVFS : �f�[�^�x�[�X��VFS�Ƀo�b�N�A�b�v����               */
/*                                                                          */
/*==========================================================================*/
Err BackupDatabaseToVFS_NNsh(Char *dbName)
{
    LocalID dbId;
    Char    fileName[MAXLENGTH_FILENAME];

    // �t�@�C�����̈������������
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, dbName);
    StrCat (fileName, ".pdb");

    // ���݂���f�[�^�x�[�X������
    dbId = DmFindDatabase(0, dbName);
    if (dbId == 0)
    {
        return (~errNone);
    }

    // (���݂��Ă���)�t�@�C�����폜���AVFS�t�@�C���ɏo�͂���
    (void) VFSFileDelete(NNshGlobal->vfsVol, fileName);
    return (VFSExportDatabaseToFile(NNshGlobal->vfsVol, fileName, 0, dbId));
}

/*==========================================================================*/
/*  RestoreDatabaseFromVFS : VFS����f�[�^�x�[�X�𕜋�����                  */
/*                                                                          */
/*==========================================================================*/
Err RestoreDatabaseFromVFS_NNsh(Char *dbName)
{
    Err     ret;
    LocalID dbId;
    UInt16  cardNo;
    Char    fileName[MAXLENGTH_FILENAME];

    // �t�@�C�����̈������������
    MemSet (fileName, MAXLENGTH_FILENAME, 0x00);
    StrCopy(fileName, DATAFILE_PREFIX);
    StrCat (fileName, dbName);
    StrCat (fileName, ".pdb");

    // �f�[�^�x�[�X�����݂��邩����
    dbId = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // ���݂��Ă����ꍇ�ADB���폜����
        (void) DmDeleteDatabase(0, dbId);
        dbId = 0;
    }

    // VFS�t�@�C������f�[�^�x�[�X���C���|�[�g����
    ret = VFSImportDatabaseFromFile(NNshGlobal->vfsVol, 
                                    fileName, &cardNo, &dbId);
    return (ret);
}
