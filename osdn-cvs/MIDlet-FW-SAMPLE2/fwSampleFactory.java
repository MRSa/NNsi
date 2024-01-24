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

        // メイン画面の登録 (newして、フォントを設定して、画面サイズを設定する)
        IMidpScreenCanvas screenObject = null;
        screenObject = new fwSampleMainCanvas(DEFAULT_SCREEN, selector, new fwSampleMainStorage(myCanvas));
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
