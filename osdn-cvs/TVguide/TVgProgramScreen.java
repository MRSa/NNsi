import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

public class TVgProgramScreen extends MidpScreenCanvas
{
    private int foreColorForInf = 0x00000000;
    private int foreColorForPgm = 0x00000000;
    private int foreColorForStn = 0x000800c0;
    private int backColor000    = 0x00e0dfe3;
    private int backColor001    = 0x00e8f2fe;
    private int backColor002    = 0x00f8f8f8;
    private int startHour       = 0;
    private int startMinutes    = 0;
    private TVgProgramDataParser dataParser = null;
    private String appendMessage = null;
    private boolean isStationMode = false;    
    
    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public TVgProgramScreen(int screenId, MidpSceneSelector object, TVgProgramDataParser storage)
    {
        super(screenId, object);
        dataParser = storage;
        appendMessage = "";
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     * 
     */
    public void start()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                dataParser.prepareDataToShow();
            };
        };
        try
        {
            thread.start();
        }
        catch (Exception ex)
        {
            //
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
        
        // タイトルと画面全体のクリアは親（継承元）の機能を使う
        super.paint(g);
        
        try
        {
            // フォントを取得する
            int fontHeight = screenFont.getHeight() + 1; 

            ///// 番組一覧を表示する /////
            if (dataParser.isDataFileIsReady() == false)
            {
                g.setColor(foreColorForInf);
                g.drawString("通信中...お待ちください" + appendMessage, (screenTopX + 5), (screenTopY + (fontHeight * 4)), (Graphics.LEFT | Graphics.TOP));
            }
            else 
            {
                // 番組データを表示する
                if (isStationMode == false)
                {
                    // 時刻ごと表示
                    drawProgramDataHour(g);
                }
                else
                {
                    // テレビ局ごと表示
                    drawProgramData(g);
                }
                if (dataParser.isReadyToConstruct() == false)
                {
                    // データ解析中の表示...
                    g.setColor(backColor000);
                    g.fillRect(screenTopX, (screenTopY + screenHeight - fontHeight - 1), screenWidth, fontHeight + 2);
                    g.setColor(foreColorForInf);
                    g.drawString("解析中... " + appendMessage, (screenTopX + 10), (screenTopY + screenHeight - fontHeight), (Graphics.LEFT | Graphics.TOP));
                }
            }
        }
        catch (Exception ex)
        {
            //
        }
        return;
    }
    
    /**
     *   番組表の表示 (時刻単位)
     * 
     * @param g
     */
    private void drawProgramDataHour(Graphics g)
    {
        // タイトルを更新する
        updateTitleString();

        int nofStations = dataParser.getNumberOfTvStations();
        int fontHeight = screenFont.getHeight() + 1; 
        int drawLine = 1;
        for (int lp = 0; lp < nofStations; lp++)
        {
            // 背景色を設定する
            if ((lp % 2) == 0)
            {
                g.setColor(backColor001);
            }
            else
            {
                   g.setColor(backColor002);
            }
            g.fillRect(screenTopX, (screenTopY + fontHeight * drawLine), (screenTopX + screenWidth), (fontHeight * 2));

            // テレビ局名を表示する
            g.setColor(foreColorForStn);
            String station = dataParser.getTvStationName(lp);
            g.drawString(station, (screenTopX + 2), (screenTopY + fontHeight * drawLine), (Graphics.LEFT | Graphics.TOP));
            drawLine++;        

            // テレビ番組を表示する
            g.setColor(foreColorForPgm);
            String program = dataParser.getProgramDataFromMinutes(lp, ((startHour * 60) + startMinutes), true);
            g.drawString(program, (screenTopX + 10), (screenTopY + fontHeight * drawLine), (Graphics.LEFT | Graphics.TOP));
            drawLine++;
        }
    }


    /**
     *   番組表の表示 (テレビ局ごと表示)
     * @param g
     */
    private void drawProgramData(Graphics g)
    {
        int fontHeight = screenFont.getHeight() + 1; 
        int lines = screenHeight / fontHeight;

        updateTitleString();

        int drawLine = 1;
        int stationId = dataParser.getTvStationIndex();
        int minute    = ((startHour * 60) + startMinutes);
        String data  = dataParser.getProgramDataFromMinutes(stationId, minute, false);
        int index    = dataParser.getProgramIndex(stationId, minute);
        int count = 0;
        while (drawLine < lines)
        {
            count++;
            if ((count % 2) == 1)
            {
                g.setColor(backColor001);
            }
            else
            {
                   g.setColor(backColor002);
            }
            g.fillRect((screenTopX + 3), (screenTopY + fontHeight * drawLine), (screenTopX + screenWidth), (fontHeight * (lines - drawLine)));
            drawLine = drawLine + drawString(g, drawLine, 3, screenFont.stringWidth("00:00 "), data);
            index++;
            data = dataParser.getProgramDataFromIndex(stationId, index, false);
        }
    }

    /**
     *   表示タイトルを更新する
     */
    private void updateTitleString()
    {
        if (isStationMode == false)
        {
        	titleForeColor = foreColorForPgm;
            titleString = dataParser.getTvStationTitle();
        }
        else
        {
        	titleForeColor = foreColorForStn;
            titleString = dataParser.getTvStationString();
        }
        String append = "";
        if (startMinutes < 10)
        {
            append = "0";
        }
        titleSubString = " (" + dataParser.getDataDate() + " " + ((startHour + 5) % 24) + ":" + append + startMinutes + "-" + ")";
    }

    /**
     *  メニューボタンが押されたとき：放送局選択画面へ遷移する
     * 
     */
    public void showMenu()
    {
        isStationMode = true;
        parent.changeScene(TVgSceneDb.SCENE_STATIONSELECTION, "放送局選択");
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
        try
        {
            appendMessage = message;
            updateTitleString();
        }
        catch (Exception ex)
        {
            // 
        }
        return;        
    }

    /**
     * 中ボタンが押されたとき：スレ一覧画面へ遷移する
     * 
     */
    public boolean inputFire(boolean isRepeat)
    {
        if (isStationMode == true)
        {
            isStationMode = false;
        }
        else
        {
            isStationMode = true;
        }
        updateTitleString();
        return (true);
    }

    /**
     * 上ボタンが押されたとき：15分 or 60分減らす
     */
    public boolean inputUp(boolean isRepeat)
    {
        if (isStationMode == false)
        {
            startMinutes = startMinutes - 15;
            if (startMinutes < 0)
            {
                startMinutes = 45;
                startHour--;
                if (startHour < 0)
                {
                    startHour = 0;
                }
            }
        }
        else
        {
            startMinutes = startMinutes - 60;
            if (startMinutes < 0)
            {
                startMinutes = 0;
                startHour--;
                if (startHour < 0)
                {
                    startHour = 0;
                }
            }
        }
        updateTitleString();
        return (true);
    }

    /**
     *  左ボタンが押されたとき：1時間戻す or 前の局選択
     */
    public boolean inputLeft(boolean isRepeat)
    {
        if (isStationMode == false)
        {
        	// 表示データを一時間戻す
        	startHour--;
            if (startHour < 0)
            {
                startHour = 0;
                startMinutes = 0;
            }
        }
        else
        {
        	// 表示局を変更する
        	dataParser.changeTvStationIndex(-1);
        }
        updateTitleString();
        return (true);
    }

    /**
     *  右ボタンが押されたとき：1時間進める or 次の局を選択する
     */
    public boolean inputRight(boolean isRepeat)
    {
        if (isStationMode == false)
        {
            // 表示データを一時間進める
        	startHour++;
            if (startHour >= 24)
            {
                startHour = 23;
                startMinutes = 50;
            }
        }
        else
        {
        	// 表示局を変更する
        	dataParser.changeTvStationIndex(1);
        }
        updateTitleString();
        return (true);
    }    

    /**
     * 下ボタンが押されたとき：15分加える or 1時間加える
     */
    public boolean inputDown(boolean isRepeat)
    {
        if (isStationMode == false)
        {
            // 一覧表示モードのときは、15分すすめる
        	startMinutes = startMinutes + 15;
        }
        else
        {
            // 局選択モードのときは、1時間すすめる
            startMinutes = startMinutes + 60;
        }
        if (startMinutes >= 60)
        {
            startMinutes = 0;
            startHour++;
            if (startHour >= 24)
            {
                startHour = 23;
                startMinutes = 50;
            }
        }
        updateTitleString();
        return (true);
    }

    /**
     *  左ボタンが押されたとき：4時間戻す
     */
    public boolean inputGameA(boolean isRepeat)
    {
        startHour = startHour - 4;
        if (startHour < 0)
        {
            startHour = 0;
            startMinutes = 0;
        }
        updateTitleString();
        return (true);
    }

    /**
     *  右ボタンが押されたとき：4時間進める
     */
    public boolean inputGameB(boolean isRepeat)
    {
        startHour = startHour + 4;
        if (startHour >= 24)
        {
            startHour = 23;
            startMinutes = 50;
        }
        updateTitleString();
        return (true);
    }    

    /**
     * GAME Cボタンが押されたとき：日付選択画面に戻る
     * 
     */
    public boolean inputGameC(boolean isRepeat)
    {
        if ((dataParser.isReadyToConstruct() == false)||(dataParser.isDataFileIsReady() == false))
        {
            // データ取得中 or データ解析中のときには、日付選択画面には戻れないようにする。
            return (false);
        }
        parent.changeScene(TVgSceneDb.SCENE_DATELIST, "日付一覧");
        return (true);
    }

    /**
     * GAME Dボタンが押されたとき：表示切替（ＴＶ局⇒時刻、時刻⇒ＴＶ局）
     * 
     */
    public boolean inputGameD(boolean isRepeat)
    {
        if (isStationMode == true)
        {
            isStationMode = false;
        }
        else
        {
            isStationMode = true;
        }
        updateTitleString();
        return (true);
    }
    /**
     *  数字キーが入力されたときの処理
     *  @param number 入力された数字
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     *
     */
    public boolean inputNumber(int number, boolean isRepeat)
    {
        if (number == 0)
        {
            int min = dataParser.getCurrentTime();
            startHour    = (min / 60);
            startMinutes = (min % 60);
            updateTitleString();
            return (true);
        }
        return (false);
    }

    /**
     *  ＃キー操作処理：末尾へ移動
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputPound(boolean isRepeat)
    {
        startHour = 23;
        startMinutes = 0;
        updateTitleString();
        return (true);
    }

    /**
     *  ＊キー操作処理：先頭へ移動
     *  @param isRepeat キーリピート入力かどうか（true:リピート入力）
     *  @return このメソッドで処理した(true) / このメソッドでは処理をしていない(false)
     */
    public boolean inputStar(boolean isRepeat)
    {
        startHour = 0;
        startMinutes = 0;
        updateTitleString();
        return (true);
    }
}
