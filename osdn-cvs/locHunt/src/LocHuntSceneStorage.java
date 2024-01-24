import java.util.Date;
import java.io.InputStream;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.informations.*;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.forms.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * アプリケーションのデータを格納するクラス
 * 
 * @author MRSa
 *
 */
public class LocHuntSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{
    private final int  MaxDataInputLength   = 2048;

    /***** 揮発データ(ここから) *****/
    private TextField    inputDataField     = null;
    private TextField    inputNumberField   = null;
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;

    private String       memoData           = "";
    private String       memoDataNumeric    = "";
    /***** 揮発データ(ここまで) *****/

    /***** その他アクセスクラス *****/
    private MidpResourceFileUtils resUtils    = null;
    private MidpFileOperations fileOperation  = null;
//    private MidpKanjiConverter kanjiConverter = null;
    private LocHuntDataStorage storageDb = null;
    
    private boolean   isFirstLocation = false;     // 初回のロケーション...
    private double    startLatitude   = 510.0;
    private double    startLongitude  = 510.0;
    private double    leftLatitude    = 510.0;
    private double    leftLongitude   = 510.0;
    private double    rightLatitude   = 510.0;
    private double    rightLongitude  = 510.0;
    private double    upLatitude      = 510.0;
    private double    upLongitude     = 510.0;
    private double    downLatitude    = 510.0;
    private double    downLongitude   = 510.0;
    
    private boolean   pictureSaveAction = false;
    
    /**
     *  コンストラクタ
     *
     *  @param dataStorage データ記録クラス
     *  @param resourceUtils リソースアクセスクラス
     *  @param fileUtils ファイルアクセスクラス
     */
    public LocHuntSceneStorage(LocHuntDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpFileOperations fileUtils, MidpKanjiConverter knjConv, MidpFileHttpCommunication httpComm, IDataProcessing dataProcess)
    {
        storageDb = dataStorage;
        fileOperation = fileUtils;
//        kanjiConverter = knjConv;
        resUtils = resourceUtils;
    }

    /**------------------------------------------------------------**
     **
     **  ここから保管データの読み書き...
     **
     **
     **------------------------------------------------------------**/
    /**
     * ディレクトリの応答
     * @return ディレクトリ名
     */
    public String getDefaultDirectory()
    {
        return (storageDb.getBaseDirectory());
    }

    /**
     *  選択されている（ファイル）数を応答する
     *  
     */
    public  int getSelectedNameCount()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  ディレクトリの応答
     * @return ディレクトリ名
     */
    public String getSelectedName(int index)
    {
        return (storageDb.getBaseDirectory());
    }
    
    /**
     *   ディレクトリの応答
     * @param ディレクトリ名
     */
    public void setSelectedName(String name)
    {
        storageDb.setBaseDirectory(name);
    }

    /**
     *  選択中のディレクトリ名をクリアする
     * 
     */
    public void clearSelectedName()
    {
        storageDb.setBaseDirectory(null);
        return;
    }
    
    /**
     *  設定したディレクトリ名を確定する
     *  (何もしない)
     * 
     */
    public void confirmSelectedName()
    {
        return;
    }
    
    /**------------------------------------------------------------**
     **
     **  ここから揮発データの読み書き...
     **
     **
     **------------------------------------------------------------**/
    /**
     * 表示するメッセージを応答する
     * 
     * @return
     */
    public String getInformationMessageToShow()
    {
        String message = "";
        try
        {
            message = resUtils.getResourceText("welcome.txt");
        }
        catch (Exception ex)
        {
            //
        }
        return (message);
    }

    /**
     *   ボタン数を応答する
     *   
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }

    /**
     * メッセージが受け付けられたかどうか設定する
     * @param buttonId 押されたボタンのID
     */
    public void setButtonId(int buttonId)
    {
        // ボタンが押された！！（画面遷移！）
        
    }

    /**
     *  情報表示画面のボタンラベルを応答
     * 
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return("Dir選択");
    }

    /**
     *  データ入力がキャンセルされたとき
     *  
     */
    public void cancelEntry()
    {
        inputDataField = null;
        inputNumberField = null;
        System.gc();
        return;
    }

    /**
     *  データ入力がＯＫされたとき
     *  
     */
    public void dataEntry()
    {
        // メモデータ
        String ddd  = inputDataField.getString();
        if (ddd.length() > 0)
        {
//            String data = kanjiConverter.UrlEncode(kanjiConverter.ParseToEuc(ddd));
            memoData = ddd;    // 現状は揮発領域...(不揮発領域に変更しても良いかも)
        }
        else
        {
        	memoData = "";
        }

        // 数字メモデータ
        ddd  = inputNumberField.getString();
        if (ddd.length() > 0)
        {
//          String data = kanjiConverter.UrlEncode(kanjiConverter.ParseToEuc(ddd));
        	memoDataNumeric = ddd;    // 現状は揮発領域...(不揮発領域に変更しても良いかも)
        }
        else
        {
        	memoDataNumeric = "";
        }

        inputDataField = null;
        inputNumberField = null;
        System.gc();
        return;
    }

    /**
     *  データ入力のための準備
     *  
     */
    public void prepare()
    {
        //  何もしない
    }

    /**
     *  次画面の切り替え
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == LocHuntSceneDB.WELCOME_SCREEN)
        {
            return (LocHuntSceneDB.DIR_SCREEN);
        }
        else if (screenId == LocHuntSceneDB.DIR_SCREEN)
        {
            return (LocHuntSceneDB.DEFAULT_SCREEN);
        }
        return (IDataInputStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  次画面の切り替え
     * 
     */
    public String nextSceneTitleToChange(int screenId)
    {
        return ("");
    }

    /**
     *  データ入力終了
     *  
     */
    public void finish()
    {
        // 何もしない
    }

    /**
     *  データ入力フィールドの個数を取得する
     *  
     */
    public int getNumberOfDataInputField()
    {
        return (2);
    }

    /**
     *  データ入力フィールドを取得する
     *  
     */
    public TextField getDataInputField(int index)
    {
    	if (index == 0)
    	{
            inputDataField = new TextField("メモ", memoData, MaxDataInputLength, TextField.ANY);
            return (inputDataField);
    	}
    	else
    	{
    		inputNumberField = new TextField("(数値)", memoDataNumeric, MaxDataInputLength, TextField.NUMERIC);    		
    	}
        return (inputNumberField);
    }

    /**
     *  オプションの個数を取得する
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (0);
    }

    /**
     *   オプションを取得する
     */
    public ChoiceGroup getOptionField(int index)
    {
        return (null);
    }

    /**
     *  選択肢の数を取得する
     *  
     */
    public int numberOfSelections()
    {
        return (0);
    }

    /**
     *  選択肢の文字列を取得する
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
    }
    
    /**
     *  選択用のガイド文字列を取得する
     *  
     */
    public String getSelectionGuidance()
    {
        return ("MenuでDir確定");
    }

    /**
     *  タイトルを表示するかどうか？
     *  
     */
    public boolean isShowTitle()
    {
        return (true);
    }

    /**
     *   start時、アイテムを初期化するか？
     * 
     */
    public boolean isItemInitialize()
    {
        return (true);
    }

    /**
     *  ボタンの数を取得する
     *  
     */
    public int getNumberOfSelectionButtons()
    {
        return (1);
    }

    /**
     *  ボタンのラベルを取得する
     *  
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("DIR選択");
    }

    /**
     *  背景を互い違いの色で表示するか？
     *  
     */
    public boolean getSelectionBackColorMode()
    {
        return (false);
    }

    
    public void setSelectedItem(int itemId)
    {
        itemSelectionIndex = itemId;
    }

    public int getSelectedItem(int itemNumber)
    {
        if (itemNumber != 0)
        {
            return (-1);
        }
        return (itemSelectionIndex);
    }
    
    public boolean isSelectedItemMulti()
    {
        return (false);
    }

    public int getNumberOfSelectedItems()
    {
        if (itemSelectionIndex < 0)
        {
            return (0);
        }
        return (1);
    }

    // 選択されたボタンIDを応答する
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    // アイテム選択をクリアする
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    // メニューボタンが押された
    public boolean triggeredMenu()
    {    
        // 画面を切り替える...
        return (true);
    }

    /**
     *  情報メッセージを設定する
     * 
     * @param message
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }

    /**
     *  情報メッセージを応答する
     * 
     * @return
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }    

    /**
     *  位置情報データを出力(追記)する
     *  (世界測地系で、かつCSV形式でデータを出力する, Super Mapple Digitalで取込できるように...)
     *  
     *   フォーマット : latitude,longitude,fileName,"memoData1", memoData2
     */
    public boolean outputLocationData(String fileName, LocationDeviceController.CurrentLocation location)
    {
    	boolean ret = false;
        try
        {
        	// 世界測地系で、かつCSV形式でデータを出力する
            double latitude =location.getLatitudeDouble(true);
            double longitude = location.getLongitudeDouble(true);

            String data = "" + latitude + "," + longitude + "," + fileName + ",";
            if (memoData.length() > 0)
            {
                // 撮影メモを出力する （入力されていた場合のみ出力）
                data = data + "\"" + memoData + "\"";
            }
            data = data + ",";
            if (memoDataNumeric.length() > 0)
            {
                // 撮影メモ(数字データ)を出力する （入力されていた場合のみ出力）
                data = data + memoDataNumeric;
            }
            // 追記モードでテキストファイル出力を行う
            fileOperation.outputDataTextFile(storageDb.getBaseDirectory() + "locHuntLog.csv", null, null, data, true);
        }
        catch (Exception ex)
        {
        	// 
        	ret = false;
        }
        return (ret);
    }

    /**
     *  付加情報データを出力する
     */
    public boolean outputInformationData(String fileNameHeader, LocationDeviceController.CurrentLocation location, boolean linkImage, int width, int height)
    {
    	boolean ret = false;
    	
        try
        {
            // 画像ファイルが出力できた場合...それと連携するデータファイルを出力する
            Date  currentDate = new Date();
            String data;
            data = "<html><head><title>";
            data = data + fileNameHeader;
            if (linkImage == true)
            {
                data = data + ".jpg";
            }
            else
            {
                data = data + ".html";
            }
            data = data + "</title></head>\r\n<body>";
            if (memoData.length() > 0)
            {
                // 撮影メモを出力する （入力されていた場合）
                data = data + memoData;
            }
            if (memoDataNumeric.length() > 0)
            {
                // 撮影メモ(数字データ)を出力する （入力されていた場合）
                data = data + "\r\n " + memoDataNumeric;
            }
            data = data + "<hr>\r\n";
            data = data + "日時：<br>" + currentDate.toString();
            data = data + "<br>\r\n";
            if (location != null)
            {
                // 位置情報を出力する （取得できていた場合場合）
                data = data + "郵便番号 : " + location.getZipCode() + "<br>\r\n";
                data = data + "緯度 : " + location.getLatitude() + " " + "経度 : " + location.getLongitude();
                data = data + "<hr>\r\n";
            }
            // 画像ファイルへのリンク
            if (linkImage == true)
            {
                data = data + "<P align=\"center\"><IMG src=\"" + fileNameHeader + ".jpg\"" + " alt =\"" + fileNameHeader + ".jpg\"";
                if ((width > 0)&&(height > 0))
                {
                    // 表示する画像データのサイズが指定されていた場合、設定する
                    data = data + " height=\"" + height + "\" width=\"" + width + "\"";
                }
                data = data + "></P>";
                data = data + "<hr>\r\n";
            }
            if (location != null)
            {
                // google mapsへのリンク (マップデータを表示する) // Zoom Level 15 が良いかも... (14でも良い？)
                double latitude =location.getLatitudeDouble(true);
                double longitude = location.getLongitudeDouble(true);
                data = data + "<a href=\"http://maps.google.com/staticmap?" + "center=" + latitude + "," + longitude +
                              "&maptype=mobile" + "&zoom=15&size=240x240" +
                              "&markers=" + latitude + "," + longitude + ",red" +
                              "&key=ABQIAAAAy2Fi9hJaQOg1ZfoEosnyWxS1zhnEwl123lCDx5CEbHiJbEqVOxTf6Oe45-Kkc7sNyF9fr9_fXAl6_w" +
                              "\">" + "位置確認(Google Maps)</a><br>\r\n";
                data = data + "<a href=\"http://mobile.yuubinbangou.net/search.php?kw=" + location.getZipCode() + "" +
                              "\">" + "場所確認(〒郵便番号検索)</a><br>\r\n";
                /*     // XMLで応答されるため、利用できない...
                data = data + "<a href=\"http://api.knecht.jp/reverse_geocoding/api/?lat=" + latitude + "&lng=" + longitude +
                              "\">" + "住所確認(api.knecht.jp)</a><br>\r\n";
                */
                data = data + "<a href=\"http://www.chizumaru.com/czm/currentadr.aspx?x=" + location.getLongitudeSeconds(false) + "&y=" + location.getLatitudeSeconds(false) +
                              "\">" + "住所確認(ちず丸)</a><br>\r\n";
            }
            // 「locHuntへ戻る」タグを埋め込む。
            data = data + "<object id=\"app\" declare=\"declare\" classid=\"x-oma-application:java-ams\">";
            data = data + "\r\n" + "  <param name=\"AMS-Filename\" value=\"http://nnsi.sourceforge.jp/archives/midp/locHunt/locHunt.jad\"/>";
            data = data + "\r\n" + "  <param name=\"MIDlet-Name\" value=\""    + resUtils.getAppProperty("MIDlet-Name") + "\"/>";
            data = data + "\r\n" + "  <param name=\"MIDlet-Version\" value=\"" + resUtils.getAppProperty("MIDlet-Version") + "\"/>";
            data = data + "\r\n" + "  <param name=\"MIDlet-Vendor\" value=\""  + resUtils.getAppProperty("MIDlet-Vendor") + "\"/>";
            data = data + "\r\n" + "<param name=\"AMS-Startup\" value=\"download-confirm\"/>";
            data = data + "\r\n" + "</object><a href=\"#app\">(" + resUtils.getAppProperty("MIDlet-Name") + "へ戻る)</a>";

            // タグの末尾を設定する
            data = data + "\r\n</body></html>";
            fileOperation.outputDataTextFile(storageDb.getBaseDirectory() + fileNameHeader + ".html", "", "", data, false);
        }
        catch (Exception ex)
        {
        	// 
        	ret = false;
        }
        return (ret);
    }
    
    /**
     *   画像データを出力する
     * @param fileNameHeader
     * @param data
     * @return
     */
    public boolean outputImageData(String fileNameHeader, InputStream inputStream)
    {
    	boolean ret = false;
        try
        {        	
            if (inputStream == null)
            {
                // 位置情報の取得
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();

                // 画像データが取得できない... 付加情報のみ出力する
                outputLocationData(fileNameHeader + ".html", location);
                ret = outputInformationData(fileNameHeader, location, false, 0, 0);            	
                return (ret);
            }
     
            // 画像データの出力
            ret = fileOperation.outputRawDataToFile(storageDb.getBaseDirectory() + fileNameHeader + ".jpg", inputStream, false);
//            inputStream.close();    // 必要であれば実行しよう...
            if (ret == true)
            {
                // 位置情報の取得 (位置情報を取得できなくても画像ファイルを出力したいから...)
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();

                // 付加情報を出力する
                int width = 120;     // HTMLで表示する画像データのサイズ (幅)
                int height = 160;    // HTMLで表示する画像データのサイズ (高さ)
                outputLocationData(fileNameHeader + ".jpg", location);
                outputInformationData(fileNameHeader, location, true, width, height);
            }
        }
        catch (Exception ex)
        {
            //
        }
        return (ret);
    }
    
    
    /**
     *  位置情報をクリアする
     *
     */
    public void clearLocation()
    {
        leftLatitude   = 510.0;
        leftLongitude  = 510.0;
        rightLatitude  = 510.0;
        rightLongitude = 510.0;
        upLatitude     = 510.0;
        upLongitude    = 510.0;
        downLatitude   = 510.0;
        downLongitude  = 510.0;
    }
    
    /**
     *  位置情報を記録する
     * @param keyCode
     * @return
     */
    public boolean setLocation(int keyCode)
    {
    	boolean ret = false;
    	try
    	{
            if (keyCode == javax.microedition.lcdui.Canvas.LEFT)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                leftLatitude = location.getLatitudeDouble(true);
                leftLongitude = location.getLongitudeDouble(true);
                ret = true;
            }
            else if (keyCode == javax.microedition.lcdui.Canvas.RIGHT)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                rightLatitude = location.getLatitudeDouble(true);
                rightLongitude = location.getLongitudeDouble(true);
                ret = true;
            }
            else if (keyCode == javax.microedition.lcdui.Canvas.UP)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                upLatitude = location.getLatitudeDouble(true);
                upLongitude = location.getLongitudeDouble(true);
                ret = true;
            }
            else if (keyCode == javax.microedition.lcdui.Canvas.DOWN)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                downLatitude = location.getLatitudeDouble(true);
                downLongitude = location.getLongitudeDouble(true);
                ret = true;        	
            }
    	}
    	catch (Exception ex)
    	{
    		ret = false;
    	}
    	return (ret);
    }
    
    /**
     *   初期の位置情報を仮に記憶する
     *
     */
    public void setStartLocation(boolean forceMemory)
    {
    	if ((forceMemory == false)&&(isFirstLocation == true))
    	{
    		return;
    	}
    	try
    	{
            // 位置情報の取得と記憶
            LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
            startLatitude = location.getLatitudeDouble(true);
            startLongitude = location.getLongitudeDouble(true);
            isFirstLocation = true;
    	}
    	catch (Exception ex)
    	{
    		// 
    	}    	
    }
    
    /**
     *  Google Static MapのＵＲＬを応答する
     * @param width              画像の幅
     * @param height             画像の高さ
     * @param isCurrentLocation  現在の位置もサポートするか？
     * @return
     */
    public String getGoogleMapStaticMapUrl(int width, int height, boolean isCurrentLocation)
    {
    	double latitude = 0.0;
    	double longitude = 0.0;
    	try
    	{
            // 位置情報の取得と記憶
            LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
            latitude = location.getLatitudeDouble(true);
            longitude = location.getLongitudeDouble(true);
    	}
    	catch (Exception ex)
    	{
    		// 
    	}
    	if (startLatitude > 500)
    	{
    		startLatitude  = latitude;
    		startLongitude = longitude;
    	}

    	String data = "http://maps.google.com/staticmap?" +
                      "&maptype=mobile" + "&size=" + width + "x" + height + 
                      "&markers=" + startLatitude + "," + startLongitude + ",blueb";
        if (upLatitude < 500)
        {
            data = data + "|" + upLatitude + "," + upLongitude + ",redu";
        }
        if (downLatitude < 500)
        {
            data = data + "|" + downLatitude + "," + downLongitude + ",redd";
        }
        if (leftLatitude < 500)
        {
            data = data + "|" + leftLatitude + "," + leftLongitude + ",redl";
        }
        if (rightLatitude < 500)
        {
            data = data + "|" + rightLatitude + "," + rightLongitude + ",redr";
        }
        if ((isCurrentLocation == true)&&(latitude < 500))
        {
            data = data + "|" + latitude + "," + longitude + ",greene";
        }
        data = data + "&key=ABQIAAAAy2Fi9hJaQOg1ZfoEosnyWxS1zhnEwl123lCDx5CEbHiJbEqVOxTf6Oe45-Kkc7sNyF9fr9_fXAl6_w";
        return (data);
    }    

    /**
     *  「データを保存するか？」を設定する
     * @param data
     */
    public void setSaveAction(boolean data)
    {
    	pictureSaveAction = data;
    }

    /**
     *  「データを保存するか？」を応答する
     * @return
     */
    public boolean getSaveAction()
    {
    	return (pictureSaveAction);
    }
}
