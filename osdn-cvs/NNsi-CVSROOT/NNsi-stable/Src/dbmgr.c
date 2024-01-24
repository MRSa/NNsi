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
/*                       �� NNsi�f�[�^�x�[�X�̍\���̂��g�p���ĕ��בւ����� */
/*-------------------------------------------------------------------------*/
static Int16 aplCompareF_Subject(void *rec1, void *rec2, Int16 other,
                                 SortRecordInfoPtr       rec1SortInfo,
                                 SortRecordInfoPtr       rec2SortInfo, 
                                 MemHandle               appInfoH)
{
    NNshSubjectDatabase *p1, *p2;
    Int16                ret;
    UInt8                sts1, sts2;
    
    p1 = (NNshSubjectDatabase *)rec1;
    p2 = (NNshSubjectDatabase *)rec2;

    // ���C�ɓ���X���Ȃ�A�D��x���ɕ��בւ���
    sts1 = (p1->msgAttribute & NNSH_MSGATTR_FAVOR);
    sts2 = (p2->msgAttribute & NNSH_MSGATTR_FAVOR);
    if (sts1 != sts2)
    {
        return (sts1 - sts2);
    }

#ifdef USE_SORT_BBS_NORMAL_ORDER
    // ���т�����(A��Z)�ɂ���ꍇ�ɂ́A���̃��W�b�N��L���ɂ���
    // (�ʏ�͋t���Ń\�[�g����)
    if ((p1->boardNick[0] == '!' && p2->boardNick[0] != '!') ||
        (p1->boardNick[0] != '!' && p2->boardNick[0] == '!'))
    {
        NNshSubjectDatabase *p3;

        /* �ǂ��炩����� !GikoShippo �̏ꍇ�́A�\�[�g���ʂ𔽓]������ */
        /* �i�K���O�ɏW�܂邩��...�j                                   */
        p3 = p1;
        p1 = p2;
        p2 = p3;
    }
    ret = StrCompare(p2->boardNick, p1->boardNick);
#else
    ret = StrCompare(p1->boardNick, p2->boardNick);
#endif
    if (ret != 0)
    {
        // �{�[�h���Ń\�[�g����
        return (ret);
    }

    // �{�[�h���������Ƃ��ŁA��Ԃ��قȂ�Ƃ�
    if (p1->state != p2->state)
    {
        switch (p1->state)
        {
          case NNSH_SUBJSTATUS_NEW:
          case NNSH_SUBJSTATUS_UPDATE:
          case NNSH_SUBJSTATUS_REMAIN:
          case NNSH_SUBJSTATUS_ALREADY: 
          case NNSH_SUBJSTATUS_UNKNOWN: 
            if (p2->state == NNSH_SUBJSTATUS_NOT_YET)
            {
                // ���ǂ͌��
                return (1);
            }
            break;
   
          case NNSH_SUBJSTATUS_NOT_YET:
          default:
            if (p2->state != NNSH_SUBJSTATUS_NOT_YET)
            {
                // ���ǂ͌��
                return (-1);
            }
            break;
        }
    }

    /*** �X���ԍ��Ń\�[�g���� ***/
    return (StrCompare(p2->threadFileName, p1->threadFileName));
}

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

/*=========================================================================*/
/*   Function : DeleteDB_wCreator                                          */
/*                                    �f�[�^�x�[�X�̍폜(�N���G�[�^ID�w��) */
/*=========================================================================*/
Err DeleteDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type)
{
    LocalID    dbId;
    UInt16     dbVersion;

    // �N���G�[�^ID���w�肳��Ă��Ȃ��ꍇ�ɂ́A�f�t�H���g��creatorID���g�p����
    if (creator == 0)
    {
        creator = SOFT_CREATOR_ID;
    }
    if (type == 0)
    {
        type    = SOFT_DBTYPE_ID;
    }

    // �f�[�^�x�[�X�����݂��邩�m�F����B
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // �f�[�^�x�[�X�̃o�[�W�����ԍ����擾
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (dbVersion == chkVer)
        {
            // �f�[�^�x�[�X�̃o�[�W��������v�����Ƃ��̂ݍ폜����
            (void) DmDeleteDatabase(0, dbId);
            return (errNone);
        }
    }
    // �f�[�^�x�[�X�̍폜�����s���Ȃ������Ƃ��ɂ́A�G���[��������
    return (~errNone);
}

/*=========================================================================*/
/*   Function : OpenDB_wCreator                                            */
/*                                �f�[�^�x�[�X�̃I�[�v��(�N���G�[�^ID�w��) */
/*=========================================================================*/
Err OpenDB_wCreator(Char *dbName, UInt16 chkVer, UInt32 creator, UInt32 type,
                                             Boolean erase, DmOpenRef *dbRef)
{
    LocalID    dbId;
    UInt16     dbVersion;

    // �N���G�[�^ID���w�肳��Ă��Ȃ��ꍇ�ɂ́A�f�t�H���g��creatorID���g�p����
    if (creator == 0)
    {
        creator = SOFT_CREATOR_ID;
    }
    if (type == 0)
    {
        type    = SOFT_DBTYPE_ID;
    }

    // �f�[�^�x�[�X�����݂��邩�m�F����B
    *dbRef    = 0;
    dbId      = DmFindDatabase(0, dbName);
    if (dbId != 0)
    {
        // �f�[�^�x�[�X�̃o�[�W�����ԍ����擾
        (void) DmDatabaseInfo(0, dbId, NULL, NULL, &dbVersion, NULL, 
                              NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if ((dbVersion < chkVer)||(erase == true))
        {
            // �f�[�^�x�[�X�̃o�[�W�������Â��Ƃ��A�܂��͏������[�h���������́A
            // ��������f�[�^�x�[�X���폜����
            (void) DmDeleteDatabase(0, dbId);
            dbId = 0;
        }
    }

    // �f�[�^�x�[�X�����݂��Ȃ��Ƃ�
    if (dbId == 0)
    {
        // �f�[�^�x�[�X��V�K�쐬
        (void) DmCreateDatabase(0, dbName, creator, type, false);

        dbId      = DmFindDatabase(0, dbName);
        if (dbId != 0)
        {
            // DB�Ƀo�[�W�����ԍ���ݒ�
            dbVersion = chkVer;
            (void) DmSetDatabaseInfo(0, dbId, NULL,NULL, &dbVersion, NULL,
                                     NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        }
    }

    // �f�[�^�x�[�X�̃I�[�v��
    *dbRef = DmOpenDatabase(0 , dbId, dmModeReadWrite);

    // �G���[�����̔���
    if (*dbRef == 0)
    {
        return (~errNone);
    }
    return (errNone);
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

      case NNSH_KEYTYPE_SUBJECT:
        (void) DmQuickSort(dbRef, (DmComparF *) aplCompareF_Subject, offset);
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

      case NNSH_KEYTYPE_SUBJECT:
        (void) DmInsertionSort(dbRef, (DmComparF *)aplCompareF_Subject,offset);
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

/*==========================================================================*/
/*  GetExistRecordList : ���X���擾���Ă���X���b�h��DB�̃��R�[�h���̎擾 */
/*  (�ꗗ�ŏd�������Ȃ����߂̏��擾)                                      */
/*==========================================================================*/
Boolean GetExistRecordList(DmOpenRef dbRef, Char *boardNick, MemHandle *listH, UInt16 *threadNum)
{
    UInt16 nofRecords, i;
    MemHandle            recH;
    NNshSubjectDatabase *chkDb;
    Char *buf;
    UInt32 areaSize, size;
    
    size       = 0;
    areaSize   = 0;
    *threadNum = 0;
    
    // �o�^�ς݃��R�[�h�����l��
    GetDBCount_NNsh(dbRef, &nofRecords);

    // boardNick �̃X�����i�[����Ă��� DB �̐擪��T���i���X���̈�̊m�ہj
    for (i = 0; i < nofRecords; i++)
    {
        (void)GetRecordReadOnly_NNsh(dbRef, i, &recH, (void **)&chkDb);
        if (StrCompare(chkDb->boardNick, boardNick) == 0)
        {
            areaSize = MAX_THREADFILENAME + 2 + 6; // 6�̓��X���i�[�p�̈�(4 + 2) 
            *listH   = MemHandleNew(areaSize);
            if(*listH == 0)
            {
                // �̈�m�ێ��s
                return (false);
            }

            // �X���̃t�@�C�����ƃ��X�����L�^����
            buf      = MemHandleLock(*listH);
            MemSet (buf, areaSize, 0x00);
            StrCopy(buf, chkDb->threadFileName);
            switch (chkDb->state)
            {
              case NNSH_SUBJSTATUS_UPDATE:
              case NNSH_SUBJSTATUS_REMAIN:
              case NNSH_SUBJSTATUS_ALREADY:
              case NNSH_SUBJSTATUS_NEW:
                // ���X�擾�ς݂̏ꍇ�́ADB�̒l�𔽉f������
                NUMCATI(buf, chkDb->maxLoc);
                break;

              default:
                // ���擾�̏ꍇ�ɂ̓[�����i�[����
                NUMCATI(buf, 0);
                break;
            }

            // �X���̃t�@�C�����̃Z�p���[�^�Ƃ��� '\x0a' ���g�p����
            size = StrLen(buf);
            buf[size] = '\x0a';

            MemHandleUnlock(*listH);
            (*threadNum)++;
            
            ReleaseRecordReadOnly_NNsh(dbRef, recH);
            break;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, recH);
    }

    // DB �ɓ����Ă��Ȃ���� false ��Ԃ��B
    if (*threadNum == 0)
    {
        return (false);
    }
    
    // boardNick �̃X�����i�[����Ă���΁A���X�g�ɒǉ�����
    for (i += 1; i < nofRecords; i++)
    {
         (void)GetRecordReadOnly_NNsh(dbRef, i, &recH, (void **)&chkDb);
        
        if (!StrCompare(chkDb->boardNick, boardNick))
        {
            // pos �̓Z�p���[�^�̈ʒu�������Ă��邽�߁A
            // pos + 1 + len + 1 -> pos + len + 2 �̗̈���m�ۂ���
            areaSize = size + MAX_THREADFILENAME + 2 + 6; // 6�̓��X���i�[�p�̈�(4 + 2) 
            if (MemHandleResize(*listH, areaSize) != errNone)
            {
                // �̈�g�厸�s...
                MemHandleFree(*listH);
                ReleaseRecordReadOnly_NNsh(dbRef, recH);
                return(false);
            }
            // �X���̃t�@�C������ۑ�            
            buf = MemHandleLock(*listH);
            MemSet (&buf[size + 1], (areaSize - (size + 1)), 0x00);
            StrCat (buf, chkDb->threadFileName);
            NUMCATI(buf, chkDb->maxLoc);

            // �X���̃t�@�C�����̃Z�p���[�^�Ƃ��� '\x0a' ���g�p����
            size = StrLen(buf);
            buf[size] = '\x0a';

            MemHandleUnlock(*listH);
            (*threadNum)++;
        }
        ReleaseRecordReadOnly_NNsh(dbRef, recH);
    }
    return(true);
}
