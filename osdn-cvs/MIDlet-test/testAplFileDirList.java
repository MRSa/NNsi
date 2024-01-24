import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;

/**
 * ディレクトリ情報(ディレクトリ内一覧情報)提供クラス
 * @author MRSa
 *
 */
public class testAplFileDirList
{
	private String    currentDirectory = "file:///";
	private Vector    scanDirList      = null;
	private long     availableSize    = 0;
	private boolean dirOnlyMode     = false;

	/**
	 * コンストラクタ
	 *
	 */
	testAplFileDirList()
	{
        // 何もしない...
	}

	/**
	 * ディレクトリ情報の準備...
	 * @return true(rootディレクトリの検索成功)/false(検索失敗)
	 */
	public boolean prepare(String directory)
	{
		return (scanDirectory(directory));
	}

	/**
	 * ディレクトリ検索モードの設定
	 * 
	 * @param isDirectoryMode true:ディレクトリだけ/ false:ファイルも含む
	 */
	public void setScanDirectoryMode(boolean isDirectoryMode)
	{
		dirOnlyMode = isDirectoryMode;
		return;
	}

    /**
     * ディレクトリをひとつ下に移動する
     * @param path (次のディレクトリ名)
     * @return true(成功)/false(失敗)
     */
    public boolean downDirectory(String path)
    {
		if ((path.endsWith("/")) == false)
		{
			// 指定された pathはディレクトリではない、終了する
			return (false);
		}
		if ((path.equals("..")) == true)
		{
			// ひとつ上のディレクトリに移動することが指定された、、
			return (upDirectory());
		}
    	return (scanDirectory(currentDirectory + path));
    }
    
    /**
     * ディレクトリをひとつ上に移動する
     * @return true(成功)/false(失敗)
     */
    public boolean upDirectory()
    {
    	// 既にrootディレクトリにいた、、終了する
        String nextPath = null;
        if (currentDirectory == null)
        {
        	return (false);
        }
		int pos = currentDirectory.lastIndexOf('/', currentDirectory.length() - 2);
		if (pos >= 0)
		{
            nextPath = currentDirectory.substring(0, pos + 1);
		}
		else
		{
			return (false);
		}
        return (scanDirectory(nextPath));
    }

	/**
     * ファイル情報を取得する...
     * @param index
     * @return ファイル情報クラス
     */
    public fileInformation getFileInfo(int index)
    {
    	if ((index < 0)||(index >= scanDirList.size()))
    	{
    		// インデックスの範囲外...
    		return (null);
    	}
    	return ((fileInformation) scanDirList.elementAt(index));
    }

    /**
     * ディレクトリ内にあるファイル数を応答する
     * @return
     */
    public int getCount()
    {
    	return (scanDirList.size());
    }

	/**
	 * ディレクトリ名を応答する
	 * @return
	 */
	public String getDirectoryName()
	{
		return (currentDirectory);
	}
	
	/**
	 * ディレクトリのスキャン
	 * @param path スキャンするディレクトリ (nullの場合にはrootから)
	 * @return true(スキャン成功)/false(失敗)
	 */
    public boolean scanDirectory(String path)
	{
/*****
    	boolean        isRoot  = false;
		Enumeration     dirEnum = null;
		FileConnection  fc      = null;
		try
		{
			if ((path == null)||(path.equals("file:///")))
			{
				// ディレクトリのルートから一覧を取得する
				dirEnum = FileSystemRegistry.listRoots();
				isRoot  = true;
				path    = "file:///";
			}
			else
			{
				// 指定されたパスから一覧を取得する
				fc = (FileConnection) Connector.open(path, Connector.READ);
				dirEnum = fc.list("*", true);
				availableSize = fc.availableSize();  // ついでにファイルサイズも取得する...
				fc.close();
			}
		}
		catch (Exception e)
		{
            try
            {
                if (fc != null)
				{
					fc.close();
				}
            }
			catch (Exception e2)
			{
				//
            }
			return (false);
		}

		// ディレクトリ内にあるファイル一覧を追加する
		scanDirList = null;
		scanDirList = new Vector();
		if (isRoot != true)
		{
			addItem(null, "..", null);
		}
		currentDirectory = path;
		while (dirEnum.hasMoreElements())
		{
			String name = (String)dirEnum.nextElement();
    		if (dirOnlyMode == true)
    		{
    			if (name.endsWith("/"))
    			{
    				addItem(path, name, null);    				
    			}
    		}
    		else
    		{
    			addItem(path, name, null);
    		}
		}
*****/
		System.gc();
		return (true);
	}

    /**
     * ファイル情報クラスを追加する
     * @param name
     * @param data
     */
    private void addItem(String path, String name, String data)
    {
    	fileInformation info = new fileInformation(name, this);
//    	info.updateDetailInfo();
     	scanDirList.addElement(info);
		return;
    }

    /**
     * ファイル情報クラスを追加する
     * @param name
     * @param data
     */
    public boolean updateDetailInfo(fileInformation info)
    {
/*****/
    	FileConnection  fc       = null;
    	try
    	{
    		String path = currentDirectory + info.getFileName();
			fc = (FileConnection) Connector.open(path, Connector.READ);
			info.setDateTime(fc.lastModified());
			info.setHidden(fc.isHidden());
			info.setWritable(fc.canWrite());
    		if (fc.isDirectory() != true)
			{
    			info.setFileSize(fc.fileSize());
			}
			fc.close();
		}
		catch (Exception e)
		{
            try
            {
                if (fc != null)
				{
					fc.close();
				}
            }
			catch (Exception e2)
			{
				//
            }
			return (false);
     	}
/*****/
		return (true);
    }

    /**
     * ファイルをすべて選択する
     * @return
     */
    public int selectAllFiles(boolean isSelected)
    {
    	int count = 0;
		Enumeration cnt = scanDirList.elements();
		while (cnt.hasMoreElements())
		{
			fileInformation data = (fileInformation) cnt.nextElement();
			if (data.isDirectory() != true)
			{
				data.setSelected(isSelected);
				count++;
			}
		}
    	return (count);
    }
    
    /**
     * ディスクの空きサイズを応答する
     * @return
     */
    public long getAvailableSize()
    {
    	return (availableSize);
    }
    
    /**
     * ファイル情報クラス
     * @author MRSa
     */
    public class fileInformation
    {
    	private testAplFileDirList parent = null;
    	private String   fileName      = null;
    	private long    dateTime      = -1;
    	private long    fileSize      = -1;
    	private boolean hiddenFile    = false;
    	private boolean writableFile  = true;
    	private boolean selected      = false;
    	private boolean isDetail      = false;

    	/**
    	 * コンストラクタ...
    	 * @param name
    	 */
    	fileInformation(String name, testAplFileDirList object)
    	{
    		fileName    = name;
    		parent      = object;
    	}

    	/**
    	 * ファイル名(ディレクトリ名)を応答する
    	 * @return
    	 */
    	public String getFileName()
    	{
    		return (fileName);
    	}

    	/**
    	 * 選択モードを切り替える
    	 * @param sel
    	 */
    	public void setSelected(boolean sel)
    	{
    		selected = sel;
    		return;
    	}

    	/**
    	 * 選択モードを切り替える
    	 * @param sel
    	 */
    	public void toggleSelected()
    	{
    		if (selected == true)
    		{
    			selected = false;
    		}
    		else
    		{
    			selected = true;
    		}
    		return;
    	}

    	/**
    	 * 表示データの更新を行う
    	 * @return
    	 */
    	public boolean updateDetailInfo()
    	{
    		isDetail = parent.updateDetailInfo(this);
    		return (isDetail);
    	}

    	/**
    	 * ファイルサイズを設定する
    	 * @param size
    	 */
    	public void setFileSize(long size)
    	{
    		fileSize = size;
    		return;
    	}

    	/**
    	 * ファイルサイズを取得する
    	 * @return
    	 */
    	public long getFileSize()
    	{
    		if (isDetail == false)
    		{
    			return (-1);
    		}    			
    		return (fileSize);
    	}

    	public void setDateTime(long time)
    	{
    		dateTime = time;
    		return;
    	}
 
    	/**
    	 * ファイル作成日時を応答する
    	 * 
    	 * @return
    	 */
    	public long getDateTime()
    	{
    		if (isDetail == false)
    		{
    			return (-1);
    		}    			
    		return (dateTime);
    	}
    	
    	/**
    	 *  隠しファイルかどうかの設定を行う
    	 */
    	public void setHidden(boolean hidden)
    	{
    		hiddenFile = hidden;
    		return;
    	}
  	
    	/**
    	 * 隠しファイルかどうか応答する
    	 * 
    	 * @return
    	 */
    	public boolean isHidden()
    	{
    		return (hiddenFile);
    	}

    	/**
    	 * 書き込み可否を設定する
    	 * @param writable
    	 */
    	public void setWritable(boolean writable)
    	{
    		writableFile = writable;
    		return;
    	}

    	/**
    	 * 書き込み可否を応答する
    	 * @return
    	 */
    	public boolean isWritable()
    	{
    		return (writableFile);
    	}
    	
    	
    	/**
    	 * ディレクトリかどうかを応答する
    	 * @return  true: ディレクトリ、false:通常ファイル
    	 */
    	public boolean isDirectory()
    	{
    		if (fileName.endsWith("/"))
    		{
    			return (true);
    		}
    		if (fileName.equals(".."))
    		{
    			return (true);
    		}
    		return (false);
    	}

    	/**
    	 * ファイルが選択中かどうか
    	 * @return
    	 */
    	public boolean isSelected()
    	{
    		return (selected);
    	}
    }
}
