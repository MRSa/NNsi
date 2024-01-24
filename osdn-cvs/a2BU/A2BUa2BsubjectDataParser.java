import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import java.util.Vector;
import java.util.Enumeration;

public class A2BUa2BsubjectDataParser
{

    // スレ管理用データ
    private Vector    subjectVector    = null;
    private String    currentBoardNick = null;

    // スレ状態の定義。
    static public final byte STATUS_NOTYETOLD  = 0;   // 未取得
    static public final byte STATUS_NEW        = 1;   // 新規取得
    static public final byte STATUS_UPDATE     = 2;   // 更新取得
    static public final byte STATUS_REMAIN     = 3;   // 未読あり
    static public final byte STATUS_ALREADY    = 4;   // 既読
    static public final byte STATUS_RESERVE    = 5;   // 取得予約
    static public final byte STATUS_UNKNOWN    = 6;   // スレ状態不明
    static public final byte STATUS_GETERROR   = 7;
    static public final byte STATUS_OVER       = 8;   // 1000超スレ
    static public final byte STATUS_NOTYET     = 10;  // 未取得
    static public final byte STATUS_DEL_OFFL   = 20;
    static public final byte STATUS_NOTSPECIFY = 127; // ステータス未定義

    // クラス...
    private A2BUSceneStorage sceneStorage = null;

    /*
     *   コンストラクタ
     *   
     */
    public A2BUa2BsubjectDataParser(A2BUSceneStorage storageDb)
    {
        sceneStorage = storageDb;
    }

    /**
     *   a2b.idx を読み込み、メモリへ展開する
     *   (以前に読んでいたデータは破棄する)
     */
    public void prepareA2BIndex(String boardNick)
    {
        // 以前に読み込んでいた a2b.idx をリセットする
        resetA2BIndex();

        subjectVector = new Vector();
        currentBoardNick = boardNick;

        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // 読み込むファイル名を生成する
        String indexFile = sceneStorage.getDefaultDirectory() + currentBoardNick + "/" + "a2b.idx";
        try
        {
            connection   = (FileConnection) Connector.open(indexFile, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + 8];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
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
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            buffer = null;
        }

        for (int offset = 0; offset < dataReadSize; offset++)
        {
            int length = 0;
            if (buffer[offset] == 10)
            {
                try
                {
                    // 1行単位でデータを抽出する...
                    length = pickupData(boardNick, buffer, (offset + 1), dataReadSize);
                    offset = offset + length;
                }
                catch (Exception e)
                {
                    // 例外発生...データを読み飛ばすようにする
                    offset = offset + dataReadSize;
                }
            }
        }
        buffer = null;
        System.gc();
        return;
    }
    
    /**
     *   a2b.idx から hashTableへデータを展開する...
     *   (かなりベタベタ...)
     * 
     */
    private int pickupData(String nick, byte[] buffer, int offset, int limit)
    {
        int length = 0;

        // データがないとき...
        if (offset + 13 > limit)
        {
            length = limit - offset + 1;
            return (length);
        }
        
        // コメント行だった、、、１行読み飛ばす
        if (buffer[offset] == 59)
        {
            while (buffer[offset + length] != 10)
            {
                length++;
            }
            return (length);
        }

        int nameOffset = offset + length;
        int nameLength = 0;

        // ファイル名を抽出する
        long threadNumber = 0;
        while ((buffer[nameOffset + nameLength] >= '0')&&(buffer[nameOffset + nameLength] <= '9'))
        {
            threadNumber = threadNumber * 10 + (buffer[nameOffset + nameLength] - '0');
            nameLength++;
        }
        while (buffer[nameOffset + nameLength] != 44)
        {
            nameLength++;
        }

        length = (nameOffset + nameLength + 1) - offset;

        // 最大レス数を抽出
        int maxNumber = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            maxNumber = maxNumber * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // 現在レス数を抽出
        int currentNumber = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            currentNumber = currentNumber * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;
        
        // 現在ステータスを抽出
        int currentStatus = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            currentStatus = currentStatus * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // 最終更新時間
        long lastModified = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            lastModified = lastModified * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;
        
        //　ダミー領域２
        int dummy2 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy2 = dummy2 * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // タイトル名を抽出する
        nameOffset = offset + length;
        nameLength = 0;
        while ((buffer[nameOffset + nameLength] != 0)&&(buffer[nameOffset + nameLength] != 10))
        {
            nameLength++;
        }

        length = (nameOffset + nameLength) - offset;
        while (buffer[offset + length] != 10)
        {
            length++;
        }

        //  データをテーブルに格納する
        subjectDataHolder dataHolder  = new subjectDataHolder(buffer, nameOffset, nameLength);
        dataHolder.threadNumber      = threadNumber;
        dataHolder.numberOfMessage   = maxNumber;
        dataHolder.currentMessage    = currentNumber;
        dataHolder.currentStatus     = (byte) currentStatus;
        dataHolder.lastModified      = lastModified;
        dataHolder.favoriteIndex     = -1;

        int favorIndex =  sceneStorage.isExistFavorite(nick, threadNumber);
        if (favorIndex >= 0)
        {
            // お気に入り登録済みデータ
            dataHolder.favoriteIndex = favorIndex;
            
            // お気に入りに記録されているスレ状態を取得する
            A2BUa2BFavoriteManager.a2BFavoriteThread data = sceneStorage.getFavoriteThreadData(favorIndex);
            try
            {
                if (data != null)
                {
                    // お気に入りデータが更新されていたか？？
                    if (data.lastModified > lastModified)
                    {
                        // スレ状態、レス最大数、現在の読んだ位置を(お気に入りのものに)反映させる
                        dataHolder.currentStatus   = data.currentStatus;
                        dataHolder.numberOfMessage = data.numberOfMessage;
                        dataHolder.currentMessage  = data.currentMessage;
                        dataHolder.lastModified    = data.lastModified;
                    }
                    else
                    {
                        // 通常状態を通常板一覧 → お気に入りへ反映させる
                        data.currentStatus   = dataHolder.currentStatus;
                        data.currentMessage  = dataHolder.currentMessage;
                        data.numberOfMessage = dataHolder.numberOfMessage;
                        data.lastModified    = dataHolder.lastModified;
                    }

                    if (data.numberOfMessage > dataHolder.numberOfMessage)
                    {
                        // a2B.idxをお気に入りの状態に同期させる
                        dataHolder.numberOfMessage = data.numberOfMessage;
                        dataHolder.currentMessage  = data.currentMessage;
                    }
                    else if (data.numberOfMessage < dataHolder.numberOfMessage)
                    {
                        // お気に入りのデータ状態を a2B.idxに同期させる
                        data.currentStatus   = dataHolder.currentStatus;
                        data.currentMessage  = dataHolder.currentMessage;
                        data.numberOfMessage = dataHolder.numberOfMessage;
                        data.lastModified    = dataHolder.lastModified;
                    }
                }
            }
            catch (Exception e)
            {
                //
            }
        }
//        subjectMap.put(fileName, dataHolder);
        subjectVector.addElement(dataHolder);

        return (length);
    }

    /**
     *  読み出したスレの数を応答する
     *  
     * @return
     */
    public int getNumberOfThread()
    {
        if (subjectVector == null)
        {
            return (-1);
        }
        return (subjectVector.size());
    }
    
    /**
     * スレ管理クラスを取得する
     * 
     * 
     */
    public subjectDataHolder getSubjectData(int index)
    {
        subjectDataHolder dataholder = null;
        try
        {
            dataholder = (subjectDataHolder) subjectVector.elementAt(index);
        }
        catch (Exception e)
        {
            return (null);
        }
        return (dataholder);
    }

    /**
     *   インデックス情報をファイル出力し、インデックスデータをクリアする
     *   
     */
    public void outputA2BIndex()
    {
        if ((subjectVector == null)||(currentBoardNick == null))
        {
            // データがないときには、ファイル出力しない
            return;
        }
        FileConnection connection = null;
        OutputStream  out = null;
        
        // 出力ファイル名を生成する
        String indexFile = sceneStorage.getDefaultDirectory() + currentBoardNick + "/" + "a2b.idx";
        try 
        {
            // ファイルの書き込み
            connection = (FileConnection) Connector.open(indexFile, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // ファイル作成
                connection.create();
            }
            else
            {
                // ファイルが存在した場合には、一度消して作り直す
                connection.delete();
                connection.create();
            }
            out = connection.openOutputStream();

            // インデックスデータのコメントを出力する
            String comment = "; fileName, max, current, status, lastModified, 0, (Title)\n";
            out.write(comment.getBytes());

            Enumeration cnt = subjectVector.elements();
            while (cnt.hasMoreElements())
            {
                subjectDataHolder data = (subjectDataHolder) cnt.nextElement();
                if (data.currentStatus != STATUS_NOTYET)
                {
                    String outputData = data.threadNumber + ".dat" + "," + data.numberOfMessage + ",";
                    outputData = outputData + data.currentMessage + "," + data.currentStatus + ",";
                    outputData = outputData + data.lastModified + "," + 0 + ",";
                    out.write(outputData.getBytes());
                    out.write(data.titleName);
                    outputData = null;
                    outputData = "\n";
                    out.write(outputData.getBytes());
                }
            }
            comment = ";[End]";
            out.write(comment.getBytes());
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (out != null)
                {
                    out.close();
                }
            }
            catch (Exception e2)
            {
            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
            
            }
        }
        return;
    }

    /**
     *  読み込んでいる a2b.idx をリセット(クリア)する
     *
     */
    public void resetA2BIndex()
    {
        // スレ一覧インデックスを削除する
        if (subjectVector != null)
        {
            subjectVector.removeAllElements();
        }
        subjectVector    = null;
        currentBoardNick = null;
        
        System.gc();
    }
    
    /**
     *  スレデータを示すクラス
     * 
     */
    public class subjectDataHolder
    {
        private final int MARGIN = 2;
        
        public long    threadNumber    = 0;       // スレ番号
        public long    lastModified    = 0;       // レス状態更新時間
        public int     numberOfMessage = 0;       // レス数
        public int     currentMessage  = 1;       // 現在表示中箇所(レス番号)
        public byte    currentStatus   = 0;       // 現在の状態
        public int     favoriteIndex   = 0;       // お気に入り登録済みかどうか
        private byte[] titleName       = null;   // スレタイトル
        public int     titleSize       = 0;       // スレタイトルサイズ

        // コンストラクタ
        public subjectDataHolder(byte[] name, int offset, int len)
        {
            titleName = new byte[len + MARGIN];
            System.arraycopy(name, offset, titleName, 0, len);
            titleName[len] = 0;
            titleSize = len;
        }

        // タイトル名を取得する
        public byte[] getTitleName()
        {
            return (titleName);
        }
        
        // タイトル名を変更する
        public void changeTitleName(byte[] name, int offset, int len)
        {
            titleName = null;
            titleName = new byte[len + MARGIN];
            System.arraycopy(name, offset, titleName, 0, len);
            titleName[len] = 0;
            titleSize = len;
            System.gc();
        }
    }
}

//--------------------------------------------------------------------------
//  a2BsubjectDataParser  --- スレ一覧データ解析＆保持クラス
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
