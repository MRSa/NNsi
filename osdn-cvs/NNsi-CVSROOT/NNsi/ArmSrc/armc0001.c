/*============================================================================*
 *
 *  $Id: armc0001.c,v 1.4 2003/12/19 02:21:46 mrsa Exp $
 *
 *  FILE: 
 *     index.c
 *
 *  Description: 
 *     Calc. Message Index Location.
 *
 *===========================================================================*/
#define INDEX_C
#include "PceNativeCall.h"
#include "armutls.h"

///////////////////////////////////////////////////////////////////////
// ���\�[�X�^�C�v��ID��o�^(���܂������Ȃ��悤��...)
#include "Standalone.h"
STANDALONE_CODE_RESOURCE_TYPE_ID('armc',0x0001);
///////////////////////////////////////////////////////////////////////

// �Ƃ肠�����̃v���g�^�C�v�錾
unsigned long CreateThreadIndexSub_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP);

/*=========================================================================*/
/*   Function : CreateThreadIndexSub_ARM                                   */
/*                                              (�C���f�b�N�X�擾�̎�����) */
/*=========================================================================*/
unsigned long CreateThreadIndexSub_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP)
{
    char           *buffer, *chkPtr, *ptr;
    unsigned long   size, msgLimit, offset, *nofMsg, *msgOffset, msg, *args;

    // �������擾(�G���f�B�A���ϊ����s��)
    /*****************************************************************/
    args      = (unsigned long *) userData68KP;
    msgOffset = (unsigned long *) ReadUnaligned32(&args[0]);
    nofMsg    = (unsigned long *) ReadUnaligned32(&args[1]);
    offset    =                     ReadUnaligned32(&args[2]);
    msgLimit  =                     ReadUnaligned32(&args[3]);
    size      =                     ReadUnaligned32(&args[4]);
    buffer    = (char *)           ReadUnaligned32(&args[5]);
    /*****************************************************************/

    // ���݂̃��b�Z�[�W�����擾����
    msg    = ReadUnaligned32(nofMsg);
    chkPtr = buffer;
    do {
        // ���b�Z�[�W�̋�؂��T��
        ptr = chkPtr;
        while ((*ptr != '\x0a')&&(*ptr != '\0'))
        {
            ptr++;
        }
        if (*ptr != '\0')
        {
            ptr = ptr + 1;
            if (msg < msgLimit)
            {
                msg++;
                // �\���̂Ƀf�[�^��������
                WriteUnaligned32((msgOffset + msg),
                     ((unsigned long) ptr - (unsigned long) buffer + offset));
                chkPtr = ptr;
            }
            else
            {
                // �I�[�o�t���[���ɂ��l���i�[����悤�ύX����
                WriteUnaligned32(nofMsg, msg);
                return (0xffff);
            }
        }
        else
        {
            // ��؂肪�Ȃ��A�Ƃ肠�������ɂ�����
            ptr = ptr + 1;
        }
        chkPtr = ptr;
    } while (chkPtr < (buffer + size));

    // ���b�Z�[�W������������
    WriteUnaligned32(nofMsg, msg);
    return (0x0000);
}
