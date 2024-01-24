import javax.microedition.lcdui.Display;
import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.forms.MidpDataInputForm;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpFactory;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import javax.microedition.midlet.MIDlet;

/**
 *  ファクトリクラス
 *  
 *  @author MRSa
 *
 */
public class TheCountFactory implements IMidpFactory
{
    private TheCountSceneDB      sceneDb = null;
    private TheCountDataStorage  dataStorage = null;
    
    /**
     *  コンストラクタ
     *
     */
    public TheCountFactory()
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
        sceneDb = new TheCountSceneDB();
        
        // データストレージクラスの生成
        dataStorage = new TheCountDataStorage(new MidpRecordAccessor(), new MidpFileOperations());
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

        // 画面サイズの取得
    	int baseWidth  = 240;
    	int baseHeight = 280;
    	try
    	{
            baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
            baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();
    	}
    	catch (Exception ex)
    	{
    		//
    	}

        // 画面(ベース)クラス : キャンバス
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare(baseWidth, baseHeight);

        // ベースクラスを設定する
        sceneDb.setScreenObjects(myCanvas);

        // ユーティリティクラスの生成
        TheCountSceneStorage sceneStorage  = new TheCountSceneStorage(dataStorage, new MidpResourceFileUtils(object));

        // ファイルディレクトリ検索クラス
        MidpFileDirList dirList = new MidpFileDirList();

        // 画面クラス : フォーム
        MidpDataInputForm myForm = new MidpDataInputForm("アイテム名設定", TheCountSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // 画面クラスの登録
        IMidpScreenCanvas screenObject = null;        

        // 情報表示用画面の登録
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(TheCountSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("TheCountの説明");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ビジー表示画面の登録
        informationOperator = null;
        BusyConfirmationStorage busyStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(busyStorage);
        screenObject = new InformationScreen(TheCountSceneDB.BUSY_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        busyStorage.prepare(screenObject);
        screenObject.setTitle("");
        screenObject.setRegion(20, 20, baseWidth - 40, baseHeight - 40);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ディレクトリ一覧画面の登録
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        screenObject = new DirectorySelectionDialog(TheCountSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // グラフ画面の登録
        screenObject = new TheCountSummaryScreen(TheCountSceneDB.SUMMARY_SCREEN, selector, dataStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("グラフ表示");
        screenObject.setRegion(2, 2, (baseWidth - 4), (baseHeight - 4));
        sceneDb.setCanvas(screenObject);
        System.gc();

        // メイン画面の登録
        screenObject = new TheCountMainScreen(TheCountSceneDB.DEFAULT_SCREEN, selector, dataStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
