import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.informations.BusyConfirmationStorage;

public class DotESelectionMenu implements ISelectionScreenStorage
{
	private int currentSelectedItem = -1;
	private int nextSceneId = SCENE_TO_PREVIOUS;
	private BusyConfirmationStorage confirmStorage = null;

    public DotESelectionMenu(BusyConfirmationStorage confirm)
    {
        confirmStorage = confirm;
    }
	
    /**
     *  選択肢の数を取得する
     * 
     * @return 選択肢の数
     */ 
    public int numberOfSelections()
    {
    	return (9);
    }

    /**
     *  選択肢の文字列を取得する
     *  
     *  @param itemNumber 選択肢
     *  @return 選択肢の文字列
     */
    public String getSelectionItem(int itemNumber)
    {
        String itemString = "";
    	switch (itemNumber)
    	{
	      case 8:
			itemString = "アプリ終了";
            break;

	      case 7:
			itemString = "領域クリア";
			break;

	      case 6:
			itemString = "領域貼付";
			break;

	      case 5:
		    itemString = "領域指定";
		    break;

	      case 4:
	        itemString = "File名変更";
	        break;

	      case 3:
	        itemString = "Dir選択";
	        break;

	      case 2:
	        itemString = "Load";
	        break;

	      case 1:
		    itemString = "Save";
            break;

	      case 0:
	      default:
		    itemString = "色の選択";
            break;
/*
  	      case 0:
          default:
        	itemString = "カラー選択";
            break;
*/
    	}
    	return (itemString);
    	
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
     *  選択したアイテムをクリアする
     *  
     */
    public void resetSelectionItems()
    {
        currentSelectedItem = -1;
        nextSceneId = SCENE_TO_PREVIOUS;
    }
    
    /**
     *  選択中のアイテム数を応答する
     * 
     *  @return 選択中のアイテム数
     */
    public int getNumberOfSelectedItems()
    {
        if (currentSelectedItem == -1)
        {
        	return (0);
        }
        return (1);
    }

    /**
     *  選択中ののアイテムIDを応答する
     *  
     *  @param itemNumber 選択中のアイテム番号(ゼロスタート)
     *  @return アイテムID
     */
    public int getSelectedItem(int itemNumber)
    {
    	return (currentSelectedItem);
    }


    /**
     *   選択したアイテムを設定する
     *   
     *   @param itemId 選択したアイテムID
     */
    public void setSelectedItem(int itemId)
    {
    	currentSelectedItem = itemId;
    }

    /**
     *  選択されたボタンIDを設定する
     * 
     *  @param buttonId ボタンID
     *  @return 終了する(true) / 終了しない (false)
     */
    public boolean setSelectedCommandButtonId(int buttonId)
    {
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
		return (true);
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
		return (null);
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
        int sceneId = DotESceneDB.DEFAULT_SCREEN;
        nextSceneId = SCENE_TO_PREVIOUS;
        // currentSelectedItem の内容で分岐
		switch (currentSelectedItem)
		{
           case 8:
        	 // アプリ終了
  		     sceneId = DotESceneDB.CONFIRMATION;
	         confirmStorage.prepareMessage("アプリを終了します。", BusyConfirmationStorage.CONFIRMATION_MODE, DotESceneDB.DEFAULT_SCREEN);
	         nextSceneId = DotESceneDB.CONFIRMATION_END;
	         break;

           case 7:
        	 // コピー領域のクリア
             sceneId = DotESceneDB.DEFAULT_SCREEN;
    	     nextSceneId = DotESceneDB.REGION_CLEAR;
             break;

           case 6:
        	 // コピー領域の貼り付け
             sceneId = DotESceneDB.DEFAULT_SCREEN;
             nextSceneId = DotESceneDB.REGION_PASTE;
             break;

           case 5:
        	 // コピー領域の指定
             sceneId = DotESceneDB.DEFAULT_SCREEN;
    	     nextSceneId = DotESceneDB.REGION_COPY;
             break;

           case 4:
             sceneId = DotESceneDB.FILENAME_FORM;
	         break;

		   case 3:
		     sceneId = DotESceneDB.DIR_SCREEN;
		     break;

           case 2:
		     sceneId = DotESceneDB.CONFIRMATION;
	         confirmStorage.prepareMessage("データを読み出します。", BusyConfirmationStorage.CONFIRMATION_MODE, DotESceneDB.DEFAULT_SCREEN);
	         nextSceneId = DotESceneDB.CONFIRMATION_LOAD;
	         break;

           case 1:
			 sceneId = DotESceneDB.CONFIRMATION;
	         confirmStorage.prepareMessage("データを保存します。", BusyConfirmationStorage.CONFIRMATION_MODE, DotESceneDB.DEFAULT_SCREEN);
	         nextSceneId = DotESceneDB.CONFIRMATION_SAVE;
		     break;

           case 0:
  			 sceneId = DotESceneDB.DEFAULT_SCREEN;
  	         nextSceneId = DotESceneDB.COLORPICK_PIXEL;
  		     break;
/*
	  	   case 0:
		     sceneId = DotESceneDB.COLORPICK_SCREEN;
		     break;
*/
	  	   default:
	         sceneId = DotESceneDB.DEFAULT_SCREEN;
	         break;
		 }
		 currentSelectedItem = -1;
         return (sceneId);
	}
	
	/**
	 *  次の画面IDを取得する
	 * @return 画面ID
	 */
	public int getNextSceneId()
	{
		return (nextSceneId);
	}	
}
