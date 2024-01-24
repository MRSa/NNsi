import javax.microedition.lcdui.Display;

import jp.sourceforge.nnsi.a2b.framework.core.MidpCanvas;
import jp.sourceforge.nnsi.a2b.framework.core.MidpDataInputForm;
import jp.sourceforge.nnsi.a2b.framework.core.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpFactory;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.framework.interfaces.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.InformationScreen;
import jp.sourceforge.nnsi.a2b.screens.operator.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.operator.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import javax.microedition.midlet.MIDlet;

/**
 *  ファクトリクラス
 *  
 *  @author MRSa
 *
 */
public class HiMemoFactory implements IMidpFactory
{
    private HiMemoSceneDB      sceneDb = null;
    private HiMemoDataStorage  dataStorage = null;
    
    /**
     *  コンストラクタ
     *
     */
    public HiMemoFactory()
    {
        //
    }

    /**
     *  オブジェクトの生成
     * 
     */
    public void prepareCoreObjects()
    {
        // シーン切替データクラスの生成
        sceneDb = new HiMemoSceneDB();
        
        // データストレージクラスの生成
        dataStorage = new HiMemoDataStorage(new MidpRecordAccessor());
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
     *  @param select シーンセレクタ
     */
    public void prepareObjects(MIDlet object, MidpSceneSelector selector)
    {
        // ユーティリティクラスの生成
        HiMemoSceneStorage sceneStorage  = new HiMemoSceneStorage(dataStorage, new MidpResourceFileUtils(object), new MidpFileOperations());

        // ファイルディレクトリ検索クラス
        MidpFileDirList dirList = new MidpFileDirList();

        // 画面サイズの取得
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

        // 画面(ベース)クラス : キャンバス
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare();

        // ベースクラスを設定する
        sceneDb.setScreenObjects(myCanvas);
        
        // 画面クラス : フォーム
        MidpDataInputForm myForm   = new MidpDataInputForm("記録ディレクトリ選択", HiMemoSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // 画面クラスの登録
        IMidpScreenCanvas screenObject = null;        

        // 情報表示用画面の登録
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(HiMemoSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("追記メモの説明");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ディレクトリ一覧画面の登録
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList);
        screenObject = new DirectorySelectionDialog(HiMemoSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);        
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();
        
        // メイン画面の登録
        screenObject = new HiMemoMainScreen(HiMemoSceneDB.DEFAULT_SCREEN, selector, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        return;
    }
}
