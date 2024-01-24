import java.util.Vector;

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
public class A2chSSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{
    private final int  MaxDataInputLength   = 2048;

    /***** 揮発データ(ここから) *****/
    private TextField    inputDataField     = null;
    private String       find2chUrl         = null;
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;
    /***** 揮発データ(ここまで) *****/

    /***** その他アクセスクラス *****/
    private IDataProcessing    dataProcessing = null;
    private MidpFileHttpCommunication httpCommunicator = null;
    private MidpResourceFileUtils resUtils    = null;
    private MidpFileOperations fileOperation  = null;
    private MidpKanjiConverter kanjiConverter = null;
    private A2chSDataStorage storageDb = null;
    
    // 2ch検索の結果格納リスト
    private Vector pickup2chList = null;
    
    /**
     *  コンストラクタ
     *
     *  @param dataStorage データ記録クラス
     *  @param resourceUtils リソースアクセスクラス
     *  @param fileUtils ファイルアクセスクラス
     */
    public A2chSSceneStorage(A2chSDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpFileOperations fileUtils, MidpKanjiConverter knjConv, MidpFileHttpCommunication httpComm, IDataProcessing dataProcess)
    {
        storageDb = dataStorage;
        fileOperation = fileUtils;
        kanjiConverter = knjConv;
        resUtils = resourceUtils;
		httpCommunicator = httpComm;
		dataProcessing = dataProcess;
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
    	    message = resUtils.getResourceText("/res/welcome.txt");
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
    	find2chUrl = null;
    	return;
    }

    /**
     *  データ入力がＯＫされたとき
     *  
     */
    public void dataEntry()
    {
        // 文字列をEUC変換してURLを生成する...
    	String ddd  = inputDataField.getString();
    	if (ddd.length() > 0)
    	{
            String data = kanjiConverter.UrlEncode(kanjiConverter.ParseToEuc(ddd));
            find2chUrl = "http://find.2ch.net/?BBS=ALL&TYPE=TITLE&STR=" + data + "&COUNT=25";
    	}
        inputDataField = null;
        pickup2chList = null;
        System.gc();
        return;
    }

    /**
     *  データ入力された検索URLがある場合...
     * 
     * @return 検索するURL
     */
    public String getUrlToSearch()
    {
        return (find2chUrl);
    }

    /**
     *  検索用URLをクリアする
     */
    public void resetUrlToSearch()
    {
    	find2chUrl = null;
    }

    /**
     *  データ入力のための準備
     *  
     */
    public void prepare()
    {
        // 何もしない        
    }

    /**
     *  次画面の切り替え
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == A2chSSceneDB.WELCOME_SCREEN)
        {
            return (A2chSSceneDB.DIR_SCREEN);
        }
        else if (screenId == A2chSSceneDB.DIR_SCREEN)
        {
            return (A2chSSceneDB.DEFAULT_SCREEN);
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
        return (1);
    }

    /**
     *  データ入力フィールドを取得する
     *  
     */
    public TextField getDataInputField(int index)
    {
        inputDataField = new TextField("検索文字列", "", MaxDataInputLength, TextField.ANY);
        return (inputDataField);
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
     *  2ch検索の実行。。。
     * 
     * @param url
     */
    public void doFind2chThreads()
    {
    	httpCommunicator.SetUserAgent("a2chS/1.00");
        httpCommunicator.DoHttpMain(find2chUrl, storageDb.getBaseDirectory(), "temporary.html");
        find2chUrl = null;
        dataProcessing.finishUpdateData(0, 0, 300);
        return;
    }

    /**
     *   テンポラリファイル（検索結果ファイル）が存在するか？
     * 
     * @return
     */
    public boolean isExistSearchResultFile()
    {
        return (MidpFileOperations.IsExistFile(storageDb.getBaseDirectory() + "temporary.html"));
    }

    /**
     *  検索結果ファイルを読み込んで準備する
     * 
     */
    public boolean prepareSearchResultFile()
    {
    	int nofThreadTitles = 0;
    	byte[] data = fileOperation.ReadFileWhole(storageDb.getBaseDirectory() + "temporary.html");
    	if (data == null)
    	{
            // ファイル読み出し失敗...
    		return (false);
    	}

        // 2chリスト
        pickup2chList = null;
        System.gc();
        pickup2chList = new Vector();

        // データ解析メイン処理...
    	nofThreadTitles = parseFind2ch(data);

        // あとしまつ
        data = null;
        System.gc();
        if (nofThreadTitles == 0)
        {
        	return (false);
        }
    	return (true);
    }

    /**
     * ２ちゃんねる検索の結果を解析する (a2Bよりもってくる)
     * 
     * @param dataString
     */
    private int parseFind2ch(byte[] dataString)
    {
        int firstIndex = 0;

        // 検索を開始する先頭を探す
        try
        {
            int endIndex   = dataString.length - 4;
            for (int i = 0; i < endIndex ; i++)
            {
                if ((dataString[i] == '<')&&(dataString[i + 1] == 'd')&&(dataString[i + 2] == 't')&&(dataString[i + 3] == '>'))
                {
                    firstIndex = i;
                    break;
                }
            }
        }
        catch (Exception e)
        {
            //
        }

        int index = firstIndex;
        while ((index >= 0)&&(index < dataString.length))
        {
            index = pickupFind2chData(dataString, index);
        }
        return (pickup2chList.size());
    }

    /**
     * ２ちゃんねる検索で拾った、URLをとってくる
     * 
     * @param buffer
     * @param startIndex
     * @return
     */
    private int pickupFind2chData(byte[] buffer, int startIndex)
    {
        int threadNumIndexStart   = startIndex;
        int threadNumIndexEnd     = startIndex;
        int threadNickIndexEnd    = startIndex;
        int threadTitleIndexStart = startIndex;
        int threadTitleIndexEnd   = startIndex;
        int boardHostIndexStart   = startIndex;
        int boardHostIndexEnd     = startIndex;
        int endIndex = buffer.length;

        try
        {
            boolean anchor = false;
        	for (int index = startIndex; index < buffer.length; index++)
            {
        		if ((buffer[index + 0] == ':')&&(buffer[index + 1] == '/')&&(buffer[index + 2] == '/'))
                {
                	boardHostIndexStart = index + 3;
                	for (int numberIndex = boardHostIndexStart; numberIndex < buffer.length; numberIndex++)
                	{
                        if (buffer[numberIndex] == '/')
                        {
                            // ホスト名の末尾
                        	boardHostIndexEnd = numberIndex;
                        	index = numberIndex;
                        	break;
                        }
                	}
                }
                else if ((buffer[index + 0] == 'r')&&(buffer[index + 1] == 'e')&&
                    (buffer[index + 2] == 'a')&&(buffer[index + 3] == 'd')&&
                    (buffer[index + 4] == '.')&&(buffer[index + 5] == 'c')&&
                    (buffer[index + 6] == 'g')&&(buffer[index + 7] == 'i')&&
                    (buffer[index + 8] == '/'))
                {
                    // "read.cgi/" を見つける
                    threadNumIndexStart = index + 9;
                    boolean firstSlash = false;  // あー、ヤダヤダ、フラグなんて、、、。
                    for (int numberIndex =threadNumIndexStart; numberIndex < buffer.length; numberIndex++)
                    {
                        if (buffer[numberIndex] == '/')
                         {
                            if (firstSlash == true)
                            {
                                threadNumIndexEnd = numberIndex;
                                index = numberIndex;
                                anchor = true;
                                break;
                            }
                            else
                            {
                                firstSlash = true;
                                threadNickIndexEnd = numberIndex;
                            }
                         }
                    }
               
                }

                //                else if ((buffer[index + 0] == '<')&&(buffer[index + 1] == 'b')&&(buffer[index + 2] == '>'))
                else if (anchor == true)
                {
                	while ((buffer[index] != '>')&&(index < buffer.length))
                	{
                		index++;
                	}
                	index++;
                    threadTitleIndexStart = index;
                    for (int titleIndex =threadTitleIndexStart; titleIndex < buffer.length; titleIndex++)
                    {
                        if ((buffer[titleIndex + 0] == '<')&&(buffer[titleIndex + 1] == '/')&&
                             (buffer[titleIndex + 2] == 'a')&&(buffer[titleIndex + 3] == '>'))
                        {
                            threadTitleIndexEnd = titleIndex; 
                            break;
                        }
                    }

                    // ここでタイトルをエントリする
                    if ((threadNumIndexStart < threadNumIndexEnd)&&
                        (threadTitleIndexStart < threadTitleIndexEnd)&&
                        (threadNumIndexStart < threadTitleIndexStart))
                    {
                        String title  = kanjiConverter.ParseFromEUC(buffer, threadTitleIndexStart, (threadTitleIndexEnd - threadTitleIndexStart));
                        String host   = new String(buffer, boardHostIndexStart, (boardHostIndexEnd - boardHostIndexStart));
                        String number = new String(buffer, (threadNickIndexEnd + 1), (threadNumIndexEnd - (threadNickIndexEnd + 1)));
                        String nick   = new String(buffer, threadNumIndexStart, (threadNickIndexEnd - threadNumIndexStart));
                        title = "[" + nick + "]"+ title;
                        pickup2chList.addElement(new a2chSThreadInformation(title, host, nick, number));
                    }
                    endIndex = threadTitleIndexEnd;
                    break;
                }
            }
        	while ((buffer[endIndex] != 0x0a)&&(endIndex < buffer.length))
        	{
        		endIndex++;
        	}
        }
        catch (Exception e)
        {
            endIndex = -1;
        }
        return (endIndex);        
    }

    /**
     *  スレタイ数を応答する
     * 
     * @return
     */
    public int getNumberOfThreadTitles()
    {
    	if (pickup2chList == null)
    	{
    		return (0);
    	}
        return (pickup2chList.size());
    }

    /**
     *   スレ情報を応答する
     * 
     * @param itemNumber
     * @return
     */
    public a2chSThreadInformation getThreadInformation(int itemNumber)
    {
    	try
    	{
            // 範囲オーバーチェック
        	if ((itemNumber < 0)||(itemNumber >= pickup2chList.size()))
        	{
        		return (null);
        	}
        	a2chSThreadInformation threadInfo = (a2chSThreadInformation) pickup2chList.elementAt(itemNumber);
        	return (threadInfo);
    	}
    	catch (Exception e)
    	{
    		//
    	}
    	return (null);
    }

    /**
     *  スレををhttp通信で取得する
     * 
     * @param threadInfo
     * @return 成功(true) / 失敗(false)
     */
    public boolean getDatFile(a2chSThreadInformation threadInfo)
    {
        setInformationMessage("datFile取得中...");

        // ディレクトリを調整する
    	String directory = storageDb.getBaseDirectory() + threadInfo.getBoardNick();
        if (directory == null)
        {
        	return (false);
        }
        // ディレクトリを生成する
        fileOperation.MakeDirectory(directory);
        
        // ファイル名
        String fileName = threadInfo.getDatFileName();

        // datファイルが存在しているかどうかチェック
        boolean fileIsExist = MidpFileOperations.IsExistFile(directory + fileName);

        // ユーザーエージェントを調整する
        httpCommunicator.SetUserAgent("Monazilla/1.00  (MIDP2.0)");

        // 通信を行ってdatファイルを取得する
        httpCommunicator.DoHttpMain(threadInfo.getUrl(), directory, fileName);

        // 既にファイルがあったか？
        if (fileIsExist == false)
        {
            // ファイルがなければa2b.idxに追加する
        	// (本来はindexファイルに記録があるかないかチェック必要)
            appendToA2Bindex(directory, fileName);
        }
        setInformationMessage("取得終了 : " + threadInfo.getTitle());
//        dataProcessing.updateInformation("取得終了 :" + threadInfo.getTitle(), 0, false);
        dataProcessing.finishUpdateData(0, 0, 300);
        return (true);
    }

    /**
     *  a2b.idxに取得したファイルが記録されていなければ追記する
     * 
     * @param directory 管理ディレクトリ
     * @param datFile datファイル
     */
    private void appendToA2Bindex(String directory, String datFile)
    {
    	String targetFileName = directory + "a2b.idx";

        // タイトル行が必要か？
    	String title = null;
    	if (MidpFileOperations.IsExistFile(targetFileName) == false)
    	{
    		// ファイルがない...a2b.idxを新規に生成する場合にはヘッダを生成する
			title = "; FILEname, max, current, status, lastModified, 0, (Title)";
    	}

    	// データ行の生成
    	String dataToWrite = datFile + ",1,1,1,0,0,(a2B_INTERIM)" + datFile + "\r\n; [End]";
        
        // インデックスファイルへの追記を実行！
    	fileOperation.outputDataTextFile(targetFileName, null, title, dataToWrite, true);
    	return;
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
     *  スレ情報格納クラス
     *  <br>2ch検索の結果を保持する
     *  <i>スレタイ</i>
     *  <i>ホスト名</i>
     *  <i>板ニックネーム</i>
     *  <i>datファイル名</i>
     * @author MRSa
     *
     */
    public class a2chSThreadInformation
    {
        private String title  = null;
        private String host   = null;
        private String nick   = null;
        private String datNum = null;    	
    	
    	/**
    	 *   コンストラクタ
    	 * @param titleString  スレタイトル
    	 * @param hostString   ホスト名
    	 * @param nickString   板ニックネーム
    	 * @param datNumString dat番号
    	 */
    	public a2chSThreadInformation(String titleString, String hostString, String nickString, String datNumString)
    	{
            title  = titleString;
            host   = hostString;
            nick   = nickString;
            datNum = datNumString;
    	}
    	
    	/**
    	 *  スレタイトルを応答する
    	 * @return スレタイトル文字列(UTF8形式に変換済み)
    	 */
    	public String getTitle()
    	{
    		return (title);
    	}

    	/**
    	 *  datファイルのURLを応答する
    	 * @return datファイルのURL 'http://(ホスト)/(nick)/dat/(dat番号).dat'
    	 */
    	public String getUrl()
    	{
    	    return ("http://" + host + "/" + nick + "/dat/" + datNum + ".dat");
    	}

    	/**
    	 *  板nick名を応答する(末尾にスラッシュあり！)
    	 * @return 板nick '(名前)'
    	 */
    	public String getBoardNick()
    	{
    		return (nick + "/");
    	}

        /**
         *  datファイル名を取得する
         * @return datファイル名 (xxxxxxxxxx.dat)
         */
    	public String getDatFileName()
    	{
    		return (datNum + ".dat");
    	}
    }
}
