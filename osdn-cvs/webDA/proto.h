/*============================================================================*
 *  $Id: proto.h,v 1.1.1.1 2004/08/25 14:42:32 mrsa Exp $
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


/******************  FUNCTION PROTOTYPES  *********************************/
/////  �ݒ�L�^�E�����֐��Q(machine.c)

/////  �f�[�^�x�[�X����֐��Q(dbmgr.c)
void OpenDatabase_NNsh(Char *dbName, UInt16 chkVersion, DmOpenRef *dbRef);
void CloseDatabase_NNsh(DmOpenRef dbRef);
void QsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
void IsortRecord_NNsh(DmOpenRef dbRef, UInt16 keyType, UInt16 offset);
Err  EntryRecord_NNsh(DmOpenRef dbRef, UInt16 size, void *recordData);
Err  IsExistRecord_NNsh(DmOpenRef dbRef, void   *target,  UInt16 keyType, UInt16 offset, UInt16 *index);
Err  SearchRecord_NNsh(DmOpenRef dbRef, void *target, UInt16 keyType,UInt16 offset, UInt16 size, void *matchedData, UInt16 *index);
Err  DeleteRecord_NNsh(DmOpenRef dbRef, void  *target, UInt16 keyType,  UInt16 offset);
Err  DeleteRecordIDX_NNsh(DmOpenRef dbRef, UInt16 index);
Err  GetRecord_NNsh(DmOpenRef dbRef, UInt16 index, UInt16 size, void *matchedData);
Err  GetRecordReadOnly_NNsh(DmOpenRef dbRef, UInt16 index, MemHandle *dataH, void **record);
Err  ReleaseRecordReadOnly_NNsh(DmOpenRef dbRef, MemHandle dataH);
Err  UpdateRecord_NNsh(DmOpenRef dbRef, UInt16 index, UInt16 size, void *updateData);
void GetDBCount_NNsh(DmOpenRef dbRef, UInt16 *count);
Err  GetDBInfo_NNsh(Char *dbName, NNshDBInfo *info);
                      
/////  �l�b�g���[�N����n�֐��Q (netmgr.c)
Err NNshNet_open (UInt16 *netRef);
Err NNshNet_close(UInt16 netRef);
Err NNshNet_write(NNshWorkingInfo *NNshGlobal);
Err NNshNet_read (NNshWorkingInfo *NNshGlobal);
Err NNshNet_disconnect(UInt16 netRef, NetSocketRef socketRef, Int32 timeout);
Err NNshNet_connect(NNshWorkingInfo *NNshGlobal);

void NNshNet_LineHangup(void);

/////   ���̑��̃��[�e�B���e�B�֐��Q(nnshcmn.c)
Err    SendToLocalMemopad(Char *title, Char *data);
void   ShowVersion_NNsh(void);
UInt16 KeyConvertFiveWayToJogChara(EventType *event);

///// �C�x���g�n���h��
Boolean Handler_EditAction(EventType *event);
Boolean Handler_MainForm  (EventType *event);

/////  BUSY�E�B���h�E
void SetMsg_BusyForm(Char *msg);
void Show_BusyForm  (Char *msg, NNshWorkingInfo *NNshGlobal);
void Hide_BusyForm  (NNshWorkingInfo *NNshGlobal);

///// ��ʊ֘A
void NNshWinSetPopItems (FormType *frm, UInt16 popId, UInt16 lstId, UInt16 item);
void NNshWinSetFieldText(FormType *frm, UInt16 fldID, Char *msg, UInt32 size);
void NNshWinGetFieldText(FormType *frm, UInt16 fldID, Char *area, UInt32 len);
Boolean InputDialog(Char *title, Char *area, UInt16 size);

/////
void NNsh_check302URL  (NNshWorkingInfo *NNshGlobal);
void NNsh_MakeHTTPmsg  (NNshWorkingInfo *NNshGlobal);
void NNshHTTPS_comm    (NNshWorkingInfo *NNshGlobal);
void NNshHTTP_comm     (NNshWorkingInfo *NNshGlobal);
void pickup_cookie     (NNshWorkingInfo *NNshGlobal);
/////
