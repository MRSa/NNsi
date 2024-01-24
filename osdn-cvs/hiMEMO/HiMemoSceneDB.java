import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.framework.core.*;
import jp.sourceforge.nnsi.a2b.framework.interfaces.*;

/**
 *  画面切替の制御指示を行う。
 * 
 * @author MRSa
 */
public class HiMemoSceneDB implements IMidpSceneDB
{
    static public  final int INPUT_SCREEN    = 300; // データ入力画面
    static public  final int DEFAULT_SCREEN  = 100; // 初期画面
    static public  final int WELCOME_SCREEN  = 101; // 画像表示画面
    static public  final int DIR_SCREEN      = 102; // ディレクトリ一覧画面
   
    private boolean          isShowForm     = false; // データ入力中(データ入力画面表示中)
    private IMidpForm         inputForm     = null;  // データ入力画面
    private MidpCanvas      baseMidpCanvas  = null;  // ベースキャンバスクラス
    private IMidpScreenCanvas defaultScreen = null;  // 初期画面
    private IMidpScreenCanvas welcomeScreen = null;  // 「ようこそ」画面
    private IMidpScreenCanvas dirScreen     = null;  // ディレクトリ選択画面

    /**
     *  コンストラクタではなにもしない
     */
    public HiMemoSceneDB()
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
          case WELCOME_SCREEN:
            // ようこそ画面
            welcomeScreen = screen;
              break;

          case DIR_SCREEN:
            // ディレクトリ選択画面
            dirScreen = screen;
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
        inputForm = form;
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
        boolean previousShowForm = isShowForm;
        IMidpScreenCanvas screen = null;
        isShowForm = false;
        switch (currentScene)
        {
          case INPUT_SCREEN:
            isShowForm = true;
            break;

          case WELCOME_SCREEN:
              screen = welcomeScreen;
            break;

            case DIR_SCREEN:
              screen = dirScreen;
            break;

            case DEFAULT_SCREEN:
            default:
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
              inputForm.setTitleString(newTitle);
        }

        // キャンバスクラス → キャンバスクラスの切替時は、
        // setCurrent()を実行しない
        if ((previousScene != NOSCREEN)&&(isShowForm == previousShowForm))
        {
            return (false);
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
            inputForm.start();
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
        if (scene == INPUT_SCREEN)
        {
            return ((Displayable) inputForm);
        }
        return (baseMidpCanvas);
    }
}
