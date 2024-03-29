import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.frameworks.samples.*;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;

/**
 *  サンプル用ファクトリクラス
 *  
 *  @author MRSa
 */
public class fwSampleFactory implements IMidpFactory
{
    /** デフォルトの画面IDです */
    public static final int DEFAULT_SCREEN = 100;
    
    /** 選択画面用の画面IDです */
    public static final int SELECTION_SCREEN = 110;
    
    private fwSampleSceneDB sceneDb = null;
    private MidpDefaultStorage dataStorage = null;
    
    /**
     *  コンストラクタでは何も処理しません
     */
    public fwSampleFactory()
    {
        //
    }

    /**
     *  コアオブジェクトを生成します。<br>
     *  ここでは最低限シーン切替用のデータベースクラス(IMidpSceneDB)と
     *  データ保存用クラス(IMidpDataStorage)を生成する必要があります
     *  
     *  @see IMidpSceneDB
     *  @see IMidpDataStorage
     */
    public void prepareCoreObjects()
    {
        // シーン切替データクラスの生成
        sceneDb = new fwSampleSceneDB();
        
        // データストレージクラスの生成
        dataStorage = new MidpDefaultStorage();
    }

    /**
     *  シーン切替データクラスを応答します
     * 
     *  @return シーン切替データクラス
     */
    public IMidpSceneDB getSceneDB()
    {
        return (sceneDb);
    }

    /**
     *  データストレージクラスを応答します
     * 
     *  @return データストレージクラス
     */
    public IMidpDataStorage getDataStorage()
    {
        return (dataStorage);
    }

    /**
     *  コアオブジェクト以外のオブジェクトを生成します<br>
     * 
     *  @param object MIDletベースクラス
     *  @param selector シーンセレクタ
     */
    public void prepareObjects(MIDlet object, MidpSceneSelector selector)
    {
        // 画面(ベース)クラス : キャンバス
        MidpCanvas myCanvas = new MidpCanvas(selector, Font.SIZE_SMALL);
        myCanvas.prepare();

        // ベースクラスを設定する
        sceneDb.setScreenObjects(myCanvas);

        // 画面サイズの取得
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        fwSampleMainStorage mainStorage = new fwSampleMainStorage(myCanvas);
        
        
        // メイン画面の登録 (newして、フォントを設定して、画面サイズを設定する)
        IMidpScreenCanvas screenObject = null;
        screenObject = new fwSampleMainCanvas(DEFAULT_SCREEN, selector, mainStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);

        // 選択画面の登録
        screenObject = null;
        screenObject = new SelectionScreen(SELECTION_SCREEN, selector, new SelectionScreenOperator(mainStorage));
        screenObject.prepare(myCanvas.getFont());
        int width  = 80;
        int height = myCanvas.getFont().getHeight() * 4;
        screenObject.setRegion((baseWidth - width), (baseHeight - height), width, height);
        sceneDb.setCanvas(screenObject);
        
        System.gc();

        return;
    }
}
