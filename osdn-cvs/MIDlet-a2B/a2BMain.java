import javax.microedition.midlet.*;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.*;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.Random;
import java.util.Vector;
import java.util.Date;

/**
 * a2Bのメインクラス。(アプリケーション全体のクラス間の調整役としても利用する。)
 * 
 * @author MRSa
 *
 */
public class a2BMain extends MIDlet
{
    // HTTP(GET)通信シーン
    public final int  SCENE_PARTGET_MESSAGE      = 1;   // スレ差分取得を実施した場合...
    public final int  SCENE_WHOLEGET_MESSAGE     = 2;   // スレ全部取得を実施した場合...
    public final int  SCENE_BBSTABLE_MESSAGE     = 3;   // 板一覧全部取得を実施した場合...
    public final int  SCENE_GETSUBJECTLIST       = 4;   // スレ一覧全部取得を実施した場合...
    public final int  SCENE_GETALL_MESSAGE       = 5;   // スレ新着確認を実施した場合...
    public final int  SCENE_PREVIEW_MESSAGE      = 6;   // プレビュー指示の場合...
    public final int  SCENE_GETRESERVE_MESSAGE   = 7;   // スレ新着確認(取得予約スレ)を実施した場合...
    public final int  SCENE_PREVIEW_MESSAGE_EUC  = 8;   // プレビュー指示(EUC)の場合...
    public final int  SCENE_PREVIEW_MESSAGE_SJIS = 9;   // プレビュー指示(SJIS)の場合...
    public final int  SCENE_GET_OSUSUME_2CH      = 10;  // おすすめ２ちゃんねるを表示する場合...
    public final int  SCENE_GET_FIND_2CH         = 11;  // 2ちゃんねる検索でスレタイトルを取得する場合...
    public final int  SCENE_GET_RELATE_KEYWORD   = 12;  // 関連キーワード検索...
    
    // HTTP通信シーン
    public final int  SCENE_MESSAGE_WRITE    = 101; // レス書き込みを実施した場合...
    public final int  SCENE_TITLE_SEARCH     = 201; // スレタイ検索モード
    public final int  SCENE_MESSAGE_SEARCH   = 202; // スレ内検索モード
    public final int  SCENE_NGWORD_INPUT     = 221; // NGワード登録モード
    public final int  SCENE_MEMOFILE_INPUT   = 222; // メモファイル名入力モード
    public final int  SCENE_THREADNUM_INPUT  = 250; // スレッド番号直接入力モード
    public final int  SCENE_PROXY_SETTING    = 300; // a2B設定(Proxy設定)モード
    public final int  SCENE_RENAMEFILE_INPUT = 310; // ファイル名変更入力モード
    public final int  SCENE_COPYFILE_INPUT   = 320; // コピーファイル名入力モード
    public final int  SCENE_GETHTTP_INPUT    = 330; // http受信URL入力モード
    
    private final int FILE_BUFFER_SIZE  = 5120;  // バッファサイズ
    private final int MARGIN            = 10;    // バッファのマージン
    static public final int TEMP_READ_SIZE    = 32768; // ファイル読み出しサイズ
    static public final int TEMP_PREVIEW_SIZE = 16384; // プレビューデータサイズ

    // a2B稼動状態
    private final byte a2B_EXECUTE_UNDEFINED     = (byte) 0;  // 起動前
    private final byte a2B_EXECUTE_PREPARE       = (byte) 1;  // 準備中 (recordStore読み出し中)
    private final byte a2B_EXECUTE_DIRSETTING    = (byte) 2;  // データ格納ディレクトリ設定中
    private final byte a2B_EXECUTE_STARTED       = (byte) 3;  // a2B起動直後
    private final byte a2B_EXECUTE_SHOWLIST      = (byte) 4;  // スレ一覧表示中
    private final byte a2B_EXECUTE_VIEW_READONLY = (byte) 5;  // ファイラーからスレ参照中 (参照モード)
    private final byte a2B_EXECUTE_VIEW_MANAGED  = (byte) 6;  // (通常の)スレ参照中 (管理モード)    

    // PROXY使用設定
    public final int PROXY_NOTUSE           = 0;   // 利用しない
    public final int PROXY_USE_ALL          = 1;   // 利用する
    public final int PROXY_USE_ONLYGET      = 2;   // GET時のみ利用する
    public final int PROXY_USE_ONLYRANGE    = 3;   // 差分取得のみ利用する
    public final int PROXY_WORKAROUND_WX310 = 4;   // WX310向けの回避策を行う

    public final int SCENE_LISTFORM         = 0;
    public final int SCENE_VIEWFORM         = 1;

    public final int FEATURE_CHANGE_FONT    = 0;   // フォント変更
    public final int FEATURE_NUMBER_MOVE    = 2;   // 数字キー移動モード
    public final int FEATURE_PICKUP_NOTREAD = 1;   // 未読スレ抽出
    
    public static final int a2B_HTTPMODE_NOUSECOOKIE = -1;
    public static final int a2B_HTTPMODE_PICKUPCOOKIE = 0;
    public static final int a2B_HTTPMODE_USEPREVIOUSCOOKIE = 1;

    // ダイアログ表示レベル
    public final byte SHOW_DEBUG     = (byte) 100;   // デバッグモード
    public final byte SHOW_EXCEPTION = (byte)  70;   // 例外発生モード
    public final byte SHOW_DEFAULT   = (byte)  50;   // ダイアログ表示範囲 (この値より大きい場合にはダイアログ非表示)
    public final byte SHOW_INFO      = (byte)  30;   // 情報モード
    public final byte SHOW_WARN      = (byte)  20;   // 警告モード
    public final byte SHOW_CONFIRM   = (byte)  10;   // 確認モード
    public final byte SHOW_ERROR     = (byte)   1;   // エラーモード

    // カテゴリ選択モード
    public final byte SHOW_CATEGORYLIST           = (byte) 10;
    public final byte SHOW_BBSLIST                = (byte) 20;
    public final byte SHOW_SUBJECTCOMMAND         = (byte) 30;
    public final byte SHOW_SUBJECTCOMMANDFAVORITE = (byte) 40;
    public final byte SHOW_SUBJECTCOMMANDSECOND   = (byte) 44;
    public final byte SHOW_LOGINMENU              = (byte) 46;
    public final byte SHOW_VIEWCOMMANDSECOND      = (byte) 49;
    public final byte CONFIRM_DELETELOGDAT        = (byte) 50;
    public final byte CONFIRM_DELETELOGFILE       = (byte) 60;
    public final byte CONFIRM_URL_FOR_BROWSER     = (byte) 120;
    public final byte CONFIRM_GET_THREADNUMBER    = (byte) 140;
    public final byte CONFIRM_SET_THREADNUMBER    = (byte) 145;
    public final byte CONFIRM_SELECT_OSUSUME2CH   = (byte) 160;
    public final byte CONFIRM_SELECT_FIND2CH      = (byte) 165;
    public final byte CONFIRM_SELECT_RELATE_KEYWORD  = (byte) 166;

    // アクション
    public final int  ACTION_NOTHING_CANCEL       = -1;
    public final int  ACTION_NOTHING              = 0;
    public final int  ACTION_THREAD_OPERATION     = 1;
    public final int  ACTION_THREAD_RESERVE       = 2;
    public final int  ACTION_THREAD_GETLOG        = 3;
    
    // クラス内変数の定義
    private a2BobjectFactory  objectFactory      = null;    // オブジェクト管理クラス
    private Displayable       currentForm        = null;    // 現在表示中フォーム
    private Displayable       previousForm       = null;    // 前回表示中のフォーム

    private boolean          isReserveSequence  = false;
    private boolean          isGetNewArrival    = false;   // 新着確認実施中...
    private boolean          isShowDialog       = false;   // ダイアログ表示中...    
    private byte             a2BexecutingStatus  = a2B_EXECUTE_UNDEFINED;  // a2B稼動状態    
    private byte[]          scratchBuffer        = null;    // 一時的にデータを格納するバッファ(byte[]型)
    private String           parseTargetString    = null;    //  一時的にデータを格納するバッファ(String型)
    private String           be2chCookie          = null;    //  一時的にCookie情報を格納するバッファ
    private String           previousHttpCookie   = null;    //  一時的にCookie情報を格納するバッファ
    public  long            watchDogMarking       = -1;       //  ウォッチドッグのマーキング

    /**
     *   コンストラクタ...
     * 
     */
    public a2BMain()
    {
        // オブジェクトを生成しておく
        objectFactory = new a2BobjectFactory(this);
    }

    /**
     *   アプリケーション起動時に呼び出される処理
     * 
     */
    protected void startApp() throws MIDletStateChangeException
    {
        // 再起動されたときには、何もしない
        if (a2BexecutingStatus != a2B_EXECUTE_UNDEFINED)
        {
            return;
        }

        // a2B稼動状態を準備中へ変更する
        a2BexecutingStatus = a2B_EXECUTE_PREPARE;

        // スプラッシュスクリーンを表示する...
        a2BSplashScreen startScreen = new a2BSplashScreen();
        Display.getDisplay(this).setCurrent(startScreen);

        // 準備実行!
        if (objectFactory.prepare() != true)
        {
            // a2B稼動状態を STARTEDに変更する
            a2BexecutingStatus = a2B_EXECUTE_STARTED;
            
            // 板一覧情報の構築
            (objectFactory.bbsCommunicator).prepareBbsDatabase();
        }
        else
        {
            // a2B稼動状態を DIRSETTINGに変更する
            a2BexecutingStatus = a2B_EXECUTE_DIRSETTING;

            // ディレクトリ選択モード
            openFileSelector();
        }

        // ウォッチドッグタイマの起動
        startWatchDog();

        // 起動画面のあとしまつ...
        startScreen.finish();
        startScreen = null;
        return;
    }

    /**
     *   アプリケーション一時停止時に呼び出される処理
     *  
     */
    protected void pauseApp()
    {
        // 現在のところ、何もしない...
        return;
    }

    /**
     *   アプリケーション正常終了時に呼び出される処理
     *   
     */
    protected void destroyApp(boolean arg0) throws MIDletStateChangeException
    {
        if (arg0 == true)
        {
            // データのバックアップとオブジェクトの削除を実行する
            objectFactory.cleanup();
        }

        // 終了指示
        notifyDestroyed();
        return;
    }
    
    /*------------------------------------------------------------------------------------------------------------*/

    /**
     *   ウオッチドッグスレッド
     * 
     */
    public void startWatchDog()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                Date  date      = new Date();
                long time      = date.getTime();
                long dummy     = time;
                long sleepTime = (objectFactory.preference).watchDogMinutes * 1000 * 60;
                if (sleepTime == 0)
                {
                    // ウォッチドッグの監視は行わない
                    return;
                }
                while (dummy != 0)
                {
                    if (watchDogMarking != -1)
                    {
                        // ウォッチドッグ停止中ではなかったとき、、、
                        time = date.getTime();
                        watchDogMarking = time;
                    }
                    try
                    {
                        Thread.sleep(sleepTime); // 待つ...
                    }
                    catch (Exception e)
                    {
                        // 何もしない...
                    }
                    if (time == watchDogMarking)
                    {
                        // タイムアウト検出、、、アプリケーションを終了する
                        objectFactory.backupFavorite();
                        objectFactory.cleanup();
                        notifyDestroyed();
                        return;
                    }
                }
            }
        };
        thread.start();
        return;
    }

    /**
     *  ウオッチドッグをクリアする。。
     *
     */
    public void keepWatchDog(int number)
    {
        watchDogMarking = number;
        return;
    }
    
    /**
     *   スレ参照画面を抜けてきた時の処理...
     *   
     *       @param  threadNumber スレ番号
     *       @param  maxNumber    最後に参照したレス番号
     *
     */
    public void EndViewForm(int threadNumber, int maxNumber)
    {
        keepWatchDog(-1);
        hideBusyMessage(); 

        // 再起動時のスレ読み出し情報として記録していたものをすべてクリアする
        if ((objectFactory.preference).openCurrentScreen > 0)
        {
            (objectFactory.preference).openCurrentScreen = 0;
        }
        (objectFactory.preference).currentMessageIndex = 0;
        (objectFactory.preference).currentScreenNumber = 0;
        (objectFactory.preference).openedFileName      = null;

        if (a2BexecutingStatus == a2B_EXECUTE_VIEW_MANAGED)
        {
            // 管理スレを表示中に参照画面から抜けてきた場合は、レス番号をDBに記録する
            (objectFactory.listForm).setCurrentMessage(threadNumber, maxNumber);
            openListForm(false);
        }
        else if (a2BexecutingStatus == a2B_EXECUTE_VIEW_READONLY)
        {
            // ログ参照モードで参照画面から抜けてきた場合は、ディレクトリ参照モードにする
            openFileSelector();
        }

        // 一覧モード参照中に設定する
        a2BexecutingStatus = a2B_EXECUTE_SHOWLIST;
        return;
    }
    
    /** 
     *   a2B終了処理...
     *   
     */
    public void quit_a2B()
    {
        keepWatchDog(-1);
        (objectFactory.listForm).setBusyMessage("a2B終了中...", false);
        (objectFactory.viewForm).setBusyMessage("a2B終了中...", false);

        // (念のため)お気に入りのデータをバックアップする
        (objectFactory.favoriteManager).doBackup();

        // a2B終了を指示された場合...
        try
        {
            // アプリ終了を呼び出す...
            destroyApp(true);
        }
        catch (Exception e)
        {
            // 例外処理としては何もしない
        }
        return;
    }

    /**
     *  スレ一覧画面をログファイルオープンモードで抜けてきた時の処理...
     * 
     *     @param  fileName   ログファイル名(*.dat)
     *     @param  msgIndex   データ管理番号
     *     @param  number     オープンするレス番号
     *  
     */
    public void EndListFormShowMode(String fileName, int msgIndex, int number)
    {
        keepWatchDog(-1);
        (objectFactory.listForm).setBusyMessage("スレ解析中...", true);

        // スレ参照画面を開く...
        if ((objectFactory.preference).openCurrentScreen == 0)
        {
            (objectFactory.preference).openCurrentScreen = 1;
        }

        // 管理スレ参照中状態に変更する
        a2BexecutingStatus = a2B_EXECUTE_VIEW_MANAGED;
        openViewForm((objectFactory.preference).getBaseDirectory() + fileName, msgIndex, number);
        return;
    }

    /**
     *  スレ一覧画面をdatファイル参照モードで抜けてきた時の処理...
     * 
     */
    public void EndListFormLogViewMode()
    {
        keepWatchDog(-1);
        (objectFactory.viewForm).setBusyMessage(null, false);

        // ファイル選択画面を開く
        openFileSelector();
        return;
    }
    
    /**
     *   書き込みフォームを抜けてきたときの処理
     * 
     * 
     */
    public void returnToPreviousForm(int afterAction)
    {
        // 前回表示していたフォームに切り替える
        keepWatchDog(0);
        Display.getDisplay(this).setCurrent(previousForm);
        currentForm  = previousForm;
        previousForm = null;
        
        // 何もしない...
        if (afterAction == ACTION_NOTHING_CANCEL)
        {
            // 検索実施用文字列をリセットする
            (objectFactory.listForm).setSearchTitleTemporary(null);
            return;
        }
        
        // 何もしない...
        if (afterAction == ACTION_NOTHING)
        {
            return;
        }

        // アイテム選択を実施する。
        if (afterAction == ACTION_THREAD_OPERATION)
        {
            (objectFactory.listForm).selectedItem(false);        
            return;
        }

        // スレ取得予約を実施する
        int  index  = 1;   // ※注※ この番号は、「スレ操作」コマンドのインデックス番号と一致していなければならない。
        byte scene  = SHOW_SUBJECTCOMMAND;        
        if (afterAction == ACTION_THREAD_RESERVE)
        {
            if (getFavoriteShowMode() == false)
            {
                // 取得予約は、「お気に入り」ではできないため、、、、。
                index = 2;
                (objectFactory.listForm).executeCommandAction(scene, index, -1);
            }
            return;
        }

        // スレを取得する
        if (afterAction == ACTION_THREAD_GETLOG)
        {
            if (getFavoriteShowMode() == true)
            {
                // お気に入りのときの取得
                scene = SHOW_SUBJECTCOMMANDFAVORITE;
                index = 1;
            }
            else
            {
                // 通常板の取得
                index = 3;
            }
            (objectFactory.listForm).executeCommandAction(scene, index, -1);
            return;
        }    
        return;
    }

    /**
     *   書き込みが可能なスレかどうかを応答する
     * 
     */    
    public boolean canWriteMessage()
    {
        if (a2BexecutingStatus == a2B_EXECUTE_VIEW_MANAGED)
        {
            return (true);
        }
        return (false);
    }
        
    /**
     *   ファイル選択の終了(ファイルを選択した場合)
     *
     *      @param  fileName  選択したファイル名
     */
    public void EndFileSelector(String fileName)
    {
        // ベースディレクトリ設定モードのとき...
        if (a2BexecutingStatus == a2B_EXECUTE_DIRSETTING)
        {
            // ディレクトリ名のみに削る...
            int pos = fileName.lastIndexOf('/', fileName.length() - 2);
            
            // ベースディレクトリをファイルに設定する
            (objectFactory.preference).setBaseDirectory(fileName.substring(0, pos + 1));

            // ベースディレクトリをファイルセレクタのデフォルト値とする
            (objectFactory.fileSelector).setnextPath((objectFactory.preference).getBaseDirectory());

            // 記憶ディレクトリを表示する。
            showDialog(SHOW_INFO, 0, "Information", "データディレクトリを " + (objectFactory.preference).getBaseDirectory() + " に設定しました。\n\n 一度a2Bを終了します。");

            // 少し(1.0s)止まる...
            sleepTime(1000);

            // 一度アプリケーションを終了する
            try
            {
                destroyApp(true);
            }
            catch (Exception e)
            {
                // 例外発生時にはなにもしない
            }
            notifyDestroyed();

            return;
        }

        // ファイル選択モードで、ファイル名が指定されていない場合...
        if (fileName == null)
        {
            // 一覧画面を開く
            System.gc();

            // 一覧モード参照中に設定する
            a2BexecutingStatus = a2B_EXECUTE_SHOWLIST;

            openListForm(true);
            return;
        }

        // ファイル選択モードのとき...
        // (指定されたファイルを先頭からオープンする)
        a2BexecutingStatus = a2B_EXECUTE_VIEW_READONLY;

        showDialog(SHOW_DEBUG, 0, "Debug", "選択したファイル名：" + fileName);
        openViewForm(fileName, -1, 1);
        return;
    }

    /**
     *    URLをWebブラウザで開く
     *     (a2B終了後にブラウザを起動する)
     * 
     *    @param url ブラウザで接続するURL
     * 
     */
    public void openWebBrowser(String url)
    {
        // (念のため)お気に入りのデータをバックアップする
        (objectFactory.favoriteManager).doBackup();
        
        String openUrl = url;
        try
        {
            if ((getUseGoogleProxy() == true)&&(url.indexOf("http://") == 0))
            {
            	if (url.indexOf("www.google.co.jp/m/") < 0)
            	{
            		// google のプロキシを使用する設定でない場合...
//                  openUrl = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + url.substring(7) + "&output=chtml";
                    openUrl = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + url.substring(7);
            	}
            }

            // URLのオープン指示を行う
            this.platformRequest(openUrl);

            // URLオープン後に継続するかどうか確認する
            if (getLaunchMode() != true)
            {
                // 継続しない場合、アプリ終了を呼び出す...
                destroyApp(true);
            }
        }
        catch (Exception e)
        {
            /////// ブラウザ起動は未サポートだった、何もしない  /////
        }
        return;
    }

    /**
     *  ダイアログを表示する
     * 
     *      @param catagory  ダイアログ表示のカテゴリ
     *      @param timeout   ダイアログ表示するタイムアウト値 (単位：ms, 0のときは無限大待ち)
     *      @param title     表示するダイアログタイトル
     *      @param message   表示するダイアログのメッセージ
     *  
     *      @return  応答コード (0：OK, 0以外：NG)
     */
    public int showDialog(byte category, int timeout, String title, String message)
    {
        AlertType alertType = null;  // alertType が nullだと、WX310SAでは音が鳴らない

        // カテゴリが「確認」だった場合にはalertTypeを変更する
        if (category == SHOW_CONFIRM)
        {
            alertType = AlertType.CONFIRMATION;
        }

        if ((objectFactory.preference).getConfirmationMode() < category)
        {
            // 設定が「ダイアログを表示しない」になっていた場合には、表示せず終了する
            return (0);
        }

        if (timeout < 0)
        {
            // 表示タイムアウト値が負だった場合には、ダイアログを表示せずに終了する
            return (0);
        }

        if (timeout != 0)
        {
            // タイムアウト値がゼロ以外だったらタイムアウト値を設定する
            (objectFactory.dialogForm).setTimeout(timeout);
        }
        else
        {
            // タイムアウト値がゼロのときには、無限待ち(FOREVER)にする
            // (この場合には、ダイアログを使うのではなく、BUSYフォームを使う)
            (objectFactory.selectForm).showBusyMessage(title, message, a2BselectionForm.CONFIRMATION_FORM);
            if (isShowDialog == true)
            {
                previousForm = currentForm;
                Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.selectForm);
            }
            else
            {
                previousForm = (Display.getDisplay(this)).getCurrent();
                Display.getDisplay(this).setCurrent(objectFactory.selectForm);
            }
            currentForm  = (Display.getDisplay(this)).getCurrent();
            return (0);
        }

        // ダイアログ表示準備...
        (objectFactory.dialogForm).setTitle(title);
        (objectFactory.dialogForm).setString(message);
        (objectFactory.dialogForm).setType(alertType);

        // ダイアログを画面表示する
        if ((objectFactory.dialogForm) == (Display.getDisplay(this)).getCurrent())
        {
            return (0);
        }
        currentForm = (Display.getDisplay(this)).getCurrent();
        Display.getDisplay(this).setCurrent((objectFactory.dialogForm), currentForm);
        
        isShowDialog = true;
        return (0);
    }

    /**
     *   ダイアログを隠す
     * 
     */
    public void hideDialog()
    {
        isShowDialog = false;
        Display.getDisplay(this).setCurrent(currentForm);
        return;
    }

    /**
     *   ビジーメッセージを表示する
     * 
     * 
     */
    public void showBusyMessage(String title, String message, int usage)
    {
        if (isGetNewArrival == true)
        {
            keepWatchDog(0);
            (objectFactory.listForm).setBusyMessage(message, false);
            (objectFactory.viewForm).setBusyMessage(message, false);
            return;
        }        

        (objectFactory.selectForm).showBusyMessage(title, message, usage);
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.selectForm);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.selectForm);
        }
        return;
    }

    /**
     *   ビジーメッセージを更新する
     * 
     * 
     */
    public void updateBusyMessage(String title, String message, boolean forceUpdate)
    {
        keepWatchDog(0);
        (objectFactory.listForm).setBusyMessage(message, forceUpdate);
        (objectFactory.viewForm).setBusyMessage(message, forceUpdate);
        (objectFactory.selectForm).showBusyMessage(title, message, a2BselectionForm.BUSY_FORM_UPDATE);
        return;
    }

    /**
     *   ビジーメッセージを隠す
     * 
     */
    public void hideBusyMessage()
    {
        keepWatchDog(0);
        (objectFactory.listForm).setBusyMessage(null, false);
        (objectFactory.viewForm).setBusyMessage(null, false);
        (objectFactory.selectForm).hideBusyMessage();
        return;
    }

    /**
     *   板一覧データベースの構築が完了した時の処理
     *   
     */
    public void completedPrepareBbsDatabase()
    {
        // 板一覧の取得が終了したことをデバッグ表示する
        showDialog(SHOW_DEBUG, 0, "Debug", "板一覧解析終了");
        
        // ガベコレ実施
        System.gc();

        // スクリーンサイズを設定する
        (objectFactory.listForm).prepareScreenSize();

        // a2B起動直後かどうか
        if (a2BexecutingStatus == a2B_EXECUTE_STARTED)
        {
            if ((objectFactory.preference).openCurrentScreen < 0)
            {
                // a2Bを必ずお気に入り一覧で起動させる場合...お気に入り設定に切り替える。
                (objectFactory.preference).favoriteShowMode = 1;
            }
        }

        // 板一覧画面を表示する
        openListForm(true);
        return;
    }

    /**
     *   スレ一覧の表示準備ができたときに呼び出す...
     * 
     * 
     */
    public void readySubjectListDisplay()
    {
        // a2B起動直後かどうか
        if (a2BexecutingStatus == a2B_EXECUTE_STARTED)
        {
            // 既読スレ参照モードかどうか
            (objectFactory.subjectParser).setOnlyDatabaseAccess((objectFactory.preference).onlyDatabaseAccess);
    
            // 前回a2B終了時にスレを参照していたかどうかのチェック
            if ((objectFactory.preference).openCurrentScreen > 0)
            {
                // a2B初回オープン時、前回の参照スレと現在の情報が整合しているかチェックする
                int currentNumber = checkFirstOpenScreen((objectFactory.preference).openedFileName, 
                                                          (objectFactory.preference).currentMessageIndex);
                if (currentNumber > 0)
                {
                    // 前回終了したときの情報と整合していた、前回参照していたスレのレスを表示する
                    (objectFactory.listForm).setCurrentMessageIndex((objectFactory.preference).currentMessageIndex);
                    a2BexecutingStatus = a2B_EXECUTE_VIEW_MANAGED;
                    openViewForm((objectFactory.preference).openedFileName, 
                                 (objectFactory.preference).currentMessageIndex,
                                 currentNumber);
                    return;
                }
            }
        }

        // スレ一覧表示準備が完了したことを一覧画面に通知する
        (objectFactory.listForm).readySubjectList();

        // スレ一覧表示中状態へ変更する
        a2BexecutingStatus = a2B_EXECUTE_SHOWLIST;

        // スレ一覧表示中、という設定にする
        if ((objectFactory.preference).openCurrentScreen > 0)
        {
            (objectFactory.preference).openCurrentScreen = 0;
        }

        // スレ取得予約シーケンスかどうか...
        if (isReserveSequence == true)
        {
            // "取得予約シーケンス"中だった場合、、、続きの処理を実行する
            afterReserveSequence();
            isReserveSequence = false;
            System.gc();
        }        
        return;
    }

    /**
     * レス参照中にa2Bを終了させてa2Bを起動したとき、前回終了時情報との整合性をチェックする
     * 
     * @param fileName     前回a2B終了時に開いていたスレのファイル名
     * @param messageIndex 前回a2B終了時に開いていたスレの管理インデックス番号
     * @return
     */
    private int checkFirstOpenScreen(String fileName, int messageIndex)
    {
        // スレのファイル名が管理インデックス番号のファイル名と一致するか？？
        String datFileName = getThreadFileName(messageIndex);
        if (fileName.indexOf(datFileName) < 0)
        {
            // 一致しない場合には、一覧表示とする
            return (-1);
        }
        
        // 前回表示していたレス番号を応答する
        return ((objectFactory.preference).currentScreenNumber);
    }
    
    /**
     *   http GET通信の実行処理、通信成功/失敗は別メソッドで判断する
     * 
     *   @param fileName   取得したデータを記録するファイル名（ベースディレクトリからの相対パス）
     *   @param url        取得するURL
     *   @param reference  送信する参照URL(リファラURL、nullの場合には指定省略)
     *   @param appendMsg  取得中に画面表示するメッセージ
     *   @param offset     送信するオフセット(取得する先頭バイトの指定、負の場合には指定省略)
     *   @param range      送信するレンジ (取得データサイズの指定、負の場合には指定省略)
     *   @param scene      http通信を使用する状況指定 (コールバック時の識別子として利用する)
     *   @param mode       ファイルに記録するとき追記するか、上書きするか (trueの時には追記モード)
     *   @return           ファイルオープン成功(true)、ファイルオープン失敗(false)
     * 
     */
    public boolean GetHttpCommunication(String fileName, String url, String reference, String appendMsg, int offset, int range, int scene, boolean mode)
    {
        // 通信中の場合、すぐに終了する
        if ((objectFactory.httpCommunicator).isCommunicating() == true)
        {
            return (false);
        }

        int appendMode = (objectFactory.httpCommunicator).APPENDMODE_NEWFILE;
        if (mode == true)
        {
            appendMode = (objectFactory.httpCommunicator).APPENDMODE_APPENDFILE;
        }
        
        String getFileName = null;
        if (fileName != null)
        {
            getFileName = (objectFactory.preference).getBaseDirectory() + fileName;
        }
        else
        {
            // 文字列モード
            appendMode = (objectFactory.httpCommunicator).APPENDMODE_STRING;
            offset = range;    
        }

        // 通信実行指示
        boolean ret = (objectFactory.httpCommunicator).openFileConnection(getFileName, appendMode);
        if (ret != true)
        {
            return (false);
        }

        // ビジー表示を行う
        updateBusyMessage("", "", false);
        showBusyMessage("http通信準備中", url, a2BselectionForm.BUSY_FORM);
        
        // HTTP通信実処理...
        if (getDivideGetHttp() == false)
        {
            // スレ通常取得モード
            (objectFactory.httpCommunicator).setParameter(url, reference, appendMsg, offset, scene, appendMode, true);
            (objectFactory.httpCommunicator).getURLUsingHttp();
        }
        else
        {
            // スレ分割取得モード
            int startRange = offset;
            (objectFactory.httpCommunicator).setParameter(url, reference, appendMsg, startRange, scene, appendMode, true);
            ret = (objectFactory.httpCommunicator).getURLUsingHttp();
            while (ret == true)
            {
                startRange = (objectFactory.httpCommunicator).getLastEndRange() + 1;
                (objectFactory.httpCommunicator).setParameter(url, reference, appendMsg, startRange, scene, (objectFactory.httpCommunicator).APPENDMODE_APPENDFILE, false);
                ret = (objectFactory.httpCommunicator).getURLUsingHttp();
            }
        }

        // BUSYウィンドウの表示をやめる
        hideBusyMessage();
        updateBusyMessage("", "", false);
        (objectFactory.httpCommunicator).closeFileConnection();
        return (true);
    }
    
    /**
     *    画面オープン処理(書き込み画面)
     *   
     */
    public void OpenWriteForm(boolean useTitle, String title, int messageIndex, int number)
    {
        (objectFactory.writeForm).prepareWriteMessageForm(useTitle, title, messageIndex, number);
        openWriteForm();
        return;
    }
    
    /**
     *    画面オープン処理(書き込み画面を文字列入力画面で利用する場合...)
     *   
     */
    public void OpenWriteFormUsingWordInputMode(int mode, String title, String subject, String defaultData, boolean ignoreCase)
    {
        String data = "";
        if (defaultData != null)
        {
            data = defaultData;
        }
        (objectFactory.writeForm).prepareWriteFormUsingDataInputMode(mode, title, subject, data, -1, ignoreCase);
        openWriteForm();
        return;
    }

    /**
     *   画面オープン処理(書き込み画面をデータ編集モードで利用する場合...)
     *   
     */
    public void OpenWriteFormUsingTextEditMode(String title, String message)
    {
        (objectFactory.writeForm).prepareWriteFormUsingTextEditMode(title, message);
        openWriteForm();
        return;
    }

    /**
     *   画面オープン処理(書き込み画面をデータ編集モードで利用する場合...)
     *   
     */
    public void OpenWriteFormUsingThreadOperationMode(String title, String message, String addInfo, int level)
    {
        (objectFactory.writeForm).prepareWriteFormUsingTextEditMode(title, message);
        (objectFactory.writeForm).prepareThreadOperationMenu(addInfo, level);
        openWriteForm();
        return;
    }

    /**
     *   画面オープン処理(書き込み画面をプレビュー表示モードで利用する場合...)
     *   
     */
    public void OpenWriteFormUsingPreviewMode(String title, String message)
    {
        (objectFactory.writeForm).prepareWriteFormUsingTextEditMode(title, message);
        (objectFactory.writeForm).preparePreviewMenu();
        openWriteForm();
        return;
    }
    
    /**
     * スレレベルを設定する
     * 
     * @param level
     */
    public void setFavorThreadLevel(int level)
    {
        (objectFactory.listForm).updateFavoriteThreadLevel(level);
        return;
    }

    /**
     *    画面オープン処理(書き込み画面をa2B設定(Proxy設定)で利用する場合...)
     *   
     */
    public void OpenWriteFormUsingProxySetting()
    {
        (objectFactory.writeForm).prepareWriteFormUsingProxySettingMode(SCENE_PROXY_SETTING, "a2B設定", getProxyUrl(), getUsedProxyScene(), getLaunchMode());
        openWriteForm();
        return;
    }
    
    /**
     * 書き込み画面のオープン(画面切り替え)処理
     *
     */
    private void openWriteForm()
    {
        previousForm = currentForm;
        if (isShowDialog == true)
        {
//            previousForm = currentForm;
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.writeForm);
        }
        else
        {
//            previousForm = (Display.getDisplay(this)).getCurrent();
            Display.getDisplay(this).setCurrent(objectFactory.writeForm);
        }
        currentForm = objectFactory.writeForm;
        keepWatchDog(-1);
        return;
    }

    /**
     *   画面オープン処理(参照画面)
     * 
     *       @param  fileName ： ログファイル名（オープンするファイル名）
     *       @param  msgIndex ： データ管理番号
     *       @param  number   ： オープンするレス番号
     */
    private void openViewForm(String fileName, int msgIndex, int number)
    {
        if (number <= 0)
        {
            number = 1;
        }
        (objectFactory.preference).openedFileName      = fileName;
        (objectFactory.preference).currentMessageIndex = msgIndex;
        (objectFactory.preference).currentScreenNumber = number;
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.viewForm);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.viewForm);
        }
        (objectFactory.viewForm).setupScreenSize();
        (objectFactory.viewForm).showMessage(fileName, msgIndex, number);
        (objectFactory.viewForm).setKeyRepeatDelay(getKeyRepeatTime());
        currentForm = objectFactory.viewForm;
        keepWatchDog(0);
        return;
    }
    
    /**
     *   画面オープン処理(一覧画面のオープン)
     * 
     */
    private void openListForm(boolean isUpdate)
    {
        showDialog(SHOW_DEBUG, 0, "Debug", "(open subject list form)");
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.listForm);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.listForm);
        }
        currentForm = objectFactory.listForm;
        if (isUpdate == true)
        {
            (objectFactory.listForm).prepareSubjectList((objectFactory.preference).currentBbsIndex, false);
        }
        ///  (objectFactory.listForm).setTitle("a2B");  // 省略するかな...
        keepWatchDog(0);
        return;
    }

    /**
     *   画面オープン処理(ファイル選択画面)
     *   
     */
    private void openFileSelector()
    {
        showDialog(SHOW_DEBUG, 0, "Debug", "(open file directory)");

        (objectFactory.fileSelector).openDirectory(false);
        if (isShowDialog == true)
        {    
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.fileSelector);
        }
        else
        {
            Display.getDisplay(this).setCurrent(objectFactory.fileSelector);
        }
        currentForm = objectFactory.fileSelector;
        return;
    }

    /**
     *   板カテゴリ/板選択モードをキャンセルした場合の後処理...
     * 
     */
    private void rewindBoardSelection()
    {
        // 板カテゴリ/板選択モードだった場合にキャンセル指示を受けた場合...
        if ((objectFactory.preference).previousBbsCategoryIndex >= 0)
        {
            (objectFactory.preference).currentBbsCategoryIndex  = (objectFactory.preference).previousBbsCategoryIndex;
            (objectFactory.preference).previousBbsCategoryIndex = -1;
        }

        // 板一覧を更新する
        (objectFactory.bbsCommunicator).refreshBoardInformationCache(getCurrentCategoryIndex(), false, null);
        (objectFactory.listForm).prepareSubjectList((objectFactory.preference).currentBbsIndex, false);
        return;
    }

    /**
     *  選択されたアイテム番号を処理する (アイテム選択画面を抜けてきたときの処理)
     * 
     */
    private boolean decideSelection(int scene, int selection, int appendInfo)
    {
        boolean ret = true;
        if (scene == SHOW_CATEGORYLIST)
        {
            // カテゴリ選択モード...
            setCurrentBbsCategoryIndex(selection);
            (objectFactory.listForm).boardSelectionMode();
            return (false);
        }
        else if (scene == SHOW_BBSLIST)
        {
            // オフセットを求める
            int index  = getCurrentCategoryIndex();
            int offset = objectFactory.bbsCommunicator.getCategoryIndex(index);
            
            // 板選択モード...
            setCurrentBbsIndex(selection + offset);

            // 通常表示モードに切り替える
            (objectFactory.preference).favoriteShowMode = 0;            

            // 板を切り替えたときには、スレタイ検索文字列をクリアする
            (objectFactory.listForm).clearSearchTitleString();

            // 板一覧を構築する
            (objectFactory.listForm).prepareSubjectList((selection + offset), false);
            return (true);
        }
        else if (scene == SHOW_SUBJECTCOMMAND)
        {
            // 選択コマンド実行(一覧画面)...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == SHOW_SUBJECTCOMMANDFAVORITE)
        {
            // 選択コマンド実行(一覧画面)...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == SHOW_SUBJECTCOMMANDSECOND)
        {
            // 一覧画面の第二メニュー...
            ret = objectFactory.listForm.executeMenuSecond((byte) scene, selection);
        }
        else if (scene == SHOW_VIEWCOMMANDSECOND)
        {
            // 参照画面の第二メニュー...
            objectFactory.viewForm.executeMenuSecond((byte) scene, selection);            
        }
        else if (scene == CONFIRM_DELETELOGDAT)
        {
            // 選択コマンド実行(一覧画面)...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == CONFIRM_DELETELOGFILE)
        {
            objectFactory.fileSelector.executeCommandAction((byte) scene, selection);
        }
        else if (scene == CONFIRM_URL_FOR_BROWSER)
        {
            // "ブラウザで開く"指示...
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == CONFIRM_GET_THREADNUMBER)
        {
            // スレ番号指定取得指示...
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == CONFIRM_SET_THREADNUMBER)
        {
            // スレ番号指定取得指示...(詳細)
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == CONFIRM_SELECT_OSUSUME2CH)
        {
            // おすすめ２ちゃんねるの表示。。。
            objectFactory.viewForm.executeCommandAction((byte) scene, selection, appendInfo);
        }
        else if (scene == CONFIRM_SELECT_FIND2CH)
        {
            // 2ちゃんねる検索でスレを選択...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == CONFIRM_SELECT_RELATE_KEYWORD)
        {
            // 関連キーワード検索で、キーワードを選択...
            objectFactory.listForm.executeCommandAction((byte) scene, selection, appendInfo);            
        }
        else if (scene == SHOW_LOGINMENU)
        {
            // 一覧画面のログインメニュー...
            objectFactory.listForm.executeLoginMenu((byte) scene, selection);
        }
        return (ret);
    }

    /**
     *   画面を一時的に切り替える
     * 
     */
    public void callScreen(Displayable a)
    {
        Display.getDisplay(this).setCurrent(a);    
        return;
    }
    
    /**
     *   画面を前の画面に戻す
     * 
     */
    public void returnScreen()
    {
        Display.getDisplay(this).setCurrent(currentForm);    
        return;
    }
    
    /**
     *  フォント設定情報をリセット
     *
     */
    public void updateFontData(boolean isReset, int fontSize)
    {
        (objectFactory.preference).updateFontData(isReset, fontSize);
        (objectFactory.listForm).changeFont(true);
        (objectFactory.viewForm).changeFontSize(true);
        return;
    }
    
    /**
     *  スレ一覧をリロードする
     * 
     */
    public void reloadCurrentMessageList()
    {
        if (isGetNewArrival() == true)
        {
            // 新着確認中は禁止する
            return;
        }

        // スレ一覧をリロードする
        (objectFactory.listForm).prepareSubjectList(((objectFactory.preference).currentBbsIndex), true);
        return;
    }

    /**
     *  スレをリロードする
     * 
     */
    public void reloadCurrentMessage()
    {
        if (isGetNewArrival() == true)
        {
            // 新着確認中は禁止する
            return;
        }

        // スレをリロードする
        (objectFactory.listForm).reloadCurrentMessage();
        return;
    }
    
    /**
     * 現在開いている板URLを応答する
     * 
     * @return 現在開いているURL
     */
    public String getCurrentMessageBoardUrl()
    {
        return ((objectFactory.listForm).getCurrentMessageBoardUrl());
    }

    /**
     *  参照中のログファイルをブラウザで開く
     * 
     * 
     */
    public void launchBrowserCurrentMessage()
    {    
        (objectFactory.listForm).launchWebBrowserCurrentSelectedIndex();
        return;
    }
    
    /**
     * メモファイル名を応答する
     * 
     * @return
     */
    public String getOutputMemoFileName()
    {
        return ((objectFactory.preference).memoFileName);
    }
    
    /**
     *  レスをファイルに出力する
     * 
     */
    public void outputMemoTextFile(String title, String message)
    {
        boolean result = (objectFactory.subjectParser).outputDataTextFile(getOutputMemoFileName(), title, message, true);
        (objectFactory.viewForm).completedOutputMessage(result);
    }

    /**
     * 板URLへ板一覧を更新する。。。
     * 
     * @param boardNick
     * @return
     */
    public boolean moveToBoard(String boardNick)
    {
        boolean ret = (objectFactory.bbsCommunicator).moveToBoard(boardNick);
        System.gc();
        if (ret == true)
        {
            // ディレクトリが掘られていない場合には作成する
            (objectFactory.bbsCommunicator).checkBoardNickName(boardNick);
            
            // 通常表示モードに切り替え、表示する板一覧を変更する...
            (objectFactory.preference).favoriteShowMode = 0;            

            // 板を切り替えたときには、スレタイ検索文字列をクリアする
            (objectFactory.listForm).clearSearchTitleString();

            // 板一覧を構築する
            (objectFactory.listForm).prepareSubjectList((objectFactory.preference).currentBbsIndex, false);
        }
        return (ret);
    }

    /**
     *  データをファイルに出力する
     * 
     */
    public boolean outputTextFile(String fileName, String title, String message, boolean appendMode)
    {
        return ((objectFactory.subjectParser).outputDataTextFile(fileName, title, message, appendMode));
    }
    
    /**
     *  参照中のスレタイトル名称を取得する
     * 
     */
    public String getSubjectName(int index)
    {
        return ((objectFactory.listForm).getSubjectName());
    }
    
    /**
     *   画面オープン処理(データ選択画面)
     * 
     *       @param  busyTitle ： タイトル
     *       @param  scene     ： 表示シーン
     */
    public void openSelectForm(String busyTitle, Vector itemList, int topItem, int scene)
    {
        (objectFactory.selectForm).showSelection(busyTitle, itemList, topItem, scene);
        if (isShowDialog == true)
        {
            Display.getDisplay(this).setCurrent(objectFactory.dialogForm, objectFactory.selectForm);
        }
        else
        {
            if (Display.getDisplay(this).getCurrent() != objectFactory.selectForm)
            {
                Display.getDisplay(this).setCurrent(objectFactory.selectForm);
            }
        }
        return;
    }

    /**
     *   画面オープン処理(データ選択画面から抜ける場合...)
     * 
     */
    public void endSelectForm(int scene, int selection, int appendInfo)
    {
        boolean redraw = true;
        Thread thread = null;
        if (selection < 0)
        {
            // 板カテゴリまたは、板選択をキャンセルしたときの処理...
            if ((scene == SHOW_CATEGORYLIST)||(scene == SHOW_BBSLIST))
            {
                // 板選択の実施
                thread = new Thread()
                {
                    public void run()
                    {
                        // 板の選択キャンセル...
                        rewindBoardSelection();            
                    }
                };
                thread.start();
            }
            else
            {
                // その他のキャンセル処理...
                if ((scene == CONFIRM_SELECT_FIND2CH)||(scene == CONFIRM_SELECT_RELATE_KEYWORD))
                {
                    // 2ch検索および関連キーワード検索はのときは、キャンセルしても"選択"を呼び出す
                    // (構築したリストをクリアする必要があるため)
                    redraw = decideSelection(scene, selection, appendInfo);                    
                }
            }
        }
        else  // if (selection >= 0)
        {
            // アイテムを選択したとき...
            redraw = decideSelection(scene, selection, appendInfo);
        }

        if (redraw == true)
        {
            // 表示画面を切り替える (元の画面に戻す)
            if (isShowDialog == true)
            {
                Display.getDisplay(this).setCurrent(objectFactory.dialogForm, currentForm);
            }
            else
            {
                Display.getDisplay(this).setCurrent(currentForm);
            }
        }
        System.gc();
        return;
    }
    
    /** 
     *  HTTP通信クラスで初回データを受け取ったときの処理...
     * 
     * 
     */
    public int getHttpDataReceivedFirst(int scene, int responseCode, boolean isDivideContinue, byte[] buffer, int offset, int length)
    {
        if (isDivideContinue == false)
        {
            // スレ分割取得時の２回目以降のリクエストだった場合...
            return (offset);
        }
        
        // スレ差分取得実行時...
        if ((scene == SCENE_PARTGET_MESSAGE)||(scene == SCENE_GETALL_MESSAGE))
        {
            if ((length == 1)&&(buffer[offset] == (byte) 10))
            {
                // スレ更新なし...
                return (-1);
            }
            if (buffer[offset] != (byte) 10)
            {
                // データ取得エラー...
                return (-10);
            }
            // 差分取得の実行！！
            return (offset + 1);
        }
        if ((scene == SCENE_WHOLEGET_MESSAGE)||(scene == SCENE_GETRESERVE_MESSAGE))
        {
            // スレ全体取得を実施する
            return (offset);
        }
        if ((scene == SCENE_PREVIEW_MESSAGE)||(scene == SCENE_PREVIEW_MESSAGE_EUC)||
            (scene == SCENE_PREVIEW_MESSAGE_SJIS)||(scene == SCENE_GET_OSUSUME_2CH)||
            (scene == SCENE_GET_RELATE_KEYWORD))
        {
            // スクラッチバッファにデータコピーする
            copyToScratchBuffer(buffer, offset, length);
            return (offset);
        }
        return (offset);
    }

    /**
     *   データをスクラッチバッファにコピーする
     * 
     * 
     */
    public void copyToScratchBuffer(byte[] buffer, int offset, int length)
    {
        // スクラッチバッファにデータコピーする
        scratchBuffer = new byte[length];
        System.arraycopy(buffer, offset, scratchBuffer, 0, length);
        return;
    }
    
    /**
     *   HTTP通信が終了したときの処理...
     * 
     * 
     */
    public void CompletedToGetHttpData(int scene, int responseCode, int length)
    {
        // スレ差分取得
        if (scene == SCENE_PARTGET_MESSAGE)
        {
            // length == 0 なら、データの追記なし...
            boolean result = false;
            (objectFactory.listForm).completedGetLogDataFile(result, length);
            return;
        }

        // スレ全部取得
        if (scene == SCENE_WHOLEGET_MESSAGE)
        {
            boolean result = true;
            (objectFactory.listForm).completedGetLogDataFile(result, length);
            return;
        }

        // 板一覧全部取得
        if (scene == SCENE_BBSTABLE_MESSAGE)
        {
            (objectFactory.bbsCommunicator).prepareBbsDatabase();
            return;
        }
        
        // スレ一覧全部取得
        if (scene == SCENE_GETSUBJECTLIST)
        {
            // スレ一覧のデータ加工指示
            (objectFactory.subjectParser).formatSubjectList();
            return;
        }
        
        // 新着確認を実施する...
        if ((scene == SCENE_GETALL_MESSAGE)||(scene == SCENE_GETRESERVE_MESSAGE))
        {
            // 新着確認結果を反映させる
            (objectFactory.listForm).completedGetLogList(length);

            // 次の新着確認実行へ...
            (objectFactory.listForm).startGetLogList(a2BMessageListForm.GETLOGLIST_CONTINUE_RECURSIVE);
            return;
        }
        
        // プレビューを実施した場合...
        if (scene == SCENE_PREVIEW_MESSAGE)
        {
            previewMessage(true, a2BsubjectDataParser.PARSE_2chMSG);
        }
        else if (scene == SCENE_PREVIEW_MESSAGE_EUC)
        {
            previewMessage(true, a2BsubjectDataParser.PARSE_EUC);
        }
        else if (scene == SCENE_PREVIEW_MESSAGE_SJIS)
        {
            previewMessage(true, a2BsubjectDataParser.PARSE_SJIS);
        }
        else if (scene == SCENE_GET_OSUSUME_2CH)
        {
            // おすすめ２ちゃんねるの表示を行う
            (objectFactory.viewForm).parseOsusume2ch(scratchBuffer);
            scratchBuffer = null;
        }
        else if (scene == SCENE_GET_FIND_2CH)
        {
            // 2ちゃんねる検索の表示を行う。(現状はプレビューのみ...)
            loadScratchBufferFromFile("temporary.html");
            (objectFactory.listForm).parseFind2ch(scratchBuffer);
            scratchBuffer = null;
            System.gc();
        }
        else if (scene == SCENE_GET_RELATE_KEYWORD)
        {
            // 受信した関連キーワードの解析を行う
            (objectFactory.listForm).parseRelateKeyword(scratchBuffer);
            scratchBuffer = null;
            System.gc();
        }
        return;
    }
    
    /**
     * ヒープ情報を応答する...
     * @return ヒープ情報
     */
    public String getHeapInfo()
    {
/***/
        return ("");
/***
        String appendString = "a2B-Dir : " + (objectFactory.preference).getBaseDirectory() + "\n";
        Runtime run = Runtime.getRuntime();
        return ("\n\n(Heap : " + run.freeMemory() + " / " + run.totalMemory() + ")\n" + appendString);
***/
    }

    /**
     *  メッセージのプレビューを行う
     * 
     * 
     */
    public void previewMessage(boolean useForm, int viewMode)
    {
        (objectFactory.subjectParser).previewMessage(scratchBuffer, useForm, viewMode);
        scratchBuffer = null;
        System.gc();
        return;
    }

    /**
     *  スレタイトルを更新する
     * 
     * @param title
     */
    public void updateThreadTitle(byte[] title, int offset, int length)
    {
        if ((a2BexecutingStatus == a2B_EXECUTE_VIEW_MANAGED)&&(title != null))
        {
            // 管理スレを表示中の場合には、スレタイトルを更新する
            (objectFactory.listForm).updateThreadTitle(title, offset, length);
        }
        return;
    }

    /*
     *  スレタイトルを抽出して応答する
     * 
     */
    public String pickupThreadTitle()
    {
        String title = (objectFactory.subjectParser).pickupThreadTitle(scratchBuffer);
        scratchBuffer = null;
        return (title);        
    }

    /**
     *  スレURLを取得する
     * 
     */
    public String getBoardURL(int index)
    {
        if (getFavoriteShowMode() == false)
        {
            return ((objectFactory.bbsCommunicator).getBoardURL(getCurrentBbsIndex()));
        }
        else
        {
            return ((objectFactory.favoriteManager).getUrl(index));
        }
    }

    /**
     *   スレファイル名を取得する
     * 
     */
    public String getThreadFileName(int index)
    {
        if (getFavoriteShowMode() == false)
        {
            return ((objectFactory.subjectParser).getThreadFileName(index));
        }
        else
        {
            return ((objectFactory.favoriteManager).getThreadNumber(index) + ".dat");
        }
    }

    /**
     * 現在読んだレス位置を登録する
     * @param number     読んだレス位置
     * @param maxNumber  レスの最大数
     */
    public void setCurrentMessage(int number, int maxNumber)
    {
        // 現在読んだレス数を記録する
        (objectFactory.listForm).setCurrentMessage(number, maxNumber);
    }

    /**
     *   スレ番号指定取得の解析
     * 
     */
    public void parseMessageThreadNumber(String threadNumber, int addInfo)
    {
        if (parseTargetString != null)
        {
            return;
        }
        parseTargetString = threadNumber;
        if (addInfo == 0)
        {
            // スレ番号取得のスレ取得予約処理...
            Thread thread = new Thread()
            {
                public void run()
                {
                    // スレ番号指定取得の解析実処理
                    (objectFactory.viewForm).lockOperation = true;
                    
                    boolean ret = false;
                    ret = (objectFactory.subjectParser).parseMessageThreadNumber(parseTargetString);
                    parseTargetString = null;
                    if ((getAutoEntryFavorite() == true)&&(ret == true)&&(getFavoriteShowMode() == true))
                    {
                        // お気に自動登録モードで、現在お気に入りモードで、スレ番号指定予約した場合...
                        // （お気に入りのインデックスが１つずれるので更新する...この対策でもだめポ...）
                    	int index = (objectFactory.listForm).getCurrentMessageIndex() + 1;
                        (objectFactory.listForm).setCurrentMessageIndex(index);
                        (objectFactory.preference).currentMessageIndex = index;
                        (objectFactory.viewForm).updateCurrentMessageIndex(index);
                    }
                    (objectFactory.viewForm).completedParseMessageThreadNumber(ret);
                    (objectFactory.viewForm).lockOperation = false;
                }
            };
            try
            {
                thread.start();
                thread.join();
            }
            catch (Exception ex)
            {
            	// 何もしない。
            }
        }
        else if ((addInfo == 1)||(addInfo == 2))
        {            
            // スレ番号取得処理... (板移動 + スレ取得(予約)処理)
            Thread thread = new Thread()
            {
                public void run()
                {
                    // スレ番号指定取得の解析実処理
                    String targetString = parseTargetString;
                    int pos = targetString.indexOf("/");
                    if (pos > 0)
                    {
                        // BUSY表示をする
                        (objectFactory.listForm).setInformationMessage(0, "板一覧切り替え中...");

                        // スレ一覧情報をバックアップする...
                        (objectFactory.subjectParser).leaveSubjectList(getCurrentBbsIndex());
                        
                        // 板を切り替える...
                        String threadNick = targetString.substring(0, (pos + 1));
                        isReserveSequence = true;
                        moveToBoard(threadNick);
                    }
                    System.gc();
                }
            };
            thread.start();
        }
        return;
    }

    /**
     *  スレ取得予約シーケンスの続き...
     * 
     */
    private void afterReserveSequence()
    {
        // 取得(予約)の実処理実行...        
        (objectFactory.listForm).setInformationMessage(0, "取得(予約)実行");
        (objectFactory.subjectParser).parseMessageThreadNumber(parseTargetString);    
        parseTargetString = null;
        
        return;
    }
    
    /**
     * スレ取得予約中にスレをスレ一覧に発見した場合...
     * 
     * @param index
     */
    public void detectMessageThreadNumber(int index)
    {
        //
        if (isReserveSequence == true)
        {
            // スレを取得する...
            (objectFactory.listForm).getLogDataFile(index, (objectFactory.listForm).GETMODE_GETALL);
            (objectFactory.listForm).setInformationMessage(-1, null);
        }
        return;
    }

    /**
     *   新着確認終了時の処理
     * 
     */
    public void endGetNewArrivalMessage()
    {
        isGetNewArrival = false;

        // ブルブルさせる...
        vibrate();

        // 終了メッセージを表示する
        (objectFactory.viewForm).setBusyMessage("新着確認終了", false);

        return;
    }
    
    /**
     *  ブルブルさせる...
     * 
     *
     */
    public void vibrate()
    {
        int milli = (objectFactory.preference).vibrateDuration;
        Display.getDisplay(this).vibrate(milli);        
        return;
    }

    
    /**
     * レスまとめ読みモードを設定する
     * 
     * 
     * @param count
     */
    public void setResBatchMode(int count)
    {        
        int batchCount = ((objectFactory.preference).resBatchMode) / 256;
        (objectFactory.preference).resBatchMode = batchCount * 256 + count;
        return;
    }

    /**
     * レスまとめ読みモードの値を応答する
     * 
     * @return
     */
    public int getResBatchMode()
    {
        int batchMode = ((objectFactory.preference).resBatchMode) % 256;
        return (batchMode);
    }

    /**
     * レスまとめ読みモードのまとめ読み値を設定する
     * 
     * 
     * @param count
     */
    public void setResBatchCount(int count)
    {
         int batchMode = ((objectFactory.preference).resBatchMode) % 256;
        (objectFactory.preference).resBatchMode = batchMode + (count * 256);
        return;
    }

    /**
     * レスまとめ読みモードの値を応答する
     * 
     * @return
     */
    public int getResBatchCount()
    {
        int batchCount = ((objectFactory.preference).resBatchMode) / 256;
        return (batchCount);
    }
    
    /**
     * ブルブルさせる時間を設定する...
     * @param milliSeconds
     */
    public void setVibrateDuration(int milliSeconds)
    {
        (objectFactory.preference).vibrateDuration = milliSeconds;
        if (milliSeconds != 0)
        {
            Display.getDisplay(this).vibrate(milliSeconds);            
        }
        return;
    }

    /**
     * 新着確認実行中かどうかを応答する
     * @return true(新着確認中) / false(新着確認していない)
     */
    public boolean isGetNewArrival()
    {
        return (isGetNewArrival);
    }

    /**
     * 新着確認の実行を開始する
     *
     */
    public void startGetLogList()
    {
        if (isGetNewArrival() == true)
        {
            // 新着確認中は禁止する
            return;
        }
        // 新着確認
        Thread thread = new Thread()
        {
            public void run()
            {
                // 新着確認を実行する
                isGetNewArrival = true;
                (objectFactory.listForm).getNewArrivalMessage();
            }
        };
        thread.start();
        return;
    }

    /**
     * ビジー画面を出さないようにする
     *
     */
    public void lockCommunicationScreen()
    {
        isGetNewArrival = true;
    }

    /**
     * ビジー画面を出すようにする
     *
     */
    public void unlockCommunicationScreen()
    {
        isGetNewArrival = false;
    }
    
    /**
     *   新着確認の準備が完了したことを報告する
     *
     * 
     */
    public void readyGetLogList()
    {
        isGetNewArrival = true;
        (objectFactory.listForm).startGetLogList(a2BMessageListForm.GETLOGLIST_START_RECURSIVE);
        return;
    }

    /**
     *   お気に入り設定する
     * 
     */
    public void setFavoriteThread(int index)
    {
        if (getFavoriteShowMode() == false)
        {
            (objectFactory.listForm).setResetFavoriteThread(index, true);
        }
        return;
    }
    
    /**
     *  スレタイ検索文字列を設定する
     * 
     */
    public void setInputData(int scene, String dataString, byte searchStatus, boolean ignoreCase, boolean use2chSearch, boolean useGoogleSearch, int searchHour)
    {
    	if (useGoogleSearch == true)
    	{
    		// Google検索を使用して検索する場合...
    		String convertStr = "http://www.google.co.jp/m/search?q=";
//            String convertStr = "http://www.google.co.jp/m/search?ie=shift_jis&q=" + a2BsjConverter.urlEncode(dataString.getBytes());
    		try
    		{
                convertStr =  convertStr + a2BsjConverter.urlEncode(dataString.getBytes("UTF-8"));
    		}
    		catch (Exception e)
    		{
                convertStr =  convertStr + a2BsjConverter.urlEncode(dataString.getBytes());    			
    		}
            openWebBrowser(convertStr);
    		dataString = "";
    		return;
    	}
        if (scene == SCENE_TITLE_SEARCH)
        {
            if (use2chSearch == true)
            {
                // ２ちゃんねる検索を使用して検索する場合...
                (objectFactory.listForm).find2chThreadNames((objectFactory.writeForm).utf8ToEUCandURLEncode(dataString));
                dataString = "";
            }
            int option = a2BMessageListForm.OPTION_NOTIGNORE_CASE;
            if (ignoreCase == true)
            {
                option = a2BMessageListForm.OPTION_IGNORE_CASE;
            }
            (objectFactory.listForm).setSearchTitleTemporary(dataString);
            (objectFactory.listForm).setSearchTitleInformation(dataString, searchStatus, option, searchHour);
        }
        else if (scene == SCENE_NGWORD_INPUT)
        {
            (objectFactory.viewForm).setNGword(dataString);
        }
        else if (scene == SCENE_MEMOFILE_INPUT)
        {
            (objectFactory.preference).memoFileName = dataString;
        }
        else if (scene == SCENE_THREADNUM_INPUT)
        {
            parseMessageThreadNumber(dataString, 0);            
        }
        else if (scene == SCENE_RENAMEFILE_INPUT)
        {
            (objectFactory.fileSelector).doRenameFileName(dataString);
        }
        else if (scene == SCENE_GETHTTP_INPUT)
        {
            (objectFactory.fileSelector).doReceiveHttp(dataString);            
        }
        else if (scene == SCENE_COPYFILE_INPUT)
        {
            (objectFactory.fileSelector).doCopyFileName(dataString);
        }
        else if (scene == SCENE_MESSAGE_SEARCH)
        {
            (objectFactory.viewForm).setSearchString(dataString, searchStatus, ignoreCase);            
        }
        return;
    }

    /**
     *  取得URLから、HTTP要求ヘッダを生成（変換）する (WX310用)
     * 
     *
     */
    private String modifyUrlforWX310(String urlToGet, String cookie, String referer)
    {
        int start = urlToGet.indexOf("://");
        start = start + 3;
        int end   = urlToGet.indexOf("/", start);
        String host = urlToGet.substring(start, end);
        urlToGet = urlToGet + " HTTP/1.0\r\n";
        urlToGet = urlToGet + "Host: " + host + "\r\n";
        urlToGet = urlToGet + "User-Agent: " + getUserAgent(true) + "\r\n";
        urlToGet = urlToGet + "Accept-Encoding: identity\r\n";
        urlToGet = urlToGet + "Content-Language: ja, en\r\n";
        if (cookie != null)
        {
            urlToGet = urlToGet + "Cookie:" + cookie + "\r\n";            
        }
        if (referer != null)
        {
            urlToGet = urlToGet + "Referer:" + referer + "\r\n";            
        }
        urlToGet = urlToGet + "Connection: Close\r\n";
        urlToGet = urlToGet + "Pragma: no-cache\r\n";
        urlToGet = urlToGet + "\r\nWX310";        
        return (urlToGet);
    }

    /**
     *  記録しているCookieをクリアする
     *  (doHttpMain() で Cookieを利用する場合)
     *
     */
    public void clearHttpCacheCookie()
    {
        previousHttpCookie = null;
    }
    
    /**
     * HTTP通信を実行してファイルを取得する
     * 
     * @param url
     * @param fileName
     * @param useCachedFile
     */
    public int doHttpMain(String url, String fileName, String referer, boolean useCachedFile, int usingMode)
    {
        int            rc  = -1;
        HttpConnection   c = null;
        InputStream     is = null;
        OutputStream    os = null;
        FileConnection dfc = null;
        byte[] data = new byte[FILE_BUFFER_SIZE + MARGIN];
        try
        {
            (objectFactory.viewForm).setBusyMessage("OPN:" + url, false);

            // 書き出しファイルの準備...
            dfc = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
            if (dfc.exists() == true)
            {
                if ((useCachedFile == true)&&(dfc.fileSize() > 0))
                {
                    // ファイル取得は実施しない...取得済みファイルをファイルを利用する
                    return (HttpConnection.HTTP_OK);
                }
                else
                {
                    try
                    {
                        // ファイルが存在する場合には削除する
                        dfc.delete();
                    }
                    catch (Exception ex)
                    {
                        (objectFactory.viewForm).setBusyMessage("DEL:" + ex.getMessage(), false);                        
                    }
                }
            }
            // ファイル作成
            dfc.create();

            // プラットフォーム名称を調べ、WX310だった場合には、gzip転送対策を行う
            String urlToGet = url;
            String platForm = System.getProperty("microedition.platform");
            if (platForm.indexOf("WX310") >= 0)
            {
                // WX310用にURLを変更する...
                if (usingMode == a2B_HTTPMODE_USEPREVIOUSCOOKIE)
                {
                    urlToGet = modifyUrlforWX310(url, previousHttpCookie, referer);
                }
                else
                {
                    urlToGet = modifyUrlforWX310(url, null, referer);
                }

                // HTTP通信の準備
                c = (HttpConnection) Connector.open(urlToGet);
            }
            else
            {
                // HTTP通信の準備
                c = (HttpConnection) Connector.open(urlToGet);

                // ネフロを偽装...
                c.setRequestMethod(HttpConnection.GET);
                c.setRequestProperty("user-agent", getUserAgent(true));
                c.setRequestProperty("accept", "text/html, */*");
                c.setRequestProperty("content-language", " ja, en");
                c.setRequestProperty("connection", " close");
                c.setRequestProperty("range", " bytes=0-");
                
                if (referer != null)
                {
                    c.setRequestProperty("referer", referer);
                }
                if (usingMode == a2B_HTTPMODE_USEPREVIOUSCOOKIE)
                {
                    c.setRequestProperty("cookie", previousHttpCookie);
                }
            }

            // Getting the response code will open the connection,
            // send the request, and read the HTTP response headers.
            // The headers are stored until requested.
            rc = c.getResponseCode();

            (objectFactory.viewForm).setBusyMessage("RCV:" + url, false);

            // Cookie を抽出する...
            if (usingMode == a2B_HTTPMODE_PICKUPCOOKIE)
            {
                int   index = 0;
                String key = "";
                String value = "";
                previousHttpCookie = "";
                try
                {
                    while (index >= 0)
                    {
                        value = c.getHeaderField(index);
                        key   = c.getHeaderFieldKey(index);
                        if (key.compareTo("set-cookie") == 0)
                        {
                            //
                            int pos = value.indexOf(";");
                            if (previousHttpCookie.length() != 0)
                            {
                                previousHttpCookie = previousHttpCookie + "; ";
                            }
                            previousHttpCookie = previousHttpCookie + value.substring(0, pos);
                        }
                        else if (key.compareTo("Set-Cookie") == 0)
                        {
                            //
                            int pos = value.indexOf(";");
                            if (previousHttpCookie.length() != 0)
                            {
                                previousHttpCookie = previousHttpCookie + "; ";
                            }
                            previousHttpCookie = previousHttpCookie + value.substring(0, pos);
                        }
                        value = "";
                        key   = "";
                        index++;
                    }
                }
                catch (Exception e)
                {
                    // エラー情報を設定...
                    (objectFactory.viewForm).setBusyMessage("a:" + e.toString(), false);
                }
            }

            // stream open
            is = c.openInputStream();
            os = dfc.openOutputStream();                

            (objectFactory.viewForm).setBusyMessage("WRT:" + url, false);

            int writeBytes = 0;
            int actual = 10;
            while (actual > 0)
            {
                actual = is.read(data, 0, FILE_BUFFER_SIZE);
                writeBytes = writeBytes + actual;
                (objectFactory.viewForm).setBusyMessage("RCV:" + writeBytes + "(" + actual + ")", false);
                if (actual > 0)
                {
                    os.write(data, 0, actual);
                }
                else
                {
                    (objectFactory.viewForm).setBusyMessage("Fin:" + writeBytes + "(" + actual + ")", false);                    
                }
            }
            os.flush();
            os.close();
            is.close();
            c.close();
            dfc.close();
            data = null;
            (objectFactory.viewForm).setBusyMessage("END:" + url, false);
        }
        catch (Exception e)
        {
            (objectFactory.viewForm).setBusyMessage("b:" + e.toString(), false);
            rc = rc * (-1000);
            try
            {
                if (os != null)
                {
                    os.flush();
                    os.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                if (is != null)
                {
                    is.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                if (c != null)
                {
                    c.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            try
            {
                if (dfc != null)
                {
                    dfc.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
        }
        data = null;
        System.gc();
        return (rc);
    }    
    
    /*
     *   現在の板カテゴリインデックスを設定する
     *   
     *       @param  index ： 板カテゴリインデックス番号
     */
    public void setCurrentBbsCategoryIndex    (int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).previousBbsCategoryIndex = (objectFactory.preference).currentBbsCategoryIndex;
        (objectFactory.preference).currentBbsCategoryIndex  = index;
        return;
    }
    
    /*
     *   現在の板インデックスを設定する
     *   
     *       @param  index ： 板インデックス番号
     */
    public void setCurrentBbsIndex(int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).currentBbsIndex          = index;
        (objectFactory.preference).previousBbsCategoryIndex = -1;
        return;
    }

    /*
     *   NGワード検出モードを切り替える 
     * 
     */
    public void toggleNgWordDetectMode()
    {
        if ((objectFactory.preference).ngWordDetectMode == 0)
        {
            // NGワード検出モードに切り替える
            (objectFactory.preference).ngWordDetectMode = 1;
        }
        else
        {
            // 通常表示モードに切り替える
            (objectFactory.preference).ngWordDetectMode = 0;            
        }
        return;
    }

    /*
     *   お気に入り一覧表示モードを切り替える 
     * 
     */
    public void toggleFavoriteShowMode()
    {
        if ((objectFactory.preference).favoriteShowMode == 0)
        {
            // お気に入り一覧表示モードに切り替える
            (objectFactory.preference).favoriteShowMode = 1;
        }
        else
        {
            // 通常表示モードに切り替える
            (objectFactory.preference).favoriteShowMode = 0;            
        }
        return;
    }

    /**
     * お気に入り表示レベルを設定する
     * 
     * @param level
     */
    public void setFavoriteShowLevel(int level)
    {
        (objectFactory.preference).favoriteShowLevel = level;
        return;
    }
    
    /*
     *   NGワード検出モードの設定を応答する
     * 
     */
    public boolean getNgWordDetectMode()
    {
        if ((objectFactory.preference).ngWordDetectMode == 0)
        {
            return (false);
        }
        return (true);
    }

    /**
     * スレ分割取得モードの設定を取得する
     * 
     * @return
     */    
    public boolean getDivideGetHttp()
    {
        return ((objectFactory.preference).divideToGetHttp);

    }
    /**
     * スレ分割取得モードを設定する
     * 
     * @return
     */    
    public void setDivideGetHttp(boolean isSet)
    {
        (objectFactory.preference).divideToGetHttp = isSet;
        return;
    }

    /**
     * キー入力モードを変更する
     * 
     * @param mode
     */
    public void toggleNumberInputMode(boolean mode)
    {
        boolean setMode = false;
        if (mode == true)
        {
            setMode = false;
        }
        else
        {
            setMode = true;
        }
        
        (objectFactory.listForm).setNumberInputMode(setMode);
        (objectFactory.viewForm).setNumberInputMode(setMode);
        
        return;
    }
    
    /**
     * スクリーンモードの設定を取得する
     * 
     * @return
     */
    public boolean getViewScreenMode()
    {
        return ((objectFactory.preference).viewScreenMode);
    }

    /**
     * スクリーンモードの設定を変更する
     * 
     * @return
     */
    public void setViewScreenMode(boolean mode)
    {
        (objectFactory.preference).viewScreenMode = mode;
    }
    
    /**
     * Proxy URLを(preferenceに)設定する
     * @param scene
     * @param proxyUrl
     * @param selection
     */
    public void setProxyUrl(int scene, String proxyUrl, int selection)
    {
        (objectFactory.preference).proxyUrl       = proxyUrl;
        (objectFactory.preference).usedProxyScene = selection;
        return;
    }

    /*
     *   proxy URLを取得する
     * 
     */
    public String getProxyUrl()
    {
        return ((objectFactory.preference).proxyUrl);
    }

    /*
     *   proxy利用シーンを取得する
     * 
     */
    public int getUsedProxyScene()
    {
        return ((objectFactory.preference).usedProxyScene);
    }
    
    /*
     *   お気に入り一覧表示モードの設定を応答する
     * 
     */
    public boolean getFavoriteShowMode()
    {
        if ((objectFactory.preference).favoriteShowMode == 0)
        {
            return (false);
        }
        return (true);
    }

    /**
     * 現在のお気に入り(表示)レベルを取得する
     * 
     * @return
     */
    public int getFavoriteShowLevel()
    {
        if ((objectFactory.preference).favoriteShowMode == 0)
        {
            return (-1);
        }
        return ((objectFactory.preference).favoriteShowLevel);
    }

    /**
     *  pic.to変換を行う
     * @return
     */
    public boolean getUsePicToConvert()
    {
        return ((objectFactory.preference).usePicToConvert);
    }
    
    /**
     *  pic.to変換を行う
     * @return
     */
    public void setUsePicToConvert(boolean isSet)
    {
        (objectFactory.preference).usePicToConvert = isSet;
    }

    /**
     *  強制的に自前の漢字変換を行う
     * @return
     */
    public boolean getForceKanjiConvertMode()
    {
        return ((objectFactory.preference).forceSelfKanjiConvert);
    }
    
    /**
     *  強制的に自前の漢字変換を行う
     * @return
     */
    public void setForceKanjiConvertMode(boolean isSet)
    {
        (objectFactory.preference).forceSelfKanjiConvert = isSet;
    }

    /**
     *  画像取得時、Googleのプロキシを使用するかどうかを応答する
     * @return 使用する <code>true</code> / 使用しない <code>false</code>
     */
    public boolean getWX220JUserAgent()
    {
        return ((objectFactory.preference).useWX220JUserAgent);
    }
    
    /**
     *  画像取得時、Googleのプロキシを利用するかどうかを設定する
     * @param isUse googleのプロキシを利用するかどうか
     */
    public void setWX220JUserAgent(boolean isUse)
    {
        (objectFactory.preference).useWX220JUserAgent = isUse;
    }

    /**
     *  画像取得時、キャッシュファイルがあればそれを利用する設定を応答する
     * @return 使用する <code>true</code> / 使用しない <code>false</code>
     */
    public boolean getUseCachePictureFile()
    {
        return ((objectFactory.preference).useCachePicFile);
    }
    
    /**
     *  画像取得時、キャッシュファイルがあればそれを利用する設定を設定する
     * @param isUse キャッシュファイルを使用するかどうか
     */
    public void setUseCachePictureFile(boolean isUse)
    {
        (objectFactory.preference).useCachePicFile = isUse;
    }

    /**
     *  カーソルの動きをページ送りにするかどうか
     * @return 使用する <code>true</code> / 使用しない <code>false</code>
     */
    public boolean getCursorSingleMovingMode()
    {
        return ((objectFactory.preference).cursorSingleMovingMode);
    }
    
    /**
     *  カーソルの動きをページ送りにするかどうか変更する
     * @param isUse <code>true</code> ページ送りにする / <code>false</code> ページ送りにしない
     */
    public void setCursorSingleMovingMode(boolean isUse)
    {
        (objectFactory.preference).cursorSingleMovingMode = isUse;
    }
    
    /**
     *  Googleのプロキシを使用するかどうかを応答する
     * @return 使用する <code>true</code> / 使用しない <code>false</code>
     */
    public boolean getUseGoogleProxy()
    {
        return ((objectFactory.preference).useGoogleProxy);
    }
    
    /**
     *  Googleのプロキシを利用するかどうかを設定する
     * @param isUse googleのプロキシを利用するかどうか
     */
    public void setUseGoogleProxy(boolean isUse)
    {
        (objectFactory.preference).useGoogleProxy = isUse;
    }
    
    /*
     *   現在のスレインデックスを設定する
     *
     *       @param  index ： スレインデックス番号
     */
    public void setSelectedSubjectIndex(int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).selectedSubjectIndex = index;
        return;
    }

    /*
     *  現在のメッセージ番号を設定する
     * 
     *       @param  index ： 現在表示中のメッセージ番号
     */
    public void setSelectedMessageNumber(int index)
    {
        if (index < 0)
        {
            index = 0;
        }
        (objectFactory.preference).selectedMessageNumber = index;
        return;
    }

    /**
     *   メッセージ番号を記憶する
     * 
     * @param messageNumber
     */
    public void setMessageNumber(int messageNumber)
    {
        (objectFactory.preference).currentScreenNumber = messageNumber;
        return;
    }    
    
    /*
     *  参照画面のフォントサイズを記憶する
     * 
     */
    public void setViewFontSize(int fontSize)
    {
        (objectFactory.preference).viewFormFontSize = fontSize;
        return;
    }

    /*
     *  参照画面のフォントサイズを取得する
     * 
     */
    public int getViewFontSize()
    {
        return ((objectFactory.preference).viewFormFontSize);
    }

    /*
     *  参照画面のフォント色を記憶する
     * 
     */
    public void setViewFontColor(int foreColor, int backColor)
    {
        (objectFactory.preference).viewFormBackColor = backColor;
        (objectFactory.preference).viewFormForeColor = foreColor;
        return;
    }

    /*
     *  参照画面の背景色を取得する
     * 
     */
    public int getViewBackColor()
    {
        return ((objectFactory.preference).viewFormBackColor);
    }

    /*
     *  参照画面のフォント色を取得する
     * 
     */
    public int getViewForeColor()
    {
        return ((objectFactory.preference).viewFormForeColor);
    }    
    
    /*
     *  一覧画面のフォントサイズを記憶する
     * 
     */
    public void setListFontSize(int fontSize)
    {
        (objectFactory.preference).listFormFontSize = fontSize;
        return;
    }

    /**
     *  次スレ取得時に自動的にお気に入りに登録するかどうかを確認する
     * 
     * 
     * @return
     */
    public boolean getAutoEntryFavorite()
    {
        return ((objectFactory.preference).autoEntryFavorite);
    }

    /**
     *  次スレ取得時に自動的にお気に入りに登録するかどうかを設定する
     * 
     * 
     * @return
     */
    public void setAutoEntryFavorite(boolean isSet)
    {
        (objectFactory.preference).autoEntryFavorite = isSet;
        return;
    }

    /**
     *  「ファイルダウンロード時のディレクトリをa2Bルートディレクトリにする」
     * 
     * 
     * @return
     */
    public boolean getSaveAtRootDirectory()
    {
        return ((objectFactory.preference).saveAtRootDirectory);
    }

    /**
     *  「ファイルダウンロード時のディレクトリをa2Bルートディレクトリにする」
     * 
     * 
     * @return
     */
    public void setSaveAtRootDirectory(boolean isSet)
    {
        (objectFactory.preference).saveAtRootDirectory = isSet;
        return;
    }
    
    /**
     *  a2Bを起動したら「お気に入り一覧」からはじめるかどうかを確認する
     * 
     * 
     * @return
     */
    public boolean getLaunchAsFavoriteList()
    {
        if ((objectFactory.preference).openCurrentScreen < 0)
        {
            return (true);
        }
        return (false);
    }
    
    /**
     *  a2B起動時には、必ず「お気に入り」一覧からはじめる設定を切り替える
     * 
     * 
     */
    public void setLaunchAsFavoriteList(boolean isSet)
    {
        int value = 0;
        if (isSet == true)
        {
            value = -1;
        }
        (objectFactory.preference).openCurrentScreen = value;
    }

    /**
     * スレタイトルを別領域に表示する
     * 
     * @return
     */
    public boolean getShowTitleAnotherArea()
    {
        return ((objectFactory.preference).isShowTitleBusyArea);
    }

    /**
     * スレタイトルを別領域に表示する
     * 
     * @return
     */
    public void setShowTitleAnotherArea(boolean setting)
    {
        (objectFactory.preference).isShowTitleBusyArea = setting;
        return;
    }

    /**
     * subject.txtをバックアップするかどうか...
     * 
     * @return
     */
    public boolean getBackupSubjectTxt()
    {
        return ((objectFactory.preference).backupSubjectTxt);
    }

    /**
     * subjectTxtをバックアップするかどうか...
     * 
     * @return
     */
    public void setBackupSubjectTxt(boolean setting)
    {
        (objectFactory.preference).backupSubjectTxt = setting;
        return;
    }

    /**
     * お気に入りバックアップ省略モードを取得する
     * 
     * @return
     */
    public boolean getIgnoreFavoriteBackup()
    {
        return ((objectFactory.preference).ignoreFavoriteBackup);
    }

    /**
     * お気に入りバックアップ省略モードを設定する
     * 
     * @return
     */
    public void setIgnoreFavoriteBackup(boolean setting)
    {
        (objectFactory.preference).ignoreFavoriteBackup = setting;
        return;
    }
    
    /**
     * Webブラウザ起動モードを取得する
     * 
     * @return
     */
    public boolean getLaunchMode()
    {
        return ((objectFactory.preference).afterLaunchWebAction);
    }

    /**
     * Webブラウザ起動モードを設定する
     * 
     * @return
     */
    public void setLaunchMode(boolean setting)
    {
        (objectFactory.preference).afterLaunchWebAction = setting;
        return;
    }
    
    /**
     * ファンクションキー(数字０キー)の機能を設定する
     * 
     * @param scene
     * @param featureNumber
     * @return
     */
    public void setFunctionKeyFeature(int scene, int featureNumber)
    {
        if (scene == SCENE_LISTFORM)        
        {
            (objectFactory.preference).listFormFunctionKey = featureNumber;
        }
        // if (scene == SCENE_VIEWFORM)
        (objectFactory.preference).viewFormFunctionKey = featureNumber;
        
        return;
    }
    
    /**
     * ファンクションキー(数字０キー)の機能を応答する
     * 
     * @param scene
     * @return
     */
    public int getFunctionKeyFeature(int scene)
    {
        if (scene == SCENE_LISTFORM)        
        {
            return ((objectFactory.preference).listFormFunctionKey);            
        }
        // if (scene == SCENE_VIEWFORM)
        return ((objectFactory.preference).viewFormFunctionKey);
    }    
    
    /*
     *  一覧画面のフォントサイズを取得する
     * 
     */
    public int getListFontSize()
    {
        return ((objectFactory.preference).listFormFontSize);
    }

    /**
     * 無操作終了時間を取得する
     * @return
     */
    public int getWatchDogTime()
    {
        return ((objectFactory.preference).watchDogMinutes);
    }

    /**
     *  スレ分割取得サイズを取得する
     * 
     * @return
     */
    public int getDivideGetSize()
    {
        return ((objectFactory.preference).divideCommunicationSize);
    }

    /**
     *  スレ分割取得サイズを設定する
     */
    public void setDivideGetSize(int size)
    {
        (objectFactory.preference).divideCommunicationSize = size;
    }

    /**
     * 無操作終了時間を設定する(0:無限大、単位:分)
     * @param time
     */
    public void setWatchDogTime(int time)
    {
        (objectFactory.preference).watchDogMinutes = time;
        return;
    }

    /**
     * EUC漢字コードをUTF8に変換する
     * @param eucString
     * @param startIndex
     * @param length
     * @return
     */
    public String eucToUtf8(byte[] eucString, int startIndex, int length)
    {
        return ((objectFactory.writeForm).eucToUtf8(eucString, startIndex, length));
    }

    /**
     * Shift JIS漢字コードをUTF8に変換する
     * @param sjString
     * @param startIndex
     * @param length
     * @return
     */
    public String sjToUtf8(byte[] sjString, int startIndex, int length)
    {
        return ((objectFactory.writeForm).sjToUtf8(sjString, startIndex, length));
    }
    
    /*
     *  書き込み画面の名前欄を記録する
     * 
     */
    public void setWriteName(String name)
    {
        (objectFactory.preference).writeName = name;
        return;
    }
    
    /*
     *  書き込み画面の名前欄を取得する
     * 
     */
    public String getWriteName()
    {
        return ((objectFactory.preference).writeName);
    }

    /**
     * データベースアクセスモードを記憶する
     * @param isDatabase
     */
    public void setOnlyDatabaseAccess(boolean isDatabase)
    {
        (objectFactory.preference).onlyDatabaseAccess = isDatabase;
        return;
    }

    /*
     *   現在のカテゴリインデックスを取得する
     *   
     */
    public int getCurrentCategoryIndex()
    {
        return ((objectFactory.preference).currentBbsCategoryIndex);
    }
    
    /**
     * AAモードのページ移動幅取得
     * 
     * @return
     */
    public int getAAwidthMargin()
    {
        return ((objectFactory.preference).aaWidth);
    }

    /**
     * AAモードのページ移動幅設定
     * 
     * @return
     */
    public void setAAwidthMargin(int width)
    {
        int margin = width % 512;
        (objectFactory.preference).aaWidth = margin;
        (objectFactory.viewForm).setAAmodeWidthMargin(margin);
        return;
    }

    /*
     *   現在の板インデックスを取得する
     *   
     */
    public int getCurrentBbsIndex()
    {
        return ((objectFactory.preference).currentBbsIndex);
    }

    /*
     *   現在のスレインデックスを設定する
     *   
     */
    public int getSelectedSubjectIndex()
    {
        return ((objectFactory.preference).selectedSubjectIndex);
    }

    /*
     *   現在のメッセージ番号を設定する
     *   
     */
    public int getSelectedMessageNumber()
    {
        return ((objectFactory.preference).selectedMessageNumber);
    }

    /*
     *   データ管理用ベースディレクトリを取得する
     * 
     * 
     */
    public String getBaseDirectory()
    {
        return ((objectFactory.preference).getBaseDirectory());
    }

    /*
     *   ログイン名の設定(Be@2ch)
     * 
     * 
     */
    public void setBeName(String name)
    {
        (objectFactory.preference).beLoginName = name;
        return;
    }
    
    /*
     *   ログイン用パスワードの設定 (Be@2ch)
     * 
     * 
     */
    public void setBePassword(String password)
    {
        (objectFactory.preference).beLoginPassword = password;
        return;
    }

    /*
     *   ログインクッキーの保存 (Be@2ch)
     * 
     * 
     */
    public void setBeCookie(String value)
    {
        Date  date      = new Date();
        (objectFactory.preference).beLoginDateTime = date.getTime();
        (objectFactory.preference).beCookie = value;
        
        date = null;
        return;
    }

    /**
     *  キーリピートを認識する時間間隔を設定
     * @param ms
     */
    public void setKeyRepeatTime(int ms)
    {
        (objectFactory.viewForm).setKeyRepeatDelay(ms);
        (objectFactory.preference).keyRepeatDelayTime = ms;
    }

    /**
     *  キーリピートを認識する時間間隔を取得
     * @return
     */
    public int getKeyRepeatTime()
    {
    	return ((objectFactory.preference).keyRepeatDelayTime);
    }
    
    /*
     *   ログインクッキーの取得 (Be@2ch)
     * 
     * 
     */
    public String getBeCookie()
    {
        Date  date      = new Date();
        long time      = date.getTime() - (24 * 60 * 60 * 1000);
        
        if (((objectFactory.preference).beLoginDateTime == 0)||
            (time >= (objectFactory.preference).beLoginDateTime))
        {
            // ログイン後24時間経過、または未ログイン時にはcookieとしてnullを応答する
            (objectFactory.preference).beLoginDateTime = 0;
            return (null);
        }
        return ((objectFactory.preference).beCookie);
    }

    /*
     *   ログイン名の取得(Be@2ch)
     * 
     * 
     */
    public String getBeName()
    {
        return ((objectFactory.preference).beLoginName);
    }
    
    /**
     * BE表示モードの設定を確認する
     * 
     * @return
     */
    public int getViewKanjiMode()
    {
    	return ((objectFactory.preference).viewKanjiMode);
    }
    
    /**
     * 漢字表示モードの設定を変更する
     * 
     * @return
     */
    public void toggleViewKanjiMode(int kanjiMode)
    {
        int mode = (objectFactory.preference).viewKanjiMode;
        if (kanjiMode == a2BsubjectDataParser.PARSE_TOGGLE)
        {
            mode = (mode % 4) + 1;
        }
        else
        {
        	mode = kanjiMode % 5;
        }
        (objectFactory.preference).viewKanjiMode = mode;
        return;
    }
    
    /*
     *   ログイン用パスワードの取得 (Be@2ch)
     * 
     * 
     */
    public String getBePassword()
    {
        return ((objectFactory.preference).beLoginPassword);
    }

    /**
     *  ファイル保存ディレクトリを応答する
     * @return ファイル保存ディレクトリ
     */
    public String getFileSaveDirectory()
    {
        if (((objectFactory.preference).fileSaveDirectory).indexOf("file:///") >= 0)
        {
            return ((objectFactory.preference).fileSaveDirectory);
        }
        return (getBaseDirectory() + (objectFactory.preference).fileSaveDirectory);
    }

    /**
     *  ファイル保存ディレクトリを設定する
     * @param fileDirectory ファイル保存ディレクトリ
     */
    public void setFileSaveDirectory(String fileDirectory)
    {
        (objectFactory.preference).fileSaveDirectory = fileDirectory;
        return;
    }
  
    /**
     *  書き込み用拡張キーワード設定
     */
    public void setWriteKeyString(String value)
    {
        (objectFactory.preference).writeKeyString = value;
        return;
    }

    /**
     *  書き込み用拡張キーワード設定
     */
    public String getWriteKeyString()
    {
    	return ((objectFactory.preference).writeKeyString);
    }

    /**
     *  書き込み用Cookie設定
     */
    public String getWriteCookie()
    {
    	return ((objectFactory.preference).writeCookie);
    }

    /**
     *  書き込み用Cookie設定
     */
    public void setWriteCookie(String value)
    {
        (objectFactory.preference).writeCookie = value;
        return;
    }

    /*
     *   ログイン名の設定
     * 
     * @param name ログイン名称
     * 
     */
    public void setOysterName(String name)
    {
        (objectFactory.preference).oysterLoginName = name;
        return;
    }

    /*
     *   ログインパスワードの設定
     * 
     * 
     */
    public void setOysterPassword(String pass)
    {
        (objectFactory.preference).oysterLoginPassword = pass;
        return;
    }
    
    /*
     *   ログイン名の取得
     * 
     * 
     */
    public String getOysterName()
    {
        return ((objectFactory.preference).oysterLoginName);
    }
    
    /*
     *   ログイン用パスワードの取得
     * 
     * 
     */
    public String getOysterPassword()
    {
        return ((objectFactory.preference).oysterLoginPassword);
    }
    
    /**
     *  ●ログインの開始
     *
     */
    public void startOysterLogin()
    {
        // ●ログイン実行開始を表示する
        (objectFactory.listForm).setInformationMessage(0, "2chログイン実行中...");

        Thread thread = new Thread()
        {
            public void run()
            {
                // ログインを実行する
                String name = (objectFactory.preference).oysterLoginName;
                String pass = (objectFactory.preference).oysterLoginPassword;
                boolean result = (objectFactory.loginUtility).doOysterLogin(name, pass);
                finishOysterLogin(result);
                System.gc();
            }
        };
        thread.start();
    
    }

    /**
     *  Be@2chログインの開始
     *
     */
    public void startBe2chLogin()
    {
        // Be@2chログイン実行開始を表示する
        (objectFactory.listForm).setInformationMessage(0, "BE@2chログイン中...");

        Thread thread = new Thread()
        {
            public void run()
            {
                // ログインを実行する
                String name = getBeName();
                String pass = getBePassword();
                boolean result = (objectFactory.loginUtility).doBe2chLogin(name, pass);
                finishBe2chLogin(result);
                System.gc();
            }
        };
        thread.start();
    
    }

    /**
     * ユーザーエージェントを設定する
     * 
     * @param userAgent
     */
    public void setUserAgent(String userAgent)
    {
        if (userAgent.length() == 0)
        {
            (objectFactory.preference).changedUserAgent = "Monazilla/1.00  NNsi/1.10(a2B)";    
        }
        else
        {
            (objectFactory.preference).changedUserAgent = userAgent;            
        }
        return;
    }
    
    /**
     * ユーザーエージェントを取得する
     * 
     * @return
     */
    public String getUserAgent(boolean isNormalBrowser)
    {
        if (isNormalBrowser == true)
        {
/*
            if ((objectFactory.preference).useWX220JUserAgent == false)
            {
                return ("Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3");
            }
            return ("Mozilla/3.0(WILLCOM;JRC/WX220J/2;1.0/0100/C50) CNF/2.0");
*/
            if ((objectFactory.preference).usedProxyScene == PROXY_WORKAROUND_WX310)
            {
                if ((objectFactory.preference).useWX220JUserAgent == false)
                {
                    return ("Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3");
                }
                return ("Mozilla/3.0(WILLCOM;JRC/WX220J/2;1.0/0100/C50) CNF/2.0");
            }
            if ((((objectFactory.preference).changedUserAgent).indexOf("Monazilla")) >= 0)
            {
                // UAがチューニングされていない場合、ネフロを偽装する...
                if ((objectFactory.preference).useWX220JUserAgent == false)
                {
                    return ("Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3");
                }
                return ("Mozilla/3.0(WILLCOM;JRC/WX220J/2;1.0/0100/C50) CNF/2.0");
            }
        }
        String userAgent = (objectFactory.preference).changedUserAgent;
        if ((objectFactory.preference).oysterLoginDateTime != 0)
        {
            // 有効、User Agent
            userAgent = (objectFactory.preference).oysterLoginUserAgent;
        }
        return (userAgent);
    }

    /**
     *   Be@2ch のクッキー情報を記録する
     * 
     * @param setCookie
     */
    public void setBe2chCookie(String setCookie)
    {
        be2chCookie = setCookie;
    }

    /**
     *  Be@2chのクッキー情報を応答する
     * @param scene
     * @return
     */
    public String getBe2chCookie(int scene)
    {
        return (be2chCookie);
    }

    /**
     * セッションIDを取得する
     * 
     * @param scene
     * @return
     */
    public String getOysterSessionId(int scene)
    {
        // 現在時刻を取得する
        String sessionId = null;
//      Date   curTime   = new Date();
//      long  diffTime  = curTime.getTime() - (objectFactory.preference).oysterLoginDateTime;
//
//        // ログイン情報は有効かどうか確認する。
//        if ((diffTime > 0)&&(diffTime < (24 * 60 * 60 * 1000)))
        if ((objectFactory.preference).oysterLoginDateTime != 0)
        {
            // 有効、確認する
            sessionId = (objectFactory.preference).oysterLoginSessionId;
        }
        else
        {
            // expired...
            sessionId = null;
            (objectFactory.preference).oysterLoginDateTime = 0;
        }
        return (sessionId);
    }

    /**
     * ●ログイン情報を記憶する
     * 
     * @param allString
     * @param sessionId
     */
    public void setOysterSessionId(String userAgent, String sessionId)
    {
        // 現在時刻を取得する
        Date curTime = new Date();

        // ●ログイン時の情報を記憶する...
        (objectFactory.preference).oysterLoginUserAgent    = userAgent;
        (objectFactory.preference).oysterLoginSessionId    = sessionId;
        (objectFactory.preference).oysterLoginDateTime     = curTime.getTime(); // ミリセカンド(24 * 60 * 60 * 1000)
    }
    
    /**
     *  ●ログイン完了
     *  
     * @return
     */
    public boolean finishOysterLogin(boolean result)
    {
        // ログイン結果を表示...
        String message;
        String msg  = (objectFactory.loginUtility).getResultString();
        if (result == true)
        {
            message = "2chログイン成功 " + msg;
        }
        else
        {
            message = "2chログイン失敗:" + msg;
        }

        // ●ログイン実行結果
        (objectFactory.listForm).setInformationMessage(0, message);
        return (false);
    }

    /**
     *  Beログイン完了
     *  
     * @return
     */
    public boolean finishBe2chLogin(boolean result)
    {
        // ログイン結果を表示...
        String message;
        String msg  = (objectFactory.loginUtility).getResultString();
        if (result == true)
        {
            message = "BEログイン成功 " + msg;
        }
        else
        {
            message = "BEログイン失敗:" + msg;
        }

        // Be@2chログイン実行結果
        (objectFactory.listForm).setInformationMessage(0, message);
        return (false);
    }

    /**
     * 少し止まる
     * 
     * @param time (ms)
     */
    public void sleepTime(long time)
    {
        try
        {
            Thread.sleep(time);
        }
        catch (Exception e)
        {
                
        }
        return;
    }
    
    /**
     *  イメージファイル(png形式)を読み出し、メモリに展開する
     * 
     * @param fileName ファイル名
     * @return         イメージデータ
     */
    public Image loadJpegThumbnailImageFromFile(String fileName)
    {
        Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]        buffer     = null;
        try
        {
            ///// 一度仮読み、、、サムネール画像があるか探す
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);
//            int size   = (int) connection.fileSize();

            // データを最初から size分読み出す...
            in      = connection.openInputStream();
            int startThumbnailOffset = 0;
            int endThumbnailOffset = 0;
            int dataReadSize = 0;
            int currentReadSize = 0;
            byte[] tempBuffer = new byte[TEMP_READ_SIZE + 8];
            tempBuffer[0] = 0x00;
            while (currentReadSize >= 0)
            {
                int index = 0;
                currentReadSize = in.read(tempBuffer, 1, TEMP_READ_SIZE);
                if (dataReadSize == 0)
                {
                    // 先頭マーカを飛ばす
                    index = index + 3;
                }
                while (index < currentReadSize)
                {
                    if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd8))
                    {
                        startThumbnailOffset = index + dataReadSize;
                    }
                    if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd9))
                    {
                        endThumbnailOffset = index + dataReadSize;
                        if (startThumbnailOffset != 0)
                        {
                            currentReadSize = -1;
                            break;
                        }
                    }
                    index++;
                }
                if ((currentReadSize >= 0)&&(index == currentReadSize))
                {
                    tempBuffer[0] = tempBuffer[currentReadSize - 1];
                }
                dataReadSize = dataReadSize + currentReadSize;
            }
            // ファイルのクローズ
            in.close();
            connection.close();
            in = null;
            connection = null;
            tempBuffer = null;

            ///// 一度仮読み、、、サムネール画像があるか探す
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);

            // データを最初から size分読み出す...
            in      = connection.openInputStream();
            int bufferSize = endThumbnailOffset - startThumbnailOffset + 10;
            if (bufferSize < 0)
            {
                bufferSize = (int) connection.fileSize();
            }
            if (startThumbnailOffset < 0)
            {
                startThumbnailOffset = 0;
            }
            buffer = new byte[bufferSize + 4];

            // ファイルの先頭まで読み出し...
            if (startThumbnailOffset != 0)
            {
                //in.skip(startThumbnailOffset);

                // in.skip(startThumbnailOffset)の処理があまりにも遅かったので、、、カラ読み処理。                
                int startOffset = startThumbnailOffset - 1;
                while (startOffset > 0)
                {
                    int readSize = 0;
                    if (startOffset > bufferSize)
                    {
                        readSize = in.read(buffer, 0, bufferSize);
                    }
                    else
                    {
                        readSize = in.read(buffer, 0, startOffset);
                    }
                    startOffset = startOffset - readSize;
                }
            }
            
            // データの読み出し処理...
            currentReadSize = in.read(buffer, 0, bufferSize);
            
            // ファイルのクローズ
            in.close();
            connection.close();

            // 壁紙データとしてイメージに変換...
            image = Image.createImage(buffer, 0, bufferSize);
            
            // バッファをクリアする
            buffer = null;

        }
        catch (OutOfMemoryError e)
        {
            (objectFactory.viewForm).setBusyMessage("MemOver", false);

            // メモリオーバフロー例外発生！！！
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            // 読み込み失敗、データを破棄する
            buffer  = null;
            image   = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            (objectFactory.viewForm).setBusyMessage("Err:" + e.getMessage(), false);
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            // 読み込み失敗、データを破棄する
            buffer  = null;
            image   = null;
        }
        System.gc();
        return (image);
    }

    /**
     * データをスクラッチバッファに読み込み...
     * 
     * @param fileName ファイル名
     */
    private void loadScratchBufferFromFile(String fileName)
    {
        FileConnection connection = null;
        InputStream    in         = null;
        try
        {
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(getBaseDirectory() + fileName, Connector.READ);
            int size   = (int) connection.fileSize();

            // データを最初から size分読み出す...
            in      = connection.openInputStream();
            scratchBuffer = new byte[size];
            in.read(scratchBuffer, 0, size);

            // ファイルのクローズ
            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            
            // 読み込み失敗、データを破棄する
            scratchBuffer  = null;
        }
        System.gc();
    }

    /**
     *  イメージファイル(png形式)を読み出し、メモリに展開する
     * 
     * @param fileName ファイル名
     * @return         イメージデータ
     */
    public Image loadImageFromFile(String fileName)
    {
        Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]        buffer     = null;
        try
        {
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(getBaseDirectory() + fileName, Connector.READ);
            int size   = (int) connection.fileSize();

            // データを最初から size分読み出す...
            in      = connection.openInputStream();
            buffer  = new byte[size + 4];
            int dataReadSize = in.read(buffer, 0, size);

            // ファイルのクローズ
            in.close();
            connection.close();

            // 壁紙データとしてイメージに変換...
            image = Image.createImage(buffer, 0, dataReadSize);
            
            // バッファをクリアする
            buffer = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            
            // 読み込み失敗、データを破棄する
            buffer  = null;
            image   = null;
        }
        System.gc();
        return (image);
    }
    /**
     * ファイル名のコピー処理を実行する
     * 
     * @param destFileName
     * @param srcFileName
     */
    public void doCopyFile(String destFileName, String srcFileName)
    {
        (objectFactory.fileSelector).doCopyFileMain(destFileName, srcFileName);
    }

    /**
     *  ファイルの削除処理を行う
     * @param fileName
     */
    public void doDeleteFile(String fileName)
    {
        (objectFactory.fileSelector).deleteFile(fileName, false);
    }
    
    /**
     *  ファイルの内容を表示する...
     * @param fileName ファイル名
     * @param previewMode プレビューモード
     */
    public void doPreviewFile(String fileName, boolean useForm, int previewMode)
    {
        // 親クラスにデータをコピーできた場合...
        if (prepareScratchBuffer(fileName, TEMP_PREVIEW_SIZE) == true)
        {
            // データプレビューを実行
            previewMessage(useForm, previewMode);
        }
        return;
    }

    /** 
     *   ファイルを読んで親のスクラッチバッファの準備を行う
     * 
     */
    public boolean prepareScratchBuffer(String dataFile, int limitSize)
    {
        FileConnection connection    = null;
        InputStream    in            = null;
        byte[]         buffer        = null;
        int            dataReadSize  = 0;

        try
        {
            connection   = (FileConnection) Connector.open(dataFile, Connector.READ);
            int size     = (int) connection.fileSize();

            // ファイルサイズが大きい場合、サイズ制限を用意する
            if (size > limitSize)
            {
                size = limitSize;
            }

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
            
            // ファイルデータを親のスクラッチバッファへコピーする
            copyToScratchBuffer(buffer, 0, dataReadSize);
        }
        catch (Exception e)
        {
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                //            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            buffer = null;
            return (false);
        }
        buffer = null;
        return (true);
    }

    /**
     * ディレクトリの作成
     * @param directoryName ディレクトリ名 (full path)
     */
    public boolean makeDirectory(String directoryName)
    {
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(directoryName, Connector.READ_WRITE);
            if (dataFileConnection.exists() != true)
            {
                // ファイルが存在しなかったとき、ディレクトリを作成する
                dataFileConnection.mkdir();
                ret = true;
            }
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
            return (false);
        }
        return (ret);
    }

    /**
     * スプラッシュスクリーン(起動画面)の表示クラス
     * 
     * @author MRSa
     *
     */
    public class a2BSplashScreen extends Canvas
    {
        Image image = null;  // イメージファイルの終了
        
        /**
         * コンストラクタ(スプラッシュスクリーンに表示するイメージを決める)
         *
         */
        public a2BSplashScreen()
        {
            Random rand = new Random();

            int item = rand.nextInt(17);
//            int item = (rand.nextInt() % 17);

            // イメージファイルを決める
            String resource = "/res/a2B_splash.png";
            switch (item)
            {
              case 1:
                resource = "/res/a2B_splash1.png";
                break;

              case 2:
                resource = "/res/a2B_splash2.png";
                break;

              case 3:
                resource = "/res/a2B_splash3.png";
                break;

              case 4:
                resource = "/res/a2B_splash4.png";
                break;

              case 5:
                resource = "/res/a2B_splash5.png";
                break;

              case 6:
                resource = "/res/a2B_splash6.png";
                break;

              case 7:
                resource = "/res/a2B_splash7.png";
                break;

              case 8:
                resource = "/res/a2B_splash8.png";
                break;

              case 9:
                resource = "/res/a2B_splash9.png";
                break;

              case 10:
                resource = "/res/a2B_splashA.png";
                break;

              case 11:
                resource = "/res/a2B_splashB.png";
                break;

              case 12:
                resource = "/res/a2B_splashC.png";
                break;

              case 13:
                resource = "/res/a2B_splashD.png";
                break;

              case 14:
                resource = "/res/a2B_splashE.png";
                break;

              case 15:
                resource = "/res/a2B_splashF.png";
                break;

              case 16:
                resource = "/res/a2B_splash0.png";
                break;

              default:
                resource = "/res/a2B_splash.png";
                break;
            }
            rand = null;
            
            // イメージファイルを読み出す
            try
            {
                image = Image.createImage(resource);
            }
            catch (Exception e)
            {
                image = null;
            }
        }

        /**
         *   起動画面を表示する
         *   
         */
        public void paint(Graphics g)
        {
            // 画面の塗りつぶし
            g.setColor(255, 255, 255);
            g.fillRect(0, 0, getWidth(), getHeight());
            
            if (image == null)
            {
                String message = "a 2ch Browser (a2B)";

                // タイトルの表示 (文字)
                g.setColor(32, 32, 32);
                Font font = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE);
                g.setFont(font);
                g.drawString(message, 0, (getHeight() / 2), (Graphics.LEFT | Graphics.TOP));
                return;
            }
            else
            {
                // イメージの表示座標を決める
                int x = ((getWidth()  - image.getWidth())  / 2);
                if ((x + image.getWidth()) > getWidth())
                {
                    x = 0;
                }

                int y = ((getHeight() - image.getHeight()) / 2);
                if ((y + image.getHeight()) > getHeight())
                {
                    y = 0;
                }
                
                // スプラッシュスクリーンのイメージを描画
                g.setColor(0);
                g.drawImage(image, x, y, (Graphics.LEFT | Graphics.TOP));
            }
            return;
        }

        /**
         *   起動画面の後始末
         *
         */
        public void finish()
        {
            // ガベコレ実施...
            image = null;
            System.gc();
            
            return;
        }
    }
}
//
// --------------------------------------------------------------------------
//    a2BMain  --- a 2ch Browser メイン部分
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
