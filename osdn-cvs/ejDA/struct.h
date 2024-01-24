/*============================================================================*
 *  $Id: struct.h,v 1.2 2004/09/13 12:10:28 mrsa Exp $
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

//  全体で使用するグローバル領域
typedef struct
{
    FormType     *currentForm;
    FormType     *previousForm;
    FormType     *prevBusyForm;
    UInt16        netRef;
    UInt16        err;
    UInt16        ret;
    UInt16        loopCount;
    NetSocketRef  socketRef;
    UInt16        sslRef;
    SslLib       *sslTemplate;
    SslContext   *sslContext;
    Char         *topPtr;
    Char         *tempPtr;
    Char         *tempPtr2;
    Char         *tempPtr3;
    Char         *sendBuf;
    Char         *recvBuf;
    Char         *workBuf;
    UInt16        sendMethod;
    UInt32        palmOSVersion;
    Char          reqURL    [BUFSIZE * 2];
    Char          reqAddress[BUFSIZE * 2];
#ifdef USE_REFERER
    Char          referer   [BUFSIZE];
#endif
    Char          tempBuf   [BUFSIZE];
    Char          tempBuf2  [BUFSIZE];
    Char          showBuf   [MINIBUF + MARGIN];
    Char          cookie    [BUFSIZE + MARGIN];
    Char          hostName  [BUFSIZE];       // ホスト名
    UInt16        portNum;                   // 通信ポート番号
    Int32         timeout;                   // 通信タイムアウト値
    UInt16        isChecking;                // 要求アドレス
    UInt16        kanjiCode;                 // 表示漢字コード
    UInt16        fieldLen;                  // 表示データサイズ
    MemHandle     memH;
    // UInt32     bufferSize;
#ifdef USE_WIDEWINDOW
    Coord         x;
    Coord         y;
    Coord         diffX;
    Coord         diffY;
    RectangleType r;
    UInt32        silkVer;
#endif
} NNshWorkingInfo;
