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
	private Command   selectCmd  = new Command("�I��",       Command.SCREEN,  COMMAND_SELECT);
	private Vector    itemVector = null;

	/*
	 *   �R���X�g���N�^
	 * 
	 */
	public a2BDfileSelector(a2BDsceneSelector object)
	{
		super("", List.IMPLICIT);
		parent = object;

		openDirectory(true);
	}

	/*
	 *   ���̃f�B���N�g���p�X��ݒ肷��
	 *
	 */
	public void setnextPath(String path)
	{
		nextPath = path;
		return;
	}
	
	/*
	 *   �f�B���N�g�����J��
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
	 *   �A�C�e����ǉ�����
	 * 
	 */
	private void addItem(String string) 
	{
		append(string, null);
		itemVector.addElement(string);
		
		return;
	}
	
	/*
	 *   �A�C�e�� �I�����̏���
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

		// �e�ɑ΂��A�I�������t�@�C������ʒm����
		parent.EndFileSelector(nextPath);

		return;
	}
		
	/*
	 *   �R�}���h�{�^�����s���̏���
	 *   
	 */
	public void commandAction(Command c, Displayable d)
	{
		if (c == exitCmd)
		{
			// �ꗗ�֖߂鏈��
			parent.EndFileSelector(null);
			return;
		}
		else if (c == selectCmd)
		{
			// �A�C�e���I�����̏���
			selection();
			return;
		}
		return;
	}

	
	/*
	 *   �t�@�C���폜�������̏���...
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
//  a2BDfileSelector  --- �t�@�C���I��
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
