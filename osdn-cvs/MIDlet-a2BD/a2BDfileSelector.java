import java.util.Enumeration;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.List;
import java.util.Vector;

public class a2BDfileSelector extends List implements CommandListener
{
	private a2BDsceneSelector   parent = null;
	private String    path   = null;
	private String    nextPath = "file:///";

	private final int COMMAND_EXIT     = -1;
	private final int COMMAND_SELECT   = 0;
	
	private Command   exitCmd    = new Command("Cancel",     Command.EXIT,    COMMAND_EXIT);
	private Command   selectCmd  = new Command("選択",       Command.SCREEN,  COMMAND_SELECT);
	private Vector    itemVector = null;

	/*
	 *   コンストラクタ
	 * 
	 */
	public a2BDfileSelector(a2BDsceneSelector object)
	{
		super("", List.IMPLICIT);
		parent = object;

		openDirectory(true);
	}

	/*
	 *   次のディレクトリパスを設定する
	 *
	 */
	public void setnextPath(String path)
	{
		nextPath = path;
		return;
	}
	
	/*
	 *   ディレクトリを開く
	 * 
	 */	
	public void openDirectory(boolean isRoot)
	{
		Enumeration dirEnum = null;
		
		if (isRoot == true)
		{
			dirEnum = FileSystemRegistry.listRoots();
		}
		else
		{
			FileConnection fc = null;
			try
			{
				fc = (FileConnection) Connector.open(nextPath, Connector.READ);
				dirEnum = fc.list();
	            fc.exists();
			}
			catch (Exception e)
			{
                try {
                    if (fc!=null)
					{
						fc.exists();
					}
                }
				catch (Exception e2)
				{
					
                }
				return;
			}
		}
		setTitle(nextPath);
		deleteAll();
		itemVector = null;
		itemVector = new Vector();
		this.addCommand(exitCmd);
		this.setSelectCommand(selectCmd);
		this.setCommandListener(this);
		
		while (dirEnum.hasMoreElements())
		{
			addItem((String)dirEnum.nextElement());
		}

		if (!nextPath.equals("file:///"))
		{
			addItem("..");
		}
		path = nextPath;
	}
	
	/*
	 *   アイテムを追加する
	 * 
	 */
	private void addItem(String string) 
	{
		append(string, null);
		itemVector.addElement(string);
		
		return;
	}
	
	/*
	 *   アイテム 選択時の処理
	 *   
	 */
	private void selection()
	{
		int index = -1;
		String name = "";

		index = getSelectedIndex();
		if (index < 0)
		{
			return;
		}
		name = (String) itemVector.elementAt(index);
		if (name.equals(".."))
		{
			int pos = path.lastIndexOf('/', path.length() - 2);
			nextPath = path.substring(0, pos + 1);
		}
		else
		{
			nextPath = path + name;
		}
		
		if (nextPath.endsWith("/"))
		{
			refreshScreen();
			return;
		}

		// 親に対し、選択したファイル名を通知する
		parent.EndFileSelector(nextPath);

		return;
	}
		
	/*
	 *   コマンドボタン実行時の処理
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		if (c == exitCmd)
		{
			// 一覧へ戻る処理
			parent.EndFileSelector(null);
			return;
		}
		else if (c == selectCmd)
		{
			// アイテム選択時の処理
			selection();
			return;
		}
		return;
	}

	
	/*
	 *   ファイル削除完了時の処理...
	 * 
	 */
	public void refreshScreen()
	{
		Thread thread = new Thread()
		{
			public void run()
			{
				openDirectory(false);
			}
		};
		thread.start();
		return;
	}
}
//--------------------------------------------------------------------------
//  a2BDfileSelector  --- ファイル選択
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
