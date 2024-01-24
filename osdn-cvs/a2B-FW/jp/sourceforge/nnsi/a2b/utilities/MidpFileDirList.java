package jp.sourceforge.nnsi.a2b.utilities;
import java.util.Enumeration;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.io.file.FileSystemRegistry;

/**
 * ディレクトリ一覧情報(ディレクトリ内ファイルの一覧情報)を提供するユーティリティ<br>
 * ファイル・ディレクトリの一覧情報だけでなく、ファイルの属性変更も可能です。
 * @author MRSa
 *
 */
public class MidpFileDirList
{
    private String    currentDirectory = "file:///";
    private Vector    scanDirList      = null;
    private long     availableSize    = 0;
    private boolean dirOnlyMode     = false;

    /**
     *   コンストラクタでは何も処理を行いません<br>
     *   （premare() にて利用準備を行います。）
     */
    public MidpFileDirList()
    {
        // 何もしない...
    }

    /**
     *  ユーティリティクラスの使用準備を行います
     * 
     * @param directory 検索を開始するディレクトリ (nullの場合はrootから)
     * @return true(rootディレクトリの検索成功) / false(検索失敗)
     */
    public boolean prepare(String directory)
    {
        return (scanDirectory(directory));
    }

    /**
     * ディレクトリ検索モードを設定します。<br>
     * ディレクトリ一覧でリストされるものが、ディレクトリだけの一覧か、
     * ディレクトリとファイルの一覧にするか決めることができます。<br>
     * 一覧表示がディレクトリだけで良い場合には、isDirectoryModeに
     * trueを入れてください。
     * 
     * @param isDirectoryMode true:ディレクトリだけ/ false:ファイルも含む
     */
    public void setScanDirectoryMode(boolean isDirectoryMode)
    {
        dirOnlyMode = isDirectoryMode;
        return;
    }

    /**
     * ディレクトリをひとつ下に移動します。<br>
     * 指定するときには、ディレクトリ名を示す "/" を文字列の末尾に
     * 格納してください。("root/" といった感じで指定してください。) <br>
     * 
     * @param path 次のディレクトリ名
     * @return true(リスト更新成功) / false(リスト更新失敗)
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
     * ディレクトリをひとつ上に移動します<br>
     * rootディレクトリにいた場合には移動失敗(false)を応答します。
     * 
     * @return true(移動成功) / false(移動失敗)
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
     * ファイル情報クラスを取得します。
     * @param index インデックス番号(0スタート)
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
     * ディレクトリ内にあるファイル数を応答します
     * @return ディレクトリ内にあるファイル数
     */
    public int getCount()
    {
        return (scanDirList.size());
    }

    /**
     * 現在のディレクトリ名を応答します
     * @return 現在のディレクトリ名(full path)
     */
    public String getDirectoryName()
    {
        return (currentDirectory);
    }

    /**
     * ディレクトリのスキャンを行います
     * @param path スキャンするディレクトリ (nullの場合にはrootから実行します)
     * @return true(スキャン成功)/false(失敗)
     */
    public boolean scanDirectory(String path)
    {
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
        System.gc();
        return (true);
    }

    /**
     * ファイル情報クラスを追加する
     * @param path
     * @param name
     * @param data
     */
    private void addItem(String path, String name, String data)
    {
        fileInformation info = new fileInformation(name, this);
//        info.updateDetailInfo();
         scanDirList.addElement(info);
        return;
    }

    /**
     * ファイル属性の変更を行います<br>
     * ファイル情報クラスに格納されている、次の情報を更新します
     * <ul>
     *   <li>最終更新時刻</li>
     *   <li>隠しファイル属性</li>
     *   <li>書き込み属性</li>
     *   <li>ファイルサイズ</li>
     *  </ul>
     * @param info ファイル情報格納クラス
     * @return データ取得成功(true) / 取得失敗(false)
     */
    public boolean updateDetailInfo(fileInformation info)
    {
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
        return (true);
    }

    /**
     * ファイルをすべて選択 / 選択解除します
     * @param isSelected 選択(true) / 選択解除(false)
     * @return 選択したファイル数
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
     * ディスクの空きサイズを応答します
     * @return ディスクの空き領域サイズ
     */
    public long getAvailableSize()
    {
        return (availableSize);
    }
    
    /**
     * ファイル情報を格納するクラスです
     * 
     * @author MRSa
     */
    public class fileInformation
    {
        private MidpFileDirList parent = null;
        private String   fileName      = null;
        private long    dateTime      = -1;
        private long    fileSize      = -1;
        private boolean hiddenFile    = false;
        private boolean writableFile  = true;
        private boolean selected      = false;
        private boolean isDetail      = false;

        /**
         * コンストラクタ
         * @param name   ファイル名
         * @param object 親クラス
         */
        private fileInformation(String name, MidpFileDirList object)
        {
            fileName    = name;
            parent      = object;
        }

        /**
         * ファイル名(ディレクトリ名)を取得します<br>
         * (パスは含まれていません)
         * @return ファイル名
         */
        public String getFileName()
        {
            return (fileName);
        }

        /**
         * アイテムを選択/非選択のマークをつけます<br>
         * (初期状態は非選択です。)
         * 
         * @param sel 選択(true) / 非選択(false)
         */
        public void setSelected(boolean sel)
        {
            selected = sel;
            return;
        }

        /**
         * 選択モードを切り替えます<br>
         * 選択中であれば非選択に、非選択であれば選択中に切り替えます
         *
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
         *   ファイル属性の更新を実行します。
         * <ul>
         *   <li>最終更新時刻</li>
         *   <li>隠しファイル属性</li>
         *   <li>書き込み属性</li>
         *   <li>ファイルサイズ</li>
         *  </ul>
         * 
         * @return データの更新成功(true) / 更新失敗(false)
         */
        public boolean updateDetailInfo()
        {
            isDetail = parent.updateDetailInfo(this);
            return (isDetail);
        }

        /**
         * ファイルサイズを設定します<br><br>
         * 本メソッドでは、ファイルシステムへの反映は行いません。<br>
         * （ファイルシステムへの反映は、updateDetailInfo() にて行います。）
         * @param size ファイルサイズ
         */
        public void setFileSize(long size)
        {
            fileSize = size;
            return;
        }

        /**
         * ファイルサイズを取得します
         * @return ファイルサイズ
         */
        public long getFileSize()
        {
            if (isDetail == false)
            {
                return (-1);
            }                
            return (fileSize);
        }

        /**
         * 時刻（ファイル作成日時）を設定します<br><br>
         * 本メソッドでは、ファイルシステムへの反映は行いません。<br>
         * （ファイルシステムへの反映は、updateDetailInfo() にて行います。）
         * 
         * @param time 時刻
         */
        public void setDateTime(long time)
        {
            dateTime = time;
            return;
        }
 
        /**
         * ファイル作成日時を取得します
         * 
         * @return ファイル作成日時
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
         *  隠しファイルかどうかの設定を行います<br><br>
         * 本メソッドでは、ファイルシステムへの反映は行いません。<br>
         * （ファイルシステムへの反映は、updateDetailInfo() にて行います。）
         * 
         *  @param hidden 隠しファイル(true) / 通常ファイル (false)
         */
        public void setHidden(boolean hidden)
        {
            hiddenFile = hidden;
            return;
        }
      
        /**
         * 隠しファイルかどうかの設定を取得します
         * 
         *  @return 隠しファイル(true) / 通常ファイル (false)
         */
        public boolean isHidden()
        {
            return (hiddenFile);
        }

        /**
         * 書き込み可否を設定します<br><br>
         * 
         * 本メソッドでは、ファイルシステムへの反映は行いません。<br>
         * （ファイルシステムへの反映は、updateDetailInfo() にて行います。） 
         * @param writable 書き込み許可(true) / 書き込み禁止(false)
         */
        public void setWritable(boolean writable)
        {
            writableFile = writable;
            return;
        }

        /**
         * 書き込み可否状態を取得します
         * @return 書き込み可否 書き込み許可(true) / 書き込み禁止(false)
         */
        public boolean isWritable()
        {
            return (writableFile);
        }
        
        
        /**
         * ディレクトリかどうかを取得します
         * 
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
         * ファイルが選択中かどうかを取得します
         * 
         * @return ファイル選択中(true)/非選択状態(false)
         */
        public boolean isSelected()
        {
            return (selected);
        }
    }
}
