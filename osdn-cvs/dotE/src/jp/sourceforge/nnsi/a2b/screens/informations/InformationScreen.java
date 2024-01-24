/**
 *  情報表示画面用のパッケージです。
 */
package jp.sourceforge.nnsi.a2b.screens.informations;
import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 * メッセージ表示用の画面<br>
 * 画面にメッセージを表示する画面です。
 * 右側にスクロールバーを表示し、スクロールして表示することもできます。
 * 画面下部に複数のボタンがもてたりもします。<br>
 * (画面を閉じるときに選択されたボタンを通知します。<br>
 * そのため、確認ダイアログとしても利用可能です。
 *
 * @author MRSa
 *
 */
public class InformationScreen extends MidpScreenCanvas
{
    private final int AREAWIDTH_MARGIN       = 5;
	private final int AREAHEIGHT_MARGIN      = 3;
	private final int selectBackColor        = 0x000000ff;
	private final int selectForeColor        = 0x00ffffff;
	private final int unselectBackColor      = 0x00ffffff;
	private final int unselectForeColor      = 0x00000000;
	private final int buttonBorderColor      = 0x00000000;
	
    private InformationScreenOperator operation = null;  // 操作クラス...

    private int nofButton = 0;
    private int selectedButtonId = 0;  // デフォルトはOK
    private int topItem = 0;
    private int maxLine = 0;

    /**
     * コンストラクタでは、必要なオブジェクトを記憶します
     * @param screenId 画面ID
     * @param object シーンセレクタ
     * @param operator データ操作クラス
     */
    public InformationScreen(int screenId, MidpSceneSelector object, InformationScreenOperator operator)
    {
        super(screenId, object);
        operation = operator;
    }

    /**
     *  データ表示を開始します
     * 
     */
    public void start()
    {
        // 表示するメッセージを加工する
    	int btnMargin = screenFont.getHeight() + (screenFont.getHeight() / 8);
    	operation.formatMessage(screenWidth, screenHeight - btnMargin, screenFont, true);
    }

    /**
     * データ表示画面を終了します
     * 
     */
    public void stop()
    {
        //
    }
    /**
     * 画面描画処理をおこないます。
     * @param g グラフィックス
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
            // タイトル領域の描画
            drawTitleArea(g);

            // ボタン数を更新する
            nofButton = operation.getNumberOfButtons(canvasId);
            
            int topY = screenFont.getHeight() + screenTopY;
            int height = screenHeight - screenFont.getHeight();
            if (nofButton != 0)
            {
                // メインパネルの描画
                int buttonAreaY = screenFont.getHeight() + screenFont.getHeight() / 8 + AREAHEIGHT_MARGIN * 2;
                drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, height, buttonAreaY);

                // スクロールマーカーの表示
                drawScrollMarker(g, screenTopX, topY, screenWidth, height - buttonAreaY);
                
                // ボタン領域の描画
                drawButtonArea(g, screenTopX, screenTopY + screenHeight - buttonAreaY, screenWidth);
            }
            else
            {
                // メインパネルの描画
                drawMainPanel(g, screenTopX, topY, screenWidth - AREAWIDTH_MARGIN, height, 0);

                // スクロールマーカーの表示
                drawScrollMarker(g, screenTopX, topY, screenWidth, height);
            }
        }
        catch (Exception e)
        {
            // なぜかヌルポが出るので...
        }
        return;
    }

    /**
     * メインパネル領域に描画する
     * 
     * @param g       グラフィックスオブジェクト
     * @param topX    左上座標(X軸)
     * @param topY    左上座標(Y軸)
     * @param width   ウィンドウ幅
     * @param height  ウィンドウ高さ
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height, int buttonAreaHeight)
    {
        // 背景を消去する
//        clearMainPanel(g, topX, topY, width + AREAWIDTH_MARGIN, height);
        clearMainPanel(g, topX, topY, width, height);

        // フォントサイズ
        int fontHeight = screenFont.getHeight();

        // 選択肢のライン数を設定する
        maxLine = (height - buttonAreaHeight) / (fontHeight + 1) + 1;

        // 行ごとに表示する...
        int maxCount  = operation.getMessageLine();
        g.setColor(unselectForeColor);
        for (int lines = 0; lines < maxLine; lines++)
        {
            String lineString = operation.getMessage(lines + topItem);
            if (lineString != null)
            {
                g.drawString(lineString, (topX + InformationScreenOperator.WIDTH_MARGIN), (topY + (fontHeight + InformationScreenOperator.HEIGHT_MARGIN) * lines), (Graphics.LEFT | Graphics.TOP));
            }
            if ((lines + topItem) >= (maxCount - 1))
            {
            	break;
            }
        }
        return;
    }

    /**
     * ボタン領域を描画する
     * 
     * @param g       グラフィックスオブジェクト
     * @param topX    左上座標(X軸)
     * @param topY    左上座標(Y軸)
     * @param width   ウィンドウ幅
     * @param height  ウィンドウ高さ
     */
    private void drawButtonArea(Graphics g, int topX, int topY, int width)
    {
        // ボタン位置の設定...
        int btnWidth   = screenFont.stringWidth(" Cancel "); 
        int btnHeight   = screenFont.getHeight() + screenFont.getHeight() / 8;

        int btnX = topX + width - (btnWidth + InformationScreenOperator.WIDTH_MARGIN);
        for (int id = nofButton; id != 0; id--)
        {
            // 右側のボタンから順次描画する
            String label = operation.getButtonLabel(id - 1);
            if (isSelectedButton(id - 1) == true)
            {
                // ボタン選択中状態
                g.setColor(selectBackColor);
                g.fillRoundRect(btnX, (topY + AREAHEIGHT_MARGIN), btnWidth, btnHeight, 4, 4);
                g.setColor(buttonBorderColor);
                g.drawRoundRect(btnX, (topY + AREAHEIGHT_MARGIN), btnWidth, btnHeight , 4, 4);
                g.setColor(selectForeColor);
                g.drawString(label, btnX + (btnWidth / 2), topY + 5, (Graphics.HCENTER | Graphics.TOP));        	
            }
            else
            {
                // ボタン非選択状態
                g.setColor(buttonBorderColor);
                g.drawRoundRect(btnX, (topY + AREAHEIGHT_MARGIN), btnWidth, btnHeight , 4, 4);
                g.drawString(label, btnX + (btnWidth / 2), topY + 5, (Graphics.HCENTER | Graphics.TOP));        	
            }
            btnX = btnX - (btnWidth + InformationScreenOperator.WIDTH_MARGIN);
            if (btnX < btnWidth)
            {
                return;
            }
        }
        return;
    }
 
    /**
     * スクロールマーカーを描画する
     * 
     * @param g       グラフィックスオブジェクト
     * @param topX    左上座標(X軸)
     * @param topY    左上座標(Y軸)
     * @param width   ウィンドウ幅
     * @param height  ウィンドウ高さ
     */
    private void drawScrollMarker(Graphics g, int topX, int topY, int width, int height)
    {
        int line = operation.getMessageLine();
    	if (maxLine >= line)
        {
            // データは一画面におさまる量、、描画せず終了する
            return;
        }

        // ラインを描画する
        int w = topX + width - (AREAWIDTH_MARGIN - 2);
        g.setColor(unselectBackColor);
        g.fillRect(w, topY, (AREAWIDTH_MARGIN - 2), height);

        // スクロールマークを表示する
        g.setColor(buttonBorderColor);
        g.drawLine(w, topY, w, topY + height);
        int par = 0;
        if (topItem != 0)
        {
        	par = (line * height) / topItem;
        }
        int start = par - 4;
        if (start < topY)
        {
        	start = topY;
        }
        else if (start >= topY + height)
        {
            start = (topY + height - 6);
        }
        g.fillRect(w, start, (AREAWIDTH_MARGIN - 2), 6);
        
        return;
    }

    /**
     * ボタンが選択されているかどうかチェックする
     * 
     * @param buttonId 選択されているかどうかチェックするボタンID
     * @return 選択されている(true) / 選択されていない(false)
     */
    public boolean isSelectedButton(int buttonId)
    {
        if (selectedButtonId == buttonId)
        {
            return (true);
        }
    	return (false);
    }

    /**
     *  メニュキーが押されたときの処理<br>
     *  選択中のボタンを右のものに切り替えます。
     */
    public void showMenu()
    {
		selectedButtonId++;
    	if (selectedButtonId == nofButton)
    	{
    	    selectedButtonId = 0;
    	}
        return;
    }

    /**
     *  左キーが押されたときの処理<br>
     *  選択中のボタンを左のものに切り替えます。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputLeft(boolean isRepeat)
    {
		selectedButtonId--;
    	if (selectedButtonId < 0)
    	{
    	    selectedButtonId = nofButton - 1;
    	}
        return (true);
    }

    /**
     *  右キーが押されたときの処理<br>
     *  選択中のボタンを右のものに切り替えます。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputRight(boolean isRepeat)
    {
		selectedButtonId++;
    	if (selectedButtonId == nofButton)
    	{
    	    selectedButtonId = 0;
    	}
        return (true);
    }

    /**
     *  上キーが押されたときの処理<br>
     *  前ページを表示します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputUp (boolean isRepeat)
    {
        topItem = topItem - (maxLine - 1);
        if (topItem < 0)
        {
            topItem = 0;
        }
        return (true);
    }

    /**
     *  下キーが押されたときの処理<br>
     *  次ページを表示します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputDown(boolean isRepeat)
	{
        topItem = topItem + (maxLine - 1);
        if (topItem >= operation.getMessageLine())
        {
            // 次ページを表示しない （元に戻す）
        	topItem = topItem - (maxLine - 1);
//        	topItem = operation.getMessageLine() - 1;
        }
        return (true);
    }

	/**
     *  センターキーが押されたときの処理<br>
     *  ボタンを押したこととして、画面を切り替えます。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputFire (boolean isRepeat)
    {
        operation.finishScreen(selectedButtonId);
        finishCanvas();
        return (true);
    }

	/**
     *  GAME Cキー(クリアキー)が押されたときの処理<br>
     *  キャンセルボタンを押したこととして、画面を切り替えます。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputGameC(boolean isRepeat)
    {
        operation.finishScreen(IInformationScreenStorage.BUTTON_CANCELED);
        finishCanvas();
        return (true);
    }

	/**
	 *   次の画面へ切り替える
	 *   （どの画面に切り替えるかは操作ユーティリティに問い合わせて決める）
	 * 
	 */
	protected void finishCanvas()
	{
		int nextScene = operation.nextSceneToChange(canvasId);
		if (nextScene < 0)
		{
		    parent.previousScene();
		}
		else
		{
			parent.changeScene(nextScene, null);
		}
        return;
	}
}
