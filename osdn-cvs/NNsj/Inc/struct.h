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
    UInt16  bufferSize;          // ワーク領域として確保するバッファサイズ
    Int32   netTimeout;          // 通信タイムアウト値
    UInt16  nofRetry;            // 通信リトライ回数
    UInt16  useVFS;              // VFSシステムの使用可否設定
    UInt16  vfsUseCompactFlash;  // VFSはＣＦを使用する
    UInt16  lastFrmID;           // 終了時に表示していた画面
    UInt16  vfsOnAutomatic;      // VFS使用可能時に自動的にVFS ON
    UInt16  currentFont;         // フォントID(Starndard用)
    UInt16  useARMlet;           // PalmOS5用機能を使用
    UInt16  sonyHRFont;          // (Sony HR用)TinyフォントID
    UInt16  disableSonyHR;       // SONYハイレゾ描画モードを使用しない
    UInt16  useSonyTinyFont;     // (Sony HR用)Tinyフォントを使用する
    UInt16  useProxy;            // Proxy経由でのアクセス
    UInt16  redrawAfterConnect;  // 接続後画面を再描画する
    Char    fileName[MAXLENGTH_FILENAME + MARGIN];  // ファイル名記憶領域
    UInt16  convertHanZen;       // 半角カナ→全角カナ変換
    Char    proxyURL[MAX_URL];   // Proxyのホスト名
    UInt16  proxyPort;           // Proxyのポート番号
    UInt16  useHardKeyControl;   // ハードキー制御を実施する
    NNshHardkeyControl useKey;   // ハードキーのキー番号格納領域
} NNshSavedPref;


typedef struct {
    UInt32  dateTime;
    UInt8   logLevel;
    UInt8   reserved0;
    Char    logData[MAX_LOGRECLEN];
    UInt16  errCode;
} NNshMacroResult;

typedef struct {
    UInt16 operator;
    Char   data[MAX_STRLEN];
} NNshMacroString;

typedef struct {
    UInt16 operator;
    UInt16 subCommand;
    UInt16 data1;
    UInt16 data2;
} NNshMacroNumber;

// マクロDBのレコード構造
typedef struct {
    UInt16  seqNum;
    UInt16  opCode;
    UInt8   dst;
    UInt8   src;
    union {
       NNshMacroString strData;    // 文字データのとき
       NNshMacroNumber numData;    // 数値データのとき
    } MacroData;
} NNshMacroRecord;

//  NNsi全体で使用するグローバル領域
typedef struct
{
    UInt16             device;
    UInt16             vfsVol;
    UInt16             netRef;
    FormType          *prevBusyForm;
    NNshWordList      *featureList;       // 機能設定ラベル
    NNshMacroRecord   *dataRec;           // マクロデータのレコード
#ifdef USE_CLIE
    UInt16   updateHR;            // SILKサイズ変更、画面表示を更新する
    UInt16   hrRef;               // SONY HR参照番号
    UInt16   hrVer;               // Sony HRのバージョン番号
    UInt16   silkRef;             // CLIE用SILKマネージャの参照番号(Temp)
    UInt16   silkVer;             // CLIE用SILKマネージャのバージョン番号(Temp)
    UInt16   prevHRWidth;         // CLIEハイレゾ表示用(画面モード記憶用)
    UInt16   totalLine;           // CLIEハイレゾ表示用(全行数)
    UInt16   nofPage;             // CLIEハイレゾ表示用(全ページ数)
    UInt16   currentPage;         // CLIEハイレゾ表示用(表示中ページ)
    UInt16   prevHRLines;         // CLIEハイレゾ表示用(表示可能行数の記憶域)
    UInt16  *jogInfoBack;         // SONYジョグ情報バックアップ
    UInt16 **jogAssistSetBack;    // SONYジョグアシストバックアップ
    UInt16   tempStatus;          // 
    UInt16   notUseHR;            // HR制御を行わない
    UInt16   notUseSilk;          // Silk制御を行わない

#endif      // #ifdef USE_CLIE
    EventType dummyEvent;         // イベント生成用バッファ
    Char      dummyBuffer[MARGIN];
    UInt32   palmOSVersion;       // PalmOSのバージョン番号
    MemHandle fileListH;          // ファイル名一覧
    UInt16   jumpSelection;       // ジャンプ番号
    UInt32   os5HighDensity;      // OS5高解像度
    UInt32   browserCreator;      // WebブラウザクリエータID
    UInt16   browserLaunchCode;   // Webブラウザ起動コード
    UInt16   notifyUpdate;        // VFS状態通知
    UInt16   maxScriptLine;       // スクリプトデータ格納サイズの最大
    UInt16   currentScriptLine;   // スクリプトデータのライン数
    Char    *scriptArea;          // スクリプトデータ格納領域
    UInt16   pageTopLine;         // スクリプトデータページTop(ゼロスタート)
    UInt16   currentCursor;       // スクリプトデータの現在参照しているデータ位置
    Boolean  editFlag;            // 編集フラグ
    UInt16   editMode;            // 編集モード
    Char     infoArea[MINIBUF];   // 情報領域にデフォルト表示するもの
} NNshWorkingInfo;

#endif
