import jp.sourceforge.nnsi.a2b.screens.selections.ISelectionScreenStorage;
import jp.sourceforge.nnsi.a2b.screens.selections.IFileSelectionStorage;
/**
 *  選択画面で表示するデータを保持するクラスです。
 * @author MRSa
 *
 */
public class TVgSelectionStorage implements ISelectionScreenStorage, IFileSelectionStorage
{
    TVgDataStorage       storageDb  = null;
    TVgProgramDataParser dataParser = null;
    boolean isConfirmation = false;

    String selectedFileName = null;
    
    int scene = -1;
    int selectedItemId = -1;

    /**
     *  コンストラクタでは、何も処理をおこないません
     *
     */
    public TVgSelectionStorage(TVgProgramDataParser parser, TVgDataStorage storage)
    {
        dataParser = parser;
        storageDb  = storage;
    }

    /**
     *  選択肢を表示するシーンを設定する
     * 
     * @param mode 表示シーン
     */
    public void setSceneMode(int mode)
    {
        scene = mode;
    }    
    
    /**
     *  選択肢の数を取得する
     * 
     * @return 選択肢の数
     */ 
    public int numberOfSelections()
    {
        int items = 0;
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // メニュー選択
            items = 3;
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // テレビ局選択
            items = dataParser.getNumberOfTvStations();
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // 電波種別選択
            items = 3;
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // 地域選択
            items = 54;
            break;

          default:
            break;
        }
        return (items);
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
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // メニュー選択
            itemString = getSelectionMenu(itemNumber);
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // テレビ局選択
            itemString = dataParser.getTvStationName(itemNumber);
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // 電波種別選択
            itemString = getStationTypeMenu(itemNumber);
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // 地域選択
            itemString = getLocalMenu(itemNumber);
            break;

          default:
            break;
        }
        return (itemString);
    }

    /**
     *  メニューの選択肢（文字列）を応答する
     * @param itemNumber アイテム番号
     * @return アイテム名
     */
    public String getSelectionMenu(int itemNumber)
    {
        String name = "";
        if (itemNumber == 0)
        {
            name = "地域の選択";
        }
        else if (itemNumber == 1)
        {
            name = "局種別選択";
        }
        else if (itemNumber == 2)
        {
            name = "ディレクトリ指定";
        }
        return (name);
    }
    
    /**
     *  テレビ局種別の選択肢（文字列）を応答する
     * @param itemNumber アイテム番号
     * @return アイテム名
     */
    public String getStationTypeMenu(int itemNumber)
    {
        String name = "";
        if (itemNumber == 0)
        {
            name = "地上波";
        }
        else if (itemNumber == 1)
        {
            name = "UHF/BS";
        }
        else if (itemNumber == 2)
        {
            name = "BSデジタル";
        }
        return (name);
    }

    /**
     *  地域選択の選択肢（文字列）を応答する
     * @param itemMenu アイテム番号
     * @return アイテム名
     */
    public String getLocalMenu(int itemMenu)
    {
        String itemString = "";
        switch (itemMenu)
        {
          case 0:
              itemString = "北海道（札幌）";
              break;
          case 1:
              itemString = "北海道（函館）";
              break;
          case 2:
              itemString = "北海道（旭川）";
              break;
          case 3:
              itemString = "北海道（帯広）";
              break;
          case 4:
              itemString = "北海道（釧路）";
              break;
          case 5:
              itemString = "北海道（北見）";
              break;
          case 6:
              itemString = "北海道（室蘭）";
              break;
          case 7:
              itemString = "青森";
              break;
          case 8:
              itemString = "岩手";
              break;
          case 9:
              itemString = "宮城";
              break;
          case 10:
              itemString = "秋田";
              break;
          case 11:
              itemString = "山形";
              break;
          case 12:
              itemString = "福島";
              break;
          case 13:
              itemString = "茨城";
              break;
          case 14:
              itemString = "栃木";
              break;
          case 15:
              itemString = "群馬";
              break;
          case 16:
              itemString = "埼玉";
              break;
          case 17:
              itemString = "千葉";
              break;
          case 18:
              itemString = "東京";
              break;
          case 19:
              itemString = "神奈川";
              break;
          case 20:
              itemString = "新潟";
              break;
          case 21:
              itemString = "山梨";
              break;
          case 22:
              itemString = "長野";
              break;
          case 23:
              itemString = "富山";
              break;
          case 24:
              itemString = "石川";
              break;
          case 25:
              itemString = "福井";
              break;
          case 26:
              itemString = "岐阜";
              break;
          case 27:
              itemString = "静岡";
              break;
          case 28:
              itemString = "愛知";
              break;
          case 29:
              itemString = "三重";
              break;
          case 30:
              itemString = "滋賀";
              break;
          case 31:
              itemString = "京都";
              break;
          case 32:
              itemString = "大阪";
              break;
          case 33:
              itemString = "兵庫";
              break;
          case 34:
              itemString = "奈良";
              break;
          case 35:
              itemString = "和歌山";
              break;
          case 36:
              itemString = "鳥取";
              break;
          case 37:
              itemString = "島根";
              break;
          case 38:
              itemString = "岡山";
              break;
          case 39:
              itemString = "広島";
              break;
          case 40:
              itemString = "山口";
              break;
          case 41:
              itemString = "徳島";
              break;
          case 42:
              itemString = "香川";
              break;
          case 43:
              itemString = "愛媛";
              break;
          case 44:
              itemString = "高知";
              break;
          case 45:
              itemString = "福岡（福岡）";
              break;
          case 46:
              itemString = "福岡（北九州）";
              break;
          case 47:
              itemString = "佐賀";
              break;
          case 48:
              itemString = "長崎";
              break;
          case 49:
              itemString = "熊本";
              break;
          case 50:
              itemString = "大分";
              break;
          case 51:
              itemString = "宮崎";
              break;
          case 52:
              itemString = "鹿児島";
              break;
          case 53:
              itemString = "沖縄";
              break;
          default:
              itemString = "";
              break;
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
        selectedItemId = -1;
    }
    
    /**
     *  選択中のアイテム数を応答する
     * 
     *  @return 選択中のアイテム数
     */
    public int getNumberOfSelectedItems()
    {
        if (selectedItemId == -1)
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
        return (itemNumber);
    }

    /**
     *  選択中ののアイテムIDを応答する
     *  
     *  @param itemNumber 選択中のアイテム番号(ゼロスタート)
     *  @return アイテムID
     */
    private int getSelectedItemLocal(int itemNumber)
    {
      int item = -1;
      switch (itemNumber)
      {
      case 0:
        item = 1;
        break;
      case 1:
        item = 52;
        break;
      case 2:
        item = 48;
        break;
      case 3:
        item = 50;
        break;
      case 4:
        item = 51;
        break;
      case 5:
        item = 49;
        break;
      case 6:
        item = 53;
        break;
      case 7:
        item = 2;
        break;
      case 8:
        item = 3;
        break;
      case 9:
        item = 4;
        break;
      case 10:
        item = 5;
        break;
      case 11:
        item = 6;
        break;
      case 12:
        item = 7;
        break;
      case 13:
        item = 8;
        break;
      case 14:
        item = 9;
        break;
      case 15:
        item = 10;
        break;
      case 16:
        item = 11;
        break;
      case 17:
        item = 12;
        break;
      case 18:
        item = 13;
        break;
      case 19:
        item = 14;
        break;
      case 20:
        item = 15;
        break;
      case 21:
        item = 16;
        break;
      case 22:
        item = 17;
        break;
      case 23:
        item = 18;
        break;
      case 24:
        item = 19;
        break;
      case 25:
        item = 20;
        break;
      case 26:
        item = 21;
        break;
      case 27:
        item = 22;
        break;
      case 28:
        item = 23;
        break;
      case 29:
        item = 24;
        break;
      case 30:
        item = 25;
        break;
      case 31:
        item = 26;
        break;
      case 32:
        item = 27;
        break;
      case 33:
        item = 28;
        break;
      case 34:
        item = 29;
        break;
      case 35:
        item = 30;
        break;
      case 36:
        item = 31;
        break;
      case 37:
        item = 32;
        break;
      case 38:
        item = 33;
        break;
      case 39:
        item = 34;
        break;
      case 40:
        item = 35;
        break;
      case 41:
        item = 36;
        break;
      case 42:
        item = 37;
        break;
      case 43:
        item = 38;
        break;
      case 44:
        item = 39;
        break;
      case 45:
        item = 40;
        break;
      case 46:
        item = 54;
        break;
      case 47:
        item = 41;
        break;
      case 48:
        item = 42;
        break;
      case 49:
        item = 43;
        break;
      case 50:
        item = 44;
        break;
      case 51:
        item = 45;
        break;
      case 52:
        item = 46;
        break;
      case 53:
        item = 47;
        break;
      default:
        break;
      }
      return (item);
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
     *  選択が確定されたときの処理
     *  
     * @param itemNumber アイテム番号
     */
    private void decideSelectedItem(int itemNumber)
    {
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // メニュー選択
            // ここでは何もしない
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // テレビ局選択
            storageDb.setTvStation(itemNumber);
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // 電波種別選択
            storageDb.setTvType(itemNumber);
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // 地域選択
            storageDb.setAreaId(getSelectedItemLocal(itemNumber), getLocalMenu(itemNumber));
            break;

          default:
            break;
        }
        
        return;
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
            // アイテムが選択された
            decideSelectedItem(selectedItemId);
            isConfirmation = true;
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
        return ("");
    }

    /**
     *  タイトルを表示するかどうか？
     *  
     *  @return タイトル表示する(true) / 表示しない(false)
     */
    public boolean isShowTitle()
    {
        if(scene == TVgSceneDb.SCENE_MENUSELECTION)
        {
            // メニュー選択時には、タイトルを表示しない
            return (false);
        }
        return (true);
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
        if (scene == TVgSceneDb.SCENE_MENUSELECTION)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  ボタンのラベルを取得する
     *  
     *  @param buttonId ボタンのID
     *  @return 表示するボタンのラベル
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("OK");
    }

    /**
     *  背景を互い違いの色で表示するかを取得する
     *
     *  @return 互い違いの色にする(true) / 背景は１色(false)
     */
    public boolean getSelectionBackColorMode()
    {
        if(scene == TVgSceneDb.SCENE_MENUSELECTION)
        {
            // メニュー選択時には、タイトルを表示しない
            return (false);
        }
        return (true);
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
        int nextId = ISelectionScreenStorage.SCENE_TO_PREVIOUS;
        switch (scene)
        {
          case TVgSceneDb.SCENE_MENUSELECTION: // メニュー選択
          case TVgSceneDb.SCREEN_MENUSELECT:
            nextId = decideNextScreen(selectedItemId);
            break;

          case TVgSceneDb.SCENE_STATIONSELECTION: // テレビ局選択
            nextId = TVgSceneDb.SCENE_PROGRAMLIST;
            break;

          case TVgSceneDb.SCENE_TVTYPESELECTION: // 電波種別選択
            nextId = TVgSceneDb.SCENE_DATELIST;
            break;

          case TVgSceneDb.SCENE_LOCALESELECTION: // 地域選択
            nextId = TVgSceneDb.SCENE_DATELIST;
            break;
            
          case TVgSceneDb.SCENE_DIRSELECTION:    // ディレクトリ選択
          case TVgSceneDb.SCREEN_DIRSELECT:
            nextId = TVgSceneDb.SCENE_DATELIST;
            break;

          case TVgSceneDb.SCENE_WELCOME:         // ようこそ画面
            nextId = TVgSceneDb.SCENE_DIRSELECTION;
            break;

          default:
            break;
        }
        isConfirmation = false;
        return (nextId);
    }
    
    /**
     *  メニュー画面から抜けたとき、どの画面に遷移するか決める
     * 
     * @param itemNumber 選択した番号
     * @return 次画面ID
     */
    private int decideNextScreen(int itemNumber)
    {
        int nextId = ISelectionScreenStorage.SCENE_TO_PREVIOUS;
        if(isConfirmation == false)
        {
            // 前画面に戻る
            return (nextId);
        }
        if (itemNumber == 0)
        {
            // 地域選択
            nextId = TVgSceneDb.SCENE_LOCALESELECTION;
        }
        else if (itemNumber == 1)
        {
            // 局種別選択
            nextId = TVgSceneDb.SCENE_TVTYPESELECTION;
        }
        else if (itemNumber == 2)
        {
            // 記録ディレクトリ指定
            nextId = TVgSceneDb.SCENE_DIRSELECTION;
        }
        return (nextId);
    }

    /**
     *  初期ディレクトリを応答する
     *  
     *  @return 初期ディレクトリ
     */
    public String getDefaultDirectory()
    {
        return (null);
    }

    /**
     *  現在選択されているファイルおよびディレクトリの数を応答する
     *  
     *  @return 現在選択されているファイル(ディレクトリ)数
     */
    public int getSelectedNameCount()
    {
        if (selectedFileName == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  選択（ファイル）名を応答する
     * 
     *  @param index 選択番号(0スタート)
     *  @return 選択番号のファイル名
     */
    public String getSelectedName(int index)
    {
        if (selectedFileName == null)
        {
            return ("");
        }
        return (selectedFileName);
    }

    /**
     *  選択（ファイル）名を設定する
     * 
     * @param name 選択したファイルの名前
     */
    public void setSelectedName(String name)
    {
        selectedFileName = name;
    }

    /**
     *  選択（ファイル）名をクリアする
     *  
     */
    public void clearSelectedName()
    {
        selectedFileName = null;
    }
    
    /**
     *  選択（ファイル）名を確定する
     *  
     */
    public void confirmSelectedName()
    {
        if (selectedFileName != null)
        {
            storageDb.setBaseDirectory(selectedFileName);
        }
        selectedFileName = null;
    }
}
