/*============================================================================*
 *  $Id: symbols.h,v 1.2 2004/09/11 05:40:45 mrsa Exp $
 *
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for Palm application.
 *
 *===========================================================================*/

/***  データベース設定  **/
#define SOFT_CREATOR_ID          'mlDA'
#define SOFT_DBTYPE_ID           'Data'
#define SOFT_DB_ID               1
#define DBNAME_SETTING           "mlDA-DB"
#define DBVERSION_SETTING           100
#define DBSIZE_SETTING               32
#define DB_RESERVE_AREA             256

/** メッセージヘッダの取得数  **/
#define GET_MESSAGE_HEADER_NUM        3

/***  バッファサイズ ***/
#define DIALOGTITLE_LENGTH           40
#define MINIBUF                      48
#define BUFSIZE                     128
#define VALUESIZE                  3072
#define MARGIN                        4
#define BUSYWIN_MAXLEN               32
#define DIALOG_BUFFER              1024
#define HEADER_BUFFER              4096
#define SENDBUFFER_DEFAULT         4096
#define RECEIVEBUFFER_DEFAULT     32700
#define WORKBUFFER_DEFAULT        32700

/** データベースのキータイプ(数字か文字列か) **/
#define NNSH_KEYTYPE_CHAR             0
#define NNSH_KEYTYPE_UINT32           1
#define NNSH_KEYTYPE_UINT16           2

/** 通信ISPの指定 **/
#define CHECKPROTOCOL_MOBILEPOINT     0


/** タイムアウト時間の設定 **/
#define NNSH_GETMAIL_TIMEOUT         15

/** SSLモードでないときの設定 **/
#define NNSH_SSLMODE_NOTSSL          10
#define NNSH_SSLMODE_DOSSL           11

/** エラーコード **/
#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)


/*** メッセージ ***/
#define MSG_ERROR         "エラーが発生しました。\n   (設定を確認してください。)"
#define MSG_SUCCESSLOGIN  "ログインに成功しました。"
#define MSG_DISCONNECT    "回線切断中..."

/***  LOGIN METHOD  ***/
#define NNSHLOGIN_POST_METHOD          0x0001
#define NNSHLOGIN_CONNECT_PORT         0x0002
#define NNSHLOGIN_SSL_PROTOCOL         0x0010
#define NNSILOGIN_GET_METHOD_HTTP      0x0000
#define NNSILOGIN_GET_METHOD_SSL       (NNSHLOGIN_SSL_PROTOCOL)
#define NNSILOGIN_POST_METHOD_SSL      ((NNSHLOGIN_SSL_PROTOCOL)|(NNSHLOGIN_POST_METHOD))
#define NNSILOGIN_POST_METHOD_HTTP     (NNSHLOGIN_POST_METHOD)
#define NNSILOGIN_POST_METHOD_OTHER    ((NNSHLOGIN_POST_METHOD)|(NNSHLOGIN_CONNECT_PORT))
#define NNSILOGIN_GET_METHOD_OTHER     (NNSHLOGIN_CONNECT_PORT)
#define NNSILOGIN_POST_METHOD_OTHERSSL ((NNSHLOGIN_POST_METHOD)|(NNSHLOGIN_CONNECT_PORT)|(NNSHLOGIN_SSL_PROTOCOL))
#define NNSILOGIN_GET_METHOD_OTHERSSL  ((NNSHLOGIN_SSL_PROTOCOL)|(NNSHLOGIN_CONNECT_PORT))

/**********  メッセージパース用  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/********** 漢字コード **********/
#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01
#define NNSH_KANJICODE_JIS          0x02
#define NNSH_KANJICODE_UTF8         0x03
#define nnDA_KANJICODE_RAW          0xff
#define ejDA_KANJICODE_DATAOUTPUT   0x1f

/**  from SonyChars.h  **/
#ifndef vchrJogPushedDown
  #define vchrJogPushedDown     (0x1704)
#endif
#ifndef vchrJogPushedUp
  #define vchrJogPushedUp       (0x1703)
#endif
#ifndef vchrJogUp
  #define vchrJogUp             (0x1700)
#endif
#ifndef vchrJogDown
  #define vchrJogDown           (0x1701)
#endif
#ifndef vchrJogPush
  #define vchrJogPush           (0x1705)
#endif
#ifndef vchrJogRelease
  #define vchrJogRelease        (0x1706)
#endif
#ifndef vchrJogBack
  #define vchrJogBack           (0x1707)
#endif

#ifndef vchrJogLeft
  #define vchrJogLeft           (0x1708)
#endif

#ifndef vchrJogRight
  #define vchrJogRight          (0x1709)
#endif


#if 0
/*============================================================================*
 *  $Id: symbols.h,v 1.2 2004/09/11 05:40:45 mrsa Exp $
 *
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for Palm application.
 *
 *===========================================================================*/

/***  データベース設定  **/
#define SOFT_CREATOR_ID          'mlDA'
#define SOFT_DBTYPE_ID           'Data'
#define SOFT_DB_ID               1
#define DBNAME_SETTING           "mlDA-DB"
#define DBVERSION_SETTING           100
#define DBSIZE_SETTING               32
#define DB_RESERVE_AREA             256

/** メッセージヘッダの取得数  **/
#define GET_MESSAGE_HEADER_NUM        3

/***  バッファサイズ ***/
#define MINIBUF                      48
#define BUFSIZE                     128
#define MARGIN                        4
#define WORKBUF_DEFAULT           30000
#define BUSYWIN_MAXLEN               32
#define DIALOG_BUFFER              1024

#define NOFISP                        7   // サポート(記憶可能)ISP数

/** データベースのキータイプ(数字か文字列か) **/
#define NNSH_KEYTYPE_CHAR             0
#define NNSH_KEYTYPE_UINT32           1
#define NNSH_KEYTYPE_UINT16           2

/** 通信ISPの指定 **/
#define CHECKPROTOCOL_MOBILEPOINT     0


/** タイムアウト時間の設定 **/
#define NNSH_GETMAIL_TIMEOUT         15

/** SSLモードでないときの設定 **/
#define NNSH_SSLMODE_NOTSSL          10
#define NNSH_SSLMODE_DOSSL           11

/** エラーコード **/
#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)


/*** メッセージ ***/
#define MSG_ERROR         "エラーが発生しました。\n   (設定を確認してください。)"
#define MSG_SUCCESSLOGIN  "ログインに成功しました。"
#define MSG_DISCONNECT    "回線切断中..."

/***  LOGIN METHOD  ***/
#define NNSHLOGIN_POST_METHOD          0x0001
#define NNSHLOGIN_CONNECT_PORT         0x0002
#define NNSHLOGIN_SSL_PROTOCOL         0x0010
#define NNSILOGIN_GET_METHOD_HTTP      0x0000
#define NNSILOGIN_GET_METHOD_SSL       (NNSHLOGIN_SSL_PROTOCOL)
#define NNSILOGIN_POST_METHOD_SSL      ((NNSHLOGIN_SSL_PROTOCOL)|(NNSHLOGIN_POST_METHOD))
#define NNSILOGIN_POST_METHOD_HTTP     (NNSHLOGIN_POST_METHOD)
#define NNSILOGIN_POST_METHOD_OTHER    ((NNSHLOGIN_POST_METHOD)|(NNSHLOGIN_CONNECT_PORT))
#define NNSILOGIN_GET_METHOD_OTHER     (NNSHLOGIN_CONNECT_PORT)
#define NNSILOGIN_POST_METHOD_OTHERSSL ((NNSHLOGIN_POST_METHOD)|(NNSHLOGIN_CONNECT_PORT)|(NNSHLOGIN_SSL_PROTOCOL))
#define NNSILOGIN_GET_METHOD_OTHERSSL  ((NNSHLOGIN_SSL_PROTOCOL)|(NNSHLOGIN_CONNECT_PORT))

/**********  メッセージパース用  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/********** 漢字コード **********/
#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01
#define NNSH_KANJICODE_JIS          0x02
#define NNSH_KANJICODE_UTF8         0x03
#define nnDA_KANJICODE_RAW          0xff

/**  from SonyChars.h  **/
#ifndef vchrJogPushedDown
  #define vchrJogPushedDown     (0x1704)
#endif
#ifndef vchrJogPushedUp
  #define vchrJogPushedUp       (0x1703)
#endif
#ifndef vchrJogUp
  #define vchrJogUp             (0x1700)
#endif
#ifndef vchrJogDown
  #define vchrJogDown           (0x1701)
#endif
#ifndef vchrJogPush
  #define vchrJogPush           (0x1705)
#endif
#ifndef vchrJogRelease
  #define vchrJogRelease        (0x1706)
#endif
#ifndef vchrJogBack
  #define vchrJogBack           (0x1707)
#endif

#ifndef vchrJogLeft
  #define vchrJogLeft           (0x1708)
#endif

#ifndef vchrJogRight
  #define vchrJogRight          (0x1709)
#endif

#endif // #if 0

