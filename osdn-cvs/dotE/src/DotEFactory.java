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
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreen;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreenOperator;
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
public class DotEFactory implements IMidpFactory
{
    private DotESceneDB      sceneDb = null;
    private DotEDataStorage  dataStorage = null;
    
    /**
     *  コンストラクタ
     *
     */
    public DotEFactory()
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
        sceneDb = new DotESceneDB();
        
        // データストレージクラスの生成
        dataStorage = new DotEDataStorage(new MidpRecordAccessor());
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
    	int baseHeight = 276;
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

        // ファイルディレクトリ検索クラス
        MidpFileDirList dirList = new MidpFileDirList();
        dirList.prepare(dataStorage.getBaseDirectory());

        // ユーティリティクラスの生成
        DotESceneStorage sceneStorage  = new DotESceneStorage(dataStorage, new MidpResourceFileUtils(object), myCanvas, dirList);
        
        // 画面クラス : フォーム
        MidpDataInputForm myForm = new MidpDataInputForm("ファイル名指定", DotESceneDB.FILENAME_FORM, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);

        // 画面クラスの登録
        IMidpScreenCanvas screenObject = null;        
        InformationScreenOperator informationOperator = new InformationScreenOperator(sceneStorage);

        // ディレクトリ一覧画面の登録
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(sceneStorage, sceneStorage, dirList, false);
        fileSelectionOperator.prepare();
        screenObject = new DirectorySelectionDialog(DotESceneDB.DIR_SCREEN, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(30, 30, baseWidth - 60, baseHeight - 60);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        
        // ビジー表示画面の登録
        informationOperator = null;
        BusyConfirmationStorage confirmStorage = new BusyConfirmationStorage();
        informationOperator = new InformationScreenOperator(confirmStorage);
        screenObject = new InformationScreen(DotESceneDB.CONFIRMATION, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        confirmStorage.prepare(screenObject);
        screenObject.setTitle(" 確認 ");
        screenObject.setRegion(25, 25, baseWidth - 50, 45);
        sceneDb.setCanvas(screenObject);
        screenObject = null;
        System.gc();

        // メニュー画面の登録(選択画面の登録)
        DotESelectionMenu selectionMenu = new DotESelectionMenu(confirmStorage);
        screenObject = new SelectionScreen(DotESceneDB.MAIN_MENU, selector, new SelectionScreenOperator(selectionMenu));
        screenObject.prepare(myCanvas.getFont());
        int width  = 84;
        int height = myCanvas.getFont().getHeight() * 9;
        screenObject.setRegion((baseWidth - width), (baseHeight - height), width, height);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // メイン画面の登録
        screenObject = new DotEMainScreen(DotESceneDB.DEFAULT_SCREEN, selector, dataStorage, confirmStorage, selectionMenu);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle(dataStorage.getApplicationName());
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        System.gc();

/*
        // 色選択画面の登録 (当初計画では、カラーピッカー画面を用意するつもりだったが、やめた）
        screenObject = new DotEColorPickerScreen(DotESceneDB.COLORPICK_SCREEN, selector, dataStorage, sceneStorage);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("カラー選択");
        screenObject.setRegion(10, 10, baseWidth - 20, baseHeight - 20);
        sceneDb.setCanvas(screenObject);
        System.gc();
*/
        return;
    }
}
