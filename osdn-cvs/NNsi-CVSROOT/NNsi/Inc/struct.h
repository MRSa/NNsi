/*============================================================================*
 *
 * $Id: struct.h,v 1.140 2012/01/10 16:27:42 mrsa Exp $
 *
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

typedef struct {
    Char    tabName[MAX_BOARDNAME];        // タブ名
    UInt16  newArrival;                    // 新着確認タブ
    UInt16  getList;                       // 一覧取得タブ
    UInt16  condition;                     // 条件
    UInt16  boardNick;                     // 板タブ
    UInt16  boardCondition;                // 指定
    UInt16  threadLevel;                   // レベル設定
    UInt16  threadStatus;                  // スレ状態指定
    UInt16  threadCreate;                  // スレ作成日
    UInt16  stringSet;                     // 文字列指定
    Char    string1[MINIBUF];              // 検索文字列１
    Char    string2[MINIBUF];              // 検索文字列２
    Char    string3[MINIBUF];              // 検索文字列３
} NNshCustomTab;

typedef struct {
    UInt16 up;              // ハードキー上
    UInt16 down;            // ハードキー下
    UInt16 key1;            // ハードキー１
    UInt16 key2;            // ハードキー２
    UInt16 key3;            // ハードキー３
    UInt16 key4;            // ハードキー４
    UInt16 jogPush;         // ジョグ押下
    UInt16 jogBack;         // ジョグBack
    UInt16 clieCapture;     // CLIEキャプチャ(NX)
    UInt16 left;            // ハードキー左
    UInt16 right;           // ハードキー右
} NNshHardkeyControl;

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
    UInt8   useNameOneLine;      // 名前欄を１行化する
    UInt8   boldMessageNum;      // 参照画面でスレ番号を【】でくくる
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
    UInt16  copyDelReadOnly;     // 参照ログスレにコピーしたとき、同時に削除
    UInt8   vfsUseCompactFlash;  // VFS使用可能時にCFを使用する
    UInt8   vfsOnAutomatic;      // VFS使用可能時に自動的にVFS ON
    UInt16  currentFont;         // フォントID(Starndard用)
    UInt8   notListReadOnlyNew;  // "未読あり"に参照ログは表示しない
    UInt8   notListReadOnly;     // "取得済み全て"に参照ログは表示しない
    UInt16  redrawAfterConnect;  // 接続後画面を再描画する
    UInt16  insertReplyNum;      // 書き込み時に参照スレ番号を挿入する
    UInt16  useARMlet;           // PalmOS5用機能を使用
    UInt16  sonyHRFont;          // (Sony HR用)Tinyフォント(参照用)
    UInt16  sonyHRFontTitle;     // (Sony HR用)Tinyフォント(一覧用)
    UInt16  useSonyTinyFont;     // (Sony HR用)Tinyフォントを使用する
    UInt16  useSonyTinyFontTitle; // (Sony HR用)Tinyフォントを使用する
    UInt8   notOpenSilkWrite;     // 書き込み時、自動的にシルクを拡大しない
    UInt8   notAutoFocus;         // 書き込み時、自動的に書込Fieldに移動しない
    UInt16  writeBufferSize;      // スレ一覧に表示するタイトルの長さ
    UInt16  browseMesNum;        // Browserで開くメッセージ番号
    UInt8   startMacroArrivalEnd; // 「新着確認」終了時にマクロを実行する
    UInt8   disconnArrivalEnd;   // 「新着確認」終了時に回線を切断する
    UInt8   autoBeep;            // 「新着確認」終了後にビープ音を鳴らす
    UInt8   autoOpenNotRead;     // 「新着確認」終了後に「未読あり」へ
    UInt16  addReturnToList;     // JumpListに「一覧へ戻る」を追加
    UInt16  notUseSilk;          // Silk制御を行わない
    UInt16  writeSequence2;      // 書き込みシーケンスPART2
    UInt16  startTitleRec;       // 表示中タイトル一覧の先頭レコード
    UInt16  endTitleRec;         // 表示中タイトル一覧の末尾レコード
    UInt16  currentSelectRec;    // 表示中タイトル一覧の選択レコード
    UInt16  selectedTitleItem;   // 表示中タイトル一覧の選択リスト番号
    UInt16  titleDispState;      // タイトル一覧表示状態
    UInt16  convertHanZen;       // 参照時、半角カナ→全角カナ変換
    UInt16  addLineDisconn;      // 参照時、選択メニューに回線切断
    UInt16  addLineGetPart;      // 参照時、選択メニューに差分取得
    UInt16  addJumpTopMsg;       // 参照時、選択メニューに先頭
    UInt16  addJumpBottomMsg;    // 参照時、選択メニューに末尾
    UInt16  addMenuSelAndWeb;    // 参照時、選択肢に全選択+WebBrowserで開く
    UInt16  addMenuMsg;          // 参照時、選択肢にメニューを追加 
    UInt16  addMenuBackRtnMsg;   // 参照時、選択肢に「１つ戻る」を追加 
    UInt16  addMenuGraphView;    // 参照時、選択肢に「描画モード変更」を追加
    UInt8   writingReplyMsg;     // Writeボタンを「引用カキコ」にする
    UInt8   addMenuFavorite;     // 参照時、選択肢に「お気に入り切替」を追加
    UInt8   addMenuGetThreadNum; // 参照時、選択肢に「スレ番指定取得」を追加
    UInt8   addMenuOutputMemo;   // 参照時、選択肢に「メモ帳へ出力」を追加
    UInt8   addNgSetting3;       // 参照時、選択肢に「NG設定」を追加 
    UInt8   addDeviceInfo;       // 参照時、選択肢にデバイス情報を追加 
    UInt8   addBtOnOff;          // 選択肢にBluetooth on/offを追加
    UInt8   addMenuTitle;        // 一覧時、選択肢にメニューを追加 
    UInt16  addMenuDeleteMsg;    // 一覧時、選択肢にMSG削除を追加 
    UInt16  addMenuCopyMsg;      // 一覧時、選択肢に参照ログへコピーを追加 
    UInt16  addMenuGetMode;      // 一覧時、選択肢にスレ取得モード切替
    UInt16  addMenuGraphTitle;   // 一覧時、選択肢に「描画モード変更」を追加
    UInt16  addMenuMultiSW1;     // 一覧時、選択肢に多目的スイッチ１を追加
    UInt16  addMenuMultiSW2;     // 一覧時、選択肢に多目的スイッチ２を追加
    UInt16  addMenuNNsiEnd;      // 一覧時、選択肢にNNsi終了を追加 
    UInt16  addMenuDeviceInfo;   // 一覧時、選択肢にデバイス情報を追加
    UInt16  disableSonyHR;       // SONYハイレゾ描画モードを使用しない
    UInt16  useColor;            // カラー表示を使用する
    UInt8   colorError;          // エラー時のカラー
    UInt8   colorOver;           // 1000超のカラー
    UInt8   colorNotYet;         // 未取得スレのカラー
    UInt8   colorNew;            // 新着スレのカラー
    UInt8   colorUpdate;         // 更新スレのカラー
    UInt8   colorRemain;         // 未読ありスレのカラー
    UInt8   colorAlready;        // 全て読んだスレのカラー
    UInt8   colorUnknown;        // 不明なスレのカラー
    UInt8   colorBackGround;     // 一覧画面の背景色
    UInt8   colorButton;         // 上下ボタンの色
    UInt8   colorViewBG;         // 参照画面の背景色
    UInt8   colorViewFG;         // 参照画面の文字色
    UInt8   rawDisplayMode;           // RAW表示モード
    UInt8   notDelOffline;            // OFFLINEスレ検索時にスレ消去しない
    UInt8   sortGoMenuAnchor;         // Goメニューアンカーを整列させる
    UInt8   notFocusNumField;         // スレ参照画面でフォーカス移動しない
    UInt16  disableUnderlineWrite;    // 書き込み時アンダーラインOFF
    UInt16  usageOfTitleMultiSwitch1; // スレ一覧画面の多目的スイッチ１
    UInt16  usageOfTitleMultiSwitch2; // スレ一覧画面の多目的スイッチ２
    UInt16  autoDeleteNotYet;         // 板移動時に未取得すれを削除する
    UInt16  notCheckBBSURL;           // 板URLの有効チェックをしない
    UInt16  displayFavorLevel;        // お気に入りに表示する最低スレレベル
    UInt16  newArrivalNotRead;        // 「未読あり」タブでの新着確認
    UInt16  getReserveFeature;        // スレ取得予約機能を使う
    Char    searchStrMessage[MAX_SEARCH_STRING]; // スレ内検索
    UInt32  searchMode;               // 検索モード
    UInt16  bbsOverwrite;             // BBS一覧を消去せず上書きする
    UInt16  useImodeURL;              // 一覧でブラウザ開くときi-mode用
    UInt16  nofRetry;                 // 通信タイムアウト時のリトライ回数
    UInt16  autoUpdateGetError;       // 差分取得失敗時に再取得
    UInt16  dispBottom;               // メッセージを末尾から表示できるように
    UInt16  blockDispMode;                 // 参照時、レスを連続表示
    Char    multiBtn1Caption[MAX_CAPTION]; // 多目的ボタン１のボタンラベル
    Char    multiBtn2Caption[MAX_CAPTION]; // 多目的ボタン２のボタンラベル
    Char    multiBtn3Caption[MAX_CAPTION]; // 多目的ボタン３のボタンラベル
    Char    multiBtn4Caption[MAX_CAPTION]; // 多目的ボタン４のボタンラベル
    Char    multiBtn5Caption[MAX_CAPTION]; // 多目的ボタン５のボタンラベル
    Char    multiBtn6Caption[MAX_CAPTION]; // 多目的ボタン６のボタンラベル
    UInt16  multiBtn1Feature;              // 多目的ボタン１のFeature
    UInt16  multiBtn2Feature;              // 多目的ボタン２のFeature
    UInt16  multiBtn3Feature;              // 多目的ボタン３のFeature
    UInt16  multiBtn4Feature;              // 多目的ボタン４のFeature
    UInt16  multiBtn5Feature;              // 多目的ボタン５のFeature
    UInt16  multiBtn6Feature;              // 多目的ボタン６のFeature

    NNshHardkeyControl useKey;             // ハードキーの総合制御情報
    NNshHardkeyControl ttlFtr;             // 一覧画面でのハードキー制御情報
    NNshHardkeyControl viewFtr;            // 参照画面でのハードキー制御情報

    UInt16  useHardKeyControl;             // ハードキーを制御する
    UInt16  checkDuplicateThread;          // 一覧表示時に重複の確認を行う
    UInt16  hideMessage;                   // ろーかるあぼーん実施
    Char    hideWord1[HIDEBUFSIZE];        // あぼーんキーワード１
    Char    hideWord2[HIDEBUFSIZE];        // あぼーんキーワード２
    NNshCustomTab custom1;                 // ユーザ設定１
    NNshCustomTab custom2;                 // ユーザ設定２
    NNshCustomTab custom3;                 // ユーザ設定３
    NNshCustomTab custom4;                 // ユーザ設定４
    NNshCustomTab custom5;                 // ユーザ設定５
    UInt8   listMesNumIsNotReadNum;        // 一覧画面で表示するレス数は未読数
    UInt8   writeJogPushDisable;           // 書き込み時PUSHボタンを無効
    UInt8   autostartMacro;                // NNsi起動時にマクロ実行
    UInt8   enableNewArrivalHtml;          // まちBBS/したらば@JBBSの新着有効
    UInt16  useRegularExpression;          // NGワードで正規表現を使用する
    UInt16  vfsOnNotDBCheck;               // VFS ONでもDBチェックしない
    UInt16  writeMessageAutoSave;          // 書き込み画面終了時、レスを保存
    Char    launchPass[PASSSIZE];          // 起動時キーワード設定
    UInt16  readOnlySelection;             // 参照ログディレクトリ選択番号
    UInt16  addMenuDirSelect;              // 一覧画面メニューにDir選択を追加
    UInt16  use_DAplugin;                  // 新着確認時にDA実行
    UInt16  listAndUpdate;                 // 一覧取得と同時に新着確認
    UInt16  msgNumLimit;                   // メッセージ番号の最大数
    UInt8   notUseTsPatch;                 // TsPatch機能を使わない
    UInt8   getROLogLevel;                 // 参照ログの取得レベル
    UInt16  viewMultiBtnFeature;           // 参照画面左下のボタンFeature
    Char    viewSearchStrHeader[SEARCH_HEADLEN]; // 参照画面検索時の初期ヘッダ
    UInt16  enableSearchStrHeader;         // 参照時に検索用初期文字列を指定
    UInt16  viewTitleSelFeature;           // 参照画面左下のボタンFeature
    Char    oysterUserId[OYSTERID_LEN];    //  2chユーザID
    Char    messageSeparator[SEPARATOR_LEN];  // スレ連続表示時の区切り文字列
    UInt8   colorMsgHeader;                 // 参照画面のヘッダ色
    UInt8   colorMsgHeaderBold;             // 参照画面のヘッダ色(強調)
    UInt8   colorMsgFooter;                 // 参照画面のフッタ色
    UInt8   colorMsgFooterBold;             // 参照画面のフッタ色(強調)
    UInt8   colorMsgInform;                 // 参照画面の情報色
    UInt8   colorMsgInformBold;             // 参照画面の情報色(強調)
    UInt8   useColorMessageToken;           // 参照画面の文字列で色をつける
    UInt8   useColorMessageBold;            // 参照画面の文字列で強調する
    NNshHardkeyControl getLogFeature;       // 参照ログ一覧のハードキー機能設定
    UInt16  getLogListPage;                 // 参照ログ一覧の最終表示ページ
    UInt16  getLogListSelection;            // 参照ログ一覧の最終選択アイテム
    UInt32  prepareDAforNewArrival;         // 新着確認前起動するDAのクリエータID
    UInt8   useGZIP;                        // GZIP圧縮を使用する 
    UInt8   reserve0;                       // とりあえず保管場所の確保... 
    UInt8   preOnDAnewArrival;              // 新着確認前にDAを起動する
    UInt8   showResPopup;                   // 参照画面で、レスをpopup表示する
    UInt16  vfsOnDefault;                   // 標準のVFS ON設定
    UInt16  useBe2chInfo;                   // 書き込み時にBe@2chの情報を使用する
    Char    be2chId[BE2chID_LEN];           // Be@2chのID
    Char    be2chPw[BE2chPW_LEN];           // Be@2chのパスワード
    UInt16  roundTripDelay;                 // 巡回時待ち時間 (ms)
    UInt8   searchPickupMode;               // 絞込み検索モード
    UInt8   getLogSiteListMode;             // 参照ログサイト一覧モード

    Char              cookieStoreBuffer[NNSH_WRITECOOKIE_BUFFER]; //  Cookieを記憶する場所

    Char    oysterPassword[OYSTER_LEN];     //  2chパスワード
    UInt16  selectedTitleItemForGetLog;     // 参照ログサイト一覧モードでの表示サイトアイテム
    UInt32  reserve2;                       // とりあえず保管場所の確保...
} NNshSavedPref;

// 板情報(データベースの構造)
typedef struct {
    Char   boardNick[MAX_NICKNAME];    // ボードニックネーム 
    Char   boardURL [MAX_URL];         // ボードURL
    Char   boardName[MAX_BOARDNAME];   // ボード名
    UInt8  bbsType;                    // ボードTYPE
    UInt8  state;                      // ボード使用状態
    Int16  reserved;                   // (予約領域：以前は使用していた)
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
  Char     searchName[MAX_BOARDNAME];  // 検索指定する文字列
  Char     PageLabel[BBSSEL_PAGEINFO]; // 表示中ページ数格納領域
} NNshBBS_Temp_Info;

// レス区切り用構造体(データベース)
typedef struct {
    UInt32 id;                              // データID
    Char   tokenPatternName[MAX_STRLEN];    // 区切りパターン名
    Char   startToken[MAX_DATATOKEN];       // 切り出し開始文字列
    Char   endToken  [MAX_DATATOKEN];       // 切り出し終了文字列
    Char   dataToken [MAX_DATATOKEN];       // データインデックス文字列
    UInt32 lastUpdate;                      // 最終更新時間
    UInt8  usable;                          // 使用する/しない
    UInt8  reserved0;                       // 予約領域
    UInt16 reserved1;                       // 予約領域
    UInt32 reserved2;                       // 予約領域  
} NNshLogTokenDatabase;

// 表示中のメッセージ情報(一時領域)
typedef struct
{
    UInt32      nofMsg;                // 読み込んだスレの最大メッセージ番号
    UInt16      openMsg;               // 現在開いているメッセージ番号
    UInt16      dispMsg;               // 表示しているメッセージ数
    UInt16      blockStartMsg;         // ゾーン表示モードで、ゾーンの先頭のメッセージ
    UInt16      nofJump;               // ジャンプ数
    UInt16      jumpMsg[JUMPBUF];      // スレ参照番号解釈の数
    UInt16      histIdx;               // 前回開いたメッセージ番号数
    UInt16      prevMsg[PREVBUF];      // 前回開いたメッセージバッファ
    UInt32      fileSize;              // 読み込み中ファイルのファイル長
    UInt16      reserved;              // (予約)
    UInt16      bbsType;               // BBSタイプ
    UInt16      msgStatus;             // メッセージの解析状態
    EventType   event;                 // ダミーのイベント用データ
    // NNshLogTokenDatabase *tokenInfo;   // ログの区切り情報データ
    UInt32     *msgOffset;             //
    Char       *jumpListTitles;        //
    MemHandle   jumpListH;             //
    Char        baseURL[MAX_URL];      // ベースURL
    NNshFileRef fileRef;
    UInt32      fileOffset;            // 読込んでいるデータの先頭からのoffset
    UInt32      fileReadSize;          // 現在バッファに読み込んでいるデータ長
    Char        showTitle  [MAX_THREADNAME]; // スレタイトル(画面表示用)
    Char        threadTitle[MAX_THREADNAME]; // スレ名称
    Char        threadFile [MAX_THREADFILENAME]; // スレID
    Char        boardNick  [MAX_NICKNAME];  // ボードニックネーム 
    Char        buffer     [MARGIN];        // データバッファ(実際はもっとある)
} NNshMessageIndex;

// 隠しメッセージのワーク領域構造体
typedef struct
{
    UInt16    nofWord;        // 文字列(リスト)数
    Char     *wordString;     // 文字列の領域(リスト形式)
    MemHandle wordmemH;       // 文字列リストデータ
} NNshWordList;

// Field を分割したときの領域定義
typedef struct
{
  UInt16 startX;
  UInt16 startY;
  UInt16 endX;
  UInt16 endY;
} fieldRegion;

// 参照ログ一覧の表示リスト
typedef struct
{
    UInt16    dataCount;
    UInt16    recNum    [MAX_GETLOG_RECORDS];
    UInt32    updateTime[MAX_GETLOG_RECORDS];
    Char     *title     [MAX_GETLOG_RECORDS];
} NNshDispList;

//  NNsi全体で使用するグローバル領域
typedef struct
{
    MemHandle   searchTitleH;
    MemHandle   searchBBSH;
    MemHandle   boardIdxH;
    MemHandle   logPrefixH;
    MemHandle   bbsTitleH;
    UInt16      backFormId;
    UInt32      palmOSVersion;       // デバイスのOSバージョン
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
    UInt16      notifyUpdate;        // Notifyイベントの更新処理
    UInt16      updateHR;            // SILKサイズ変更、画面表示を更新する
    UInt16      tempStatus;          //
#ifdef USE_CLIE
    UInt16      hrRef;               // SONY HR参照番号
    UInt16      hrVer;               // Sony HRのバージョン番号
    UInt16      notUseHR;            // HR制御を行わない
    UInt16      notUseSilk;          // Silk制御を行わない
    UInt16      silkRef;             // CLIE用SILKマネージャの参照番号(Temp)
    UInt16      silkVer;             // CLIE用SILKマネージャのVersion番号(Temp)
    UInt16     *jogInfoBack;         // SONYジョグ情報バックアップ
    UInt16    **jogAssistSetBack;    // SONYジョグアシストバックアップ
    UInt16      sonyJogMask[(NUMBER_OF_JOGMASKS * (2 + 1)) + MARGIN];
#endif // #ifdef USE_CLIE
#ifdef USE_COLOR
    RGBColorType fgColor;                // 前景色
    RGBColorType bgColor;                // 背景色
    RGBColorType txColor;                // テキスト色
    RGBColorType bgFormColor;            // (フォーム)背景色
    RGBColorType bgDialogColor;          // (ダイアログ)背景色
    RGBColorType bgAlertColor;           // (アラート)背景色
    RGBColorType bgObjectColor;          // (オブジェクト)背景色
#endif  // #ifdef USE_COLOR
    UInt32             os5HighDensity;   // 高解像度サポートの有無
    UInt16             jumpSelection;    // ジャンプ選択(+ 一覧画面機能実行番号)
    EventType          dummyEvent;       // イベント生成用バッファ
    Char               dummyBuffer[MARGIN];
    FormType          *prevBusyForm;
    Char              *bbsTitles;

    UInt16             nofTitleItems;   // スレタイトル数
    Char              *msgListStrings;  // スレタイトルデータ格納領域
    Char              **msgTitleIndex;  // (1件ごとの)スレタイトル先頭
    UInt16            *msgListIndex;    // タイトルリスト→スレレコード番号変換
    UInt16            *msgTitleStatus;  // (1件ごとの)スレタイトル状態
    UInt16            *msgNumbers;      // (1件ごとの)メッセージ数

    UInt32            be2chLoginDateTime;    //  Be@2chのログイン実行時刻
    Char              *be2chCookie;          //  Be@2chのログイン時Cookie

//    Char              cookieStoreBuffer[NNSH_WRITECOOKIE_BUFFER]; //  Cookieを記憶する場所

    Char              *customizedUserAgent;  //  ユーザーエージェント名
    Char              *connectedSessionId;   //  2ch用セッションID
    UInt32            loginDateTime;         //  ログイン実施時間
//    Char              oysterPassword[OYSTER_LEN];  //  2chパスワード

    NNshBBS_Temp_Info *tempInfo;
    NNshMessageIndex  *msgIndex;
    fieldRegion        fieldPos[2];       // 参照画面の各領域情報
    UInt32             tokenId;           // 参照画面区切りパターン
    Char              *bookmarkFileName;  // しおり設定時のスレID
    Char              *bookmarkNick;      // しおり設定時のスレNick
    Char               restoreSetting;    // 設定の復旧
    MemHandle          listHandle;        // リストハンドル
    NNshWordList       hide1;             // NG設定１リスト
    NNshWordList       hide2;             // NG設定２リスト
    NNshWordList      *featureList;       // 機能設定ラベル
    UInt16             featureLockCnt;    // 機能設定ラベルのロック回数
#ifdef USE_LOGCHARGE
    NNshDispList      *dispList;          // 参照ログ一覧
    Char               getLogListUrlNameLabel[MAX_TITLE_LABEL];
#endif // #ifdef USE_LOGCHARGE
#ifdef USE_ADDR_NETLIB
    // NetLib時には宣言なし
#else
    NetSocketAddrType  sockAddr;          // 通信先アドレス
#endif
#ifdef USE_ZLIB
    UInt16             getRetry;          // 再取得
    UInt16             useGzip;           // GZIP圧縮を使用する(実処理)
    void             *streamZ;           // zgipストリーム
    Char              *inflateBuf;        // zlib解凍先バッファ
#endif // #ifdef USE_ZLIB
    UInt16             work1;             // ワーク領域(その１)
    UInt16             work2;             // ワーク領域(その２)
    UInt16             work3;             // ワーク領域(その３)
    UInt32            *work4;             // ワーク領域(その４)
    NNshSavedPref     *NNsiParam;         // NNsi設定用データベース
} NNshWorkingInfo;

typedef struct {
    UInt8 importDB:1;                        // DB を復元するかどうか
    UInt8 deleteFile:1;                        // VFS の DBを削除するかどうか
    UInt8 fileExist:1;                        // VFS に DB があるかどうか
    UInt32 modDateVFS;                        // VFS の DB の更新時間
    UInt32 modDateMEM;                        // MEM の DB の更新時間
    Char dateStrMEM[24];
    Char dateStrVFS[24];
} NNshVFSDBInfo;

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

typedef struct {
  UInt16  popId;
  UInt16  lstId;
} NNshListItem;

typedef struct {
    Char   dataFileName[MAX_THREADFILENAME]; // データファイル名
    Char   dataTitlePrefix[MAX_DATAPREFIX];  // データヘッダ
    Char   boardName      [MAX_NICKNAME];    // ボードニックネーム(未使用) 
    Char   dataURL    [MAX_GETLOG_URL];      // データ取得先URL
    Char   getDateTime[MAX_GETLOG_DATETIME]; // 更新時刻
    Char   dataTitle  [MAX_TITLE_LABEL];     // データ名称
    UInt8  wwwcMode;                         // ログ取得しない
    UInt8  rssData;                          // RSSデータ
    UInt16 notUse0;                          // 未使用領域(廃止)
    UInt16 notUse1;                          // 未使用領域(廃止)
    UInt32 lastUpdate;                       // 最終更新時間
    UInt16 id;                               // データID
    UInt8  state;                            // 参照ログ状態
    UInt8  getAfterHours;                    // 再取得する時間間隔
    UInt8  usable;                           // 取得する/しない
    UInt8  getDepth;                         // 取得階層指定
    UInt8  kanjiCode;                        // 漢字コード
    UInt8  getSubData;                       // 「１つ下取得」で取得したデータ数
    UInt32 tokenId;                          // 区切りのID
    UInt32 subTokenId;                       // サブ区切りのID
} NNshGetLogDatabase;

//
//  階層下にあるファイルの取得情報
//
typedef struct {
    Char   dataURL     [MAX_GETLOG_URL];      // データ取得先URL
    Char   dataFileName[MAX_THREADFILENAME];  // データファイル名
    UInt32 lastUpdate;                        // 最終更新時間
    UInt32 parentDbId;                        // 親のレコードID(ユニークID)
    UInt32 logDbId;                           // ログのレコードID(ユニークID)
    UInt32 tokenId;                           // 区切り文字のレコードID(ユニークID)
    UInt32 reserve0;                          // 予約領域
    UInt32 reserve1;                          // 予約領域
    UInt32 reserve2;                          // 予約領域
    UInt32 reserve3;                          // 予約領域
} NNshGetLogSubFileDatabase;


// 「１つ下も取得」で、ＵＲＬ解析時に作成する
//  テンポラリデータベース
typedef struct {
    Char  dataURL[MAX_GETLOG_URL + MARGIN];
    Char  titleName[MAX_THREADNAME + MARGIN];
} NNshGetLogTempURLDatabase;


typedef struct {
    Char   ngWord[NGWORD_LEN];
    UInt8  checkArea;
    UInt8  matchedAction;
    UInt16 reserved1;    
} NNshNGwordDatabase;

// UnicodeToJis0208 テーブルレコード
typedef struct
{
    UInt16 size;
    UInt16 table[256];
} UnicodeToJis0208Rec;

#ifdef USE_SSL
#ifdef USE_SSL_V2

/* RC4 */
typedef struct rc4_key_st
{
  Int32 x;
  Int32 y;
  Int32 data[256];
} RC4_KEY;

/* MD5 context. */
typedef struct {
  UInt32 state [4];                                   /* state (ABCD) */
  UInt32 count [2];        /* number of bits, modulo 2^64 (lsb first) */
  UInt8  buffer[64];                                  /* input buffer */
} MD5_CTX;

typedef struct {
    UInt16        state;
    UInt16        netRef;
    NetSocketRef  socketRef;
    UInt8         challenge[SSL2_CHALLENGE_LENGTH + MARGIN];
    UInt8         clientWriteKey[SSL2_RWKEY_LENGTH + MARGIN];
    UInt8         clientReadKey[SSL2_RWKEY_LENGTH + MARGIN];
    UInt8         clientMasterKey[SSL2_MASTERKEY_LENGTH + MARGIN];
    UInt8         PADDING[64];
    MD5_CTX       context;
    UInt8         digest[SSL2_MD5_HASH_LENGTH + MARGIN];
    UInt32        seqNum;
    RC4_KEY       workReadKey;
    RC4_KEY       workWriteKey;
    UInt8        *serverPublicKey;
    UInt16        serverPublicLen;
    UInt8        *connectionId;
    UInt16        connectionIdLen;
    UInt16        bufLen;
    Char         *buffer;
} NNshSSLv2Ref;
#endif
#endif

#ifdef USE_MACRO
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
#endif

// ping用 ICMPパケット構造体
typedef struct {
  UInt8  icmp_type;
  UInt8  icmp_code;
  UInt16 icmp_cksum;
  UInt16 icmp_id;
  UInt16 icmp_seq;
  UInt8  icmp_data[32];
} ICMP_packet;

// IPパケット構造体(PING用)
typedef struct {
  UInt8       ip_ver;
  UInt8       ip_tos;
  UInt16      ip_len;  
  UInt16      ip_id;
  UInt16      ip_off;
  UInt8       ip_ttl;
  UInt8       ip_p;
  UInt16      ip_sum;
  NetIPAddr   ip_src;
  NetIPAddr   ip_dst;
  ICMP_packet icmp;
} PING_packet;

#endif
