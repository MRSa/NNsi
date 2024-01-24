import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.*;

/**
 *  画面切替の制御指示を行う。
 * 
 * @author MRSa
 */
public class DotESceneDB implements IMidpSceneDB
{
    static public final int DEFAULT_SCREEN    = 100;  // メイン画面
    static public final int MAIN_MENU         = 200;  // メインメニュー
    static public final int FILENAME_FORM     = 300;  // ファイル名設定画面
    static public final int CONFIRMATION      = 400;  // 確認画面
    static public final int CONFIRMATION_LOAD = 402;  // LOAD
    static public final int CONFIRMATION_SAVE = 404;  // SAVE
    static public final int CONFIRMATION_END  = 406;  // END
    static public final int COLORPICK_SCREEN  = 500;  // 色選択画面
    static public final int COLORPICK_PIXEL   = 502;  // いわゆるスポイト(色を拾う)
    static public final int REGION_COPY       = 504;  // 領域選択
    static public final int REGION_PASTE      = 506;  // 領域貼り付け
    static public final int REGION_CLEAR      = 508;  // 領域クリア
    static public final int DIR_SCREEN        = 600;  // ディレクトリ選択画面

    private boolean          isShowForm        = false; // データ入力中(データ入力画面表示中)
    private IMidpForm         preferenceForm    = null;  // 設定項目画面
    private MidpCanvas        baseMidpCanvas    = null;  // ベースキャンバスクラス
    
    private IMidpScreenCanvas defaultScreen     = null;  // 初期画面
    private IMidpScreenCanvas mainMenu          = null;  // メインメニュー
    private IMidpScreenCanvas confirmation      = null;  // 確認画面
    private IMidpScreenCanvas colorPickScreen   = null;  // 色選択画面
    private IMidpScreenCanvas dirScreen         = null;  // ディレクトリ選択画面

    /**
     *  コンストラクタではなにもしない
     */
    public DotESceneDB()
    {
        //
    }

    /**
     *  キャンバス描画クラスを設定する
     *  @param screen キャンバス描画クラス
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        switch (screen.getCanvasId())
        {
          case DIR_SCREEN:
            // Dir選択画面
            dirScreen = screen;
            break;

          case MAIN_MENU:
            // メインメニュー
            mainMenu = screen;
            break;

          case CONFIRMATION:
            // 確認画面(LOAD)
            confirmation = screen;
            break;

          case COLORPICK_SCREEN:
        	// 色選択画面
            colorPickScreen = screen;
            break;
          case DEFAULT_SCREEN:
          default:
            // 初期画面
            defaultScreen = screen;
            break;
        }
        return;
    }

    /**
     *  データ入力用画面を設定する
     *  @param データ入力用画面
     *  
     */
    public void setForm(IMidpForm form)
    {
        preferenceForm = form;
        return;
    }

    
    /**
     *  クラスの実行準備...何もしない
     *
     */
    public void prepare()
    {
        //
    }

    /**
     *  ベースキャンバスクラスを記憶する
     *  
     *  @param baseCanvas ベースキャンバスクラス
     */
    public void setScreenObjects(MidpCanvas baseCanvas)
    {
        baseMidpCanvas  = baseCanvas;
    }

    /**
     *  デフォルト画面のIDを応答...
     *  @return デフォルトの画面ID (DEFAULT_SCREEN)
     */
    public int getDefaultScene()
    {
        return (DEFAULT_SCREEN);
    }

    /**
     *   画面を切り替えてOKかチェックする<br>
     *   基本的にはスルー、切替を許可する
     *
     *  @param sceneToChange 切り替える画面画面
     *  @param currentScene  現在表示中の画面
     */
    public boolean isAvailableChangeScene(int sceneToChange, int currentScene)
    {
        // 前画面はない、、、切り替えNGとする
        if (sceneToChange == NOSCREEN)
        {
            return (false);
        }
        return (true);
    }

    /**
     *   画面を切り替える
     *   @param currentScene  切り替える画面
     *   @param previousScene 切替前の画面（現在表示している画面）
     *   @param newTitle  新しい画面のタイトル
     *   @return Display.setCurrent() が必要な場合にtrueを応答
     */
    public boolean changeScene(int currentScene, int previousScene, String newTitle)
    {
        IMidpScreenCanvas screen = null;
        isShowForm = false;
        switch (currentScene)
        {
          case DIR_SCREEN:
            // Dir選択画面
            screen = dirScreen;
            break;

          case MAIN_MENU:
            // メインメニュー
            screen = mainMenu;
            break;

          case CONFIRMATION:
            // 確認画面
            screen = confirmation;
            break;

          case FILENAME_FORM:
            isShowForm = true;
            break;

          case COLORPICK_SCREEN:
        	screen = colorPickScreen;
        	break;

          case DEFAULT_SCREEN:
          default:
            // 初期画面
            screen = defaultScreen;
            break;  
        }
        if (isShowForm == false)
        {
            // キャンバスクラスへ切り替える
            baseMidpCanvas.changeActiveCanvas(screen);
        }
        else
        {
            // データ入力画面へ切り替える
            preferenceForm.setTitleString(newTitle);
        }
        return (true);
    }

    /**
     *   画面を切り替えた後の処理
     *   
     *   @param currentScene  切り替えた画面
     *   @param previousScene 切替前の画面
     */
    public void sceneChanged(int currentScene, int previousScene)
    {
        if (isShowForm == true)
        {
            preferenceForm.start();
        }
        return;
    }

    /**
     *   画面情報(文字列)を表示する
     *  @param scene 現在表示中の画面ID
     *  @return 画面情報(文字列)
     */
    public String getInfoMessage(int scene)
    {
        return ("");
    }

    /**
     *   画面クラスを応答する
     *   @param scene 現在表示中の画面ID
     *   @return 画面クラス
     */
    public Displayable getScreen(int scene)
    {
        if (scene == FILENAME_FORM)
        {
            return ((Displayable) preferenceForm);
        }
        return (baseMidpCanvas);
    }
}
