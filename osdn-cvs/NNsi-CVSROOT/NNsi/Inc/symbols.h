/*============================================================================*
 *
 * $Id: symbols.h,v 1.236 2012/01/10 16:27:42 mrsa Exp $
 *
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
#define SOFT_PREFVERSION      204

#define NNSH_PREFERID         200
#define NNSH_PREFERTYPE       105

#define SOFT_DBTYPE_ID        'Data'
#define OFFLINE_DBTYPE_ID     'GIKO'

#define DBNAME_BBSLIST        "BBS-NNsi"
#define DBVERSION_BBSLIST     160

#define DBNAME_SUBJECT        "Subject-NNsi"
#define DBVERSION_SUBJECT     160

#define DBNAME_SETTING        "Settings-NNsi"
#define DBVERSION_SETTING     121
#define DBSIZE_SETTING        32
#define DBVERSION_XML_SETTING 100

#define DBNAME_DIRINDEX       "DirIndex-NNsi"
#define DBVERSION_DIRINDEX    100

#define DBNAME_GETRESERVE     "GetReserve-NNsi"
#define DBVERSION_GETRESERVE  100

#define DBNAME_PERMANENTLOG    "LogPermanent-NNsi"
#define DBVERSION_PERMANENTLOG 100

#define DBNAME_TEMPORARYLOG    "LogTemporary-NNsi"
#define DBVERSION_TEMPORARYLOG 100

#define DBNAME_TEMPURL         "TempURL-NNsi"
#define DBVERSION_TEMPURL      100

#define DBNAME_NGWORD          "NGWord-NNsi"
#define DBVERSION_NGWORD       100

#define DBNAME_MACROSCRIPT     "Macro-NNsi"
#define DBVERSION_MACROSCRIPT  100

#define DBNAME_RUNONCEMACRO    "RunOnce-NNsi"
#define DBNAME_MACRORESULT     "MacroResult-NNsi"
#define DBVERSION_MACRO        100

#define DBNAME_LOGTOKEN        "LogToken-NNsi"
#define DBVERSION_LOGTOKEN     100

#define NNSH_CREATORID_PREPARE_NEWARRIVAL 'mlDA'

#define DBSOFT_CREATOR_ID      'NNsj'
#define DATA_DBTYPE_ID         'Mcro'

//////////////////////////////////////////////////////////////////////////////

#define RSCID_createThreadIndexSub_ARM     1     // ARMlet関数番号その１
#define RSCID_parseMessage_ARM             2     // ARMlet関数番号その２

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define TINYBUF                      8
#define MINIBUF                     48
#define BUFSIZE                    128
#define HIDEBUFSIZE                128
#define PASSSIZE                     8
#define BIGBUF                     640
#define PREVBUF                     12
#define JUMPBUF                     16
#define NGWORD_LEN                  40
#define OYSTERID_LEN                64
#define OYSTER_LEN                  64
#define BE2chID_LEN                 64
#define BE2chPW_LEN                 32
#define SEARCH_HEADLEN              16
#define MAX_GETLOG_RECORDS        2048
#define MAX_MACRO_DBFILES          260

#define MAX_TIMEOUT_VALUE          999

#define MAX_SELECTLOGRECORD         80
#define MAX_LOGRECLEN               40
#define MAX_STRLEN                  32
#define MAX_DBNAMELEN               32

/** 一時確保領域の幅(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4
#define FREE_SPACE_MARGIN         2048

/** 書き込み時に確保するバッファサイズ **/
#define NNSH_WRITEBUF_MIN         4096
#define NNSH_WRITEBUF_MAX        16384
#define NNSH_WRITEBUF_DEFAULT     8192

#define NNSH_WRITECOOKIE_BUFFER   2560

#define NNSH_WRITELBL_MAX           26
#define NNSH_WRITELBL_HEAD        "書"

#define NNSH_NEWTHREAD_MAX          32
#define NNSH_NEWTHREAD_LBLHEAD    "スレ立て:"

/** BBS-DBの文字列サイズ **/
#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16
#define LIMIT_BOARDNAME             20

/** Thread-DB の文字列サイズ **/
#define MAX_THREADFILENAME          32
#define MAX_THREADNAME              72
#define MAX_DIRNAME                 40
#define LIMIT_TITLENAME_DEFAULT     72
#define LIMIT_TITLENAME_MINIMUM     12
#define LIMIT_TITLE_HEADER_SIZE    (MAX_THREADNAME - MARGIN)
#define MAX_DEPTH                   10
#define MAX_LISTLENGTH              24

/** GetData-DBの文字列サイズ **/
#define MAX_DATAPREFIX              32
#define MAX_DATATOKEN               80
#define MAX_TITLE_LABEL             32
#define MAX_GETLOG_URL             244
#define MAX_GETLOG_DATETIME         38
#define MAX_URL_CNT                 66
#define MAX_URLLIST_CNT             80
#define MAX_GETLOG_REGETHOUR      0xff

/** URL一覧の表示サイズ **/
#define MAX_NAMELABEL               48
#define MAX_URL_LIST                24

/** タイトルおよびラベルの領域サイズ **/
#define BBSSEL_PAGEINFO             12
#define NNSH_MSGTITLE_BUFSIZE       10
#define SEPARATOR_LEN               24

/** VFSファイル名の最大サイズ  **/
#define MAXLENGTH_FILENAME          256

/** 分割ダウンロード時に想定するHTTPヘッダサイズ **/
#define NNSH_HTTPGET_HEADERMAX      400
#define NNSH_DOWNLOADSIZE_MIN      2048

/** 通信タイムアウト初期値(tick) **/
#define NNSH_NET_TIMEOUT           2500

/** メッセージの最大レス番号 **/
#define NNSH_MESSAGE_MAXNUMBER     1005
#define NNSH_MESSAGE_MAXLIMIT      25000
#define NNSH_MESSAGE_LIMIT         1000

/** BBS URL **/
#define URL_BBSTABLE             "http://menu.2ch.net/bbstable.html"
#define URL_BBSTABLE_OLD         "http://www.ff.iij4u.or.jp/~ch2/bbstable.html"

#define URL_PREFIX                     "test/read.cgi/"
#define URL_PREFIX_IMODE               "test/r.i/"
#define URL_PREFIX_MOBILE              "http://c.2ch.net/test/-/"
//#define URL_PREFIX_MACHIBBS            "bbs/read.pl?BBS="
//#define URL_PREFIX_MACHIBBS_IMODE      "bbs/read.pl?IMODE=TRUE&BBS="
#define URL_PREFIX_MACHIBBS            "bbs/read.cgi?BBS="
#define URL_PREFIX_MACHIBBS_IMODE      "bbs/read.cgi?IMODE=TRUE&BBS="
#define URL_PREFIX_SHITARABA           "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABA_IMODE     "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABAJBBS       "bbs/read.cgi?BBS="
#define URL_PREFIX_SHITARABAJBBS_IMODE "bbs/i.cgi?BBS="
#define FILE_THREADLIST                "subject.txt"

#define URL_PROTOCOL_USESSL            "https://"

#define URL_BE2ch_PROFILE              "http://be.2ch.net/test/p.php"
#define URL_PREFIX_BE2ch_PROFILE       "?i="

#define MAX_REDIRECT_COUNT          3

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont
#define NNSH_DEFAULT_FONT_HR        stdFont

#define NNSH_NOTENTRY_THREAD        0xffff
#define NNSH_NOTENTRY_BBS           0xffff
#define NNSH_DATABASE_BLANK         0xffff
#define NNSH_DATABASE_UNKNOWN       0xfffe
#define NNSH_DATABASE_PENDING       0xfffd

/** OFFLINE LOG(for GIKO Shippo) **/
#define OFFLINE_THREAD_NAME         "参照ログ"
#define OFFLINE_THREAD_NICK         "!GikoShippo/"
#define OFFLINE_THREAD_URL          "file://"

#define FAVORITE_THREAD_NAME        "お気に入り"
#define GETALL_THREAD_NAME          "取得済み全て"
#define NOTREAD_THREAD_NAME         "未読あり"
#define CUSTOM1_THREAD_NAME         "取得エラー"
#define CUSTOM2_THREAD_NAME         "Palm関連"
#define CUSTOM3_THREAD_NAME         "新規(3日以内)"
#define CUSTOM4_THREAD_NAME         "最大超"
#define CUSTOM5_THREAD_NAME         "取得保留中"

#define CUSTOM1_THREAD_HEAD         "1>"
#define CUSTOM2_THREAD_HEAD         "2>"
#define CUSTOM3_THREAD_HEAD         "3>"
#define CUSTOM4_THREAD_HEAD         "4>"
#define CUSTOM5_THREAD_HEAD         "5>"


#define NNSH_NOF_SPECIAL_BBS        9   // 特殊な板名の数
#define NNSH_SELBBS_NOTREAD         0   // 未読あり
#define NNSH_SELBBS_GETALL          1   // 取得済み全て
#define NNSH_SELBBS_FAVORITE        2   // お気に入りスレ
#define NNSH_SELBBS_CUSTOM1         3   // CUSTOM1
#define NNSH_SELBBS_CUSTOM2         4   // CUSTOM2
#define NNSH_SELBBS_CUSTOM3         5   // CUSTOM3
#define NNSH_SELBBS_CUSTOM4         6   // CUSTOM4
#define NNSH_SELBBS_CUSTOM5         7   // CUSTOM5
#define NNSH_SELBBS_OFFLINE         8   // OFFLINEスレ

// for NaNaShi FILE NAMEs
#define FILE_WRITEMSG               "write.txt"
#define FILE_WRITENAME              "writeName.txt"
#define FILE_WRITEMAIL              "writeMail.txt"
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define FILE_XML_NNSISET            "NNsiSet.xml"
#define FILE_LOGCHARGE_PREFIX       "NNsiLog"
#define FILE_LOGCHARGE_SUFFIX       ".htm"
#define FILE_TRIMLOG_PREFIX         "TrimLog"
#define FILE_TRIMLOG_SUFFIX         ".htm"

#define NNSH_HTTP_CONTENT_LENGTH    "Content-Length:"
#define NNSH_HTTP_CONTENT_GZIPED    "Content-Encoding: gzip"     // by Nickle
#define NNSH_HTTP_CONTENT_XGZIPED   "Content-Encoding: x-gzip"

#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define LOGDATAFILE_PREFIX          "/PALM/Programs/2ch/"
#define LOGDIR_FILE                 "/PALM/Programs/2ch/logdir.txt"
#define DATAFILE_SUFFIX             ".dat"
#define CGIFILE_SUFFIX              ".cgi"

//  time diference between from 'Jan/01/1904' to 'Jan/01/1970'.
#define TIME_CONVERT_1904_1970      (2082844800 - 32400)
#define TIME_CONVERT_1904_1970_UTC  (2082844800)
#define TIME_24HOUR_SECOND          ((Int32) 24 * ((Int32) 60 * (Int32) 60))

#define TIME_CONVERT_1970_2005      (1104505200)

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002
#define NNSH_VFS_USEOFFLINE         0x0004
#define NNSH_VFS_DBBACKUP           0x0008
#define NNSH_VFS_DBIMPORT           0x0010
#define NNSH_VFS_DIROFFLINE         0x0020
#define NNSH_VFS_USE_CF             0x0040

#define NNSH_COPY_PALM_TO_VFS       0
#define NNSH_COPY_VFS_TO_VFS        1
#define NNSH_COPY_VFS_TO_PALM       2
#define NNSH_COPY_PALM_TO_PALM      4

#define NNSH_FILEMODE_CREATE        1
#define NNSH_FILEMODE_READWRITE     2
#define NNSH_FILEMODE_READONLY      4
#define NNSH_FILEMODE_TEMPFILE      8
#define NNSH_FILEMODE_APPEND        16

#define FILEMGR_STATE_DISABLED      0
#define FILEMGR_STATE_OPENED_VFS    1
#define FILEMGR_STATE_OPENED_STREAM 2

#define NNSH_NGCHECK_ALLAREA        0
#define NNSH_NGCHECK_NAME_MAIL      1
#define NNSH_NGCHECK_NOTMESSAGE     2
#define NNSH_NGCHECK_MESSAGE        3

#define NNSH_COPYSTATUS_SEARCHTOP   0
#define NNSH_COPYSTATUS_COPYING     1
#define NNSH_COPYSTATUS_COPYDONE    2

#define NNSH_GETLOGDB_CHECKURL          0x0000  
#define NNSH_GETLOGDB_CHECKPARENT       0x0001
#define NNSH_GETLOGDB_CHECKTOKEN        0x0002
#define NNSH_GETLOGDB_CHECKDATE         0x0004
#define NNSH_GETLOGDB_CHECK_PARENT_DATE 0x0005
#define NNSH_GETLOGDB_FORCE_DELETE      0x0008

#define NNSH_GETLOGDBMAX_SUBDATA    20000
#define NNSH_GETLOGDBMAX_SITEDATA   190

/** BBS-DBの表示状態(選択画面で表示するか/しないか) **/
#define NNSH_BBSSTATE_VISIBLE       0x01         // 表示
#define NNSH_BBSSTATE_INVISIBLE     0x00         // 非表示
#define NNSH_BBSSTATE_VISIBLEMASK   0xfe         // 表示・非表示用マスク

#define NNSH_BBSSTATE_LEVELMASK     0x0e         // スレレベル抽出マスク
#define NNSH_BBSSTATE_NOTFAVOR      0xf1         // スレレベルクリア
#define NNSH_BBSSTATE_FAVOR_L1      0x02         // スレレベルL1
#define NNSH_BBSSTATE_FAVOR_L2      0x04         // スレレベルL2
#define NNSH_BBSSTATE_FAVOR_L3      0x06         // スレレベルL3
#define NNSH_BBSSTATE_FAVOR_L4      0x08         // スレレベルL4
#define NNSH_BBSSTATE_FAVOR         0x0e         // スレレベルHIGH

#define NNSH_BBSSTATE_SUSPEND       0x10   // 「取得停止」BBSである
#define NNSH_BBSSTATE_NOTSUSPEND    0xef   // 「取得停止」BBSではない(MASK値)

/** BBS-DBのボードタイプ **/
#define NNSH_BBSTYPE_2ch                 0x00    // 2ch(通常モード)
#define NNSH_BBSTYPE_MACHIBBS            0x01    // まちBBS(特殊モード)
#define NNSH_BBSTYPE_SHITARABA           0x02    // したらばBBS
#define NNSH_BBSTYPE_SHITARABAJBBS_OLD   0x03    // したらば@JBBS
#define NNSH_BBSTYPE_OTHERBBS            0x04    // 2ch互換BBS
#define NNSH_BBSTYPE_PLUGINSUPPORT       0x05    // 2ch形式変換BBS
#define NNSH_BBSTYPE_HTML                0x06    // html形式データ
#define NNSH_BBSTYPE_SHITARABAJBBS_RAW   0x07    // したらば@JBBS(RAWMODE)
#define NNSH_BBSTYPE_PLAINTEXT           0x08    // 通常のテキスト形式
#define NNSH_BBSTYPE_2ch_EUC             0x09    // 2ch(EUC形式)
#define NNSH_BBSTYPE_OTHERBBS_2          0x0a    // 2ch互換形式(その2)
#define NNSH_BBSTYPE_ERROR               0xff    // ボードタイプエラー
#define NNSH_BBSTYPE_MASK                0x1f    // ボードタイプMASK
#define NNSH_BBSTYPE_CHARSETMASK         0xe0    // ボードの文字コード
#define NNSH_BBSTYPE_CHAR_SJIS           0x00    // 標準文字コード(SHIFT JIS)
#define NNSH_BBSTYPE_CHAR_EUC            0x20    // EUC漢字コード
#define NNSH_BBSTYPE_CHAR_JIS            0x40    // JIS漢字コード(iso-2022-jp)
#define NNSH_BBSTYPE_CHAR_SHIFT             5    // 右シフト

/** 一覧表示がBBS一覧かお気に入りか **/
#define NNSH_FAVORLIST_MODE         1
#define NNSH_BBSLIST_MODE           0 

// データベースのキータイプ(数字か文字列か)
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2
#define NNSH_KEYTYPE_SUBJECT        3

// HTTPメッセージ作成時に指定
#define HTTP_SENDTYPE_GET                    10
#define HTTP_SENDTYPE_GET_NOTMONA            11
#define HTTP_SENDTYPE_POST                   20
#define HTTP_SENDTYPE_POST_SEQUENCE2         21
#define HTTP_SENDTYPE_POST_2chLOGIN          24
#define HTTP_SENDTYPE_POST_OTHERMETHOD       25
#define HTTP_SENDTYPE_POST_Be2chLOGIN        27
#define HTTP_SENDTYPE_POST_OTHERBBS          30
#define HTTP_SENDTYPE_POST_MACHIBBS          40
#define HTTP_SENDTYPE_POST_SHITARABAJBBS     80
#define HTTP_SENDTYPE_POST_SHITARABA         90
#define HTTP_SENDTYPE_MASK               0x00ff
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETLOG_LEN             1536
#define HTTP_GETLOG_START           0
#define HTTP_GETSUBJECT_START       0
#define HTTP_GETSUBJECT_PART        0

// 検索方向
#define NNSH_SEARCH_ALL             0
#define NNSH_SEARCH_FORWARD         1
#define NNSH_SEARCH_BACKWARD       -1

// 書き込みメッセージ確認コマンド
#define NNSH_WRITECHK_SPID          0
#define NNSH_WRITECHK_REPLY         1

// メッセージの属性
#define NNSH_MSGATTR_NOTSPECIFY    0xff   // 「お気に入り」指定は指定しない
#define NNSH_MSGATTR_NOTFAVOR      0x4f   // 「お気に入り」スレでない(MASK値)
#define NNSH_MSGATTR_FAVOR_L1      0x10   // 「お気に入り」レベル(低)
#define NNSH_MSGATTR_FAVOR_L2      0x20   // 「お気に入り」レベル(中低)
#define NNSH_MSGATTR_FAVOR_L3      0x30   // 「お気に入り」レベル(中)
#define NNSH_MSGATTR_FAVOR_L4      0x80   // 「お気に入り」レベル(中高)
#define NNSH_MSGATTR_FAVOR         0xb0   // 「お気に入り」スレ(高)
#define NNSH_MSGATTR_NOTERROR      0xbf   // 「エラー発生」スレでない(MASK値)
#define NNSH_MSGATTR_ERROR         0x40   // 「エラー発生」スレである
#define NNSH_MSGATTR_GETRESERVE    0x08   // 「取得予約」スレである
#define NNSH_MSGATTR_NOTRESERVE    0xf7   // 「取得予約」スレをクリアする(MASK)
#define NNSH_MSGATTR_NGWORDSET     0x04   //  NGワード設定 ON
#define NNSH_MSGATTR_CLRNGWORD     0xfb   //  NGワード設定 OFF (クリア)

#define NNSH_ATTRLABEL_TITLE       ", Level:" 
#define NNSH_ATTRLABEL_NOTFAVOR    "-"    // 「お気に入り」ではない
#define NNSH_ATTRLABEL_FAVOR_L1    "L"    // 「お気に入り」レベル(低)
#define NNSH_ATTRLABEL_FAVOR_L2    "1"    // 「お気に入り」レベル(中低)
#define NNSH_ATTRLABEL_FAVOR_L3    "2"    // 「お気に入り」レベル(中)
#define NNSH_ATTRLABEL_FAVOR_L4    "3"    // 「お気に入り」レベル(中高)
#define NNSH_ATTRLABEL_FAVOR       "H"    // 「お気に入り」レベル(高)

#define NUMBER_OF_JOGMASKS          4

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
#define NNSH_DB_UPDATE_LOGSUBDB     0x0010
#define NNSH_DB_UPDATE_LOGCHARGE    0x0020
#define NNSH_DB_UPDATE_LOGTOKEN     0x0040

#define NNSH_DB_MASK_BBS            0xfffe
#define NNSH_DB_MASK_SUBJECT        0xfffd
#define NNSH_DB_MASK_SETTING        0xfffb
#define NNSH_DB_MASK_DIRINDEX       0xfff7

//////////////////////////////////////////////////////////////////////////////

/********** HTML状態解析用 ********/
#define HTTP_ANCHORSTATE_NORMAL     101
#define HTTP_ANCHORSTATE_ANCHOR     102
#define HTTP_ANCHORSTATE_LABEL      103
#define HTTP_ANCHORSTATE_KANJI      104

/**********  スレ読み出し状態用  *********/
#define NNSH_SUBJSTATUS_NOT_YET      0
#define NNSH_SUBJSTATUS_NEW          1
#define NNSH_SUBJSTATUS_UPDATE       2
#define NNSH_SUBJSTATUS_REMAIN       3
#define NNSH_SUBJSTATUS_ALREADY      4
#define NNSH_SUBJSTATUS_DELETE       5
#define NNSH_SUBJSTATUS_UNKNOWN      6
#define NNSH_SUBJSTATUS_GETERROR     7
#define NNSH_SUBJSTATUS_OVER         8
#define NNSH_SUBJSTATUS_DELETE_OFFLINE  20

#define NNSH_MSGSTATE_NOTMODIFIED   10
#define NNSH_MSGSTATE_ERROR         20

#define NNSH_GETLOGSTATE_SELECTED    0x0002
#define NNSH_GETLOGSTATE_UPDATE      0x0001
#define NNSH_GETLOGSTATE_DATETIME    0x0004
#define NNSH_GETLOGSTATE_NOTMODIFIED 0x0000
#define NNSH_GETLOGSTATE_MASK_UPDATE 0x0001
#define NNSH_GETLOGSTATE_MASK_SELECT 0x0002

#define NNSH_ERRORCODE_PARTGET        (~errNone - 12)
#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)
#define NNSH_ERRORCODE_THREADNOTHING  (~errNone - 88)
#define NNSH_ERRORCODE_WRONGVFSMODE   (~errNone - 10)
#define NNSH_ERRORCODE_COPYFAIL       (~errNone - 11)
#define NNSH_ERRORCODE_NOTFOUND       (~errNone - 12)
#define NNSH_ERRORCODE_NOTACCEPTEDMSG (~errNone + 100)

/**********  メッセージパース用  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/**********  BBS所有スレ数  **************/
#define NNSH_BBSLIST_AGAIN         -1            // スレ数を再度確認する


/*********** 新着確認実施コマンド ********/
#define NNSH_NEWARRIVAL_NORMAL      0            // 通常取得モード
#define NNSH_NEWARRIVAL_ONLYMESNUM  1            // スレ番指定予約のもののみ

/////////////////////////////////////////////////////////////////////////////

/*** DATA TOKENS ***/
#define DATA_SEPARATOR              "<>"
#define DATA_NEWLINE                "<br>"

/** for BBS NAME PARSER **/
#define HTTP_TOKEN_ST_ANCHOR        "<A"
#define HTTP_TOKEN_ED_ANCHOR        "</A>"
#define HTTP_TOKEN_END              ">"


#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01
#define NNSH_KANJICODE_JIS          0x02
#define NNSH_KANJICODE_UTF8         0x03
#define NNSH_KANJICODE_NOT_TEXT     0xf0

#define NNSH_CHARCOLOR_ESCAPE       '\x1b'
#define NNSH_CHARCOLOR_NORMAL       '\x09'
#define NNSH_CHARCOLOR_HEADER       '\x0b'
#define NNSH_CHARCOLOR_BOLDHEADER   '\x0f'
#define NNSH_CHARCOLOR_FOOTER       '\x0c'
#define NNSH_CHARCOLOR_INFORM       '\x0e'
#define NNSH_CHARCOLOR_ANCHOR       '\x08'
#define NNSH_CHARCOLOR_BOLDDATA     '\x11'

#define NNSH_HTTP_HEADER_ENDTOKEN   "\x0d\x0a\x0d\0a"
#define NNSH_NGSET_SEPARATOR        ','
#define NNSH_FIELD_SEPARATOR        "<>"
#define NNSH_FIELD_SEPARATOR_MACHI  ","
#define NNSH_RECORD_SEPARATOR       "\x0a"
#define NNSH_SUBJTITLE_END          "("

#define NNSH_JBBSTOKEN_START        "\x0a<dt>"
#define NNSH_JBBSTOKEN_END          "<br><br>\x0a"
#define NNSH_JBBSTOKEN_MSGSTART     "\xcc\xbe\xc1\xb0\xa1\xa7" // 名前：
#ifdef OLD_MACHIBBS_TOKEN
#define NNSH_MACHITOKEN_START       "\x0d\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a\x0d\x0a"
#else
#define NNSH_MACHITOKEN_START       "\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a"
#endif
#define NNSH_MACHITOKEN_BEGINCOMMENT "<!--"
#define NNSH_JBBSTOKEN_RAW_START    "\x0d\x0a"
#define NNSH_JBBSTOKEN_RAW_START_RES "<>"
#define NNSH_JBBSTOKEN_RAW_END_RES  "\x0a"
#define NNSH_MACHITOKEN_MSGSTART2   "<dt>"
#define NNSH_MACHITOKEN_MSGSTART    "名前："
#define NNSH_MACHINAME_SUFFIX       "[まちBBS]"
#define NNSH_SHITARABA_SUFFIX       "[したらば]"
#define NNSH_SHITARABAJBBS_SUFFIX   "[したらば@JBBS]"

#define NNSH_TOKEN_BEFORE_NUMBER     0x01    // トークンの前に数字
#define NNSH_TOKEN_BEFORE_LETTER     0x02    // トークンの前に文字
#define NNSH_TOKEN_AFTER_NUMBER      0x10    // トークンの後に数字
#define NNSH_TOKEN_AFTER_LETTER      0x20    // トークンの後に文字

/** **/
#define NNSH_SYMBOL_WRITE_DONE      "書きこみました。"
#define NNSH_SYMBOL_WRITE_NG        "ＥＲＲＯＲ"

#define NNSH_DEVICE_NORMAL         0x0000     // 標準Palm
#define NNSH_DEVICE_HANDERA        0x0001     // Handera 330
#define NNSH_DEVICE_DIASUPPORT     0x0002     // Dynamic Input Areaサポート機
#define NNSH_DEVICE_GARMIN         0x0003     // Garmin iQUE 3600

#define NNSH_VFS_USEMEDIA_ANY       0
#define NNSH_VFS_USEMEDIA_CF        1
#define NNSH_VFS_USEMEDIA_MS        2
#define NNSH_VFS_USEMEDIA_SD        3
#define NNSH_VFS_USEMEDIA_SM        4
#define NNSH_VFS_USEMEDIA_RD        5
#define NNSH_VFS_USEMEDIA_DoC       6

#define NNSH_JUMPMSG_LEN            14
#define NNSH_JUMPMSG_HEAD           ">>"

#define NNSH_JUMPMSG_BACK           "１つ戻る"
#define NNSH_JUMPMSG_TO_LIST        "一覧へ"
#define NNSH_JUMPMSG_OPEN_MENU      "メニュー"
#define NNSH_JUMPMSG_UPDATE         "差分取得"
#define NNSH_JUMPMSG_HANGUP         "回線切断"
#define NNSH_JUMPMSG_TOP            "先頭へ"
#define NNSH_JUMPMSG_BOTTOM         "末尾へ"
#define NNSH_JUMPMSG_OPENWEB        "全選択後にWeb"
#define NNSH_JUMPMSG_CHANGEVIEW     "描画モード変更"
#define NNSH_JUMPMSG_FAVORITE       "スレ情報"
#define NNSH_JUMPMSG_OUTPUTMEMO     "メモ帳へ"
#define NNSH_JUMPMSG_BOOKMARK       "しおり設定"
#define NNSH_JUMPMSG_DEVICEINFO     "デバイス情報"
#define NNSH_JUMPMSG_OPENLINK       "リンクを開く"
#define NNSH_JUMPMSG_EXECA2DA       "a2DA(AA参照)"
#define NNSH_JUMPMSG_BTOOTH_ON      "Bt On"
#define NNSH_JUMPMSG_BTOOTH_OFF     "Bt Off"
#define NNSH_JUMPMSG_MEMOURL        "MemoURLへ出力"
#define NNSH_JUMPMSG_TOGGLERAWMODE  "RAW/通常切替"
#define NNSH_JUMPMSG_SELECTSEPA     "区切り変更"
#define NNSH_JUMPMSG_EDITSEPA       "区切り編集"
#define NNSH_JUMPMSG_OPENLINK_WEBDA "リンク先参照"
#define NNSH_JUMPMSG_TOGGLE_NGWORD  "NG切替"
#define NNSH_JUMPMSG_BACK_AND_DEL   "削除後一覧へ"
#define NNSH_JUMPMSG_GET_THREADNUM  "スレ番指定"
#define NNSH_JUMPMSG_ENTRY_NGWORD   "NG登録"
#define NNSH_JUMPMSG_REPLYMSG       "参照カキコ"

#define NNSH_NOF_JUMPMSG_EXT        28
#define NNSH_JUMPSEL_BACK           0x8001
#define NNSH_JUMPSEL_TO_LIST        0x8002
#define NNSH_JUMPSEL_OPEN_MENU      0x8003
#define NNSH_JUMPSEL_UPDATE         0x8004
#define NNSH_JUMPSEL_HANGUP         0x8005
#define NNSH_JUMPSEL_TOP            0x8006
#define NNSH_JUMPSEL_BOTTOM         0x8007
#define NNSH_JUMPSEL_OPENWEB        0x8008
#define NNSH_JUMPSEL_CHANGEVIEW     0x8009
#define NNSH_JUMPSEL_FAVORITE       0x800a
#define NNSH_JUMPSEL_OUTPUTMEMO     0x800b
#define NNSH_JUMPSEL_BOOKMARK       0x800c
#define NNSH_JUMPSEL_DEVICEINFO     0x800d
#define NNSH_JUMPSEL_OPENLINK       0x800e
#define NNSH_JUMPSEL_EXECA2DA       0x800f
#define NNSH_JUMPSEL_BTOOTH_ON      0x8011
#define NNSH_JUMPSEL_BTOOTH_OFF     0x8012
#define NNSH_JUMPSEL_MEMOURL        0x8013
#define NNSH_JUMPSEL_TOGGLERAWMODE  0x8014
#define NNSH_JUMPSEL_SELECTSEPA     0x8015
#define NNSH_JUMPSEL_EDITSEPA       0x8016
#define NNSH_JUMPSEL_OPENLINK_WEBDA 0x8017
#define NNSH_JUMPSEL_TOGGLE_NGWORD  0x8018
#define NNSH_JUMPSEL_BACK_AND_DEL   0x8019
#define NNSH_JUMPSEL_GET_THREADNUM  0x801a
#define NNSH_JUMPSEL_ENTRY_NGWORD   0x801b
#define NNSH_JUMPSEL_REPLYMSG       0x801c


#define NNSH_JUMPMSG_OPENBBS        "板を選択"
#define NNSH_JUMPMSG_OPENBBS_GETLOG "サイト選択"
#define NNSH_JUMPMSG_OPENMENU       "メニュー"
#define NNSH_JUMPMSG_OPENNEW        "新着確認"
#define NNSH_JUMPMSG_OPENLIST       "一覧取得"
#define NNSH_JUMPMSG_OPENGET        "スレ取得"
#define NNSH_JUMPMSG_OPENINFO       "スレ情報"
#define NNSH_JUMPMSG_OPENMES        "スレ参照"
#define NNSH_JUMPMSG_DISCONNECT     "回線切断"
#define NNSH_JUMPMSG_COPYMSG        "参照へコピー"
#define NNSH_JUMPMSG_DELMSG         "スレ削除"
#define NNSH_JUMPMSG_GRAPHMODE      "描画モード変更"
#define NNSH_JUMPMSG_MULTISW1       "多目的SW1"
#define NNSH_JUMPMSG_MULTISW2       "多目的SW2"
#define NNSH_JUMPMSG_NNSIEND        "NNsi終了"
#define NNSH_JUMPMSG_SHOWDEVINFO    "デバイス情報"
#define NNSH_JUMPMSG_DIRSELECTION   "Dir選択(chdir)"
#define NNSH_JUMPMSG_CHANGELOGLOC   "ログ位置変更"
#define NNSH_JUMPMSG_CHANGELOGDIR   "Dir変更(move)"
#define NNSH_JUMPMSG_BT_ON          "Bt On"
#define NNSH_JUMPMSG_BT_OFF         "Bt Off"
#define NNSH_JUMPMSG_OPENGETLOGLIST "参照ログ一覧"

#define NNSH_ITEMLEN_JUMPLIST       14      // リストアイテム文字列長
#define NNSH_JUMPSEL_NUMLIST        21      // リストアイテム数
#define NNSH_JUMPSEL_OPENBBS        0x9000  // 板を選択
#define NNSH_JUMPSEL_OPENMENU       0x9001  // メニュー
#define NNSH_JUMPSEL_DISCONNECT     0x9002  // 回線切断
#define NNSH_JUMPSEL_OPENNEW        0x9003  // 新着確認
#define NNSH_JUMPSEL_OPENLIST       0x9004  // 一覧取得
#define NNSH_JUMPSEL_OPENGET        0x9005  // スレ取得
#define NNSH_JUMPSEL_OPENMES        0x9006  // スレ参照
#define NNSH_JUMPSEL_OPENINFO       0x9007  // スレ情報
#define NNSH_JUMPSEL_COPYMSG        0x9008  // 参照へコピー
#define NNSH_JUMPSEL_DELMSG         0x9009  // スレ削除
#define NNSH_JUMPSEL_GRAPHMODE      0x900a  // 描画モード変更
#define NNSH_JUMPSEL_MULTISW1       0x900b  // 多目的SW1
#define NNSH_JUMPSEL_MULTISW2       0x900c  // 多目的SW2
#define NNSH_JUMPSEL_NNSIEND        0x900d  // NNsi終了
#define NNSH_JUMPSEL_SHOWDEVINFO    0x900e  // デバイス情報
#define NNSH_JUMPSEL_DIRSELECTION   0x900f  // ディレクトリ選択
#define NNSH_JUMPSEL_CHANGELOGLOC   0x9010  // ログ位置変更
#define NNSH_JUMPSEL_CHANGELOGDIR   0x9011  // ディレクトリ変更
#define NNSH_JUMPSEL_BT_ON          0x9012  // Bt On
#define NNSH_JUMPSEL_BT_OFF         0x9013  // Bt Off
#define NNSH_JUMPSEL_OPENGETLOGLIST 0x9014  // 参照ログ一覧へ

#define NNSH_ITEMLEN_GETLOGJUMP     14      // リストアイテム文字列長
#define NNSH_NOF_JUMPMSG_GETLOG     16
#define NNSH_JUMPSEL_GETLOGRETURN   0xA001
#define NNSH_JUMPSEL_GETLOG_OPEN    0xA002
#define NNSH_JUMPSEL_GETLOGBROWSER  0xA003
#define NNSH_JUMPSEL_GETLOG_MEMOURL 0xA004
#define NNSH_JUMPSEL_GETLOG_CHECK   0xA005
#define NNSH_JUMPSEL_GETLOG_OPENMNU 0xA006
#define NNSH_JUMPSEL_GETLOG_LEVEL   0xA007
#define NNSH_JUMPSEL_GETLOG_DISCONN 0xA008
#define NNSH_JUMPSEL_GETLOG_BT_ON   0xA009
#define NNSH_JUMPSEL_GETLOG_BT_OFF  0xA00A
#define NNSH_JUMPSEL_GETLOG_NNSIEND 0xA00B
#define NNSH_JUMPSEL_GETLOG_EDIT    0xA00C  // 未実装
#define NNSH_JUMPSEL_GETLOG_NEW     0xA00D  // 
#define NNSH_JUMPSEL_GETLOG_SEPA    0xA00E  // 
#define NNSH_JUMPSEL_GETLOG_WWWINFO 0xA00F
#define NNSH_JUMPSEL_GETLOG_DELETE_READ 0xA010

#define NNSH_JUMPMSG_GETLOGRETURN   "スレ一覧へ"
#define NNSH_JUMPMSG_GETLOG_OPEN    "開く"
#define NNSH_JUMPMSG_GETLOGBROWSER  "ブラウザで開く"
#define NNSH_JUMPMSG_GETLOG_MEMOURL "MemoURLに送る"
#define NNSH_JUMPMSG_GETLOG_CHECK   "新着確認"
#define NNSH_JUMPMSG_GETLOG_OPENMNU "メニュー"
#define NNSH_JUMPMSG_GETLOG_LEVEL   "Level変更"
#define NNSH_JUMPMSG_GETLOG_DISCONN "回線切断"
#define NNSH_JUMPMSG_GETLOG_BT_ON   "Bt On"
#define NNSH_JUMPMSG_GETLOG_BT_OFF  "Bt Off"
#define NNSH_JUMPMSG_GETLOG_NNSIEND "NNsi終了"

#define NNSH_JUMPMSG_GETLOG_EDIT    "編集"
#define NNSH_JUMPMSG_GETLOG_NEW     "新規登録"
#define NNSH_JUMPMSG_GETLOG_SEPA    "区切編集"
#define NNSH_JUMPMSG_GETLOG_WWWINFO "サイト情報"
#define NNSH_JUMPMSG_GETLOG_DELETE_READ "既読記事削除"

#define NNSH_STEP_ITEMUP           -1
#define NNSH_STEP_ITEMDOWN          1
#define NNSH_STEP_PAGEUP           -1
#define NNSH_STEP_PAGEDOWN          1
#define NNSH_STEP_REDRAW            0
#define NNSH_STEP_UPDATE           10
#define NNSH_STEP_TO_BOTTOM       100
#define NNSH_STEP_TO_TOP          NNSH_STEP_UPDATE
#define NNSH_STEP_NEWOPEN         200

#define NNSH_ITEM_LASTITEM         0xffff
#define NNSH_ITEM_BOTTOMITEM       0xfffd

#define NNSH_DISP_NOTHING          0x0000   // なし
#define NNSH_DISP_UPPERLIMIT       0x0001   // 上限
#define NNSH_DISP_HALFWAY          0x0002   // 途中
#define NNSH_DISP_ALL              0x0004   // 全表示
#define NNSH_DISP_LOWERLIMIT       0x0008   // 下限

#define NNSH_DISPMSG_NOTHING       "なし"
#define NNSH_DISPMSG_UPPERLIMIT    "上限"
#define NNSH_DISPMSG_HALFWAY       "途中"
#define NNSH_DISPMSG_ALL           "全表示"
#define NNSH_DISPMSG_LOWERLIMIT    "下限"
#define NNSH_DISPMSG_UNKNOWN       "?????"

#define NNSH_MSGNUM_UNKNOWN        0xffff
#define NNSH_MSGNUM_NOTDISPLAY     0x0000

#define NNSH_LIMIT_COPYBUFFER      2000
#define NNSH_LIMIT_MEMOPAD         4000
#define NNSH_LIMIT_RETRY           5
#define NNSH_MEMOBUFFER_MERGIN     96

#define NNSH_BUSYWIN_MAXLEN        32
#define NNSH_MAX_OMIT_LENGTH       10

#define NNSH_WRITE_DELAYTIME       7
#define NNSH_NEWTHREAD_DELAYTIME   2
#define NNSH_WRITE_DELAYSECOND     45

#define NNSH_MSGTOKENSTATE_NONE    0
#define NNSH_MSGTOKENSTATE_MESSAGE 1
#define NNSH_MSGTOKENSTATE_END     2

#define NNSH_INPUTWIN_SEARCH       "TITLE検索"
#define NNSH_INPUTWIN_GETMESNUM    "スレ取得(スレ番指定)"
#define NNSH_INPUTWIN_MODIFYTITLE  "スレタイトル変更"
#define NNSH_INPUTWIN_NGWORD1      "NG文字列１の登録"
#define NNSH_INPUTWIN_NGWORD2      "NG文字列２の登録"
#define NNSH_INPUTWIN_SETPASS      "NNsi起動ワード設定"
#define NNSH_INPUTWIN_ENTPASS      "Register..."
#define NNSH_INPUTWIN_RESOLVE      "Host Name"

#define NNSH_DIALOG_USE_SEARCH      10
#define NNSH_DIALOG_USE_GETMESNUM   20
#define NNSH_DIALOG_USE_MODIFYTITLE 40
#define NNSH_DIALOG_USE_SEARCH_TITLE 50
#define NNSH_DIALOG_USE_SEARCH_MESSAGE 60
#define NNSH_DIALOG_USE_SETNGWORD   80
#define NNSH_DIALOG_USE_PASS       100
#define NNSH_DIALOG_USE_RESOLVE    110
#define NNSH_DIALOG_USE_INPUTURL   120
#define NNSH_DIALOG_USE_MACROINPUT 130
#define NNSH_DIALOG_USE_OTHER      150

// ダイアログを省略する設定
#define NNSH_OMITDIALOG_NOTHING      0x00
#define NNSH_OMITDIALOG_CONFIRM      0x01
#define NNSH_OMITDIALOG_WARNING      0x02
#define NNSH_OMITDIALOG_INFORMATION  0x04
#define NNSH_OMITDIALOG_ALLOMIT      ((NNSH_OMITDIALOG_CONFIRM)|(NNSH_OMITDIALOG_WARNING)|(NNSH_OMITDIALOG_INFORMATION))

// 多目的ボタン関連の設定
#define NNSH_SWITCHUSAGE_FORMMASK   0x3f00
#define NNSH_SWITCHUSAGE_FUNCMASK   0x00ff

#define NNSH_SWITCHUSAGE_TITLE      0x2100

#define NNSH_SWITCHUSAGE_NOUSE      0x00  // 多目的スイッチを使用しない
#define NNSH_SWITCHUSAGE_GETPART    0x01  // スレ取得モード(一括/分割)
#define NNSH_SWITCHUSAGE_HANZEN     0x02  // 半角カナ→全角カナ変換
#define NNSH_SWITCHUSAGE_USEPROXY   0x03  // Proxy経由でアクセス
#define NNSH_SWITCHUSAGE_CONFIRM    0x04  // 確認ダイアログの省略

#define NNSH_SWITCHUSAGE_SUBJECT    0x05  // スレ一覧全取得切り替え  (一覧画面用)
#define NNSH_SWITCHUSAGE_TITLEDISP  0x06  // スレ一覧描画モード切替  (一覧画面用)
#define NNSH_SWITCHUSAGE_MSGNUMBER  0x07  // スレ一覧にMSG番号       (一覧画面用)
#define NNSH_SWITCHUSAGE_GETRESERVE 0x08  // スレ取得保留モード      (一覧画面用)
#define NNSH_SWITCHUSAGE_IMODEURL   0x09  // i-mode URL使用モード    (一覧画面用)
#define NNSH_SWITCHUSAGE_BLOCKDISP  0x0a  // ゾーン表示モード        (一覧画面用)
#define NNSH_SWITCHUSAGE_MACHINEW   0x0b  // まちBBSも新着確認する   (一覧画面用)
#define NNSH_SWITCHUSAGE_USEPLUGIN  0x0c  // 新着確認後にメール確認  (一覧画面用)
#define NNSH_SWITCHUSAGE_LISTUPDATE 0x0d  // 一覧取得とスレ更新      (一覧画面用)
#define NNSH_SWITCHUSAGE_NOREADONLY 0x0e  // 取得済み全てには参照ログなし
#define NNSH_SWITCHUSAGE_NORDOLY_NW 0x0f  // 未読ありには参照ログなし
#define NNSH_SWITCHUSAGE_DISCONN_NA 0x10  // 新着確認後に回線を切断する
#define NNSH_SWITCHUSAGE_PREEXEC_NA 0x11  // 新着確認前にDAを実行する
#define NNSH_SWITCHUSAGE_PR_NOTREAD 0x12  // 一覧では未読数を表示する
#define NNSH_SWITCHUSAGE_GETLOGURL  0x13  // 一覧に表示するのは参照ログで、サイト毎


// 多目的ボタンの初期設定値
#define NNSH_SWITCH1USAGE_DEFAULT   NNSH_SWITCHUSAGE_GETPART    // スレ取得モード
#define NNSH_SWITCH2USAGE_DEFAULT   NNSH_SWITCHUSAGE_USEPROXY   // Proxy経由でのアクセス

#define NNSH_SUBJSTATUSMSG_NOT_YET  "未取得"
#define NNSH_SUBJSTATUSMSG_NEW      "新規取得"
#define NNSH_SUBJSTATUSMSG_UPDATE   "更新取得"
#define NNSH_SUBJSTATUSMSG_REMAIN   "未読あり"
#define NNSH_SUBJSTATUSMSG_ALREADY  "既読"
#define NNSH_SUBJSTATUSMSG_UNKNOWN  "状態不明"

/** スレ内検索 **/
#define MAX_SEARCH_STRING 64

#define NNSH_VIEW_SEARCHSTRING_HEADER  "ID:"  // 検索用文字列抽出ヘッダ

/** スレ内検索方法 **/
#define NNSH_SEARCH_METHOD_FROM_TOP    0 //先頭から検索
#define NNSH_SEARCH_METHOD_NEXT        1 //次検索
#define NNSH_SEARCH_METHOD_PREV        2 //前検索
#define NNSH_SEARCH_METHOD_FROM_BOTTOM 3 //末尾から検索

/** メッセージ取得方法 **/
#define GET_MESSAGE_TYPE_TOP    1 // 末尾方向への検索用
#define GET_MESSAGE_TYPE_HALF   2 // スレ参照用
#define GET_MESSAGE_TYPE_BOTTOM 3 // 先頭方向への検索用


// FORM ID LIST
#define NNSH_FRMID_THREAD        0x1200    // 一覧画面
#define NNSH_FRMID_MESSAGE       0x1201    // 参照画面
#define NNSH_FRMID_DUMMY         0x1202    // ダミー画面
#define NNSH_FRMID_WRITE_MESSAGE 0x1203    // 書き込み画面
#define NNSH_FRMID_GETLOGLIST    0x1204    // 参照ログ一覧画面


// 多目的ボタンの設定
#define MAX_CAPTION                  6
#define LIMIT_CAPTION                4

#define MULTIBTN1_CAPTION_DEFAULT    "参照"
#define MULTIBTN1_FEATURE_DEFAULT    MULTIBTN_FEATURE_OPEN

#define MULTIBTN2_CAPTION_DEFAULT    "取得"
#define MULTIBTN2_FEATURE_DEFAULT    MULTIBTN_FEATURE_GET

#define MULTIBTN3_CAPTION_DEFAULT    "削除"
#define MULTIBTN3_FEATURE_DEFAULT    MULTIBTN_FEATURE_DELETE

#define MULTIBTN4_CAPTION_DEFAULT    "切断"
#define MULTIBTN4_FEATURE_DEFAULT    MULTIBTN_FEATURE_DISCONN

#define MULTIBTN5_CAPTION_DEFAULT    "一覧"
#define MULTIBTN5_FEATURE_DEFAULT    MULTIBTN_FEATURE_GETLIST

#define MULTIBTN6_CAPTION_DEFAULT    "新着"
#define MULTIBTN6_FEATURE_DEFAULT    MULTIBTN_FEATURE_NEWARRIVAL

#define UPBTN_FEATURE_DEFAULT        MULTIBTN_FEATURE_SELECTPREV
#define DOWNBTN_FEATURE_DEFAULT      MULTIBTN_FEATURE_SELECTNEXT

#define JOGPUSH_FEATURE_DEFAULT      MULTIBTN_FEATURE_OPEN
#define JOGBACK_FEATURE_DEFAULT      MULTIBTN_FEATURE_SELMENU
#define CAPTURE_FEATURE_DEFAULT      MULTIBTN_FEATURE_INFO

#define LEFTBTN_FEATURE_DEFAULT      MULTIBTN_FEATURE_PREVPAGE
#define RIGHTBTN_FEATURE_DEFAULT     MULTIBTN_FEATURE_NEXTPAGE

#define MULTIBTN_FEATURE_MASK        0x00ff
#define MULTIBTN_FEATURE             0x7600

// 一覧画面の機能設定（機能番号）
//   ※ 設定用タイトルは、リソースで定義(そちらと同期してある必要がある)
#define MULTIBTN_FEATURE_INFO        0  // "情報"
#define MULTIBTN_FEATURE_OPEN        1  // "参照"
#define MULTIBTN_FEATURE_GET         2  // "スレ取得"
#define MULTIBTN_FEATURE_ALLGET      3  // "新規(再)取得"
#define MULTIBTN_FEATURE_PARTGET     4  // "差分取得"
#define MULTIBTN_FEATURE_DELETE      5  // "スレ削除"
#define MULTIBTN_FEATURE_SORT        6  // "スレ整列"
#define MULTIBTN_FEATURE_MODTITLE    7  // "スレタイ変更"
#define MULTIBTN_FEATURE_OPENWEB     8  // "ブラウザで開く"
#define MULTIBTN_FEATURE_MESNUM      9  // "スレ番指定取得"
#define MULTIBTN_FEATURE_SEARCH     10  // "スレタイ検索"
#define MULTIBTN_FEATURE_NEXT       11  // "次検索"
#define MULTIBTN_FEATURE_PREV       12  // "前検索"
#define MULTIBTN_FEATURE_TOP        13  // "先頭へ移動"
#define MULTIBTN_FEATURE_BOTTOM     14  // "末尾へ移動"
#define MULTIBTN_FEATURE_GETBBS     15  // "BBS一覧取得"
#define MULTIBTN_FEATURE_USEBBS     16  // "使用BBS選択"
#define MULTIBTN_FEATURE_COPYGIKO   17  // "参照ログへCOPY"
#define MULTIBTN_FEATURE_DELNOTGET  18  // "未取得一覧削除"
#define MULTIBTN_FEATURE_DBCOPY     19  // "DBをVFSへCOPY"
#define MULTIBTN_FEATURE_GRAPHMODE  20  // "描画モード変更"
#define MULTIBTN_FEATURE_ROTATE     21  // "画面の回転(HE)"
#define MULTIBTN_FEATURE_FONT       22  // "フォント変更"
#define MULTIBTN_FEATURE_NETWORK    23  // "Network設定"
#define MULTIBTN_FEATURE_SELMENU    24  // "選択メニュー"
#define MULTIBTN_FEATURE_DISCONN    25  // "回線切断"
#define MULTIBTN_FEATURE_GETLIST    26  // "スレ一覧取得"
#define MULTIBTN_FEATURE_NEWARRIVAL 27  // "新着確認"
#define MULTIBTN_FEATURE_NNSHSET    28  // "NNsi設定(概略)"
#define MULTIBTN_FEATURE_NNSHSET1   29  // "NNsi設定-1"
#define MULTIBTN_FEATURE_NNSHSET2   30  // "NNsi設定-2"
#define MULTIBTN_FEATURE_NNSHSET3   31  // "NNsi設定-3"
#define MULTIBTN_FEATURE_NNSHSET4   32  // "NNsi設定-4"
#define MULTIBTN_FEATURE_NNSHSET5   33  // "NNsi設定-5"
#define MULTIBTN_FEATURE_NNSHSET6   34  // "NNsi設定-6"
#define MULTIBTN_FEATURE_NNSHSET7   35  // "NNsi設定-7"
#define MULTIBTN_FEATURE_NNSHSET8   36  // "NNsi設定-8"
#define MULTIBTN_FEATURE_NNSHSET9   37  // "NNsi設定-9"
#define MULTIBTN_FEATURE_VERSION    38  // "バージョン"
#define MULTIBTN_FEATURE_SELECTNEXT 39  // "１つ下"
#define MULTIBTN_FEATURE_SELECTPREV 40  // "１つ上"
#define MULTIBTN_FEATURE_PREVPAGE   41  // "前ページ"
#define MULTIBTN_FEATURE_NEXTPAGE   42  // "後ページ"
#define MULTIBTN_FEATURE_NNSIEND    43  // "NNsi終了"
#define MULTIBTN_FEATURE_SETNGWORD1 44  // "NG設定"
#define MULTIBTN_FEATURE_SETNGWORD2 45  // "NG設定"
#define MULTIBTN_FEATURE_USER1TAB   46  // "ユーザ1タブ"
#define MULTIBTN_FEATURE_USER2TAB   47  // "ユーザ2タブ"
#define MULTIBTN_FEATURE_DEVICEINFO 48  // "デバイス情報"
#define MULTIBTN_FEATURE_GOTODUMMY  49  // "ダミー画面へ"
#define MULTIBTN_FEATURE_NEWTHREAD  50  // "スレ立て"
#define MULTIBTN_FEATURE_USER3TAB   51  // "ユーザ3タブ"
#define MULTIBTN_FEATURE_USER4TAB   52  // "ユーザ4タブ"
#define MULTIBTN_FEATURE_USER5TAB   53  // "ユーザ5タブ"
#define MULTIBTN_FEATURE_DIRSELECT  54  // "Dir選択"
#define MULTIBTN_FEATURE_TABINFO    55  // "タブ情報"
#define MULTIBTN_FEATURE_MOVELOGLOC 56  // "ログ管理変更"
#define MULTIBTN_FEATURE_MOVELOGDIR 57  // "Dir変更"
#define MULTIBTN_FEATURE_TO_NOTREAD 58  // "'未読あり'へ"
#define MULTIBTN_FEATURE_TO_GETALL  59  // "'取得済み全て'へ"
#define MULTIBTN_FEATURE_LOGCHARGE  60  // "参照ログ取得"
#define MULTIBTN_FEATURE_OPENURL    61  // "URLを取得"
#define MULTIBTN_FEATURE_SETNGWORD3 62  // "NG設定"
#define MULTIBTN_FEATURE_SETLOGCHRG 63  // "参照ログ取得設定"
#define MULTIBTN_FEATURE_BT_ON      64  // "Bt-On"
#define MULTIBTN_FEATURE_BT_OFF     65  // "Bt-Off"
#define MULTIBTN_FEATURE_TO_OFFLINE 66  // '参照ログ'へ 
#define MULTIBTN_FEATURE_TO_FAVOR   67  // 'お気に入り'へ
#define MULTIBTN_FEATURE_TO_USER1   68  // 'ユーザ1'へ
#define MULTIBTN_FEATURE_TO_USER2   69  // 'ユーザ2'へ
#define MULTIBTN_FEATURE_TO_USER3   70  // 'ユーザ3'へ
#define MULTIBTN_FEATURE_TO_USER4   71  // 'ユーザ4'へ
#define MULTIBTN_FEATURE_TO_USER5   72  // 'ユーザ5'へ
#define MULTIBTN_FEATURE_MACRO_EXEC 73  // MACRO実行
#define MULTIBTN_FEATURE_MACRO_VIEW 74  // MACRO表示
#define MULTIBTN_FEATURE_MACRO_LOG  75  // MACROログ表示
#define MULTIBTN_FEATURE_MACRO_SET  76  // MACRO設定
#define MULTIBTN_FEATURE_SELECT_BBS 77  // 板を選択
#define MULTIBTN_FEATURE_OYSTERLOGIN 78 // '●ログイン'
#define MULTIBTN_FEATURE_USAGE      79  // 操作ヘルプ
#define MULTIBTN_FEATURE_SETTOKEN   80  // 区切り設定
#define MULTIBTN_FEATURE_OPENGETLOGLIST 81 // 参照ログ取得一覧画面へ
#define MULTIBTN_FEATURE_SEARCHBBS  82     // 板を検索する
#define MULTIBTN_FEATURE_NEXTSEARCHBBS  83 // 板を次検索する
#define MULTIBTN_FEATURE_BEAMURL    84     // URLをBeamする
#define MULTIBTN_FEATURE_UPDATERESNUM 85   // 取得済みレス番号を更新する
#define MULTIBTN_FEATURE_OPENWEBDA  86     // webDAで開く
#define MULTIBTN_FEATURE_SELECT_MACRO_EXEC 87  // マクロ選択実行
#define MULTIBTN_FEATURE_BE_LOGIN   88     // Be@2chログイン
#define MULTIBTN_FEATURE_LIST_GETLOG 89    // スレ番予約一覧

#define HARDKEY_FEATURE_MASK         0x00ff
#define HARDKEY_FEATURE              0x0000

// ハードキーの制御設定
#define HARDKEY_FEATURE_DONOTHING    0  // NNsiで制御しない
#define HARDKEY_FEATURE_CONTROLNNSI  1  // NNsiで制御する
#define HARDKEY_FEATURE_TRANSJOGPUSH 2  // JOG PUSHと等価にする
#define HARDKEY_FEATURE_TRANSJOGBACK 3  // JOG BACKと等価にする

#define NNSH_BLOCKDISP_NOFMSG        5

#define NNSH_VFSDB_SETTING           0
#define NNSH_VFSDB_SUBJECT           1
#define NNSH_VFSDB_BBSLIST           2

#define NNSH_CONDITION_AND           0x0000  // AND条件
#define NNSH_CONDITION_OR            0x0001  // OR条件
#define NNSH_CONDITION_ALL           0x0003  // (全条件)

#define NNSH_CONDITION_NOTREADONLY   0x0004  // 参照ログは除外(ビット)

#define NNSH_CONDITION_LOGLOC_ALL    0x0018  // 
#define NNSH_CONDITION_LOGLOC_PALM   0x0008  // Palm内ログ
#define NNSH_CONDITION_LOGLOC_VFS    0x0010  // VFS内ログ

#define NNSH_CONDITION_GETERR_ALL    0x0060  // 
#define NNSH_CONDITION_GETERR_NONE   0x0020  //  エラーなし
#define NNSH_CONDITION_GETERR_ERROR  0x0040  //  取得エラー

#define NNSH_CONDITION_GETRSV_ALL    0x0180  // 
#define NNSH_CONDITION_GETRSV_RSV    0x0080  //  保留中スレ
#define NNSH_CONDITION_GETRSV_NONE   0x0100  //  非保留スレ

#define NNSH_THREADLEVEL_MASK        0x00ff  // スレレベルマスク
#define NNSH_THREADCOND_MASK         0x7f00  // スレ状態マスク
#define NNSH_THREADCOND_SHIFT        8       // スレ状態マスク

#define NNSH_THREADLEVEL_UPPER       0       // スレレベル ”以上”
#define NNSH_THREADLEVEL_LOWER       1       // スレレベル ”以下”
#define NNSH_THREADLEVEL_ELSE        2       // スレレベル ”以外”
#define NNSH_THREADLEVEL_MATCH       3       // スレレベル ”限定”

#define NNSH_BOARD_MATCH             0       // 板設定 ”内”
#define NNSH_BOARD_ELSE              1       // 板設定 ”以外”

// 重複スレのチェック方法
#define NNSH_DUP_CHECK_HIGH          1       // メモリを使うが高速版
#define NNSH_DUP_CHECK_SLOW          2       // メモリは使わないが低速版
#define NNSH_DUP_NOCHECK             3       // 重複チェックを行わない

#define NNSH_STRING_SELECTION        0x0100  // 
#define NNSH_STRING_SETMASK          0x00ff  // 
#define NNSH_STRING_OR               0x0001  // 
#define NNSH_STRING_CUSTOM2_DEFAULT1 "Palm"
#define NNSH_STRING_CUSTOM2_DEFAULT2 "palm"
#define NNSH_STRING_CUSTOM2_DEFAULT3 "CLIE"

#define NNSH_LEVEL_ERROR            0x0000
#define NNSH_LEVEL_WARN             0x0001
#define NNSH_LEVEL_INFO             0x0002
#define NNSH_LEVEL_CONFIRM          0x0004
#define NNSH_LEVEL_DEBUG            0x0010

#define NNSH_TITLE_HARDKEYSET_TITLE  "一覧画面ハードキー設定"
#define NNSH_HARDKEYCONTROL_TITLE    "ハードキー制御設定"
#define NNSH_TITLE_HARDKEYVIEW_TITLE "参照画面ハードキー設定"
#define NNSH_TITLE_GETLOGLIST_TITLE  "参照ログ一覧画面ハードキー設定"

#define MULTIVIEWBTN_FEATURE_MASK    0x00ff
#define MULTIVIEWBTN_FEATURE         0x7300

#define GETLOGBTN_FEATURE_MASK       0x00ff
#define GETLOGBTN_FEATURE            0x0000

#define UPBTN_VIEWFEATURE_DEFAULT    MULTIVIEWBTN_FEATURE_PAGEUP
#define DOWNBTN_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_PAGEDOWN

#define JOGPUSH_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_GETPART
#define JOGBACK_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_BACKTOLIST
#define CAPTURE_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_INFO

#define LEFTBTN_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_PREVIOUS
#define RIGHTBTN_VIEWFEATURE_DEFAULT MULTIVIEWBTN_FEATURE_NEXT

#define NNSH_DEFAULT_VIEW_LVLBTNFEATURE   MULTIVIEWBTN_FEATURE_INFO
#define NNSH_DEFAULT_VIEW_TITLESELFEATURE MULTIVIEWBTN_FEATURE_BACKTOLIST


// 参照画面の機能設定（機能番号）
//   ※ 設定用タイトルは、リソースで定義(そちらと同期してある必要がある)
#define MULTIVIEWBTN_FEATURE_INFO        0  // "スレ情報"
#define MULTIVIEWBTN_FEATURE_BACKTOLIST  1  // "一覧画面へ"
#define MULTIVIEWBTN_FEATURE_WRITE       2  // "書き込み"
#define MULTIVIEWBTN_FEATURE_GETPART     3  // "差分取得"
#define MULTIVIEWBTN_FEATURE_GETMESNUM   4  // "スレ番指定取得"
#define MULTIVIEWBTN_FEATURE_GOTOTOP     5  // "先頭MSGへ"
#define MULTIVIEWBTN_FEATURE_PREVIOUS    6  // "前MSGへ"
#define MULTIVIEWBTN_FEATURE_PAGEUP      7  // "前スクロール"
#define MULTIVIEWBTN_FEATURE_PAGEDOWN    8  // "後スクロール"
#define MULTIVIEWBTN_FEATURE_NEXT        9  // "次MSGへ"
#define MULTIVIEWBTN_FEATURE_GOTOBOTTOM 10  // "末尾MSGへ"
#define MULTIVIEWBTN_FEATURE_JUMP       11  // "Go"
#define MULTIVIEWBTN_FEATURE_RETURN     12  // "１つ戻る"
#define MULTIVIEWBTN_FEATURE_COPY       13  // "文字列コピー"
#define MULTIVIEWBTN_FEATURE_SELECTALL  14  // "文字列すべて選択"
#define MULTIVIEWBTN_FEATURE_OPENWEB    15  // "文字列をWeb"
#define MULTIVIEWBTN_FEATURE_SELANDWEB  16  // "選択+Web"
#define MULTIVIEWBTN_FEATURE_OUTPUTMEMO 17  // "メモ帳出力"
#define MULTIVIEWBTN_FEATURE_SEARCH     18  // "検索"
#define MULTIVIEWBTN_FEATURE_SEARCHNEXT 19  // "次検索"
#define MULTIVIEWBTN_FEATURE_SEARCHPREV 20  // "前検索"
#define MULTIVIEWBTN_FEATURE_SEARCHTOP  21  // "先頭から検索"
#define MULTIVIEWBTN_FEATURE_SEARCHBTM  22  // "末尾から検索"
#define MULTIVIEWBTN_FEATURE_CHANGEFONT 23  // "フォント変更"
#define MULTIVIEWBTN_FEATURE_DRAWMODE   24  // "描画モード変更"
#define MULTIVIEWBTN_FEATURE_ROTATE     25  // "画面の回転(HE)"
#define MULTIVIEWBTN_FEATURE_DISCONNECT 26  // "回線切断"
#define MULTIVIEWBTN_FEATURE_SETNETWORK 27  // "Network設定"
#define MULTIVIEWBTN_FEATURE_BOOKMARK   28  // "しおり設定"
#define MULTIVIEWBTN_FEATURE_DEVICEINFO 29  // "デバイス情報"
#define MULTIVIEWBTN_FEATURE_PAGEDOWN10 30  // "10レス後"
#define MULTIVIEWBTN_FEATURE_PAGEUP10   31  // "10レス前"
#define MULTIVIEWBTN_FEATURE_GOTODUMMY  32  // "ダミー画面へ"
#define MULTIVIEWBTN_FEATURE_OPENURL    33  // "リンクを開く"
#define MULTIVIEWBTN_FEATURE_EXECA2DA   34  // "A2DA起動"
#define MULTIVIEWBTN_FEATURE_BT_ON      35  // "Bt On"
#define MULTIVIEWBTN_FEATURE_BT_OFF     36  // "Bt Off"
#define MULTIVIEWBTN_FEATURE_USAGE      37  // "操作ヘルプ"
#define MULTIVIEWBTN_FEATURE_MEMOURL    38  // "MemoURLへ送る"
#define MULTIVIEWBTN_FEATURE_CHANGERAW  39  // "RAWモード切替"
#define MULTIVIEWBTN_FEATURE_CHGSEPA    40  // "区切り文字変更"
#define MULTIVIEWBTN_FEATURE_EDITSEPA   41  // "区切り文字編集"
#define MULTIVIEWBTN_FEATURE_OPENURL_HTTP 42 // "URLを開く(MSGから抽出)"
#define MULTIVIEWBTN_FEATURE_MEMOURLDA  43  // "MemoURLへ(DA経由で)送る"
#define MULTIVIEWBTN_FEATURE_OPENWEBDA  44  // "webDAで見る"
#define MULTIVIEWBTN_FEATURE_GETIMAGE   45  // "イメージ(ファイル)を取得"
#define MULTIVIEWBTN_FEATURE_DELETEBACK 46  // "削除後一覧へ戻る"
#define MULTIVIEWBTN_FEATURE_VIEWBEPROF 47  //　"Beプロフ参照"
#define MULTIVIEWBTN_FEATURE_LVLUPDATE  48  //  "スレレベル更新"
#define MULTIVIEWBTN_FEATURE_PICKUP_NG  49  //  "NG登録"
#define MULTIVIEWBTN_FEATURE_ENTRY_NG   50  //  "NG設定"
#define MULTIVIEWBTN_FEATURE_REPLYMSG   51  //  "引用カキコ"

// Notifyの更新処理
#define NNSH_VFS_UNMOUNTED              100
#define NNSH_VFS_UNMOUNTED_WRITE        101
#define NNSH_VFS_UNMOUNTED_MESSAGE      102
#define NNSH_VFS_UNMOUNTED_OTHER        103
#define NNSH_VFS_MOUNTED                110

#define NNSH_DISP_RESIZED               200


// NGチェックを使用する・しない
#define NNSH_USE_NGWORD1AND2            0x0001
#define NNSH_USE_NGWORD3                0x0010
#define NNSH_USE_NGWORD_EACHTHREAD      0x0020
#define NNSH_RESETMASK_NGWORD3          0xffef
#define NNSH_USE_NGWORD_CLEAREACHTHREAD 0xffdf

//
#define NNSJ_NNSIEXT_SELECTMACRO  "<NNsi:NNsiExt type=\"SelectMacro\"/>"

// a2DAとの連携機能用定義
#define A2DA_NNSIEXT_AADATASTART  "<NNsi:NNsiExt type=\"AA\">"
#define A2DA_NNSIEXT_ENDDATA      "</NNsi:NNsiExt>"
#define A2DA_NNSIEXT_SRC_STREAM   "<NNsi:Source>stream</NNsi:Source>"
#define A2DA_NNSIEXT_DATANAME     "<NNsi:Name>"
#define A2DA_NNSIEXT_ENDDATANAME  "</NNsi:Name>"
#define A2DA_NNSIEXT_NNSICREATOR  "<NNsi:Creator>NNsi</NNsi:Creator>"

#define nnDA_NNSIEXT_VIEWSTART    "<NNsi:NNsiExt type=\"VIEW\">"
#define nnDA_NNSIEXT_ENDVIEW      "</NNsi:NNsiExt>"
#define nnDA_NNSIEXT_INFONAME     "<NNsi:Info>"
#define nnDA_NNSIEXT_ENDINFONAME  "</NNsi:Info>"

#define nnDA_NNSIEXT_FILE         "FILE:"
#define nnDA_NNSIEXT_HELPLIST     "HELP:LIST"
#define nnDA_NNSIEXT_HELPVIEW     "HELP:VIEW"
#define nnDA_NNSIEXT_HELPFAVORTAB "HELP:FAVORTAB"
#define nnDA_NNSIEXT_HELPNNSISET  "HELP:NNSISET"
#define nnDA_NNSIEXT_HELPGETLOG   "HELP:GETLOG"
#define nnDA_NNSIEXT_FILERECVFILE "FILE:NNsi:recv.txt"
#define nnDA_NNSIEXT_FILESENDFILE "FILE:NNsi:send.txt"
#define nnDA_NNSIEXT_VIEWMEM      "MEM:"
#define nnDA_NNSIEXT_VIEWMEM_FOLD "FOLDMEM:"
#define nnDA_NNSIEXT_FOLDHTMLFILE "FOLDHTMLFILE:NNsi:"
#define nnDA_NNSIEXT_SHOWJPEG     "SHOWJPEG:"
#define nnDA_NNSIEXT_VFSFILE      "VFSFILE:"
#define nnDA_NNSIEXT_VFSFILE_FOLD "FOLDVFS:"

// NNsi間通信用定義
#define NNSI_EXCHANGEINFO_SUFFIX  "nsi"

// スレ番号
#define NNSI_NNSIEXT_THREADNUMBER_START "<NNsi:NNsiExt type=\"THREAD\">"
#define NNSI_NNSIEXT_THREADNUMBER_END   "</NNsi:NNsiExt>"

// URL
#define NNSI_NNSIEXT_URL_START          "<NNsi:NNsiExt type=\"URL\">"
#define NNSI_NNSIEXT_URL_END            "</NNsi:NNsiExt>"


// 参照ログ一覧画面の機能一覧
#define NNSH_GETLOG_FEATURE_NONE           0xffff   // 機能なし
#define NNSH_GETLOG_FEATURE_CLOSE_GETLOG        0   // 一覧画面へ
#define NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL    1   // 新着確認
#define NNSH_GETLOG_FEATURE_EDITITEM            2   // サイト詳細
#define NNSH_GETLOG_FEATURE_PREVITEM            3   // １つ上
#define NNSH_GETLOG_FEATURE_NEXTITEM            4   // １つ下
#define NNSH_GETLOG_FEATURE_PREVPAGE            5   // 前ページ
#define NNSH_GETLOG_FEATURE_NEXTPAGE            6   // 後ページ
#define NNSH_GETLOG_FEATURE_TOP                 7   // 先頭
#define NNSH_GETLOG_FEATURE_BOTTOM              8   // 末尾
#define NNSH_GETLOG_FEATURE_CREATELIST          9   // 一覧再構築
#define NNSH_GETLOG_FEATURE_EDIT_SEPARATOR     10   // 区切編集
#define NNSH_GETLOG_FEATURE_ENTRY_NEWSITE      11   // サイト登録
#define NNSH_GETLOG_FEATURE_OPEN_GETLEVEL      12   // Level変更
#define NNSH_GETLOG_FEATURE_REDRAW             13   // 再描画
#define NNSH_GETLOG_FEATURE_OPENWEB            14   // ブラウザで開く
#define NNSH_GETLOG_FEATURE_BT_ON              15   // Bt On
#define NNSH_GETLOG_FEATURE_BT_OFF             16   // Bt Off
#define NNSH_GETLOG_FEATURE_DISCONNECT         17   // 回線切断
#define NNSH_GETLOG_FEATURE_NETCONFIG          18   // Network設定
#define NNSH_GETLOG_FEATURE_DEVICEINFO         19   // デバイス情報
#define NNSH_GETLOG_FEATURE_OPEN_DUMMY         20   // ダミー画面へ
#define NNSH_GETLOG_FEATURE_OPEN_NNSISET       21   // NNsi設定画面へ
#define NNSH_GETLOG_FEATURE_SHOWVERSION        22   // バージョン情報
#define NNSH_GETLOG_FEATURE_HELP               23   // 操作説明
#define NNSH_GETLOG_FEATURE_NNSIEND            24   // NNsi終了
#define NNSH_GETLOG_FEATURE_OPENMENU           25   // 選択メニュー
#define NNSH_GETLOG_FEATURE_OPEN_NNSI          26   // NNsiで開く
#define NNSH_GETLOG_FEATURE_OPEN_MEMOURL       27   // MemoURLで開く
#define NNSH_GETLOG_FEATURE_OPEN_GETLOGMENU    28   // メニューを開く
#define NNSH_GETLOG_FEATURE_FORCE_DISCONNECT   29   // 回線切断-2
#define NNSH_GETLOG_FEATURE_CONFIG_GETLOG      30   // 参照ログ一覧設定
#define NNSH_GETLOG_FEATURE_SITE_INFORMATION   31   // サイト情報
#define NNSH_GETLOG_FEATURE_BEAM_URL           32   // URLをBEAMする
#define NNSH_GETLOG_FEATURE_OPENWEBDA          33   // URLをwebDAで開く
#define NNSH_GETLOG_FEATURE_DELETELOG          34   // ログを一括削除
#define NNSH_GETLOG_FEATURE_DELETELOG_READ     35   // 既読ログを一括削除

#define NNSH_GETLOG_NEWARRIVAL_LOG         0x8000   // 未読ログあり
#define NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK  0x7fff   // 未読ログをOFF

// SSLモード定義
#define NNSH_SSLMODE_NOTSSL             0
#define NNSH_SSLMODE_SSLV3              1
#define NNSH_SSLMODE_SSL_V2SPECIAL      2

#ifdef USE_SSL
#ifdef USE_SSL_V2

//
#define NNSHSSL_STATE_MASK             0x0FFF
#define NNSHSSL_STATE_CONNECT          0x1000
#define NNSHSSL_STATE_ACCEPT           0x2000
#define NNSHSSL_STATE_INIT        (NNSHSSL_STATE_CONNECT|NNSHSSL_STATE_ACCEPT)
#define NNSHSSL_STATE_BEFORE           0x4000

#define NNSHSSL_STATE_OK               0x03
#define NNSHSSL_STATE_RENEGOTIATE     (0x04|NNSHSSL_STATE_INIT)

/* Protocol Version Codes */
#define SSL2_VERSION                   0x0002
#define SSL2_VERSION_MAJOR             0x00
#define SSL2_VERSION_MINOR             0x02
/* #define SSL2_CLIENT_VERSION          0x0002 */
/* #define SSL2_SERVER_VERSION          0x0002 */

#define NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER  16383 /* 2^14-1 */

/* SSLv2 */
/* client */
#define SSL2_ST_SEND_CLIENT_HELLO            (0x10|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_GET_SERVER_HELLO             (0x20|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_SEND_CLIENT_MASTER_KEY       (0x30|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_CREATE_ENCRYPTION_KEY        (0x31|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_SEND_CLIENT_FINISHED         (0x40|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_GET_SERVER_VERIFY            (0x60|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_GET_SERVER_FINISHED          (0x70|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_START_ENCRYPTION             (0x80|NNSHSSL_STATE_CONNECT)

/* Protocol Message Codes */
#define SSL2_MT_ERROR                         0
#define SSL2_MT_CLIENT_HELLO                  1
#define SSL2_MT_CLIENT_MASTER_KEY             2
#define SSL2_MT_CLIENT_FINISHED               3
#define SSL2_MT_SERVER_HELLO                  4
#define SSL2_MT_SERVER_VERIFY                 5
#define SSL2_MT_SERVER_FINISHED               6
#define SSL2_MT_REQUEST_CERTIFICATE           7
#define SSL2_MT_CLIENT_CERTIFICATE            8

//#define SSL2_CHALLENGE_LENGTH                32
#define SSL2_CHALLENGE_LENGTH                16
#define SSL2_MD5_HASH_LENGTH                 16
#define SSL2_RWKEY_LENGTH                    SSL2_MD5_HASH_LENGTH
#define SSL2_MASTERKEY_LENGTH                16
// #define SSL2_MASTERKEY_LENGTH             128
// #define SSL2_SSL_SESSION_ID_LENGTH         16
#define SSL2_MAX_SSL_SESSION_ID_LENGTH       32
#endif  // #ifdef USE_SSL_V2
#endif  // #ifdef USE_SSL

#define NNSH_GETLOGMSG_VIEWTITLE "スレ番号指定取得一覧"
#define NNSH_GETLOGMSG_NONE      "(なし)"

#ifdef USE_MACRO

// ロギングレベル
#define NNSH_MACROLOGLEVEL_FATAL    0
#define NNSH_MACROLOGLEVEL_ERROR    1
#define NNSH_MACROLOGLEVEL_WARN     2
#define NNSH_MACROLOGLEVEL_INFO     3
#define NNSH_MACROLOGLEVEL_DEBUG    9

// opコード(リソースと一致する必要あり)
#define NNSH_MACROCMD_MACROEND 0   // マクロ終了
#define NNSH_MACROCMD_MOVE     1   // 代入
#define NNSH_MACROCMD_ADD      2   // 加算
#define NNSH_MACROCMD_SUB      3   // 減算
#define NNSH_MACROCMD_MESSAGE  4   // ログ出力
#define NNSH_MACROCMD_EXECUTE  5   // 機能の実行
#define NNSH_MACROCMD_CMP      6   // 数値の比較
#define NNSH_MACROCMD_JMP      7   // 無条件ジャンプ
#define NNSH_MACROCMD_JZ       8   // 一致した場合ジャンプ
#define NNSH_MACROCMD_JNZ      9   // 一致しなかった場合ジャンプ
#define NNSH_MACROCMD_BEEP    10   // ビープ音を鳴らす
#define NNSH_MACROCMD_OPERATE 11   // NNsi設定の値設定
#define NNSH_MACROCMD_BBSTYPE 12   // BBSタイプを代入
#define NNSH_MACROCMD_STRSTR  13   // 文字列が含まれるかどうか
#define NNSH_MACROCMD_STRCMP  14   // 文字列が一致するかどうか
#define NNSH_MACROCMD_SETTABNUM 15 // タブに含まれる数を代入
#define NNSH_MACROCMD_GETMESNUM 16 // スレに含まれるメッセージ数を代入
#define NNSH_MACROCMD_MESSTATUS 17 // スレ状態を代入
#define NNSH_MACROCMD_AND     18   // 論理積
#define NNSH_MACROCMD_OR      19   // 論理和
#define NNSH_MACROCMD_XOR     20   // 排他的論理和
#define NNSH_MACROCMD_IMPORT  21   // NNsi設定インポート
#define NNSH_MACROCMD_EXPORT  22   // NNsi設定エクスポート
#define NNSH_MACROCMD_STATUS  23   // NNsi設定の値取得
#define NNSH_MACROCMD_LAUNCHDA 24  // DA起動
#define NNSH_MACROCMD_CLIPCOPY  25  // クリップボードにコピー
#define NNSH_MACROCMD_CLIPADD   26  // クリップボードに追加
#define NNSH_MACROCMD_CLIPINSTR 27  // クリップボードの文字列に含まれるか確認
#define NNSH_MACROCMD_TITLECLIP 28  // スレタイトルをクリップボードにコピー
#define NNSH_MACROCMD_SET_FINDBBSSTR 29 // 板タブ検索用文字列の設定
#define NNSH_MACROCMD_SET_FINDTHREADSTR  30 // スレタイ検索用文字列の設定
#define NNSH_MACROCMD_UPDATE_RESNUM  31 // スレのレス番号を更新する(1.03)
#define NNSH_MACROCMD_JPL       32   // 値が正の場合にはジャンプ(1.03)
#define NNSH_MACROCMD_JMI       33   // 値が負の場合にはジャンプ(1.03)
#define NNSH_MACROCMD_OUTPUTTITLE 34 // スレタイトルをログ出力(1.03)
#define NNSH_MACROCMD_OUTPUTCLIP 35  // クリップボードの内容をログ出力(1.03)
#define NNSH_MACROCMD_REMARK    36   // 注釈文(何もしない) (1.03)
#define NNSH_MACROCMD_PUSH      37   // スタックにPUSH(1.04)
#define NNSH_MACROCMD_POP       38   // スタックにPOP(1.04)
#define NNSH_MACROCMD_GETRECNUM 39   // レコード番号を取得(1.04)
#define NNSH_MACROCMD_FINDTHREADSTRCOPY 40 // スレ検索文字列をクリップボードへ(1.04)
#define NNSH_MACROCMD_INPUTDIALOG  41      // 文字列を入力、クリップボードへコピー (1.05)
#define NNSH_MACROCMD_OUTPUTDIALOG  42     // 文字列を出力する (1.05)
#define NNSH_MACROCMD_CONFIRMDIALOG 43     // 確認ダイアログを表示する (1.05)
#define NNSH_MACROCMD_MESATTRIBUTE  44     // スレの属性を代入 (1.06)
#define NNSH_MACROCMD_JUMPLAST      45     // スレの最下行の場合にはジャンプする (1.06)


// データの操作関連指示(NNsi設定の変更とか)
#define NNSH_MACROOPCMD_DISABLE            0xffff  // (マクロ操作なし)
#define NNSH_MACROOPCMD_AUTOSTARTMACRO     0xfffe  // (マクロ自動実行SET)
#define NNSH_MACROOPCMD_NEWARRIVAL_LINE    0xfffd  // (新着確認後の回線切断)
#define NNSH_MACROOPCMD_NEWARRIVAL_DA      0xfffc  // (新着確認後のDA起動)
#define NNSH_MACROOPCMD_NEWARRIVAL_ROLOG   0xfffb  // (参照ログ取得レベル)
#define NNSH_MACROOPCMD_NEWARRIVAL_BEEP    0xfffa  // (新着確認後のビープ音)
#define NNSH_MACROOPCMD_COPY_AND_DELETE    0xfff9  // (参照ログにコピー時削除も)

#define NNSH_MACROCMD_VIEWTITLE  "マクロ内容"
#define NNSH_MACROLOG_VIEWTITLE  "マクロ実行結果"

#define MAX_HALT_OPERATION     100   // 機能実行の制限
#define MAX_LOGVIEW_BUFFER      80   //
#define MAX_LOGOUTPUT_BUFFER    72   //
#define MAX_FUNCCHAR_BUF        20   //
#define TEXTBUFSIZE            256

// マクロのレジスタ関連
#define NOF_REGISTER             16  // レジスタの数
#define NNSH_MACROOP_NOSPECIFY 0x00  // 指定なし
#define NNSH_MACROOP_IMMEDIATE 0x01  // 直値指定

#define NNSH_MACROOP_GR_OFFSET 0xe0  // レジスタオフセット
#define NNSH_MACROOP_GR0       0xe0  // GR0
#define NNSH_MACROOP_GR1       0xe1  // GR1
#define NNSH_MACROOP_GR2       0xe2  // GR2
#define NNSH_MACROOP_GR3       0xe3  // GR3
#define NNSH_MACROOP_GR4       0xe4  // GR4
#define NNSH_MACROOP_GR5       0xe5  // GR5
#define NNSH_MACROOP_GR6       0xe6  // GR6
#define NNSH_MACROOP_GR7       0xe7  // GR7
#define NNSH_MACROOP_GR8       0xe8  // GR8
#define NNSH_MACROOP_GR9       0xe9  // GR9
#define NNSH_MACROOP_GRa       0xea  // GRa
#define NNSH_MACROOP_GRb       0xeb  // GRb
#define NNSH_MACROOP_GRc       0xec  // GRc
#define NNSH_MACROOP_GRd       0xed  // GRd
#define NNSH_MACROOP_GRe       0xee  // GRe
#define NNSH_MACROOP_GRf       0xef  // GRf
#define NNSH_MACROOP_GR_MAX    0xef  // レジスタ最大値
#define NNSH_MACROOP_BUF0      0xf0  // テキストバッファ

#endif  // #ifdef USE_MACRO

#define  ICMP_ECHOREPLY       0
#define  ICMP_DEST_UNREACH    3
#define  ICMP_SOURCE_QUENCH   4
#define  ICMP_REDIRECT        5
#define  ICMP_ECHO            8
#define  ICMP_TIME_EXCEEDED  11
#define  ICMP_PARAMETERPROB  12
#define  ICMP_TIMESTAMP      13
#define  ICMP_TIMESTAMPREPLY 14
#define  ICMP_INFO_REQUEST   15
#define  ICMP_INFO_REPLY     16
#define  ICMP_MASKREQ        17
#define  ICMP_MASKREPLY      18
 
#define  ICMP_NET_UNREACH     0
#define  ICMP_HOST_UNREACH    1
#define  ICMP_PROT_UNREACH    2
#define  ICMP_PORT_UNREACH    3
#define  ICMP_FRAG_NEEDED     4
#define  ICMP_SR_FAILED       5
#define  ICMP_NET_UNKNOWN     6
#define  ICMP_HOST_UNKNOWN    7
#define  ICMP_HOST_ISOLATED   8
#define  ICMP_NET_UNR_TOS    11
#define  ICMP_HOST_UNR_TOS   12
#define  ICMP_PKT_FILTERED   13
#define  ICMP_PREC_VIOLATION 14
#define  ICMP_PREC_CUTOFF    15
