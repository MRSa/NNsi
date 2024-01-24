import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

public class testAplCanvas  extends Canvas implements CommandListener
{
	private testAplSceneSelector parent         = null;
	private testAplFileDirList   fileListObject = null;
	private boolean             painting       = false;

	private Font screenFont = null;
	private int  keyValue   = 0;
	private int  keyNumber  = 0;

	/**
	 *  コンストラクタ
	 * 
	 * @param object
	 */
	testAplCanvas(testAplSceneSelector object)
	{
		parent = object;
		// タイトルの設定
		setTitle(null);

		// フォントの設定
		screenFont = Font.getDefaultFont();
		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);
	}
	
	/**
	 *  描画クラスの準備...
	 *
	 */
	public void prepare()
	{
		return;
	}

	/**
	 *   画面の準備...
	 */
	public void prepareScreen()
	{
		return;
	}

	/**
	 * 画面再描画指示...
	 *
	 */
	public void redraw()
	{
		repaint();
	}

	/**
	 * 画面の描画を実施する
	 * @param g グラフィックキャンバス
	 */
	public void paint(Graphics g)
	{
		// 画面描画中のときには折り返す
		if (painting == true)
		{
			return;
		}
		painting = true;

		// 画面の塗りつぶし
		g.setColor(0x00f0f0f0);
		g.fillRect(0, 0, getWidth(), getHeight());
		g.setColor(0x00000000);
		g.setFont(screenFont);

		if (keyValue != 0)
		{			
	        String str = "keyCode : " + String.valueOf(keyValue);
            g.drawString(str, 10, 10, (Graphics.LEFT | Graphics.TOP));
		}
		if (keyNumber != 0)
		{
	        String str = "gameAction : " + String.valueOf(keyNumber);
            g.drawString(str, 10, 40, (Graphics.LEFT | Graphics.TOP));
		}
		painting = false;
        return;
	}
	/**
	 *   キー入力
	 */
	public void keyPressed(int keyCode)
	{
		if ((keyCode == 0)||(painting == true))
		{
			// キー入力がない、または描画中の場合には折り返す
			keyValue  = 0;
			keyNumber = 0;
			return;
		}
		keyValue =  keyCode;
		keyNumber = getGameAction(keyCode);
		
		repaint();
		return;
	}

	/**
	 *   コマンドボタン実行時の処理
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		int command = c.getPriority();
		commandActionMain(command);
		return;
	}

	/**
	 *   コマンドの実行メイン
	 * 
	 */
	private void commandActionMain(int command)
	{
		prepareFileList();
        return;
	}
	

	/**
	 *  ファイルリストオブジェクトの準備...
	 *
	 */
	private void prepareFileList()
	{
		//
		fileListObject = parent.getDirList();
        return;
	}
}
