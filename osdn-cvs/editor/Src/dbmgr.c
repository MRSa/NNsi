/*============================================================================*
 *  FILE: 
 *     dbMgr.c
 *
 *  Description: 
 *     �f�[�^�x�[�X�}�l�[�W��
 *===========================================================================*/
#define DBMGR_C
#include "local.h"

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF                                                */
/*                                                (�\�[�e�B���O�p��r�֐�) */
/*                        �� DB����other�o�C�g�ڂɁu������Łvkey�������  */
/*                                                         ������̂Ƃ���B*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_Char(void *rec1, void *rec2, Int16 other,
                              SortRecordInfoPtr       rec1SortInfo,
                              SortRecordInfoPtr       rec2SortInfo, 
                              MemHandle               appInfoH)
{
    Char *str1, *str2;

    str1 = ((Char *) rec1) + other;
    str2 = ((Char *) rec2) + other;
    return (StrCompare(str1, str2));
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt32                                         */
/*                                                (�\�[�e�B���O�p��r�֐�) */
/*                        �� DB����other�o�C�g�ڂɁuUInt32�Łvkey�������  */
/*                                                         ������̂Ƃ���B*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt32(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt32 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt32 *) loc1;
    data2 = (UInt32 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : aplCompareF_UInt16                                         */
/*                                                (�\�[�e�B���O�p��r�֐�) */
/*                    �� key���擪���other�o�C�g�ڂ���uUInt16�Łv�����  */
/*                                                         ������̂Ƃ���B*/
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_UInt16(void *rec1, void *rec2, Int16 other,
                                SortRecordInfoPtr       rec1SortInfo,
                                SortRecordInfoPtr       rec2SortInfo, 
                                MemHandle               appInfoH)
{
    Char   *loc1,  *loc2;
    UInt16 *data1, *data2;

    loc1  = ((Char *) rec1) + other;
    loc2  = ((Char *) rec2) + other;

    data1 = (UInt16 *) loc1;
    data2 = (UInt16 *) loc2;

    return (*data1 - *data2);
}

/*-------------------------------------------------------------------------*/
/*   Function : searchRecordSub                                            */
/*                                                        (���R�[�h������) */
/*-------------------------------------------------------------------------*/
static Err searchRecordSub(DmOpenRef dbRef, void *target, UInt16 keyType, 
                           UInt16   offset, UInt16 *idx,  MemHandle *hnd)
{
    UInt16     nofData, start, end, mid;
    Int16      chk;
    MemHandle  dataH;
    Char      *data;

    // �������镶������L���X�g����
    data = (Char *)target;
    data = data + offset;

    // �f�[�^�����[���������猟�����s
    nofData = DmNumRecords(dbRef);
    if (nofData == 0)
    {
        return (~errNone - 5);
    }

    // �Q�������Ŏw�肳�ꂽ�f�[�^���o�^����Ă��邩�`�F�b�N
    mid   = 0;
    start = 0;
    end   = nofData;
    while (start <= end)
    {
        mid = start + (end - start) / 2;
        dataH = DmQueryRecord(dbRef, mid);
        if (dataH == 0)
        {
            // ������Ȃ������I (�����Ŕ�����͗l)
            return (~errNone);
        }

        // �w�肳�ꂽ�L�[�ɂ���āA�`�F�b�N���@��ς���
        data = (Char *) MemHandleLock(dataH);
        data = data + offset;        // offset���Ȃ������A�A�A
        switch (keyType)
        {
          case NNSH_KEYTYPE_UINT32:
            chk  = *((UInt32 *) data) - *((UInt32 *) target);
            break;

          case NNSH_KEYTYPE_UINT16:
            chk  = *((UInt16 *) data) - *((UInt16 *) target);
            break;

          case NNSH_KEYTYPE_CHAR:
          default:
            chk  = StrCompare(data, (Char *) target);
            break;
        }
        if (chk == 0)
        {
            // ���������I(�f�[�^���܂邲�ƃR�s�[)
            MemHandleUnlock(dataH);
            *idx  = mid;
            *hnd  = dataH;
            return (errNone);
        }
        MemHandleUnlock(dataH);
        if (chk < 0)
        {
            start = mid + 1;
        }
        else
        {
            end   = mid - 1;
        }
    }
    return (~errNone - 4);
}

/*==========================================================================*/
/*  OpenDatabase_NNsh : �f�[�^�x�[�X�A�N�Z�X�̏�����                        */
/*                                                                          */
/*==========================================================================*/
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef)
{
    LocalID dbId;
    UInt16  dbVersion;

    // �f�[�^�x�[�X�����݂��邩�m�F����B
    *dbRef    = 0;
    dbVersion = 0;
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // �f�[�^�x�[�X�̃o�[�W�����ԍ����擾
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (dbVersion < chkVersion)
        {
            // �f�[�^�x�[�X�̃o�[�W�������Â��Ƃ��́A�f�[�^�x�[�X���폜����
            (void) DmDeleteDatabase(0, dbId);
            dbId = 0;
        }
    }

    // �f�[�^�x�[�X�����݂��Ȃ��Ƃ�
    if (dbId == 0)
    {
        // �f�[�^�x�[�X��V�K�쐬
        (void) DmCreateDatabase(0, dbName, 
                                SOFT_CREATOR_ID, SOFT_DBTYPE_ID, false);

        dbId      = DmFindDatabase(0, dbName);
        if (dbId != 0)
        {
            // DB�Ƀo�[�W�����ԍ���ݒ�
            dbVersion = chkVersion;
            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, &dbVersion, NULL,
                                     NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        }
    }

    // �f�[�^�x�[�X�̃I�[�v��
    *dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);

    return;
}

/*==========================================================================*/
/*  closeDatabase_NNsh() : �f�[�^�x�[�X���N���[�Y����                       */
/*                                                                          */
/*==========================================================================*/
void CloseDatabase_NNsh(DmOpenRef dbRef)
{
    // DB���I�[�v���ł��Ă�����CLOSE����B
    if (dbRef != 0)
    {
        (void) DmCloseDatabase(dbRef);
        dbRef = 0;
    }
    return;
}

/*=========================================================================*/
/*   Function : QsortRecord_NNsh                                           */
/*                                                (���R�[�h�̃\�[�e�B���O) */
/*=========================================================================*/
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // �N�C�b�N�\�[�g���d�|����
    switch (keyType)
    {

      case NNSH_KEYTYPE_UINT32:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : IsortRecord_NNsh                                           */
/*                                                (���R�[�h�̃\�[�e�B���O) */
/*=========================================================================*/
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset)
{

    // �}���\�[�g���d�|����
    switch (keyType)
    {
      case NNSH_KEYTYPE_UINT32:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt32, offset);
        break;

      case NNSH_KEYTYPE_UINT16:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_UInt16, offset);
        break;

      case NNSH_KEYTYPE_CHAR:
      default:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_Char, offset);
        break;
    }
    return;
}

/*=========================================================================*/
/*   Function : EntryRecord_NNsh                                           */
/*                                               (�c�a�̐擪�Ƀf�[�^�o�^)  */
/*=========================================================================*/
Err EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData)
{
    MemHandle  newRecH;
    UInt16     index;
    void      *dbP;

    // ���R�[�h�̐V�K�ǉ�
    index   = 0;
    newRecH = DmNewRecord(dbRef, &index, size + sizeof(UInt32));
    if (newRecH == 0)
    {
        // ���R�[�h�ǉ��G���[
        return (DmGetLastErr());
    }

    // ���R�[�h�Ƀf�[�^��������
    dbP = MemHandleLock(newRecH);
    DmWrite(dbP, 0, recordData, size);
    MemHandleUnlock(newRecH);

    // ���R�[�h������i�ύX�I���j
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*=========================================================================*/
/*   Function : IsExistRecord_NNsh                                         */
/*                                                (���R�[�h�̑��݃`�F�b�N) */
/*=========================================================================*/
Err IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index)
{
    MemHandle  dataH;

    *index  = 0;
    dataH   = 0;
    return (searchRecordSub(dbRef, target, keyType, offset, index, &dataH));
}

/*=========================================================================*/
/*   Function : IsExistRecordRR_NNsh                                       */
/*                     (���R�[�h�̑��݃`�F�b�N - �擪���疖���܂łP���Â�) */
/*=========================================================================*/
Err IsExistRecordRR_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType,
                       UInt16 offset,   UInt16 *index)
{
    UInt16     nofData, loop;
    MemHandle  dataH;
    Int16      chk;
    Char      *data;

    // �f�[�^�����[���������猟�����s
    nofData = DmNumRecords(dbRef);
    if (nofData == 0)
    {
        return (~errNone - 5);
    }

    for (loop = 0; loop < nofData; loop++)
    {
        dataH = DmQueryRecord(dbRef, loop);
        if (dataH == 0)
        {
            // ������Ȃ������I (�����Ŕ�����͗l)
            return (~errNone);
        }
        // �w�肳�ꂽ�L�[�ɂ���āA�`�F�b�N���@��ς���
        data = (Char *) MemHandleLock(dataH);
        data = data + offset;  // �I�t�Z�b�g��������
        switch (keyType)
        {
          case NNSH_KEYTYPE_UINT32:
            chk  = *((UInt32 *) data) - *((UInt32 *) target);
            break;

          case NNSH_KEYTYPE_UINT16:
            chk  = *((UInt16 *) data) - *((UInt16 *) target);
            break;

          case NNSH_KEYTYPE_CHAR:
          default:
            chk  = StrCompare(data, (Char *) target);
            break;
        }
        MemHandleUnlock(dataH);
        if (chk == 0)
        {
            // ���������I
            *index  = loop;
            return (errNone);
        }     
    }
    *index = 0;
    return (~errNone - 4);
}

/*=========================================================================*/
/*   Function : SearchRecord_NNsh                                          */
/*                                                        (���R�[�h������) */
/*=========================================================================*/
Err SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,
                      UInt16 offset, UInt16 size, void *matchedData,
                      UInt16 *index)
{
    Err        ret;
    MemHandle  dataH;
#ifdef COPY_SELF
    Char      *sc,  *ds;
    UInt32    *src, *dest;
    UInt16     start;
#else
    UInt32    *src;
#endif

    *index = 0;
    dataH  = 0;

    ret    = searchRecordSub(dbRef, target, keyType, offset, index, &dataH);
    if (ret != errNone)
    {
        // �f�[�^�����炸
        return (ret);
    }
    if (dataH == 0)
    {
        // �������n���h���ُ�
        return ((~errNone) - 2);
    }

    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // �������|�C���^�ُ�
        return ((~errNone) - 3);
    }

    // �f�[�^���܂邲�ƃR�s�[���ĉ���
#ifdef COPY_SELF
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }
#else
    MemMove(matchedData, src, size);
#endif

    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : DeleteRecord_NNsh                                          */
/*                                                    (�Y�����R�[�h���폜) */
/*=========================================================================*/
Err DeleteRecord_NNsh(DmOpenRef dbRef, void  *target,
                      UInt16 keyType,  UInt16 offset)
{
    Err       ret;
    UInt16    index;
    MemHandle dataH;

    index = 0;
    dataH = 0;
    ret = searchRecordSub(dbRef, target, keyType, offset, &index, &dataH);
    if (ret != errNone)
    {
        return (errNone + 1);
    }

    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : DeleteRecordIDX_NNsh                                       */
/*                                                    (�Y�����R�[�h���폜) */
/*=========================================================================*/
Err DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index)
{
    // �f�[�^�����I�[�o���Ă��邩�m�F
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone);
    }
    return (DmRemoveRecord(dbRef, index));
}

/*=========================================================================*/
/*   Function : GetRecord_NNsh                                             */
/*                                          (���R�[�h���C���f�b�N�X�Ŋl��) */
/*=========================================================================*/
Err GetRecord_NNsh(DmOpenRef dbRef, UInt16 index,
                   UInt16 size, void *matchedData)
{
    MemHandle  dataH;
#ifdef COPY_SELF
    UInt16     start;
    UInt32    *src, *dest;
    Char      *sc,  *ds;
#else
    UInt32    *src;
#endif

    // �f�[�^�����I�[�o���Ă��邩�m�F
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    dataH = DmQueryRecord(dbRef, index);
    if (dataH == 0)
    {
        // ������Ȃ������I
        return (~errNone);
    }
    src = (UInt32 *) MemHandleLock(dataH);
    if (src == NULL)
    {
        // �������|�C���^�ُ�
        return ((~errNone) - 3);
    }

    // �f�[�^���܂邲�ƃR�s�[���ĉ���
#ifdef COPY_SELF
    MemSet(matchedData, size, 0x00);
    dest   = (UInt32 *) matchedData;
    for (start = 0; start < (size - sizeof(UInt32));
         start = start + sizeof(UInt32))
    {
        *dest++ = *src++;
    }
    for (sc = (Char *) src, ds = (Char *) dest;start < size;
         start = start + sizeof(Char))
    {
         *ds++ = *sc++;
    }
#else
    MemMove(matchedData, src, size);
#endif
    MemHandleUnlock(dataH);
    return (errNone);
}

/*=========================================================================*/
/*   Function : GetRecordReadOnly_NNsh                                     */
/*                                          (���R�[�h���C���f�b�N�X�Ŋl��) */
/*=========================================================================*/
Err GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index,
                           MemHandle *dataH, void **record)
{
    // �f�[�^�����I�[�o���Ă��邩�m�F
    if (index >= DmNumRecords(dbRef))
    {
        return (~errNone - 6);
    }

    *dataH = DmQueryRecord(dbRef, index);
    if (*dataH == 0)
    {
        // ������Ȃ������I
        return (~errNone);
    }

    // �̈�����b�N����
    *record = MemHandleLock(*dataH);
    if (record == NULL)
    {
        // �������|�C���^�ُ�
        return ((~errNone) - 3);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : ReleaseRecordReadOnly_NNsh                                 */
/*                                                (���R�[�h�ǂݏo�����I��) */
/*=========================================================================*/
Err ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH)
{
    if (dataH != 0)
    {
        MemHandleUnlock(dataH);
    }
    return (errNone);
}

/*=========================================================================*/
/*   Function : UpdateRecord_NNsh                                          */
/*                                          (���R�[�h���C���f�b�N�X�ōX�V) */
/*=========================================================================*/
Err UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, 
                      UInt16 size, void *updateData)
{
    MemHandle recH;
    void     *dbP;

    recH = DmGetRecord(dbRef, index);
    if (recH == 0)
    {
        // ���R�[�h�Q�ƃG���[
        return (DmGetLastErr());
    }

    // ���R�[�h�Ƀf�[�^��������
    dbP = MemHandleLock(recH);
    DmWrite(dbP, 0, updateData, size);
    MemHandleUnlock(recH);

    // ���R�[�h������i�ύX�I���j
    DmReleaseRecord(dbRef, index, true);

    return (errNone);
}

/*=========================================================================*/
/*   Function : GetRecordID_NNsh                                           */
/*                                      (���R�[�h�̃��j�[�N��ID���擾����) */
/*=========================================================================*/
Err GetRecordID_NNsh(DmOpenRef dbRef, UInt16 index, UInt32 *recId)
{
    return (DmRecordInfo(dbRef, index, NULL, recId, NULL));
}

/*=========================================================================*/
/*   Function : GetRecordIndexFromRecordID_NNsh                            */
/*                  (���R�[�h�̃��j�[�N��ID����C���f�b�N�X�ԍ����擾����) */
/*=========================================================================*/
Err GetRecordIndexFromRecordID_NNsh(DmOpenRef dbRef, UInt32 recId, UInt16 *index)
{
    return (DmFindRecordByID(dbRef, recId, index));
}

/*==========================================================================*/
/*  GetDBCount_NNsh : �f�[�^�x�[�X�ɓo�^����Ă���f�[�^��������������      */
/*                                                                          */
/*==========================================================================*/
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count)
{
    *count = DmNumRecords(dbRef);
    return;
}

/*==========================================================================*/
/*  GetDBInfo_NNsh : �f�[�^�x�[�X�̏��̎擾                               */
/*                                                                          */
/*==========================================================================*/
Err GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info)
{
    LocalID dbId;
    
    dbId = DmFindDatabase(0, dbName);

    // �f�[�^�x�[�X�����݂��Ȃ��Ƃ�
    if (dbId == 0)
        return(~errNone);

    return(DmDatabaseInfo(0, dbId,
                          info->nameP, info->attributesP,
                          info->versionP, info->crDateP,
                          info->modDateP, info->bckUpDateP,
                          info->modNumP, info->appInfoIDP,
                          info->sortInfoIDP, info->typeP,
                          info->creatorP));
}
