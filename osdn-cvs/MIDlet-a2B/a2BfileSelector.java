import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;
import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.List;
import java.util.Vector;

/**
 * ファイル選択クラス (簡易ファイラクラス)
 * 
 * @author MRSa
 *
 */
public class a2BfileSelector extends List implements CommandListener
{
    private a2BMain   parent = null;
    private String    path   = null;
    private String    nextPath = "file:///";
    private String    targetFilePath   = null;
    private String    targetFileName   = null;
    private boolean  isRefreshScreen  = false;
    private boolean  isUpdateTitle    = true;

    private final int COMMAND_EXIT     = -1;
    private final int COMMAND_SELECT   = 0;
    private final int COMMAND_FILEINFO = 1;
    private final int COMMAND_DELETE   = 2;
    private final int COMMAND_RENAME   = 3;
    private final int COMMAND_COPYFILE = 4;
    private final int COMMAND_RESCAN   = 5;
    private final int COMMAND_GETHTTP  = 6;

    private final int COPY_BUFFER_SIZE = 16384;
    private final int FILE_BUFFER_SIZE = 5120;
    
    private Command   exitCmd    = new Command("一覧へ",           Command.EXIT,    COMMAND_EXIT);
    private Command   selectCmd  = new Command("選択",             Command.SCREEN,  COMMAND_SELECT);
    private Command   deleteCmd  = new Command("削除",             Command.ITEM,    COMMAND_DELETE);
    private Command   renameCmd  = new Command("改名",             Command.ITEM,    COMMAND_RENAME);
    private Command   infoCmd    = new Command("プレビュー(dat)",  Command.ITEM,    COMMAND_FILEINFO);
    private Command   copyCmd    = new Command("コピー",           Command.ITEM,    COMMAND_COPYFILE);
    private Command   rescanCmd  = new Command("表示更新",         Command.ITEM,    COMMAND_RESCAN);
    private Command   getHttpCmd = new Command("http受信",         Command.ITEM,     COMMAND_GETHTTP);
    private Vector    itemVector = null;

    /*
     *   コンストラクタ
     * 
     */
    public a2BfileSelector(a2BMain object)
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
                try
                {
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

        // タイトルの表示
        if (isUpdateTitle == true)
        {
            setTitle("wait...一覧更新中");
        }

        // 画面情報をクリアする
        deleteAll();
        itemVector = null;
        itemVector = new Vector();
        this.addCommand(exitCmd);
//        this.addCommand(selectCmd);
        this.addCommand(infoCmd);
        this.addCommand(renameCmd);
        this.addCommand(deleteCmd);
        this.addCommand(copyCmd);
        this.addCommand(getHttpCmd);
        this.addCommand(rescanCmd);
        this.setSelectCommand(selectCmd);
        this.setCommandListener(this);

        // パス情報を更新する
        path = nextPath;

/**
        // ひとつ上の階層にあがるのを追加する（先頭）
        if (!nextPath.equals("file:///"))
        {
            addItem("..", null);
        }
**/

        // ディレクトリ内にあるファイル一覧を(すべて)表示する
        while (dirEnum.hasMoreElements())
        {
            String name = (String)dirEnum.nextElement();
            if (name.toLowerCase().endsWith(".dat"))
            {
                // ファイルがdatファイルの時にはタイトルを解析する
                parent.prepareScratchBuffer((nextPath + name), FILE_BUFFER_SIZE);
                String title = parent.pickupThreadTitle();
                addItem(name, title);                
            }
            else
            {
                // ファイル名をそのまま表示する
                addItem(name, null);
            }
        }

        // ひとつ上の階層にあがるのを追加する（末尾）
        if (!nextPath.equals("file:///"))
        {
            addItem("..", null);
        }

        // タイトルを表示する...
        if (isUpdateTitle == true)
        {
            setTitle(nextPath);
        }
        return;
    }
    
    /**
     * 表示するアイテムを追加する
     * 
     * @param string   ファイル名
     * @param title    タイトル(ファイルがスレの場合)
     */
    private void addItem(String string, String title) 
    {
        if (title != null)
        {
            append(title + " (" + string + ")", null);            
        }
        else
        {
            append(string, null);
        }
        itemVector.addElement(string);
        return;
    }

    /**
     * ファイル名を抽出する
     * 
     * @param data
     * @return
     */
    private String pickupFileName()
    {
        int index = getSelectedIndex();
        if (index < 0)
        {
            return ("");
        }
        String data = (String) itemVector.elementAt(index);
        return (data);
    }

    /**
     *  アイテムを削除したときの処理
     *
     */
    private void deleteItem()
    {
        // ファイル名とパスを設定する
        if (setFileNameAndPath() == false)
        {
            return;
        }
        
        // タイトルの表示...
        String title = "削除： " + targetFilePath + targetFileName;

        Vector commandList = new Vector(2);
        String doDelete  = "削除実行";
        String cancelCmd = "キャンセル";
        commandList.addElement(cancelCmd);
        commandList.addElement(doDelete);

        // ファイル削除を実行するか確認する
        parent.openSelectForm(title, commandList, 0, parent.CONFIRM_DELETELOGFILE);
        commandList = null;

        return;
    }

    /**
     * ファイル名とパスを設定する
     * 
     * @return
     */
    private boolean setFileNameAndPath()
    {
        String name = pickupFileName();
        if (name.equals(".."))
        {
            // 何もしない...
            return (false);
        }

        if (name.endsWith("/"))
        {
            // ディレクトリは改名できない
            return (false);
        }

        targetFilePath = path;
        targetFileName = name;
        return (true);        
    }
    
    /**
     *   アイテムリネーム指示時の処理
     * 
     */
    private void renameItem()
    {
        // ファイル名とファイルパスを設定する
        if (setFileNameAndPath() == false)
        {
            return;
        }

        // ファイル名の入力
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_RENAMEFILE_INPUT, "改名", "新ファイル名", targetFileName, false);
        return;
    }

    /**
     *   httpでファイルを受信する処理
     * 
     */
    private void getHttpFile()
    {
        // ファイルは、今いるディレクトリに受信する...
        targetFilePath = path;
        targetFileName = "";

        Thread thread = new Thread()
        {
            public void run()
            {
                // URLの入力
                parent.OpenWriteFormUsingWordInputMode(parent.SCENE_GETHTTP_INPUT, "http受信", "取得先URL", "http://", false);
            }
        };
        thread.start();

        return;
    }

    /**
     *   アイテムコピー指示時の処理
     * 
     */
    private void copyItem()
    {
        // ファイル名とファイルパスを設定する
        if (setFileNameAndPath() == false)
        {
            return;
        }

        // ファイル名の入力
        parent.OpenWriteFormUsingWordInputMode(parent.SCENE_COPYFILE_INPUT, "コピー", "コピー先ファイル名", targetFileName, false);
        return;
    }

    /**
     * コマンドの実行メイン
     * 
     * @param scene  処理種別
     * @param index  選択アイテム
     */
    public void executeCommandAction(byte scene, int index)
    {
        if (scene == parent.CONFIRM_DELETELOGFILE)
        {
            if (index == 1)
            {
                // ファイルの削除を実施する
                deleteFile(targetFilePath + targetFileName, true);
            }
            return;
        }
        return;
    }
    
    /**
     *    アイテム 選択時の処理
     *   
     */
    private void selection()
    {
        String name = pickupFileName();
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
        }

        if ((name.toLowerCase().endsWith(".txt"))||
            (name.toLowerCase().endsWith(".bak"))||
            (name.toLowerCase().endsWith(".dat"))||
            (name.toLowerCase().endsWith(".csv"))||
            (name.toLowerCase().endsWith(".idx"))||
            (name.toLowerCase().endsWith(".htm"))||
            (name.toLowerCase().endsWith(".html"))||
            (name.toLowerCase().endsWith(".cnf")))
        {
            // 親に対し、選択したテキストファイル名を通知する
            parent.EndFileSelector(nextPath);
        }
        return;
    }

    /**
     *  削除処理の実処理
     * 
     * 
     */
    private void doDeleteMain(String targetFileName)
    {
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() == true)
            {
                // 削除実行
                dataFileConnection.delete();
            }
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
        }
        return;
    }    

    /**
     * ファイル名のリネーム処理メイン
     * 
     * @param destFileName
     * @param srcFileName
     */
    private void doRenameMain(String destFileName, String srcFileName)
    {
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
            if (dataFileConnection.exists() == true)
            {
                // ファイル名の変更実行。。。
                dataFileConnection.rename(destFileName);
            }
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
        }
        return;
    }    
    
    /**
     * ファイル名のコピー処理メイン
     * 
     * @param destFileName
     * @param srcFileName
     */
    public void doCopyFileMain(String destFileName, String srcFileName)
    {
        FileConnection srcFileConnection = null;
        FileConnection dstFileConnection = null;
        
        InputStream   is = null;
        OutputStream  os = null;

        if (destFileName == srcFileName)
        {
            // ファイル名が同じだった場合にはコピーを実行しない
            return;
        }
        
        try
        {
            srcFileConnection = (FileConnection) Connector.open(srcFileName, Connector.READ_WRITE);
            if (srcFileConnection.exists() != true)
            {
                // ファイルが存在しなかった、、、終了する
                srcFileConnection.close();
                return;
            }
            is = srcFileConnection.openInputStream();

            long dataFileSize = srcFileConnection.fileSize();
            byte[] buffer = new byte[COPY_BUFFER_SIZE + 4];

            dstFileConnection = (FileConnection) Connector.open(destFileName, Connector.READ_WRITE);
            if (dstFileConnection.exists() == true)
            {
                // ファイルが存在した、、、削除して作り直す
                dstFileConnection.delete();
            }
            dstFileConnection.create();

            os = dstFileConnection.openOutputStream();
            if ((is != null)&&(os != null))
            {
                while (dataFileSize > 0)
                {
                    int size = is.read(buffer, 0, COPY_BUFFER_SIZE);
                    if (size <= 0)
                    {
                        break;
                    }
                    os.write(buffer, 0, size);
                }
            }
            os.close();
            is.close();
            dstFileConnection.close();
            srcFileConnection.close();

            is = null;
            os = null;
            srcFileConnection = null;
            dstFileConnection = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
            try
            {
                if (is != null)
                {
                    is.close();
                }
                is = null;
                
                if (os != null)
                {
                    os.close();
                }
                os = null;
                
                if (srcFileConnection != null)
                {
                    srcFileConnection.close();
                }
                srcFileConnection = null;

                if (dstFileConnection != null)
                {
                    dstFileConnection.close();
                }
                dstFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
        }
        return;
    }

    
    /**
     *  コマンド選択時の処理
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
        else if (c == deleteCmd)
        {
            // ファイル削除指示時の処理
            deleteItem();
            return;
        }
        else if (c == renameCmd)
        {
            // ファイルリネーム指示
            renameItem();
            return;
        }
        else if (c == getHttpCmd)
        {
            // html通信でファイルを受信
            getHttpFile();
            return;
        }
        else if (c == copyCmd)
        {
            // ファイルコピー指示
            copyItem();
            return;
        }
        else if (c == infoCmd)
        {
            // プレビュー処理を実行する
            doPreviewDat();
            return;
        }
        else if (c == rescanCmd)
        {
            // 再描画指示...
            refreshScreen();
            return;
        }
        return;
    }

    /**
     *  プレビュー処理の実行
     * 
     *
     */
    private void doPreviewDat()
    {
        // プレビュー指示時の処理
        Thread thread = new Thread()
        {
            public void run()
            {
                // ファイル名を取得する
                String name = pickupFileName();
                if (name.equals(".."))
                {
                    // 親ディレクトリを選択したときはプレビューを表示しない(表示できない)
                    return;
                }

                // データファイル名を(フルパスで)表示する
                name = path + name;
                if (name.endsWith("/"))
                {
                    // ディレクトリを選択したときはプレビューを表示しない(表示できない)
                    return;
                }
                parent.doPreviewFile(name, false, a2BsubjectDataParser.PARSE_2chMSG);
            }
        };
        thread.start();
    }
    
    /**
     *  リネームするファイル名が登録されたとき
     * 
     * 
     * @param fileName
     */
    public void doRenameFileName(String fileName)
    {
        // ファイル名を設定する
        targetFilePath = targetFilePath + targetFileName;  // 旧ファイル名 (full path)
        targetFileName = fileName;                         // 新ファイル名 (ファイル名だけ)

        // ファイルの改名を実施する
        Thread thread = new Thread()
        {
            public void run()
            {
                doRenameMain(targetFileName, targetFilePath);
                targetFilePath = null;
                targetFileName = null;
                refreshScreen();
            }
        };
        thread.start();
        return;
    }

    /**
     * HTTPでファイル受信が指示されたとき
     * 
     * @param url
     */
    public void doReceiveHttp(String url)
    {
        String fileName = "index.html";
        int pos = url.lastIndexOf('/');
        if (url.length() > (pos + 1))
        {
            fileName = url.substring(pos + 1);
        }
        
        // ファイル名を設定する
        targetFilePath = targetFilePath + fileName;  // 書き込むファイル名
        targetFileName = url;                        // 取得先URL

        // HTTP通信を実行する。。
        Thread thread = new Thread()
        {
            public void run()
            {
                setTitle("通信中:" + targetFileName);
                isUpdateTitle = false;
                parent.doHttpMain(targetFileName, targetFilePath, null, false, a2BMain.a2B_HTTPMODE_NOUSECOOKIE);
                targetFilePath = null;
                targetFileName = null;
                isUpdateTitle = true;
                refreshScreen();
                System.gc();
                parent.vibrate();
            }
        };
        thread.start();
        return;
    }
        
    /**
     *  コピーするファイル名が登録されたとき
     *  (ファイルコピーを実行)
     * 
     * @param fileName コピー先ファイル名
     */
    public void doCopyFileName(String fileName)
    {
        copyFile(targetFilePath + fileName, targetFilePath + targetFileName, true);
        return;
    }

    /**
     *   画面表示更新時の処理...
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
        try
        {
            setTitle("表示更新中...");
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            // 何もしない...
        }
        return;
    }

    /**
     * ファイルコピーの実行
     * 
     * @param dstFileName コピー先ファイル名 (full path)
     * @param srcFileName コピー元ファイル名 (full path)
     */
    public void copyFile(String dstFileName, String srcFileName, boolean refresh)
    {
        targetFilePath  = dstFileName;
        targetFileName  = srcFileName;
        isRefreshScreen = refresh;

        // ファイルのコピーを実施する
        Thread thread = new Thread()
        {
            public void run()
            {
                if (isRefreshScreen == true)
                {
                    setTitle("wait...コピー中");
                }
                doCopyFileMain(targetFilePath, targetFileName);
                targetFilePath = null;
                targetFileName = null;
                if (isRefreshScreen == true)
                {
                    openDirectory(false);
                }
                isRefreshScreen = false;
            }
        };
        thread.start();        
    }

    /**
     * ファイルを削除する
     * 
     * @param fileName 削除するファイル名 (full path)
     */    
    public void deleteFile(String fileName, boolean refresh)
    {
        targetFileName = fileName;
        isRefreshScreen = refresh;

        Thread thread = new Thread()
        {
            public void run()
            {
                if (isRefreshScreen == true)
                {
                    setTitle("wait...削除中");
                }
                doDeleteMain(targetFileName);
                targetFilePath = null;
                targetFileName = null;
                if (isRefreshScreen == true)
                {
                    openDirectory(false);
                }
                isRefreshScreen = false;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            // 何もしない。。。
        }
        return;
    }

    /**
     *  ファイルサイズを取得する
     * 
     * @param targetFileName 確認するファイル名
     * @return ファイルサイズ
     */
    static public long GetFileSize(String targetFileName)
    {
        long           ret = -1;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.fileSize();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
            ret = -1;
        }
        return (ret);
    }

    /**
     *  ファイルの存在を確認する
     * 
     * @param targetFileName 確認するファイル名
     * @return 存在する(true) / 存在しない (false)
     */
    static public boolean IsExistFile(String targetFileName)
    {
        boolean       ret = false;
        FileConnection dataFileConnection = null;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(targetFileName, Connector.READ);
            ret = dataFileConnection.exists();
            dataFileConnection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (dataFileConnection != null)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e2)
            {
                //
            }
            ret = false;
        }
        return (ret);
    }
}
//--------------------------------------------------------------------------
//  a2BfileSelector  --- ファイル選択
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
