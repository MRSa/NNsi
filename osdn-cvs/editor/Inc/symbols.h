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
#define SOFT_CREATOR_ID       'KnEd'
#define SOFT_DBTYPE_ID        'Data'

#define SOFT_PREFID           100
#define SOFT_PREFVERSION      100

#define NNSH_PREFERID         100
#define NNSH_PREFERTYPE       100

#define FTRID_PARAMPTR          1
#define FTRID_GLOBALPTR         2

//////////////////////////////////////////////////////////////////////////////

#define DBNAME_SETTING        "KnEd-Prefs"
#define DBVERSION_SETTING     101
#define DBSIZE_SETTING         32

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12
#define MAX_URL                     80
#define MAX_FILELIST               256

#define NNSH_BUSYWIN_MAXLEN        32

/** 一時確保領域の幅(5120 < x < 63000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         63000
#define NNSH_WORKBUF_DEFAULT     30500
#define MARGIN                       4
#define FREE_SPACE_MARGIN         2048

#define NNSH_WORKBUF_FTR             1

/** 状態値 **/
#define NNSH_NOT_EFFECTIVE           0
#define NNSH_UPDATE_DISPLAY          1
#define NNSH_SILK_CONTROL            1

#define NNSH_DISABLE                 0
#define NNSH_ENABLE                  1


/** デバイス種 **/
#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1
#define NNSH_DEVICE_DIASUPPORT      2
#define NNSH_DEVICE_GARMIN          3

/** VFS Media type **/
#define NNSH_VFS_USEMEDIA_ANY       0
#define NNSH_VFS_USEMEDIA_CF        1
#define NNSH_VFS_USEMEDIA_MS        2
#define NNSH_VFS_USEMEDIA_SD        3
#define NNSH_VFS_USEMEDIA_SM        4
#define NNSH_VFS_USEMEDIA_RD        5
#define NNSH_VFS_USEMEDIA_DoC       6

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002

// COPY type
#define NNSH_COPY_PALM_TO_VFS       0
#define NNSH_COPY_VFS_TO_VFS        1
#define NNSH_COPY_VFS_TO_PALM       2
#define NNSH_COPY_PALM_TO_PALM      4

#define NNSH_FILEMODE_CREATE        1
#define NNSH_FILEMODE_READWRITE     2
#define NNSH_FILEMODE_READONLY      4
#define NNSH_FILEMODE_TEMPFILE      8
#define NNSH_FILEMODE_APPEND        16

#define FILEMGR_STATE_DISABLED               0
#define FILEMGR_STATE_OPENED_VFS             1
#define FILEMGR_STATE_OPENED_STREAM          2
#define FILEMGR_STATE_OPENED_STREAM_READONLY 3
#define FILEMGR_STATE_OPENED_VFS_DOS         4
#define FILEMGR_STATE_OPENED_VFS_MAC         5
#define FILEMGR_STATE_OPENED_VFS_JIS         6
#define FILEMGR_STATE_OPENED_VFS_EUC         7
#define FILEMGR_STATE_OPENED_VFS_UTF8        8
#define FILEMGR_STATE_OPENED_VFS_READONLY    9


/** VFSファイル名の最大サイズ  **/
#define MAXLENGTH_FILENAME          256

// for FILE NAMEs
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define FILE_TEMPFILE               "#KnEd-Edit.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/Text/"
#define DATAFILE_SUFFIX             ".txt"
#define BACKFILE_SUFFIX             ".bak"

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont

/** データベースのキータイプ(数字か文字列か) **/
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2

/** HTTPメッセージ作成時に指定 **/
#define HTTP_SENDTYPE_GET                10
#define HTTP_SENDTYPE_GET_NOTMONA        11
#define HTTP_SENDTYPE_POST               20
#define HTTP_SENDTYPE_POST_OTHERBBS      30
#define HTTP_SENDTYPE_POST_MACHIBBS      40
#define HTTP_SENDTYPE_POST_SHITARABAJBBS 80
#define HTTP_SENDTYPE_POST_SHITARABA     90
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETSUBJECT_START       0
#define HTTP_GETSUBJECT_PART        0

/** 検索方向 **/
#define NNSH_SEARCH_FORWARD         1
#define NNSH_SEARCH_BACKWARD       -1

/** フォームID(内部管理用) **/
#define NNSH_FRMID_MAIN             10
#define NNSH_FRMID_CONFIG_NNSH      20

/** ダイアログ表示レベル **/
#define NNSH_LEVEL_ERROR            0x0000
#define NNSH_LEVEL_WARN             0x0001
#define NNSH_LEVEL_INFO             0x0002
#define NNSH_LEVEL_CONFIRM          0x0004
#define NNSH_LEVEL_DEBUG            0x0010


// ダイアログを省略する設定
#define NNSH_OMITDIALOG_NOTHING      0x00
#define NNSH_OMITDIALOG_CONFIRM      0x01
#define NNSH_OMITDIALOG_WARNING      0x02
#define NNSH_OMITDIALOG_INFORMATION  0x04
#define NNSH_OMITDIALOG_ALLOMIT      ((NNSH_OMITDIALOG_CONFIRM)|(NNSH_OMITDIALOG_WARNING)|(NNSH_OMITDIALOG_INFORMATION))

#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)

#define NNSH_NGSET_SEPARATOR        ','

// VFS状態(変更)値
#define NNSH_VFS_UNMOUNTED   1
#define NNSH_VFS_MOUNTED     2

#define NNSH_DISP_RESIZED               200

#define NNSH_KANJI_SHIFTJIS  0
#define NNSH_KANJI_EUC       1
#define NNSH_KANJI_JIS       2
#define NNSH_KANJI_UTF8      3

#define NNSH_LINECODE_LF     0
#define NNSH_LINECODE_CR     1
#define NNSH_LINECODE_CRLF   2

// ジョグダイヤル設定用
#define NUMBER_OF_JOGMASKS   2
#define NUMBER_OF_JOGDATAS   6
