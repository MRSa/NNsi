import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;
import java.util.Hashtable;

/**
 * キャンバスクラスの表示（メイン）処理...
 * 
 * @author MRSa
 *
 */
public class kaniFileCanvas  extends Canvas implements CommandListener
{
	public static final int   SCENE_FILELIST         = 0;
    public static final int   SCENE_DIRLIST          = 1;
    public static final int   SCENE_FILEOPEMODE      = 2;
    public static final int   SCENE_CONFIRMATION     = 3;
    public static final int   SCENE_TEXTEDIT         = 4;
    public static final int   SCENE_TEXTEDIT_COMMAND = 5;
    public static final int   SCENE_PICTUREVIEW      = 6;

    private final String        SCENESTR_FILELIST          = "F";
	private final String        SCENESTR_DIRLIST           = "D";
	private final String        SCENESTR_FILEOPEMODE       = "O";
	private final String        SCENESTR_CONFIRMATION      = "C";
	private final String        SCENESTR_TEXTEDIT          = "T";
	private final String        SCENESTR_TEXTEDIT_COMMAND  = "E";
	private final String        SCENESTR_PICTUREVIEW       = "P";

	private final int           COMMAND_EXIT      = 0;
	private final int           COMMAND_EXECUTE   = 1;
	
	private Font                 screenFont        = null;
	private kaniFileKeyInput     keyIn             = null;

	private Hashtable            drawObjectMap     = null;
	private IkaniFileCanvasPaint currentDrawObject = null;
	private kaniFileSceneSelector  parent          = null;

	private boolean             working           = false;
	private boolean             painting          = false;
	private boolean             isBusyMode        = false;
	private String               busyMessage       = null;

	private Command opeCmd  = new Command("実行",  Command.ITEM, COMMAND_EXECUTE);
//	private Command exitCmd = new Command("終了",  Command.EXIT, COMMAND_EXIT);

	/**
	 * コンストラクタ
	 * @param parent
	 */
	kaniFileCanvas(kaniFileSceneSelector object)
	{
		// タイトルの設定
		setTitle(null);

		// フォントの設定
		screenFont = Font.getDefaultFont();
		screenFont = Font.getFont(screenFont.getFace(), screenFont.getStyle(), Font.SIZE_SMALL);

		// キー入力クラスの設定
		keyIn = new kaniFileKeyInput();
	
		// 画面描画オブジェクトの準備...
		drawObjectMap = new Hashtable();
		prepareScreenObjects(object);

		parent = object;
		
		// コマンドの追加
		this.addCommand(opeCmd);
//		this.addCommand(exitCmd);
		this.setCommandListener(this);	
	}

	/**
	 * 画面描画オブジェクトの生成...
	 * 
	 * @param object
	 */
	private void prepareScreenObjects(kaniFileSceneSelector object)
	{
		/// ファイル/ディレクトリ表示モードの準備...
		IkaniFileCanvasPaint screenObject = new kaniFileScreenDirList(object);
		screenObject.prepare(screenFont, SCENE_FILELIST);
		screenObject.setRegion(0, 0, getWidth(), getHeight());
		drawObjectMap.put(SCENESTR_FILELIST, screenObject);	

		// ディレクトリ表示モードの準備...
		screenObject = new kaniFileScreenDirList(object);
		screenObject.prepare(screenFont, SCENE_DIRLIST);
		screenObject.setRegion(15, 15, getWidth() - 30, getHeight() - 30);
		drawObjectMap.put(SCENESTR_DIRLIST, screenObject);	

		// コマンド選択モードの準備...
		screenObject = new kaniFileScreenCommandList(object);
		screenObject.prepare(screenFont, SCENE_FILEOPEMODE);
		screenObject.setRegion(2, getHeight() - (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)) - 2, getWidth() - 4, (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)));
		drawObjectMap.put(SCENESTR_FILEOPEMODE, screenObject);

		// コマンド選択モード(テキスト編集時)の準備...
		screenObject = new kaniFileScreenCommandList(object);
		screenObject.prepare(screenFont, SCENE_TEXTEDIT_COMMAND);
		screenObject.setRegion(2, getHeight() - (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)) - 2, getWidth() - 4, (screenFont.getHeight() * (kaniFileScreenCommandList.selectionList + 1)));
		drawObjectMap.put(SCENESTR_TEXTEDIT_COMMAND, screenObject);

		// 確認画面の準備...
		screenObject = new kaniFileScreenConfirmation(object);
		screenObject.prepare(screenFont, SCENE_CONFIRMATION);
		screenObject.setRegion(2, (getHeight() / 2) - (screenFont.getHeight() * 2), getWidth() - 4, (screenFont.getHeight() * 3));
		drawObjectMap.put(SCENESTR_CONFIRMATION, screenObject);

		// テキスト編集画面の準備...
		screenObject = new kaniFileScreenTextEdit(object);
		screenObject.prepare(screenFont, SCENE_TEXTEDIT);
		screenObject.setRegion(0, 0, getWidth(), getHeight());
		drawObjectMap.put(SCENESTR_TEXTEDIT, screenObject);

		// グラフィック表示画面の準備...
		screenObject = new kaniFileScreenPictureScreen(object);
		screenObject.prepare(screenFont, SCENE_PICTUREVIEW);
		screenObject.setRegion(10, 10, getWidth() - 20, getHeight() - 20);
		drawObjectMap.put(SCENESTR_PICTUREVIEW, screenObject);

		return;
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
		//
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
		
		if ((isBusyMode == false)&&(busyMessage != null))
		{
			// BUSY モードを解除するときだった...
			hideBusyWindow(g);			
		}
		
        /////////////////////////////////////

		// 描画エンジンが指定されているか？
		if (currentDrawObject != null)
		{
			g.setFont(screenFont);
/**
			// 画面全体のクリア...は、描画エンジンで...
			g.setColor(0x00ffffff);
			g.fillRect(currentDrawObject.getTopX(), currentDrawObject.getTopY(), currentDrawObject.getWidth(), currentDrawObject.getHeight());
**/

			// ウィンドウが画面全体ではないときは、枠を表示する
			if (currentDrawObject.getTopY() > 0)
			{
				g.setColor(0);
				g.drawRect(currentDrawObject.getTopX() - 1, currentDrawObject.getTopY() - 1, currentDrawObject.getWidth() + 1, currentDrawObject.getHeight() + 1);
			}
			
			// 画面の描画を実施...
			currentDrawObject.paint(g);
		}

        //////////////////////////////////////
		
		if ((isBusyMode == true)&&(busyMessage != null))
		{
			// BUSY モードだった...BUSYメッセージを表示する
			showBusyWindow(g);			
		}

		// 終了する...
		painting = false;
		return;
	}

	/**
	 * 検索中の表示...
	 * 
	 */
	private void showBusyWindow(Graphics g)
	{
		int height = screenFont.getHeight() + 4;

		// 画面の塗りつぶし
		g.setColor(0x00ffffc0);
		g.fillRect(0, (getHeight() - height - 2), getWidth(), (getHeight() - 2));
		g.setColor(0, 0, 0);
		g.setFont(screenFont);
		
		// コマンドウィンドウタイトルの表示
		g.drawString(busyMessage, 5, (getHeight() - height), (Graphics.LEFT | Graphics.TOP));
		return;		
	}

	/**
	 * 検索中の表示削除...
	 * 
	 */
	private void hideBusyWindow(Graphics g)
	{
		int height = screenFont.getHeight() + 4;
		
		// 画面の塗りつぶし
		g.setColor(0x00ffffff);
		g.fillRect(0, (getHeight() - height - 2), getWidth(), (getHeight() - 2));

		busyMessage = null;

		return;		
	}
	
	/**
	 * 表示を切り替える...
	 * @param scene
	 */
	public void changeScene(int scene, String message)
	{
		try
		{
			String sceneString = convertScene(scene);
			IkaniFileCanvasPaint object = (IkaniFileCanvasPaint) drawObjectMap.get(sceneString);
			if (object != null)
			{
				currentDrawObject = object;
				currentDrawObject.setInformation(message);
				currentDrawObject.changeScene();
				keyIn.setScreenCanvas(currentDrawObject);
				repaint();
			}
		}
		catch (Exception e)
		{
			// 何もしない...
		}
		return;
	}

	/**
	 * メッセージを表示する..
	 * @param message
	 */
	public void setInformation(String message)
	{
		if (currentDrawObject != null)
		{
			currentDrawObject.setInformation(message);
		}
		return;
	}

	/**
	 * 情報領域の情報を更新する
	 * 
	 */
	public void updateInformation(String message, int mode)
	{
		if (currentDrawObject != null)
		{
			currentDrawObject.updateInformation(message, mode);
			repaint();
		}
		return;
	}
	
	public void startUpdate(String message)
	{
		isBusyMode  = true;
		busyMessage = message;
		repaint();
		return;
	}
	
	/**
	 *  画面更新...
	 *
	 */
	public void updateData()
	{
		if (working == true)
		{
			return;
		}
		if (currentDrawObject != null)
		{
			working = true;
			currentDrawObject.updateData();
			working = false;
		}
		isBusyMode = false;
		repaint();
		return;
	}

	/**
	 * コマンド実行時の処理
	 */
	public void commandAction(Command c, Displayable d) 
	{
		int command = c.getPriority();
		
		commandActionMain(command);
		return;
	}

	/**
	 * 表示シーンを特定する。。。
	 * @param scene
	 * @return
	 */
	private String convertScene(int scene)
	{
		if (scene == SCENE_DIRLIST)
		{
			return (SCENESTR_DIRLIST);
		}
		if (scene == SCENE_FILELIST)
		{
			return (SCENESTR_FILELIST);
		}
		if (scene == SCENE_FILEOPEMODE)
		{
			return (SCENESTR_FILEOPEMODE);
		}
		if (scene == SCENE_CONFIRMATION)
		{
			return (SCENESTR_CONFIRMATION);
		}
		if (scene == SCENE_TEXTEDIT)
		{
			return (SCENESTR_TEXTEDIT);
		}
		if (scene == SCENE_TEXTEDIT_COMMAND)
		{
			return (SCENESTR_TEXTEDIT_COMMAND);
		}
		if (scene == SCENE_PICTUREVIEW)
		{
			return (SCENESTR_PICTUREVIEW);
		}
		return (null);
	}
	
	/**
	 *   キー入力
	 */
	public void keyPressed(int keyCode)
	{
		boolean repaint = false;
		if ((keyCode == 0)||(painting == true)||(working == true))
		{
			// キー入力がない、または描画中の場合には折り返す
			return;
		}

		// 数字キーの入力をチェックする。。。
		int number = keyIn.numberInputMode(keyCode);
		if (number == -1)
		{
			// コマンドを実行した場合...
			repaint();
			return;
		}
		if (number >= 0)
		{
            // 数字が入力された時...
			repaint();
			return;
		}

		// カーソルキーの入力。。。
		switch (getGameAction(keyCode))
		{
          case Canvas.LEFT:
        	repaint = keyIn.inputLeft(false);
			break;
			  
          case Canvas.RIGHT:
        	repaint = keyIn.inputRight(false);
			break;

		  case Canvas.DOWN:
			repaint = keyIn.inputDown(false);
	        break;

		  case Canvas.UP:
			repaint = keyIn.inputUp(false);
            break;

		  case Canvas.FIRE:
			repaint = keyIn.inputFire(false);
			break;

		  case Canvas.GAME_A:
			// GAME-Aキー
			repaint = keyIn.inputGameA(false);
			break;

		  case Canvas.GAME_B:
			// GAME-Bキー
			repaint = keyIn.inputGameB(false);
			break;

		  case Canvas.GAME_C:
			// GAME-Cキー
			repaint = keyIn.inputGameC(false);
			break;

		  case Canvas.GAME_D:
			// GAME-Dキー
			repaint = keyIn.inputGameD(false);
			break;

          default:
        	repaint = false;
            break;
		}
		if (repaint == true)
		{
			repaint();
		}
		return;
	}

	/*
	 *   キーリピートしているとき...
	 * 
	 * 
	 */
	public void keyRepeated(int keyCode)
	{
		boolean repaint = false;
		if ((keyCode == 0)||(painting == true)||(working == true))
		{
			// キー入力がない、または描画中の場合には折り返す
			return;
		}

		// カーソルキーの入力。。。
		switch (getGameAction(keyCode))
		{
          case Canvas.LEFT:
        	repaint = keyIn.inputLeft(true);
			break;
			  
          case Canvas.RIGHT:
        	repaint = keyIn.inputRight(true);
			break;

		  case Canvas.DOWN:
			repaint = keyIn.inputDown(true);
	        break;

		  case Canvas.UP:
			repaint = keyIn.inputUp(true);
            break;

		  case Canvas.FIRE:
			repaint = keyIn.inputFire(true);
			break;

		  case Canvas.GAME_A:
			// GAME-Aキー
			repaint = keyIn.inputGameA(true);
			break;

		  case Canvas.GAME_B:
			// GAME-Bキー
			repaint = keyIn.inputGameB(true);
			break;

		  case Canvas.GAME_C:
			// GAME-Cキー
			repaint = keyIn.inputGameC(true);
			break;

		  case Canvas.GAME_D:
			// GAME-Dキー
			repaint = keyIn.inputGameD(true);
			break;

          default:
        	repaint = false;
            break;
		}
		if (repaint == true)
		{
			repaint();
		}
        return;
	}

	/**
	 *  操作を選択する
	 * 
	 */
	private void selectOperation()
	{
		if (currentDrawObject == null)
		{
			return;
		}
		
		if (currentDrawObject.getMode() == SCENE_FILEOPEMODE)
		{
            // コマンド表示モードのときには、画面を切り替えない...
			return;
		}
		
		if (currentDrawObject.getMode() == SCENE_DIRLIST)
		{
			// ディレクトリ選択モードでディレクトリが選択された場合...
			keyIn.inputDummy();
			return;
		}
		if (currentDrawObject.getMode() == SCENE_TEXTEDIT)
		{
			// テキスト編集モードで「実行」ボタンが押されたとき...
			keyIn.inputDummy();
			repaint();
			return;
		}
		
		if (currentDrawObject.getMode() == SCENE_PICTUREVIEW)
		{
            // 画像表示モードで「実行」ボタンが押されたとき...
			// (画面を切り替えない)
			keyIn.inputDummy();
			return;
		}

		// ダミー入力を入れる。。。(無選択なら、カーソル位置のアイテムを選択する)
		keyIn.inputDummy();
		
		// 画面を切り替える...
		changeScene(SCENE_FILEOPEMODE, "<<<<< 操作選択 >>>>>");
        repaint();
		return;
	}
	
	/**
	 * コマンド実行時の処理。。。
	 * @param command
	 */
	private void commandActionMain(int command)
	{
		if (command == COMMAND_EXECUTE)
		{
			// 操作を選択する...
			selectOperation();
			return;
		}
		else if (command == COMMAND_EXIT)
		{
			// アプリケーションを終了させる
			parent.finishApp();
		}
		return;
	}
}
