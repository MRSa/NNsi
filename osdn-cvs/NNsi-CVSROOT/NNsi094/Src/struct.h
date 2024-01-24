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

typedef struct {
    UInt32  NNsiVersion;
    UInt32  prefSize;
    UInt16  prefType;
    UInt16  debugMode;
} NNshSoftwareVersionInfo;

// システム情報(起動時にデータベースから読み込む情報、、、増えすぎ...)
typedef struct {
    /** NNsi設定 **/
    Char    handleName[BUFSIZE]; // コテハン名
    Char    bbs_URL   [MAX_URL]; // BBS一覧取得用ＵＲＬ
    Char    proxyURL  [MAX_URL]; // Proxyのホスト名
    Int32   netTimeout;          // 通信タイムアウト値
    UInt16  getAllThread;        // 全てのスレ名を取得
    UInt16  confirmationDisable; // 確認ダイアログを省略する
    UInt16  openAutomatic;       // スレ取得後自動的にオープン
    UInt16  writeAlwaysSage;     // sage書き込み
    UInt16  useBookmark;         // しおり機能を使う
    UInt16  searchCaseless;      // 検索時半角大文字小文字を無視する
    UInt16  offChkLaunch;        // NNsi起動時にOFFLINEスレ検索
    UInt16  disableUnderline;    // スレ参照時のUNDERLINE OFF
    UInt16  debugMessageON;      // デバッグメッセージの表示
    UInt16  printNofMessage;     // スレ一覧でスレ数を表示
    UInt16  boldMessageNum;      // 参照画面でスレ番号を【】でくくる
    UInt16  useVFS;              // VFSシステムの使用可否設定
    UInt16  bufferSize;          // ワーク領域として確保するバッファサイズ
    UInt16  partGetSize;         // メッセージ分割受信サイズ
    UInt16  enablePartGet;       // 分割ダウンロード
    UInt16  lastFrmID;           // 終了時に表示していた画面
    UInt16  lastBBS;             // 終了時に表示していたBBS名
    UInt16  openMsgIndex;        // 表示するメッセージINDEX(番号)
    UInt16  bookMsgIndex;        // BookmarkメッセージINDEX
    UInt16  notCursor;           // 参照時カーソル表示しない
    UInt16  bookMsgNumber;       // Bookmarkメッセージ番号
    UInt16  openThreadIndex;     // 表示するスレリスト番号
    UInt16  useFixedHandle;      // コテハン機能を使用する
    UInt16  disconnectNNsiEnd;   // NNsi終了時に回線切断
    UInt16  useProxy;            // Proxy経由でのアクセス
    UInt16  proxyPort;           // Proxyのポート番号
    UInt16  useCookieWrite;      // 受信したCookieを使用して書き込み
    UInt16  copyDelReadOnly;     // 参照専用スレにコピーしたとき、同時に削除
    UInt16  vfsOnAutomatic;      // VFS使用可能時に自動的にVFS ON
    UInt16  currentFont;         // フォントID(Starndard用)
    UInt16  notListReadOnly;     // "取得済み全て"には参照専用ログは表示しない
    UInt16  redrawAfterConnect;  // 接続後画面を再描画する
    UInt16  insertReplyNum;      // 書き込み時に参照スレ番号を挿入する
    UInt16  useARMlet;           // PalmOS5用機能を使用
    UInt16  sonyHRFont;          // (Sony HR用)TinyフォントID
    UInt16  useSonyTinyFont;     // (Sony HR用)Tinyフォントを使用する
    UInt16  notAutoFocus;        // 書き込み時、自動的に書込Fieldに移動しない
    UInt16  titleListSize;       // スレ一覧に表示するタイトルの長さ
    UInt16  browseMesNum;        // Browserで開くメッセージ番号
    UInt16  disconnArrivalEnd;   // 「新着確認」終了時に回線を切断する
    UInt16  autoOpenNotRead;     // 「新着確認」終了後に「未読あり」へ
    UInt16  jogBackBtnAsGo;      // Jog BackボタンをGoボタンと等価に
    UInt16  addReturnToList;     // JumpListに「一覧へ戻る」を追加
    UInt16  jogBackBtnAsMenu;    // 一覧表示時Jog BackボタンでをMenu表示
    UInt16  notUseSilk;          // Silk制御を行わない
} NNshSavedPref;

// 板情報(データベースの構造)
typedef struct {
    Char   boardNick[MAX_NICKNAME];    // ボードニックネーム 
    Char   boardURL [MAX_URL];         // ボードURL
    Char   boardName[MAX_BOARDNAME];   // ボード名
    UInt8  bbsType;                    // ボードTYPE
    UInt8  state;                      // ボード使用状態
    Int16  threadCnt;                  // (取得済み)スレタイトル数
} NNshBoardDatabase;

// スレ情報(データベースの構造)
typedef struct {
    Char   threadFileName[MAX_THREADFILENAME]; // ボード名
    Char   threadTitle   [MAX_THREADNAME];     // スレ名
    Char   boardNick     [MAX_NICKNAME];       // ボードニックネーム 
    UInt16 dirIndex;                           // スレのディレクトリIndex番号
    UInt16 reserved;                           // (未使用)
    UInt8  state;                              // メッセージ取得状況
    UInt8  msgAttribute;                       // メッセージの属性
    UInt8  msgState;                           // メッセージの記録先
    UInt8  bbsType;                            // BBS(メッセージ)の取得形式
    UInt16 maxLoc;                             // 最大メッセージ番号
    UInt16 currentLoc;                         // 表示メッセージ番号
    UInt32 fileSize;                           // ファイルサイズ
} NNshSubjectDatabase;

// スレ格納ディレクトリ情報(データベースの構造)
typedef struct {
    UInt16   dirIndex;                         // ディレクトリIndex(ID)
    Char     boardNick[MAX_NICKNAME];          // ボードニックネーム 
    Char     dirName[MAX_DIRNAME];             // ディレクトリ名称
    UInt16   previousIndex;                    // ひとつ上のディレクトリIndex
    UInt32   depth;                            // ディレクトリの深度
    UInt32   reserved;                         // 予備１
} NNshDirectoryDatabase;

// 板インデックス情報(一時領域)
typedef struct {
    UInt16   bbsIndex;                 // 使用するBBSのINDEX(厳密には先頭)
} NNshBBSRelation;

// BBS使用情報(一時領域、BBS選択画面で使用)
typedef struct
{
  Int16    currentPage;                // 現在表示しているページ番号
  Int16    maxPage;                    // 最大のページ番号
  UInt16   nofRecords;                 // BBS名のレコード数
  UInt16   nofRow;                     // 1ページあたりに表示可能なBBS数
  UInt16  *checkBoxState;              // チェックボックスの数(全BBS数分)
  Char    *BBSName_Temp;               // ＢＢＳ名リスト(画面に表示する数分)
  UInt32   BBSName_size;               // ＢＢＳ名リストの大きさ
  Char     PageLabel[BBSSEL_PAGEINFO]; // 表示中ページ数格納領域
} NNshBBS_Temp_Info;

// 表示中のメッセージ情報(一時領域)
typedef struct
{
    UInt32      nofMsg;                // 読み込んだスレの最大メッセージ番号
    UInt16      openMsg;               // 現在開いているメッセージ番号
    UInt16      nofJump;               // ジャンプ数
    UInt16      jumpMsg[JUMPBUF];      // スレ参照番号解釈の数
    UInt16      histIdx;               // 前回開いたメッセージ番号数
    UInt16      prevMsg[PREVBUF];      // 前回開いたメッセージバッファ
    UInt32      fileSize;              // 読み込み中ファイルのファイル長
    UInt32      msgOffset[NNSH_MESSAGE_MAXNUMBER]; // スレインデックス
    UInt16      reserved;              // (予約)
    UInt16      bbsType;               // BBSタイプ
    EventType   event;                 // ダミーのイベント用データ
    Char       *jumpListTitles;        //
    MemHandle   jumpListH;             //
    NNshFileRef fileRef;
    UInt32      fileOffset;            // 読込んでいるデータの先頭からのoffset
    UInt32      fileReadSize;          // 現在バッファに読み込んでいるデータ長
    Char        threadTitle[MAX_THREADNAME]; // スレ名称
    Char        boardNick  [MAX_NICKNAME];  // ボードニックネーム 
    Char        buffer     [MARGIN];        // データバッファ(実際はもっとある)
} NNshMessageIndex;

//  NNsi全体で使用するグローバル領域
typedef struct
{
    MemHandle   searchTitleH;
    MemHandle   threadIdxH;
    MemHandle   boardIdxH;
    MemHandle   logPrefixH;
    MemHandle   bbsTitleH;
    MemHandle   threadTitleH;
    MemHandle   msgTTLStringH;
    Int16       tappedPrev;
    UInt16      updateDatabaseInfo;  // DBがアップデートされた情報
    UInt16      useBBS;
    UInt16      netRef;              // ネットワーク参照番号
    UInt16      device;
    UInt16      vfsVol;
    UInt16      browserLaunchCode;   // WebBrowserのLaunch Code
    UInt32      browserCreator;      // WebBrowserのCreatorID
    UInt16      prevHRWidth;         // グラフィックモード表示用(画面Mode記憶)
    UInt16      totalLine;           // グラフィックモード表示用(全行数)
    UInt16      nofPage;             // グラフィックモード表示用(全ページ数)
    UInt16      currentPage;         // グラフィックモード表示用(表示中ページ)
    UInt16      prevHRLines;         // CLIEハイレゾ表示用(表示可能行数記憶域)
#ifdef USE_CLIE
    UInt16      updateHR;            // SILKサイズ変更、画面表示を更新する
    UInt16      hrRef;               // SONY HR参照番号
    UInt16      hrVer;               // Sony HRのバージョン番号
    UInt16      notUseSilk;          // Silk制御を行わない
    UInt16      silkRef;             // CLIE用SILKマネージャの参照番号(Temp)
    UInt16      silkVer;             // CLIE用SILKマネージャのVersion番号(Temp)
    UInt16     *jogInfoBack;         // SONYジョグ情報バックアップ
    UInt16    **jogAssistSetBack;    // SONYジョグアシストバックアップ
    UInt16      tempStatus;          //
#endif // #ifdef USE_CLIE
    UInt16             jumpSelection; // ジャンプ選択
    EventType          dummyEvent;    // イベント生成用バッファ
    Char               dummyBuffer[MARGIN];
    FormType          *prevBusyForm;
    Char              *bbsTitles;
    UInt16             threadCount;  // スレ一覧画面に表示されてるスレ(LIST)数
    Char              *threadTitles;
    NNshBBS_Temp_Info *tempInfo;
    NNshMessageIndex  *msgIndex;
} NNshWorkingInfo;

#endif
