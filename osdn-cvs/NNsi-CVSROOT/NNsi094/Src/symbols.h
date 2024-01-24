/*============================================================================*
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for NNsh.
 *
 *===========================================================================*/

//////////////////////////////////////////////////////////////////////////////
/***  DATABASE VERSIONS  ***/
#define SOFT_PREFID           100
#define SOFT_PREFVERSION      203

#define NNSH_PREFERID         200
#define NNSH_PREFERTYPE       105

#define SOFT_DBTYPE_ID        'Data'
#define OFFLINE_DBTYPE_ID     'GIKO'

#define DBNAME_BBSLIST        "BBS-NNsi"
#define DBVERSION_BBSLIST     160

#define DBNAME_SUBJECT        "Subject-NNsi"
#define DBVERSION_SUBJECT     160

#define DBNAME_SETTING        "Settings-NNsi"
#define DBVERSION_SETTING     101
#define DBSIZE_SETTING        32

#define DBNAME_DIRINDEX       "DirIndex-NNsi"
#define DBVERSION_DIRINDEX    100

//////////////////////////////////////////////////////////////////////////////

#define RSCID_createThreadIndexSub_ARM     1     // ARMlet関数番号その１
#define RSCID_parseMessage_ARM             2     // ARMlet関数番号その２

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12

// JUMPBUFの値を変更した場合には、ARMlet内の同じシンボルを変更すること
#define JUMPBUF                     12  // 注意：ARMletに同じシンボル定義あり

/** 一時確保領域の幅(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4

/** BBS-DBの文字列サイズ **/
#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16
#define LIMIT_BOARDNAME             20

/** Thread-DB の文字列サイズ **/
#define MAX_THREADFILENAME          32
#define MAX_THREADNAME              72
#define MAX_DIRNAME                 40
#define LIMIT_TITLENAME_DEFAULT     36
#define LIMIT_TITLENAME_MINIMUM     12

/** タイトルおよびラベルの領域サイズ **/
#define BBSSEL_PAGEINFO             12
#define NNSH_MSGTITLE_BUFSIZE       10

/** VFSファイル名の最大サイズ  **/
#define MAXLENGTH_FILENAME          256

/** 分割ダウンロード時に想定するHTTPヘッダサイズ **/
#define NNSH_HTTPGET_HEADERMAX      400
#define NNSH_DOWNLOADSIZE_MIN      2048

/** 通信タイムアウト初期値(tick) **/
#define NNSH_NET_TIMEOUT           2500

/** メッセージの最大レス番号 **/
#define NNSH_MESSAGE_MAXNUMBER     1005
#define NNSH_MESSAGE_LIMIT         1000

/** BBS URL **/
#define URL_BBSTABLE_OLD            "http://www6.ocn.ne.jp/~mirv/bbstable.html"
#define URL_BBSTABLE                "http://www.ff.iij4u.or.jp/~ch2/bbstable.html"
#define URL_PREFIX                  "test/read.cgi/"
#define URL_PREFIX_MACHIBBS         "bbs/read.pl?BBS="
#define FILE_THREADLIST             "subject.txt"

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont

#define NNSH_NOTENTRY_THREAD        0xffff

/** OFFLINE LOG(for GIKO Shippo) **/
#define OFFLINE_THREAD_NAME         "参照専用ログ"
#define OFFLINE_THREAD_NICK         "!GikoShippo/"
#define OFFLINE_THREAD_URL          "file://"

#define FAVORITE_THREAD_NAME        "お気に入り"
#define GETALL_THREAD_NAME          "取得済み全て"
#define NOTREAD_THREAD_NAME         "未読あり"

#define NNSH_NOF_SPECIAL_BBS        4   // 特殊な板名の数
#define NNSH_SELBBS_FAVORITE        0   // お気に入りスレ
#define NNSH_SELBBS_GETALL          1   // 取得済み全て
#define NNSH_SELBBS_NOTREAD         2   // 未読あり
#define NNSH_SELBBS_OFFLINE         3   // OFFLINEスレ

// for NaNaShi FILE NAMEs
#define FILE_WRITEMSG               "write.txt"
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define LOGDATAFILE_PREFIX          "/PALM/Programs/2ch/"
#define LOGDIR_FILE                 "/PALM/Programs/2ch/logdir.txt"
#define DATAFILE_SUFFIX             ".dat"

//  time diference between from 'Jan/01/1904' to 'Jan/01/1970'.
#define TIME_CONVERT_1904_1970      (2082844800 - 32400)    // JST
#define TIME_CONVERT_1904_1970_UTC  (2082844800)            // UTC

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002
#define NNSH_VFS_USEOFFLINE         0x0004
#define NNSH_VFS_DBBACKUP           0x0008

#define NNSH_FILEMODE_CREATE        1
#define NNSH_FILEMODE_READWRITE     2
#define NNSH_FILEMODE_READONLY      4
#define NNSH_FILEMODE_TEMPFILE      8
#define NNSH_FILEMODE_APPEND        16

#define FILEMGR_STATE_DISABLED      0
#define FILEMGR_STATE_OPENED_VFS    1
#define FILEMGR_STATE_OPENED_STREAM 2

/** BBS-DBの表示状態(選択画面で表示するか/しないか) **/
#define NNSH_BBSSTATE_VISIBLE       1            // 表示
#define NNSH_BBSSTATE_INVISIBLE     0            // 非表示

/** BBS-DBのボードタイプ **/
#define NNSH_BBSTYPE_2ch            0x00         // 2ch(通常モード)
#define NNSH_BBSTYPE_MACHIBBS       0x01         // まちBBS(特殊モード)
#define NNSH_BBSTYPE_ERROR          0xff         // ボードタイプエラー

/** 一覧表示がBBS一覧かお気に入りか **/
#define NNSH_FAVORLIST_MODE         1
#define NNSH_BBSLIST_MODE           0 


// データベースのキータイプ(数字か文字列か)
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2

// HTTPメッセージ作成時に指定
#define HTTP_SENDTYPE_GET           10
#define HTTP_SENDTYPE_POST          20
#define HTTP_SENDTYPE_POST_MACHIBBS 40
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETSUBJECT_START       0
#define HTTP_GETSUBJECT_PART        0

// 検索方向
#define NNSH_SEARCH_FORWARD         1
#define NNSH_SEARCH_BACKWARD       -1

// 書き込みメッセージ確認コマンド
#define NNSH_WRITECHK_SPID          0
#define NNSH_WRITECHK_REPLY         1

// メッセージの属性
#define NNSH_MSGATTR_NOTSPECIFY    0xff   // 「お気に入り」指定は指定しない
#define NNSH_MSGATTR_NOTFAVOR      0x7f   // 「お気に入り」スレでない(MASK値)
#define NNSH_MSGATTR_FAVOR         0x80   // 「お気に入り」スレである
#define NNSH_MSGATTR_NOTERROR      0xbf   // 「エラー発生」スレでない(MASK値)
#define NNSH_MSGATTR_ERROR         0x40   // 「エラー発生」スレである


// 一時状態(現在、ジョグ押し回し状態を記憶する)
#define NNSH_TEMPTYPE_PUSHEDJOGUP   16
#define NNSH_TEMPTYPE_PUSHEDJOGDOWN  8
#define NNSH_TEMPTYPE_CLEARSTATE     0

//  その他の状態値
#define NNSH_NOT_EFFECTIVE          0
#define NNSH_UPDATE_DISPLAY         1
#define NNSH_SILK_CONTROL           1

#define NNSH_DISABLE                0
#define NNSH_ENABLE                 1

// NNsiのデータベース更新情報
#define NNSH_DB_UPDATE_BBS          0x0001
#define NNSH_DB_UPDATE_SUBJECT      0x0002
#define NNSH_DB_UPDATE_SETTING      0x0004
#define NNSH_DB_UPDATE_DIRINDEX     0x0008

#define NNSH_DB_MASK_BBS            0xfffe
#define NNSH_DB_MASK_SUBJECT        0xfffd
#define NNSH_DB_MASK_SETTING        0xfffb
#define NNSH_DB_MASK_DIRINDEX       0xfff7

//////////////////////////////////////////////////////////////////////////////

/********** HTML状態解析用 ********/
#define HTTP_ANCHORSTATE_NORMAL     101
#define HTTP_ANCHORSTATE_ANCHOR     102
#define HTTP_ANCHORSTATE_LABEL      103

/**********  スレ読み出し状態用  *********/
#define NNSH_SUBJSTATUS_NOT_YET      0
#define NNSH_SUBJSTATUS_NEW          1
#define NNSH_SUBJSTATUS_UPDATE       2
#define NNSH_SUBJSTATUS_REMAIN       3
#define NNSH_SUBJSTATUS_ALREADY      4
#define NNSH_SUBJSTATUS_DELETE       5
#define NNSH_SUBJSTATUS_UNKNOWN      6
#define NNSH_SUBJSTATUS_GETERROR     7

#define NNSH_MSGSTATE_NOTMODIFIED   10
#define NNSH_MSGSTATE_ERROR         20

/**********  メッセージパース用  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/**********  BBS所有スレ数  **************/
#define NNSH_BBSLIST_AGAIN         -1            // スレ数を再度確認する

///////////////////////////////////////////////////////////////////////////////

/*** DATA TOKENS ***/
#define DATA_SEPARATOR              "<>"
#define DATA_NEWLINE                "<br>"

/** for BBS NAME PARSER **/
#define HTTP_TOKEN_ST_ANCHOR        "<A"
#define HTTP_TOKEN_ED_ANCHOR        "</A>"
#define HTTP_TOKEN_END              ">"

#define NNSH_FIELD_SEPARATOR        "<>"
#define NNSH_FIELD_SEPARATOR_MACHI  ","
#define NNSH_RECORD_SEPARATOR       "\x0a"
#define NNSH_SUBJTITLE_END          "("

#define NNSH_MACHITOKEN_START       "\x0d\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a\x0d\x0a"
#define NNSH_MACHITOKEN_MSGSTART    "名前："
#define NNSH_MACHINAME_SUFFIX       "[まちBBS]"

/** **/
#define NNSH_SYMBOL_WRITE_DONE      "書きこみました。"

#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1

#define NNSH_JUMPMSG_LEN            10
#define NNSH_JUMPMSG_HEAD           ">>"
#define NNSH_JUMPMSG_BACK           "１つ戻る"
#define NNSH_JUMPMSG_TO_LIST        "一覧へ"
#define NNSH_JUMPSEL_TO_LIST         0
#define NNSH_JUMPSEL_BACK_USELIST    1
#define NNSH_JUMPSEL_BACK            0

#define NNSH_JUMPMSG_OPENBBS        "板を選択"
#define NNSH_JUMPMSG_OPENMENU       "メニュー"
#define NNSH_JUMPMSG_OPENNEW        "新着確認"
#define NNSH_JUMPMSG_OPENLIST       "一覧取得"
#define NNSH_JUMPMSG_OPENGET        "スレ取得"
#define NNSH_JUMPMSG_OPENINFO       "スレ情報"
#define NNSH_JUMPSEL_NUMLIST        6  // リストアイテム数
#define NNSH_JUMPSEL_OPENBBS        0  // 板を選択
#define NNSH_JUMPSEL_OPENMENU       1  // メニュー
#define NNSH_JUMPSEL_OPENNEW        2  // 新着確認
#define NNSH_JUMPSEL_OPENLIST       3  // 一覧取得
#define NNSH_JUMPSEL_OPENGET        4  // スレ取得
#define NNSH_JUMPSEL_OPENINFO       5  // スレ情報

