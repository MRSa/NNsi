import javax.microedition.lcdui.Alert;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.rms.RecordStore;
import javax.microedition.lcdui.Font;

public class a2BobjectFactory 
{
    private a2BMain                 parent          = null;  // 親オブジェクト

    public a2BPreferenceDatabase   preference       = null;  // 設定情報保持クラス

    public a2BMessageListForm      listForm         = null;  // 一覧画面
    public a2BMessageViewForm      viewForm         = null;  // 参照画面
    public a2BMessageWriteForm     writeForm        = null;  // 書き込み画面
    public a2BfileSelector         fileSelector     = null;  // ファイルセレクタ画面
    public a2BselectionForm        selectForm       = null;  // データ選択用画面
    public Alert                   dialogForm       = null;  // 警告メッセージダイアログ

    public a2BbbsCommunicator      bbsCommunicator  = null;  // 板一覧の解析・データ通信クラス
    public a2BHttpCommunicator     httpCommunicator = null;  // HTTPデータ通信クラス
    public a2BsubjectDataParser    subjectParser    = null;  // スレ一覧の解析クラス
    public a2BFavoriteManager      favoriteManager  = null;  // お気に入りスレ管理クラス
    public a2BloginHolder          loginUtility     = null;  // ログインユーティリティ
    
    private a2BdialogListener      dialogListener   = null;  // ダイアログ表示中の処理
    private a2Butf8Conveter        utf8Converter    = null;  // SHIFT JIS => UTF8変換クラス
    private a2BsjConverter         sjConverter      = null;  // UTF8 => SHIFT JIS変換クラス
    private a2BNgWordManager       ngWordManager    = null;  // ローカルあぼーんキーワード管理クラス

    private final byte[] a2B_PREF_VERSION = { (byte) 4, (byte) 0 };  // preference DBのバージョン...

    private  boolean     baseDirSettingMode = false;  // ベースディレクトリ設定モード...

    private final int    a2B_VERSION_RECORDID     = 1; // preference DBのバージョン格納レコード
    private final int    a2B_BASEDIR_RECORDID     = 2; // preference DBのベースディレクトリ格納レコード
    private final int    a2B_CURRENTBBS_RECORDID  = 3; // preference DBの現在BBSカテゴリ設定
    private final int    a2B_OYSTERNAME_RECORDID  = 4;
    private final int    a2B_OYSTERPASS_RECORDID  = 5;
    private final int    a2B_WRITE_NAME_RECORDID  = 6;
    private final int    a2B_FONT_SIZE_RECORDID   = 7;
    private final int    a2B_PROXYURL_RECORDID    = 8;
    private final int    a2B_CURRENTLOAD_RECORDID = 9;
    private final int    a2B_BENAME_RECORDID      = 10;
    private final int    a2B_BEPASS_RECORDID      = 11;
    private final int    a2B_USERAGENT_RECORDID   = 12;
    private final int    a2B_FILESAVEDIR_RECORDID = 13;
    private final int    a2B_MEMOFILE_RECORDID    = 14;
    private final int    a2B_WRITEKEY_RECORDID    = 15;
    private final int    a2B_WRITECOOKIE_RECORDID = 16;
    
    // コンストラクタ
    public a2BobjectFactory(a2BMain object)
    {
        parent = object;
    }

    // オブジェクトの準備
    public boolean prepare()
    {
        // ダイアログの準備
        dialogForm = new Alert("");
        dialogListener = new a2BdialogListener(parent);
        dialogForm.setCommandListener(dialogListener);

        // データベースの準備
        prepareDatabase();

        // 画面の表示準備
        prepareScreen();        

        // パラメータの読み出し (スレッドを起こして実行)
//        boolean ret = restoreParameters();
        Thread thread = new Thread()
        {
            public void run()
            {
                baseDirSettingMode = restoreParameters();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            //
        }
        thread = null;
        System.gc();

        // 画面サイズの設定(表示準備その2)
        prepareScreenSize();
        
        // 漢字変換を強制的に自前のものを使う場合...
        if (preference.forceSelfKanjiConvert == true)
        {
            utf8Converter.forceSelfKanjiConvertor();
        }
        
        return (baseDirSettingMode);
    }

    /*  
     *  オブジェクトを削除する
     *
     */
    public void cleanup()
    {
        backupParameters();

        favoriteManager  = null;
        ngWordManager    = null;
        utf8Converter    = null;
        sjConverter      = null;
        httpCommunicator = null;
        bbsCommunicator  = null;
        subjectParser    = null;
        loginUtility     = null;

        viewForm         = null;
        listForm         = null;
        writeForm        = null;
        fileSelector     = null;
        selectForm       = null;
        dialogForm       = null;
        dialogListener   = null;
        
        preference       = null;
    }

    /**
     * 　お気に入りをバックアップする
     *
     */
    public void backupFavorite()
    {
         favoriteManager.backup();
    }

    /**
     *   データベースを準備する
     * 
     */
    private void prepareDatabase()
    {
        // 設定情報保持クラスを生成する
        //preference = new a2BPreferenceDatabase(parent);

        // 文字列コンバータ(to UTF8)を生成する
        utf8Converter = new a2Butf8Conveter();

        // 文字列コンバータ(to Shift JIS)を生成する
        sjConverter   = new a2BsjConverter();

        // NGワード管理クラスを生成する
        ngWordManager = new a2BNgWordManager(parent, utf8Converter, sjConverter);

        // HTTP GETデータ通信クラスを生成する
        httpCommunicator = new a2BHttpCommunicator(parent);

        // ログインユーティリティクラスを生成する
        loginUtility     = new a2BloginHolder(parent, utf8Converter);
        
        // 板一覧解析・データコミュニケータを生成する
        bbsCommunicator = new a2BbbsCommunicator(parent, utf8Converter);

        // お気に入りスレ管理クラスを生成する
        favoriteManager = new a2BFavoriteManager(parent, utf8Converter);        

        // スレ一覧解析クラスを生成する
        subjectParser = new a2BsubjectDataParser(parent, bbsCommunicator, utf8Converter, favoriteManager);
    }

    /*
     *   使用する画面を準備する
     * 
     */
    private void prepareScreen()
    {
        // 参照画面の準備
        viewForm = new a2BMessageViewForm("a2B", parent, subjectParser, utf8Converter, ngWordManager);
        viewForm.setCommandListener(viewForm);

        // 一覧画面の準備
        listForm = new a2BMessageListForm("a2B", parent, bbsCommunicator, subjectParser, favoriteManager);
        listForm.setCommandListener(listForm);

        // 書き込み画面の準備
        writeForm = new a2BMessageWriteForm("Write", parent, sjConverter, utf8Converter);
        writeForm.setCommandListener(writeForm);

        // ファイルセレクタの準備
        fileSelector = new a2BfileSelector(parent);

        // アイテム選択フォームの準備
        selectForm = new a2BselectionForm(parent);
        
        return;
    }
    
    /*
     *   画面サイズの設定
     * 
     * 
     */
    private void prepareScreenSize()
    {
        // 壁紙の読み出し
        viewForm.loadWallPaperImage();
        
        // ヘルプイメージの読み出し
        listForm.loadHelpImage();

        listForm.prepareScreenSize();

        viewForm.prepareScreenSize();
        
        selectForm.prepareScreenSize();
        
        return;
    }
    
    /**
     * a2B設定情報をリセットするかどうか
     * 
     * @param isReset
     * @return  true : リセットする、false : リセットしない
     * 
     */
    private boolean checkResetPreference(boolean isReset)
    {
        if (isReset == true)
        {
            // 本来、a2B設定情報をリセットする場合...
            String baseDir = null;
            try
            {
                // jadファイルでディレクトリの場所が指定されているか？
                baseDir = parent.getAppProperty("a2B-Directory");

                //  ※ WX310での指定例 :  a2B-Directory: file:///SD:/$USER_DATA/a2B/
                //  ※ 702NKでの指定例 :  a2B-Directory: file:///E:/WORK/a2B/
                //  ※ W-ZERO3の設定例 :  a2B-Directory: file:///miniSD/WORK/a2B/
                //  ※ Androidの設定例 :  a2B-Directory: file:///sdcard/a2B/
            }
            catch (Exception e)
            {
                baseDir = null;
            }
            if (baseDir != null)
            {
                // jadファイルに書かれているベースディレクトリを設定情報として書き出す
                preference.setBaseDirectory(baseDir);
                
                // ベースディレクトリをファイルセレクタのデフォルト値とする
                fileSelector.setnextPath(preference.getBaseDirectory());                    

                // jadファイルにベースディレクトリ情報が設定されていた場合、リセットしない
                return (false);
            }
        }
        return (isReset);
    }
    
    /*
     *   パラメータからデータを読み込む処理
     *   
     */
    private boolean restoreParameters()
    {
        if (preference == null)
        {
            preference = new a2BPreferenceDatabase(parent);
        }

        boolean     isReset            = false;
        byte[]       versionInfo       = null;
        byte[]       baseDirectory     = null;
        byte[]       boardInfo         = null;
        byte[]       fontInfo          = null;
        RecordStore  prefDB             = null;
        try
        {
            // レコードストアをオープンする
            prefDB = RecordStore.openRecordStore("a2B-Settings", true);
            if (prefDB.getNumRecords() == 0)
            {
                // レコードが登録されていなかった場合は、初期データを登録する
                isReset = true;
            }
            else
            {
                // データベースのバージョン確認...
                versionInfo = prefDB.getRecord(a2B_VERSION_RECORDID);
                if (versionInfo.length == a2B_PREF_VERSION.length)
                {
                    for (int loop = 0; ((loop < versionInfo.length)&&(isReset == false)); loop++)
                    {
                        if (versionInfo[loop] != a2B_PREF_VERSION[loop])
                        {
                            // バージョン情報違い、、、リセットする
                            isReset = true;
                            loop = versionInfo.length;
                        }
                    }
                }
                else
                {
                    // バージョン情報違い、DBをリセットする
                    isReset = true;
                }

                // ベースディレクトリを読み出す
                baseDirectory = prefDB.getRecord(a2B_BASEDIR_RECORDID);
                if (baseDirectory.length == 0)
                {
                    // ベースディレクトリ情報が見つからなかった...
                    parent.showDialog(parent.SHOW_INFO, 600000, "情報", "データ(bbstable.html)の格納場所が記録されていません。指定してください。");    
                    baseDirSettingMode  = true;
                }
                else
                {
                    // ベースディレクトリを設定情報に読み出す
                    preference.setBaseDirectory(new String(baseDirectory));

                    // ベースディレクトリをファイルセレクタのデフォルト値とする
                    fileSelector.setnextPath(preference.getBaseDirectory());                    
                }

                // 選択中板情報、レス番号を読み出す
                boardInfo = prefDB.getRecord(a2B_CURRENTBBS_RECORDID);
                if (boardInfo.length == 72)
                {
                    
                    preference.currentBbsCategoryIndex = deSerializeData(boardInfo,  0);
                    preference.currentBbsIndex         = deSerializeData(boardInfo,  4);
                    preference.selectedSubjectIndex    = deSerializeData(boardInfo,  8);
                    preference.selectedMessageNumber   = deSerializeData(boardInfo, 12);
                    preference.ngWordDetectMode        = deSerializeData(boardInfo, 16);
                    preference.favoriteShowMode        = deSerializeData(boardInfo, 20);
                    preference.usedProxyScene          = deSerializeData(boardInfo, 24);
                    preference.openCurrentScreen       = deSerializeData(boardInfo, 28);
                    preference.currentMessageIndex     = deSerializeData(boardInfo, 32);
                    preference.currentScreenNumber     = deSerializeData(boardInfo, 36);
                    preference.vibrateDuration         = deSerializeData(boardInfo, 40);
                    int optionParameters              = deSerializeData(boardInfo, 44);
                    preference.favoriteShowLevel       = deSerializeData(boardInfo, 48);
                    preference.viewKanjiMode           = deSerializeData(boardInfo, 52);
                    preference.resBatchMode            = deSerializeData(boardInfo, 56);
                    preference.keyRepeatDelayTime      = deSerializeData(boardInfo, 60);
                    int optionDatas                   =  deSerializeData(boardInfo, 64);
                    preference.divideCommunicationSize = deSerializeData(boardInfo, 68);

                    if ((optionParameters & 1) != 0)
                    {
                        preference.viewScreenMode = true;
                    }
                    else
                    {
                        preference.viewScreenMode = false;                        
                    }

                    if ((optionParameters & 2) != 0)
                    {
                        preference.afterLaunchWebAction = true;
                    }
                    else
                    {
                        preference.afterLaunchWebAction = false;                        
                    }
                    if ((optionParameters & 4) != 0)
                    {
                        preference.autoEntryFavorite = true;
                    }
                    else
                    {
                        preference.autoEntryFavorite = false;                        
                    }
                    if ((optionParameters & 8) != 0)
                    {
                        preference.saveAtRootDirectory = true;
                    }
                    else
                    {
                        preference.saveAtRootDirectory = false;
                    }                
                    if ((optionParameters & 16) != 0)
                    {
                        preference.divideToGetHttp = true;
                    }
                    else
                    {
                        preference.divideToGetHttp = false;
                    }                
                    if ((optionParameters & 32) != 0)
                    {
                        preference.onlyDatabaseAccess = true;
                    }
                    else
                    {
                        preference.onlyDatabaseAccess = false;
                    }                
                    if ((optionParameters & 64) != 0)
                    {
                        preference.isShowTitleBusyArea = true;
                    }
                    else
                    {
                        preference.isShowTitleBusyArea = false;
                    }
                    if ((optionParameters & 128) != 0)
                    {
                        preference.backupSubjectTxt = true;
                    }
                    else
                    {
                        preference.backupSubjectTxt = false;
                    }
                    if ((optionParameters & 256) != 0)
                    {
                        preference.useGoogleProxy = true;
                    }
                    else
                    {
                        preference.useGoogleProxy = false;
                    }
                    if ((optionParameters & 512) != 0)
                    {
                        preference.useCachePicFile = true;
                    }
                    else
                    {
                        preference.useCachePicFile = false;
                    }
                    if ((optionParameters & 1024) != 0)
                    {
                        preference.ignoreFavoriteBackup = true;
                    }
                    else
                    {
                        preference.ignoreFavoriteBackup = false;
                    }
                    if ((optionParameters & 2048) != 0)
                    {
                        preference.cursorSingleMovingMode = true;
                    }
                    else
                    {
                        preference.cursorSingleMovingMode = false;
                    }
                    if ((optionParameters & 4096) != 0)
                    {
                        preference.useWX220JUserAgent = true;
                    }
                    else
                    {
                        preference.useWX220JUserAgent = false;
                    }
                    if ((optionParameters & 8192) != 0)
                    {
                        preference.reserve1 = true;
                    }
                    else
                    {
                        preference.reserve1 = false;
                    }
                    if ((optionParameters & 16384) != 0)
                    {
                        preference.forceSelfKanjiConvert = true;
                    }
                    else
                    {
                        preference.forceSelfKanjiConvert = false;
                    }
                    if ((optionParameters & 32768) != 0)
                    {
                        preference.usePicToConvert = true;
                    }
                    else
                    {
                        preference.usePicToConvert = false;
                    }
                    preference.aaWidth = optionDatas % 512;
                }

                byte[] byteData = null;
                byteData = prefDB.getRecord(a2B_OYSTERNAME_RECORDID);
                if (byteData != null)
                {
                    preference.oysterLoginName = new String(byteData);
                    if ((preference.oysterLoginName).compareTo(" ") == 0)
                    {
                        preference.oysterLoginName = "";
                    }
                }
                else
                {
                    preference.oysterLoginName = new String("name");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_OYSTERPASS_RECORDID);
                if (byteData != null)
                {
                    preference.oysterLoginPassword = new String(byteData);
                    if ((preference.oysterLoginPassword).compareTo(" ") == 0)
                    {
                        preference.oysterLoginPassword = "";
                    }
                }
                else
                {
                    preference.oysterLoginPassword = new String("pass");
                }
                byteData = null;
                
                byteData = prefDB.getRecord(a2B_WRITE_NAME_RECORDID);
                if (byteData != null)
                {
                    preference.writeName = new String(byteData);
                    if ((preference.writeName).compareTo(" ") == 0)
                    {
                        preference.writeName = "";
                    }
                }
                else
                {
                    preference.writeName = new String("7743@a2B");
                }
                byteData = null;

                // 設定しているフォントサイズを読み出す
                fontInfo = prefDB.getRecord(a2B_FONT_SIZE_RECORDID);
                if (fontInfo.length == 32)
                {
                    preference.listFormFontSize    = deSerializeData(fontInfo,  0);
                    preference.viewFormFontSize    = deSerializeData(fontInfo,  4);
                    preference.viewFormBackColor   = deSerializeData(fontInfo,  8);
                    preference.viewFormForeColor   = deSerializeData(fontInfo,  12);
                    preference.watchDogMinutes     = deSerializeData(fontInfo,  16);
                    preference.listFormFunctionKey = deSerializeData(fontInfo,  20);
                    preference.viewFormFunctionKey = deSerializeData(fontInfo,  24);
                    preference.resetSettings       = deSerializeData(fontInfo,  28);
                }

                // a2B設定をクリアする...
                if (preference.resetSettings < 0)
                {
                    preference.resetSettings = 0;
                    isReset = true;
                }
                
                // proxy URL情報の読み出し
                byteData = null;
                byteData = prefDB.getRecord(a2B_PROXYURL_RECORDID);
                if (byteData != null)
                {
                    preference.proxyUrl = new String(byteData);
                    if ((preference.proxyUrl).compareTo(" ") == 0)
                    {
                        preference.proxyUrl = null;
                    }
                }
                else
                {
                    preference.proxyUrl = null;
                }
                byteData = null;
                
                // スレ読み出し中ファイル名の読み出し
                byteData = null;
                byteData = prefDB.getRecord(a2B_CURRENTLOAD_RECORDID);
                if (byteData != null)
                {
                    preference.openedFileName = new String(byteData);
                    if ((preference.openedFileName).compareTo(" ") == 0)
                    {
                        preference.openedFileName = null;
                    }
                }
                else
                {
                    preference.openedFileName = null;
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_BENAME_RECORDID);
                if (byteData != null)
                {
                    preference.beLoginName = new String(byteData);
                    if ((preference.beLoginName).compareTo(" ") == 0)
                    {
                        preference.beLoginName = "";
                    }
                }
                else
                {
                    preference.beLoginName = new String("name");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_BEPASS_RECORDID);
                if (byteData != null)
                {
                    preference.beLoginPassword = new String(byteData);
                    if ((preference.beLoginPassword).compareTo(" ") == 0)
                    {
                        preference.beLoginPassword = "";
                    }
                }
                else
                {
                    preference.beLoginPassword = new String("pass");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_USERAGENT_RECORDID);
                if (byteData != null)
                {
                    preference.changedUserAgent = new String(byteData);
                    if ((preference.changedUserAgent).compareTo(" ") == 0)
                    {
                        preference.changedUserAgent = "Monazilla/1.00  NNsi/1.10(a2B)";
                    }
                }
                else
                {
                    preference.changedUserAgent = new String("Monazilla/1.00  NNsi/1.10(a2B)");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_FILESAVEDIR_RECORDID);
                if (byteData != null)
                {
                    preference.fileSaveDirectory = new String(byteData);
                    if ((preference.fileSaveDirectory).compareTo(" ") == 0)
                    {
                        preference.fileSaveDirectory = "a2B_fetch";
                    }
                }
                else
                {
                    preference.fileSaveDirectory = new String("a2B_fetch");
                }
                byteData = null;

                byteData = prefDB.getRecord(a2B_MEMOFILE_RECORDID);
                if (byteData != null)
                {
                    preference.memoFileName = new String(byteData);
                    if ((preference.memoFileName).compareTo(" ") == 0)
                    {
                        preference.memoFileName = "memo.txt";
                    }
                }
                else
                {
                    preference.memoFileName = new String("memo.txt");
                }

                // 書き込み用キーワードの読み込み
                byteData = null;
                byteData = prefDB.getRecord(a2B_WRITEKEY_RECORDID);
                if (byteData != null)
                {
                    preference.writeKeyString = new String(byteData);
                    if ((preference.writeKeyString).compareTo(" ") == 0)
                    {
                        preference.writeKeyString = null;
                    }
                }
                else
                {
                    preference.writeKeyString = null;
                }

                // 書き込み用Cookieの読み込み
                byteData = null;
                byteData = prefDB.getRecord(a2B_WRITECOOKIE_RECORDID);
                if (byteData != null)
                {
                    preference.writeCookie = new String(byteData);
                    if ((preference.writeCookie).compareTo(" ") == 0)
                    {
                        preference.writeCookie = null;
                    }
                }
                else
                {
                    preference.writeCookie = null;
                }
                byteData = null;
            }

            // レコードを閉じる
            prefDB.closeRecordStore();
            
        }
        catch (Exception e)
        {
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION(restoreRecord)  e:" + e.toString());

            // レコードストアが閉じてなければ閉じる
            try
            {
                if (prefDB != null)
                {
                    prefDB.closeRecordStore();
                }
            }
            catch (Exception e2)
            {
                // 何もしない...
            }
            isReset = true;
        }

        // a2B設定情報をリセットするか？
        if (checkResetPreference(isReset) == true)
        {
            // a2B設定情報をリセットする
            preference.reset();
            
            // 表示ルーチンを呼び出す...
            Thread thread = new Thread()
            {
                public void run()
                {    
                    // a2B設定の格納場所を指定するように変更する
                    parent.showDialog(parent.SHOW_INFO, 3000, "情報", " a2B設定をクリアしました。\n\n データ(bbstable.html)の格納場所を指定してください。");    
//                    parent.showDialog(parent.SHOW_INFO, 600000, "情報", " a2B設定をクリアしました。\n\n データ(bbstable.html)の格納場所を指定してください。");    
//                    parent.showDialog(parent.SHOW_INFO, 0, "情報", " a2B設定をクリアしました。\n\n データ(bbstable.html)の格納場所を指定してください。");    
                }
            };
            thread.start();
            baseDirSettingMode  = true;
        }
        else
        {
            // ローカルあぼーんキーワードを読み込む...
            ngWordManager.restore();
            
            // お気に入りスレ情報を読み込む...
            favoriteManager.restore();

            System.gc();

            // ローカルあぼーんキーワードを空書きする...
            ngWordManager.backup();    
        }
        return (baseDirSettingMode);
    }

    /*
     *   パラメータをバックアップする処理
     *   
     */
    private void backupParameters()
    {
        // 終了時にはインデックスを保存する
        //if (subjectParser.currentSubjectIndexSize() != 0)
        if (baseDirSettingMode != true)
        {            
            subjectParser.leaveSubjectList(preference.currentBbsIndex);
        }

        // ローカルあぼーんキーワードを保存する... (WX310SAではファイルが消えるため、ここで保存できない)
        //ngWordManager.backup();
        
        // お気に入りスレ情報を保存する... (WX310SAではファイルが消えるため、ここで保存できない)
        //favoriteManager.backup();
        
        // ダミーファイル出力処理
        //parent.dummyBackup();

        RecordStore  prefDB = null;
        try
        {
            // レコードストアをオープンする (他アプリから参照・書き込み許可)
            prefDB = RecordStore.openRecordStore("a2B-Settings", true, RecordStore.AUTHMODE_ANY, true);

            // 1レコード目...バージョン番号
            int nofRecord = prefDB.getNumRecords();
            if (nofRecord >= a2B_VERSION_RECORDID)
            {
                // バージョン情報の書き込み(更新)...
                prefDB.setRecord(a2B_VERSION_RECORDID, a2B_PREF_VERSION, 0, a2B_PREF_VERSION.length);
            }
            else
            {
                // バージョン情報の書き込み(追加)...
                prefDB.addRecord(a2B_PREF_VERSION, 0, a2B_PREF_VERSION.length);                    
            }

            // 2レコード目...ベースディレクトリ
            int size = ((preference.getBaseDirectory()).getBytes()).length;
            if (nofRecord >= a2B_BASEDIR_RECORDID)
            {
                // ベースディレクトリ情報の書き込み(更新)...
                prefDB.setRecord(a2B_BASEDIR_RECORDID, (preference.getBaseDirectory()).getBytes(), 0, size);
            }
            else
            {
                // ベースディレクトリ情報の書き込み(追加)...
                prefDB.addRecord((preference.getBaseDirectory()).getBytes(), 0, size);                    
            }

            // オプションのパラメータ
            int optionParameters = 0;
            if (preference.usePicToConvert == true)
            {
                optionParameters = optionParameters + 32768;                
            }
            if (preference.forceSelfKanjiConvert == true)
            {
                optionParameters = optionParameters + 16384;
            }
            if (preference.reserve1 == true)
            {
                optionParameters = optionParameters + 8192;
            }
            if (preference.useWX220JUserAgent == true)
            {
                optionParameters = optionParameters + 4096;
            }
            if (preference.cursorSingleMovingMode == true)
            {
                optionParameters = optionParameters + 2048;
            }
            if (preference.ignoreFavoriteBackup == true)
            {
                optionParameters = optionParameters + 1024;
            }
            if (preference.useCachePicFile == true)
            {
                optionParameters = optionParameters + 512;
            }
            if (preference.useGoogleProxy == true)
            {
                optionParameters = optionParameters + 256;
            }
            if (preference.backupSubjectTxt == true)
            {
                optionParameters = optionParameters + 128;
            }
            if (preference.isShowTitleBusyArea == true)
            {
                optionParameters = optionParameters + 64;
            }
            if (preference.onlyDatabaseAccess == true)
            {
                optionParameters = optionParameters + 32;
            }
            if (preference.divideToGetHttp == true)
            {
                // 
                optionParameters = optionParameters + 16;
            }
            if (preference.saveAtRootDirectory == true)
            {
                // 
                optionParameters = optionParameters + 8;
            }
            if (preference.autoEntryFavorite == true)
            {
                // 次スレ取得時に自動でお気に入りに登録する
                optionParameters = optionParameters + 4;
            }
            if (preference.afterLaunchWebAction == true)
            {
                // WEBブラウザ起動後、終了しない
                optionParameters = optionParameters + 2;
            }

            if (preference.viewScreenMode == true)
            {
                // フルスクリーンモード
                optionParameters = optionParameters + 1;
            }

            int optionDatas = preference.aaWidth;
             
            // 3レコード目...板情報、選択中レス番号...
            size = 72;
            byte[] bbsInfo = new byte[size];
            serializeData(bbsInfo,  0, preference.currentBbsCategoryIndex);
            serializeData(bbsInfo,  4, preference.currentBbsIndex);
            serializeData(bbsInfo,  8, preference.selectedSubjectIndex);
            serializeData(bbsInfo, 12, preference.selectedMessageNumber);
            serializeData(bbsInfo, 16, preference.ngWordDetectMode);
            serializeData(bbsInfo, 20, preference.favoriteShowMode);
            serializeData(bbsInfo, 24, preference.usedProxyScene);
            serializeData(bbsInfo, 28, preference.openCurrentScreen);
            serializeData(bbsInfo, 32, preference.currentMessageIndex);
            serializeData(bbsInfo, 36, preference.currentScreenNumber);
            serializeData(bbsInfo, 40, preference.vibrateDuration);
            serializeData(bbsInfo, 44, optionParameters);
            serializeData(bbsInfo, 48, preference.favoriteShowLevel);
            serializeData(bbsInfo, 52, preference.viewKanjiMode);
            serializeData(bbsInfo, 56, preference.resBatchMode);
            serializeData(bbsInfo, 60, preference.keyRepeatDelayTime);
            serializeData(bbsInfo, 64, optionDatas);
            serializeData(bbsInfo, 68, preference.divideCommunicationSize);

            if (nofRecord >= a2B_CURRENTBBS_RECORDID)
            {
                // 選択中板情報、レス番号の書き込み(更新)...
                prefDB.setRecord(a2B_CURRENTBBS_RECORDID, bbsInfo, 0, size);
            }
            else
            {
                // 選択中板情報、レス番号の書き込み(追加)...
                prefDB.addRecord(bbsInfo, 0, size);                    
            }
            bbsInfo = null;

            // 4レコード目...ユーザ名
            if (preference.oysterLoginName == null)
            {
                preference.oysterLoginName = " ";
            }
            size = (preference.oysterLoginName).getBytes().length;
            if (size == 0)
            {
                preference.oysterLoginName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_OYSTERNAME_RECORDID)
            {
                // ベースディレクトリ情報の書き込み(更新)...
                prefDB.setRecord(a2B_OYSTERNAME_RECORDID, (preference.oysterLoginName).getBytes(), 0, size);
            }
            else
            {
                // ベースディレクトリ情報の書き込み(追加)...
                prefDB.addRecord((preference.oysterLoginName).getBytes(), 0, size);
            }

            // 5レコード目...パスワード
            if (preference.oysterLoginPassword == null)
            {
                preference.oysterLoginPassword = " ";
            }
            size = (preference.oysterLoginPassword).getBytes().length;
            if (size == 0)
            {
                preference.oysterLoginPassword = " ";
                size = 1;
            }
            if (nofRecord >= a2B_OYSTERPASS_RECORDID)
            {
                // ベースディレクトリ情報の書き込み(更新)...
                prefDB.setRecord(a2B_OYSTERPASS_RECORDID, (preference.oysterLoginPassword).getBytes(), 0, size);
            }
            else
            {
                // ベースディレクトリ情報の書き込み(追加)...
                prefDB.addRecord((preference.oysterLoginPassword).getBytes(), 0, size);                    
            }

            // 6レコード目...書き込み時の名前
            if (preference.writeName == null)
            {
                preference.writeName = " ";
            }
            size = (preference.writeName).getBytes().length;
            if (size == 0)
            {
                preference.writeName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_WRITE_NAME_RECORDID)
            {
                // ベースディレクトリ情報の書き込み(更新)...
                prefDB.setRecord(a2B_WRITE_NAME_RECORDID, (preference.writeName).getBytes(), 0, size);
            }
            else
            {
                // ベースディレクトリ情報の書き込み(追加)...
                prefDB.addRecord((preference.writeName).getBytes(), 0, size);                    
            }

            // 7レコード目...フォント情報
            size = 32;
            byte[] fontInfo = new byte[size];
            serializeData(fontInfo,  0, preference.listFormFontSize);
            serializeData(fontInfo,  4, preference.viewFormFontSize);
            serializeData(fontInfo,  8, preference.viewFormBackColor);
            serializeData(fontInfo, 12, preference.viewFormForeColor);
            serializeData(fontInfo, 16, preference.watchDogMinutes);
            serializeData(fontInfo, 20, preference.listFormFunctionKey);
            serializeData(fontInfo, 24, preference.viewFormFunctionKey);
            serializeData(fontInfo, 28, preference.resetSettings);
            if (nofRecord >= a2B_FONT_SIZE_RECORDID)
            {
                // 選択中板情報、フォント情報の書き込み(更新)...
                prefDB.setRecord(a2B_FONT_SIZE_RECORDID, fontInfo, 0, size);
            }
            else
            {
                // 選択中板情報、フォント情報の書き込み(追加)...
                prefDB.addRecord(fontInfo, 0, size);                    
            }
            fontInfo = null;
            
            // 8レコード目...proxy URL
            if (preference.proxyUrl == null)
            {
                preference.proxyUrl = " ";
            }
            size = (preference.proxyUrl).getBytes().length;
            if (size == 0)
            {
                preference.proxyUrl = " ";
                size = 1;
            }
            if (nofRecord >= a2B_PROXYURL_RECORDID)
            {
                // proxyUrl情報の書き込み(更新)...
                prefDB.setRecord(a2B_PROXYURL_RECORDID, (preference.proxyUrl).getBytes(), 0, size);
            }
            else
            {
                // proxyUrl情報の書き込み(追加)...
                prefDB.addRecord((preference.proxyUrl).getBytes(), 0, size);
            }

            // 9レコード目...参照中スレのファイル名
            if (preference.openedFileName == null)
            {
                preference.openedFileName = " ";
            }
            size = (preference.openedFileName).getBytes().length;
            if (size == 0)
            {
                preference.openedFileName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_CURRENTLOAD_RECORDID)
            {
                // 参照中スレのファイル名の書き込み(更新)...
                prefDB.setRecord(a2B_CURRENTLOAD_RECORDID, (preference.openedFileName).getBytes(), 0, size);
            }
            else
            {
                // 参照中スレのファイル名の書き込み(追加)...
                prefDB.addRecord((preference.openedFileName).getBytes(), 0, size);
            }

            // 10レコード目...ユーザ名(Be@2ch)
            if (preference.beLoginName == null)
            {
                preference.beLoginName = " ";
            }
            size = (preference.beLoginName).getBytes().length;
            if (size == 0)
            {
                preference.beLoginName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_BENAME_RECORDID)
            {
                // ＢＥユーザ名の書き込み(更新)...
                prefDB.setRecord(a2B_BENAME_RECORDID, (preference.beLoginName).getBytes(), 0, size);
            }
            else
            {
                // ＢＥユーザ名の書き込み(追加)...
                prefDB.addRecord((preference.beLoginName).getBytes(), 0, size);
            }

            // 11レコード目...パスワード
            if (preference.beLoginPassword == null)
            {
                preference.beLoginPassword = " ";
            }
            size = (preference.beLoginPassword).getBytes().length;
            if (size == 0)
            {
                preference.beLoginPassword = " ";
                size = 1;
            }
            if (nofRecord >= a2B_BEPASS_RECORDID)
            {
                // パスワードのの書き込み(更新)...
                prefDB.setRecord(a2B_BEPASS_RECORDID, (preference.beLoginPassword).getBytes(), 0, size);
            }
            else
            {
                // パスワードの書き込み(追加)...
                prefDB.addRecord((preference.beLoginPassword).getBytes(), 0, size);                    
            }

            // 12レコード目...ユーザエージェント情報            
            if (preference.changedUserAgent == null)
            {
                preference.changedUserAgent = " ";
            }
            size = (preference.changedUserAgent).getBytes().length;
            if (size == 0)
            {
                preference.changedUserAgent = " ";
                size = 1;
            }
            if (nofRecord >= a2B_USERAGENT_RECORDID)
            {
                // ユーザエージェント情報の書き込み(更新)...
                prefDB.setRecord(a2B_USERAGENT_RECORDID, (preference.changedUserAgent).getBytes(), 0, size);
            }
            else
            {
                // ユーザエージェント情報の書き込み(追加)...
                prefDB.addRecord((preference.changedUserAgent).getBytes(), 0, size);                    
            }

            // 13レコード目...取得ファイル記録ディレクトリ
            if (preference.fileSaveDirectory == null)
            {
                preference.fileSaveDirectory = " ";
            }
            size = (preference.fileSaveDirectory).getBytes().length;
            if (size == 0)
            {
                preference.fileSaveDirectory = " ";
                size = 1;
            }
            if (nofRecord >= a2B_FILESAVEDIR_RECORDID)
            {
                // 取得ファイル記録ディレクトリの書き込み(更新)...
                prefDB.setRecord(a2B_FILESAVEDIR_RECORDID, (preference.fileSaveDirectory).getBytes(), 0, size);
            }
            else
            {
                // 取得ファイル記録ディレクトリの書き込み(追加)...
                prefDB.addRecord((preference.fileSaveDirectory).getBytes(), 0, size);                    
            }

            // 14レコード目...メモファイル名
            if (preference.memoFileName == null)
            {
                preference.memoFileName = " ";
            }
            size = (preference.memoFileName).getBytes().length;
            if (size == 0)
            {
                preference.memoFileName = " ";
                size = 1;
            }
            if (nofRecord >= a2B_MEMOFILE_RECORDID)
            {
                // メモファイル名の書き込み(更新)...
                prefDB.setRecord(a2B_MEMOFILE_RECORDID, (preference.memoFileName).getBytes(), 0, size);
            }
            else
            {
                // メモファイル名の書き込み(追加)...
                prefDB.addRecord((preference.memoFileName).getBytes(), 0, size);                    
            }

            // 15レコード目...書き込み用キーワード
            if (preference.writeKeyString == null)
            {
                preference.writeKeyString = " ";
            }
            size = (preference.writeKeyString).getBytes().length;
            if (size == 0)
            {
                preference.writeKeyString = " ";
                size = 1;
            }
            if (nofRecord >= a2B_WRITEKEY_RECORDID)
            {
                // 書き込み用キーワードの書き込み(更新)...
                prefDB.setRecord(a2B_WRITEKEY_RECORDID, (preference.writeKeyString).getBytes(), 0, size);
            }
            else
            {
                // 書き込み用キーワードの書き込み(追加)...
                prefDB.addRecord((preference.writeKeyString).getBytes(), 0, size);                    
            }

            // 16レコード目...書き込み用Cookie
            if (preference.writeCookie == null)
            {
                preference.writeCookie = " ";
            }
            size = (preference.writeCookie).getBytes().length;
            if (size == 0)
            {
                preference.writeCookie = " ";
                size = 1;
            }
            if (nofRecord >= a2B_WRITECOOKIE_RECORDID)
            {
                // 書き込み用Cookieの書き込み(更新)...
                prefDB.setRecord(a2B_WRITECOOKIE_RECORDID, (preference.writeCookie).getBytes(), 0, size);
            }
            else
            {
                // 書き込み用Cookieの書き込み(追加)...
                prefDB.addRecord((preference.writeCookie).getBytes(), 0, size);                    
            }

            // レコードを閉じる
            prefDB.closeRecordStore();
        }
        catch (Exception e)
        {
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION(backupRecord)  e:" + e.getMessage());

            // レコードストアが閉じてなければ閉じる
            try
            {
                if (prefDB != null)
                {
                    prefDB.closeRecordStore();
                }
            }
            catch (Exception e2)
            {
                // 何もしない...
            }
        }
        preference = null;
        return;
    }

    /*
     *  データを int型からバイト列へ変換する
     * 
     * 
     */
    private void serializeData(byte[] buffer, int offset, int convId)
    {
        buffer[offset + 0] = (byte)((convId >> 24)&(0xff));
        buffer[offset + 1] = (byte)((convId >> 16)&(0xff));
        buffer[offset + 2] = (byte)((convId >>  8)&(0xff));
        buffer[offset + 3] = (byte)((convId      )&(0xff));
        return;
    }
    
    /*
     *  バイト列データをint型へ変換する
     * 
     * 
     */
    private int deSerializeData(byte[] buffer, int offset)
    {
        return ((((buffer[offset + 0]&(0xff))<< 24) + ((buffer[offset + 1]&(0xff))<< 16) +
                 ((buffer[offset + 2]&(0xff))<<  8) + ((buffer[offset + 3]&(0xff)))));
    }    
    
    /*
     *   Dialog消去用のコマンドリスナクラス
     *   
     */
    public class a2BdialogListener implements CommandListener
    {
        a2BMain parent;
        
        // コンストラクタ
        public a2BdialogListener(a2BMain object)
        {
            parent = object;
        }

        // コマンド受信...
        public void commandAction(Command c, Displayable d)
        {
            // ダイアログを隠す...
            parent.hideDialog();
            return;
        }
    }
    
    /*
     *   a2B設定保持クラス...
     *   
     */
    public class a2BPreferenceDatabase
    {
//        ////// 定数の定義 //////
        private final int SLEEP_WATCHDOG_TIME   = 5;  // 5分待つ... (最長10分程度)

        private a2BMain parent = null;

        private String   baseDirectory          = null;
        private byte    confirmationMode       = 0;

        // 前回のカテゴリインデックスと板インデックスを記憶する
        public int    previousBbsCategoryIndex = -1;
//        public int    previousBbsIndex         = -1;

        // 現在のBBSカテゴリとインデックス
        public int     currentBbsCategoryIndex = 0;
        public int     currentBbsIndex         = 0;
        public int     currentMessageIndex     = 0;
        public int     currentScreenNumber     = 0;
        
        // 現状の画面モード...
        public int   openCurrentScreen         = 0;
                
        // 現在の選択中スレとレス番号
        public int     selectedSubjectIndex    = 0;
        public int     selectedMessageNumber   = 0;

        public int     ngWordDetectMode        = 0;
        public int     favoriteShowMode        = 0;
        public int     favoriteShowLevel       = 0;
        
        public int     listFormFunctionKey     = 4;
        public int     viewFormFunctionKey     = 7;

        public int     keyRepeatDelayTime      = 400;        

        public int     viewKanjiMode              = 0;        

        public int     vibrateDuration         = 300;
        public int     divideCommunicationSize = 49100;
//        public boolean backgroundComm         = true;
        public boolean afterLaunchWebAction   = false;
        public boolean autoEntryFavorite      = false;
        public boolean saveAtRootDirectory    = false;
        public boolean divideToGetHttp        = false;
        public boolean onlyDatabaseAccess     = false;
        public boolean viewScreenMode         = false;
        public boolean isShowTitleBusyArea    = false;
        public boolean backupSubjectTxt       = false;
        public boolean ignoreFavoriteBackup   = false;
        public boolean useGoogleProxy         = false;
        public boolean useWX220JUserAgent     = false;
        public boolean useCachePicFile        = false;
        public boolean cursorSingleMovingMode = false;
        public boolean forceSelfKanjiConvert  = false;
        public boolean usePicToConvert        = false;
        public boolean reserve1               = false;

        public int     listFormFontSize        = Font.SIZE_SMALL;
        public int     viewFormFontSize        = Font.SIZE_SMALL;
        
        public int     viewFormBackColor       = 0x00ffffff;
        public int     viewFormForeColor       = 0x00000000;

        public int     resBatchMode            = 1 + (256 * 3);
        
        public int     watchDogMinutes         = SLEEP_WATCHDOG_TIME;
        public long    oysterLoginDateTime     = 0;
        public String   oysterLoginName         = null;
        public String   oysterLoginPassword     = null;
        public String   oysterLoginSessionId    = null;
        public String   oysterLoginUserAgent    = null;
        public String   beLoginName             = null;
        public String   beLoginPassword         = null;
        public String   beCookie                = null;
        public long    beLoginDateTime         = 0;
        public String   writeName               = "7743@a2B";
        public String   writeKeyString          = null;
        public String   writeCookie             = null;
        
        public String   proxyUrl                = null;
        public String   changedUserAgent        = "Monazilla/1.00  NNsi/1.10(a2B)";
        public String   fileSaveDirectory       = "a2B_fetch";
        public String   memoFileName            = "memo.txt";
        public int     usedProxyScene          = 0;
        public int     resetSettings           = 0;
        public int     aaWidth                 = 99;
        public String   openedFileName          = null;

        // コンストラクタ
        public a2BPreferenceDatabase(a2BMain object)
        {
            // プラットフォーム名称を調べ、WX310だった場合には、gzip転送対策を設定する
            String platForm = System.getProperty("microedition.platform");
            if (platForm.indexOf("WX310") >= 0)
            {
                usedProxyScene = 4;
            }

            parent = object;
            confirmationMode  = parent.SHOW_DEFAULT;
        }

        // 設定を初期化する
        public void reset()
        {
            baseDirectory      = null;
            vibrateDuration    = 300;
            confirmationMode   = parent.SHOW_DEFAULT;

            // ガベコレ実施
            System.gc();
            return;
        }

        /**
         *  フォント情報を更新する
         *
         */
        public void updateFontData(boolean isReset, int fontSize)
        {
        	if (isReset == true)
        	{
                listFormFontSize   = Font.SIZE_SMALL;
                viewFormFontSize   = Font.SIZE_SMALL;
                viewFormBackColor  = 0x00ffffff;
                viewFormForeColor  = 0x00000000;
        	}
        	else
        	{
                listFormFontSize   = fontSize;
                viewFormFontSize   = fontSize;        		
        	}
            return;
        }        
        
        // データ格納ベースディレクトリを設定する
        public void setBaseDirectory(String directory)
        {
            baseDirectory = directory;

            // 記憶ディレクトリを表示する。
            parent.showDialog(parent.SHOW_DEBUG, 0, "Debug", "データディレクトリは、 '" + baseDirectory + "' です。");
        }

        // データ格納ベースディレクトリを返す
        public String getBaseDirectory()
        {
            return (baseDirectory);
        }

        // 応答モードの設定
        public void setConfirmationMode(byte mode)
        {
            confirmationMode = mode;
        }

        // 応答モード設定状態を応答する
        public byte getConfirmationMode()
        {
            return (confirmationMode);
        }    
    }
}
//--------------------------------------------------------------------------
//  a2BobjectFactory  --- オブジェクト管理クラス
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
