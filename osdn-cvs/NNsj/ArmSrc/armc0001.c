/*============================================================================*
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

/** ���b�Z�[�W�̍ő僌�X�ԍ� **/
#define NNSH_MESSAGE_MAXNUMBER     1005

// �Ƃ肠�����̃v���g�^�C�v�錾
unsigned long CreateThreadIndexSub_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP);

/*=========================================================================*/
/*   Function : CreateThreadIndexSub_ARM                                   */
/*                                              (�C���f�b�N�X�擾�̎�����) */
/*=========================================================================*/
unsigned long CreateThreadIndexSub_ARM(const void *emulStateP, void *userData68KP, Call68KFuncType *call68KFuncP)
{
    char           *buffer, *chkPtr, *ptr;
    unsigned long   size, offset, *nofMsg, *msgOffset, msg, *args;

    // �������擾(�G���f�B�A���ϊ����s��)
    /*****************************************************************/
    args      = (unsigned long *) userData68KP;
    msgOffset = (unsigned long *) ReadUnaligned32(&args[0]);
    nofMsg    = (unsigned long *) ReadUnaligned32(&args[1]);
    offset    =                   ReadUnaligned32(&args[2]);
    size      =                   ReadUnaligned32(&args[3]);
    buffer    = (char *)          ReadUnaligned32(&args[4]);
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
        if (*ptr == '\0')
        {
            // ��؂肪�Ȃ�����A��͏I��
            break;
        }
        ptr = ptr + 1;
        if (msg < NNSH_MESSAGE_MAXNUMBER)
        {
            msg++;
            // �\���̂Ƀf�[�^��������
            WriteUnaligned32((msgOffset + msg),
                     ((unsigned long) ptr - (unsigned long) buffer + offset));
            chkPtr = ptr;
        }
        else
        {
            WriteUnaligned32(nofMsg, 0);
            return (0xffff);
        }
        chkPtr = ptr;
    } while (chkPtr < (buffer + size));

    // ���b�Z�[�W������������
    WriteUnaligned32(nofMsg, msg);
    return (0x0000);
}
