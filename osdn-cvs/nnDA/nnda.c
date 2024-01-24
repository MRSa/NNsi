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


// �\���f�[�^�̎擾�֐��Q
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

// UTF8 -> Shift JIS�ϊ��e�[�u�� (AMsoft����̂c�a���g�p����)
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
/*                                    �������m�ۊ֐�(�����I�Ɏg������...)  */
/*=========================================================================*/
MemPtr MemPtrNew_NNsh(UInt32 size, nnDADataType *adtP)
{
    if (adtP->palmOSVersion > 0x04000000)
    {
        // over 64kB�̗̈���m�ۂ���(OS4.x�ȏ�)
        return (MemGluePtrNew(size));
    }
    // �ʏ�̃������m��
    return (MemPtrNew(size));
}
#endif

/*-------------------------------------------------------------------------*/
/*   Function :  getFileStreamData                                         */
/*                                          fileStream����f�[�^���擾���� */
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
    
    // creatorID�̒��o
    ptr = (Char *) &creator;
    *ptr       = infoName[0];
    *(ptr + 1) = infoName[1];
    *(ptr + 2) = infoName[2];
    *(ptr + 3) = infoName[3];

    // �t�@�C�����̒��������o
    len      = 0;
    ptr      = infoName + 5;  // �N���G�[�^ID + ":" ���A�|�C���^��i�߂�
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

    // �t�@�C�����I�[�v�����A�f�[�^���R�s�[����
    filRef = FileOpen(0, adtP->fileName, 0, creator, fileModeReadOnly, &err);
    if (filRef == 0)
    {
        // �t�@�C���I�[�v���Ɏ��s������A�G���[�R�[�h��\������
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], err);
        goto FUNC_ABEND;
    }

    // �t�@�C���T�C�Y���擾
    if (FileTell(filRef, &dataSize, &err) == -1)
    {
        // �t�@�C���T�C�Y�̎擾�Ɏ��s�A�t�@�C����������\������
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

        // �ϊ���̕����񂪍쐬�ł�����A�̈���J������
        MEMFREE_PTR(ptr);
        ptr = ptr2;
        break;

      case nnDA_EDITCOMMAND_BINDATA:
        ptr2 = outputHexString(ptr, len, adtP);
        if (ptr2 != NULL)
        {
            // �o�C�i���\���f�[�^���쐬�ł�����A�̈���J������
            MEMFREE_PTR(ptr);
            ptr = ptr2;
        }
        break;

      default:
        // �Ȃɂ����Ȃ�...
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
/*                                            VFS file����f�[�^���擾���� */
/*-------------------------------------------------------------------------*/
static Char *getVfsFileData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    Err     ret;
    Char   *ptr, *ptr2;
    Boolean result;
    UInt32  dataSize, len;

    // xRRRR:/...(fileName)...
    //   x: �ǂݍ��݃t�@�C���̂��肩(�t�@�C���X�g���[�� or VFS�̎w��)
    //       v : VFS (�w��Ȃ�)
    //       c : VFS (CF)
    //       m : VFS (MS)
    //       s : VFS (SD)
    //       t : VFS (SmartMedia ... Toshiba��T)
    //       r : VFS (RD  : ramdisk)
    //       d : VFS (DoC : �����f�o�C�X)
    //   RRRR: �\��̈�
    //   /...(fileName)...  �t�@�C������
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
        //  VFS�����o�A�I������
        return (NULL);
    }

    // �t�@�C������ �ϐ��t�@�C�����ɃR�s�[
    ptr  = adtP->fileName;
    ptr2 = &(infoName[6]);
    while ((*ptr2 != '<')&&(*ptr2 != '\0'))
    {
        *ptr = *ptr2;
        ptr++;
        ptr2++;
    }

    // �t�@�C�����I�[�v������(VFS)
    ret = VFSFileOpen(adtP->vfsVol,adtP->fileName,vfsModeRead,&(adtP->vfsFileRef));
    if (ret != errNone)
    {
        // �t�@�C���I�[�v���Ɏ��s������A�t�@�C���������\������
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], ret);
        adtP->vfsFileRef = 0;
        goto FUNC_ABEND;
    }

    // �t�@�C���T�C�Y���擾
    ret = VFSFileSize(adtP->vfsFileRef, &(dataSize));
    if (ret != errNone)
    {
        // �t�@�C���T�C�Y�̎擾�Ɏ��s�A�t�@�C����������\������
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
/*                                             �f�o�b�O�p�ɐ��l�փf�[�^�ϊ� */
/*--------------------------------------------------------------------------*/
static Char *outputHexString(Char *data, UInt16 len, nnDADataType *adtP)
{
    UInt16   loop;
    UInt8   *dat;
    Char     buff[12], *buffer;

    buffer = MEMALLOC_PTR(len * 3 + MARGIN);
    if (buffer == NULL)
    {
         // �̈�m�ێ��s�A�����\�������I������
         return (NULL);
    }
    MemSet(buffer, (len * 3 + MARGIN), 0x00);

    // ���l�f�[�^����ʕ\��
    dat = data;
    for (loop = 0; loop < len; loop++)
    {
        if ((loop % 8) == 0)
        {
            // �f�[�^���W���Â�؂�
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
/*                        �ꗗ��ʂ̑���w���v�f�[�^(�X�g�����O)���擾���� */
/*-------------------------------------------------------------------------*/
static Char *getStringListData(nnDADataType *adtP)
{
    MemHandle  memH, memH2;
    UInt16     len;
    Char      *ptr, *ptr2, *data;

    // �X�g�����O���\�[�X���擾
    memH = DmGetResource('tSTR', HELPSTRING_LIST);
    if (memH == 0)
    {
        // ���\�[�X�̎擾���s�ANULL����������
        return (NULL);
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        // ���\�[�X�̎擾���s�ANULL����������
        return (NULL);
    }
    len = StrLen(ptr);
    
    // �X�g�����O���\�[�X���擾
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

    // �f�[�^�̊i�[�̈���m�ۂ��A���\�[�X�f�[�^���R�s�[
    // �i�����Ŋm�ۂ����̈�́AnnDA�I�����ɉ������j
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
/*                                  ����w���v�f�[�^(�X�g�����O)���擾���� */
/*-------------------------------------------------------------------------*/
static Char *getStringData(UInt16 rscID, nnDADataType *adtP)
{
    MemHandle  memH;
    UInt16     len;
    Char      *ptr, *data;

    // �X�g�����O���\�[�X���擾
    memH = DmGetResource('tSTR', rscID);
    if (memH == 0)
    {
        // ���\�[�X�̎擾���s�ANULL����������
        return (NULL);
    }
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        // ���\�[�X�̎擾���s�ANULL����������
        return (NULL);
    }
    len = StrLen(ptr);

    // �f�[�^�̊i�[�̈���m�ۂ��A���\�[�X�f�[�^���R�s�[
    // �i�����Ŋm�ۂ����̈�́AnnDA�I�����ɉ������j
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
/*                           ��ʃ��[�h(PalmOS5���𑜓x�T�|�[�g)�̃`�F�b�N */
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
    // Yomeru5 �𗘗p���Ă��邩�H
    if (FtrGet('Ymru', 5000, &version) == 0)
    {
        // Yomeru5�𗘗p���A�A�A���𑜓x���[�h�͎g�p���Ȃ����Ƃɂ���
        return;
    }
**/

    // ���𑜓x�t�B�[�`���[�̎擾
    if (FtrGet(sysFtrCreator, sysFtrNumWinVersion, &(version)) != 0)
    {
        // ���𑜓x�t�B�[�`���[�̃T�|�[�g�Ȃ�
        return;
    }
    if (version < 4)
    {
        //�@���𑜓x�̃T�|�[�g���Ȃ��ꍇ�̏���
        //  (����Ő������񂾂낤��...�}�j���A���ɂ͂��������Ă�������...)
#ifdef USE_CLIE
        // CLIE�p���𑜓x���T�|�[�g����Ă��邩�`�F�b�N����    
        // HRLIB�̎Q�Ɣԍ����擾����
        ret = SysLibFind(sonySysLibNameHR, &(adtP->hrRef));
        if (ret == sysErrLibNotFound)
        {
            // HRLIB�������[�h�̏ꍇ�A���[�h�ł��邩Try���Ă݂�B
            ret = SysLibLoad('libr', sonySysFileCHRLib, &(adtP->hrRef));
        }
        if (ret != errNone)
        {
            // HRLIB�͎g�p�ł��Ȃ��A�W���𑜓x���g��
            adtP->hrRef = 0;
            return;
        }

        // HRLIB�̎g�p�錾��HRLIB�̃o�[�W�����ԍ��擾
        HROpen(adtP->hrRef);
        HRGetAPIVersion(adtP->hrRef, &(adtP->hrVer));
        if (adtP->hrVer < HR_VERSION_SUPPORT_FNTSIZE)
        {
            // ���o�[�W�����̃n�C���]...��ʃ��[�h���m�F����...
            ret = HRWinScreenMode(adtP->hrRef, winScreenModeGet, 
                                  &width, &height, &depth, &isColor);
            if ((ret != errNone)||(width != hrWidth))
            {
                // �n�C���]���[�h�ł͂Ȃ��̂ŁAHRLIB�͎g�p���Ȃ�
                HRClose(adtP->hrRef);
                adtP->hrRef = 0;
                return;
            }
        }
#endif // #ifdef USE_CLIE
        return;
    }

    // Window�̉𑜓x���擾����
    WinScreenGetAttribute(winScreenDensity, &(adtP->os5density));

    // �Ƃ肠�����A�W���𑜓x�ɂ��Ă���
    WinSetCoordinateSystem(kCoordinatesStandard);

    return;
}

/*=========================================================================*/
/*   Function :  getInformation                                            */
/*                          �^����ꂽ��񂩂�A�\�����ׂ����̐ݒ���s�� */
/*=========================================================================*/
Char *getInformation(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    // �^����ꂽ��񂪈ُ�H
    if (infoName == NULL)
    {
        // ����������������B
        return (NULL);
    }

    ////////////////////// �\���^�C�g���ƕ\��������̐��� //////////////////

    // �t�@�C������f�[�^���擾����
    if (StrNCompare(infoName, nnDA_NNSIEXT_FILE, (sizeof(nnDA_NNSIEXT_FILE) - 1)) == 0)
    {
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE) - 1), title, titleSize, adtP));
    }

    // �t�@�C������f�[�^���擾����(�܂�Ԃ��Ή�)
    if (StrNCompare(infoName, nnDA_NNSIEXT_FILE_FOLD, (sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // �܂肽���ݑΉ��w��
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1), title, titleSize, adtP));
    }

    // �ꗗ��ʂ̃w���v�f�[�^���擾����
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPLIST, (sizeof(nnDA_NNSIEXT_HELPLIST) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_LIST, titleSize);
        return (getStringListData(adtP));
    }

    // �Q�Ɖ�ʂ̃w���v�f�[�^���擾����
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPVIEW, (sizeof(nnDA_NNSIEXT_HELPVIEW) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_VIEW, titleSize);
        return (getStringData(HELPSTRING_VIEW, adtP));
    }

    // ���[�U�ݒ�^�u��ʂ̃w���v�f�[�^���擾����
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPUSERTAB, (sizeof(nnDA_NNSIEXT_HELPUSERTAB) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_USERTAB, titleSize);
        return (getStringData(HELPSTRING_USERTAB, adtP));
    }

    // NNsi�ݒ��ʂ̃w���v�f�[�^���擾����
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPNNSISET, (sizeof(nnDA_NNSIEXT_HELPNNSISET) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_NNSISET, titleSize);
        return (getStringData(HELPSTRING_NNSISET, adtP));
    }

    // �Q�ƃ��O�ꗗ��ʂ̃w���v�f�[�^���擾����
    if (StrNCompare(infoName, nnDA_NNSIEXT_HELPGETLOG, (sizeof(nnDA_NNSIEXT_HELPGETLOG) - 1)) == 0)
    {
        StrNCopy(title, nnDA_TITLE_HELP_GETLOG, titleSize);
        return (getStringData(HELPSTRING_GETLOG, adtP));
    }

    // �t�@�C������̃f�[�^�ǂݏo��...
    if (StrNCompare(infoName, nnDA_NNSIEXT_FILE, (sizeof(nnDA_NNSIEXT_FILE) - 1)) == 0)
    {
        return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_FILE) - 1), title, titleSize, adtP));
    }

    // �o�C�i���t�@�C������̃f�[�^�ǂݏo��...
    if (StrNCompare(infoName, nnDA_NNSIEXT_BINFILE, (sizeof(nnDA_NNSIEXT_BINFILE) - 1)) == 0)
    {
        adtP->editCommand = nnDA_EDITCOMMAND_BINDATA;
        return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_BINFILE) - 1), title, titleSize, adtP));
    }

    // �������n���h������̃f�[�^�ǂݏo��...
    if (StrNCompare(infoName, nnDA_NNSIEXT_VIEWMEM, (sizeof(nnDA_NNSIEXT_VIEWMEM) - 1)) == 0)
    {
        return (getMemoryHandleData(infoName + (sizeof(nnDA_NNSIEXT_VIEWMEM) - 1), title, titleSize, adtP));
    }

    // �������n���h������̃f�[�^�ǂݏo��...(�܂肽���ݎw���Ή�)
    if (StrNCompare(infoName, nnDA_NNSIEXT_VIEWMEM_FOLD, (sizeof(nnDA_NNSIEXT_VIEWMEM_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // �܂肽���ݑΉ��w��
        return (getMemoryHandleData(infoName + (sizeof(nnDA_NNSIEXT_VIEWMEM_FOLD) - 1), title, titleSize, adtP));
    }

    // HTML�t�@�C������̃f�[�^�ǂݏo��...(�܂肽���ݎw���Ή�)
    if (StrNCompare(infoName, nnDA_NNSIEXT_HTML_FOLD, (sizeof(nnDA_NNSIEXT_HTML_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // �܂肽���ݑΉ��w��
        adtP->editCommand    = nnDA_EDITCOMMAND_HTMLPARSE;
        return (getFileStreamHtmlData(infoName + (sizeof(nnDA_NNSIEXT_HTML_FOLD) - 1), title, titleSize, adtP));
    }

    // HTML�t�@�C������̃f�[�^�ǂݏo��...
    if (StrNCompare(infoName, nnDA_NNSIEXT_HTMLFILE, (sizeof(nnDA_NNSIEXT_HTMLFILE) - 1)) == 0)
    {
        adtP->editCommand    = nnDA_EDITCOMMAND_HTMLPARSE;
        return (getFileStreamHtmlData(infoName + (sizeof(nnDA_NNSIEXT_HTMLFILE) - 1), title, titleSize, adtP));
    }

    // UTF8�t�@�C������̃f�[�^�ǂݏo��
    if (StrNCompare(infoName, nnDA_NNSIEXT_UTF8FILE, (sizeof(nnDA_NNSIEXT_UTF8FILE) - 1)) == 0)
    {
        if (DmFindDatabase(0, "UnicodeToJis0208") != 0)
        {
            adtP->editCommand    = nnDA_EDITCOMMAND_UTF8;
            return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_UTF8FILE) - 1), title, titleSize, adtP));
        }
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1), title, titleSize, adtP));
    }

    // UTF8�t�@�C������̃f�[�^�ǂݏo��(�܂肽���ݎw���Ή�)
    if (StrNCompare(infoName, nnDA_NNSIEXT_UTF8FILEFOLD, (sizeof(nnDA_NNSIEXT_UTF8FILEFOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // �܂肽���ݑΉ��w��
        if (DmFindDatabase(0, "UnicodeToJis0208") != 0)
        {
            adtP->editCommand    = nnDA_EDITCOMMAND_UTF8;
            return (getFileStreamData(infoName + (sizeof(nnDA_NNSIEXT_UTF8FILEFOLD) - 1), title, titleSize, adtP));
        }
        return (getFileStreamData((infoName + sizeof(nnDA_NNSIEXT_FILE_FOLD) - 1), title, titleSize, adtP));
    }

    // VFS�t�@�C������̃f�[�^�ǂݏo��
    if (StrNCompare(infoName, nnDA_NNSIEXT_VFSFILE, (sizeof(nnDA_NNSIEXT_VFSFILE) - 1)) == 0)
    {
        return (getVfsFileData(infoName + (sizeof(nnDA_NNSIEXT_VFSFILE) - 1), title, titleSize, adtP));
    }

    // VFS�t�@�C������̃f�[�^�ǂݏo��(�܂肽���ݎw���Ή�)
    if (StrNCompare(infoName, nnDA_NNSIEXT_VFSFILE_FOLD, (sizeof(nnDA_NNSIEXT_VFSFILE_FOLD) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_FOLD;  // �܂肽���ݑΉ��w��
        return (getVfsFileData(infoName + (sizeof(nnDA_NNSIEXT_VFSFILE_FOLD) - 1), title, titleSize, adtP));
    }

    // JPEG�t�@�C���̕\�����s��...
    if (StrNCompare(infoName, nnDA_NNSIEXT_SHOWJPEG, (sizeof(nnDA_NNSIEXT_SHOWJPEG) - 1)) == 0)
    {
        adtP->displayCommand = nnDA_DISPCOMMAND_JPEGDISP;
        return (getJpegFileData((infoName + sizeof(nnDA_NNSIEXT_SHOWJPEG) - 1), title, titleSize, adtP));
    }

    // nnDA��T�|�[�g�B�B�B
    return (NULL);
}

/*-------------------------------------------------------------------------*/
/*   Function :  nnDA_EventHandler                                         */
/*                                                   nnDA �� event ����    */
/*-------------------------------------------------------------------------*/
static Boolean nnDA_EventHandler(EventPtr evP)
{
    UInt16        btnId   = 0;
    UInt32        is5Navi = 0;
    Boolean       mov;
    nnDADataType *adtP = NULL;
    FormType     *frm;
    ControlType  *obj;

    // global pointer���擾����
    if (FtrGet(nnDA_Creator, ADT_FTRID, (UInt32 *) &adtP) != 0)
    {
        // �O���[�o���|�C���^�擾���s
        return (false);
    }

    // �V�t�g�C���W�P�[�^���m�F����
    frm = FrmGetActiveForm();
    obj = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));
    mov = CtlEnabled(obj);

    // 5way-nav�𖳌��ɂ���(PalmTX�Ή�...)
    if (FtrGet(sysFtrCreator, sysFtrNumFiveWayNavVersion, &is5Navi) == errNone)
    {
        FrmSetNavState(FrmGetActiveForm(), kFrmNavStateFlagsInteractionMode);
    }

    switch (evP->eType)
    {
      case keyDownEvent:
        switch (evP->data.keyDown.chr)
        {
          // JOG PUSH/Z�L�[���������Ƃ��̏���
          case vchrJogRelease:
          case vchrThumbWheelPush:
          case chrCapital_Z:
          case chrSmall_Z:
            // �㉺�ړ����[�h�E���E�ړ����[�h�̐؂�ւ������s
            btnId = BTNID_SHIFT;
            break;

          case vchrPageDown:
          case chrDownArrow:
          case vchrJogDown:
          case chrCapital_J:
          case chrSmall_J:
            // ���X�N���[��
            if (mov == true)
            {
                // ���E�ړ����[�h(�E�X�N���[��)
                btnId = BTNID_NEXT;
            }
            else
            {
                // �㉺�ړ����[�h(���X�N���[��)
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
                // ���E�ړ����[�h(�E�X�N���[��)
                btnId = BTNID_PREV;
            }
            else
            {
                // �㉺�ړ����[�h(��X�N���[��)
                btnId = BTNID_UP;
            }
            break;

          case vchrJogLeft:
          case chrLeftArrow:
          case vchrJogPushedUp:
          case chrCapital_H:
          case chrSmall_H:
          case vchrRockerLeft:
            // ���X�N���[��
            btnId = BTNID_PREV;
            break;

          case vchrJogRight:
          case chrRightArrow:
          case vchrJogPushedDown:
          case chrCapital_L:
          case chrSmall_L:
          case vchrRockerRight:
            // �E�X�N���[��
            btnId = BTNID_NEXT;
            break;

          case vchrHard2:
          case vchrHard3:
          case chrCapital_Q:
          case chrSmall_Q:
          case vchrRockerCenter:
            // �n�j�����������Ƃɂ���
            CtlHitControl(FrmGetObjectPtr(FrmGetActiveForm(),
                                          FrmGetObjectIndex(FrmGetActiveForm(),
                                          BTNID_DA_OK)));
            return (true);
            break;
            
          case chrBackspace:
          case chrEscape:
            // About�\��
            btnId = BTNID_DA_ABOUT;
            break;

          case vchrNavChange:
            // 5way navigator�Ή�
            if ((evP->data.keyDown.keyCode) & navBitLeft)
            {
                // �J�[�\���L�[���Ɠ���(���X�N���[��)
                btnId = BTNID_PREV;
                break;
            }
            if ((evP->data.keyDown.keyCode) & navBitRight)
            {
                // �J�[�\���L�[�E�Ɠ���(�E�X�N���[��)
                btnId = BTNID_NEXT;
                break;
            }
            break;

          case chrCapital_F:
          case chrSmall_F:
            // �t�H���g�T�C�Y�̕ύX...
            adtP->fontId = FontSelect(adtP->fontId);
            FntSetFont(adtP->fontId);
            btnId = GADID_DA_VIEW;
            break;
           
          case chrSpace:
          case chrCarriageReturn:
          case chrLineFeed:
          default:
            // �������Ȃ�
            return (false);
            break;
        }
        break;

      case ctlSelectEvent:
        // ���Ƃŏ�������
        btnId = evP->data.ctlSelect.controlID;
        break;
        
      case ctlRepeatEvent:
        // ���Ƃŏ�������
        btnId = evP->data.ctlRepeat.controlID;

      default:
        // �������Ȃ�
        return (false);
        break;
    }

    // �{�^���������ꂽ�Ƃ��̏�������
    switch (btnId)
    {
      case BTNID_DA_ABOUT:
        // About�\��
        FrmAlert(ALERTID_DA_ABOUT);
        break;

      case BTNID_UP:
        // ��ړ�
        if (adtP->startY > 0)
        {
            adtP->startY = adtP->startY - (adtP->area.extent.y / 2);
        }
        break;      

      case BTNID_DOWN:
        // ���ړ�
        if (adtP->startY < adtP->maxY)
        {
            adtP->startY = adtP->startY + (adtP->area.extent.y / 2);
        }
        break;

      case BTNID_PREV:
        // �O(���ړ�)
        if (adtP->startX > 0)
        {
            adtP->startX = adtP->startX - (adtP->area.extent.x / 2);
        }
        break;

      case BTNID_NEXT:
        // ��(�E�ړ�)
        if (adtP->startX < adtP->maxX)
        {
            adtP->startX = adtP->startX + (adtP->area.extent.x / 2);
        }
        break;

      case BTNID_SHIFT:
        // �V�t�g��Ԑ؂�ւ�
        // ON/OFF���؂�ւ������A"!"�̕\����؂�ւ���
        if (mov == true)
        {
            // ���E�ړ����[�h���㉺�ړ����[�h(!������)
            CtlSetEnabled(obj, false);
            CtlSetUsable (obj, false);
            FrmHideObject(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));                
        }
        else
        {
            // �㉺�ړ����[�h�����E�ړ����[�h(!��\������)
            CtlSetEnabled(obj, true);
            CtlSetUsable (obj, true);
            FrmShowObject(frm, FrmGetObjectIndex(frm, BTNID_SHIFT));   
        }
        break;

      case GADID_DA_VIEW:
        // ��ʂ��ĕ`�悷��
        break;

      default:
        // �������Ȃ�
        return (false);
        break;
    }

    // JPEG�\���̏ꍇ�ɂ́A�Ȃɂ����Ȃ�
    if (adtP->displayCommand == nnDA_DISPCOMMAND_JPEGDISP)
    {
        return (false);
    }

    // ��ʂ��ĕ`��(�e�L�X�g�\���̂Ƃ�)
    setDisp_nnDA_DAData  (FrmGetActiveForm(), adtP);
    setnnDA_DAData       (FrmGetActiveForm(), adtP);
    resetDisp_nnDA_DAData(FrmGetActiveForm(), adtP);

    return (true);
}

#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  startClieDisp_DAData                                      */
/*                                              �`�惂�[�h�ݒ菈��(CLIE�p) */
/*-------------------------------------------------------------------------*/
static void startClieDisp_DAData(FormType *frm, nnDADataType *adtP)
{

    // draw���Ȃ��ƁAWindow��ɗ̈悪�\������Ȃ��A�A�A
    FrmDrawForm(frm);

    // �K�W�F�b�g�̗̈�T�C�Y���擾����
    FrmGetObjectBounds(frm,
                       FrmGetObjectIndex(frm, GADID_DA_VIEW),
                       &(adtP->dimF));

    // CLIE���𑜓x�p�ɕ`��G���A���g�傷��
    adtP->area.topLeft.x  = adtP->dimF.topLeft.x * 2;
    adtP->area.extent.x   = adtP->dimF.extent.x  * 2;
    adtP->area.extent.y   = adtP->dimF.extent.y  * 2;
    adtP->area.topLeft.y  = adtP->dimF.topLeft.y * 2;

    // �`��̈�̃N���A
    HRWinEraseRectangle(adtP->hrRef, &(adtP->area), 0);

    return;
}
#endif  // #ifdef USE_CLIE


#ifdef USE_CLIE
/*-------------------------------------------------------------------------*/
/*   Function :  endClieDisp_DAData                                        */
/*                                                  ��ʕ`��㏈��(CLIE�p) */
/*-------------------------------------------------------------------------*/
static void endClieDisp_DAData(FormType *frm, nnDADataType *adtP)
{

    return;
}
#endif // #ifdef USE_CLIE


/*-------------------------------------------------------------------------*/
/*   Function :  WinDrawChar_NNsh                                          */
/*                                                            �����\������ */
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
/*                                            ��ʂɕ������\��t���鏈�� */
/*-------------------------------------------------------------------------*/
static void setDisp_nnDA_DAData(FormType *frm, nnDADataType *adtP)
{
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        startClieDisp_DAData(frm, adtP);  //  ��ʕ\����������
        return;
    }
#endif

    // draw���Ȃ��ƁAWindow��ɗ̈悪�\������Ȃ��A�A�A
    FrmDrawForm(frm);

    // �K�W�F�b�g�̗̈�T�C�Y���擾����
    FrmGetObjectBounds(frm,FrmGetObjectIndex(frm, GADID_DA_VIEW),
                                                               &(adtP->dimF));

    // �𑜓x���[�h���m�F���A�̈���N���A����(area�Ɏ��𑜓x���i�[)
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
    // �`��̈�̃N���A
    WinEraseRectangle(&(adtP->area), 0);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  resetDisp_nnDA_DAData                                     */
/*                                    ��ʂɕ������\��t���鏈���̌㏈�� */
/*-------------------------------------------------------------------------*/
static void resetDisp_nnDA_DAData(FormType *frm, nnDADataType *adtP)
{

    // ���𑜓x�̐ݒ������
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
/*                                    ��ʂɕ������\��t���鏈���̌㏈�� */
/*-------------------------------------------------------------------------*/
static void endnnDA_DAData(FormType *frm, nnDADataType *adtP)
{
#ifdef USE_CLIE
    if (adtP->hrRef != 0)
    {
        endClieDisp_DAData(frm, adtP);  //  ��ʕ\����������
        return;
    }
#endif

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  setnnDA_DAData                                            */
/*                                            ��ʂɕ������\��t���鏈�� */
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
    marginX = FntCharWidth('\x01');   // missing character symbol�̕������
    while ((UInt8 *) ptr < adtP->areaP + adtP->length)
    {
        // �w�肳�ꂽ�R�[�h��\������
        if ((*ptr == 0x0a)||(*ptr == 0x0d))
        {
            if ((*ptr == 0x0d)&&(*(ptr + 1) == 0x0a))
            {
                // ���s�R�[�h���b�q�{�k�e�������ꍇ
                ptr++;
            }

            // ���s�R�[�h�̂Ƃ�
            y = y + FntCharHeight();
            adtP->maxY = y;
            x = topX;
            ptr++;
            if (y >= adtP->startY + adtP->area.extent.y)
            {
                // �`��̈敝�𒴂����A�I������
                break;
            }
            continue;
        }

        // �܂�Ԃ��\�����s���ꍇ�̃`�F�b�N�A�A�A(�\���͈͂𒴂��Ă��邩�H)
        if ((adtP->displayCommand == nnDA_DISPCOMMAND_FOLD)&&
            (x >= adtP->startX + adtP->area.extent.x))
        {
            // ���s�����{����
            y = y + FntCharHeight();
            adtP->maxY = y;
            x = topX;
            if (y >= adtP->startY + adtP->area.extent.y)
            {
                // �`��̈敝�𒴂����A�I������
                break;
            }
            continue;
        }

        // ���� or ANK ?
        if ((*ptr <= 0x80)||((*ptr >= 0xa1)&&(*ptr <= 0xdf)))
        {            
            // �V���O���o�C�g�̏ꍇ
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
            // �������v�Z�����l�ɂ���...
            x = x + FntCharWidth(letter);
#else
            x = x + marginX;
#endif
           adtP->maxX = x;
        }
        else
        {            
            // �_�u���o�C�g�̏ꍇ�AJIS�R�[�h�ɕϊ����Ă���
            // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
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
/*                                                        DA�f�[�^�\������ */
/*=========================================================================*/
void nnDAMain(void)
{
    FontID        fontId;
    UInt16        result;
    Char         *infoName, *infoData;
    FormType     *diagFrm,  *prevFrm;
    nnDADataType *adtP;

    // ���݂̃t�H�[�����擾
    prevFrm = FrmGetActiveForm();

    // ���L�f�[�^�̊m��
    adtP = MEMALLOC_PTR(sizeof(nnDADataType) + MARGIN);
    if (adtP == NULL)
    {
        // �̈�m�ێ��s�A�I������
        return;
    }
    MemSet(adtP, (sizeof(nnDADataType) + MARGIN), 0x00);
    StrNCopy(adtP->title, nnDA_DEFTITLE, nnDA_TITLESIZE);
    infoData  = NULL;

    // global pointer��feature�Ƃ��ăZ�b�g����
    FtrSet(nnDA_Creator, ADT_FTRID, (UInt32) adtP);

    // OS�o�[�W�����̊m�F
    if (FtrGet(sysFtrCreator, sysFtrNumROMVersion, &(adtP->palmOSVersion)) != errNone)
    {
        adtP->palmOSVersion = 0;
    }

    // �t�H���g�T�C�Y���ꎞ�I�ɕύX����
    fontId = FntGetFont();
    adtP->fontId = stdFont;
    FntSetFont(adtP->fontId);

    //  ��ʃ��[�h�̃`�F�b�N
    checkDisplayMode(adtP);

    //  �_�C�A���O�E�B���h�E�̍쐬
    createDialogWindow(&diagFrm, adtP);

    // window�̃��T�C�Y
    resizeDialogWindow(diagFrm, adtP);

    // Viewer Form�̃C�x���g�n���h���o�^
    FrmSetEventHandler(diagFrm, nnDA_EventHandler);

    ////////////////////////////////////////////////////////////
    //  �f�[�^�̉�ʐݒ菈��
    //   (�N���b�v�{�[�h�ɂ���f�[�^���擾��\������)
    infoData = NULL;
    if (CheckClipboard(adtP, &infoName) == true)
    {
        // �\�����ׂ����̎擾
        infoData = getInformation(infoName, adtP->title, nnDA_TITLESIZE, adtP);

        // ��񖼗̈�̉��
        MEMFREE_PTR(infoName);
        infoName = NULL;

        // �\�����ׂ���񂪎擾�ł������H
        if (infoData != NULL)
        {
            // �\������f�[�^�̐ݒ�
            adtP->areaP  = infoData;
            adtP->length = StrLen(infoData);
        }
        else
        {
            if (adtP->displayCommand != nnDA_DISPCOMMAND_JPEGDISP)
            {
                // ��񂪎擾�ł��Ȃ������Ƃ��A�A�A
                goto DISPLAY_SCREEN;
            }
        }
    }
    else
    {
        // �N���b�v�{�[�h�̓��e��\��������Ƃ���
        adtP->areaP = copyClipboard();
        if (adtP->areaP == NULL)
        {
            // ��񂪎擾�ł��Ȃ������A�A�A�I������
            goto DISPLAY_SCREEN;   
        }
    }

    setDisp_nnDA_DAData  (diagFrm, adtP);  //  ��ʕ\����������
    if (adtP->displayCommand == nnDA_DISPCOMMAND_JPEGDISP)
    {
        // JPEG��ʕ\��
        adtP->bitmapPP = NULL;
        adtP->bitmapPPV3 = NULL;
        adtP->jpegRef  = 0;

        // JPEG�t�@�C���̈ʒu���w�肳��Ă��Ȃ�����...
        if (adtP->sourceLocation == 0)
        {
            resetDisp_nnDA_DAData(diagFrm, adtP);  //  ��ʕ\���㏈��

            FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", " a JPEG file", ".");
            goto DISPLAY_SCREEN;
        }

        // JpegLib���g�p�\���m�F����
        if (checkPnoJpegLibInstalled(&(adtP->jpegRef)) == true)
        {
            // JpegLib���g�p�\�A���������g�p���ĉ摜��\������
#ifdef USE_PNOJPEGLIB2
            displayJpegUsingPnoJpegLib2(FrmGetActiveForm(), adtP);
#else  // #ifdef USE_PNOJPEGLIB2
            displayJpegUsingPnoJpegLib1(FrmGetActiveForm(), adtP);
#endif  // #ifdef USE_PNOJPEGLIB2
        }
        else if (checkJpegLibInstalled(&(adtP->jpegRef)) == true)
        {
            // JpegLib���g�p�\�A���������g�p���ĉ摜��\������
            displayJpegUsingJpegLib(FrmGetActiveForm(), adtP);
        }
        else if (checkSonyJpegLibInstalled(&(adtP->jpegRef)) == true)
        {
            // Sony JpegLib���g�p�\�A����𗘗p���ĉ摜�\������
            displayJpegUsingSonyJpegLib(FrmGetActiveForm(), adtP);
        }
        else
        {
            resetDisp_nnDA_DAData(diagFrm, adtP);  //  ��ʕ\���㏈��

            // �T�|�[�g���Ă��� JPEG���C�u������������Ȃ�����...
            FrmCustomAlert(ALTID_ERROR, "Could not find ", "any JPEG library", ".");
            goto DISPLAY_SCREEN;
        }

        // �㉺���E�{�^���������Ȃ��悤�ɂ���...
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
        // �ʏ�̕�����\��
        setnnDA_DAData       (FrmGetActiveForm(), adtP);
    }
    resetDisp_nnDA_DAData(diagFrm, adtP);  //  ��ʕ\���㏈��

    ////////////////////////////////////////////////////////////
DISPLAY_SCREEN:
    // �t�H�[���̃^�C�g�����R�s�[����
    FrmCopyTitle(diagFrm, adtP->title);

    // Dialog�\��
    result = FrmDoDialog(diagFrm);
    if (result == BTNID_DA_OK)
    {
        // OK�{�^���������ꂽ�A�A�A���ǁA����͕K��OK�Ŕ�����͂�
    }
    
    ////////////////////////////////////////////////////////////
    //  �f�[�^�̉�ʐݒ�̌㏈��
    //
    endnnDA_DAData(diagFrm, adtP);
    //
    ////////////////////////////////////////////////////////////

    // JPEG�\���̌㏈��(�K�v�ł����)
    postProcessJpegDisplay(adtP);

    // �t�H���g�T�C�Y�����ɖ߂�
    FntSetFont(fontId);            

    // �e���|�����f�[�^�o�b�t�@���������
    if (infoData != NULL)
    {
        MEMFREE_PTR(infoData);
    }

    // �t�H�[����global pointer�̌�n��
    FrmSetActiveForm(prevFrm);
    FrmDeleteForm(diagFrm);

    FtrUnregister(nnDA_Creator, ADT_FTRID);
    MEMFREE_PTR(adtP);
}

/* --------------------------------------------------------------------------
 *   AMsoft�����UTF8�ϊ��e�[�u���𗘗p���Ď�������
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     ������̃R�s�[(UTF8����SHIFT JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, LocalID dbId)
{
    UInt8     buffer[2];
    UInt16    cnt, codes, ucode;
    Boolean   kanji;
    DmOpenRef dbRef;   

    // �ϊ��e�[�u�������邩�m�F����
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
            // ���p���[�h...
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

        // �����R�[�h�̕ϊ�
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

    // �f�[�^�x�[�X�̃��R�[�h���Ȃ������Ƃ�...
    if (dbRef == 0)
    {
        // �G���[...
        return (0x2121);
    }
    num  = utf8H;

    // �������킹���
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
        // �G���[...
        return (0x2122);
    }
    rec = (UnicodeToJis0208Rec *) MemHandleLock(recH);
    if (rec->size > 0)
    {
        // �f�[�^�L��
        code  = rec->table[col];
    }
    else
    {
        // �f�[�^�Ȃ�
        code = 0x2121;
    }
    MemHandleUnlock(recH);
    return (code);
}

/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS�����R�[�h��SHIFT JIS�����R�[�h�ɕϊ����� */
/*=========================================================================*/
static Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
    upper = *ptr;
    lower = *(ptr + 1);

    // ���������C����ɂ��A���S���Y�����T(p.110)�̃R�[�h���g�킹�Ă�������
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
/*                                  �����R�[�h��EUC����SHIFT JIS�ɕϊ����� */
/*=========================================================================*/
static Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte)
{
    UInt8 temp[2];

    *copyByte = 0;
    if (*ptr == 0x8e)
    {
        // ���p�J�i�R�[�h
        ptr++;
        *dst = *ptr;
        *copyByte = 1;
        return (true); 
    }
    if ((*ptr >= ((0x80)|(0x21)))&&(*ptr <= ((0x80)|(0x7e))))
    {
        // EUC�����R�[�h�Ɣ���AJIS�����R�[�h�Ɉ�x�ϊ����Ă���SHIFT JIS�ɕϊ�
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
/*                                                  (���b�Z�[�W�̐��`����) */
/*-------------------------------------------------------------------------*/
static void ParseMessage(Char *buf, Char *src, UInt32 size, UInt16 kanjiCode)
{
    Boolean kanjiMode;
    UInt16  len;
    Char   *ptr, *dst, dataStatus;

    dataStatus = MSGSTATUS_NAME;
    kanjiMode = false;

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    dst  = buf;
    ptr  = src;

    while (ptr < (src + size))
    {
        // JIS�����R�[�h�̕ϊ����W�b�N
        if (kanjiCode == NNSH_KANJICODE_JIS)
        {
            // JIS��SHIFT JIS�̃R�[�h�ϊ�
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x24')&&(*(ptr + 2) == '\x42'))
            {
                // �������[�h�ɐ؂�ւ�
                kanjiMode = true;
                ptr = ptr + 3;
                continue;
            }
            if ((*ptr == '\x1b')&&(*(ptr + 1) == '\x28')&&(*(ptr + 2) == '\x42'))
            {
                // ANK���[�h�ɐ؂�ւ�
                kanjiMode = false;
                ptr = ptr + 3;
                continue;
            }
            if (kanjiMode == true)
            {
                // JIS > SJIS �����ϊ�
                ConvertJIStoSJ(dst, ptr);
                ptr = ptr + 2;
                dst = dst + 2;
                continue;
            }
        }
        if (*ptr == '&')
        {
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&lt;" �� '<' �ɒu��
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                *dst++ = '<';
                ptr = ptr + 4;   // StrLen(TAG_GT);
                continue;
            }
            // "&quot;" �� '"' �ɒu��
            if ((*(ptr + 1) == 'q')&&(*(ptr + 2) == 'u')&&
                (*(ptr + 3) == 'o')&&(*(ptr + 4) == 't')&&(*(ptr + 5) == ';'))
            {
                *dst++ = '"';
                ptr = ptr + 6;    // StrLen(TAG_QUOT); 
                continue;
            }
            // "&nbsp;" �� '    ' �ɒu��
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
            // "&amp;" �� '&' �ɒu��
            if ((*(ptr + 1) == 'a')&&(*(ptr + 2) == 'm')&&
                (*(ptr + 3) == 'p')&&(*(ptr + 4) == ';'))
            {
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
                    if (*ptr == ' ')
                    {
                        // �X�y�[�X�͓ǂݔ�΂�
                        ptr++;
                    }
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
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')&&(*(ptr + 3) == '>')))
            {
                // �s���ƍs���̃X�y�[�X������Ă݂�ꍇ
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

            //  "<p*>" �́A���s2�ɒu��
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

            // <li>�^�O�����s�R�[�h��:�ɒu������
            if (((*(ptr + 1) == 'l')&&(*(ptr + 2) == 'i')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'L')&&(*(ptr + 2) == 'I')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = '�';
                ptr = ptr + 4;
                continue;
            }   

            //  "<hr>" �́A���s === ���s �ɒu��
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

            //  "<dt>"�́A���s�ɒu��
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

            //  "</tr>" ����� "</td>" �́A���s�ɒu��
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

            //  "<dd>" �́A���s�Ƌ󔒂Q�ɒu��
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" �́A���s + ���s �ɒu��
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "</h?>" �́A���s + ���s �ɒu��
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            // <tr>, <td> �̓X�y�[�X�P�ɕϊ�
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                // continue�͂Ȃ��B�B (�^�O�͓ǂݔ�΂�����)
            }
SKIP_TAG:
            // ���̑��̃^�O�͓ǂݔ�΂�
#ifdef USE_STRSTR
            ptr = StrStr(ptr, ">");
#else
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
#endif
            ptr++;
            // *dst++ = ' ';   // �^�O�͊��S�����Ƃ���B
            continue;
        }
        // �����R�[�h��EUC�������ꍇ...
        if (kanjiCode == NNSH_KANJICODE_EUC)
        {
            if (ConvertEUCtoSJ((UInt8 *) dst, (UInt8 *) ptr, &len) == true)
            {
                // EUC > SHIFT JIS�ϊ������s����
                dst = dst + len;
                ptr = ptr + 2;
                continue;
            }
            if (*ptr != '\0')
            {
                // ���ʂ̈ꕶ���]��
                *dst++ = *ptr;
            }
            ptr++;
            continue;
        }

        // �X�y�[�X���A�����Ă����ꍇ�A�P�Ɍ��炷
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < (src + size)))
            {
                ptr++;
            }
            ptr--;
        }

        // NULL ����� 0x0a, 0x0d, 0x09(�^�u) �͖�������
        if ((*ptr != '\0')&&(*ptr != 0x0a)&&(*ptr != 0x0d)&&(*ptr != 0x09))
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
/*   Function :  getMemoryHandleData                                       */
/*                                      �������n���h������f�[�^���擾���� */
/*-------------------------------------------------------------------------*/
static Char *getMemoryHandleData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    MemHandle  memH;
    Char      *ptr, *buf;
    Int16      times;
	Int32      size;

    // �������n���h�����擾����
	ptr  = infoName;
	size  = 0;
    times = 1;
    if (*ptr == '-')
    {
        // ���̐�...
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

	// �����R�[�h�̌���
    switch (*ptr)
    {
      case 'e':
        // �����R�[�h��EUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'E':
        // �����R�[�h��EUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        break;

      case 'r':
        // ���f�[�^�\��
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'R':
        // ���f�[�^�\��
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        break;

      case 'j':
        // �����R�[�h��JIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'J':
        // �����R�[�h��JIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        break;

      case 's':
        // �����R�[�h��SHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      default:
        // ���̑���SHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        break;
    }
    ptr++;
    ptr++;

    // Title�f�[�^�̃R�s�[
    StrNCopy(title, ptr, titleSize);
    ptr = StrStr(title, "<");
    if (ptr != NULL)
    {
        *ptr = '\0';
    }

    // �\���f�[�^�̎擾
    ptr = MemHandleLock(memH);
    if (ptr == NULL)
    {
        StrNCopy(title, "MemHandleLock()", titleSize);
        return (NULL);
    }
    size = StrLen(ptr);
    
    // �\���̈�̊m��
    buf = MEMALLOC_PTR((size * times) + BUF_MARGIN);
    if (buf == NULL)
    {
        StrNCopy(title, "MEMALLOC_PTR()", titleSize);
        MemHandleUnlock(memH);
        return (NULL);
    }
    MemSet (buf, ((size * times) + BUF_MARGIN), 0x00);

	// �f�[�^�̕\���R�[�h�̊m�F...
	if (adtP->kanjiCode != nnDA_KANJICODE_RAW)
	{
        // �\���f�[�^�̐��`
        ParseMessage(buf, ptr, size, adtP->kanjiCode);
	}
	else
	{
		// ���f�[�^�\��
		StrNCopy(buf, ptr, size);
	}
	MemHandleUnlock(memH);
    return (buf);
}

/*-------------------------------------------------------------------------*/
/*   Function :  getFileStreamHtmlData                                     */
/*                                fileStream����f�[�^���擾����(HTML���H) */
/*-------------------------------------------------------------------------*/
static Char *getFileStreamHtmlData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    UInt16     len;
    Char      *fileName, *ptr, *buf;
    Err        err;
    FileHand   filRef;
    
    // creatorID�̒��o
    ptr = (Char *) &(adtP->creator);
    *ptr       = infoName[0];
    *(ptr + 1) = infoName[1];
    *(ptr + 2) = infoName[2];
    *(ptr + 3) = infoName[3];

    // �t�@�C�����̒��������o
    len      = 0;
    ptr      = infoName + 5;  // �N���G�[�^ID + ":" ���A�|�C���^��i�߂�

    // �����R�[�h�̎擾
    switch (*ptr)
    {
      case 'e':
        // �����R�[�h��EUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'E':
        // �����R�[�h��EUC
        adtP->kanjiCode = NNSH_KANJICODE_EUC;
        adtP->times = 2;
        break;

      case 'r':
        // ���f�[�^�\��
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        adtP->fontId = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'R':
        // ���f�[�^�\��
        adtP->kanjiCode = nnDA_KANJICODE_RAW;
        adtP->times = 1;
        break;

      case 'j':
        // �����R�[�h��JIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      case 'J':
        // �����R�[�h��JIS
        adtP->kanjiCode = NNSH_KANJICODE_JIS;
        adtP->times     = 3;
        break;

      case 's':
        // �����R�[�h��SHIFT JIS
        adtP->kanjiCode = NNSH_KANJICODE_SHIFTJIS;
        adtP->times     = 2;
        adtP->fontId    = largeFont;
		FntSetFont(adtP->fontId);
        break;

      default:
        // ���̑���SHIFT JIS
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

    // �t�@�C�����I�[�v�����A�f�[�^���R�s�[����
    filRef = FileOpen(0, fileName, 0, adtP->creator, fileModeReadOnly, &err);
    if (filRef == 0)
    {
        // �t�@�C���I�[�v���Ɏ��s������A�G���[�R�[�h��\������
        StrCopy(title, "err:0x");
        StrIToH(&title[StrLen(title)], err);
        return (fileName);
    }

    // �t�@�C���T�C�Y���擾
    if (FileTell(filRef, &(adtP->dataSize), &err) != -1)
    {
        // �^�C�g���͋󔒂�
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
                // �\���f�[�^�T�C�Y���ۂ߂�...
                len = DISP_LIMIT;
                adtP->dataSize = (adtP->dataSize * 2 / 3);
            }
            buf = MEMALLOC_PTR(len + BUF_MARGIN);
            if (buf != NULL)
            {
                MemSet(buf, (len + BUF_MARGIN), 0x00);

                // �\���f�[�^�̐��`
                ParseMessage(buf, ptr, adtP->dataSize, adtP->kanjiCode);

                // �ǂݍ��񂾃f�[�^�o�b�t�@�͊J������
                MEMFREE_PTR(ptr);
                ptr = buf;
            }
        }
        MEMFREE_PTR(fileName);
    }
    else
    {
        // �t�@�C���T�C�Y�̎擾�Ɏ��s�A�t�@�C����������\������
        StrNCopy(title, "filetell", titleSize);
        ptr = fileName;
    }
    FileClose(filRef);
    return (ptr);
}

/*!
 *   Progress Dialog�p�̃R�[���o�b�N�֐�
 */
Boolean textCallBack(PrgCallbackDataPtr cbP)
{
    // �i�����p�[�Z���g�\������
    StrPrintF(cbP->textP, "%d%%...", cbP->stage);
    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :  displayJpegUsingSonyJpegLib                               */
/*                                       JPEG�t�@�C���\��(SonyJpegLib�g�p) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingSonyJpegLib(FormType *frm, nnDADataType *adtP)
{
#ifdef USE_CLIE
#ifdef USE_SONY_JPEG
    Err          err;
    UInt32       apiVer;

    // SONY JPEG���C�u�����̃I�[�v��
    err = jpegUtilLibOpen(adtP->jpegRef);
    if (err != errNone)
    {
        // SONY JPEG���C�u�����̃I�[�v���Ɏ��s�A�I������
        FrmCustomAlert(ALTID_ERROR,
                       "Could not open",
                       "Sony JPEG library",
                       ".");
        return;
    }

    // SONY JPEG���C�u�����̃o�[�W�����m�F (2�ȏ�Ȃ�΁AFileStream�ǂݏo����)
    adtP->bitmapPP = 0;
    apiVer = jpegUtilLibGetLibAPIVersion(adtP->jpegRef);
    if ((apiVer < 2)&&(adtP->sourceLocation == nnDA_JPEGSOURCE_FILESTREAM))
    {
        // jpeg API����Ή��ASONY JPEG���C�u�������N���[�Y���I������
        jpegUtilLibClose(adtP->jpegRef);
        FrmCustomAlert(ALTID_ERROR,
                       "Do not support a stream JPEG file",
                       "at Sony JPEG library",
                       ".");
        return;
    }

    // SONY JPEG���C�u�������g�p����JPEG�t�@�C������ʕ`�悷��
    // (�`���A���\�[�X���N���[�Y����)
    switch (adtP->sourceLocation)
    {
      case nnDA_JPEGSOURCE_FILESTREAM:
        if (adtP->streamRef != 0)
        {
            // �摜����ʂɕ`�悷��(�t�@�C���X�g���[������)
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
            // �摜����ʂɕ`�悷��(VFS�t�@�C������)
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
        // �����������ɂ���JPEG�f�[�^����ʂɕ\������
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

    // SONY JPEG���C�u�����̃N���[�Y
    jpegUtilLibClose(adtP->jpegRef);

    // �`��OK�Ȃ�A�\������
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
/*                                         JPEG�t�@�C���\��(JpegLib���g�p) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingJpegLib(FormType *frm, nnDADataType *adtP)
{
    Err          err;
    UInt16       nc;
    Coord        width, height;
    Char         textBuffer[20];

    // JpegLib�̃I�[�v��
    err = JpegLibOpen(adtP->jpegRef, jpegLibCurrentVersion);
    if (err != errNone)
    {
        // JPEG���C�u�������g�p�ł��Ȃ�...(�G���[�I������)...
        FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", "JPEG library", ".");
        return;
    }

    // �u�҂āv�\��������...
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
            // �摜����ʂɕ`�悷��(FileStream����)
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
            // �摜����ʂɕ`�悷��(VFS�t�@�C������)
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
        // �����������ɂ���JPEG�f�[�^����ʂɕ\������
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

    // ���C�u�����̃N���[�Y
    JpegLibClose(adtP->jpegRef, &nc);

    // �`��OK�Ȃ�ABitmap��\������
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
/*                   JPEG�t�@�C���f�[�^�̕\�������{����(pnoJpegLib v2�g�p) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingPnoJpegLib2(FormType *frm, nnDADataType *adtP)
{
    Err           err;
    Coord         width, height;
    Char          textBuffer[20];

    // jpegLib�̃I�[�v��
    err = pnoJpegOpen(adtP->jpegRef);
    if (err != errNone)
    {
        // pnoJPEG���C�u�������g�p�ł��Ȃ�...(�G���[�I������)...
        FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", "pnoJpegLib2", ".");
        return;
    }

    // �u�҂āv�\��������...
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

    // ���[�N�̈�̊m��
    err = pnoJpeg2Create(adtP->jpegRef, &(adtP->pnoData));
    if (err != errNone)
    {
        // pnoJPEG���C�u�����p�̃��[�N�̈悪�m�ۂł��Ȃ�...(�G���[�I������)...
        FrmCustomAlert(ALTID_ERROR, "Could not alloc ", "pnoJpegLib2 ", "work area.");
        return;
    }

    switch (adtP->sourceLocation)
    {
      case nnDA_JPEGSOURCE_FILESTREAM:
        if (adtP->streamRef != 0)
        {
            // FileStream����f�[�^��ǂݏo��...
            err = pnoJpeg2LoadFromFileStream(adtP->jpegRef, adtP->pnoData, adtP->streamRef);
        }
        else
        {
            // �t�@�C�����I�[�v������Ă��Ȃ�...�I������@
            err = ~errNone;
        }
        break;

      case nnDA_JPEGSOURCE_VFS:
        // ��������VFS�t�@�C�����N���[�Y����
        VFSFileClose(adtP->vfsFileRef);
        adtP->vfsFileRef = 0;

        // VFS�t�@�C������ǂݏo��
        err = pnoJpeg2LoadFromVFS(adtP->jpegRef, adtP->pnoData, adtP->vfsVol, adtP->fileName);
        break;


      case nnDA_JPEGSOURCE_MEMORY:
      default:
        // �����������ɂ���JPEG�f�[�^����ʂɕ\������
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
            // �摜�T�C�Y��OK
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
                // �摜�T�C�Y�𔼕���
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
                // �摜�T�C�Y�� 1/4 ��
                err = pnoJpeg2SetScaleFactor(adtP->jpegRef, adtP->pnoData, 4);        
            }
            else
            {
                err = ~errNone;
            }
        }

        if (err != errNone)
        {
            // �摜�T�C�Y�� 1/8 ��
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

    // pnoJpegLib2�̊m�ۗ̈���J������
    pnoJpeg2Free(adtP->jpegRef, &(adtP->pnoData));
    adtP->pnoData = 0;

    // pnoJpeg2���C�u�������N���[�Y����
    pnoJpegClose(adtP->jpegRef);
 
    // �`��OK�Ȃ�ABitmap��\������
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
/*                   JPEG�t�@�C���f�[�^�̕\�������{����(pnoJpegLib v1�g�p) */
/*-------------------------------------------------------------------------*/
static void displayJpegUsingPnoJpegLib1(FormType *frm, nnDADataType *adtP)
{
    Err          err;
    Coord        width, height;
    Char         textBuffer[20];

    // VFS�`���ȊO�́ApnoJpegLib�ł̓T�|�[�g���Ȃ� (�Ƃ肠����)
    if (adtP->sourceLocation != nnDA_JPEGSOURCE_VFS)
    {
        // jpeg API����Ή��AJPEG���C�u�������N���[�Y���I������
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

    // jpegLib�̃I�[�v��
    err = pnoJpegOpen(adtP->jpegRef);
    if (err != errNone)
    {
        // pnoJPEG���C�u�������g�p�ł��Ȃ�...(�G���[�I������)...
        FrmCustomAlert(ALTID_ERROR, "Could not OPEN ", "pnoJpegLib", ".");
        return;
    }

    // �u�҂āv�\��������...
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

    // pnoJpeg���C�u�������N���[�Y����
    pnoJpegClose(adtP->jpegRef);

    // �`��OK�Ȃ�ABitmap��\������
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
/*                                  JPEG�t�@�C���f�[�^�̃I�[�v�������{���� */
/*-------------------------------------------------------------------------*/
static Char *getJpegFileData(Char *infoName, Char *title, UInt16 titleSize, nnDADataType *adtP)
{
    Err     ret;
    Char   *ptr, *ptr2;
    Boolean result;

    // SHOWJPEG:xRRRR:/...(fileName)...
    //   x: �ǂݍ��݃t�@�C���̂��肩(�t�@�C���X�g���[�� or VFS�̎w��)
    //       f : �t�@�C���X�g���[��
    //       v : VFS (�w��Ȃ�)
    //       c : VFS (CF)
    //       m : VFS (MS)
    //       s : VFS (SD)
    //       t : VFS (SmartMedia ... Toshiba��T)
    //       r : VFS (RD  : ramdisk)
    //       d : VFS (DoC : �����f�o�C�X)
    //   RRRR: �\��̈�
    //   /...(fileName)...  �t�@�C������
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
        //  VFS�����o�A�I������
        FrmCustomAlert(ALTID_ERROR, " not found proper VFS volume ", "", "");
        return (NULL);
    }

    // �t�@�C������ �ϐ��t�@�C�����ɃR�s�[
    MemSet(adtP->fileName, (nnDA_FILENAMESIZE + 2), 0x00);
    ptr  = adtP->fileName;
    ptr2 = &(infoName[6]);
    while ((*ptr2 != '<')&&(*ptr2 != '\0')&&(ptr < (adtP->fileName + nnDA_FILENAMESIZE)))
    {
        *ptr = *ptr2;
        ptr++;
        ptr2++;
    }

    // �t�@�C�����I�[�v������(VFS)
    ret = VFSFileOpen(adtP->vfsVol,adtP->fileName,vfsModeRead,&(adtP->vfsFileRef));
    if (ret != errNone)
    {
        // �t�@�C���I�[�v���Ɏ��s������A�t�@�C���������\������
        StrCopy(title, "ERR:0x");
        StrIToH(&title[StrLen(title)], ret);
        adtP->vfsFileRef = 0;
        goto FUNC_ABEND;
    }

    // �t�@�C���̃I�[�v�������I�i�Q�Ɣԍ����\���̂ɃR�s�[����)
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
/*                                          JPEG�t�@�C���f�[�^�̕\���㏈�� */
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
/*                 VFS�̃T�|�[�g���m�F(adtP->vfsVol��VFS volume���i�[����) */
/*-------------------------------------------------------------------------*/
static Boolean checkVFSdevice(UInt32 aMediaType, nnDADataType *adtP)
{
    Err err;

    // VFS�̃T�|�[�g�L�����m�F����B
    adtP->vfsVolumeIterator = 0;
    err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &(adtP->vfsVolumeIterator));
    if (err != errNone)
    {
        adtP->vfsVol = 0;
        return (false);
    }

    //  VFS���g�p�\���m�F����B
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
            // �t�@�C���V�X�e���̊m�F
            if (adtP->volInfo.fsType == vfsFilesystemType_VFAT)
            {                    
                // VFS�f�o�C�X�𔭌��I
                return (true);                
            }
        }
    }
    // VFS�f�o�C�X��������Ȃ�����...
    adtP->vfsVol = 0;
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  openFileStreamForJPEG                                     */
/*                      JPEG�t�@�C���f�[�^�̃I�[�v�������{����(FileStream) */
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

    // �t�@�C�������擾����
    ptr  = adtP->fileName;
    ptr2 = &(infoName[6]);
    while ((*ptr2 != '<')&&(*ptr2 != '\0'))
    {
        *ptr = *ptr2;
        ptr++;
        ptr2++;
    }

    // �t�@�C�����I�[�v������(fileStream)
    adtP->streamRef = FileOpen(0,adtP->fileName, 0, creator, fileModeReadOnly, &err);
    if (adtP->streamRef == 0)
    {
        // �t�@�C���I�[�v���Ɏ��s������A�t�@�C���������\������
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
/*                            pnoJpegLib���C���X�g�[������Ă��邩�m�F���� */
/*-------------------------------------------------------------------------*/
static Boolean checkPnoJpegLibInstalled(UInt16 *jpegRef)
{
    Err err;

    // pnoJpegLib���g�p�\���m�F����
    err = SysLibFind(pnoJpegName, jpegRef);
    if (err != errNone) // not already loaded; do it now
    {
        err = SysLibLoad(pnoJpegTypeID, pnoJpegCreatorID, jpegRef);
    }
    if (err == errNone)
    {
        // pnoJpegLib���C���X�g�[������Ă���
        return (true);
    }
    // pnoJpegLib�͖��C���X�g�[���A�A
    return (false);    
}

/*-------------------------------------------------------------------------*/
/*   Function :  checkJpegLibInstalled                                     */
/*                               JpegLib���C���X�g�[������Ă��邩�m�F���� */
/*-------------------------------------------------------------------------*/
static Boolean checkJpegLibInstalled(UInt16 *jpegRef)
{
    Err err;

    // JpegLib���g�p�\���m�F����
    err = SysLibFind(jpegLibName, jpegRef);
    if (err != errNone) // not already loaded; do it now
    {
        err = SysLibLoad(sysResTLibrary, jpegLibCreator, jpegRef);
    }
    if (err == errNone)
    {
        // JpegLib���g�p�\�A���������g�p���ĉ摜��\������
        return (true);
    }
    // JpegLib���g�p�ł��Ȃ�...�I������
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  checkSonyJpegLibInstalled                                 */
/*                           SonyJpegLib���C���X�g�[������Ă��邩�m�F���� */
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
        // Sony JpegLib���g�p�\�A���������g�p���ĉ摜��\������
        return (true);
    }
    // Sony JpegLib���g�p�ł��Ȃ�...�I������
    return (false);
}

/*-------------------------------------------------------------------------*/
/*   Function :  resizeDialogWindow                                        */
/*                                        �_�C�A���O�E�B���h�E�T�C�Y�̒��� */
/*-------------------------------------------------------------------------*/
static void resizeDialogWindow(FormType *diagFrm, nnDADataType *adtP)
{
    UInt16         objIndex;
    Coord          x, y, diffX, diffY;
    RectangleType  r;

    /* �S�̂�Window�T�C�Y�擾 */
    WinGetDisplayExtent(&x, &y);

    /* dialog�̃T�C�Y�擾 */
    FrmGetFormBounds(diagFrm, &r);

    /* �E�B���h�E�̊g��T�C�Y�����߂� */
    r.topLeft.x = 2;
    r.topLeft.y = 3;
    diffX       = r.extent.x;
    diffY       = r.extent.y;
    r.extent.y  = y - 6;
    r.extent.x  = x - 4;
    
    /* �I�u�W�F�N�g�̈ړ��T�C�Y�����߂� */
    diffX = r.extent.x - diffX;
    diffY = r.extent.y - diffY;

    /* dialog�̊g�� */
    WinSetWindowBounds(FrmGetWindowHandle(diagFrm), &r);

    /* OK �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_OK);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* �� �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_UP);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* �� �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_PREV);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* �E �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_NEXT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* �� �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DOWN);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* SHIFT �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_SHIFT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    // �����l�́A�㉺�ړ����[�h�ɐݒ�
    CtlSetEnabled(FrmGetObjectPtr(diagFrm, objIndex), false);
    CtlSetUsable (FrmGetObjectPtr(diagFrm, objIndex), false);
    FrmHideObject(diagFrm, objIndex);                

    /* About �{�^���̈ړ� */
    objIndex = FrmGetObjectIndex(diagFrm, BTNID_DA_ABOUT);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    // r.topLeft.x = r.topLeft.x + diffX;
    r.topLeft.y = r.topLeft.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    /* gadget�̊g�� */
    objIndex = FrmGetObjectIndex(diagFrm, GADID_DA_VIEW);
    FrmGetObjectBounds(diagFrm, objIndex, &r);
    r.extent.x = r.extent.x + diffX;
    r.extent.y = r.extent.y + diffY;
    FrmSetObjectBounds(diagFrm, objIndex, &r);

    return;
}

/*-------------------------------------------------------------------------*/
/*   Function :  createDialogWindow                                        */
/*                                 DIA�T�|�[�g�̃`�F�b�N�ƃ_�C�A���O�̐��� */
/*-------------------------------------------------------------------------*/
static Boolean createDialogWindow(FormType **diagFrm, nnDADataType *adtP)
{
#ifdef USE_DIA
    UInt32 diaVar;
    UInt16 objIndex, result;

    // DIA�T�|�[�g�����邩�ǂ����m�F����
    result = FtrGet(pinCreator, pinFtrAPIVersion, &diaVar);
    if (result != errNone)
    {
        // DIA ��T�|�[�g�f�o�C�X�A�_�C�A���O�T�C�Y�̊g��͂��Ȃ�
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // DIA�T�|�[�g�f�o�C�X �A�V���N�����I�ɓ���\���m�F����
    result = FtrGet(sysFtrCreator, sysFtrNumInputAreaFlags, &diaVar);
    if ((result != errNone)|
        ((diaVar & grfFtrInputAreaFlagDynamic) == 0)||
        ((diaVar & grfFtrInputAreaFlagCollapsible) == 0))
    {
        // �G���[�ɂȂ�ꍇ�A�������͓��I�V���N��T�|�[�g�̏ꍇ�A��������������
        *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
        FrmSetActiveForm(*diagFrm);
        return (false);
    }

    // ���݂̃V���N��Ԃ��L������
    objIndex = PINGetInputAreaState();

    // �_�C�A���O�̃I�[�v��
    *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
    FrmSetActiveForm(*diagFrm);

    // �V���N�����L���ɂ���
    FrmSetDIAPolicyAttr(*diagFrm, frmDIAPolicyCustom);
    PINSetInputTriggerState(pinInputTriggerEnabled);

    // ���炢�x�^�A�A�A�E�B���h�E�T�C�Y�̍ő�l�ŏ��l��ݒ�
    WinSetConstraintsSize(FrmGetWindowHandle(*diagFrm),
                          160, 225, 225, 160, 225, 225);

    // �V���N�̈��ۑ����Ă������̂ɂ���
    PINSetInputAreaState(objIndex);

#else  // #ifdef USE_DIA
    // Viewer Form�̏�����
    *diagFrm = FrmInitForm(FRMID_DA_VIEWER);
    FrmSetActiveForm(*diagFrm);
#endif  // #ifdef USE_DIA

    return (true);
}

/*-------------------------------------------------------------------------*/
/*   Function :   CheckClipboard                                           */
/*                                                   �N���b�v�{�[�h�̊m�F  */
/*-------------------------------------------------------------------------*/
static Boolean CheckClipboard(nnDADataType *adtP, Char **infoName)
{
    // TODO �F �N���b�v�{�[�h�̘A�g�����Ȃ��ꍇ�́A���̂܂ܕ������
    //         ��������悤�ɕύX����K�v������B

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
        // �N���b�v�{�[�h�Ƀf�[�^���Ȃ��A�I������
        return (false);
    }
    areaP = MemHandleLock(memH);
    if (areaP == NULL)
    {
        // �N���b�v�{�[�h�̃|�C���^�擾���s�A�I������
        goto FUNC_END;
    }

    ///////////////////// NNsi�Ƃ̘A�g���� ��������
    if (StrNCompare(areaP, nnDA_NNSIEXT_VIEWSTART, StrLen(nnDA_NNSIEXT_VIEWSTART)) != 0)
    {
        // NNsi�Ƃ̘A�g�͂��Ȃ��A�I������
        goto FUNC_END;
    }

    // �f�[�^������̐擪������
    ptr = StrStr(areaP, nnDA_NNSIEXT_INFONAME);
    if ((ptr == NULL)||(ptr > (areaP + len)))
    {
        // �f�[�^������̐擪�̌��o�Ɏ��s�A�I������
        goto FUNC_END;
    }

    // �f�[�^�̐擪�Ɉړ�����
    ptr = ptr + StrLen(nnDA_NNSIEXT_INFONAME);

    // �f�[�^�����񖼂�؂�o��
    *infoName = MEMALLOC_PTR(BUFSIZE);
    if (*infoName == NULL)
    {
        // �i�[�̈�̊m�ۂɎ��s�A�I������
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
    ///////////////////// NNsi�Ƃ̘A�g���� �����܂�

FUNC_END:
    // �N���b�v�{�[�h�̈���J������
    if (memH != 0)
    {
        MemHandleUnlock(memH);
    }
    return (result);
}

/*-------------------------------------------------------------------------*/
/*   Function :   copyClipboard                                            */
/*                                             �N���b�v�{�[�h�̓��e�R�s�[  */
/*-------------------------------------------------------------------------*/
static Char *copyClipboard(void)
{
    MemHandle memH;
    UInt16    len;
    Char     *ptr;

    // �N���b�v�{�[�h�ɘA�g�����񂪂Ȃ�����...
    memH = ClipboardGetItem(clipboardText, &len);
    if ((memH == 0)||(len == 0))
    {
        // �N���b�v�{�[�h�Ƀf�[�^���Ȃ��A�I������
        return (NULL);
    }

    // �N���b�v�{�[�h�̃f�[�^��`��p�ɃR�s�[����
    ptr = MEMALLOC_PTR(len + 4);
    if (ptr != NULL)
    {
        MemSet (ptr, (len + 4), 0x00);
        MemMove(ptr, MemHandleLock(memH), len);
        MemHandleUnlock(memH);
    }
    return (ptr);
}
