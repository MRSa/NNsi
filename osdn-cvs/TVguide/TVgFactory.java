import javax.microedition.lcdui.Display;

import jp.sourceforge.nnsi.a2b.frameworks.MidpCanvas;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpFactory;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpDataStorage;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpSceneDB;
import jp.sourceforge.nnsi.a2b.frameworks.IMidpScreenCanvas;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialog;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreen;
import jp.sourceforge.nnsi.a2b.screens.selections.DirectorySelectionDialogOperator;
import jp.sourceforge.nnsi.a2b.screens.selections.SelectionScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationDefaultStorage;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreenOperator;
import jp.sourceforge.nnsi.a2b.screens.informations.InformationScreen;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileDirList;
import jp.sourceforge.nnsi.a2b.utilities.MidpRecordAccessor;
import jp.sourceforge.nnsi.a2b.utilities.MidpDefaultA2BhttpCommunicator;
import jp.sourceforge.nnsi.a2b.utilities.MidpA2BhttpCommunication;
import jp.sourceforge.nnsi.a2b.utilities.MidpResourceFileUtils;
import jp.sourceforge.nnsi.a2b.utilities.MidpDateUtils;
import javax.microedition.midlet.MIDlet;

/**
 *  ファクトリクラス
 *  
 *  @author MRSa
 *
 */
public class TVgFactory implements IMidpFactory
{
    private TVgSceneDb      sceneDb = null;
    private TVgDataStorage  dataStorage = null;
    
    /**
     *  コンストラクタ
     *
     */
    public TVgFactory()
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
        sceneDb = new TVgSceneDb();
        
        // データストレージクラスの生成
        dataStorage = new TVgDataStorage(new MidpRecordAccessor());
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
        dataStorage.setObjects(myCanvas);
        
        // ベースクラスを設定する
        sceneDb.setScreenObjects(myCanvas);

        // ユーティリティクラスの生成
        MidpDefaultA2BhttpCommunicator httpCommunicatorSet = new MidpDefaultA2BhttpCommunicator(false);
        dataStorage.setHttpCommunicator(httpCommunicatorSet);
        String cookie = "areaId=" + dataStorage.getAreaId() + "; span=24";
        httpCommunicatorSet.SetCookie(cookie);
        MidpA2BhttpCommunication httpComm = new MidpA2BhttpCommunication(httpCommunicatorSet, null);
        dataStorage.setResourceUtils(new MidpResourceFileUtils(object));

        // ファイルディレクトリ検索クラス
        MidpFileDirList dirList = new MidpFileDirList();

        // 画面サイズの取得
        int baseWidth  = (Display.getDisplay(object)).getCurrent().getWidth();
        int baseHeight = (Display.getDisplay(object)).getCurrent().getHeight();

/**
        // 画面クラス : フォーム (今回は(まだ)使わない)
        MidpDataInputForm myForm = new MidpDataInputForm("検索文字列", A2chSSceneDB.INPUT_SCREEN, selector, sceneStorage);
        myForm.prepare();
        myForm.setOkButton(false);
        myForm.setCancelButton(false);
        sceneDb.setForm(myForm);
**/
        // 画面クラスの登録
        IMidpScreenCanvas screenObject = null;        

        // 情報表示用画面(Welcome画面)の登録
        InformationDefaultStorage infoStorage = new InformationDefaultStorage();
        dataStorage.setInformationStorage(infoStorage);
        InformationScreenOperator informationOperator = new InformationScreenOperator(infoStorage);
        screenObject = new InformationScreen(TVgSceneDb.SCREEN_INFORMATION, selector, informationOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setTitle("説明");
        screenObject.setRegion(7, 7, baseWidth - 15, baseHeight - 15);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // 番組一覧表示画面の登録
        TVgProgramDataParser dataParser = new TVgProgramDataParser(dataStorage, httpComm, myCanvas, new MidpDateUtils());
        TVgSelectionStorage selectionStorage = new TVgSelectionStorage(dataParser, dataStorage);
        sceneDb.setSelectionStorage(selectionStorage);
        screenObject = new TVgProgramScreen(TVgSceneDb.SCREEN_PROGRAMLIST, selector, dataParser);
        screenObject.prepare((myCanvas.getFont()));
        screenObject.setTitle("");
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // 日付一覧表示画面の登録
        screenObject = new TVgDateScreen(TVgSceneDb.SCREEN_DATELIST, selector, dataParser);
        screenObject.prepare((myCanvas.getFont()));
        screenObject.setTitle("");
        screenObject.setRegion(0, 0, baseWidth, baseHeight);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // ディレクトリ一覧画面の登録
        DirectorySelectionDialogOperator fileSelectionOperator = new DirectorySelectionDialogOperator(selectionStorage, selectionStorage, dirList, true);
        screenObject = new DirectorySelectionDialog(TVgSceneDb.SCREEN_DIRSELECT, selector, fileSelectionOperator, fileSelectionOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(20, 20, baseWidth - 40, baseHeight - 40);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // メニュー画面の登録
        SelectionScreenOperator sceneOperator = new SelectionScreenOperator(selectionStorage);
        screenObject = new SelectionScreen(TVgSceneDb.SCREEN_MENUSELECT, selector, sceneOperator);
        screenObject.prepare(myCanvas.getFont());
        int width  = 140;
        int height = myCanvas.getFont().getHeight() * 3;
        screenObject.setRegion((baseWidth - width), (baseHeight - height), width, height);
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        // 選択画面の登録
        screenObject = new SelectionScreen(TVgSceneDb.SCREEN_SELECTION, selector, sceneOperator);
        screenObject.prepare(myCanvas.getFont());
        screenObject.setRegion(4, 4, (baseWidth - 8), (baseHeight - 8));
        sceneDb.setCanvas(screenObject);
        screenObject = null;

        System.gc();
        return;
    }
}
