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
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import javax.microedition.midlet.MIDlet;

/**
 *  ファクトリクラス
 *  
 *  @author MRSa
 *
 */
public class A2BUFactory implements IMidpFactory
{
    private A2BUSceneDB      sceneDb = null;
    private A2BUDataStorage  dataStorage = null;
    
    /**
     *  コンストラクタ
     *
     */
    public A2BUFactory()
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
        sceneDb = new A2BUSceneDB();
        
        // データストレージクラスの生成
        dataStorage = new A2BUDataStorage(new MidpRecordAccessor());
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
        // 画面(ベース)クラス : キャンバス
        MidpCanvas myCanvas = new MidpCanvas(selector, dataStorage.getFontSize());
        myCanvas.prepare();

        // ベースクラスを設定する
        sceneDb.setScreenObjects(myCanvas);

        // ファイルディレクトリ検索クラス
        MidpFileDirList dirList = new MidpFileDirList();

        // ユーティリティクラスの生成
        A2BUSceneStorage sceneStorage  = new A2BUSceneStorage(dataStorage, new MidpResourceFileUtils(object), myCanvas, dirList);

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
        
        // 画面クラス : フォーム
        MidpDataInputForm myForm = new MidpDataInputForm("a2BU設定", A2BUSceneDB.EDITPREF_FORM, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // 画面クラスの登録
        IMidpScreenCanvas screenObject = null;        

        // 情報表示用画面の登録
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);
        screenObject = new InformationScreen(A2BUSceneDB.WELCOME_SCREEN, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("a2BUの説明");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ディレクトリ一覧画面の登録
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        screenObject = new DirectorySelectionDialog(A2BUSceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // ビジー表示画面の登録
        informationOperator = null;
        BusyConfirmationStorage busyStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(busyStorage);
        screenObject = new InformationScreen(A2BUSceneDB.CONFIRMATION, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        busyStorage.prepare(screenObject);
        screenObject.setTitle("");
        screenObject.setRegion(25, 25, baseWidth - 50, baseHeight - 50);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // メイン画面の登録
        screenObject = new A2BUMainScreen(A2BUSceneDB.DEFAULT_SCREEN, selector, dataStorage, busyStorage, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        // メイン画面の登録
        screenObject = new A2BUProcessScreen(A2BUSceneDB.PROCESS_SCREEN, selector, busyStorage, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("新着確認実行中...");
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

        return;
    }
}
