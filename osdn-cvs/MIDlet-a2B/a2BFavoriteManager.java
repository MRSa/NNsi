import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;
import java.util.Date;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

//import a2BsubjectDataParser.subjectDataHolder;

/**
 *  お気に入り管理クラス
 * @author MRSa
 *
 */
public class a2BFavoriteManager
{
    // スレ状態の定義。
    public  final byte STATUS_NOTYET   = 0;   // 未取得
    public  final byte STATUS_NEW      = 1;   // 新規取得
    public  final byte STATUS_UPDATE   = 2;   // 更新取得
    public  final byte STATUS_REMAIN   = 3;   // 未読あり
    public  final byte STATUS_ALREADY  = 4;   // 既読
    public  final byte STATUS_RESERVE  = 5;   // 取得予約
    public  final byte STATUS_UNKNOWN  = 6;   // スレ状態不明
    public  final byte STATUS_GETERROR = 7;
    public  final byte STATUS_OVER     = 8;
    public  final byte STATUS_DEL_OFFL = 20;

    // オブジェクト
    private a2BMain         parent        = null;
    private a2Butf8Conveter utf8Converter = null;
    private final String    favoriteFile  = "favorite.idx";
    private final int       bufferMargin  = 16; 
    
    private Vector          favoriteList  = null;
    private boolean         isUpdated     = false;
    private boolean         isBackup      = false;

    /**
     * コンストラクタ
     * @param object     親クラス
     * @param converter  SJ -> utf8変換クラス
     */
    public a2BFavoriteManager(a2BMain object, a2Butf8Conveter converter)
    {
        // 親を記録する...
        parent = object;
        
        // コンバータの登録
        utf8Converter = converter;
        
        favoriteList = new Vector();
    }
    
    /**    
     *  スレがお気に入りに登録済みかチェックする
     *
     */
    public int exist(String nick, long threadNumber)
    {
        int limit = favoriteList.size();
        for (int loop = 0; loop < limit; loop++)
        {
            a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(loop);
            if (data != null)
            {
                if (data.threadNumber == threadNumber)
                {
                    String boardNick = data.getBoardNick();
                    if (boardNick.startsWith(nick) == true)
                    {
                        // お気に入りに登録済み
                        return (loop);
                    }
                }
            }            
        }
        return (-1);
    }
    
    /**
     *  お気に入り登録済みスレの場合、削除する
     *
     */
    public void removeFavorite(String nick, long threadNumber)
    {
        int limit = favoriteList.size();
        for (int loop = 0; loop < limit; loop++)
        {
            a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(loop);
            if (data != null)
            {
                if (data.threadNumber == threadNumber)
                {
                    String boardNick = data.getBoardNick();
                    if (boardNick.startsWith(nick) == true)
                    {
                        // お気に入りから消去する
                        favoriteList.removeElementAt(loop);
                        data = null;
                    
                        // ファイルにバックアップする
                        isUpdated = true;
                        backup();

                        return;
                    }
                }
            }
        }
        return;
    }
/*
    // お気に入り登録済みスレの場合、更新する
    public void updateFavorite(String nick, long threadNumber, String title, int maxMessage, int currentMessage, byte status)
    {
        int limit = favoriteList.size();
        for (int loop = 0; loop < limit; loop++)
        {
            a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(loop);
            if (data != null)
            {
                if (data.threadNumber == threadNumber)
                {
                    String boardNick = data.getBoardNick();
                    if (boardNick.startsWith(nick) == true)
                    {
                        // スレお気に入り状態を更新する
                        if (maxMessage >= 0)
                        {
                            data.numberOfMessage = maxMessage;
                        }                        
                        if (currentMessage >= 0)
                        {
                            data.currentMessage  = currentMessage;
                        }
                        if (status >= 0)
                        {
                            data.currentStatus   = status;
                        }
                        isUpdated = true;
                        return;
                    }
                }
            }            
        }
        return;
    }
*/
    /**
     *   お気に入り情報を展開する
     * 
     */
    public void restore()
    {
        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // ファイル名を生成する
        String fileName = parent.getBaseDirectory() + favoriteFile;
        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + bufferMargin];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
            in = null;
            connection = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(favorite list)", fileName + " EXCEPtion  e:" + e.getMessage());
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

        // データの読み出しメイン部分...
        for (int offset = 0; offset < dataReadSize; offset++)
        {
            int length = 0;
            if ((buffer[offset] == 10)||(buffer[offset] == 0x0a))
            {
                try
                {
                    // 1行単位でデータを抽出する...
                    length = pickupData(buffer, (offset + 1), dataReadSize);
                    offset = offset + length;                    
                }
                catch (Exception e)
                {
                    // 例外発生...
                    offset = offset + dataReadSize;
                }
            }
        }
        buffer = null;
        System.gc();
        return;
    }

    public void doBackup()
    {
        // (念のため)お気に入りのデータをバックアップする
        Thread thread = new Thread()
        {
            public void run()
            {
            	backup();
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
        	// 例外処理としては何もしない。
        }
    }

    /**
     *   お気に入り情報を保管する
     * 
     */
    public void backup()
    {
        // データが更新されていなければ保存しない
        if (isUpdated == false)
        {
            return;
        }

        // バックアップ中には書き込みを行わない
        if (isBackup == true)
        {
            // バックアップができていないことを記録して終了する
            // (次回書き込みタイミングでバックアップする)
            isUpdated = true;
            return;
        }

        // データのバックアップを実行する
        backupMain();

        isUpdated = false;

        System.gc();
        return;
    }
        
    /**
     *   お気に入り情報を保管する
     * 
     */
    private void backupMain()
    {
        isBackup = true;
        FileConnection connection = null;
        OutputStream  out = null;
        
        // ファイル名を生成する
        String fileName = parent.getBaseDirectory() + favoriteFile;
        try 
        {
            // ファイルの書き込み
            connection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
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
            out        = connection.openOutputStream();

            // インデックスデータのコメントを出力する
            String comment = ";; fileName, max, current, status, level, dateTime, 0, 0, url, nick, (Title)\n";
            out.write(comment.getBytes());

            Enumeration cnt = favoriteList.elements();
            while (cnt.hasMoreElements())
            {
                a2BFavoriteThread data = (a2BFavoriteThread) cnt.nextElement();
                if (data != null)
                {
                    String outputData = data.threadNumber + ".dat," + data.numberOfMessage + ",";
                    outputData = outputData + data.currentMessage + "," + data.currentStatus + ",";
                    outputData = outputData + data.threadLevel + "," + data.lastModified + ",0,0,";
                    outputData = outputData + data.getThreadUrl() + "," + data.getBoardNick() + ",";
                    out.write(outputData.getBytes());
                    out.write(data.getThreadTitle());
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
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(output Favorite list)", " Exception  ee:" + e.getMessage());
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
                //
            }
        }
        isBackup = false;
        out = null;
        connection = null;
        System.gc();
        return;        
    }

    /**
     *  キーワードの初期化
     *
     */
    public void clean()
    {
        favoriteList = null;
        System.gc();
    }

    /**
     *  お気に入りの登録数を返す
     *
     */
    public int getNumberOfFavorites()
    {
        return (favoriteList.size());
    }

    /**
     *  お気に入りのスレ番号を応答する
     *
     */
    public long getThreadNumber(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (-1);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.threadNumber);
    }

    /**
     *  お気に入りのスレのレス数を応答する
     *
     */
    public int getNumberOfMessages(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (-1);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.numberOfMessage);
    }

    /**
     *  お気に入りのスレのレス数を応答する
     *
     */
    public int getCurrentMessage(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (-1);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.currentMessage);
    }

    /**
     *  お気に入りのスレレベルを応答する
     *
     */
    public int getThreadLevel(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (-1);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.threadLevel);
    }

    /**
     *  お気に入りのスレ状態を設定する
     *
     */
    public void setStatus(int index, byte status, int number, int maxNumber)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return;
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return;
        }
        if (status >= 0)
        {
            data.currentStatus = status;
        }
        Date curTime = new Date();
        data.lastModified = curTime.getTime();
        
        if (number > 0)
        {
            data.currentMessage = number;
        }
        if (maxNumber > 0)
        {
            data.numberOfMessage = maxNumber;
        }
        isUpdated = true;
        return;
    }
    
    /**
     * スレレベルを更新する
     * 
     * @param index
     * @param level
     */
    public void setThreadLevel(int index, int level)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return;
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return;
        }
        // スレレベルを設定する
        data.threadLevel = level;
        return;
    }

    /**
     *  (仮登録のお気に入りだった場合には)タイトル名を張り替える...
     * 
     * @param index
     * @param title
     */
    public void updateThreadTitle(int index, byte[] title, int offset, int length)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return;
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return;
        }
        String currentTitle = new String(data.getThreadTitle());
        if (currentTitle.indexOf("(a2B_INTERIM)") >= 0)
        {
            // 仮登録のタイトルと判定、タイトル名を張り替える
            data.changeTitleName(title, offset, length);
        }
        return;
    }

    /**
     *  お気に入りのスレ状態を応答する
     *
     */
    public byte getStatus(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (-1);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.currentStatus);
    }

    /**
     *  お気に入りのスレ状態更新時間を応答する
     *
     */
    public long getLastModified(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (0);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (0);
        }
        return (data.lastModified);
    }

    /**
     *  お気に入りのスレURLを応答する
     *
     */
    public String getUrl(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (null);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (null);
        }
        return (data.getThreadUrl());
    }

    /**
     *   お気に入りのスレnickを応答する
     *
     */
    public String getBoardNick(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            return (null);
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            return (null);
        }
        return (data.getBoardNick());
    }

    /**
     *  お気に入りにデータ登録する
     *
     */
    public void entryFavorite(byte[] buffer, String url, String nick, long threadNumber, int maxNumber, int currentNumber, byte currentStatus)
    {
        // データをVector領域に格納する
        a2BFavoriteThread dataHolder    = new a2BFavoriteThread(buffer, url, nick, threadNumber);
        dataHolder.numberOfMessage   = maxNumber;
        dataHolder.currentMessage    = currentNumber;
        dataHolder.currentStatus     = currentStatus;
//        favoriteList.addElement(dataHolder);
        favoriteList.insertElementAt(dataHolder, 0);

        // ファイルにバックアップする
        isUpdated = true;
        backup();
        
        return;
    }
    
    /**
     *   FAVORITE.CSV から Vectorへデータを展開する...
     *   (かなりベタベタベタ...)
     * 
     */
    private int pickupData(byte[] buffer, int offset, int limit)
    {
        int length = 0;

        // データがないとき...
        if (offset + 12 > limit)
        {
            length = limit - offset + 1;
            return (length);
        }
        
        // コメント行だった、、、１行読み飛ばす
        if (buffer[offset] == 59)
        {
            while (((offset + length) < limit)&&(buffer[offset + length] != 10))
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

        // スレレベルを抽出
        int threadLevel = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            threadLevel = threadLevel * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // 情報更新時間を抽出
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

        //　ダミー領域３
        int dummy3 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy3 = dummy3 * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // データ読み飛ばし...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // スレURLを抽出する
        int urlOffset = offset + length;
        int urlLength = 0;
        while ((buffer[urlOffset + urlLength] != 0)&&(buffer[urlOffset + urlLength] != 10)&&(buffer[urlOffset + urlLength] != 44))
        {
            urlLength++;
        }
        String url = new String(buffer, urlOffset, urlLength);

        // ボードニックネームを抽出する
        int nickOffset = urlOffset + urlLength + 1;
        int nickLength = 0;
        while ((buffer[nickOffset + nickLength] != 0)&&(buffer[nickOffset + nickLength] != 10)&&(buffer[nickOffset + nickLength] != 44))
        {
            nickLength++;
        }
        String nick = new String(buffer, nickOffset, nickLength);

        // タイトル名を抽出する
        offset     = nickOffset + nickLength + 1;
        nameOffset = offset;
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

        // データをVector領域に格納する
        a2BFavoriteThread dataHolder    = new a2BFavoriteThread(buffer, nameOffset, nameLength, url, nick, threadNumber);
        dataHolder.numberOfMessage   = maxNumber;
        dataHolder.currentMessage    = currentNumber;
        dataHolder.currentStatus     = (byte) currentStatus;
        dataHolder.threadLevel       = threadLevel;
        dataHolder.lastModified      = lastModified;
        favoriteList.addElement(dataHolder);
        
        // 一応、変数クリア
        url  = null;
        nick = null;

        return (length);
    }

    /**
     *  状態シンボルを取得する
     * 
     */
    private String getStatusSymbol(int status)
    {
        String objName;
        switch (status)
        {
          case 0:  // subjectParser.STATUS_NOTYET:
            objName = " ";
            break;

          case 1:  // subjectParser.STATUS_NEW:
            objName = "*";
            break;

          case 2:  // subjectParser.STATUS_UPDATE:
            objName = "+";
            break;

          case 3:  // subjectParser.STATUS_REMAIN:
            objName = "-";
            break;

          case 4:  // subjectParser.STATUS_ALREADY:
            objName = ":";
            break;

          case 5:  // subjectParser.STATUS_RESERVE:
            objName = "!";
            break;

          case 6:  // subjectParser.STATUS_UNKNOWN:
            objName = "?";
            break;

          case 7:  // subjectParser.STATUS_GETERROR:
            objName = "#";
            break;

          case 8:  // subjectParser.STATUS_OVER:
            objName = " ";
            break;

          default:
            objName = " ";
            break;
        }
        return (objName);
    }


    /**
     * お気に入りスレURLの更新
     * 
     * @return
     */
    public boolean updateURLData(a2BbbsCommunicator bbsAccessor)
    {
        if (bbsAccessor == null)
        {
            return (false);
        }

        // お気に入りスレの全件、URLを更新する 
        // (しかし、、えらい時間がかかるだろうなぁ...)
        Enumeration cnt = favoriteList.elements();
        while (cnt.hasMoreElements())
        {
            parent.keepWatchDog(0);  // かなり時間がかかるので...
            a2BFavoriteThread data = (a2BFavoriteThread) cnt.nextElement();
            if (data != null)
            {
                data.boardUrl = bbsAccessor.getBoardURL(data.getBoardNick());
            }
        }
        backupMain();  // 更新結果をファイル出力する
        return (true);
    }
    
    /**
     *  スレタイトルを取得する
     * 
     */
    public String getSubjectName(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            // 指定レス番号オーバフロー...
            return ("x");
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            // 指定レス番号オーバフロー...
            return (";");
        }
        return (utf8Converter.parsefromSJ(data.getThreadTitle()));
    }
    
    /**
     *   スレタイトル一覧用の表示（スレ状態サマリ）を応答する
     * 
     */
    public String getSubjectSummary(int index)
    {
        if ((index < 0)||(index >= favoriteList.size()))
        {
            // 指定レス番号オーバフロー...
            return (" ");
        }
        a2BFavoriteThread data = (a2BFavoriteThread) favoriteList.elementAt(index);
        if (data == null)
        {
            // 指定レス番号オーバフロー...
            return (" ");
        }

        try
        {
            if ((data.currentStatus == STATUS_NEW)||(data.currentStatus == STATUS_UPDATE))
            {
                return (getStatusSymbol(data.currentStatus) + "[New] " + utf8Converter.parsefromSJ(data.getThreadTitle()));
            }
            else if (data.currentStatus == STATUS_RESERVE)
            {
                return (getStatusSymbol(data.currentStatus) + "[---] " + utf8Converter.parsefromSJ(data.getThreadTitle()));
            }
            else
            {
                return (getStatusSymbol(data.currentStatus) + "[" + data.numberOfMessage + "] " + utf8Converter.parsefromSJ(data.getThreadTitle()));                    
            }
        }
        catch (Exception e)
        {
            // 何もしない
            return ("?");
        }
    }

    /**
     *  お気に入りスレの保持クラス
     * 
     * @author MRSa
     *
     */
    public class a2BFavoriteThread
    {
        private final int MARGIN      = 2;      // あまり領域

        public long   threadNumber     = 0;     // スレ管理番号
        public long   lastModified     = 0;     // 更新時刻
        public int    numberOfMessage  = 0;     // 検出済みレス数
        public int    currentMessage   = 1;     // 参照中レス番号
        public int    threadLevel      = 0;     // スレレベル
//        public int    updated          = 0;     // 更新フラグ
        public byte   currentStatus    = 0;     // スレ参照状態
        
        private String  boardNick      = null;  // スレニックネーム
        private String  boardUrl       = null;  // スレURL
        private byte[] threadTitle     = null;  // スレタイトル
        
        static public final int LEVEL_OTHER = -1;
        static public final int LEVEL_NONE  = 0;   // スレレベルなし
        static public final int LEVEL_1     = 1;   // スレレベル１
        static public final int LEVEL_2     = 2;   // スレレベル２
        static public final int LEVEL_3     = 3;   // スレレベル３
        static public final int LEVEL_4     = 4;   // スレレベル４
        static public final int LEVEL_5     = 5;   // スレレベル５
        static public final int LEVEL_6     = 6;   // スレレベル６
        static public final int LEVEL_7     = 7;   // スレレベル７
        static public final int LEVEL_8     = 8;   // スレレベル８
        static public final int LEVEL_9     = 9;   // スレレベル９

        /**
         * コンストラクタ
         *
         */
        public a2BFavoriteThread(byte[] title, String url, String nick, long number)
        {
            int len     = title.length;
            threadTitle  = new byte[len + MARGIN];
            boardUrl     = new String(url);
            boardNick    = new String(nick);
            System.arraycopy(title, 0, threadTitle, 0, len);

            threadNumber     = number;
            threadTitle[len] = 0;
            threadLevel      = LEVEL_1;
        }

        /**
         * コンストラクタ
         *
         */
        public a2BFavoriteThread(byte[] title, int offset, int len, String url, String nick, long number)
        {
            threadTitle = new byte[len + MARGIN];
            boardUrl    = new String(url);
            boardNick   = new String(nick);
            System.arraycopy(title, offset, threadTitle, 0, len);

            threadNumber     = number;
            threadTitle[len] = 0;
            threadLevel      = LEVEL_1;
        }

        /**
         * スレタイトルを応答する
         *
         */
        public byte[] getThreadTitle()
        {
            return (threadTitle);
        }

        /**
         * スレのURLを応答する
         *
         */
        public String getThreadUrl()
        {
            return (boardUrl);
        }

        /**
         * スレが管理されている板ニックネームを取得する
         *
         */
        public String getBoardNick()
        {
            return (boardNick);
        }

        /**
         * タイトル名を変更する
         *
         */
        public void changeTitleName(byte[] name, int offset, int len)
        {
            threadTitle = null;
            threadTitle = new byte[len + MARGIN];
            System.arraycopy(name, offset, threadTitle, 0, len);
            threadTitle[len] = 0;
            System.gc();
        }
    }
}
//--------------------------------------------------------------------------
//  a2BFavoriteManager  --- お気に入りのデータ管理
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
