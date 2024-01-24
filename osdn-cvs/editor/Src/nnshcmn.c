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
    UInt32        size;
    Err           err;

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
    exgSocket.name        = "NNsiMess.txt";

    // set to comm. with local machine (PIM) only
    exgSocket.localMode = 1;

    err = ExgPut(&exgSocket);
    if (err == errNone)
    {
        ExgSend(&exgSocket, data, size, &err);
        ExgDisconnect(&exgSocket, err);
    }

    NNsh_InformMessage(ALTID_INFO, MSG_SEND_TO_MEMOPAD, "", err);
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
/*   Function : StrCopySJtoJIS                                             */
/*                                     JIS�����R�[�h�ɕϊ����ĕ�����R�s�[ */
/*=========================================================================*/
void StrCopySJtoJIS(UInt8 *dst, UInt8 *src)
{
    UInt16 mode;

    mode = 0;  // ANK���[�h
    while (*src != '\0')
    {
        // ���p�A���t�@�x�b�g�R�[�h�͂��̂܂ܕ\������
        if (*src <= 0x80)
        {
            if (mode == 1)
            {
                *dst = 0x1b;
                dst++;
                *dst = 0x28;
                dst++;
                *dst = 0x42;
                dst++;
            }
            mode = 0;
 
            *dst = *src;
            dst++;
            src++;
        }
        else
        {
            if (mode == 0)
            {
                *dst = 0x1b;
                dst++;
                *dst = 0x24;
                dst++;
                *dst = 0x42;
                dst++;
            }
            mode = 1;  // KNJ���[�h

            (void) ConvertSJtoJIS(dst, src);
            dst = dst + 2;
            src = src + 2;
        }
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
/*   Function : convertLF                                                  */
/*                                                LF(���s�R�[�h)��ϊ����� */
/*=========================================================================*/
static void convertLF(UInt16 newLine, UInt8 *dst, UInt16 *size)
{
    // LF�R�[�h��ϊ�����
    switch (newLine)
    {
      case NNSH_LINECODE_CR:
        // ���s�R�[�h�́ACR
        *dst = 0x0d;
        *size = 1;
        break;

      case NNSH_LINECODE_CRLF:
        // ���s�R�[�h�́ACR + LF
        *dst = 0x0d;
        *(dst + 1) = 0x0a;
        *size = 2;
        break;

      case NNSH_LINECODE_LF:
      default:
        // ���s�R�[�h�́ALF
        *dst = 0x0a;
        *size = 1;
        break;
    }
    return;
}

/*-------------------------------------------------------------------------*/
/*   Function : checkNewLine                                               */
/*                                          ���s�R�[�h���ǂ����`�F�b�N���� */
/*-------------------------------------------------------------------------*/
static Boolean checkNewLine(UInt16 newLine, UInt8 *src, UInt16 *size)
{
    *size = 0;
    switch (newLine)
    {
      case NNSH_LINECODE_CR:
        // ���s�R�[�h�́ACR
        if (*src == 0x0d)
        {
            *size = 1;
            return (true);
        }
        break;

      case NNSH_LINECODE_CRLF:
        // ���s�R�[�h�́ACR + LF
        if ((*src == 0x0d)&&(*(src + 1) == 0x0a))
        {
            *size = 2;
            return (true);
        }
        break;

      case NNSH_LINECODE_LF:
      default:
        // ���s�R�[�h�́ALF
        if (*src == 0x0a)
        {
            *size = 1;
            return (true);
        }
        break;
    }
    return (false);
}

/*=========================================================================*/
/*   Function : StrNCopyJIStoSJ                                            */
/*                      ������̃R�s�[(JIS����SHIFT JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopyJIStoSJ(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine)
{
    UInt16 cnt, codes;
    Boolean kanji;
    
    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        // ���s�R�[�h
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // ���s�R�[�h���o�ALF�ɕϊ�����
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }

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
            cnt = cnt + 2;
            src = src + 2;
        }
        else
        {
            // �ʏ탂�[�h...
            *dst = *src;
            dst++;
            cnt++;
            src++;
        }
    }
    *dst = '\0';
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoSJLF                                           */
/*                                    ������̃R�s�[(���s�R�[�h�̕ϊ����{) */
/*=========================================================================*/
void StrNCopySJtoSJLF(Char *dst, Char *src, UInt32 size, UInt16 newLine)
{
    UInt16 cnt, codes;
    
    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // ���s�R�[�h
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // ���s�R�[�h���o�ALF�ɕϊ�����
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }

        // 1�o�C�g�R�s�[
        *dst = *src;
        dst++;
        cnt++;
        src++;
    }
    *dst = '\0';

    return;
}

/*=========================================================================*/
/*   Function : StrNCopyEUCtoSJ                                            */
/*                      ������̃R�s�[(EUC����SHIFT JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopyEUCtoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine)
{
    UInt16 copyByte, cnt, codes;

    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // ���s�R�[�h���o
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }

        if (ConvertEUCtoSJ(dst, src, &copyByte) == true)
        {
            dst = dst + copyByte;
            cnt = cnt + copyByte;
            src = src + 2;
        }
        else
        {
            *dst = *src;
            dst++;
            cnt++;
            src++;
        }
    }
    *dst = '\0';
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoJIS                                            */
/*                      ������̃R�s�[(SHIFT JIS����JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopySJtoJIS(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    Char   buffer[4];
    UInt16 mode, cnt, codes;

    mode = 0;  // ANK���[�h
    cnt  = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // ���s�R�[�h(LF)���R���o�[�g����
        // ���s�R�[�h(LF)���R���o�[�g����
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }

        // ���p�A���t�@�x�b�g�R�[�h�͂��̂܂ܕ\������
        if (*src <= 0x80)
        {
            if (mode == 1)
            {
                if (isFtr == true)
                {
                    buffer[0] = 0x1b;
                    buffer[1] = 0x28;
                    buffer[2] = 0x42;
                    DmWrite(dst, cnt, buffer, 3);
                    cnt = cnt + 3;
                }
                else
                {
                    *(dst + cnt) = 0x1b;
                    cnt++;
                    *(dst + cnt) = 0x28;
                    cnt++;
                    *(dst + cnt) = 0x42;
                    cnt++;
                }
            }
            mode = 0;
 
            if (isFtr == true)
            {
                DmWrite(dst, cnt, src, 1);
            }
            else
            {
                *(dst + cnt) = *src;
            }
            cnt++;
            src++;
        }
        else
        {
            if (mode == 0)
            {
                if (isFtr == true)
                {
                    buffer[0] = 0x1b;
                    buffer[1] = 0x24;
                    buffer[2] = 0x42;
                    DmWrite(dst, cnt, buffer, 3);
                    cnt = cnt + 3;
                }
                else
                {
                    *(dst + cnt) = 0x1b;
                    cnt++;
                    *(dst + cnt) = 0x24;
                    cnt++;
                    *(dst + cnt) = 0x42;
                    cnt++;
                }
            }
            mode = 1;  // KNJ���[�h

            if (isFtr == true)
            {
                (void) ConvertSJtoJIS(buffer, src);
                DmWrite(dst, cnt, buffer, 2);
            }
            else
            {
                (void) ConvertSJtoJIS((dst + cnt), src);            
            }
            cnt = cnt + 2;
            src = src + 2;
        }
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoEUC                                            */
/*                      ������̃R�s�[(SHIFT JIS����EUC�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopySJtoEUC(Char *dst, Char *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    Char   buffer[4];
    UInt16 copyByte, parseByte, cnt, codes;

    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // ���s�R�[�h(LF)���R���o�[�g����
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }
        if (isFtr == true)
        {
            (void) ConvertSJtoEUC(buffer, src, &copyByte, &parseByte);
            DmWrite(dst, cnt, buffer, copyByte);
        }
        else
        {
            (void) ConvertSJtoEUC((dst + cnt), src, &copyByte, &parseByte);
        }
        cnt = cnt + copyByte;
        src = src + parseByte;
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : StrNCopySJtoSJ                                             */
/*                                    ������̃R�s�[(���s�R�[�h�̕ϊ����{) */
/*=========================================================================*/
void StrNCopySJtoSJ(Char *dst, Char *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    Char buffer[2];
    UInt16 cnt, codes;
    
    cnt = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // ���s�R�[�h(LF)���R���o�[�g����
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }

        // 1�o�C�g�R�s�[
        if (isFtr == true)
        {
            DmWrite(dst, cnt, src, 1);
        }
        else
        {
            *(dst + cnt) = *src;
        }
        cnt++;
        src++;
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}
/*=========================================================================*/
/*   Function : StrNCopySJtoUTF8                                           */
/*                      ������̃R�s�[(SHIFT JIS����JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopySJtoUTF8(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine, Boolean isFtr)
{
    UInt8  buffer[4];
    UInt16 cnt, codes;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    cnt  = 0;
    while ((*src != '\0')&&(cnt < size))
    {
        // ���s�R�[�h(LF)���R���o�[�g����
        if (*src == 0x0a)
        {
            if (isFtr == true)
            {
                convertLF(newLine, buffer, &codes);
                DmWrite(dst, cnt, buffer, codes);
            }
            else
            {
                convertLF(newLine, (dst + cnt), &codes);
            }
            cnt = cnt + codes;
            src++;
            continue;
        }

        // ���p�A���t�@�x�b�g�R�[�h�͂��̂܂ܕ\������
        if (*src <= 0x80)
        { 
            if (isFtr == true)
            {
                DmWrite(dst, cnt, src, 1);
            }
            else
            {
                *(dst + cnt) = *src;
            }
            cnt++;
            src++;
            continue;
        }

        // �����R�[�h��SJ -> JIS�ϊ�
        (void) ConvertSJtoJIS(buffer, src);

        // ���̌�AUTF8�ϊ�...
        codes = Jisx0208ToUnicode11(NNsiGlobal->jis2unicodeDB, buffer[0], buffer[1]);

        // UCS-4 > UTF-8 3byte
        buffer[0] = (((codes >> 12) & 0x000F) | 0xE0);
        buffer[1] = (((codes >>  6) & 0x003F) | 0x80);
        buffer[2] = (((codes)       & 0x003F) | 0x80);
        if (isFtr == true)
        {
            DmWrite(dst, cnt, buffer, 3);
        }
        else
        {
            *(dst + cnt)     = buffer[0];
            *(dst + cnt + 1) = buffer[1];
            *(dst + cnt + 2) = buffer[2];
        }
        cnt = cnt + 3;
        src = src + 2;
    }
    if (isFtr == true)
    {
        buffer[0] = '\0';
        DmWrite(dst, cnt, buffer, 1);
    }
    else
    {
        *(dst + cnt) = '\0';
    }
    return;
}

/*=========================================================================*/
/*   Function : StrNCopyUTF8toSJ                                           */
/*                     ������̃R�s�[(UTF8����SHIFT JIS�R�[�h�ւ̕ϊ����{) */
/*=========================================================================*/
void StrNCopyUTF8toSJ(UInt8 *dst, UInt8 *src, UInt32 size, UInt16 newLine)
{
    UInt8  buffer[2];
    UInt16 cnt, codes, ucode;
    Boolean kanji;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);
    
    kanji = false; 
    cnt = 0;
    while ((*src != '\0')&&(cnt <size))
    {
        // ���s�R�[�h
        if (checkNewLine(newLine, src, &codes) == true)
        {
            // ���s�R�[�h���o�ALF�ɕϊ�����
            *dst = 0x0a;
            dst++;
            cnt++;
            src = src + codes;
            continue;
        }
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
        codes = Unicode11ToJisx0208(NNsiGlobal->unicode2jisDB, buffer[0], buffer[1]);
        buffer[0] = ((codes & 0xff00) >> 8);
        buffer[1] = (codes & 0x00ff);
        ConvertJIStoSJ(dst, buffer);

        dst = dst + 2;
        src = src + 3;
        cnt = cnt + 2;
    }
    *dst = '\0';
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
    NNshSavedPref *NNsiParam;
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_PARAMPTR, (UInt32 *)&NNsiParam);

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
    MemPtrFree(osVer);

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
    if ((NNsiParam->useVFS & NNSH_VFS_ENABLE) != 0)
    {
        // VFS�̗e�ʂ��m�F����
        ret = VFSVolumeSize(NNsiGlobal->vfsVol, &useSize, &totalSize);
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
    wordList->wordString = MemPtrNew_NNsh(len);
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
/*   Function : ShowVersion_NNsh                                           */
/*                                                    �o�[�W�������̕\�� */
/*=========================================================================*/
void ShowVersion_NNsh(void)
{
    Char         buffer[BUFSIZE * 2];

    MemSet (buffer, sizeof(buffer), 0x00);
    StrCopy(buffer, SOFT_NAME);
    StrCat (buffer, "\n");
    StrCat (buffer, SOFT_VERSION);
    StrCat (buffer, SOFT_REVISION);
    StrCat (buffer, "\n(");
    StrCat (buffer, SOFT_DATE);
#ifdef OFFLINE_DEBUG
    StrCat (buffer, "[OFFLINE DEBUG]");
#endif
    StrCat (buffer, ")\n    ");
    StrCat (buffer, SOFT_INFO);
    FrmCustomAlert(ALTID_INFO, buffer, "", "");

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

/*=========================================================================*/
/*   Function : MemPtrNew_NNsh                                             */
/*                                                         �������m�ۊ֐�  */
/*=========================================================================*/
MemPtr MemPtrNew_NNsh(UInt32 size)
{
    NNshWorkingInfo *NNsiGlobal;

    // �p�����[�^�̈�̎擾
    FtrGet(SOFT_CREATOR_ID, FTRID_GLOBALPTR, (UInt32 *)&NNsiGlobal);

    if (NNsiGlobal->palmOSVersion > 0x04000000)
    {
        // over 64kB�̗̈���m�ۂ���(OS4.x�ȏ�)
        return (MemGluePtrNew(size));
    }
    // �ʏ�̃������m��
    return (MemPtrNew(size));
}
