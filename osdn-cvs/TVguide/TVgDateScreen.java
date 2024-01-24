import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

public class TVgDateScreen extends MidpScreenCanvas
{
    
    private final int MAX_DAY = 5;
    private int foreColorForOpe = 0x00000000;
//    private int backColorForOpe = 0x00f0f0ff;
    
    private TVgProgramDataParser dataParser = null;
    
    private int currentSelectionItem = 1;  
        
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public TVgDateScreen(int screenId, MidpSceneSelector object, TVgProgramDataParser storage)
    {
        super(screenId, object);
        dataParser = storage;
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        if (dataParser.start() == false)
        {
            parent.changeScene(TVgSceneDb.SCENE_WELCOME, "TV番組表について");
        }
        return;
    }

    /**
     * 画面を表示
     * 
     */
    public void paint(Graphics g)
    {
        if (g == null)
        {
            return;
        }     
        
        try
        {
            // 画面タイトルを更新する (地域名と種別を表示する)
            titleString = dataParser.getTvStationTitle();

            // タイトルと画面全体のクリアは親（継承元）の機能を使う
            super.paint(g);
            
            ///// 一覧を表示する /////
            drawDateList(g);
            
         }
        catch (Exception ex)
        {
            //
        }
        return;
    }

    /**
     *   日付一覧を表示する
     * @param g グラフィック
     */
    private void drawDateList(Graphics g)
    {
        // フォントを取得する
        int fontHeight = screenFont.getHeight() + 1;
        int fontWidth = screenFont.stringWidth("⇒ ") + 5;

        // 昨日から 5日分の情報を一覧表示する
        for (int lp = -1; lp < MAX_DAY; lp++)
        {
            g.setColor(foreColorForOpe);
            g.drawString(dataParser.getDateDataString(lp), (screenTopX + fontWidth), (screenTopY + (fontHeight * 2) + (fontHeight * (lp + 1))), (Graphics.LEFT | Graphics.TOP));
        }

        // カーソルを表示する
        g.setColor(foreColorForOpe);
        g.drawString("⇒", (screenTopX + 5), (screenTopY + (fontHeight * 2) + (fontHeight * currentSelectionItem)), (Graphics.LEFT | Graphics.TOP));
   }

    /**
     *  メニューボタンが押されたとき：検索文字列入力画面へ遷移する
     * 
     */
    public void showMenu()
    {
        parent.changeScene(TVgSceneDb.SCENE_MENUSELECTION, "選択メニュー");
        return;
    }

    /**
     *  アプリケーションを終了させるか？
     * 
     *  @return  true : 終了させる、false : 終了させない
     */
    public boolean checkExit()
    {
        // 終了したシーンを記録する
        return (true);
    }

    /**
     * 情報領域の情報を更新する
     * @param message 表示するメッセージ
     * @param mode 表示モード
     * 
     */
    public void updateInformation(String message, int mode)
    {
        // TODO : 実装すべし！
        return;        
    }

    /**
     * 中ボタンが押されたとき
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.changeScene(TVgSceneDb.SCENE_PROGRAMLIST, "番組表");
                dataParser.selectedDateData(currentSelectionItem - 1, false);
            }
        };
        thread.start();
        return (true);
    }

    /**
     *  上キーが押されたときの処理<br>
     *  前のアイテムを選択します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
        currentSelectionItem--;
        if (currentSelectionItem < 0)
        {
            currentSelectionItem = MAX_DAY;
        }
        return (true);
    }

    /**
     *  下キーが押されたときの処理<br>
     *  次のアイテムを選択します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputDown (boolean isRepeat)
    {
        currentSelectionItem++;
        if (currentSelectionItem > (MAX_DAY))
        {
            currentSelectionItem = 0;
        }
        return (true);
    }

    /**
     *  左キーが押されたときの処理<br>
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputLeft (boolean isRepeat)
    {
        return (true);
    }

    /**
     *  右キーが押されたときの処理<br>
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
        return (true);
    }

    /**
     * GAME Cボタンが押されたとき：日付選択画面に戻る
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
/*
        if (lockOperation == true)
        {
            return (true);
        }
        String fileName = storageDb.getDefaultDirectory() + "temporary.html";
        parent.platformRequest(fileName, true);
*/
        return (true);
    }

    /**
     * GAME Dボタンが押されたとき：データを取得する
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                parent.changeScene(TVgSceneDb.SCENE_PROGRAMLIST, "番組表");
                dataParser.selectedDateData(currentSelectionItem - 1, true);
            }
        };
        thread.start();
        return (true);
    }
}
