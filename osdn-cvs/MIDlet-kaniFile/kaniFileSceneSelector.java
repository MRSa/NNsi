import java.util.Vector;
import javax.microedition.lcdui.Display;
import javax.microedition.midlet.MIDlet;
import javax.microedition.lcdui.Image;

/**
 * シーンセレクタ
 * 
 * @author MRSa
 *
 */
public class kaniFileSceneSelector
{
    private final int  SCENE_DEFAULT  = kaniFileCanvas.SCENE_FILELIST;
    
    private final int SCENE_NOTSPECIFY          = -1;
    private final int SCENE_DELETE              = 200;
    private final int SCENE_COPYFILE            = 201;
    private final int SCENE_SAVETEXTFILE        = 202;
    private final int SCENE_OUTPUT_FILELIST     = 203;
    private final int SCENE_EXTRACT_ZIP         = 204;
    private final int SCENE_EXTRACT_ZIP_SAMEDIR = 205;

	private MIDlet          parent  = null;
	private kaniFileFactory factory = null;

	private boolean zipSame = false;
	private int    currentScene     = SCENE_DEFAULT;
	private String  showDirectory   = null;
	private String  memoryDirectory = null;
	private String  lastName        = null;
	private String  changeName      = null;
	private String  infoMessage     = null;
	private int    lastIndex          = -1;
	private int    selectedFileCount  = 0;
	private int    memoryFileCount    = 0;
	private int    confirmationMode   = SCENE_NOTSPECIFY;
	private int    currentIndexNumber = -1;
	private int    fileOperationInfo  = -1;
    private Vector targetFileList     = null;
	
	/**
	 * コンストラクタ
	 * @param parent
	 */
	kaniFileSceneSelector(MIDlet object)
	{
		parent  = object;
		factory = new kaniFileFactory();
	}

	/**
	 * 起動準備
	 * 
	 * @return
	 */
	public boolean initialize()
	{
		// 内部情報の準備...
		factory.prepare(this);
		
		// 画面表示の準備...
		factory.prepareScreen();

		// 前回のディレクトリを思い出す
		showDirectory = (factory.getDataStorage()).getDirectory();

		System.gc();

		return (true);
	}
	
	/**
	 *  初期画面にシーンを切り替える
	 *
	 */
	public void start()
	{
		changeScene(SCENE_DEFAULT, "");
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
		return;
	}

	/**
	 *  初期画面にシーンを切り替える
	 *
	 */
	public void stop()
	{
		// ディレクトリを記録する
		(factory.getDataStorage()).setDirectory(showDirectory);

		factory.cleanup();
		return;
	}

	/**
	 *   アプリケーションを終了させる
	 *
	 */
	public void finishApp()
	{

		try
		{
			// データのバックアップとオブジェクトの削除を実行する
			stop();

			// ガベコレの実施
	        System.gc();

			// アプリケーションの終了...
			parent.notifyDestroyed();
		}
		catch (Exception ex)
		{
			//
		}
		return;
	}
	
	/**
	 *    URLをWebブラウザで開く
	 *     (アプリ終了後にブラウザを起動する)
	 * 
	 *    @param url ブラウザで接続するURL
	 * 
	 */
	private void switchToWebBrowser(String url)
	{
		try
		{
			// URLのオープン指示を行う
			parent.platformRequest(url);

			// データのバックアップとオブジェクトの削除を実行する
			stop();

			// ガベコレの実施
            System.gc();

			// アプリケーションの終了...
			parent.notifyDestroyed();
		}
		catch (Exception e)
		{
			/////// ブラウザ起動は未サポートだった、何もしない  /////
		}
		return;
	}

	/**
	 *  画面表示切替スレッド実行
	 *
	 */
	private void changeScene(int scene, String message)
	{
		currentScene = scene;
		infoMessage  = message;
		Thread thread = new Thread()
		{
			public void run()
			{
				//
                if (isShowCanvasForm(currentScene) == true)
                {
                	// 画面シーンを変更する...
                	(factory.getCanvas()).changeScene(currentScene, infoMessage);
                	
            		// キャンバスクラスに切り替える...
            		Display.getDisplay(parent).setCurrent(factory.getCanvas());
                }
                else
                {
                	// 画面シーンを変更する...
                	(factory.getForm()).changeScene(currentScene, infoMessage);

                	// 入力フォームに切り替える...
            		Display.getDisplay(parent).setCurrent(factory.getForm());                	
                }
			}
		};
		thread.start();
		return;
	}
	
	/**
	 * キャンバスを表示するか、フォームを表示するか...
	 * @return
	 */
	private boolean isShowCanvasForm(int scene)
	{
        // 表示シーンに合わせて、キャンバスフォームにする(true)か、入力フォームにする(false)か判定する
		if (scene == kaniFileForm.SCENE_RENAMEINPUT)
		{
			return (false);
		}	
		if (scene == kaniFileForm.SCENE_MAKEDIR)
		{
			return (false);
		}
		if (scene == kaniFileForm.SCENE_DUPLICATEINPUT)
		{
			return (false);
		}
		if (scene == kaniFileForm.SCENE_EDITATTRIBUTE)
		{
			return (false);
		}
		if (scene == kaniFileForm.SCENE_TEXTEDITLINE)
		{
			return (false);
		}
		return (true);
	}

	/**
	 * 現在のディレクトリ名を応答する
	 * @return
	 */
    public String getCurrentDirectory()
    {
    	return ((factory.getDirectoryInfo()).getDirectoryName());
    }
    
    /**
     * ディレクトリの移動...
     * @param pathName
     */
    public void moveDirectory(String pathName)
    {
		if (showDirectory == null)
		{
			showDirectory = "file:///";
		}
    	showDirectory = showDirectory + pathName;
    	selectedFileCount = 0;
        Thread thread = new Thread()
		{
			public void run()
			{
				(factory.getCanvas()).startUpdate("Checking " + showDirectory);
				boolean ret = (factory.getDirectoryInfo()).scanDirectory(showDirectory);
				if (ret == false)
				{
					// ディレクトリの検索に失敗した...
					showDirectory = "file:///";
                    ret = (factory.getDirectoryInfo()).scanDirectory(showDirectory);
				}
				if (ret != false)
				{
					(factory.getCanvas()).updateData();
				}
			}
		};
		thread.start();
        return;    	
    }

    /**
     * ディレクトリの移動...
     * @param pathName
     */
    public void upDirectory()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
				(factory.getCanvas()).startUpdate("Checking ..");
				(factory.getDirectoryInfo()).upDirectory();
            	(factory.getCanvas()).updateData();
            	showDirectory = (factory.getDirectoryInfo()).getDirectoryName();
            	selectedFileCount = 0;
			}
		};
		thread.start();
        return;    	
    }

    /**
     * 現在のディレクトリにあるファイル数を取得する
     * @return
     */
    public int getNumberOfFiles()
    {
    	return ((factory.getDirectoryInfo()).getCount());
    }

    /**
     * ファイル情報を取得する
     * @param index
     * @return
     */
    public kaniFileDirList.fileInformation getFileInfo(int index)
    {
    	kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(index);
    	return (info);
    }
    
    /**
     * ファイルが選択された/選択解除されたとき...
     * @param fileName
     * @param isSelected
     */
    public void selectedFile(String fileName, int index, boolean isSelected)
    {
        if (isSelected == true)
        {
        	selectedFileCount++;
        	lastName  = fileName;
        	lastIndex = index;
        }
        else
        {
        	selectedFileCount--;
        }    	
    	return;
    }

    /**
     * 無選択だった場合、ファイルを選択する
     * @param fileName
     * @param index
     * @return
     */
    public boolean selectedFile(String fileName, int index)
    {
    	if (selectedFileCount == 0)
    	{
    		selectedFileCount++;
    		lastName  = fileName;
    		lastIndex = index;
    		return (true);
    	}
    	return (false);
    }

    /**
     * 全選択/全選択解除
     *
     */
    public void selectAllFiles(boolean isSelected)
    {
    	selectedFileCount = (factory.getDirectoryInfo()).selectAllFiles(isSelected);
    	if (isSelected == false)
    	{
    		selectedFileCount = 0;
    	}
    	return;
    }
    
    /**
     * 現在のディレクトリを記憶する...
     *
     */
    public void memoryCurrentDirectory()
    {
    	memoryDirectory = showDirectory;
    	return;
    }

    /**
     * データ入力をキャンセルする...
     *
     */
    public void cancelForm()
    {
    	if (currentScene == kaniFileForm.SCENE_TEXTEDITLINE)
    	{
			// 画面シーンをテキスト編集モードに変更する...
    		changeTextEditScreen();
    		return;
    	}

    	showDirectory = memoryDirectory;
		currentScene = SCENE_DEFAULT;
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
    	changeScene(SCENE_DEFAULT, "");
		return;
    }

    /**
     *  テキスト編集モードに戻る...
     *
     */
    public void cancelCommandInputTextEdit()
    {
		// 画面シーンをテキスト編集モードに変更する...
		changeTextEditScreen();
    	return;
    }

    /**
     * 表示画面をキャンセルする...
     *
     */
    public void cancelPictureView()
    {
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		currentScene = SCENE_DEFAULT;
//		(factory.getDirectoryInfo()).setScanDirectoryMode(false);		
		return;
    }

    /**
     * コマンド入力をキャンセルする...
     *
     */
    public void cancelCommandInput()
    {
    	showDirectory = memoryDirectory;
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		currentScene = SCENE_DEFAULT;
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);		
		return;
    }

    /**
     * ディレクトリ選択入力をキャンセルする...
     *
     */
    public void cancelCommandDirInput()
    {
    	showDirectory = changeName;
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		currentScene = SCENE_DEFAULT;
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);		
		return;
    }

    /**
     * 画像表示モードを指定したとき、、
     * @return
     */
    public boolean selectedPictureView()
    {
    	changeScene(kaniFileCanvas.SCENE_PICTUREVIEW, lastName);
    	return (false);
    }
    
    /**
     * ディレクトリ作成指示を受けた...
     * @return
     */
    public boolean selectedMkDir()
    {
    	changeScene(kaniFileForm.SCENE_MAKEDIR, "");
    	return (false);
    }

    /**
     * ファイルの出力..
     * 
     * 
     * @return
     */
    public boolean selectedOutputFileList()
    {
    	if (pickupCopyFile(showDirectory, true) <= 0)
    	{
    		return (false);
    	}
    	String message = "file一覧をfilelist.htmlに(上書き)出力します。";
    	confirmationMode = SCENE_OUTPUT_FILELIST;
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    	return (false);
    }
    
    /**
     * ファイル情報を取得する
     * @param index
     * @return
     */
    public void updateFileDetail(int index)
    {
    	currentIndexNumber = index;
        Thread thread = new Thread()
		{
			public void run()
			{
		    	kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(currentIndexNumber);
		    	info.updateDetailInfo();

		    	// 描画更新指示...
	    		(factory.getCanvas()).updateInformation("", 0);
		    	currentIndexNumber = -1;
			}
		};
		thread.start();
		return;
    }

    private boolean checkSingleTargetFile()
    {
    	if (selectedFileCount != 1)
    	{
    		return (false);
    	}
		if (lastName.equals(".."))
		{
			return (false);
		}
        return (true);    	
    }
    
    /**
     * リネーム指示をうけた...
     * @return
     */
    public boolean selectedRename()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			int pos = lastName.lastIndexOf('/');
			lastName = lastName.substring(0, pos);
		}
    	(factory.getForm()).setTargetName(lastName);
    	changeScene(kaniFileForm.SCENE_RENAMEINPUT, "");
    	return (false);
    }

    /**
     * ファイルコピーが選択された場合...
     * @return
     */
    public boolean selectedCopy()
    {
    	if (pickupCopyFile(showDirectory, false) <= 0)
    	{
    		return (false);
    	}
    	String message = "Copy先Dirで'実行'を押して下さい.";
    	changeName = showDirectory;
		(factory.getDirectoryInfo()).setScanDirectoryMode(true);
    	changeScene(kaniFileCanvas.SCENE_DIRLIST, message);
    	return (false);
    }

    /**
     * コピーするファイル名を取得する
     * @param path      取得ディレクトリ
     * @param isAllFile 全てのファイル
     * @return
     */
    private int pickupCopyFile(String path, boolean isAllFile)
    {
    	memoryFileCount = 0;
    	targetFileList = null;
    	targetFileList = new Vector();
    	int count = (factory.getDirectoryInfo()).getCount();
    	for (int loop = 0; loop < count; loop++)
    	{
    		kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(loop);
            if (info.isDirectory() == false)
            {    		
    		    if ((isAllFile == true)||(info.isSelected() == true))
    		    {
	        	    // コピーファイルとして登録する
    			    String fileName = info.getFileName();
    			    targetFileList.addElement(fileName);
    			    memoryFileCount++;
    		    }
    		}
    	}
    	return (memoryFileCount);
    }

    /**
     * Webブラウザで開く、を指示した場合...
     * 
     * @return
     */
    public boolean selectedOpenWebBrowser()
    {
    	if (checkSingleTargetFile() == false)
    	{
            // ファイルが複数選択されていたときは、何もしない
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}

        // チェック...
		String platForm = System.getProperty("microedition.platform");
        if ((platForm.indexOf("WX34") >= 0)||(platForm.indexOf("WX01K") >= 0))
        {
            // WX340K or WX341Kの時には、ディレクトリを補正する。。。
			String fileName = showDirectory + lastName;
        	if (fileName.indexOf("file:///data/") >= 0)
        	{
        		// データフォルダの場合...
        		switchToWebBrowser("file:///D:/" + fileName.substring(8)); //   "file:///" を置換
        	}
        	else
        	{
        	    // SDカードの場合...
        		switchToWebBrowser("file:///F:/" + fileName.substring(12)); //  "file:///SD:/" を置換
        	}
        }
		else if ((platForm.indexOf("WS023T") >= 0)||(platForm.indexOf("WS018KE") >= 0)||(platForm.indexOf("WS009KE") >= 0))
        {
            // WS023T or WS018KEの時には、ディレクトリを補正する。。。
			String fileName = showDirectory + lastName;
            switchToWebBrowser("file:///affm/nand0/PS/data/" + fileName.substring(19)); // "file:///DataFolder/" を置換
        }
        else if ((platForm.indexOf("WX320K") >= 0)||(platForm.indexOf("WX33") >= 0)||(platForm.indexOf("WX35") >= 0))
        {
            // WX320Kの時には、ディレクトリを補正する。。。
			String fileName = showDirectory + lastName;
            switchToWebBrowser("file://localhost/D:/" + fileName.substring(8));
        }
		else
		{
            // ブラウザで開く
            switchToWebBrowser(showDirectory + lastName);
        }
    	return (false);
    }
    
    /**
     * ファイルコピーを指示された場合...
     * 
     * @return
     */
    public boolean selectedCopyDirectory()
    {
    	String message = memoryFileCount + "ファイルのコピーを実行します。";
    	confirmationMode = SCENE_COPYFILE;
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    	return (false);
    }

    /**
     * ファイル(Dir)削除を指示された場合...
     * 
     * @return
     */
    public boolean selectedDelete()
    {
    	String message = selectedFileCount + "アイテムの削除を実行します。";
    	confirmationMode = SCENE_DELETE;
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    	return (false);
    }


    /**
     *  ファイルのZIP解凍を指示された場合...
     * @return
     */
    public boolean selectedExtractZip(boolean isSameDirectory)
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}
    	(factory.getForm()).setTargetName(lastName);
    	String message = "ZIP展開";
    	if (isSameDirectory == true)
    	{
    		message = message + "(同一Dir)";
    	}
    	message = message + ":" + lastName;
    	if (isSameDirectory == true)
    	{
    		confirmationMode = SCENE_EXTRACT_ZIP_SAMEDIR;
    	}
    	else
    	{
    	    confirmationMode = SCENE_EXTRACT_ZIP;
    	}
    	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);    	
    	return (false);
    }
    
    /**
     * ファイルの複製指示...
     * @return
     */
    public boolean selectedDuplicate()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}
    	(factory.getForm()).setTargetName(lastName);
    	changeScene(kaniFileForm.SCENE_DUPLICATEINPUT, "");
    	return (false);
    }

    /**
     * 選択したファイルを編集する
     * 
     *
     */
    public boolean selectedEdit()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
		if (lastName.endsWith("/"))
		{
			return (false);
		}
		executeEditText();
    	return (false);
    }
    
    /**
     * テキスト編集中にコマンドモードに切り替えた...
     * 
     * @return
     */
    public boolean selectedEditTextCommand()
    {
		// 画面シーンを変更する...
    	(factory.getCanvas()).changeScene(kaniFileCanvas.SCENE_TEXTEDIT_COMMAND, lastName);
    	
		// キャンバスクラスに切り替える...
		Display.getDisplay(parent).setCurrent(factory.getCanvas());

		(factory.getCanvas()).updateInformation("<<< 操作選択 >>>", -1);
    	
		return (true);
    }
    
    /**
     * 選択したファイルの属性を編集する
     * 
     *
     */
    public boolean selectedEditAttribute()
    {
    	if (checkSingleTargetFile() == false)
    	{
    		return (false);
    	}
    	
        Thread thread = new Thread()
		{
			public void run()
			{
                // 選択したファイルのファイル情報を更新して、属性編集画面を開く...
				kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(lastIndex);
		    	info.updateDetailInfo();
		    	(factory.getForm()).setFileInfo(info);
		    	changeScene(kaniFileForm.SCENE_EDITATTRIBUTE, "");
			}
		};
		thread.start();
    	
    	return (false);
    }

    /**
     *  サウンドの再生・停止 (トリガ)
     * 
     */
    public boolean selectedPlayStopSound()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
				// 音を鳴らす
				playSound();
//		    	vibrate(400);
		    	System.gc();

		    	// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());
			}
		};
		thread.start();
		return (false);
    }

    /**
     * 入力結果の反映...
     * @param fileName
     */
    public void executeInputForm(String fileName, int informations)
    {
    	if (currentScene == kaniFileForm.SCENE_MAKEDIR)
    	{
    		// ディレクトリ作成
    		executeMakeDirectory(fileName);
    		return;
    	}
    	if (currentScene == kaniFileForm.SCENE_RENAMEINPUT)
    	{
    		// ファイル名変更
    		executeRenameFile(fileName);
    		return;
    	}
    	if (currentScene == kaniFileForm.SCENE_DUPLICATEINPUT)
    	{
    		// ファイル複製
    		executeDuplicateFile(fileName);
    		return;
    	}
    	if (currentScene == kaniFileForm.SCENE_EDITATTRIBUTE)
    	{
    		// ファイルの属性変更
    		executeEditAttribute(informations);
    		return;
    	}
    	return;
    }

    /**
     * ディレクトリの作成メイン
     * @param fileName
     */
    private void executeMakeDirectory(String fileName)
    {
		lastName = fileName;
        Thread thread = new Thread()
		{
			public void run()
			{
				// ディレクトリを作成する
//				(factory.getFileOperation()).makeDirectory(showDirectory + lastName);
				kaniFileOperations.MakeDirectory(showDirectory + lastName);

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName = null;
			}
		};
		thread.start();
		return;
    }

    /**
     * ファイル名の変更メイン
     * @param fileName
     */
    private void executeRenameFile(String fileName)
    {
		changeName = fileName;
        Thread thread = new Thread()
		{
			public void run()
			{
				// ファイルをリネームする
				(factory.getFileOperation()).renameFile(showDirectory + lastName, changeName);

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
			}
		};
		thread.start();
		return;
    }

    /**
     * ファイルの複製メイン
     * @param fileName
     */
    private void executeDuplicateFile(String fileName)
    {
		changeName = fileName;
        Thread thread = new Thread()
		{
			public void run()
			{
				// ファイルを複製する...
				(factory.getFileOperation()).copyFile(showDirectory + changeName, showDirectory + lastName);

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
			}
		};
		thread.start();
		return;
    }

    /**
     * ファイルの複製メイン
     * @param fileName
     */
    private void executeEditAttribute(int attribute)
    {
    	fileOperationInfo = attribute;
        Thread thread = new Thread()
		{
			public void run()
			{
				boolean hidden   = false;
				boolean writable = true;  // 画面のフラグとwritableフラグは、逆になっていることに注意

				// 書き込み専用ファイル設定をセット (※ 画面のフラグと writableフラグは逆になっていることに注意)
				if ((fileOperationInfo == 1)||(fileOperationInfo == 3))
				{
					writable = false; // writable → false ： 書き込み禁止設定
				}

				// 隠しファイル設定をセット
				if ((fileOperationInfo == 2)||(fileOperationInfo == 3))
				{
					hidden = true;  // hidden → true ： 隠しファイル属性
				}

				// ファイルを複製する...
				(factory.getFileOperation()).changeAttribute((showDirectory + lastName), writable, hidden);

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName          = null;
        		changeName        = null;
        		fileOperationInfo = -1;
        		lastIndex         = -1;
			}
		};
		thread.start();
		return;
    }
    
    /**
     * 削除の実行
     *
     */
    private void executeDelete()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
	        	int count = (factory.getDirectoryInfo()).getCount();
	        	for (int loop = 0; loop < count; loop++)
	        	{
	        		kaniFileDirList.fileInformation info = (factory.getDirectoryInfo()).getFileInfo(loop);
	        		if (info.isSelected() == true)
	        		{
	    	        	// ファイルを削除する
	        			String fileName = showDirectory + info.getFileName();
	    				(factory.getFileOperation()).deleteFile(fileName);
	        		}
	        	}

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
			}
		};
		thread.start();    	
    }

    
    /**
     *  ZIPファイル展開の実行
     * 
     *
     */
    private void executeExtractZip(boolean isSame)
    {
    	zipSame = isSame;
        Thread thread = new Thread()
		{
			public void run()
			{
				// ZIPファイルを展開するロジックを呼び出す
				(factory.getZipOperation()).extractZipFile(showDirectory, lastName, zipSame);

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());
			}
		};
		try
		{
			thread.start();
//			thread.join();
		}
		catch (Exception ex)
		{
			//
		}
    }

    /**
     *  ZIPファイル展開状況を画面描画する
     *
     */
    public void updateProgressForZipExtract()
    {
    	String data = (factory.getZipOperation()).getCurrentStatus();
    	(factory.getCanvas()).updateInformation("ZIP展開中... " + data, -1);
    	(factory.getCanvas()).redraw();
    	return;
    }
    
    /**
     *  ファイル一覧の出力...
     * 
     *
     */
    private void executeOutputFileList()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
				try
				{
			        (factory.getFileOperation()).outputFileListAsHtml((showDirectory + "fileList.html"), targetFileList);
				}
				catch (Exception e)
				{
					// 何もしない...
				}

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
				targetFileList = null;
				System.gc();
			}
		};
		thread.start();
		return;
    }
    
    /**
     * コピーの実行
     *
     */
    private void executeCopy()
    {
        Thread thread = new Thread()
		{
			public void run()
			{
				try
				{
					int count = targetFileList.size();
					for (int loop = 0; loop < count; loop++)
					{
				        String fileName = (String) targetFileList.elementAt(loop);
				        (factory.getFileOperation()).copyFile((showDirectory + fileName), (changeName + fileName));
					}
				}
				catch (Exception e)
				{
					// 何もしない...
				}

				// 画面シーンを変更する...
            	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
        		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
            	
        		// キャンバスクラスに切り替える...
        		Display.getDisplay(parent).setCurrent(factory.getCanvas());

        		lastName   = null;
        		changeName = null;
				targetFileList = null;
				System.gc();
			}
		};
		thread.start();
		return;
    }

    /**
     *  テキスト編集ファイルの編集実行
     * 
     */
    private void executeEditText()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
                // 選択したファイルをメモリに展開する
				if ((factory.getTextDataStorage()).openFile(showDirectory + lastName) > 0)
				{
					// 画面シーンを変更する...
		    		changeTextEditScreen();
				}			
			}
		};
		thread.start();
		
		return;
    }

    /**
     * テキストデータファイルの表示位置取得
     * 
     * @return
     */
    public kaniFileTextFile getTextFileData()
    {
    	return ((factory.getTextDataStorage()));
    }
    
    /**
     * コマンドの実行指示...
     * 
     * @return
     */
    public boolean confirmation()
    {
    	if (confirmationMode == SCENE_DELETE)
    	{
    		// ファイル削除の実行...
    		(factory.getCanvas()).updateInformation("削除中...", -1);
    		executeDelete();
    		return (true);
    	}
    	if (confirmationMode == SCENE_EXTRACT_ZIP)
    	{
    		// ZIPファイル展開の実行...
    		(factory.getCanvas()).updateInformation("ZIP展開中...", -1);
    		executeExtractZip(false);
    		return (true);    		
    	}
    	if (confirmationMode == SCENE_EXTRACT_ZIP_SAMEDIR)
    	{
    		// ZIPファイル展開の実行...
    		(factory.getCanvas()).updateInformation("ZIP展開(同Dir)...", -1);
    		executeExtractZip(true);
    		return (true);
    	}
    	if (confirmationMode == SCENE_COPYFILE)
    	{
    		// ファイルコピーの実行...
    		(factory.getCanvas()).updateInformation("コピー中...", -1);
    		executeCopy();
    		return (true);    		
    	}
    	if (confirmationMode == SCENE_OUTPUT_FILELIST)
    	{
    		// ファイル一覧の出力...
    		(factory.getCanvas()).updateInformation("ファイル一覧出力中...", -1);
    		executeOutputFileList();
    		return (true);    		
    	}
    	if (confirmationMode == SCENE_SAVETEXTFILE)
    	{
    		// テキストファイルの保存実行...
    		(factory.getCanvas()).updateInformation("保存中...", -1);
    		saveTextEditFile();

    		// 画面シーンを変更する...
        	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
    		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
        	
    		// キャンバスクラスに切り替える...
    		Display.getDisplay(parent).setCurrent(factory.getCanvas());

    		return (true);
    	}
    	return (false);
    }

    /**
     *  テキスト編集モードを抜ける
     *
     */
    public void exitTextEdit()
    {
    	if ((factory.getTextDataStorage()).isModified() == true)
    	{
            // ファイル編集時には、、すぐに抜けず、確認メッセージを表示する
//        	String message = "    実行：保存終了    中止：破棄終了";
        	String message = "保存も実行しますか？";
        	confirmationMode = SCENE_SAVETEXTFILE;
        	changeScene(kaniFileCanvas.SCENE_CONFIRMATION, message);
    		return;
    	}

		// 画面シーンを変更する...
    	(factory.getCanvas()).changeScene(SCENE_DEFAULT, null);
		(factory.getDirectoryInfo()).setScanDirectoryMode(false);
    	
		// キャンバスクラスに切り替える...
		Display.getDisplay(parent).setCurrent(factory.getCanvas());

		return;
    }
    
    /**
     * １行のテキスト編集
     * 
     *
     */
    public void selectedTextEditLine()
    {
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	String lineData = new String((factory.getTextDataStorage()).getLine(line));
    	(factory.getForm()).setTargetName(lineData);
    	changeScene(kaniFileForm.SCENE_TEXTEDITLINE, (line + 1) + "");
    	return;
    }
    
    /**
     * データの編集確定 (データの入れ替え)
     * 
     * @param lineData
     */
    public void commitLineEdit(String lineData)
    {
        // １行データの入れ替え
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	(factory.getTextDataStorage()).replaceLine(lineData.getBytes(), line);
    	
    	// テキスト編集モードに切り替える
    	changeTextEditScreen();
    	
    	return;
    }

    /**
     * 画面シーンをテキスト編集モードに変更する
     * 
     *
     */
    private void changeTextEditScreen()
    {
		// 画面シーンをテキスト編集モードに変更する...
    	(factory.getCanvas()).changeScene(kaniFileCanvas.SCENE_TEXTEDIT, lastName);
    	
		// キャンバスクラスに切り替える...
		Display.getDisplay(parent).setCurrent(factory.getCanvas());    	

		return;
    }
    
    /**
     *  編集したテキストファイルを保存する。
     *
     */
    public void saveTextEditFile()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
				// ファイルデータを出力する
		    	(factory.getTextDataStorage()).outputFile(null);

		    	// 画面再描画
		    	(factory.getCanvas()).redraw();
			}
		};
		thread.start();
		return;
    }
    
    /**
     * 一行挿入する
     * 
     * @param isNext
     */
    public void insertLine(boolean isNext)
    {
        // １行挿入
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	if (isNext == true)
    	{
    		line++;
    	}
    	(factory.getTextDataStorage()).insertLine(line);
    }
    
    /**
     * 一行貼り付ける
     *
     */
    public void pasteLine()
    {
        // １行貼り付け
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	(factory.getTextDataStorage()).pasteLine(line);
    }

    /**
     * 1行削除する
     *
     */
    public void deleteLine()
    {
        // １行削除
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	(factory.getTextDataStorage()).deleteLine(line);
    	
    }
    
    /**
     * URLをブラウザで開く...
     *
     */
    public void openUrl(boolean useGoogleProxy)
    {
        // URLを抽出する
    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
    	String url = (factory.getTextDataStorage()).pickupUrl(line);
    	if (url != null)
    	{
    		if (useGoogleProxy == true)
    		{
    			// Googleのプロキシ経由でURLをブラウザで開く設定の場合
                String openUrl = url;
            	if (url.indexOf("www.google.co.jp/m/") < 0)
            	{
            		// google のプロキシを使用する設定でない場合...
                    openUrl = "http://www.google.co.jp/gwt/n?u=http%3A%2F%2F" + url.substring(7);
            	}
            	// Webブラウザで開く...
    		    switchToWebBrowser(openUrl);
    		}
    		else
    		{
                // Webブラウザで開く...
    		    switchToWebBrowser(url);
    		}
    	}
    	System.gc();
    }

    /**
     * URLを取得し、ファイルに記録する(終了後、ブルブルする)
     *
     */
    public void getFileFromUrl()
    {
		Thread thread = new Thread()
		{
			public void run()
			{
			    // URLを抽出する
		    	int line = (factory.getTextDataStorage()).getCurrentSelectedLine();
		    	String url = (factory.getTextDataStorage()).pickupUrl(line);
		    	if (url != null)
		    	{
		            // HTTP通信の実行...
			        (factory.getHttpCommunicator()).doHttpMain(null, url, getCurrentDirectory());
		    	}
		    	vibrate(350);
		    	System.gc();
			}
		};
		thread.start();
    }

    /**
     *  拡張子をチェックし、サウンドを再生する
     *  (サウンドが鳴っていた場合には音を止めてから再生する)
     *
     */
    public void playSound()
    {
        // サウンドを止める(鳴っていた場合には...)
        if ((factory.getUtils()).isPlayingSound() == true)
        {
            (factory.getUtils()).StopSound();
            //return;
        }

        boolean isPlay = false;
        String playType = "audio/midi";
        if ((lastName.indexOf(".mld") > 0)||(lastName.indexOf(".mid") > 0)||
             (lastName.indexOf(".MLD") > 0)||(lastName.indexOf(".MID") > 0))
        {
            playType = "audio/midi";
            isPlay = true;
        }

        if ((lastName.indexOf(".wav") > 0)||(lastName.indexOf(".WAV") > 0))
        {
            playType = "audio/x-wav";
           isPlay = true;
        }
        if (isPlay == true)
        {
        	// サウンドを鳴らす!
            (factory.getUtils()).playSoundFromFile((showDirectory + lastName), playType);
        }
        return;
    }
    
    /**
     * イメージを展開する
     * 
     * @return
     */
    public Image getImage(boolean usingThumbnail)
    {        
        boolean isJpeg = false;
        if ((lastName.indexOf(".jpg") > 0)||(lastName.indexOf(".JPG") > 0))
        {
            isJpeg = true;
            if (usingThumbnail == true)
            {
                return ((factory.getUtils()).loadJpegThumbnailImageFromFile(showDirectory + lastName));
            }
        }

    	Image img = (factory.getUtils()).loadImageFromFile(showDirectory + lastName);
        if ((img == null)&&(isJpeg == true)&&
    	    ((lastName.indexOf(".jpg") > 0)||(lastName.indexOf(".JPG") > 0)))
        {
            // 拡張子 .jpg の場合には、サムネール部分を表示する...
        	System.gc();
            return ((factory.getUtils()).loadJpegThumbnailImageFromFile(showDirectory + lastName));
        }
        return (img);
    }    

	/**
	 *  ブルブルさせる...
	 * 
	 *
	 */
	public void vibrate(int milli)
	{
        Display.getDisplay(parent).vibrate(milli);
		return;
	}

	/**
     *  jadファイルで指定されている値を取得する
     * 
     * @param key jadファイルに書かれているキーワード
     * @return プロパティの値
     */	
	public String getAppProperty(String key)
	{
    	// jadファイルでディレクトリの場所が指定されているか？
    	return (parent.getAppProperty(key));
	}
    
}
