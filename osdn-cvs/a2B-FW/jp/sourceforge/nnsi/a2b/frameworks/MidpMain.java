/**
 *  a2B Frameworkの中核パッケージです。
 */
package jp.sourceforge.nnsi.a2b.frameworks;
import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;


/**
 * MidpMain : MIDP起動部 
 * (このクラスを継承してアプリケーションの起動部分を作成する)
 * 
 * @author MRSa
 */
public class MidpMain extends MIDlet
{
    protected MidpSceneSelector sceneSelector = null;  // シーンセレクタ
    private   IMidpFactory      midpFactory   = null;   // ファクトリクラス

    /**
     *  コンストラクタ
     *  
     *  @param factory ファクトリクラス
     */
    public MidpMain(IMidpFactory factory)
    {
        // MIDletクラスの初期化
        super();

        // オブジェクトの生成・設定
        midpFactory = factory;

        // シーンセレクタは外に出したくないため、２段階で生成
        midpFactory.prepareCoreObjects();
        IMidpDataStorage storage = midpFactory.getDataStorage();
        IMidpSceneDB sceneDb = midpFactory.getSceneDB();
        sceneSelector = new MidpSceneSelector(this, storage, sceneDb);
    }

    /**
     *  アプリケーションの起動処理
     * 
     */
    protected void startApp() throws MIDletStateChangeException
    {
        // スプラッシュスクリーンを生成・表示する...
        splashScreen startScreen = new splashScreen(sceneSelector.getSplashImageName());
        Display.getDisplay(this).setCurrent(startScreen);

        // 起動準備を実施する
        sceneSelector.prepare();
        midpFactory.prepareObjects(this, sceneSelector);

        // シーンセレクタに初期化を指示する
        boolean ret = sceneSelector.initialize();
        if (ret == false)
        {
            // 起動失敗...終了する
            destroyApp(true);
            return;
        }

        // シーンセレクタに処理の開始を指示する
        sceneSelector.start();
        
        // あとしまつ(スプラッシュスクリーンを消す)...
        startScreen.finish();
        startScreen = null;

        return;
    }

    /**
     *  アプリケーションの一時停止処理
     * 
     */
    protected void pauseApp()
    {
        // 現在のところ、何もしない...
        return;
    }

    /**
     *  アプリケーションの終了処理
     *  @param arg0 終了するか？
     */
    protected void destroyApp(boolean arg0) throws MIDletStateChangeException
    {
        if (arg0 == true)
        {
            // データのバックアップとオブジェクトの削除を実行する
            sceneSelector.stop();

            // 終了指示
            notifyDestroyed();
        }
        return;
    }

    /**
     *  スプラッシュスクリーン(起動画面)の表示クラス
     * @author MRSa
     */
    private class splashScreen extends Canvas
    {
        Image image = null;  // イメージファイルの終了

        /**
         * コンストラクタでは、イメージファイルを読み出す処理を行います
         * @param imageFileName イメージファイル名
         */
        public splashScreen(String imageFileName)
        {
            if (imageFileName == null)
            {
                // イメージファイル名が無いときは、画像読み出しを行わない
                return;
            }

            // イメージファイルを読み出す
            try
            {
                image = Image.createImage(imageFileName);
            }
            catch (Exception e)
            {
                image = null;
            }
        }

        /**
         *  起動画面を表示する実処理
         *  @param g グラフィック
         */
        public void paint(Graphics g)
        {
            // タイトルの設定
            String screenTitle = sceneSelector.getApplicationName();
            
            // 画面の塗りつぶし
            g.setColor(255, 255, 255);
            g.fillRect(0, 0, getWidth(), getHeight());
            
            if (image == null)
            {
                // イメージがない場合には、アプリケーション名（文字）の表示
                g.setColor(32, 32, 32);
                Font font = Font.getFont(Font.FACE_MONOSPACE, Font.STYLE_BOLD, Font.SIZE_LARGE);
                g.setFont(font);
                g.drawString(screenTitle, 0, (getHeight() / 2), (Graphics.LEFT | Graphics.TOP));
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
         *  起動画面の後始末
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
