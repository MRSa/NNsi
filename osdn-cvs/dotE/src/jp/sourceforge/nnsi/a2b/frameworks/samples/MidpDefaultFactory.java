/**
 *  a2B Frameworkのサンプル実装パッケージです。<br>
 *  最低限アプリとして成立させるために必要なクラスをパッケージ化しています。
 */
package jp.sourceforge.nnsi.a2b.frameworks.samples;

import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Font;

/**
 *  ファクトリクラス(のサンプル実装)
 *  
 *  @author MRSa
 *
 */
public class MidpDefaultFactory implements IMidpFactory
{
    /** デフォルトの画面IDです */
    public final int DEFAULT_SCREEN = 100;
    
    private MidpDefaultSceneDB sceneDb = null;
    private MidpDefaultStorage dataStorage = null;
    
    /**
     *  コンストラクタでは何も処理しません
     *
     */
    public MidpDefaultFactory()
    {
        //
    }

    /**
     *  オブジェクトを生成します。<br>
     *  ここでは最低限シーン切替用のデータベースクラス(IMidpSceneDB)と
     *  データ保存用クラス(IMidpDataStorage)を生成する必要があります
     *  
     *  @see IMidpSceneDB
     *  @see IMidpDataStorage
     * 
     */
    public void prepareCoreObjects()
    {
        // シーン切替データクラスの生成
        sceneDb = new MidpDefaultSceneDB();
        
        // データストレージクラスの生成
        dataStorage = new MidpDefaultStorage();
    }

    /**
     *  シーン切替データクラスを応答する
     * 
     *  @return シーン切替データクラス
     */
    public IMidpSceneDB getSceneDB()
    {
        return (sceneDb);
    }

    /**
     *  データストレージクラスを応答する
     * 
     *  @return データストレージクラス
     */
    public IMidpDataStorage getDataStorage()
    {
        return (dataStorage);
    }

    /**
     *  オブジェクトを生成する
     * 
     *  @param object MIDletベースクラス
     *  @param selector シーンセレクタ
     */
    public void prepareObjects(MIDlet object, MidpSceneSelector selector)
    {

        // 画面サイズの取得
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // 画面(ベース)クラス : キャンバス
        MidpCanvas myCanvas = new MidpCanvas(selector, Font.SIZE_SMALL);
        myCanvas.prepare(baseWidth, baseHeight);

        // ベースクラスを設定する
        sceneDb.setScreenObjects(myCanvas);

        // 画面サイズの取得
        // int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        // int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // 画面の登録
        IMidpScreenCanvas screenObject = null;
        screenObject = new MidpScreenCanvas(DEFAULT_SCREEN, selector);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(10, 10, (baseWidth - 20), (baseHeight - 20));
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
