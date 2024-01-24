/*============================================================================*
 *  FILE: 
 *     nnshcmn.c
 *
 *  Description: 
 *     NNsi common functions.
 *
 *===========================================================================*/
#define NNSHCMN_C
#include "local.h"
#ifdef USE_ZLIB // by Nickle 
  #include "SysZLib.h"
  #include "armutls.h" // �G���f�B�A���X���b�v�Ɏg��
#endif

/*=========================================================================*/
/*   Function : ConvertHanZen                                              */
/*                                                  ���p�J�i���S�p�J�i�ϊ� */
/*=========================================================================*/
Boolean ConvertHanZen(UInt8 *dst, UInt8 *ptr)
{
    // ���p�J�i���S�p�J�i�ϊ����K�v�ȕ���
    if ((*ptr >= 0xa1)&&(*ptr <= 0xdf))
    {
        // �e�[�u���ϊ����s��
        StrCopy(dst, Han2ZenTable[((unsigned char)*ptr) - 0xa1]);
        return(true);
    }
    return (false);
}

/*=========================================================================*/
/*   Function : ConvertJIStoSJ                                             */
/*                            JIS�����R�[�h��SHIFT JIS�����R�[�h�ɕϊ����� */
/*=========================================================================*/
Boolean ConvertJIStoSJ(UInt8 *dst, UInt8 *ptr)
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
Boolean ConvertEUCtoSJ(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte)
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

/*=========================================================================*/
/*   Function : ConvertSJtoJIS                                             */
/*                                  �����R�[�h��SHIFT JIS����JIS�ɕϊ����� */
/*                                          (����܂���������̃R�[�h����) */
/*=========================================================================*/
Boolean ConvertSJtoJIS(UInt8 *dst, UInt8 *ptr)
{
    UInt8 upper, lower;

    // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
    upper   = *ptr;
    lower   = *(ptr + 1);

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
    *dst = upper;
    dst++;
    *dst = lower;

    return (true);
}


/*=========================================================================*/
/*   Function : ConvertSJtoEUC                                             */
/*                                  �����R�[�h��SHIFT JIS����EUC�ɕϊ����� */
/*=========================================================================*/
Boolean ConvertSJtoEUC(UInt8 *dst, UInt8 *ptr, UInt16 *copyByte, UInt16 *parseByte)
{
    // ���p�A���t�@�x�b�g�R�[�h�͂��̂܂ܕ\������
    if (*ptr <= 0x80)
    {
        *dst = *ptr;
        *copyByte  = 1;
        *parseByte = 1;
        return (true);
    }

    // ���p�J�i�R�[�h��EUC�����R�[�h�ɕϊ�����
    if ((*ptr >= 0xa1)&&(*ptr <= 0xdf))
    {
        *dst = 0x8e;
        dst++;
        *dst = *ptr;
        *copyByte  = 2;
        *parseByte = 1;
        return (true);
    }

    // JIS�����R�[�h�֕ϊ�
    ConvertSJtoJIS(dst, ptr);

    // EUC�����R�[�h�ɕϊ�
    *dst       = (*dst)|(0x80);
    *(dst + 1) = (*(dst + 1))|(0x80);

    *copyByte  = 2;
    *parseByte = 2;

    return (true);
}

/*=========================================================================*/
/*   Function : SendToLocalMemopad                                         */
/*                    Exchange�}�l�[�W�����g�p���ă������Ƀf�[�^��]������ */
/*                    (http://www.palmos.com/dev/support/docs/recipes/     */
/*                                    recipe_exg_mgr_send_local.html ���) */
/*=========================================================================*/
Err SendToLocalMemopad(Char *title, Char *data)
{
    ExgSocketType exgSocket;
    UInt32        size, ret, creatorId;
    Err           err;
    Char          exgName[] = "?_local:NNsiMess.txt";

    // �f�[�^�̏�����
    err  = errNone;
    size = StrLen(data) + 1;

    // �\���̂��N���A����
    MemSet(&exgSocket, sizeof(exgSocket), 0x00);

    // ���X�]����A���������N�����Ȃ��悤�ɂ���
    exgSocket.noGoTo      = 1;

    // ���X�]�����\�����s��Ȃ�
    exgSocket.noStatus    = true;

    // will comm. with memopad app
    exgSocket.description = title;
    exgSocket.name        = exgName;
    exgSocket.type        = ".TXT";

    creatorId = 0;
    ExgGetDefaultApplication(&creatorId,  exgRegExtensionID , "txt");
    if (creatorId == 'DTGP')
    {
        // Palm TX�́A�Ȃ��� Docs To Go �ɓn���Ă��܂��̂�...
        // (������� memo (�V�ł̃�����)�֓n���悤�ɂ���B
        exgSocket.target = 'PMem';
    }

    // set to comm. with local machine (PIM) only
    exgSocket.localMode = 1;

    ret = exgErrUserCancel;
    err = ExgPut(&exgSocket);
    if (err == errNone)
    {
        ret = ExgSend(&exgSocket, data, size, &err);
        err = ExgDisconnect(&exgSocket, err);
    }
    if ((err == errNone)&&(ret != exgErrUserCancel))
    {
        // �L�����Z������Ȃ������Ƃ��ɂ́A�������ɏo�͂����|�\������
        NNsh_InformMessage(ALTID_INFO, MSG_SEND_TO_MEMOPAD, " size:", size);
    }
    
    return (err);
}

/*=========================================================================*/
/*   Function : SendToExternalDevice                                       */
/*                            Exchange�}�l�[�W�����g�p���ăf�[�^��]������ */
/*                                (SendToLocalMemopad()�ƂقƂ�ǂ���Ȃ�) */
/*=========================================================================*/
Err SendToExternalDevice(Char *title, Char *data, UInt32 creator)
{
    ExgSocketType exgSocket;
    UInt32        size, ret;
    Err           err;

    // �f�[�^�̏�����
    err  = errNone;
    size = StrLen(data) + 1;

    // �\���̂��N���A����
    MemSet(&exgSocket, sizeof(exgSocket), 0x00);

    // ���X�]����A���������N�����Ȃ��悤�ɂ���
    exgSocket.noGoTo      = 1;

    // ���X�]�����\�����s��
    exgSocket.noStatus    = false;

    // will comm. with memopad app
    exgSocket.description = title;
    exgSocket.name        = "NNsiData." NNSI_EXCHANGEINFO_SUFFIX;
    exgSocket.target      = creator;

    // set to comm. with local machine (PIM) only
    // exgSocket.localMode = 1;

    ret = exgErrUserCancel;
    err = ExgPut(&exgSocket);
    if (err == errNone)
    {
        ret = ExgSend(&exgSocket, data, size, &err);
        err = ExgDisconnect(&exgSocket, err);
    }
    if ((err == errNone)&&(ret != exgErrUserCancel))
    {
        // �L�����Z������Ȃ������Ƃ��ɂ́A�������ɏo�͂����|�\������
        NNsh_InformMessage(ALTID_INFO, MSG_SEND_TO_NNSIDATA, " size:", size);
    }
    return (err);
}

/*=========================================================================*/
/*   Function : StrCopyEUC                                                 */
/*                                     EUC�����R�[�h�ɕϊ����ĕ�����R�s�[ */
/*=========================================================================*/
void StrCopyEUC(Char *dst, Char *src)
{
    UInt16 copyByte, parseByte;

    while (*src != '\0')
    {
        (void) ConvertSJtoEUC(dst, src, &copyByte, &parseByte);
        dst = dst + copyByte;
        src = src + parseByte;
    }
    *dst = *src;
    return;
}


/*=========================================================================*/
/*   Function : StrCopySJ                                                  */
/*                          EUC�����R�[�h��SHIFT JIS�ɕϊ����ĕ�����R�s�[ */
/*=========================================================================*/
void StrCopySJ(Char *dst, Char *src)
{
    UInt16 copyByte;

    while (*src != '\0')
    {
        if (ConvertEUCtoSJ(dst, src, &copyByte) == true)
        {
            dst = dst + copyByte;
            src = src + 2;
        }
        else
        {
            *dst = *src;
            dst++;
            src++;
        }
    }
    *dst = *src;
    return;
}

/*=========================================================================*/
/*   Function : StrCopyJIStoSJ                                             */
/*                          JIS�����R�[�h��SHIFT JIS�ɕϊ����ĕ�����R�s�[ */
/*=========================================================================*/
void StrCopyJIStoSJ(Char *dst, Char *src)
{
    Boolean kanji;
    
    kanji = false; 
    while (*src != '\0')
    {
        if ((*src == '\x1b')&&(*(src + 1) == '\x24')&&(*(src + 2) == '\x42'))
        {
            // �������[�h�ɐ؂�ւ�
            kanji = true;
            src = src + 3;
            continue;
        }
        if ((*src == '\x1b')&&(*(src + 1) == '\x28')&&(*(src + 2) == '\x42'))
        {
            // ANK���[�h�ɐ؂�ւ�
            kanji = false;
            src = src + 3;
            continue;
        }

        // �f�[�^�̂P�����R�s�[
        if (kanji == true)
        {
            ConvertJIStoSJ(dst, src);
            dst = dst + 2;
            src = src + 2;
        }
        else
        {
            // �ʏ탂�[�h...
            *dst = *src;
            dst++;
            src++;
        }
    }
    *dst = *src;
    return;
}

/*=========================================================================*/
/*   Function : StrNCopyHanZen                                             */
/*              ������̃R�s�[(���p�J�i���S�p�J�i�ϊ������&quot;���̍l��) */
/*=========================================================================*/
void StrNCopyHanZen(Char *dest, Char *src, UInt16 length)
{
    Char *ptr, *dst;
 
    ptr = src;
    dst = dest;
    while ((dst < dest + length)&&(*ptr != '\0'))
    {
        if (*ptr == '&')
        {
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&
                (*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);
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
                *dst++ = '&';
                ptr = ptr + 5;    // StrLen(TAG_AMP);
                continue;
            }
            // ����͂��肦�Ȃ��͂�����...�ꉞ�B
            *dst++ = '&';
            ptr++;
            continue;
        }

        // ���p�J�i���S�p�J�i�ϊ����L���ȂƂ�
        if ((NNshGlobal->NNsiParam)->convertHanZen != 0)
        {
            // �����ɕs����Ăт����ȏ���(�o�b�t�@�s���`�F�b�N)...
            if (((UInt8) *ptr >= 0x81)&&((UInt8) *ptr <= 0x9f))
            {
                // 2�o�C�g�����Ɣ���
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }

            // �����ɕs����Ăт����ȏ���(�o�b�t�@�s���`�F�b�N)...
            if (((UInt8) *ptr >= 0xe0)&&((UInt8) *ptr <= 0xef))
            {
                // 2�o�C�g�����Ɣ���
                *dst++ = *ptr;
                ptr++;
                *dst++ = *ptr;
                ptr++;
                continue;
            }
            if (ConvertHanZen((UInt8 *) dst, (UInt8 *) ptr) == true)
            {
                // ���p�J�i���S�p�ϊ�
                dst = dst + 2;
                ptr++;
                continue;
            }
        }
        *dst = *ptr;
        ptr++;
        dst++;
    }
    *dst = '\0';
    return;
}

/*=========================================================================*/
/*   Function : StrCatURLencode                                            */
/*                                           �������URL�G���R�[�h���ĘA�� */
/*=========================================================================*/
Char *StrCatURLencode(Char *dst, Char *src)
{
    Char   *ptr, logBuf[MINIBUF];
    UInt32  num;

    // �R�s�[����ꏊ��ݒ肷��
    ptr = &dst[StrLen(dst)];
    while (*src != '\0')
    {
        // �X�y�[�X�́{�ɕϊ�
        if (*src == ' ')
        {
            *ptr = '+';
            ptr++;
            src++;
            continue;
        }

        // ���s�́ACR + LF�ɕϊ�
        if (*src == '\n')
        {
            StrCopy(ptr, "%0D%0A");
            ptr = ptr + 6; // 6 == StrLen("%0D%0A");
            src++;
            continue;
        }

        // ���̂܂܏o��(���̂P)
        if ((*src == '.')||(*src == '_')||(*src == '-')||(*src == '*'))
        {
            *ptr = *src;
            ptr++;
            src++;
            continue;
        }

        // ���̂܂܏o��(���̂Q)
        if (((*src >= '0')&&(*src <= '9'))||
            ((*src >= 'A')&&(*src <= 'Z'))||
            ((*src >= 'a')&&(*src <= 'z')))
        {
            *ptr = *src;
            ptr++;
            src++;
            continue;
        }

        // ��̏����̂ǂ�ɂ����Ă͂܂�Ȃ��ꍇ�ɂ́A�R���o�[�g����B
        *ptr = '%';
        ptr++;
        
        num = (UInt32) *src;
        MemSet(logBuf, sizeof(logBuf), 0x00);
        StrIToH(logBuf, num);

        // ���l�̉��Q�����~�����̂�...
        *ptr       = logBuf[6];
        *(ptr + 1) = logBuf[7];
        ptr = ptr + 2; // 2 == StrLen(data);
        src++;
    }
    return (dst);
}

/*=========================================================================*/
/*   Function : ShowVersion_NNsh                                           */
/*                                                    �o�[�W�������̕\�� */
/*=========================================================================*/
void ShowVersion_NNsh(void)
{
    Char         buffer[BUFSIZE * 2];

    MemSet (buffer, sizeof(buffer), 0x00);
    StrCopy(buffer, "NNsi\n");
    StrCat (buffer, SOFT_VERSION);
    StrCat (buffer, "\n");
    StrCat (buffer, SOFT_REVISION);
    StrCat (buffer, "\n(");
    StrCat (buffer, SOFT_DATE);
#ifdef SMALL
    StrCat (buffer, ", OS31NNsi.prc");
#endif
#ifdef STD
    StrCat (buffer, ", NNsiSTD.prc");
#endif
#ifdef OFFLINE_DEBUG
    StrCat (buffer, "[OFFLINE DEBUG]");
#endif
    StrCat (buffer, ")");

    FrmCustomAlert(ALTID_INFO, buffer, "", "");

    return;
}

/************************************************************************
 Sample Code Disclaimer Copyright ) 2001 Palm, Inc. or its subsidiaries. 
 All rights reserved.

 You may incorporate this sample code (the "Code") into your applications
 for Palm OS� platform products and may use the Code to develop
 such applications without restriction.  The Code is provided to you on
 an "AS IS" basis and the responsibility for its operation is 100% yours.
 PALM, INC. AND ITS SUBSIDIARIES (COLLECTIVELY, "PALM") DISCLAIM
 ALL WARRANTIES, TERMS AND CONDITIONS WITH RESPECT TO THE CODE, EXPRESS,
 IMPLIED, STATUTORY OR OTHERWISE, INCLUDING WARRANTIES, TERMS OR
 CONDITIONS OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 NONINFRINGEMENT AND SATISFACTORY QUALITY.  You are not permitted to
 redistribute the Code on a stand-alone basis and you may only
 redistribute the Code in object code form as incorporated into your
 applications.  TO THE FULL EXTENT ALLOWED BY LAW, PALM ALSO EXCLUDES ANY
 LIABILITY, WHETHER BASED IN CONTRACT OR TORT (INCLUDING NEGLIGENCE), FOR
 INCIDENTAL, CONSEQUENTIAL, INDIRECT, SPECIAL OR PUNITIVE DAMAGES OF ANY
 KIND, OR FOR LOSS OF REVENUE OR PROFITS, LOSS OF BUSINESS, LOSS OF
 INFORMATION OR DATA, OR OTHER FINANCIAL LOSS ARISING OUT OF OR IN
 CONNECTION WITH THE USE OR PERFORMANCE OF THE CODE.  The Code is subject
 to Restricted Rights for U.S. government users and export regulations.

 SAMPLE NAME: GetOSFreeMem function
 
 FILE:        GetOSFreeMem.c

 DESCRIPTION: Determine the amount of free memory and the amount of
              total memory in the current device.  Results are expressed
              in KB. The returned value is the amount of free memory in all
              heaps other than the dynamic heap.  This is very ACCURATE!
 
 REVISION HISTORY:   Name    Date         Description
                     ----    ----         -----------
                     mt      09/19/00     initial version  
                     mak     01/11/01     comments/dynamic memory conversion 
 ***********************************************************************/
// PalmSource��Knowledge Base(Answer ID #291)���ꕔ����
//       (�����T�C�Y���o�C�g���ŉ�������悤�ɕύX����)
UInt32 GetOSFreeMem(UInt32 *totalMemoryP, UInt32 *dynamicMemoryP)
{
    Int16  i,         nCards;
    UInt16 cardNo,    heapID;
    UInt32 heapFree,  max;
    UInt32 freeMemory    = 0;
    UInt32 totalMemory   = 0;
    UInt32 dynamicMemory = 0;

    // Iterate through each card to support devices with multiple cards.
    nCards = MemNumCards();
    for (cardNo = 0; cardNo < nCards; cardNo++)
    {
        // Iterate through the RAM heaps on a card (excludes ROM).
        for (i = 0; i < MemNumRAMHeaps(cardNo); i++)
        {
            // Obtain the ID of the heap.
            heapID = MemHeapID(cardNo, i);

            if (MemHeapDynamic(heapID) != 0)
            {
                // If the heap is dynamic, increment the dynamic memory total.
                dynamicMemory = dynamicMemory + MemHeapSize(heapID);
            }
            else
            {
                // The heap is nondynamic.
                // Calculate the total memory and free memory of the heap.
                totalMemory = totalMemory + MemHeapSize(heapID);
                MemHeapFreeBytes(heapID, &heapFree, &max);
                freeMemory = freeMemory + heapFree;
            }
        }
    }

    if (totalMemoryP != NULL)
    {
        *totalMemoryP = totalMemory;
    }
    if (dynamicMemoryP != NULL)
    {
        *dynamicMemoryP = dynamicMemory;
    }
    return (freeMemory);
}

/*=========================================================================*/
/*   Function : ShowDeviceInfo_NNsh                                        */
/*                                                  �f�o�C�X�ʏ��̕\�� */
/*=========================================================================*/
void ShowDeviceInfo_NNsh(void)
{
    Err          ret;
    Char         *osVer, buffer[BUFSIZE * 2];
    UInt8        remainPer;
    UInt32       currentSec, useSize, totalSize, freeSize;
    DateTimeType dateBuf;

    MemSet (buffer, sizeof(buffer), 0x00);

    // ���ݎ����ƃo�b�e�������擾����
    MemSet(&dateBuf, sizeof(dateBuf), 0x00);
    remainPer      = 0;
    currentSec     = TimGetSeconds ();
    SysBatteryInfo(false, NULL, NULL, NULL, NULL, NULL, &remainPer);
    TimSecondsToDateTime(currentSec, &dateBuf);    

    // ���݂̓������o�b�t�@�ɏo�͂���
    StrCat (buffer, MSG_DEVICEINFO_DATE);
    DateToAscii((UInt8)  dateBuf.month,
                (UInt8)  dateBuf.day,
                (UInt16) dateBuf.year, 
                dfYMDLongWithDot,
                &buffer[StrLen(buffer)]);
    TimeToAscii((UInt8) dateBuf.hour,
                (UInt8) dateBuf.minute,
                tfColon24h,
                &buffer[StrLen(buffer)]);
    StrCat (buffer, "\n");

    // OS�̃o�[�W�������o�͂���
    osVer = SysGetOSVersionString();
    StrCat (buffer, "PalmOS ");
    StrCat (buffer, osVer);
    StrCat (buffer, "\n");
    MEMFREE_PTR(osVer);

    // ���݂̃o�b�e���c�ʂ��o�b�t�@�ɏo�͂���
    StrCat (buffer, MSG_DEVICEINFO_BATTERY);
    NUMCATI(buffer, (UInt16) remainPer); 
    StrCat (buffer, "%");

    // �󂫃��������̎擾
    freeSize = GetOSFreeMem(&totalSize, &useSize);
    {
        // VFS�̗e��(��/�ő�)���o�͂���
        StrCat (buffer, MSG_DEVICEINFO_MEM);
        NUMCATI(buffer, ((freeSize)/1024));
        StrCat (buffer, "kB/");
        NUMCATI(buffer, ((totalSize/1024))); 
        StrCat (buffer, "kB");
    }

    // VFS���̎擾
    if (((NNshGlobal->NNsiParam)->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS�̗e�ʂ��m�F����
        ret = VFSVolumeSize(NNshGlobal->vfsVol, &useSize, &totalSize);
        if (ret == errNone)
        {
            // VFS�̗e��(��/�ő�)���o�͂���
            StrCat (buffer, MSG_DEVICEINFO_VFS);
            NUMCATI(buffer, ((totalSize - useSize)/1024));
            StrCat (buffer, "kB/");
            NUMCATI(buffer, ((totalSize/1024))); 
            StrCat (buffer, "kB");
        }
    }
    FrmCustomAlert(ALTID_INFO, buffer, "", "");

    return;
}

/*=========================================================================*/
/*   Function : SeparateWordList                                           */
/*                                  ��������Z�p���[�^�ŕ������ăR�s�[���� */
/*=========================================================================*/
Err SeparateWordList(Char *string, NNshWordList *wordList)
{
    Char   *ptr;
    UInt16  len;

    // �L�[���[�h�����N���A����
    wordList->nofWord = 0;

    if (StrLen(string) == 0)
    {
        //  �������镶���񂪎w�肳��Ă��Ȃ������ꍇ�ɂ͏I������B
        return (errNone);
    }

    // �̈���m�ۂ��ANG��������R�s�[
    MEMFREE_PTR(wordList->wordString);
    len = StrLen(string) + MARGIN;
    wordList->wordString = MEMALLOC_PTR(len);
    if (wordList->wordString == NULL)
    {
        return (~errNone);
    }
    MemSet (wordList->wordString, len, 0x00);
    StrCopy(wordList->wordString, string);

    // �Z�p���[�^(',')�P�ʂŕ�����̐擪�|�C���^���`�F�b�N����
    ptr = wordList->wordString;
    (wordList->nofWord)++;
    ptr++;
    while (*ptr != '\0')
    {
        if (*ptr == NNSH_NGSET_SEPARATOR)
        {
            // ������̃Z�p���[�^�����I(���������؂�)
            *ptr = '\0';
            // �Z�p���[�^���A�����Ă����ꍇ�͓ǂ݂Ƃ΂�...
            do
            {
                ptr++;
            } while ((*ptr != '\0')&&(*ptr == NNSH_NGSET_SEPARATOR));
            if (*ptr != '\0')
            {
                (wordList->nofWord)++;
            }
        }
        else
        {
            ptr++;
        }
    }

    // �����񃊃X�g�̃������n���h�����쐬����
    MEMFREE_HANDLE(wordList->wordmemH);
    wordList->wordmemH = 
        SysFormPointerArrayToStrings(wordList->wordString, wordList->nofWord);

    return (errNone);
}

/*=========================================================================*/
/*   Function : ReleaseWordList                                            */
/*                                            ���X�g������̗̈��������� */
/*=========================================================================*/
void ReleaseWordList(NNshWordList *wordList)
{
    MEMFREE_PTR   (wordList->wordString);
    MEMFREE_HANDLE(wordList->wordmemH);
    wordList->nofWord = 0;    

    return;
}

/*=========================================================================*/
/*   Function : LaunchResource_NNsh                                        */
/*                                   ���\�[�X�֐�(��FDA�A�v��)���N������  */
/*=========================================================================*/
Boolean LaunchResource_NNsh(UInt32    type,    UInt32  creator,
                            DmResType resType, DmResID resID)
{
    UInt8               *funcPtr;
    Boolean              result;
    Err                  ret;
    UInt16               cardNo;
    LocalID              dbID;
    DmOpenRef            dbRef;
    MemHandle            memH;
    DmSearchStateType    state;
    FormActiveStateType  formState;

    // �ϐ��̏�����
    cardNo = 0;
    dbID   = 0;
    result = false;
    MemSet(&state,     sizeof(state),     0x00);
    MemSet(&formState, sizeof(formState), 0x00);

    // �w�肳�ꂽ���\�[�X���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &state,  type, creator, 
                                         true, &cardNo, &dbID);
    if (ret != errNone)
    {
        // �C���X�g�[������Ă��Ȃ������A�I������B
        return (false);
    }

    // �t�H�[���̌��݂̏�Ԃ��L������
    FrmSaveActiveState(&formState);
    
    // DB���E��
    dbRef = DmOpenDatabase(cardNo, dbID, dmModeReadOnly);
    if (dbRef == 0)
    {
        // DB�擾���s
        goto FUNC_END;
    }

    // ���\�[�X���擾
    memH = DmGet1Resource(resType, resID);
    if (memH == 0)
    {
        // �������n���h���擾���s
        goto CLOSE_DB;
    }
    funcPtr = MemHandleLock(memH);
    if (funcPtr == NULL)
    {
        // �A�h���X�擾���s
        goto CLOSE_DB;
    }

    ///////////////////////////////////////////
    // �擾�������\�[�X���֐��Ƃ��ČĂяo��
    ///////////////////////////////////////////
    ((void (*)(void))funcPtr)();

    // "���s������"�t���O�����Ă�
    result = true;

    // ���\�[�X�����         
    MemHandleUnlock(memH);
    DmReleaseResource(memH);

CLOSE_DB:
    (void) DmCloseDatabase(dbRef);

FUNC_END:
    // �ۑ����Ă����t�H�[���̏�Ԃ�߂�
    FrmRestoreActiveState(&formState);
    return (result);
}

/*=========================================================================*/
/*   Function : CheckInstalledResource_NNsh                                */
/*                         ���\�[�X�֐�(��FDA�A�v��)�����邩�`�F�b�N����  */
/*=========================================================================*/
Boolean CheckInstalledResource_NNsh(UInt32 type, UInt32 creator)
{
    Err                  ret;
    UInt16               cardNo;
    LocalID              dbID;
    DmSearchStateType    state;

    // �ϐ��̏�����
    cardNo = 0;
    dbID   = 0;
    MemSet(&state, sizeof(state), 0x00);

    // �w�肳�ꂽ���\�[�X���C���X�g�[������Ă��邩�m�F����
    ret = DmGetNextDatabaseByTypeCreator(true, &state,  type, creator, 
                                         true, &cardNo, &dbID);
    if (ret != errNone)
    {
        // �C���X�g�[������Ă��Ȃ������A�I������B
        return (false);
    }
    return (true);
}

/* --------------------------------------------------------------------------
 *   AMsoft�����UTF8�ϊ��e�[�u���𗘗p���Ď�������
 *       (http://amsoft.minidns.net/palm/gfmsg_chcode.html)
 * --------------------------------------------------------------------------*/

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     ������̃R�s�[(UTF8����SHIFT JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size)
{
    UInt8     buffer[2];
    UInt16    cnt, codes, ucode;
    Boolean   kanji;
    DmOpenRef dbRef;   
    LocalID   dbId;

    // �ϊ��e�[�u�������邩�m�F����
    dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        // �Ȃ������ꍇ�ɂ́A�ϊ��������̂܂܃R�s�[����
        StrNCopy(dst, src, size);
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
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

#ifdef USE_LOGCHARGE
/*-------------------------------------------------------------------------*/
/*   Function : ParseMessage_UTF8                                          */
/*                                          (���b�Z�[�W�̐��`����/RDF�`��) */
/*-------------------------------------------------------------------------*/
void ParseMessage_UTF8(Char *buf, Char *source, UInt32 size,
                       UInt16 *nofJmp, UInt16 *jmpBuf, UInt16 kanjiCode,
                       Boolean setJumpFlag)
{
    UInt16 fontFlag, ucode, codes;
    UInt8 *ptr, *dst;
    Char   convBuf[4];
    DmOpenRef dbRef;   
    LocalID   dbId;

    // �ϊ��e�[�u�������邩�m�F����
    dbId  = DmFindDatabase(0, "UnicodeToJis0208");
    if (dbId == 0)
    {
        // �Ȃ������ꍇ�ɂ́A�ϊ��������̂܂܃R�s�[����
        StrNCopy(buf, source, size);
        return;
    }
    dbRef = DmOpenDatabase(0 , dbId, dmModeReadOnly);

#ifdef USE_COLOR
    if (((NNshGlobal->NNsiParam)->useColor != 0)&&
        ((NNshGlobal->NNsiParam)->useColorMessageToken != 0)&&
        ((NNshGlobal->NNsiParam)->useSonyTinyFont != 0))
    {
        fontFlag = NNSH_ENABLE;
    }
    else
#endif  //#ifdef USE_COLOR
    {
        fontFlag = NNSH_DISABLE;
    }

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    dst  = buf;
    ptr  = source;
    MemSet(convBuf, sizeof(convBuf), 0x00);
    while (ptr < ((UInt8 *) source + size))
    {
        // UTF8�ɕ������ϊ�����...
        if (*ptr >= 0x80)
        {
            ucode = ((*ptr & 0x0f) << 12);
            ucode = ucode | ((*(ptr + 1) & 0x3f) << 6);
            ucode = ucode | ((*(ptr + 2) & 0x3f));

            convBuf[0] = ((ucode & 0xff00) >> 8);
            convBuf[1] = ((ucode & 0x00ff));

            // �����R�[�h�̕ϊ�(UTF8 -> SHIFT JIS)
            codes = Unicode11ToJisx0208(dbRef, convBuf[0], convBuf[1]);
            convBuf[0] = ((codes & 0xff00) >> 8);
            convBuf[1] = (codes & 0x00ff);
            ConvertJIStoSJ(dst, convBuf);
            ptr = ptr + 3;
            dst = dst + 2;
            continue;
        }

        if (*ptr == '&')
        {
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                ptr = ptr + 4;   // StrLen(TAG_GT);
                *dst++ = '>';
                continue;
            }
            // "&lt;" �� '<' �ɒu��
            if ((*(ptr + 1) == 'l')&&(*(ptr + 2) == 't')&&(*(ptr + 3) == ';'))
            {
                ptr = ptr + 4;   // StrLen(TAG_LT);

                ///// �G���R�[�f�B���O����Ă���A���J�[ / ���s�͓ǂݔ�΂�
                if (((*ptr == 'b')||(*ptr == 'B'))&&
                    ((*(ptr + 1) == 'r')||(*(ptr + 1) == 'R')))
                {
                    *dst++ = '\n';

                    // �A���J�[�̖����܂ŏȗ�����
                    ptr = ptr + 2;
                    while ((ptr > (UInt8 *) source)&&((*ptr != '&')||
                            (*(ptr + 1) != 'g')||(*(ptr + 2) != 't')||(*(ptr + 3) != ';')))
                    {
                        ptr++;
                    }
                    ptr = ptr + 4;
                    continue;
                }
                if ((*ptr == 'a')||(*ptr == 'A'))
                {
                    // �A���J�[�̖����܂ŏȗ�����
                    ptr++;
                    while ((ptr < ((UInt8 *) source + size))&&((*ptr != '&')||
                            (*(ptr + 1) != 'g')||(*(ptr + 2) != 't')||(*(ptr + 3) != ';')))
                    {
                        ptr++;
                    }
                    ptr = ptr + 4;
                    continue;
                }
                if ((*ptr == '/')&&((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A')))
                {
                    // �A���J�[�̖����܂ŏȗ�����
                    ptr = ptr + 2;
                    while ((ptr < ((UInt8 *) source + size))&&((*ptr != '&')||
                            (*(ptr + 1) != 'g')||(*(ptr + 2) != 't')||(*(ptr + 3) != ';')))
                    {
                        ptr++;
                    }
                    ptr = ptr + 4;
                    continue;
                }
                *dst++ = '<';
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
            //  "<>" �́A�Z�p���[�^(��������)
            if (*(ptr + 1) == '>')
            {
                ptr = ptr + 2;   // StrLen(DATA_SEPARATOR);
                continue;
            }
            //  "<br>" �́A���s�ɒu��
            if (((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r'))||
                ((*(ptr + 1) == 'B')&&(*(ptr + 2) == 'R')))
            {
                // �s���ƍs���̃X�y�[�X������Ă݂�ꍇ
                if ((ptr > (UInt8 *) source)&&(*(ptr - 1) == ' '))
                {
                    dst--;
                }
                if (*(ptr + 4) == ' ')
                {
                    *dst++ = '\n';
                    ptr = ptr + 5;        // StrLen(DATA_NEWLINE);
                    goto SKIP_TAG;
                }
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                goto SKIP_TAG;
            }

            //  "<p*>" �́A���s2�ɒu��
            if ((*(ptr + 1) == 'p')||(*(ptr + 1) == 'P'))
            {
                *dst++ = '\n';
                *dst++ = '\n';
                ptr = ptr + 2;
                goto SKIP_TAG;
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

            //  "<dd>" �́A���s�Ƌ󔒂S�ɒu��
            if (((*(ptr + 1) == 'd')&&(*(ptr + 2) == 'd')&&(*(ptr + 3) == '>'))||
                ((*(ptr + 1) == 'D')&&(*(ptr + 2) == 'D')&&(*(ptr + 3) == '>')))
            {
                *dst++ = '\n';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                *dst++ = ' ';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }

            //  "<h?" �́A���s + ���s �ɒu��
            if ((*(ptr + 1) == 'h')&&(*(ptr + 1) == 'H'))
            {
                *dst++ = '\n';
                // �����F�ɂ���
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_HEADER;
                }
                goto SKIP_TAG;
            }

            //  "</h?>" �́A���s + ���s �ɒu��
            if (((*(ptr + 2) == 'h')||(*(ptr + 2) == 'H'))&&(*(ptr + 1) == '/'))
            {
                *dst++ = '\n';
                // �ʏ�F�ɂ���
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <item �͉��s
            if (((*(ptr + 1) == 'i')||(*(ptr + 1) == 'I'))&&
                 ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                 ((*(ptr + 3) == 'e')||(*(ptr + 3) == 'E'))&&
                 ((*(ptr + 4) == 'm')||(*(ptr + 4) == 'M')))                 
            {
                *dst++ = '\n';
                ptr = ptr + 5;
                goto SKIP_TAG;
            }

            // <link �͐F�t��
            if (((*(ptr + 1) == 'l')||(*(ptr + 1) == 'L'))&&
                 ((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                 ((*(ptr + 3) == 'n')||(*(ptr + 3) == 'N'))&&
                 ((*(ptr + 4) == 'k')||(*(ptr + 4) == 'K')))                 
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_ANCHOR;
                }
                ptr = ptr + 5;
                goto SKIP_TAG;
            }


            // <IMAGE �` </IMAGE> �͕\�����Ȃ�
            if (((*(ptr + 1) == 'i')||(*(ptr + 1) == 'I'))&&
                 ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M'))&&
                 ((*(ptr + 3) == 'a')||(*(ptr + 3) == 'A'))&&
                 ((*(ptr + 4) == 'g')||(*(ptr + 4) == 'G'))&&                 
                 ((*(ptr + 5) == 'e')||(*(ptr + 5) == 'E')))
            {
                ptr = ptr + 6;
                while ((ptr < ((UInt8 *) source + size))&&
                        ((*ptr != '<')&&(*(ptr + 1) != '/'))&&
                        (((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                        ((*(ptr + 3) == 'm')||(*(ptr + 3) == 'M'))&&
                        ((*(ptr + 4) == 'a')||(*(ptr + 4) == 'A'))&&
                        ((*(ptr + 5) == 'g')||(*(ptr + 5) == 'G'))&&                 
                        ((*(ptr + 6) == 'e')||(*(ptr + 6) == 'E'))))
                {
                    ptr++;
                }
                goto SKIP_TAG;
            }


            // <TITLE �͐F�t��
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                 ((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                 ((*(ptr + 3) == 't')||(*(ptr + 3) == 'T'))&&
                 ((*(ptr + 4) == 'l')||(*(ptr + 4) == 'L'))&&                 
                 ((*(ptr + 5) == 'e')||(*(ptr + 5) == 'E')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_HEADER;
                }
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // </title �͉��s
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 't')||(*(ptr + 2) == 'T'))&&
                 ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
                 ((*(ptr + 4) == 't')||(*(ptr + 4) == 'T'))&&                 
                 ((*(ptr + 5) == 'l')||(*(ptr + 5) == 'L'))&&
                 ((*(ptr + 6) == 'e')||(*(ptr + 6) == 'E')))                 
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                ptr = ptr + 7;
                goto SKIP_TAG;
            }

            // </link �͉��s
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 'l')||(*(ptr + 2) == 'L'))&&
                 ((*(ptr + 3) == 'i')||(*(ptr + 3) == 'I'))&&
                 ((*(ptr + 4) == 'n')||(*(ptr + 4) == 'N'))&&                 
                 ((*(ptr + 5) == 'k')||(*(ptr + 5) == 'K')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // </item �͉��s
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 'i')||(*(ptr + 2) == 'I'))&&
                 ((*(ptr + 3) == 't')||(*(ptr + 3) == 'T'))&&
                 ((*(ptr + 4) == 'e')||(*(ptr + 4) == 'E'))&&                 
                 ((*(ptr + 5) == 'm')||(*(ptr + 5) == 'M')))
            {
                if (*(dst - 1) != '\n')
                {
                    *dst++ = '\n';
                }
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // </desc ��SPACE
            if (((*(ptr + 1) == '/')||(*(ptr + 1) == '/'))&&
                 ((*(ptr + 2) == 'd')||(*(ptr + 2) == 'D'))&&
                 ((*(ptr + 3) == 'e')||(*(ptr + 3) == 'E'))&&
                 ((*(ptr + 4) == 's')||(*(ptr + 4) == 'S'))&&                 
                 ((*(ptr + 5) == 'c')||(*(ptr + 5) == 'C')))
            {
                *dst++ = ' ';
                ptr = ptr + 6;
                goto SKIP_TAG;
            }

            // <a* �̓A���J�[�F��
            if (((*(ptr + 1) == 'a')||(*(ptr + 1) == 'A'))&&(*(ptr + 2) == ' '))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_ANCHOR;
                }
                goto SKIP_TAG;
            }

            // </a* �͒ʏ�F��
            if (((*(ptr + 2) == 'a')||(*(ptr + 2) == 'A'))&&(*(ptr + 1) == '/'))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <EM* �͋����F��
            if (((*(ptr + 1) == 'e')||(*(ptr + 1) == 'E'))&&
                ((*(ptr + 2) == 'm')||(*(ptr + 2) == 'M')))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_BOLDDATA;
                }
                goto SKIP_TAG;
            }

            // </em* �͒ʏ�F��
            if (((*(ptr + 2) == 'e')||(*(ptr + 2) == 'E'))&&
                ((*(ptr + 3) == 'm')||(*(ptr + 3) == 'M'))&&
                (*(ptr + 1) == '/'))
            {
                if (fontFlag != NNSH_DISABLE)
                {
                    *dst++ = NNSH_CHARCOLOR_ESCAPE;
                    *dst++ = NNSH_CHARCOLOR_NORMAL;
                }
                goto SKIP_TAG;
            }

            // <tr>, <td> �̓X�y�[�X�P�ɕϊ�
            if (((*(ptr + 1) == 't')||(*(ptr + 1) == 'T'))&&
                ((*(ptr + 2) == 'r')||(*(ptr + 2) == 'R')||
                 (*(ptr + 2) == 'd')||(*(ptr + 2) == 'D')))
            {
                *dst++ = ' ';
                goto SKIP_TAG;
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

        // �X�y�[�X���A�����Ă����ꍇ�A�P�Ɍ��炷
        if ((*ptr == ' ')&&(*(ptr + 1) == ' '))
        {
            ptr++;
            while ((*ptr == ' ')&&(ptr < ((UInt8 *) source + size)))
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

    DmCloseDatabase(dbRef);
    return;
}
#endif  // #ifdef USE_LOGCHARGE

#ifdef USE_ZLIB
/*=========================================================================*/
/*   Function :   OpenZLib                                                 */
/*                                            zlib�̏��������s�� by Nickle */
/*=========================================================================*/
Boolean OpenZLib(void)
{
    z_stream *z;

    // zlib�̏�����
    if(ZLSetup != errNone)
    {
        // ���s�BSysZLib.prc���Ȃ��H
        NNsh_DebugMessage(ALTID_ERROR, "Can't init zlib","", 0);
        return (false);
    }
 
    // z_stream�̗̈���m�ۂ���
    NNshGlobal->streamZ = MEMALLOC_PTR((sizeof(z_stream) + MARGIN));
    if (NNshGlobal->streamZ == NULL)
    {
        // �̈�m�ۂɎ��s�B�B�B
        NNsh_DebugMessage(ALTID_ERROR, "Can't allocate z_stream","", 0);
        return (false);
    }
    z = NNshGlobal->streamZ;
    MemSet(z, (sizeof(z_stream) + MARGIN), 0x00);
 
    // �o�b�t�@�̃N���A
    z->zalloc   = Z_NULL;
    z->zfree    = Z_NULL;
    z->opaque   = Z_NULL;
    z->next_in  = Z_NULL;
    z->avail_in = 0;

    // gzip�̏ꍇ�͂������������Ȃ��Ƃ����Ȃ� inflateInit(&z) ���Ⴞ��
    // thx to http://ghanyan.monazilla.org/gzip.html
    #define MAX_WBITS 15
    if(inflateInit2(NNshGlobal->streamZ, -MAX_WBITS) != Z_OK)
    {
        // ���s�B�Ȃ�ŁH
        NNsh_DebugMessage(ALTID_ERROR, "Can't init zlib","(inflateInit2())", 0);
        return (false);
    }
 
    // zlib�W�J��o�b�t�@�̊m��
    NNshGlobal->inflateBuf = MEMALLOC_PTR((NNshGlobal->NNsiParam)->bufferSize + MARGIN);
    if (NNshGlobal->inflateBuf == NULL)
    {
        // �̈�̊m�ێ��s�I
        NNsh_DebugMessage(ALTID_ERROR, "ZLIB:inflateBuf", "MEMALLOC_PTR(), size:",
                          (NNshGlobal->NNsiParam)->bufferSize + MARGIN);
        return (false);
    }
    MemSet(NNshGlobal->inflateBuf, ((NNshGlobal->NNsiParam)->bufferSize + MARGIN), 0x00);

    return (true);
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*=========================================================================*/
/*   Function :   CloseZLib                                                */
/*                                          zlib�̏I���������s�� by Nickle */
/*=========================================================================*/
void CloseZLib(void)
{
    // �m�ۂ��Ă����̈���J������
    inflateEnd(NNshGlobal->streamZ);
    MEMFREE_PTR(NNshGlobal->streamZ);
    MEMFREE_PTR(NNshGlobal->inflateBuf);
    ZLTeardown;
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*-------------------------------------------------------------------------*/
/*   Function :   GetStatusCodeSize                                        */
/*        �n���ꂽ�o�b�t�@�̐擪��offlaw.cgi�̕Ԃ������R�[�h���������ꍇ�A */
/*            ���̃T�C�Y���v�Z���ĕԂ� (InflateZLib�̓����Ŏg�p) by Nickle */
/*-------------------------------------------------------------------------*/
static UInt32 Get2chStatusCodeSize(Char* buf, UInt32 bufSize)
{
    Char   *ptr;
    UInt16  cnt;

    // [+OK] �̏ꍇ�͍���
    // [-INCR] (Incorrect)�̏ꍇ�͂��ׂẴf�[�^
    // [-ERR (�e�L�X�g)]�̏ꍇ�͂Ȃ񂩃G���[
    // ��F+OK 23094/512K
    //       -INCR 23094/512K
    //       -ERR ����ȔȂ��ł�
    // thx to �M�R�i�r ItenDownload.pas
    if (((buf[0] != '+')||(buf[1] != 'O')||(buf[2] != 'K'))&&
        ((buf[0] != '-')||(buf[1] != 'I')||(buf[2] != 'N')||
         (buf[3] != 'C')||(buf[4] != 'R'))&&
        ((buf[0] != '-')||(buf[1] != 'E')||(buf[2] != 'R')||(buf[3] != 'R')))
    {
        // �����R�[�h��������Ȃ������A�I������
        return (0);
    }

    // ���s�R�[�h��T��
    cnt = 0;
    ptr = buf;
    while ((*ptr != '\0')&&(*ptr != '\r')&&(*ptr != '\n'))
    {
        cnt++;
        ptr++;
    }
    if (*ptr == '\0')
    {
        // ���s�R�[�h��������Ȃ������A�I������
        return (0);
    }
    // ����������������
    return (cnt);
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*-------------------------------------------------------------------------*/
/*   Function :   InflateZLib                                              */
/*      gzip���k�f�[�^��W�J���A�t�@�C���ɏ�������                     */
/*                                       (ParseGZip�̓����Ŏg�p) by Nickle */
/*-------------------------------------------------------------------------*/
static Err InflateZLib(NNshFileRef* fileRef, Char* inputBuf, UInt16 inputSize, Boolean firstBlock)
{
    int      status;
    UInt32    writeSize, fileWriteSize;
    Char*     mergeBuf = NULL;
    Err       err;
    UInt32    skipBytes = 0; // offlaw.cgi�̉����R�[�h����������ǂݔ�΂��B���̃T�C�Y
    z_stream *z;

    z = NNshGlobal->streamZ;
    
    /* �o�̓|�C���^(next_out) */
    z->next_out  = NNshGlobal->inflateBuf;

    /* �o�̓o�b�t�@�c��(avail_out) */
    z->avail_out = (NNshGlobal->NNsiParam)->bufferSize;
    if (z->avail_in != 0)
    {
        // �O��̃f�[�^�̎c�肪����
        // �O��̎c��ƍ���̃f�[�^���}�[�W����
        mergeBuf = MEMALLOC_PTR(z->avail_in + inputSize);
        if (mergeBuf == NULL)
        {
            return (~errNone);
        }         
        MemMove(mergeBuf, NNshGlobal->inflateBuf, z->avail_in);
        MemMove(mergeBuf + z->avail_in, inputBuf, inputSize);

        z->next_in  = mergeBuf;
        z->avail_in = z->avail_in + inputSize;
    }
    else
    {
        // �O��̃f�[�^�̎c��͂Ȃ�
        z->next_in  = inputBuf;
        z->avail_in = inputSize;
    }

    // gzip�W�J�����{����
    status = inflate(z, Z_NO_FLUSH);
    if ((status != Z_OK)&&(status != Z_STREAM_END))
    {
        NNsh_DebugMessage(ALTID_ERROR, "zlib decode error", z->msg, status);
        MEMFREE_PTR(mergeBuf);
        z->avail_in = 0; // ����ɃS�~��inflate���Ȃ��悤�ɃN���A���Ă���
        return (~errNone);
    }

    while ((z->avail_in != 0)&&(status != Z_STREAM_END))
    {
        // �܂����̓f�[�^������̂Ńt�@�C���ɏ����o���đ�����W�J
        fileWriteSize = (NNshGlobal->NNsiParam)->bufferSize - z->avail_out;

        // content�̍ŏ��Ȃ�Aofflaw.cgi�̕Ԃ��X�e�[�^�X�R�[�h�����Ă邩��
        if (firstBlock != false)
        {
            skipBytes = Get2chStatusCodeSize(NNshGlobal->inflateBuf, fileWriteSize);
        }
        else
        {
            skipBytes = 0;
        }

        // �t�@�C���ɒǋL����
        err = AppendFile_NNsh(fileRef, (fileWriteSize - skipBytes), (NNshGlobal->inflateBuf + skipBytes), &writeSize);
        if (err != errNone)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", err);
        }
        if (writeSize == 0)
        {
            NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " write size == 0", writeSize);
        }

        // �o�̓|�C���^��߂�(���̓|�C���^�͎����Ői��ł���)
        z->next_out  = NNshGlobal->inflateBuf;
        z->avail_out = (NNshGlobal->NNsiParam)->bufferSize;

        // ������W�J
        status = inflate(z, Z_NO_FLUSH);
        if ((status != Z_OK)&&(status != Z_STREAM_END))
        {
            NNsh_DebugMessage(ALTID_ERROR, "zlib decode error(2)", z->msg, status);
            z->avail_in = 0; // ����ɃS�~��inflate���Ȃ��悤��
            break;
        }
    }

    // �Ō�̈��̏�������
    fileWriteSize = (NNshGlobal->NNsiParam)->bufferSize - z->avail_out;

    // content�̍ŏ��Ȃ�Aofflaw.cgi�̕Ԃ��X�e�[�^�X�R�[�h�����Ă邩��
    if(firstBlock != false)
    {
        skipBytes = Get2chStatusCodeSize(NNshGlobal->inflateBuf, fileWriteSize);
    }
    else
    {
        skipBytes = 0;
    }

    // �t�@�C���ɒǋL����
    err = AppendFile_NNsh(fileRef, (fileWriteSize - skipBytes), (NNshGlobal->inflateBuf + skipBytes), &writeSize);
    if (err != errNone)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " ret:", err);
    }
    if (writeSize == 0)
    {
        NNsh_DebugMessage(ALTID_ERROR, "AppendFile() ", " write size == 0", writeSize);
    }

    if(z->avail_in != 0)
    {
         // ���̓o�b�t�@�̖����Ƀf�R�[�h�ł��ĂȂ��f�[�^���c���Ă�
         // �Ȃ̂ŁA�Ƃ肠���������inflateBuf�ɓ���Ă����Ď���g��
         MemMove(NNshGlobal->inflateBuf, z->next_in, z->avail_in);
    }
    MEMFREE_PTR(mergeBuf);

    return err;
}
#endif  // #ifdef USE_ZLIB

#ifdef USE_ZLIB
/*=========================================================================*/
/*   Function :   ParseGZip                                                */
/*                               gzip���k���ꂽcontent��W�J���� by Nickle */
/*=========================================================================*/
Err ParseGZip(NNshFileRef* fileRef, Char* inputBuf, UInt16 inputSize, Boolean firstBlock)
{
    // see RFC 1952 - GZIP file format specification version 4.3
    // #pragma pack(1) ���ƂȂ���ignore�����̂ł��������Ă݂�
    struct gzipHeader
    {
        UInt8  id1   __attribute__((packed));
        UInt8  id2   __attribute__((packed));
        UInt8  cm    __attribute__((packed));
        UInt8  flg   __attribute__((packed));
        UInt32 mtime __attribute__((packed));
        UInt8  xfl   __attribute__((packed));
        UInt8  os    __attribute__((packed));
    };

    // flg�̃r�b�g�}�X�N
    // static UInt8 FTEXT       = (1 << 0); �����ł͎g��Ȃ�
    static UInt8 FHCRC      = (1 << 1);
    static UInt8 FEXTRA     = (1 << 2);
    static UInt8 FNAME      = (1 << 3);
    static UInt8 FCOMMENT   = (1 << 4);

    struct gzipHeader* gzHeader;
  
    gzHeader = (struct gzipHeader*)inputBuf;
  
    if ((firstBlock != false)&&
        (gzHeader->id1 == 0x1f)&&
        (gzHeader->id2 == 0x8b)&&
        (((gzHeader->flg) >> 5) == 0))
    {
        // �w�b�_�̏����𖞂����̂ŁA�w�b�_����ǂݔ�΂�
        int headerSize = sizeof(struct gzipHeader);

        if ((gzHeader->flg & FEXTRA) != 0)
        {
            headerSize = headerSize + 2 + ByteSwap16(*(unsigned int*)(inputBuf + headerSize));
        }

        if ((gzHeader->flg & FNAME) != 0)
        {
            headerSize = headerSize + StrLen(inputBuf + headerSize);
        }

        if ((gzHeader->flg & FCOMMENT) != 0)
        {
            headerSize = headerSize + StrLen(inputBuf + headerSize);
        }

        if ((gzHeader->flg & FHCRC) != 0)
        {
            headerSize = headerSize + 2;
        }

        // �w�b�_����������zlib�ɓn��
        NNsh_DebugMessage(ALTID_INFO, "info", " gzip headerSize:", headerSize);
        return (InflateZLib(fileRef, (inputBuf + headerSize), (inputSize - headerSize), firstBlock));
    }

    // �w�b�_�ł͂Ȃ��f�[�^�̓r���Ȃ̂ł��̂܂�zlib�ɓn��
    // (���S�ȃ`�F�b�N�ł͂Ȃ����c���p����Ȃ��Ǝv��)
    return InflateZLib(fileRef, inputBuf, inputSize, firstBlock);
}
#endif  // #ifdef USE_ZLIB

/*=========================================================================*/
/*   Function :   NNsh_BeepSound                                           */
/*                                                              ����炷 */
/*=========================================================================*/
void NNsh_BeepSound(SndSysBeepType soundType)
{
#ifdef USE_WAVE_MELODY
    if ((soundType == sndAlarm)&&((NNshGlobal->NNsiParam)->melody != 0))
    {
        // Sound Stream Feature���T�|�[�g����Ă��邩�m�F����
        Err    ret;
        UInt32 flags;
        ret = FtrGet(sysFileCSoundMgr, sndFtrIDVersion, &version);
        if (ret == errNone)
        {
            SndPtr soundPtr; 

            // TO DO
            // WAVE�̃��\�[�X�����܂��A�|�C���^��soundPtr�ɐݒ肷��..
            //

            Int32  volume = sndAlarmVolume;
            flags = sndFlagSync;
            ret = SndPlayResource(soundPtr, volume, flags);
            if (ret != errNone)
            {
                SndPlaySystemSound(soundType);
            }
        }
    }
    else
#endif  // #ifdef USE_WAVE_MELODY
    {
        if (soundType == sndClick)
        {
            SndPlaySystemSound(sndAlarm);
        }
        else
        {
            SndPlaySystemSound(soundType);
        }
    }
	return;
}
