import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.*;

/**
 * シーン切替クラス
 * @author MRSa
 *
 */
public class fwSampleSceneDB  implements IMidpSceneDB
{
    protected MidpCanvas baseScreen = null;
    protected IMidpScreenCanvas screenCanvas = null;
    protected IMidpScreenCanvas selectionCanvas = null;
    
    /**
     *  コンストラクタでは何も操作しません
     */
    public fwSampleSceneDB()
    {
        
    }

    /**
     *  クラスの準備処理
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
        return (fwSampleFactory.DEFAULT_SCREEN);
    }

    /**
     *  スクリーン表示画面クラスを設定する
     *  @param screen スクリーン表示画面クラス
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        if (screen.getCanvasId() == fwSampleFactory.DEFAULT_SCREEN)
        {
            screenCanvas = screen;
        }
        else
        {
            selectionCanvas = screen;
        }
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
     *  画面の切り替え実行処理
     *  @param next 切り替えたい画面ID
     *  @param current 現在の画面ID
     *  @param title 新しい画面のタイトル
     *  @return true:切替OK, false:切替NG
     */
    public boolean changeScene(int next, int current, String title)
    {
        if (next == fwSampleFactory.SELECTION_SCREEN)
        {
            // 選択画面へ切り替える
            baseScreen.changeActiveCanvas(selectionCanvas);            
        }
        else
        {
            // メイン画面へ切り替える
            baseScreen.changeActiveCanvas(screenCanvas);
        }
        return (true);
    }

    /**
     *  画面切り替え完了時の処理
     *  @param current 切替後の画面ID
     *  @param previous 切替前の画面ID
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
        return ("");
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
