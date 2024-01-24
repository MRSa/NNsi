package jp.sourceforge.nnsi.a2b.screens.selections;
import javax.microedition.lcdui.Graphics;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 * アイテム選択用の画面<br>
 * {@link jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas}の差分として
 * 作成しています。
 *
 * @see jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas
 *
 * @author MRSa
 *
 */
public class SelectionScreen extends MidpScreenCanvas
{
    private final int SELECTION_JUMPCOUNT    = 3;
	private final int AREAHEIGHT_MARGIN      = 3;
	private final int buttonLineForeColor    = 0x00202020;
	private final int buttonBorderColor      = 0x00000000;
	private final int selectBackColor        = 0x000000ff;
	private final int selectForeColor        = 0x00ffffff;
	private final int unselectForeColor      = 0x00000000;
    private final int upDownPageForeColor    = 0x00008f00;
	//	private final int unselectBackColor      = 0x00ffffff;

    private SelectionScreenOperator operation = null;  // 操作クラス...
    private int buttonSelection = 0;
    private int selectedButtonId = 0;
    private int topItem = 0;
    private int maxLine = 0;
    private int nofButton = 0;

    /**
     * コンストラクタでは、画面のIDや必要なユーティリティを設定します
     * @param screenId 画面ID
     * @param object シーンセレクタ
     * @param operator 選択画面用操作ユーティリティ
     */
    public SelectionScreen(int screenId, MidpSceneSelector object, SelectionScreenOperator operator)
    {
        super(screenId, object);
        operation = operator;
    }

    /**
     *  データ表示の開始
     * 
     */
    public void start()
    {
        // アイテムを初期化
        if (operation.isItemInitialize() == true)
        {
            selectedButtonId = 0;
    	    topItem = 0;
            buttonSelection = 0;
        }
    }

    /**
     * データ表示の終了
     * 
     */
    public void stop()
    {
        // 
    }

    /**
     * 画面描画の実施
     * 
     * @param g グラフィック
     * 
     */
    public void paint(Graphics g)
    {
        try
        {        	
        	// タイトル領域の描画
            int titleAreaHeight = screenFont.getHeight();
            if (operation.isShowTitle() == true)
            {
                drawTitleArea(g);
            }
            else
            {
            	titleAreaHeight = 0;
            }

            // ボタン数を更新する
            nofButton = operation.getNumberOfButtons();

            if (nofButton != 0)
            {
                // メインパネルの描画
                int buttonAreaY = screenFont.getHeight() + screenFont.getHeight() / 8 + AREAHEIGHT_MARGIN * 2;
                drawMainPanel(g, screenTopX, screenTopY + titleAreaHeight, screenWidth, screenHeight - titleAreaHeight, buttonAreaY);

                // ボタン領域の描画
                drawButtonArea(g, screenTopX, screenTopY + screenHeight - buttonAreaY, screenWidth);
            }
            else
            {
                // メインパネルの描画
                drawMainPanel(g, screenTopX, screenTopY + titleAreaHeight, screenWidth, screenHeight - titleAreaHeight, 0);
            }
        }
        catch (Exception e)
        {
            // なぜかヌルポが出るので...
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
        
        // ラインの表示
    	g.setColor(buttonLineForeColor);
        g.drawLine((topX + SelectionScreenOperator.WIDTH_MARGIN), (topY + 1), (topX + width - (SelectionScreenOperator.WIDTH_MARGIN)), (topY + 1));

        int btnX = topX + width - (btnWidth + SelectionScreenOperator.WIDTH_MARGIN);
        for (int id = nofButton; id != 0; id--)
        {
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
            btnX = btnX - (btnWidth + SelectionScreenOperator.WIDTH_MARGIN);
            if (btnX < topX)
            {
            	// ボタン表示エリアが不足した場合、終了する
                return;
            }
        }
        // 空きエリアがあればガイドメッセージを表示する
        String guide = operation.getGuideMessage();
        if (guide.length() == 0)
        {
            // ガイドメッセージがない場合、選択肢のアイテム数を表示する
        	guide = (buttonSelection + 1) + "/" + operation.getSelectionItemCount();
        }
        int area = btnX + btnWidth - (topX + SelectionScreenOperator.WIDTH_MARGIN);
        if (area > screenFont.stringWidth(guide))
        {
            g.setColor(buttonBorderColor);
            g.drawString(guide, topX + SelectionScreenOperator.WIDTH_MARGIN, topY + AREAHEIGHT_MARGIN + 2, (Graphics.LEFT | Graphics.TOP));
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
     * @param height  ウィンドウ高さ(ボタン領域高さを含む)
     * @param buttonAreaHeight ボタン領域高さ
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height, int buttonAreaHeight)
    {
        // 背景を消去する
        clearMainPanel(g, topX, topY, width, height);
        
        // フォントサイズ
        int fontHeight = screenFont.getHeight();

        // 選択肢のライン数を設定する
        maxLine = (height - buttonAreaHeight) / (fontHeight + 1) + 1;
        int maxCount = operation.getSelectionItemCount();
        g.setColor(0);
        for (int lines = 0; lines < maxLine; lines++)
        {
            String itemString = operation.getSelectionItem(lines + topItem);
            if (itemString.length() != 0)
            {
                if ((lines + topItem) == buttonSelection)
                {
            	    g.setColor(selectBackColor);
            	    g.fillRect(topX, (topY + (fontHeight * lines)), (width), (fontHeight + 1));
                    g.setColor(selectForeColor);
            	    g.drawString(itemString, (topX + SelectionScreenOperator.WIDTH_MARGIN), (topY + (fontHeight + SelectionScreenOperator.HEIGHT_MARGIN) * lines), (Graphics.LEFT | Graphics.TOP));
                }
                else
                {
                	g.setColor(operation.getSelectionBackColor(lines));
            	    g.fillRect(topX, (topY + (fontHeight * lines)), (width), (fontHeight + 1));
                    g.setColor(unselectForeColor);
            	    g.drawString(itemString, (topX + SelectionScreenOperator.WIDTH_MARGIN), (topY + (fontHeight + SelectionScreenOperator.HEIGHT_MARGIN) * lines), (Graphics.LEFT | Graphics.TOP));
                }
            }
            if ((lines + topItem) >= (maxCount - 1))
            {
            	break;
            }
        }

        if (topItem != 0)
        {
            // 選択肢がまだ上にある場合...
            g.setColor(upDownPageForeColor);
            g.drawString("▲", (topX + width - screenFont.stringWidth("▲ ")), (topY + SelectionScreenOperator.HEIGHT_MARGIN), (Graphics.LEFT | Graphics.TOP));
        }
        
        if ((topItem + maxLine) < maxCount)
        {
            // 選択肢がまだ下にある場合...
            g.setColor(upDownPageForeColor);
            g.drawString("▼", (topX + width - screenFont.stringWidth("▼ ")), (topY + (fontHeight + SelectionScreenOperator.HEIGHT_MARGIN) * (maxLine - 1)), (Graphics.LEFT | Graphics.TOP));
        }
        return;
    }

    /**
     * ボタンが選択されているかどうかチェックします
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
     *  上キーが押されたときの処理<br>
     *  前のアイテムを選択します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
    public boolean inputUp   (boolean isRepeat)
    {
        if (buttonSelection > 0)
        {
            buttonSelection--;            
        }
        if (topItem > buttonSelection)
        {
            topItem = topItem - (maxLine - 1);
            if (topItem < 0)
            {
            	topItem = 0;
            }
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
        if (buttonSelection < (operation.getSelectionItemCount() - 1))
        {
            buttonSelection++;
        }
        if ((topItem + maxLine) <= buttonSelection)
        {
            topItem = topItem + (maxLine - 1);
            if (topItem >= operation.getSelectionItemCount())
            {
            	topItem = operation.getSelectionItemCount() - 1;
            }
        }
        return (true);
    }

    /**
     *  左キーが押されたときの処理<br>
     *  前ページを表示します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputLeft (boolean isRepeat)
    {
        if (buttonSelection > 0)
        {
            buttonSelection = buttonSelection - (maxLine - 1);
            if (buttonSelection < 0)
            {
            	buttonSelection = 0;
            }
        }
        if (topItem > buttonSelection)
        {
            topItem = topItem - (maxLine - 1);
            if (topItem < 0)
            {
            	topItem = 0;
            }
        }
        return (true);
    }

    /**
     *  右キーが押されたときの処理<br>
     *  次ページを表示します。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputRight(boolean isRepeat)
    {
        if (buttonSelection < (operation.getSelectionItemCount() - 1))
        {
            buttonSelection = buttonSelection + (maxLine - 1);
            if (buttonSelection > (operation.getSelectionItemCount() - 1))
            {
            	buttonSelection = operation.getSelectionItemCount() - 1;
            }
        }
        if ((topItem + maxLine) <= buttonSelection)
        {
            topItem = topItem + (maxLine - 1);
            if (topItem >= operation.getSelectionItemCount())
            {
            	topItem = operation.getSelectionItemCount() - 1;
            }
        }
        return (true);
    }

	/**
     *  メニュー表示が指示されたときの処理<br>
     *  （選択中のボタンを変更します）
     */
    public void showMenu()
    {
        if (nofButton == 1)
        {
            // 選択したアイテムを通知する
        	operation.selectedItem(buttonSelection);

            // ボタンが押されたことを通知する
        	if (operation.triggeredMenu() == true)
        	{
        		finishCanvas();
        	}
            return;
        }
        if (nofButton == 0)
        {
        	// ボタンが存在しないときは、ただクローズするのみ
        	if (operation.triggeredMenu() == true)
        	{
        		finishCanvas();
        	}
        }

        // 選択中のボタンを切り替える
        selectedButtonId++;
    	if (selectedButtonId == nofButton)
    	{
    	    selectedButtonId = 0;
    	}
        return;
    }

	/**
     *  センターキーが押されたときの処理<br>
     *  アイテムを選択します。複数アイテム選択モードの場合には、アイテムの選択のみを行います。<br>
     *  （この場合、ボタン長押しでアイテムが確定します。）
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputFire (boolean isRepeat)
    {
        // 選択したアイテムを記録する
        operation.selectedItem(buttonSelection);
        if ((isRepeat == true)||(operation.isSelectedItemMulti() != true))
        {
            // ボタン長押し、あるいはシングルアイテム選択モードなら終了
        	if (operation.selectedCommandButton(selectedButtonId) == true)
        	{
                finishCanvas();
        	}
        }
        return (true);
    }

	/**
     *  GAME Dキーが押されたときの処理<br>
     *  選択直後に画面を切り替えます。<br>
     *  （ほとんどセンターキーの操作と同じですが、複数選択モードでも、ボタンを押した
     *  アイテムを確定させます）
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputGameD(boolean isRepeat)
    {
        // 選択したアイテムを記録して終了する
        operation.selectedItem(buttonSelection);
        if (operation.selectedCommandButton(selectedButtonId) == true)
        {
            finishCanvas();
        }
        return (true);
    }

	/**
     *  GAME Cキー(クリアキー)が押されたときの処理<br>
     *  選択をキャンセルし、画面を切り替えます。
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputGameC(boolean isRepeat)
    {
        // 押されたボタンを記録して終了する
		if (operation.selectedCommandButton(ISelectionScreenStorage.BUTTON_CANCELED) == true)
		{
            finishCanvas();
		}
        return (true);
    }

	/**
     *  GAME Aキーが押されたときの処理<br>
     *  ある程度（３つ）飛ばして上へアイテムを移動させます
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputGameA(boolean isRepeat)
    {
        if (buttonSelection > 0)
        {
            buttonSelection = buttonSelection - SELECTION_JUMPCOUNT;
            if (buttonSelection < 0)
            {
            	buttonSelection = 0;
            }
        }
        if (topItem > buttonSelection)
        {
            topItem = topItem - (maxLine - 1);
            if (topItem < 0)
            {
            	topItem = 0;
            }
        }
        return (true);
    }

    /**
     *  GAME Bキーが押されたときの処理<br>
     *  ある程度（３つ）飛ばして下へアイテムを移動させます
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputGameB(boolean isRepeat)
    {
        if (buttonSelection < (operation.getSelectionItemCount() - 1))
        {
            buttonSelection = buttonSelection + SELECTION_JUMPCOUNT;
            if (buttonSelection > (operation.getSelectionItemCount() - 1))
            {
            	buttonSelection = operation.getSelectionItemCount() - 1;
            }
        }
        if ((topItem + maxLine) <= buttonSelection)
        {
            topItem = topItem + (maxLine - 1);
            if (topItem >= operation.getSelectionItemCount())
            {
            	topItem = operation.getSelectionItemCount() - 1;
            }
        }
        return (true);
    }

    /**
     *  ＊キーが押されたときの処理<br>
     *  （アイテムの選択を末尾のアイテムへ移動させます）
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputStar(boolean isRepeat)
    {
		topItem = 0;
		buttonSelection = 0;
        return (true);
    }

	/**
     *  ＃キーが押されたときの処理<br>
     *  （アイテムの選択を末尾のアイテムへ移動させます）
     *  @param isRepeat キーリピート入力
     *  @return 操作した(true) / 操作しない(false)
     */
	public boolean inputPound(boolean isRepeat)
    {
		int nofPage = ((operation.getSelectionItemCount() - 1) / (maxLine - 1));
        topItem = nofPage * (maxLine - 1);
        if (topItem < 0)
        {
            topItem = 0;
        }
        buttonSelection = operation.getSelectionItemCount() - 1;
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
