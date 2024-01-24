package jp.sourceforge.nnsi.a2b.frameworks.samples;

import javax.microedition.lcdui.Displayable;

import jp.sourceforge.nnsi.a2b.frameworks.IMidpForm;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;


/**
 *  シーンデータベース(のサンプル実装)
 * 
 * @author MRSa
 *
 */
public class MidpDefaultSceneDB implements IMidpSceneDB
{
    protected MidpCanvas baseScreen = null;
    protected IMidpScreenCanvas screenCanvas = null;
    
    /**
     *  コンストラクタでは何も操作しません
     *
     */
    public MidpDefaultSceneDB()
    {
        
    }
    
    /**
     *  クラスの準備
     *  
     */
    public void prepare()
    {
        return;
    }

    /**
     *  スクリーンオブジェクトの設定
     *  @param baseCanvas ベースキャンバスクラス
     */
    public void setScreenObjects(MidpCanvas baseCanvas)
    {
        baseScreen = baseCanvas;
        return;
    }

    /**
     *  デフォルトの画面を応答する
     *  @return デフォルトの画面ID
     */
    public int getDefaultScene()
    {
        return (IMidpSceneDB.NOSCREEN);
    }

    /**
     *  スクリーン表示画面クラスを設定する
     *  @param screen スクリーン表示画面クラス
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        screenCanvas = screen;
        return;
    }

    /**
     *  データ入力用クラスを設定する
     *  @param form データ入力フォーム
     */
    public void setForm(IMidpForm form)
    {
        return;
    }

    /**
     *  画面切り替えの可否判断
     *  @param next 切り替えたい画面ID
     *  @param current 現在の画面ID
     *  @return true:切替OK, false:切替NG
     */
    public boolean isAvailableChangeScene(int next, int current)
    {
        return (true);
    }

    /**
     *  画面の切り替え実行処理(ここで画面を切り替えます)<br>
     *  (baseMidpCanvas.changeActiveCanvas() を呼んだり、とかを行います。)<br>
     *  画面を切り替える場合には、シーンセレクタは getScreen()を呼び出し、
     *  Displayableクラス(画面表示クラス)を取得します。
     *  
     *  @param next 切り替えたい画面ID
     *  @param current 現在の画面ID
     *  @param title 新しい画面のタイトル
     *  @return <code>true</code>:Display.setCurrent()を実行します。<br>
     *   <code>false</code>:Display.setCurrent()を実行しません。
     *   (MidpScreenCanvas()クラスを切替るとき、とかはこちらを応答してください。)
     */
    public boolean changeScene(int next, int current, String title)
    {
        // 選択画面へ切り替える
        baseScreen.changeActiveCanvas(screenCanvas);            
        return (true);
    }

    /**
     *  画面切り替え完了時の処理
     *  @param current 切替後の画面ID
     *  @param previous 切替前の画面ID
     *  
     */
    public void sceneChanged(int current, int previous)
    {
        return;
    }

    /**
     *  画面に関わる文字列取得
     *  @param scene 画面ID
     *  @return 画面にかかわる文字列
     */
    public String  getInfoMessage(int scene)
    {
        return ("DefaultSceneDB");
    }

    /**
     *  表示クラスの取得
     *  @param scene 画面ID
     *  @return 画面表示クラス
     */
    public Displayable getScreen(int scene)
    {
        return (baseScreen);
    }
}
