import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;

import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.forms.IDataInputStorage;
import jp.sourceforge.nnsi.a2b.utilities.IMidpCyclicTimerReport;
import jp.sourceforge.nnsi.a2b.utilities.MidpCyclicTimer;
import jp.sourceforge.nnsi.a2b.frameworks.IDataProcessing;

/**
 *  メイン画面用操作クラス
 * 
 * @author MRSa
 *
 */
public class fwSampleMainStorage implements ISampleMainCanvasStorage, IMidpCyclicTimerReport, ISelectionScreenStorage, IDataInputStorage
{
    private MidpCyclicTimer myTimer = null;
    private IDataProcessing reportDestination = null;
    private TextField       inputField = null;
    private int timerStatus = ISampleMainCanvasStorage.TIMER_READY;
    private int setTime     = 180;  // 設定時間（デフォルトは3分）
    private int currentTime = 0;    // 現在の経過時間
    private int vibrateTime = 500;  // タイムアウト時に振動する時間
    private int selectedItemId = -1;
    
    /**
     *  コンストラクタではタイマークラス生成と、タイムアウト発生時の報告先を設定します
     *
     */
    public fwSampleMainStorage(IDataProcessing destination)
    {
        myTimer = new MidpCyclicTimer(this, 1000);        
        myTimer.start();

        reportDestination = destination;
    }

    /**
     *   タイマーがスタートしているかどうかの状態を応答する
     * 
     * @return タイマーの状態
     */
    public int getTimerStatus()
    {
        return (timerStatus);
    }
    
    /**
     *   実行/停止が指示されたときの処理<br>
     * 　<li>停止中 → タイマースタート</li>
     *   <li>開始中 → 一時停止</li>
     *   <li>一時停止中 → タイマー再開</li>
     *   <li>終了中 → 停止中</li>
     * 
     * @return <code>true</code>:指示受付 / <code>false</code>:指示できる状態ではない
     */
    public boolean triggered()
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_READY)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_STARTED;
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_PAUSED;
        }
        else if (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_STARTED;
        }
        else // if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            timerStatus = ISampleMainCanvasStorage.TIMER_READY;
        }
        return (true);
    }
    
    /**
     *   タイマーをリセットする<br>
     *   停止中、あるいは終了中に受け付け可能。
     * 
     * @return <code>true</code>:指示受付 / <code>false</code>:指示できる状態ではない
     */    
    public boolean reset()
    {
        boolean ret = false;
        if ((timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)||
            (timerStatus == ISampleMainCanvasStorage.TIMER_READY)||
            (timerStatus == ISampleMainCanvasStorage.TIMER_PAUSED))
        {
            currentTime = 0;
            timerStatus = ISampleMainCanvasStorage.TIMER_READY;
            ret = true;
        }
        return (ret);
    }

    /**
     *  タイムアウト設定秒数を応答する
     */
    public int getSetTimeSeconds()
    {
        return (setTime);
    }

    /**
     *   タイムアウトするまでの残り秒数を応答する
     * @return 残り秒数、負のときはタイムアウト発生中
     */
    public int getRestSeconds()
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_TIMEOUT)
        {
            return (-1);
        }
        return (setTime - currentTime);
    }

    /**
     *  タイムアウト受信時の処理
     *  @return タイムアウト監視を継続する
     */
    public boolean reportCyclicTimeout(int count)
    {
        if (timerStatus == ISampleMainCanvasStorage.TIMER_STARTED)
        {
            currentTime++;
            if (currentTime >= setTime)
            {
                // タイムアウト検出！！
                timerStatus = ISampleMainCanvasStorage.TIMER_TIMEOUT;
                
                // タイムアウトを報告する！
                reportDestination.finishUpdateData(0, ISampleMainCanvasStorage.TIMER_TIMEOUT, vibrateTime);
            }
            else
            {
                // 画面を更新する
                reportDestination.updateData(0);
            }
        }
        return (true);
    }


    /**
     *  選択肢の数を取得する
     * 
     * @return 選択肢の数
     */ 
    public  int numberOfSelections()
    {
        return (5);
    }

    /**
     *  選択肢の文字列を取得する
     *  
     *  @param itemNumber 選択肢
     *  @return 選択肢の文字列
     */
    public String getSelectionItem(int itemNumber)
    {
        String item = "";
        switch (itemNumber)
        {
          case 4:
            item = "設定秒数入力";
            break;

          case 3:
            item = "7分(420秒) 設定";
            break;

          case 2:
            item = "5分(300秒) 設定";
            break;

          case 1:
            item = "3分(180秒) 設定";
            break;

          case 0:
          default:
            item = "2分(120秒) 設定";
            break;
        }
        return (item);
    }

    /**
     *  アイテム選択を複数同時に可能かどうかを応答する
     * 
     *  @return 同時選択可能(true) / １つだけ(false)
     */
    public boolean isSelectedItemMulti()
    {
        return (false);
    }

    /**
     *  選択したアイテムをクリアする(何もしない) 
     */
    public void resetSelectionItems()
    {
        
    }

    /**
     *  選択中のアイテム数を応答する(0)
     * 
     *  @return 選択中のアイテム数(0を応答する)
     */
    public int getNumberOfSelectedItems()
    {
        return (0);
    }

    /**
     *  選択中ののアイテムIDを応答する
     *  
     *  @param itemNumber 選択中のアイテム番号(ゼロスタート)
     *  @return アイテムID(そのまま応答する)
     */
    public int getSelectedItem(int itemNumber)
    {
        return (itemNumber);
    }

    /**
     *   選択したアイテムを設定する
     *   
     *   @param itemId 選択したアイテムID
     */
    public void setSelectedItem(int itemId)
    {
        selectedItemId = itemId;
    }

    /**
     *  選択されたボタンIDを設定する
     * 
     *  @param buttonId ボタンID
     *  @return 終了する(true) / 終了しない (false)
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId != ISelectionScreenStorage.BUTTON_CANCELED)
        {
            // タイマー設定値を変更する
            switch (selectedItemId)
            {
              case 4:
                // ここでは何もしない...
                break;

              case 3:
                setTime = 420;
                break;

              case 2:
                setTime = 300;
                break;

              case 1:
                setTime = 180;
                break;

              case 0:
              default:
                setTime = 120;
                break;
            }
        }
        return (true);
    }

    /**
     *  選択用のガイド文字列を取得する<br>
     *  （文字列を応答した場合、画面下部にガイド文字列を表示する)
     *  
     *  @return ガイド文字列
     *  
     */
    public String getSelectionGuidance()
    {
        return (null);
    }

    /**
     *  タイトルを表示するかどうか？
     *  
     *  @return タイトル表示する(true) / 表示しない(false)
     */
    public boolean isShowTitle()
    {
        return (false);
    }

    /**
     *  start時に、アイテムを初期化するかどうか<br>
     *  (カーソル位置を保持しておきたいときにfalseとする
     *  
     *  @return 初期化する(true) / 初期化しない(false)
     */
    public boolean isItemInitialize()
    {
        return (false);
    }

    /**
     *  ボタンの数を取得する
     * 
     *  @return 表示するボタンの数
     */
    public int getNumberOfSelectionButtons()
    {
        return (0);
    }

    /**
     *  ボタンのラベルを取得する
     *  
     *  @param buttonId ボタンのID
     *  @return 表示するボタンのラベル
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("");
    }

    /**
     *  背景を互い違いの色で表示するかを取得する
     *
     *  @return 互い違いの色にする(true) / 背景は１色(false)
     */
    public boolean getSelectionBackColorMode()
    {
        return (false);
    }

    /**
     *  メニューボタンが押されたタイミングを通知する
     * 
     *  @return 処理を継続するかどうか
     */
    public boolean triggeredMenu()
    {
        return (true);
    }

    /**
     *  次の画面切り替え先画面IDを取得する
     *  
     *  @param screenId (現在の)画面ID
     *  @return 切り替える画面ID
     */
    public int nextSceneToChange(int screenId)
    {
    	if (screenId == fwSampleFactory.DATAINPUT_SCREEN)
    	{
    		return (fwSampleFactory.DEFAULT_SCREEN);
    	}
    	if (screenId == fwSampleFactory.SELECTION_SCREEN)
    	{
    		if (selectedItemId == 4)
    		{
    			return (fwSampleFactory.DATAINPUT_SCREEN);
    		}
    	}
        return (ISelectionScreenStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  データ入力がキャンセルされたときの処理
     */
	public void cancelEntry()
	{
	    inputField = null;
	}

    /**
     *  データ入力がOKされたときの処理
     */
	public void dataEntry()
	{
        // 設定秒数を格納する
        setTime = Integer.valueOf(inputField.getString()).intValue();
        inputField = null;	
	}

	/**
     *  データ入力のための準備処理
     */
	public void prepare()
	{
		
	}

    /**
	 *  データ入力終了処理
	 */
	public void finish()
	{
	    System.gc();
	}

	/**
     *  表示するデータ入力フィールドの個数を取得する
     *  
     *  @return 表示するデータ入力フィールドの個数
     */
    public int getNumberOfDataInputField()
    {
    	return (1);
    }

    /**
     *  データ入力フィールドを取得する
     *  @param index データインデックス番号（０スタート）
     *  @return テキスト入力フィールド
     */
	public TextField getDataInputField(int index)
	{
		inputField = new TextField("設定時間(単位：秒)", "", 3, TextField.DECIMAL);
		return (inputField);
	}

	/**
     *  表示するオプション(選択入力項目)の個数を取得する
     *  @return 表示するオプションの個数 
     */
    public int getNumberOfOptionGroup()
    {
    	return (0);
    }
    
    /**
     *  表示するオプション(選択入力項目)を取得する
     *  @param index オプションインデックス番号（０スタート）
     *  @return オプション(選択入力項目)
     */
	public ChoiceGroup getOptionField(int index)
	{
		return (null);
	}

	/**
     *  画面切替先のタイトルを取得する
     *  @param screenId 画面ID
     *  @return 画面タイトル
     */
	public String nextSceneTitleToChange(int screenId)
	{
		return ("");
	}

}
