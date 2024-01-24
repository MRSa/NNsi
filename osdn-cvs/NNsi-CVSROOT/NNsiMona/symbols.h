/*============================================================================*
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for NNsh.
 *
 *===========================================================================*/
///// NNsiMona専用定義 { /////
#define DBSIZE_BBSLIST    140
#define DBSIZE_SUBJECT    136
#define DBSIZE_DIRINDEX    68
#define DBSIZE_MARGIN       4
///// } NNsiMona専用定義 /////
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
#define DBVERSION_SETTING     111
#define DBSIZE_SETTING        32

#define DBNAME_DIRINDEX       "DirIndex-NNsi"
#define DBVERSION_DIRINDEX    100

//////////////////////////////////////////////////////////////////////////////

#define RSCID_createThreadIndexSub_ARM     1     // ARMlet関数番号その１
#define RSCID_parseMessage_ARM             2     // ARMlet関数番号その２

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define TINYBUF                      8
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12
#define JUMPBUF                     16

/** 一時確保領域の幅(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4

/** 書き込み時に確保するバッファサイズ **/
#define NNSH_WRITEBUF_MIN         4096
#define NNSH_WRITEBUF_MAX        61000
#define NNSH_WRITEBUF_DEFAULT     6144

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
#define URL_BBSTABLE_OLD         "http://www6.ocn.ne.jp/~mirv/bbstable.html"
#define URL_BBSTABLE             "http://www.ff.iij4u.or.jp/~ch2/bbstable.html"

#define URL_PREFIX                     "test/read.cgi/"
#define URL_PREFIX_IMODE               "test/r.i/"
#define URL_PREFIX_MACHIBBS            "bbs/read.pl?BBS="
#define URL_PREFIX_MACHIBBS_IMODE      "bbs/read.pl?IMODE=TRUE&BBS="
#define URL_PREFIX_SHITARABA           "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABA_IMODE     "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABAJBBS       "bbs/read.cgi?BBS="
#define URL_PREFIX_SHITARABAJBBS_IMODE "bbs/i.cgi?BBS="
#define FILE_THREADLIST                "subject.txt"

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont

#define NNSH_NOTENTRY_THREAD        0xffff
#define NNSH_NOTENTRY_BBS           0xffff

/** OFFLINE LOG(for GIKO Shippo) **/
#define OFFLINE_THREAD_NAME         "参照専用ログ"
#define OFFLINE_THREAD_NICK         "!GikoShippo/"
#define OFFLINE_THREAD_URL          "file://"

#define FAVORITE_THREAD_NAME        "お気に入り"
#define GETALL_THREAD_NAME          "取得済み全て"
#define NOTREAD_THREAD_NAME         "未読あり"
#define CUSTOM1_THREAD_NAME         "取得エラー"
#define CUSTOM2_THREAD_NAME         "Palm関連"

#define NNSH_NOF_SPECIAL_BBS        6   // 特殊な板名の数
#define NNSH_SELBBS_FAVORITE        0   // お気に入りスレ
#define NNSH_SELBBS_GETALL          1   // 取得済み全て
#define NNSH_SELBBS_NOTREAD         2   // 未読あり
#define NNSH_SELBBS_CUSTOM1         3   // CUSTOM1
#define NNSH_SELBBS_CUSTOM2         4   // CUSTOM2
#define NNSH_SELBBS_OFFLINE         5   // OFFLINEスレ

// for NaNaShi FILE NAMEs
#define FILE_WRITEMSG               "write.txt"
#define FILE_WRITENAME              "writeName.txt"
#define FILE_WRITEMAIL              "writeMail.txt"
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define LOGDATAFILE_PREFIX          "/PALM/Programs/2ch/"
#define LOGDIR_FILE                 "/PALM/Programs/2ch/logdir.txt"
#define DATAFILE_SUFFIX             ".dat"

//  time diference between from 'Jan/01/1904' to 'Jan/01/1970'.
#define TIME_CONVERT_1904_1970      (2082844800 - 32400)
#define TIME_CONVERT_1904_1970_UTC  (2082844800)

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002
#define NNSH_VFS_USEOFFLINE         0x0004
#define NNSH_VFS_DBBACKUP           0x0008
#define NNSH_VFS_DBIMPORT           0x0010

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
#define NNSH_BBSTYPE_SHITARABA      0x02         // したらばBBS
#define NNSH_BBSTYPE_SHITARABAJBBS  0x03         // したらば@JBBS
#define NNSH_BBSTYPE_ERROR          0xff         // ボードタイプエラー

/** 一覧表示がBBS一覧かお気に入りか **/
#define NNSH_FAVORLIST_MODE         1
#define NNSH_BBSLIST_MODE           0 


// データベースのキータイプ(数字か文字列か)
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2
#define NNSH_KEYTYPE_SUBJECT        3

// HTTPメッセージ作成時に指定
#define HTTP_SENDTYPE_GET                10
#define HTTP_SENDTYPE_GET_NOTMONA        11
#define HTTP_SENDTYPE_POST               20
#define HTTP_SENDTYPE_POST_MACHIBBS      40
#define HTTP_SENDTYPE_POST_SHITARABAJBBS 80
#define HTTP_SENDTYPE_POST_SHITARABA     90
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
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

#define NNSH_ATTRLABEL_TITLE       ", Level:" 
#define NNSH_ATTRLABEL_NOTFAVOR    "-"    // 「お気に入り」ではない
#define NNSH_ATTRLABEL_FAVOR_L1    "L"    // 「お気に入り」レベル(低)
#define NNSH_ATTRLABEL_FAVOR_L2    "1"    // 「お気に入り」レベル(中低)
#define NNSH_ATTRLABEL_FAVOR_L3    "2"    // 「お気に入り」レベル(中)
#define NNSH_ATTRLABEL_FAVOR_L4    "3"    // 「お気に入り」レベル(中高)
#define NNSH_ATTRLABEL_FAVOR       "H"    // 「お気に入り」レベル(高)


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
#define NNSH_SUBJSTATUS_OVER         8

#define NNSH_MSGSTATE_NOTMODIFIED   10
#define NNSH_MSGSTATE_ERROR         20

#define NNSH_ERRORCODE_PARTGET      (~errNone - 12)

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


#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01

#define NNSH_FIELD_SEPARATOR        "<>"
#define NNSH_FIELD_SEPARATOR_MACHI  ","
#define NNSH_RECORD_SEPARATOR       "\x0a"
#define NNSH_SUBJTITLE_END          "("

#define NNSH_JBBSTOKEN_START        "\x0a<dt>"
#define NNSH_JBBSTOKEN_END          "<br><br>\x0a"
#define NNSH_JBBSTOKEN_MSGSTART     "\xcc\xbe\xc1\xb0\xa1\xa7" // 名前：
#define NNSH_MACHITOKEN_START       "\x0d\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a\x0d\x0a"
#define NNSH_MACHITOKEN_MSGSTART    "名前："
#define NNSH_MACHINAME_SUFFIX       "[まちBBS]"
#define NNSH_SHITARABA_SUFFIX       "[したらば]"
#define NNSH_SHITARABAJBBS_SUFFIX   "[したらば@JBBS]"

/** **/
#define NNSH_SYMBOL_WRITE_DONE      "書きこみました。"
#define NNSH_SYMBOL_WRITE_NG        "ＥＲＲＯＲ"

#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1

#define NNSH_JUMPMSG_LEN            10
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

#define NNSH_NOF_JUMPMSG_EXT        12
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

#define NNSH_JUMPMSG_OPENBBS        "板を選択"
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
#define NNSH_ITEMLEN_JUMPLIST       14      // リストアイテム文字列長
#define NNSH_JUMPSEL_NUMLIST        14      // リストアイテム数
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

#define NNSH_STEP_PAGEUP           -1
#define NNSH_STEP_PAGEDOWN          1
#define NNSH_STEP_REDRAW            0
#define NNSH_STEP_UPDATE           10

#define NNSH_ITEM_LASTITEM         0xffff

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

#define NNSH_LIMIT_COPYBUFFER      2047
#define NNSH_LIMIT_MEMOPAD         4000
#define NNSH_LIMIT_RETRY           5
#define NNSH_MEMOBUFFER_MERGIN     96

#define NNSH_BUSYWIN_MAXLEN        32

#define NNSH_WRITE_DELAYTIME       7

#define NNSH_INPUTWIN_SEARCH       "TITLE検索"
#define NNSH_INPUTWIN_GETMESNUM    "スレ取得(スレ番指定)"
#define NNSH_INPUTWIN_MODIFYTITLE  "スレタイトル変更"
#define NNSH_INPUTWIN_NGWORD1      "NG文字列１の登録"
#define NNSH_INPUTWIN_NGWORD2      "NG文字列２の登録"

#define NNSH_DIALOG_USE_SEARCH      10
#define NNSH_DIALOG_USE_GETMESNUM   20
#define NNSH_DIALOG_USE_MODIFYTITLE 40
#define NNSH_DIALOG_USE_SEARCH_MESSAGE 60
#define NNSH_DIALOG_USE_SETNGWORD   80



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

// 多目的ボタンの初期設定値
#define NNSH_SWITCH1USAGE_DEFAULT   NNSH_SWITCHUSAGE_GETPART    // スレ取得モード
#define NNSH_SWITCH2USAGE_DEFAULT   NNSH_SWITCHUSAGE_USEPROXY   // Proxy経由でのアクセス

#define NNSH_SWITCHMSG_NOUSE        "(使用しない)"   // 多目的スイッチを使用しない
#define NNSH_SWITCHMSG_GETPART      "分割/一括"      // スレ取得モード(一括/分割)
#define NNSH_SWITCHMSG_HANZEN       "半角カナ変換"   // 半角カナ→全角カナ変換
#define NNSH_SWITCHMSG_USEPROXY     "Proxy使用"      // Proxy経由でアクセス
#define NNSH_SWITCHMSG_CONFIRM      "確認を省略"     // 確認ダイアログの省略

#define NNSH_SWITCHMSG_SUBJECT      "一覧全取得"     // スレ一覧全取得切り替え
#define NNSH_SWITCHMSG_TITLEDISP    "描画モード"     // スレ一覧描画モード切替
#define NNSH_SWITCHMSG_MSGNUMBER    "一覧に番号"     // スレ一覧にMSG番号
#define NNSH_SWITCHMSG_GETRESERVE   "取得保留"       // スレ取得保留
#define NNSH_SWITCHMSG_IMODEURL     "i-mode URL"     // i-mode URL使用
#define NNSH_SWITCHMSG_BLOCKDISP    "ゾーン表示"     // ゾーン表示モード

#define NNSH_SUBJSTATUSMSG_NOT_YET  "未取得"
#define NNSH_SUBJSTATUSMSG_NEW      "新規取得"
#define NNSH_SUBJSTATUSMSG_UPDATE   "更新取得"
#define NNSH_SUBJSTATUSMSG_REMAIN   "未読あり"
#define NNSH_SUBJSTATUSMSG_ALREADY  "既読"
#define NNSH_SUBJSTATUSMSG_UNKNOWN  "状態不明"

/** スレ内検索 **/
#define MAX_SEARCH_STRING 64

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
#define NNSH_FRMID_THREAD        0x1200
#define NNSH_FRMID_MESSAGE       0x1201


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
#define JOGBACK_FEATURE_DEFAULT      MULTIBTN_FEATURE_INFO

#define MULTIBTN_FEATURE_MASK        0x00ff
#define MULTIBTN_FEATURE             0x7600

// この定義は、リソースのリスト(のかかれている順番)と１対１対応が必要！
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
#define MULTIBTN_FEATURE_COPYGIKO   17  // "参照専用へCOPY"
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
#define MULTIBTN_FEATURE_NNSHSET1   28  // "NNsi設定"
#define MULTIBTN_FEATURE_NNSHSET2   29  // "NNsi設定-2"
#define MULTIBTN_FEATURE_NNSHSET3   30  // "NNsi設定-3"
#define MULTIBTN_FEATURE_NNSHSET4   31  // "NNsi設定-4"
#define MULTIBTN_FEATURE_NNSHSET5   32  // "NNsi設定-5"
#define MULTIBTN_FEATURE_NNSHSET6   33  // "NNsi設定-6"
#define MULTIBTN_FEATURE_NNSHSET7   34  // "NNsi設定-7"
#define MULTIBTN_FEATURE_VERSION    35  // "バージョン"
#define MULTIBTN_FEATURE_SELECTNEXT 36  // "１つ下"
#define MULTIBTN_FEATURE_SELECTPREV 37  // "１つ上"
#define MULTIBTN_FEATURE_PREVPAGE   38  // "前ページ"
#define MULTIBTN_FEATURE_NEXTPAGE   39  // "後ページ"
#define MULTIBTN_FEATURE_NNSIEND    40  // "NNsi終了"
#define MULTIBTN_FEATURE_NGWORD1    41  // "NG1設定"
#define MULTIBTN_FEATURE_NGWORD2    42  // "NG2設定"
#define MULTIBTN_FEATURE_USER1TAB   43  // "ユーザ1タブ"
#define MULTIBTN_FEATURE_USER2TAB   44  // "ユーザ2タブ"

#define NNSH_BLOCKDISP_NOFMSG        5

#define NNSH_VFSDB_SETTING           0
#define NNSH_VFSDB_SUBJECT           1
#define NNSH_VFSDB_BBSLIST           2

#define NNSH_CONDITION_AND           0x0000  // AND条件
#define NNSH_CONDITION_OR            0x0001  // OR条件
#define NNSH_CONDITION_ALL           0x0003  // (全条件)

#define NNSH_CONDITION_NOTREADONLY   0x0004  // 参照専用ログは除外(ビット)

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
