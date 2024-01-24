package jp.sourceforge.nnsi.a2b.frameworks;
import javax.microedition.lcdui.Displayable;

/**
 * 画面（と操作の）クラスのデータベース<br>
 * (インタフェースクラス) 
 *
 * @author MRSa
 */
public interface IMidpSceneDB
{
    /** スクリーンなし */
    static public  final int NOSCREEN         = -1; // スクリーンなし

    /** 情報表示画面 */
    static public  final int INFORM_SCREEN    =  0; // 情報表示画面

    /** 選択肢表示画面 */
    static public  final int SELECTION_SCREEN =  1; // 選択肢表示画面

    /**
     *  クラスの準備
     */
    public abstract void prepare();

    /**
     *  スクリーンオブジェクトの設定
     * 
     *  @param baseCanvas ベースキャンバス
     */
    public abstract void setScreenObjects(MidpCanvas baseCanvas);

    /**
     *  デフォルトの画面を応答する
     * 
     *  @return デフォルトの画面ID
     */
    public abstract int getDefaultScene();

    /**
     *  スクリーン表示画面クラスを設定する
     *  @param screen スクリーン表示画面クラス
     */
    public abstract void setCanvas(IMidpScreenCanvas screen);

    /**
     *  データ入力用クラスを設定する
     * 
     *  @param form データ入力用クラス
     */
    public abstract void setForm(IMidpForm form);
    
    /**
     *  画面切り替えの可否判断
     *  
     *  @param next 切り替える画面ID
     *  @param current 現在の画面ID
     *  @return 切り替える(<code>true</code>) / 切り替えない(<code>false</code>)
     */
    public abstract boolean isAvailableChangeScene(int next, int current);

    /**
     *  画面の切り替え実行前の処理
     * 
     *  @param next 切り替える画面ID
     *  @param current 現在の画面ID
     *  @param title タイトル
     *  @return 実行成功(<code>true</code>) / 実行失敗(<code>false</code>)
     */
    public abstract boolean changeScene(int next, int current, String title);

    /**
     * 画面切り替え完了後の処理
     * @param current 切替後の画面ID
     * @param previous 切替前の画面ID
     */
    public abstract void    sceneChanged(int current, int previous);

    /**
     *  画面に関わる文字列の取得
     * 
     *  @param scene 画面ID
     *  @return 画面にかかわる文字列
     */
    public abstract String  getInfoMessage(int scene);

    /**
     *  表示クラスの取得
     * 
     *  @param scene 画面ID
     *  @return 画面表示クラス
     */
    public abstract Displayable getScreen(int scene);
}
