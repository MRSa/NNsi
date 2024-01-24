import java.io.InputStream;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 *  BBSデータ管理クラス...
 * 
 * @author MRSa
 *
 */
public class A2BUa2BbbsCommunicator
{
    private String    baseDirectory = null;
    private Hashtable bbsList       = null;
    private Hashtable usingBbsList  = null;
    private Vector    getBbsList    = null;

    private final String FILE_BBSTABLE = "bbstable.html";  // 板一覧のファイル名
    private final int    MARGIN = 16;
    
    /**
     *   コンストラクタ
     *
     */
    public A2BUa2BbbsCommunicator(String directory)
    {
        baseDirectory = directory;
        bbsList = new Hashtable();
        usingBbsList = new Hashtable();
    }

    /**
     *   板一覧データベースを作成(準備)する
     *   
     */
    public void prepare()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // 板一覧データベースの構築
                updateBbsDatabase();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            // 何もしない...
        }
        System.gc();
        return;
    }
    
    /**
     *   (板Nickディレクトリが存在しなければ)ディレクトリを作成する
     *  
     */
    public boolean checkBoardNickName(String nickName)
    {
        boolean ret = true;
        FileConnection fc = null;
        String directory = baseDirectory + nickName;
        try 
        {
            fc = (FileConnection) Connector.open(directory, Connector.READ_WRITE);
            if (fc.exists() == false)
            {
                fc.mkdir();
                ret = false;
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
                // 何もしない...
            }
            return (false);
        }
        return (ret);
    }

    /**
     *   登録済み板数を応答する
     *   
     */
    public int getNumberOfBoardName()
    {
        return (getBbsList.size());
    }
    
    /**
     *  板Indexから、URLを取得する
     * @param boardIndex
     * @return
     */
    public String getBoardUrl(int boardIndex)
    {
        String boardUrl = null;
        try
        {
            bbsDataHolder holder = (bbsDataHolder) getBbsList.elementAt(boardIndex);
            if (holder != null)
            {
                boardUrl = holder.getBoardUrl();
            }
        }
        catch (Exception ex)
        {
            return ("");
        }
        return (boardUrl);
    }

    /**
     * 板Nickから、URLを取得する...
     * 
     * @param boardNick
     * @return
     */
    public String getBoardURL(String boardNick)
    {
        bbsDataHolder holder = (bbsDataHolder) usingBbsList.get(boardNick);
        if (holder == null)
        {
            return ("");
        }
        return (holder.getBoardUrl());
    }

    /**
     *  使用している板としてマークする
     * 
     * @param boardNick
     * @return
     */
    public boolean markUsingBbs(String boardNick)
    {
    	String key = boardNick.toLowerCase();
        bbsDataHolder holder = (bbsDataHolder) bbsList.get(key);
        if (holder == null)
        {
            return (false);
        }
        usingBbsList.put(boardNick, holder);
        return (true);
    }
    
    /**
     *  板一覧を使用するものだけに縮小する
     * 
     * @return
     */
    public int trimBbsList()
    {
        bbsList = null;
        getBbsList = new Vector();
        for (Enumeration e = usingBbsList.elements() ; e.hasMoreElements(); )
        {
            getBbsList.addElement(e.nextElement());
        }
        return (getBbsList.size());
    }

    /**
     *   カテゴリ内板一覧データベースの（無理やり）構築処理
     *   
     */
    private void parseBbsDatabase(byte[] buffer, int bufferSize, int offset)
    {
        int loop;

        // カテゴリ、ボードURL、ボード名を抽出する。
        for (loop = 0; loop < bufferSize; loop++)
        {
            // 板一覧DB作成開始...
            if ((buffer[loop] == 'h')&&(buffer[loop + 1] == 't')&&(buffer[loop + 2] == 't')&&(buffer[loop + 3] == 'p'))
            {
                // URL検出!
                int index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '>')&&(buffer[loop + index] != 34)&&(buffer[loop + index] != 39))
                {
                    index++;
                }
                // URL名を取り込む！
                String boardUrl = new String(buffer, loop, (index - 1));
                loop = loop + index;
                if (buffer[loop] != '>')
                {
                    // 想定外のデータ...
                    continue;
                }
                loop++;

                // 板Nickを抽出する
                String boardNick = convertBoardNick(boardUrl);
                if (boardNick != null)
                {
                    // データをVector領域に格納する
                    boardNick = boardNick + "/";
                    bbsDataHolder dataHolder    = new bbsDataHolder(boardNick, boardUrl);
                    bbsList.put(boardNick, dataHolder);
                }
                
                // ボード名称を検出する
                index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '<'))
                {
                    index++;
                }
                loop = loop + index;
            }
        }
        return;
    }

    /**
     *  板Nickを生成する
     * @param boardUrl
     * @return
     */
    public String convertBoardNick(String boardUrl)
    {
        // 取得先および取得ファイル名を生成する
        int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);
        if (pos < 7)
        {
            // '/' が "http://" のところにマッチするようだったら、板Nickではない、と判断する
            return ("");
        }

        // 板ニックネームを決定する
        return (boardUrl.substring(pos + 1));
    }    
    
    /**
     *   板一覧データベースを作成する...
     *   
     */
    private void updateBbsDatabase()
    {
        FileConnection  bbsFileConnection  = null;
        InputStream     in                 = null;
        String          fileName           = baseDirectory + FILE_BBSTABLE;    
        byte[]         buffer             = null;

        try
        {
            bbsFileConnection = (FileConnection) Connector.open(fileName, Connector.READ);

            // データ読み出し準備...
            int dataFileSize = (int) bbsFileConnection.fileSize();
            int dataReadSize = 0;
            buffer = new byte[dataFileSize + MARGIN];

            // ファイルからデータを読み出す...
            in = bbsFileConnection.openInputStream();
            dataReadSize = in.read(buffer, 0, dataFileSize);            

            // リソースをクローズする
            in.close();
            bbsFileConnection.close();

            // データの解析を実施する
            parseBbsDatabase(buffer, dataReadSize, 0);
        }
        catch (Exception e)
        {
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // 
            }
            try
            {
                if (bbsFileConnection != null)
                {
                    bbsFileConnection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }    
        }
        in = null;
        bbsFileConnection = null;
        buffer = null;
        return;
    }

    /**
     *     板情報を保持するクラス
     *
     */
    public class bbsDataHolder
    {
        private String boardUrl  = null;
        private String boardNick = null;    
        /**
         *  コンストラクタ
         * @param url
         */
        public bbsDataHolder(String nick, String url)
        {
            boardUrl = url;
            boardNick = nick;
        }

        /**
         * 板nickを抽出する
         * @return
         */
        public String getBoardNick()
        {
            return (boardNick);
        }

        /**
         *  板URLを取得する
         * @return
         */        
        public String getBoardUrl()
        {
            return (boardUrl);
        }
    }
}
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
//
