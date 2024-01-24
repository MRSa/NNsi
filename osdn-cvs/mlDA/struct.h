/*============================================================================*
 *  $Id: struct.h,v 1.9 2008/03/08 20:03:26 mrsa Exp $
 *
 *  FILE: 
 *     struct.h
 *
 *  Description: 
 *     structure definitions for Palm application.
 *
 *===========================================================================*/

/********** My Local structures **********/

// UnicodeToJis0208 テーブルレコード
typedef struct
{
    UInt16 size;
    UInt16 table[256];
} UnicodeToJis0208Rec;

// Jis0208ToUnicode テーブルレコード
typedef struct
{
    UInt16 code;
} Jis0208ToUnicodeRec;

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
    Char    userName[NOFISP][BUFSIZE0]; // ユーザ名
    Char    password[NOFISP][BUFSIZE0]; // パスワード
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
    ControlType  *selP;
    UInt16        btnId;
    UInt16        netRef;
    UInt16        err;
    UInt16        ret;
    UInt16        loopCount;
    NetSocketRef  socketRef;
    UInt16        sslRef;
    SslLib       *sslTemplate;
    SslContext   *sslContext;
    Char         *tempPtr2;
    Char         *tempPtr;
    Char         *sendBuf;
    Char         *recvBuf;
    Char         *workBuf;
    UInt16        isChecking;
    UInt16        isRedirect;
    UInt16        setPass;
    UInt16        sendMethod;
    UInt32        palmOSVersion;
    Char          tempBuf   [BUFSIZE];
    Char          tempBuf2  [BUFSIZE];
    Char          reqAddress[BUFSIZE * 2];
#ifdef USE_REFERER
    Char          referer   [BUFSIZE * 3];
#endif
    Char          cookie    [BUFSIZE + MARGIN];
} NNshWorkingInfo;

