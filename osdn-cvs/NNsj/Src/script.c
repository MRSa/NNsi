/*============================================================================*
 *  FILE: 
 *     script.c
 *
 *  Description: 
 *     �}�N���X�N���v�g�̓ǂݍ���/�����o������
 *
 *===========================================================================*/
#define SCRIPT_C
#include "local.h"

/*=========================================================================*/
/*   Function : readMacroDBData                                            */
/*                                                  �}�N���f�[�^�̓ǂݏo�� */
/*=========================================================================*/
Err ReadMacroDBData(Char *dbName, UInt16 ver, UInt32 creator, UInt32 type,
                           UInt16 *cnt, UInt16 size, Char *dataPtr)
{
    Err              ret;
    UInt16           lp;
    NNshMacroRecord *lineData;
    DmOpenRef        dbRefR;

    *cnt = 0;
    if (dataPtr == NULL)
    {
        // �o�b�t�@�i�[�̈悪�Ȃ��A�I�[�v�����s
        return (~errNone);
    }

    // �f�[�^�x�[�X���J��(�X�N���v�g�t�@�C��)
    OpenDB_wCreator(dbName, ver, creator, type, false, &dbRefR);
    if (dbRefR == 0)
    {
        // �G���[�����A�I������
        return (~errNone);
    }

    // �X�N���v�g�̃��R�[�h�����m�F����
    GetDBCount_NNsh(dbRefR, cnt);

    // �i�[�̈���z�����Ƃ��́A�o�b�t�@�T�C�Y�܂łɂ���
    if (*cnt > size)
    {
        *cnt = size;
    }

    // �f�[�^�̏�����
    lineData = (NNshMacroRecord *) dataPtr;
 
    // ���O�f�[�^�̃o�b�t�@�ւ̓ǂݍ���
    for (lp = 0; lp < *cnt; lp++)
    {
        // ���R�[�h��ǂݏo��
        ret = GetRecord_NNsh(dbRefR, lp, sizeof(NNshMacroRecord), &lineData[lp]);
        if (ret != errNone)
        {
            // ���R�[�h�ǂݏo�����s�A�f�[�^���N���A���Ď����R�[�h��
            MemSet(&lineData[lp], sizeof(NNshMacroRecord), 0x00);
        }
    }

    // ���Ƃ��܂�
    CloseDatabase_NNsh(dbRefR);
    return (errNone);
}

/*=========================================================================*/
/*   Function : WriteMacroDBData                                           */
/*                                                  �}�N���f�[�^�̏����o�� */
/*=========================================================================*/
Err WriteMacroDBData(Char *dbName, UInt16 ver, UInt32 creator, UInt32 type, 
                                                   UInt16 cnt, Char *dataPtr)
{
    UInt16           lp;
    DmOpenRef        dbRefW;
    NNshMacroRecord *lineData;

    // �f�[�^�x�[�X��V�K�ɊJ��
    OpenDB_wCreator(dbName, ver, creator, type, true, &dbRefW);
    if (dbRefW == 0)
    {
        // �G���[�����A���������I������
        return (~errNone);
    }

    // �f�[�^�̏�����
    lineData = (NNshMacroRecord *) dataPtr;
 
    // ���O�f�[�^�̃o�b�t�@�ւ̓ǂݍ���
    for (lp = 0; lp < cnt; lp++)
    {
        // ���R�[�h�������o��
        lineData[lp].seqNum = lp + 1;
        (void) EntryRecord_NNsh(dbRefW, sizeof(NNshMacroRecord), &lineData[lp]);
    }

    // ���R�[�h�̕��בւ������{
    QsortRecord_NNsh(dbRefW, NNSH_KEYTYPE_UINT16, 0);

    // ���Ƃ��܂�
    CloseDatabase_NNsh(dbRefW);
    return (errNone);
}

/*=========================================================================*/
/*   Function : AllocateMacroDBData                                        */
/*                                                      �}�N���f�[�^�̊m�� */
/*=========================================================================*/
Char *AllocateMacroDBData(UInt16 len)
{
    Char *dataPtr;

    // �f�[�^�̏�����
    dataPtr = MemPtrNew_NNsh(sizeof(NNshMacroRecord) * len + MARGIN);
    if (dataPtr == NULL)
    {
        // �������m�ۃG���[�����A�I������
        return (NULL);
    }
    MemSet(dataPtr, (sizeof(NNshMacroRecord) * len + MARGIN), 0x00);

    return (dataPtr);
}

/*=========================================================================*/
/*   Function : ReleaseMacroDBData                                         */
/*                                              �m�ۂ����}�N���f�[�^�̊J�� */
/*=========================================================================*/
void ReleaseMacroDBData(Char *dataPtr)
{
    if (dataPtr != NULL)
    {
        MemPtrFree(dataPtr);
    }
    return;
}
