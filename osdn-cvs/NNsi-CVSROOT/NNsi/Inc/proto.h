/*============================================================================*
 *
 * $Id: proto.h,v 1.35 2004/12/26 14:46:24 mrsa Exp $
 *
 *  FILE: 
 *     proto.h
 *
 *  Description: 
 *     Prototype definitions for NNsh.
 *
 *===========================================================================*/

/************************   MACROs   **************************************/
#define NUMCATI(buf,num)   StrIToA(&(buf)[StrLen((buf))], (num))
#define NUMCATH(buf,num)   StrIToH(&(buf)[StrLen((buf))], (num))

// �̈�̊m��(�|�C���^��)
#define MEMALLOC_PTR(size)    MemPtrNew(size)

// �̈�̃N���A(�|�C���^��)
#define MEMFREE_PTR(ptr)        \
    if (ptr != NULL)            \
    {                           \
        MemPtrFree(ptr);        \
        ptr = NULL;             \
    }

// �̈�̃N���A(�n���h����)
#define MEMFREE_HANDLE(handle)  \
    if (handle != 0)            \
    {                           \
        MemHandleFree(handle);  \
        handle = 0;             \
    }

// ���݂̃I�u�W�F�N�g�|�C���^���擾����
#define FRM_GET_ACTIVE_OBJECT_PTR(index)                                   \
                FrmGetObjectPtr(FrmGetActiveForm(),                         \
                                FrmGetObjectIndex(FrmGetActiveForm(),index))

#define LaunchDA_NNsh(creator) LaunchResource_NNsh('DAcc',creator,'code',1000)

#define NNsh_ErrorMessage(altId,mes1,mes2,num)   NNsh_DialogMessage(NNSH_LEVEL_ERROR,   altId, mes1, mes2, num)
#define NNsh_WarningMessage(altId,mes1,mes2,num) NNsh_DialogMessage(NNSH_LEVEL_WARN,    altId, mes1, mes2, num)
#define NNsh_InformMessage(altId,mes1,mes2,num)  NNsh_DialogMessage(NNSH_LEVEL_INFO,    altId, mes1, mes2, num)
#define NNsh_ConfirmMessage(altId,mes1,mes2,num) NNsh_DialogMessage(NNSH_LEVEL_CONFIRM, altId, mes1, mes2, num)
#define NNsh_DebugMessage(altId,mes1,mes2,num)   NNsh_DialogMessage(NNSH_LEVEL_DEBUG,   altId, mes1, mes2, num)

////////////////////////////////////////////////////////////////////////////
//   �v���g�^�C�v�錾�́A�}���`�Z�O�����g�̃Z�N�V�����錾�Ƌ��L�����邽�߁A
// �����ɂ͋L�q���Ȃ��B�B
////////////////////////////////////////////////////////////////////////////
