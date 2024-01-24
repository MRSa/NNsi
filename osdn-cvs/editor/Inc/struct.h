/*============================================================================*
 *  FILE: 
 *     struct.h
 *
 *  Description: 
 *     構造体定義
 *
 *===========================================================================*/

/********** My Local structures **********/
#ifdef STRUCT_H
#else
#define STRUCT_H

// Sony FileDB Header
typedef struct sonyFileDBHeader {
    UInt32  headerID;
    UInt32  fileAttr;
    UInt32  LADate;
    UInt16  ExAttr;
    UInt16  Reserved;
    Char    Filename[256];
} FileDBHeader;

// ファイル参照子
typedef struct {
  UInt16   fileLocation;
  FileRef  vfsRef;
  FileHand streamRef;
} NNshFileRef;

// 文字列(カンマ区切り)のリスト構造
typedef struct
{
    UInt16    nofWord;        // 文字列(リスト)数
    Char     *wordString;     // 文字列の領域(リスト形式)
    MemHandle wordmemH;       // 文字列リストデータ
} NNshWordList;

// DB の情報取得用
typedef struct {
    Char *nameP;
    UInt16 *attributesP;
    UInt16 *versionP;
    UInt32 *crDateP;
    UInt32 *modDateP;
    UInt32 *bckUpDateP;
    UInt32 *modNumP;
    LocalID *appInfoIDP;
    LocalID *sortInfoIDP;
    UInt32 *typeP;
    UInt32 *creatorP;
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
    Char  up;              // ハードキー上
    Char  down;            // ハードキー下
    Char  key1;            // ハードキー１
    Char  key2;            // ハードキー２
    Char  key3;            // ハードキー３
    Char  key4;            // ハードキー４
} NNshHardkeyControl;

// システム情報(起動時にデータベースから読み込む情報、、、増えすぎ...)
typedef struct {
    /** NNsi設定 **/
    UInt16  confirmationDisable; // 確認ダイアログを省略する
    UInt16  debugMessageON;      // デバッグメッセージの表示
    UInt32  bufferSize;          // ワーク領域として確保するバッファサイズ
    UInt16  useVFS;              // VFSシステムの使用可否設定
    UInt16  vfsUseCompactFlash;  // VFSはＣＦを使用する
    UInt16  currentFont;         // フォントID(Starndard用)
    UInt16  useARMlet;           // PalmOS5用機能を使用
    UInt16  sonyHRFont;          // (Sony HR用)TinyフォントID
    UInt16  disableSonyHR;       // SONYハイレゾ描画モードを使用しない
    UInt16  useEndSaveDialog;    // 終了時に保存ダイアログ
    UInt16  autoRestore;         // 前回の編集ファイルを復帰
    UInt16  convertHanZen;       // 半角カナ→全角カナ変換
    UInt16  useHardKeyControl;   // ハードキー制御を実施する
    UInt16  notAutoFocus;        // フォーカスを自動設定しない
    UInt16  silkMax;             // シルクエリアを拡大する
    UInt16  notUseTsPatch;       // TsPatchを使用しない
    NNshHardkeyControl useKey;   // ハードキーのキー番号格納領域
    Char    fileName[MAXLENGTH_FILENAME + MARGIN];  // ファイル名記憶領域
    UInt16  fileLocation;        // ファイルのありか
    UInt16  kanjiCode;           // ファイルの漢字コード
    UInt16  newLineCode;         // ファイルの改行コード
} NNshSavedPref;

typedef struct
{
   UInt16 utf8;
   UInt16 sj;
} NNshUTF8ConvTable;

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

//  NNsi全体で使用するグローバル領域
typedef struct
{
    UInt16             device;
    UInt16             vfsVol;
    FormType          *prevBusyForm;
#ifdef USE_CLIE
    UInt16   updateHR;            // SILKサイズ変更、画面表示を更新する
    UInt16   hrRef;               // SONY HR参照番号
    UInt16   hrVer;               // Sony HRのバージョン番号
    UInt16   silkRef;             // CLIE用SILKマネージャの参照番号(Temp)
    UInt16   silkVer;             // CLIE用SILKマネージャのバージョン番号(Temp)
    UInt16   prevHRWidth;         // CLIEハイレゾ表示用(画面モード記憶用)
    UInt16  *jogInfoBack;         // SONYジョグ情報バックアップ
    UInt16 **jogAssistSetBack;    // SONYジョグアシストバックアップ
    UInt16   tempStatus;          // 
    UInt16   notUseHR;            // HR制御を行わない
    UInt16   notUseSilk;          // Silk制御を行わない
    UInt16   sonyJogMask[NUMBER_OF_JOGMASKS * 2 + NUMBER_OF_JOGDATAS]; // JOG情報
#endif      // #ifdef USE_CLIE
    EventType dummyEvent;         // イベント生成用バッファ
    Char      dummyBuffer[MARGIN];
    UInt32   palmOSVersion;       // PalmOSのバージョン番号
    UInt16   jumpSelection;       // ジャンプ番号
    UInt32   os5HighDensity;      // OS5高解像度
    UInt32   browserCreator;      // WebブラウザクリエータID
    UInt16   browserLaunchCode;   // Webブラウザ起動コード
    UInt16   notifyUpdate;        // VFS状態通知
    UInt16   autoOpenFile;        // ファイルを自動オープンする
    Char     dirName[MAXLENGTH_FILENAME + MARGIN];  // ディレクトリ名記憶領域
    MemHandle fileListH;          // ファイル名一覧
    DmOpenRef unicode2jisDB;      // UTF8 -> JIS 変換テーブル
    DmOpenRef jis2unicodeDB;      // JIS  -> UTF8変換テーブル
} NNshWorkingInfo;

// kaniEdit用起動コード定義
typedef enum {
    kaniEditCmdOpenFileStreamReadOnly = sysAppLaunchCmdCustomBase, // 0x8000
    kaniEditCmdOpenFileStream,                                     // 0x8001
    kaniEditCmdOpenVFSreadOnly,                                    // 0x8002
    kaniEditCmdOpenVFS,                                            // 0x8003
    kaniEditCmdOpenVFSdos,                                         // 0x8004
    kaniEditCmdOpenVFSmac,                                         // 0x8005
    kaniEditCmdOpenVFSjis,                                         // 0x8006
    kaniEditCmdOpenVFSeuc,                                         // 0x8007
    kaniEditCmdOpenVFSutf8                                         // 0x8008
} MyAppCustomActionCodes;

#endif
