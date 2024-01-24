import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * busyForm
 * 
 * @author MRSa
 *
 */
public class busyForm  extends Canvas  implements CommandListener
{
	private a2BDmain  parent     = null;
	private boolean painting     = false;
	private String   busyTitle    = "";
	private String   busyMessage  = "";
	private String   busyMessage2 = "";
	private String   busyMessage3 = "";
	private Font     screenFont   = null;
	private int     foreColor    = 0;
	private int     backColor    = 0x00ffffff;
	
	private boolean enableExit  = false;
	private Command  exitCmd      = new Command("終了", Command.EXIT, -1);

	/**
	 * コンストラクタ
	 * @param arg0
	 */
	public busyForm(a2BDmain object, String arg0)
    {
		// タイトル
		setTitle(arg0);

		// 画面のコマンドを登録する
		this.addCommand(exitCmd);
		this.setCommandListener(this);

		// 親クラスを記憶する
        parent = object;    	
    }
	
	/**
	 *   フォントと画面サイズを準備する
	 * 
	 * 
	 */
	public void prepareScreen()
	{
		screenFont = Font.getDefaultFont();
//		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);
		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_MEDIUM);
	}

	/**
	 * 描画色を設定する
	 * @param color
	 */
	public void setForeColor(int color)
	{
		foreColor = color;
	}

	/**
	 * 背景色を設定する
	 * @param color
	 */
	public void setBackColor(int color)
	{
		backColor = color;
	}
	
	/**
	 * BUSYメッセージを設定する
	 * @param title
	 * @param message
	 * @param message2
	 */
	public void SetBusyMessage(String title, String message, String message2, String message3, boolean isExit)
	{
		enableExit   = isExit;
		busyTitle    = title;
		busyMessage  = message;
		busyMessage2 = message2;
		busyMessage3 = message3;
		repaint();
		return;
	}
	
	public void HideBusyMessage()
	{
		busyTitle    = "";
		busyMessage  = "";
		busyMessage2 = "";
		repaint();
		return;		
	}
	
	/*
	 *   画面描画...
	 * 
	 * 
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
		g.setColor(backColor);
		g.fillRect(0, 0, getWidth(), getHeight());
		g.setColor(foreColor);
		g.setFont(screenFont);

		// タイトルの設定
		if (busyTitle != null)
		{
			setTitle(busyTitle);
		}

		int line = 0;
		
		// メッセージ１の反映
        if (busyMessage != null)
        {
  		    g.setColor(foreColor);
            line = line + drawString(g, line, busyMessage);
        }
        
        // メッセージ２の反映
        if (busyMessage2 != null)
        {
  		    g.setColor(foreColor);
            line = line + drawString(g, line, busyMessage2);
        }
        
        // メッセージ３の反映
        if (busyMessage3 != null)
        {
  		    g.setColor(foreColor);
            line = line + drawString(g, line, busyMessage3);
       }

        painting = false;
		return;
	}

	/**
	 * 文字を出力する
	 * @param g
	 * @param startLine
	 * @param msg
	 * @return
	 */
	private int drawString(Graphics g, int startLine, String msg)
	{
		// 表示可能行を設定する
		int line    = 0;
		int y       = (startLine * (screenFont.getHeight())) + 4;
		int x       = 0;
		int limitX  = getWidth() - 4;
		for (int msgIndex = 0; msgIndex < msg.length(); msgIndex++)
		{
			char drawChar = msg.charAt(msgIndex);
			if (drawChar != '\n')
			{
				g.drawChar(drawChar, x, y, (Graphics.LEFT | Graphics.TOP));
				x = x + screenFont.charWidth(drawChar);
			}
			else
			{
				// 改行コードのときは表示せずに改行する...
				x = x + limitX;
			}
			if (x >= limitX)
			{
				x = 0;
				y = y + screenFont.getHeight();
				line++;
			}
		}
        return (line + 1);
	}

	/**
	 * コマンド実行時の処理
	 * 
	 **/
	public void commandAction(Command c, Displayable d)
	{
        if (enableExit == false)
        {
        	 // コマンド受付許可じゃない場合には終了する。
        	 return;
        }
        if (c == exitCmd)
        {
        	// アプリケーションの終了...
        	parent.quit_a2BD();
            return;
        }
        return;
    }
}