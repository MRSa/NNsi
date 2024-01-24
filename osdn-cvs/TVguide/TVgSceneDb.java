import javax.microedition.lcdui.Displayable;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpForm;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;

/**
 *  シーン切替データベース
 * 
 * @author MRSa
 *
 */
public class TVgSceneDb implements IMidpSceneDB
{
    static public final int SCREEN_INFORMATION = 100; // 情報表示画面
    static public final int SCREEN_DIRSELECT   = 200; // ディレクトリ選択画面
    static public final int SCREEN_MENUSELECT  = 300; // メニュー表示画面
    static public final int SCREEN_SELECTION   = 400; // データ選択画面
    static public final int SCREEN_PROGRAMLIST = 500; // 番組一覧表示画面
    static public final int SCREEN_DATELIST    = 600; // 日付一覧表示画面

    static public final int SCENE_WELCOME          = 110; // 「ようこそ」表示
    static public final int SCENE_BUSY             = 120; // 「ビジー」表示
    static public final int SCENE_DIRSELECTION     = 210; // ディレクトリ選択
    static public final int SCENE_MENUSELECTION    = 310; // メニュー選択
    static public final int SCENE_STATIONSELECTION = 410; // テレビ局選択
    static public final int SCENE_TVTYPESELECTION  = 420; // 電波種別選択
    static public final int SCENE_LOCALESELECTION  = 430; // 地域選択
    static public final int SCENE_PROGRAMLIST      = 510; // 番組リスト
    static public final int SCENE_DATELIST         = 610; // 日付リスト   
    
    protected MidpCanvas baseScreen = null;

    private String infoTitle = null;

    private TVgSelectionStorage selectionStorage = null;

    private IMidpScreenCanvas infoScreen    = null;  // 情報表示
    private IMidpScreenCanvas dirScreen     = null;  // ディレクトリ選択表示
    private IMidpScreenCanvas menuScreen    = null;  // メニュー表示
    private IMidpScreenCanvas selectScreen  = null;  // データ選択表示
    private IMidpScreenCanvas programScreen = null;  // 番組一覧表示
    private IMidpScreenCanvas dateScreen    = null;  // 日付一覧表示

    /**
     *  コンストラクタでは何も操作しません
     *
     */
    public TVgSceneDb()
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
     *  選択ストレージを記憶する
     * @param storage
     */
    public void setSelectionStorage(TVgSelectionStorage storage)
    {
        selectionStorage = storage;
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
        return (SCENE_DATELIST);
    }

    /**
     *  スクリーン表示画面クラスを設定する
     *  @param screen スクリーン表示画面クラス
     */
    public void setCanvas(IMidpScreenCanvas screen)
    {
        int canvasId = screen.getCanvasId();
        switch (canvasId)
        {
          case SCREEN_INFORMATION: // 情報表示画面
              infoScreen = screen;
            break;

          case SCREEN_DIRSELECT: // ディレクトリ選択画面
              dirScreen = screen;
            break;

          case SCREEN_MENUSELECT: // メニュー表示画面
              menuScreen = screen;
            break;

          case SCREEN_SELECTION: // データ選択画面
              selectScreen = screen;
            break;

          case SCREEN_PROGRAMLIST: // 番組一覧表示画面
              programScreen = screen;
            break;

          case SCREEN_DATELIST: // 日付一覧表示画面
              dateScreen = screen;
            break;

          default:
            break;
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
        IMidpScreenCanvas screen = null;
        int nextCanvasId = getNextCanvasId(next);
        switch (nextCanvasId)
        {
          case SCREEN_INFORMATION: // 情報表示画面
              screen = infoScreen;
            break;

          case SCREEN_DIRSELECT: // ディレクトリ選択画面
              screen = dirScreen;
            break;

          case SCREEN_MENUSELECT: // メニュー表示画面
              screen = menuScreen;
            break;

          case SCREEN_SELECTION: // データ選択画面
              screen = selectScreen;
            break;

          case SCREEN_PROGRAMLIST: // 番組一覧表示画面
              screen = programScreen;
            break;

          case SCREEN_DATELIST: // 日付一覧表示画面
              screen = dateScreen;
            break;

          default:
            break;
        }

        if (current == SCENE_WELCOME)
        {
            // ようこそ画面だった場合、ディレクトリ選択画面へ切り替える
            screen = dirScreen;
            next = SCENE_DIRSELECTION;
        }
          else if (current == SCENE_DIRSELECTION)
        {
            // ディレクトリ画面だった場合、日付一覧表示画面へ切り替える
            screen = dateScreen;
            next = SCENE_DATELIST;
        }

        infoTitle = title;

        if (screen != null)
        {
            // シーン設定を切り替える
            selectionStorage.setSceneMode(next);
            
            // 画面タイトルを設定する
            screen.setTitle(title);
            
            // キャンバスクラスへ切り替える
            baseScreen.changeActiveCanvas(screen);
        }
        return (true);
    }

    /**
     *  次画面のスクリーンクラス特定
     * @param nextScene
     * @return
     */
    private int getNextCanvasId(int nextScene)
    {
        int canvasId = nextScene;
        switch (nextScene)
        {
          case SCENE_WELCOME:  // 「ようこそ」表示
          case SCENE_BUSY:     // 「ビジー」表示
            canvasId = SCREEN_INFORMATION;
            break;

          case SCENE_DIRSELECTION:     // ディレクトリ選択
              canvasId = SCREEN_DIRSELECT;
            break;
          case SCENE_MENUSELECTION:    // メニュー選択
              canvasId = SCREEN_MENUSELECT;
            break;
          case SCENE_STATIONSELECTION: // テレビ局選択
          case SCENE_TVTYPESELECTION:  // 電波種別選択
          case SCENE_LOCALESELECTION:  // 地域選択
              canvasId = SCREEN_SELECTION;
            break;
          case SCENE_PROGRAMLIST:      // 番組リスト
              canvasId = SCREEN_PROGRAMLIST;
            break;
          case SCENE_DATELIST:         // 日付リスト   
              canvasId = SCREEN_DATELIST;
            break;
          default:
            // その他はそのまま
            break;
        }
        return (canvasId);
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
        return (infoTitle);
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
