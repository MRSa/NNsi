package jp.sourceforge.nnsi.a2b.frameworks;

import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;



/**
 * シーンセレクタ : 画面の切り替えを行う
 * 
 * @author MRSa
 */
public class MidpSceneSelector
{
    private MIDlet           parent   = null;
    private MidpTimer        myTimer  = null;
    private IMidpDataStorage storage  = null;
    private IMidpSceneDB     sceneDb  = null;
    
    private String     currentTitle = null;
    private int       previousScene = IMidpSceneDB.NOSCREEN; // 前のシーン
    private int        currentScene = IMidpSceneDB.NOSCREEN; // 現在のシーン

    /**
     * コンストラクタ
     * @param object メインMIDlet
     * @param dao データアクセスユーティリティ
     * @param scene シーンセレクタ
     */
    public MidpSceneSelector(MIDlet object, IMidpDataStorage dao, IMidpSceneDB scene)
    {
        parent   = object;
        storage  = dao;
        sceneDb  = scene;
    }

    /**
     *  アプリケーション名を応答する
     * 
     * @return アプリケーション名称
     */
    public String getApplicationName()
    {
        return (storage.getApplicationName());
    }
    
    /**
     *  起動時のイメージ名を応答する
     * 
     * @return 起動時のイメージ（ファイル）名称
     */
    public String getSplashImageName()
    {
        return (storage.getSplashImageName());
    }

    /**
     * 起動準備を行う
     * 
     */
    public void prepare()
    {
        // 内部情報の準備...
        storage.prepare(parent);

        // データストレージの復旧...
        storage.restore();

        return;
    }

    /**
     *   初期化処理を行う
     * 
     */
    public boolean initialize()
    {

        // シーン切り替え情報の初期化
        sceneDb.prepare();

        // 画面表示の準備...
        storage.prepareScreen(this);
 
        // タイマークラスの生成
        myTimer = new MidpTimer(this);

        // ガベコレ実施
        System.gc();

        return (true);
    }
    
    /**
     *  初期画面にシーンを切り替える
     *
     */
    public void start()
    {
        changeScene(sceneDb.getDefaultScene(), null);
        long timeout = storage.getTimeoutCount();
        if (timeout > 0)
        {
            myTimer.startWatchDog(timeout);
        }
        return;
    }

    /**
     *  アプリケーションを停止する処理
     *
     */
    public void stop()
    {
        storage.backup();
        storage.cleanup();
        parent.notifyDestroyed();
        return;
    }

    /**
     *  タイムアウトの受信(アプリケーションを終了)
     *
     *  @param  reason  true:タイムアウト発生
     */
    public void detectTimeout(boolean reason)
    {
        if (reason == true)
        {
            stop();
            parent.notifyDestroyed();
        }
        return;
    }

    /**
     *  タイマーの更新を行う
     *
     */
    public void extendTimer(int number)
    {
        myTimer.extendTimer(number);
        return;
    }

    /**
     *  画面を１つ前に戻す...
     *
     */
    public void previousScene()
    {
        changeScene(previousScene, null);
    }

    /**
     *  画面表示切替スレッド実行
     *
     *  @param  scene  切り替える画面シーン
     *  @param newTitle 画面タイトル
     */
    public void changeScene(int scene, String newTitle)
    {
        // 画面切り替え可能かどうかチェックする
        if (sceneDb.isAvailableChangeScene(scene, currentScene) != true)
        {
            // 画面切り替え不可だった場合、終了する
            return;
        }

        // 切り替えた前のシーンを記憶する
        if (currentScene != previousScene)
        {
            // 前回の画面シーンと今回の画面シーンが異なったときだけ記憶する
            previousScene = currentScene;
        }
        currentScene  = scene;
        currentTitle  = newTitle;

        // 画面シーン切り替えスレッド実行。。。
        Thread thread = new Thread()
        {
            public void run()
            {
                // 画面を切り替える...
                if (sceneDb.changeScene(currentScene, previousScene, currentTitle) == true)
                {
                    Display.getDisplay(parent).setCurrent(sceneDb.getScreen(currentScene));
                }

                // 画面切り替え後の処理
                sceneDb.sceneChanged(currentScene, previousScene);
            }
        };
        try
        {
            thread.start();  // 画面の切り替え処理...
            //thread.join();   // 切り替わるまで待つ...
        }
        catch (Exception ex)
        {
            // 何もしない...
        }

        // あとしまつ
        thread = null;
        System.gc();
        return;
    }

    /**
     *    URLをWebブラウザで開く、などの端末個別要求処理
     * 
     *    @param uri プラットフォーム要求ストリング
     *    @param isExit trueの場合要求後、アプリケーションを終了させる
     * 
     */
    public void platformRequest(String uri, boolean isExit)
    {
        try
        {
            // 要求を行う
            parent.platformRequest(uri);

            // アプリケーションの実行を継続するかどうか
            if (isExit == true)
            {
                stop();
            }
        }
        catch (Exception e)
        {
            /////// 未サポートだった、何もしない  /////
        }
        return;
    }

    /**
     *  ブルブルさせる...
     *
     * @param milli (ms)
     */
    public void vibrate(int milli)
    {
        Display.getDisplay(parent).vibrate(milli);
        return;
    }

    /**
     * 少し止まる
     * 
     * @param time (ms)
     */
    public void sleep(long time)
    {
        try
        {
            Thread.sleep(time);
        }
        catch (Exception e)
        {
            // 何もしない...
        }
        return;
    }
}
