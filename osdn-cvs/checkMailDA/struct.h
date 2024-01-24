/*============================================================================*
 *  $Id: struct.h,v 1.3 2004/05/08 15:31:16 mrsa Exp $
 *
 *  FILE: 
 *     struct.h
 *
 *  Description: 
 *     structure definitions for Palm application.
 *
 *===========================================================================*/

/********** My Local structures **********/

// DB の情報取得用
typedef struct {
    Char    *nameP;
    UInt16  *attributesP;
    UInt16  *versionP;
    UInt32  *crDateP;
    UInt32  *modDateP;
    UInt32  *bckUpDateP;
    UInt32  *modNumP;
    LocalID *appInfoIDP;
    LocalID *sortInfoIDP;
    UInt32  *typeP;
    UInt32  *creatorP;
} NNshDBInfo;

// NNsiのリストアイテムID
typedef struct {
  UInt16  popId;
  UInt16  lstId;
} NNshListItem;

// バージョン情報(システムPreferenceに記録する構造体)
typedef struct {
    UInt32  NNsiVersion;
    UInt32  prefSize;
    UInt16  prefType;
    UInt16  debugMode;
} NNshSoftwareVersionInfo;

typedef struct {
    UInt16 up;              // ハードキー上
    UInt16 down;            // ハードキー下
    UInt16 key1;            // ハードキー１
    UInt16 key2;            // ハードキー２
    UInt16 key3;            // ハードキー３
    UInt16 key4;            // ハードキー４
} NNshHardkeyControl;

// システム情報(起動時にデータベースから読み込む情報)
typedef struct {
    /** NNsi設定 **/
    UInt16  debugMessageON;            // デバッグメッセージの表示
    UInt8   getMessageHeader;          // メッセージヘッダを取得
    UInt8   confirmationDisable;       // 確認ダイアログを省略する
    UInt16  disconnect;                // 終了時に回線切断
    UInt16  protocol;                  // 通信プロトコル
    UInt16  portNum;                   // 通信ポート番号
    Int32   timeout;                   // 通信タイムアウト値
    Char    hostName[BUFSIZE];         // ホスト名
    Char    userName[BUFSIZE];         // ユーザ名
    Char    password[BUFSIZE];         // パスワード
    UInt16  useHardKeyControl;         // ハードキー制御を実施する
    UInt16  bufferSize;                // バッファサイズ
    NNshHardkeyControl useKey;         // ハードキーのキー番号格納領域
    UInt32  reserved[DB_RESERVE_AREA]; // 予備領域
} NNshSavedPref;

//  全体で使用するグローバル領域
typedef struct
{
    FormType     *currentForm;
    FormType     *previousForm;
    FormType     *prevBusyForm;
    // FormActiveStateType formState;
    ControlType  *selP;
    UInt16        btnId;
    UInt16        netRef;
    UInt16        err;
    UInt16        ret;
    UInt16        size;
    NetSocketRef  socketRef;
    Char          tempBuf[BUFSIZE * 2];
    Char         *tempPtr2;
    Char         *tempPtr;
    Char         *sendBuf;
    Char         *recvBuf;
    UInt8         PADDING[64];
    MD5_CTX       context;
    UInt8         digest[20];
#ifdef USE_OSVERSION
    UInt32        palmOSVersion;
#endif
} NNshWorkingInfo;
