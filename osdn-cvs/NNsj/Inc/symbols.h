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
#define SOFT_CREATOR_ID       'NNsj'
#define DATA_CREATOR_ID       'NNsi'
#define SOFT_DBTYPE_ID        'Data'
#define DATA_DBTYPE_ID        'Mcro'

#define SOFT_PREFID           100
#define SOFT_PREFVERSION      100

#define NNSH_PREFERID         100
#define NNSH_PREFERTYPE       100

#define DBNAME_MACROSCRIPT     "Macro-NNsi"
#define DBVERSION_MACROSCRIPT  100

//////////////////////////////////////////////////////////////////////////////

#define DBNAME_SETTING        "NNsj-Setting"
#define DBVERSION_SETTING     100
#define DBSIZE_SETTING        32

//////////////////////////////////////////////////////////////////////////////
//
#define NNSJ_NNSIEXT_SELECTMACRO  "<NNsi:NNsiExt type=\"SelectMacro\"/>"

/** buffer size(small) **/
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12
#define MAX_URL                     80
#define MAX_FILELIST               256

#define NNSH_BUSYWIN_MAXLEN        32

/** 一時確保領域の幅(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4
#define FREE_SPACE_MARGIN         2048

/** 状態値 **/
#define NNSH_NOT_EFFECTIVE           0
#define NNSH_UPDATE_DISPLAY          1
#define NNSH_SILK_CONTROL            1

#define NNSH_DISABLE                 0
#define NNSH_ENABLE                  1


/** デバイス種 **/
#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1

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

#define FILEMGR_STATE_DISABLED      0
#define FILEMGR_STATE_OPENED_VFS    1
#define FILEMGR_STATE_OPENED_STREAM 2


/** VFSファイル名の最大サイズ  **/
#define MAXLENGTH_FILENAME          256

// for FILE NAMEs
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define DATAFILE_SUFFIX             ".dat"

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

#define NNSH_MODE_NORMAL            0
#define NNSH_MODE_INSERT            1
#define NNSH_MODE_DELETE            2

// ライン状態
#define NNSH_STEP_PAGEUP           -1
#define NNSH_STEP_PAGEDOWN          1
#define NNSH_STEP_REDRAW            0
#define NNSH_STEP_UPDATE           10
#define NNSH_STEP_TO_BOTTOM       100
#define NNSH_STEP_TO_TOP          NNSH_STEP_UPDATE

#define NNSH_DISP_NOTHING          0x0000   // なし
#define NNSH_DISP_UPPERLIMIT       0x0001   // 上限
#define NNSH_DISP_HALFWAY          0x0002   // 途中
#define NNSH_DISP_ALL              0x0004   // 全表示
#define NNSH_DISP_LOWERLIMIT       0x0008   // 下限


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
#define NNSH_MACROCMD_UPDATE_RESNUM  31 // スレのレス番号を更新する
#define NNSH_MACROCMD_JPL       32   // 値が正の場合にはジャンプ(1.03)
#define NNSH_MACROCMD_JMI       33   // 値が負の場合にはジャンプ(1.03)
#define NNSH_MACROCMD_OUTPUTTITLE 34 // スレタイトルをログ出力(1.03)
#define NNSH_MACROCMD_OUTPUTCLIP 35  // クリップボードの内容をログ出力(1.03)
#define NNSH_MACROCMD_REMARK    36   // 注釈文(何もしない) (1.03)
#define NNSH_MACROCMD_PUSH      37   // スタックにPUSH(1.04)
#define NNSH_MACROCMD_POP       38   // スタックにPOP(1.04)
#define NNSH_MACROCMD_GETRECNUM 39   // レコード番号を取得(1.04)
#define NNSH_MACROCMD_FINDTHREADSTRCOPY 40 // スレ検索文字列をクリップボードへ(1.04)
#define NNSH_MACROCMD_INPUTDIALOG   41     // 文字列を入力、クリップボードへコピー (1.05)
#define NNSH_MACROCMD_OUTPUTDIALOG  42     // 文字列を出力する (1.05)
#define NNSH_MACROCMD_CONFIRMDIALOG 43     // 確認ダイアログを表示する (1.05)
#define NNSH_MACROCMD_MESATTRIBUTE  44     // スレの属性を代入 (1.06)
#define NNSH_MACROCMD_JUMPLAST      45     // スレの最下行の場合にはジャンプする (1.06)


// データの操作関連指示(NNsi設定の変更とか)
#define NNSH_MACROOPCMD_DISABLE            0xffff  // (マクロ操作なし)
#define NNSH_MACROOPCMD_AUTOSTARTMACRO     0xfffe  // (マクロ自動実行SET)
#define NNSH_MACROOPCMD_NEWARRIVAL_LINE    0xfffd  // (新着確認後の回線切断)
#define NNSH_MACROOPCMD_NEWARRIVAL_DA      0xfffc  // (新着確認後のDA起動)
#define NNSH_MACROOPCMD_NEWARRIVAL_ROLOG   0xfffb  // (参照専用ログ取得レベル)
#define NNSH_MACROOPCMD_NEWARRIVAL_BEEP    0xfffa  // (新着確認後のビープ音)
#define NNSH_MACROOPCMD_COPY_AND_DELETE    0xfff9  // (参照ログにコピー時削除も)

#define NNSH_MACROCMD_VIEWTITLE  "マクロ内容"
#define NNSH_MACROLOG_VIEWTITLE  "マクロ実行結果"

#define MAX_MACRO_STEP         256   // 編集可能なマクロサイズの最大

#define MAX_HALT_OPERATION     100   // 機能実行の制限
#define MAX_LOGVIEW_BUFFER      96   //
#define MAX_LOGOUTPUT_BUFFER    80   //
#define MAX_FUNCCHAR_BUF        20   //
#define TEXTBUFSIZE            256

#define MAX_LOGRECLEN               40
#define MAX_STRLEN                  32

#define FILE_XML_NNSISET            "NNsiSet.xml"

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
