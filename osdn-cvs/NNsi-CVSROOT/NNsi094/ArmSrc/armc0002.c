/*============================================================================*
 *  FILE: 
 *     parse.c
 *
 *  Description: 
 *     ���b�Z�[�W�̐��`(ARMlet)
 *
 *===========================================================================*/
#define PARSE_C
#include "PceNativeCall.h"
#include "armutls.h"

///////////////////////////////////////////////////////////////////////
// ���\�[�X�^�C�v��ID��o�^(���܂������Ȃ��悤��...)
#include "Standalone.h"
STANDALONE_CODE_RESOURCE_TYPE_ID('armc',0x0002);
///////////////////////////////////////////////////////////////////////

/*** Jump�p�o�b�t�@�̍ő�T�C�Y ***/
#define JUMPBUF                     12
/*** ���b�Z�[�W�ԍ��̍ő�l ***/
#define NNSH_MESSAGE_LIMIT          1000

/**********  ���b�Z�[�W�p�[�X�p�V���{��(parse.c)  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

// �Ƃ肠�����̃v���g�^�C�v�錾
void ParseMessage_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP);
/*=========================================================================*/
/*   Function : CreateThreadIndexSub_ARM                                   */
/*                                              (�C���f�b�N�X�擾�̎�����) */
/*=========================================================================*/
void ParseMessage_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP)
{
    char          *buf, *source, *ptr, *dst, *num;
    unsigned long  nofJmp, *jmpBuf, *numjmp; 
    unsigned long  size, dataStatus, *args, jmp;

    // �������擾(�G���f�B�A���ϊ����s��)
    /*****************************************************************/
    args    = userData68KP;
    buf     = (char *) ReadUnaligned32(&args[0]);
    source  = (char *) ReadUnaligned32(&args[1]);
    size    =          ReadUnaligned32(&args[2]);
    numjmp  = (unsigned long *) ReadUnaligned32(&args[3]);
    jmpBuf  = (unsigned long *) ReadUnaligned32(&args[4]);
    /*****************************************************************/

    nofJmp     = 0;
    dataStatus = MSGSTATUS_NAME;

    // ���x�����߃��[�`������... (�P�����Âp�[�X����)
    dst  = buf;
    ptr  = source;
    while (ptr < source + size)
    {
        if (*ptr == '&')
	{
            // "&gt;" �� '>' �ɒu��
            if ((*(ptr + 1) == 'g')&&(*(ptr + 2) == 't')&&
		(*(ptr + 3) == ';'))
            {
                *dst++ = '>';
                ptr = ptr + 4;   // StrLen(TAG_GT);

                if ((*ptr <= '9')&&(*ptr >= '0'))
                {
                    // �X���Ԃ��擾
                    num = ptr;
                    jmp = 0;
                    while ((jmp < NNSH_MESSAGE_LIMIT)&&
                           (*num <= '9')&&(*num >= '0'))
                    {
                        jmp = jmp * 10 + (*num - '0');
                        num++;
                    }
                    if ((nofJmp < JUMPBUF)&&(jmp != 0))
                    {
                        // �X���Ԃ��o�b�t�@�ɒǉ�
                        WriteUnaligned32(&jmpBuf[nofJmp], jmp);
                        (nofJmp)++;
                    }
                }
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
                    break;

                  case MSGSTATUS_NORMAL:
                  default:
                    // ���b�Z�[�W�̏I�[
                    *dst++ = ' ';
                    *dst++ = '\0';
                    WriteUnaligned32(numjmp, nofJmp);
                    return;
                    break;
                }
                continue;
            }
            //  "<br>" �́A���s�ɒu��
            if ((*(ptr + 1) == 'b')&&(*(ptr + 2) == 'r')&&
		(*(ptr + 3) == '>'))
            {
                *dst++ = '\n';
                ptr = ptr + 4;        // StrLen(DATA_NEWLINE);
                continue;
            }
            // �^�O�͓ǂݔ�΂�
            while ((*ptr != '>')&&(*ptr != '\0'))
            {
                ptr++;
            }
            ptr++;
            *dst++ = ' ';
            continue;
        }

        // �I�[�����𖳎�����悤�ɕύX����
        if (*ptr != '\0')
        {
            // ���ʂ̈ꕶ���]��
            *dst++ = *ptr;
        }
	ptr++;
    }
    *dst++ = '\0';

    WriteUnaligned32(numjmp, nofJmp);
    return;
}
