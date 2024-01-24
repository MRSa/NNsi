import java.io.InputStream;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;
import java.util.Date;

public class a2BsubjectDataParser
{
    // 漢字コード指定...
    static final int          PARSE_JIS       = 4;
    static final int          PARSE_UTF8      = 3;
    static final int          PARSE_EUC       = 2;
    static final int          PARSE_SJIS      = 1;    
    static final int          PARSE_2chMSG    = 0;
    static final int          PARSE_TOGGLE    = -1;

    // 関連オブジェクト
    private a2BMain            parent          = null;
    private a2BbbsCommunicator bbsCommunicator = null;
    private a2Butf8Conveter    utf8Converter   = null;
    private a2BFavoriteManager favorManager    = null;

    // スレ管理用データ
    private Hashtable          subjectMap            = null;
    private Vector             subjectVector         = null;
    private int               subjectListCount      = 0;

    private byte[]            subjectTxt            = null;
    private int               subjectTxtSize        = 0;

    private int[]             subjectTxtOffsetArray = null;
    private int               subjectTitles         = 0;

    private boolean          useOnlyDatabase       = false;
    private boolean          dataExtracted         = false;
    private int               favoriteLevel         = -1;

    // スレ状態の定義。
    static public  final byte       STATUS_NOTYETOLD  = 0;   // 未取得
    static public  final byte       STATUS_NEW        = 1;   // 新規取得
    static public  final byte       STATUS_UPDATE     = 2;   // 更新取得
    static public  final byte       STATUS_REMAIN     = 3;   // 未読あり
    static public  final byte       STATUS_ALREADY    = 4;   // 既読
    static public  final byte       STATUS_RESERVE    = 5;   // 取得予約
    static public  final byte       STATUS_UNKNOWN    = 6;   // スレ状態不明
    static public  final byte       STATUS_GETERROR   = 7;
    static public  final byte       STATUS_OVER       = 8;   // 1000超スレ
    static public  final byte       STATUS_NOTYET     = 10;  // 未取得
    static public  final byte       STATUS_DEL_OFFL   = 20;
    static public  final byte       STATUS_NOTSPECIFY = 127; // ステータス未定義

    // バッファのマージン領域...
    private final int        readBufferSize    = 10240;  // 仮読み用のバッファサイズ
    private final int        bufferMargin      = 8;
//    private final int        subjectManageSize = 3000;  // スレ管理可能の最大サイズ
    private final int        subjectManageSize = 1200;  // スレ管理可能の最大サイズ
    private final int        indexReadSize     = 120;   // インデックス読み出しサイズ

    // ワーク領域...
    private String            gettingLogDataFile = null;
    private String            gettingLogDataURL  = null;
    private String            deleteLogDataFile  = null;
    private String            boardNick          = null;

    /*
     *   コンストラクタ
     *   
     */
    public a2BsubjectDataParser(a2BMain object, a2BbbsCommunicator communicator, a2Butf8Conveter converter, a2BFavoriteManager favor)
    {
        parent                = object;
        bbsCommunicator       = communicator;
        favorManager          = favor;

        utf8Converter         = converter;
        subjectMap            = new Hashtable();
        subjectVector         = new Vector();
        subjectTxtOffsetArray = new int[subjectManageSize + bufferMargin];        
    }

    /**
     * スレ一覧を更新し準備する
     * 
     * @param boardIndex
     */
    public void prepareSubjectListUpdate(int boardIndex)
    {
//      // スレ一覧情報を初期化し、再構築する場合... (強制再取得)
//        // スレ情報を a2B.idxへ出力する
//        outputSubjectIndex(boardIndex);
//
//        // スレインデックスの解析を実施する
//        parseSubjectIndex(boardIndex);

        // スレ一覧を取得する
        getSubjectTxt(boardIndex);
        return;
    }
    
    /*
     *   スレ一覧を準備する
     * 
     */
    public void prepareSubjectList(int boardIndex)
    {
        // スレインデックスの解析を実施する
        parseSubjectIndex(boardIndex);

        // 取得URLと更新ファイル名の確定
        decideFileNameAndUrl(boardIndex, "subject.txt", false);

        // スレ一覧の解析を実施する
        parseSubjectTxt();

        // タイトル一覧構築完了の通知
        parent.readySubjectListDisplay();
        return;
    }

    /*
     *   スレ一覧の解析処理...
     * 
     * 
     */
    public void formatSubjectList()
    {
        // スレ一覧の解析を実施する
        parseSubjectTxt();

        // タイトル一覧構築完了の通知
        parent.readySubjectListDisplay();
        return;
    }

    /**
     *  スレ情報を出力・クリアする (板切り替え・終了時に呼び出す)
     * 
     */
    public void leaveSubjectList(int boardIndex)
    {
        // スレ情報を a2B.idxへ出力する
        outputSubjectIndex(boardIndex);
        boardNick = null;
        return;
    }

    /**
     * スレ管理クラスを取得する
     * 
     * 
     */
    private subjectDataHolder getSubjectDataHolder(int index)
    {
        // 取得済み設定スレ一覧の場合...
        if (useOnlyDatabase == true)
        {
            subjectDataHolder dataholder = null;
            try
            {
                dataholder = (subjectDataHolder) subjectVector.elementAt(index);
//              subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
//                String key = data.threadNumber + ".dat";
//              dataholder = (subjectDataHolder) subjectMap.get(key);
            }
            catch (Exception e)
            {
                return (null);
            }
            return (dataholder);
        }        

        // 通常一覧の場合。。。
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = getThreadFileName(index);
            dataHolder = (subjectDataHolder) subjectMap.get(fileName);
        }
        catch (Exception e)
        {
            return (null);
        }
        return (dataHolder);
    }
    
    /*
     *   スレタイトル一覧用の表示（スレ状態サマリ）を応答する
     * 
     * 
     */
    public String getSubjectSummary(int index)
    {
        String headerString = "";
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        try
        {
            if (dataHolder != null)
            {
                if (dataHolder.entryFavorite > 0)
                {
                    headerString = "}";
                }
                headerString = headerString + getStatusSymbol(dataHolder.currentStatus);
                if ((dataHolder.currentStatus == STATUS_NEW)||(dataHolder.currentStatus == STATUS_UPDATE))
                {
                    return (headerString + "[New] " + getSubjectName(index));
                }
                else if (dataHolder.currentStatus == STATUS_RESERVE)
                {
                    return (headerString + "[---] " + getSubjectName(index));
                }
                else
                {
                    return (headerString + "[" + dataHolder.numberOfMessage + "] " + getSubjectName(index));                    
                }
            }
            int  resNum = getNofMessage(index);
            byte status = getStatus(index);
            if (isFavorite(index) == true)
            {
                headerString = "}";
            }
            headerString = headerString + getStatusSymbol(status);
            if ((status == STATUS_NEW)||(status == STATUS_UPDATE))
            {
                return (headerString + "[NEW] " + getSubjectName(index));
            }
            else if (status == STATUS_RESERVE)
            {
                return (headerString + "[---] " + getSubjectName(index));
            }
            else if (resNum == 0)
            {
                return ("");
            }
            else
            {
                return (headerString + "[" + resNum + "] " + getSubjectName(index));
            }
        }
        catch (Exception e)
        {
            // 何もしない
        }
        return ("");
    }

    /*
     *   スレ一覧の保持数を応答する
     *
     */
    public int getNumberOfSubjects()
    {
        if (useOnlyDatabase == true)
        {
            return (subjectMap.size());
        }
        return (subjectTitles);
    }
    
    /*
     *   スレ一覧タイトルを取得する
     *   
     */
    public String getSubjectName(int index)
    {
        if (useOnlyDatabase == true)
        {
            subjectDataHolder dataHolder = getSubjectDataHolder(index);
            if (dataHolder == null)
            {
                return ("");
            }
            return (utf8Converter.parsefromSJ(dataHolder.getTitleName()));
        }
        
        if ((subjectTxtOffsetArray == null)||(subjectTxt == null))
        {
            return ("");
        }
        
        if ((index >= subjectTitles)||(index < 0))
        {
            return (" ");
        }
        int offset = subjectTxtOffsetArray[index];
        while (subjectTxt[offset] != '>')
        {
            offset++;
        }
        offset++;
        int length = subjectTxtOffsetArray[index + 1] - offset;

        while ((length > 0)&&(subjectTxt[offset + length] != '('))
        {
            length--;
        }
        if (subjectTxt[offset + (length - 1)] == ' ')
        {
            length--;
        }
        if (length <= 0)
        {
            return ("  ");
        }
        return (utf8Converter.parsefromSJ(subjectTxt, offset, length));
    }

    /*
     *  現在表示しているレス数を取得する
     *
     */
    public int getCurrentMessage(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder != null)
        {
            return (dataHolder.currentMessage);
        }
        return (1);
    }

    /*
     *  スレの全レス数を取得する
     *
     */
    public int getNumberOfMessages(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder != null)
        {
            return (dataHolder.numberOfMessage);
        }
        return (1);
    }

    /*
     *   現在表示レス数と最大レス数を設定
     *   
     */
    public void setCurrentMessage(int index, int number, int maxNumber)
    {
        //
        byte status = STATUS_ALREADY;
        if (number < maxNumber)
        {
            status = STATUS_REMAIN;
        }
        if (number > 1000)
        {
            status = STATUS_OVER;
        }
        if (number <= 0)
        {
            status = STATUS_RESERVE;
        }
        if (maxNumber < 0)
        {
            status = STATUS_NEW;
        }
        
        // お気に入りデータ表示モードかどうか確認する
        if (parent.getFavoriteShowMode() == true)
        {
            // お気に入りデータだった場合、こちらの情報を更新
            favorManager.setStatus(index, status, number, maxNumber);
            return;
        }

        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        try
        {
            if (dataHolder != null)
            {
                dataHolder.currentMessage  = number;
                dataHolder.numberOfMessage = maxNumber;
                dataHolder.currentStatus   = status;
                Date curTime = new Date();
                dataHolder.lastModified = curTime.getTime();
                if (dataHolder.entryFavorite != 0)
                {
                    // お気に入りデータだった場合には、そちらも更新
                    int favoriteIndex = favorManager.exist(boardNick, dataHolder.threadNumber);
                    if (favoriteIndex >= 0)
                    {
                        favorManager.setStatus(favoriteIndex, status, number, maxNumber);
                    }
                    
                }
                return;
            }
        }
        catch (Exception e)
        {
            // 何もしない
        }

        // 新規登録
        entryThreadDataHolder(index, status, number, maxNumber);
        return;
    }

    /*
     *   現在表示レス数と最大レス数を設定
     *   
     */
    public void setCurrentMessage(long threadNumber, int number, int maxNumber)
    {
        //
        byte status = STATUS_ALREADY;
        if (number < maxNumber)
        {
            status = STATUS_REMAIN;
        }
        if (number > 1000)
        {
            status = STATUS_OVER;
        }
        if (number <= 0)
        {
            status = STATUS_RESERVE;
        }
        if (maxNumber < 0)
        {
            status = STATUS_NEW;
        }
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = threadNumber + ".dat";
            dataHolder = (subjectDataHolder) subjectMap.get(fileName);
            if (dataHolder != null)
            {
                dataHolder.currentMessage  = number;
                dataHolder.numberOfMessage = maxNumber;
                dataHolder.currentStatus   = status;
                Date curTime = new Date();
                dataHolder.lastModified = curTime.getTime();

                return;
            }
        }
        catch (Exception e)
        {
            // 何もしない
        }
        return;
    }

    /*
     *  データベースのみアクセスするモードかどうかの状態を設定する
     * 
     */
    public void setOnlyDatabaseAccess(boolean isDatabase)
    {
        useOnlyDatabase = isDatabase;
        parent.setOnlyDatabaseAccess(isDatabase);
        return;
    }    
    
    /*
     *  データベースのみアクセスするモードかどうかの状態を取得する
     * 
     */
    public boolean getOnlyDatabaseAccess()
    {
        return (useOnlyDatabase);
    }

    /*
     *   現在表示レス状態を取得
     *   
     */
    public byte getStatus(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder == null)
        {
            return (STATUS_NOTYET);
        }
        return (dataHolder.currentStatus);
    }

    /*
     *   現在表示レスがお気に入りに登録されているかチェックする
     *   
     */
    public boolean isFavorite(int index)
    {
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder == null)
        {
            return (false);
        }
        if (dataHolder.entryFavorite != 0)
        {
            return (true);
        }
        return (false);
    }
    
    /*
     *   現在表示レス状態を設定
     *  
     */
    public void setStatus(int index, byte status)
    {
        if ((subjectTitles <= index)||(index < 0))
        {
            // 指定レス番号オーバフロー...
            return;
        }
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        try
        {
            if (dataHolder != null)
            {
                Date curTime = new Date();
                dataHolder.lastModified = curTime.getTime();
                dataHolder.currentStatus = status;
                if (dataHolder.entryFavorite != 0)
                {
                    // お気に入りに登録済みのスレ、お気に入りデータも状態を更新する
                    int favorIndex = favorManager.exist(boardNick, dataHolder.threadNumber);
                    if (favorIndex >= 0)
                    {
                        // お気に入りのデータ状態を a2B.idxに同期させる
                        favorManager.setStatus(favorIndex, dataHolder.currentStatus, dataHolder.currentMessage, dataHolder.numberOfMessage);
                    }
                }
                return;
            }
        }
        catch (Exception e)
        {
            // 何もしない
        }
        entryThreadDataHolder(index, status, 1, -1);
        return;
    }

    /*
     *   スレファイル名を取得する
     * 
     * 
     */
    public String getThreadFileName(int index)
    {
        long threadNumber = getThreadNumber(index);
        if (threadNumber == 0)
        {
            return ("");
        }
         return (threadNumber + ".dat");
    }

    /**
     *  ファイルを削除する
     * 
     * @param datFileName
     */
    private void deleteFile(String datFileName)
    {
        // ファイル名を作成する
        deleteLogDataFile = parent.getBaseDirectory() + datFileName;

        // スレ一覧の取得
        Thread thread = new Thread()
        {
            public void run()
            {
                FileConnection    connection = null;
                try
                {
                    // ログファイルの削除を実行！！
                    connection = (FileConnection) Connector.open(deleteLogDataFile, Connector.READ_WRITE);
                    if (connection.exists() == true)
                    {
                        // ファイルが存在した場合には、ファイルを削除する
                        connection.delete();
                    }
                    connection.close();            
                }
                catch (Exception e)
                {
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
                deleteLogDataFile = null;
            }
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception e)
        {
            //
        }
    }
    
    /*
     *  ログファイルを削除する
     * 
     */
    public void deleteLogDataFile(int index, String datFileName)
    {
        if (index < 0)
        {
            deleteFile(datFileName);
            return;
        }
        
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = getThreadFileName(index);
            dataHolder = (subjectDataHolder) subjectMap.remove(fileName);
            if (dataHolder != null)
            {
                // Hashtable/Vectorから削除する...
                subjectVector.removeElement(dataHolder);
                dataHolder = null;

                // ファイルを削除する
                deleteFile(datFileName);
            }
        }
        catch (Exception e)
        {
            //
        }

        // ログファイルがなかったため、削除しない...
        return;    
    }
    
    /*
     *   スレデータクラスを登録する
     * 
     */
    private void entryThreadDataHolder(int index, byte status, int currentMessage, int maxMessage)
    {
         if ((subjectTitles <= index)||(index < 0))
        {
            // 指定レス番号オーバフロー...
            return;
        }

        subjectDataHolder dataHolder   = null;

        // スレタイトルからレス数を取得する
        long    threadNumber = getThreadNumber(index);
        int     resNumber    = getNofMessage(index);
        String   fileName     = threadNumber + ".dat";
        int     length       = getSubjectDataTitleLength(index);
        int     top          = subjectTxtOffsetArray[index];
        int     end          = subjectTxtOffsetArray[index+ 1] - 1;
        int     offset       = top;
        while ((offset < end)&&(subjectTxt[offset] != '>'))
        {
            offset++;
        }
        offset++;

        //
        if (currentMessage < 0)
        {
            currentMessage = 1;
        }

        //
        if (maxMessage < 0)
        {
            maxMessage = resNumber;
        }
        
        //  データをテーブルに格納する
        dataHolder                   = null;
        dataHolder                   = new subjectDataHolder(subjectTxt, offset, length);
        dataHolder.threadNumber      = threadNumber;
        dataHolder.numberOfMessage   = maxMessage;
        dataHolder.currentMessage    = currentMessage;
        dataHolder.currentStatus     = status;
        subjectMap.put(fileName, dataHolder);
        subjectVector.addElement(dataHolder);
        dataExtracted = true;
        return;
    }

    /**
     * スレタイトルの長さを抽出する
     * 
     * @param index
     * @return
     */
    private int getSubjectDataTitleLength(int index)
    {
        if ((subjectTitles <= index)||(index < 0))
        {
            // 指定レス番号オーバフロー...
            return (0);
        }

        int     offset        = subjectTxtOffsetArray[index];
        int     endOffset     = subjectTxtOffsetArray[index+ 1] - 1;
        while ((offset < endOffset)&&(subjectTxt[offset] != '>'))
        {
            offset++;
        }
        offset++;
        
        while ((offset < endOffset)&&(subjectTxt[endOffset] != '('))
        {
            endOffset--;
        }
        endOffset--;

        return (endOffset - offset);
    }
    
    /**
     *   現在表示中のスレ番号を取得する
     *   
     */
    public long getThreadNumber(int index)
    {
        if (useOnlyDatabase == true)
        {
            if (index < 0)
            {
                return (0);
            }
            subjectDataHolder dataHolder = getSubjectDataHolder(index);
            if (dataHolder == null)
            {
                return (0);
            }
            return (dataHolder.threadNumber);
        }        

        if ((subjectTitles <= index)||(index < 0))
        {
            // 指定レス番号オーバフロー...
            return (0);
        }
    
        // スレ未取得の場合には、追加する
        long number = 0;
        int offset = subjectTxtOffsetArray[index];
        while ((subjectTxt[offset] >= '0')&&(subjectTxt[offset] <= '9'))
        {
            number = number * 10 + (subjectTxt[offset] - '0');
            offset++;
        }
        return (number);
    }

    /**
     *  スレのレス数を取得する
     * 
     * 
     */
    private int getNofMessage(int index)
    {
        // インデックス番号異常チェック
        if ((subjectTitles <= index)||(index < 0))
        {
            // 指定レス番号オーバフロー...
            return (0);
        }
        if (useOnlyDatabase == true)
        {
            subjectDataHolder dataHolder = getSubjectDataHolder(index);
            if (dataHolder == null)
            {
                return (0);
            }
            return (dataHolder.numberOfMessage);
        }            

        // ぬるぽ回避...
        if ((subjectTxtOffsetArray == null)||(subjectTxt == null))
        {
            return (0);
        }
        
        // スレタイトルからレス数を取得する
        int offset    = subjectTxtOffsetArray[index];
        int endOffset = subjectTxtOffsetArray[index+ 1] - 1;
        while ((offset < endOffset)&&(subjectTxt[endOffset] != '('))
        {
            endOffset--;
        }
        endOffset++;
        int number = 0;
        while ((subjectTxt[endOffset] >= '0')&&(subjectTxt[endOffset] <= '9'))
        {
            number = number * 10 + (subjectTxt[endOffset] - '0');
            endOffset++;
        }
        return (number);
    }

    /**
     *   ファイル名とURLを決める
     * 
     * 
     */
    private boolean decideFileNameAndUrl(int boardIndex, String fileName, boolean checkDirectory)
    {
        boolean ret = checkDirectory;
        
        // 現在のカテゴリと板名を表示する
        gettingLogDataURL = bbsCommunicator.getBoardURL(boardIndex);

        // 板ニックネームを決定する
        boardNick = bbsCommunicator.getBoardNickName(gettingLogDataURL);

        // ディレクトリチェックモードを確認
        if (checkDirectory == true)
        {
            // ディレクトリがなければ作成する
            ret = bbsCommunicator.checkBoardNickName(boardNick);
        }

        // ファイル名の生成
        gettingLogDataFile = boardNick + fileName;

        // URL名の確定...
        gettingLogDataURL = gettingLogDataURL + fileName;

        return (ret);
    }
    
    /**
     *   スレ一覧の取得処理
     *
     */
    private void getSubjectTxt(int boardIndex)
    {
        // 取得URLと更新ファイル名の確定
        decideFileNameAndUrl(boardIndex, "subject.txt", true);

        // スレ一覧の取得を実施
        Thread thread = new Thread()
        {
            public void run()
            {
                // バックアップ
                if (parent.getBackupSubjectTxt() == true)
                {
                    // ファイルをバックアップしてから通信を行う...
                    parent.doCopyFile(parent.getBaseDirectory() + gettingLogDataFile + ".bak", parent.getBaseDirectory() + gettingLogDataFile);
                }
                parent.GetHttpCommunication(gettingLogDataFile, gettingLogDataURL, null, "", 0, 0, parent.SCENE_GETSUBJECTLIST, false);
            }
        };
        thread.start();            

        return;
    }

    /**
     *    スレ一覧データをファイルからバッファに読み出し、インデックスを張る
     *
     *
     */
    private boolean parseSubjectTxt()
    {
        FileConnection connection = null;
        InputStream    in         = null;
        byte[] subjectTxtTmp     = null;

        if (gettingLogDataFile == null)
        {
            // ファイルデータ取得直後ではない、、解析失敗...
            return (false);
        }

        // subject.txtをファイルから読み出す...
        try 
        {
            // ファイルオープン準備...
            connection = (FileConnection) Connector.open( parent.getBaseDirectory() + gettingLogDataFile, Connector.READ);

            // データ読み出し準備...
            int dataFileSize   = (int) connection.fileSize();
            if ((dataFileSize == 0)&&(parent.getBackupSubjectTxt() == true))
            {
                // 通信失敗？？ ファイルサイズがゼロだった...
                connection.close();
                
                // バックアップしたファイルを書き戻す...
                parent.doCopyFile(parent.getBaseDirectory() + gettingLogDataFile, parent.getBaseDirectory() + gettingLogDataFile + ".bak");

                // ファイルオープン準備...
                connection = (FileConnection) Connector.open( parent.getBaseDirectory() + gettingLogDataFile, Connector.READ);

                // データ読み出し準備...
                dataFileSize   = (int) connection.fileSize();
            }
            subjectTxt          = null;
            subjectTxtSize      = 0;
            subjectTxtTmp       = new byte[dataFileSize + bufferMargin];
            
            // ファイルの読み出しメイン...
            in = connection.openInputStream();
            subjectTxtSize = in.read(subjectTxtTmp, 0, dataFileSize);
            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", gettingLogDataFile + " Exception  ee:" + e.getMessage());
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // 何もしない
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
                // 何もしない
            }
            gettingLogDataFile = null;        
            return (false);
        }
        
        // subject.txtを解析する...
//        int index  = 0;
//        subjectTxtOffsetArray[index] = 0;
//        index++;
//        for (int offset = 0; ((offset < subjectTxtSize)&&(index <= subjectManageSize)); offset++)
//        {
//            // 改行コードがある場所を見つける...
//            if (subjectTxt[offset] == 10)
//            {
//                subjectTxtOffsetArray[index] = offset + 1;
//                index++;
//            }
//        }
//        subjectTxtOffsetArray[index] = subjectTxtSize; // 予防策...
//        subjectTitles = (index - 1);

        int index = 1;
        int subjectTxtSizeTmp = subjectTxtSize; 
        // テキストを初めから読んでいく
        for (int offset = 0; offset < subjectTxtSizeTmp; offset++)
        {
            // 改行コードがある場所を見つける...
            if (subjectTxtTmp[offset] == 10)
            {
                offset++;
                subjectTxtOffsetArray[index] = offset;
                index++;
                if (index == subjectManageSize)
                {
                    break;
                }
            }
        }

        // オブジェクト変数の参照先を変更
        subjectTxt = subjectTxtTmp;

        subjectTxtOffsetArray[index - 1] = subjectTxtSize; // 予防策...
        subjectTitles = (index - 1);

        // ファイル名は用済みなのでクリアする
        gettingLogDataFile = null;

        System.gc();
        return (true);
    }

    /**
     *  インデックス情報数を確認する
     * 
     */
    public int currentSubjectIndexSize()
    {
        return (subjectMap.size());
    }

    /**
     *   インデックス情報をファイル出力し、インデックスデータをクリアする
     *   
     */
    private void outputSubjectIndex(int boardIndex)
    {
        if (dataExtracted != true)
        {
            // データがないときには、ファイル出力しない
            return;
        }
        FileConnection connection = null;
        OutputStream  out = null;
        
        // 取得先および取得ファイル名を生成する
        String url = bbsCommunicator.getBoardURL(boardIndex);
        int pos = url.lastIndexOf('/', url.length() - 2);
        // 板ニックネームを決定する
        boardNick = url.substring(pos + 1);
        String indexFile = parent.getBaseDirectory() + boardNick + "a2b.idx";
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

            Enumeration cnt = subjectMap.elements();
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
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " Exception  ee:" + e.getMessage() + " index:" + boardIndex);
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

        // スレ一覧インデックスを削除する
        subjectMap.clear();
        subjectVector.removeAllElements();
        dataExtracted = false;

        subjectTxt     = null;
        subjectTxtSize = 0;
        subjectTitles  = 0;
//        boardNick      = null;
        System.gc();
        return;
    }

    /**
     *   スレ一覧をインデックスファイルから展開する...
     *   
     */
    private void parseSubjectIndex(int boardIndex)
    {
        // スレ一覧インデックスファイルが構築済みの場合には、そのまま戻る...
        dataExtracted = true;
        if (subjectMap.size() != 0)
        {
            return;
        }

        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // 取得先および取得ファイル名を生成する
        String url = bbsCommunicator.getBoardURL(boardIndex);
        int pos = url.lastIndexOf('/', url.length() - 2);
        boardNick = url.substring(pos + 1);
        String indexFile = parent.getBaseDirectory() + boardNick + "a2b.idx";
        try
        {
            connection   = (FileConnection) Connector.open(indexFile, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + bufferMargin];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " EXCEption  e:" + e.getMessage());
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

        // ファイル名をストリング化する
        String fileName = new String(buffer, nameOffset, nameLength);
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

        int favorIndex = favorManager.exist(nick, threadNumber);
        if (favorIndex >= 0)
        {
            // お気に入り登録済みデータ
            dataHolder.entryFavorite = 1;
            
            // お気に入りに記録されているスレ状態を取得する
            int sts    = favorManager.getStatus(favorIndex);
            int nofMsg = favorManager.getNumberOfMessages(favorIndex);
            int curMsg = favorManager.getCurrentMessage(favorIndex);
            long lastModifiedFavor = favorManager.getLastModified(favorIndex);

            // お気に入りデータが更新されていたか？？
            if (lastModifiedFavor > lastModified)
            {
                // スレ状態、レス最大数、現在の読んだ位置を(お気に入りのものに)反映させる
                dataHolder.currentStatus = (byte) sts;
                dataHolder.numberOfMessage   = nofMsg;
                dataHolder.currentMessage    = curMsg;
                dataHolder.lastModified      = lastModifiedFavor;
            }
            else
            {
                // 通常状態を通常板一覧 → お気に入りへ反映させる
                favorManager.setStatus(favorIndex, dataHolder.currentStatus, dataHolder.currentMessage, dataHolder.numberOfMessage);
            }

            if (nofMsg > dataHolder.numberOfMessage)
            {
                // a2B.idxをお気に入りの状態に同期させる
                dataHolder.numberOfMessage = nofMsg;
                dataHolder.currentMessage = curMsg;
            }
            else if (nofMsg < dataHolder.numberOfMessage)
            {
                // お気に入りのデータ状態を a2B.idxに同期させる
                favorManager.setStatus(favorIndex, dataHolder.currentStatus, dataHolder.currentMessage, dataHolder.numberOfMessage);
            }
        }
        subjectMap.put(fileName, dataHolder);
        subjectVector.addElement(dataHolder);

        return (length);
    }
    
    /**
     *   お気に入りの登録を解除する
     * 
     */
    public void removeFavorite(String nick, long threadNumber)
    {
        subjectDataHolder dataHolder = null;
        try
        {
            String fileName = threadNumber + ".dat";
            dataHolder = (subjectDataHolder) subjectMap.get(fileName);
            if (dataHolder == null)
            {
                return;
            }
            dataHolder.entryFavorite = 0;
        }
        catch (Exception e)
        {
            // 何もしない
            return;
        }
        return;
    }
    
    /**
     *   お気に入りの登録/登録解除を実施する
     * 
     */
    public void setResetFavorite(int index, String url, String nick, boolean mode)
    {
        setResetFavorite(getSubjectDataHolder(index), url, nick, mode);
        return;
    }

    /**
     *   お気に入りの登録/登録解除を実施する
     * 
     * @param dataHolder
     * @param url
     * @param nick
     * @param mode
     */
    private boolean setResetFavorite(subjectDataHolder dataHolder, String url, String nick, boolean mode)
    {
        if (dataHolder == null)
        {
            return (false);
        }
        long threadNumber = dataHolder.threadNumber;
        if (favorManager.exist(nick, threadNumber) < 0)
        {
            // お気に入り登録を行う
            if (mode == true)
            {
                favorManager.entryFavorite(dataHolder.getTitleName(), url, nick, threadNumber, dataHolder.numberOfMessage, dataHolder.currentMessage, dataHolder.currentStatus);
                dataHolder.entryFavorite = 1;
            }
        }
        else
        {
            // お気に入り登録を解除する
            if (mode == false)
            {
                favorManager.removeFavorite(nick, threadNumber);
                dataHolder.entryFavorite = 0;
            }
        }
        return (true);
    }

    /**
     *   新着確認の準備を実施する
     * 
     * 
     */
    public void prepareGetLogList(int favorLevel)
    {
        favoriteLevel = favorLevel;
        if (favoriteLevel < 0)
        {
            subjectListCount = subjectMap.size();
        }
        else
        {
            subjectListCount = favorManager.getNumberOfFavorites();
        }

        // 新着確認実施準備完了を報告する
        parent.readyGetLogList();
        return;    
    }

    /**
     * 
     * 
     * 
     */
    public long getThreadNumberGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (-1);
        }
        if (favoriteLevel >= 0)
        {
            return (favorManager.getThreadNumber(index));
        }
        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.threadNumber);
    }

    /**
     * 
     * 
     * 
     */
    public int getThreadResNumberGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (-1);
        }
        if (favoriteLevel >= 0)
        {
            int resNumber = favorManager.getNumberOfMessages(index);
            return (resNumber);
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return (-1);
        }
        return (data.numberOfMessage);
    }
    
    
    /**
     * 
     * 
     * 
     */
    public byte getThreadStatusGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (STATUS_NOTYET);
        }
        if (favoriteLevel >= 0)
        {
            byte status = (byte) favorManager.getStatus(index);
            return (status);
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return (STATUS_NOTYET);
        }
        return (data.currentStatus);
    }

    /**
     * スレタイトルを取得する
     * 
     * 
     */
    public String getSubjectNameGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return ("");
        }
        if (favoriteLevel >= 0)
        {
            return (favorManager.getSubjectName(index));
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        if (data == null)
        {
            return ("");
        }
        return (utf8Converter.parsefromSJ(data.getTitleName()));
    }

    /**
     *   URLを取得する
     * 
     */
    public String getFavoriteUrlGetLogList(int index)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return (null);
        }
        if (favoriteLevel < 0)
        {
            return (null);
        }
        if (favorManager.getNumberOfMessages(index) > 1000)
        {
            return (null);
        }
        int level = favorManager.getThreadLevel(index);
        if ((favoriteLevel == 0)||(favoriteLevel == level))
        {
            return (favorManager.getUrl(index));
        }
        return (null);
    }
    
    /**
     * 
     * 
     */
    public void setGetLogStatus(int index, byte status)
    {
        if ((index >= subjectListCount)||(index < 0))
        {
            return ;
        }
        if (favoriteLevel >= 0)
        {
            // お気に入りの状態更新...
            favorManager.setStatus(index, status, -1, -1);
            if ((favorManager.getBoardNick(index)).equals(boardNick) == true)
            {
                // ここの(通常)データも状態を更新する
                String key = favorManager.getThreadNumber(index) + ".dat";
                try
                {
                    subjectDataHolder dataHolder = (subjectDataHolder) subjectMap.get(key);
                    if (dataHolder != null)
                    {
                        dataHolder.currentStatus = status;
                        return;
                    }
                }
                catch (Exception e)
                {
                    // 何もしない
                }
            }
            return;
        }

        subjectDataHolder data = (subjectDataHolder) subjectVector.elementAt(index);
        String key = data.threadNumber + ".dat";
        try
        {
            subjectDataHolder dataHolder = (subjectDataHolder) subjectMap.get(key);
            if (dataHolder != null)
            {
                dataHolder.currentStatus = status;
            
                if (dataHolder.entryFavorite != 0)
                {
                    // お気に入りデータだった場合には、そちらも更新
                    int favoriteIndex = favorManager.exist(boardNick, dataHolder.threadNumber);
                    if (favoriteIndex >= 0)
                    {
                        favorManager.setStatus(favoriteIndex, status, -1, -1);
                    }    
                }
                return;
            }
        }
        catch (Exception e)
        {
            // 何もしない
        }
        return;
    }

    /**
     * 
     * 
     * 
     */
    public void clearGetLogList()
    {
        if (favoriteLevel >= 0)
        {
            // お気に入り情報を保存する
            favorManager.backup();
        }
        favoriteLevel    = -1;
        subjectListCount =  0;
        System.gc();
        return;
    }
    /**
     *  データプレビューを実施する。
     * 
     */
    public void previewMessage(byte[] targetData, boolean useForm, int parseMode)
    {
        byte[] data = targetData;
        
        if (parseMode == a2BsubjectDataParser.PARSE_EUC)
        {
            data = new byte[targetData.length * 2];
            utf8Converter.StrCopyEUCtoSJ(data, 0, targetData, 0, targetData.length);
        }
        else
        {
            data = targetData;
        }

        // 超簡単なメッセージの解析...
        byte[] parsedData = new byte[data.length];
        int tokenCount = 0;
        int srcIndex = 0;
        int dstIndex = 0;
        while (srcIndex < data.length)
        {
            // 1レスの最後を検出した場合には抜ける...
            if ((parseMode == PARSE_2chMSG)&&(data[srcIndex] == (byte) 0x0a))
            {
                break;
            }
            else if (data[srcIndex] == (byte) '<')
            {
                // タグは読み飛ばす...
                srcIndex++;
                if (data[srcIndex] == (byte) '>')
                {
                    tokenCount++;
                    if (tokenCount >= 3)
                    {
                        // 3個目以降の <> は、改行に置換する
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                    }
                    else
                    {
                        // 3個までの <> は、スペース１個に置換する
                        parsedData[dstIndex] = (byte) ' ';                        
                    }
                    dstIndex++;
                    srcIndex++;
                }
                else if (((data[srcIndex] == (byte) 'b')||(data[srcIndex] == (byte) 'B'))&&
                    ((data[srcIndex + 1] == (byte) 'r')||(data[srcIndex + 1] == (byte) 'R')))
                {
                    parsedData[dstIndex] = (byte) '\r';
                    dstIndex++;
                    parsedData[dstIndex] = (byte) '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 2;
                    while ((srcIndex < data.length)&&(data[srcIndex] != (byte) '>'))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                }
                else if (((data[srcIndex] == (byte) 'b')||(data[srcIndex] == (byte) 'B'))&&
                        ((data[srcIndex + 1] == (byte) 'o')||(data[srcIndex + 1] == (byte) 'O'))&&
                        ((data[srcIndex + 2] == (byte) 'd')||(data[srcIndex + 2] == (byte) 'D'))&&
                        ((data[srcIndex + 3] == (byte) 'y')||(data[srcIndex + 3] == (byte) 'Y'))&&
                        (data[srcIndex + 4] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 5;
                }
                else if (((data[srcIndex] == (byte) 'h')||(data[srcIndex] == (byte) 'H'))&&
                        ((data[srcIndex + 1] == (byte) 'e')||(data[srcIndex + 1] == (byte) 'E'))&&
                        ((data[srcIndex + 2] == (byte) 'a')||(data[srcIndex + 2] == (byte) 'A'))&&
                        ((data[srcIndex + 3] == (byte) 'd')||(data[srcIndex + 3] == (byte) 'D'))&&
                        (data[srcIndex + 4] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 5;
                }
                else if (((data[srcIndex] == (byte) '/'))&&
                        ((data[srcIndex + 1] == (byte) 'b')||(data[srcIndex + 1] == (byte) 'B'))&&
                        ((data[srcIndex + 2] == (byte) 'o')||(data[srcIndex + 2] == (byte) 'O'))&&
                        ((data[srcIndex + 3] == (byte) 'd')||(data[srcIndex + 3] == (byte) 'D'))&&
                        ((data[srcIndex + 4] == (byte) 'y')||(data[srcIndex + 4] == (byte) 'Y'))&&
                        (data[srcIndex + 5] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 6;
                        break;
                }
                else if (((data[srcIndex] == (byte) '/'))&&
                        ((data[srcIndex + 1] == (byte) 'h')||(data[srcIndex + 1] == (byte) 'H'))&&
                        ((data[srcIndex + 2] == (byte) 't')||(data[srcIndex + 2] == (byte) 'T'))&&
                        ((data[srcIndex + 3] == (byte) 'm')||(data[srcIndex + 3] == (byte) 'M'))&&
                        ((data[srcIndex + 4] == (byte) 'l')||(data[srcIndex + 4] == (byte) 'L'))&&
                        (data[srcIndex + 5] == (byte) '>'))
                {
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 6;
                        break;
                }
                else
                {
                    if (((data[srcIndex + 0] == (byte) 'h')||(data[srcIndex + 0] == (byte) 'H'))||
                         ((data[srcIndex + 0] == (byte) 'p')||(data[srcIndex + 0] == (byte) 'P'))||
                         ((data[srcIndex + 0] == (byte) 'd')||(data[srcIndex + 0] == (byte) 'D'))||
                         ((data[srcIndex + 0] == (byte) 'a')||(data[srcIndex + 0] == (byte) 'A'))||
                         ((data[srcIndex + 0] == (byte) 'l')||(data[srcIndex + 0] == (byte) 'L')))
                    {
                        // 改行コードを入れる
                        parsedData[dstIndex] = (byte) '\r';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 1;
                    }
//                    parsedData[dstIndex] = (byte) ' ';
//                    dstIndex++;
                    while ((srcIndex < data.length)&&(data[srcIndex] != (byte) '>'))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                }
            }
            else if (data[srcIndex] == (byte) '&')
            {
                srcIndex++;
                if (((data[srcIndex + 0] == (byte) 'n')||(data[srcIndex + 0] == (byte) 'N'))&&
                        ((data[srcIndex + 1] == (byte) 'b')||(data[srcIndex + 1] == (byte) 'B'))&&
                        ((data[srcIndex + 2] == (byte) 's')||(data[srcIndex + 2] == (byte) 'S'))&&
                        ((data[srcIndex + 3] == (byte) 'p')||(data[srcIndex + 3] == (byte) 'P'))&&
                        (data[srcIndex + 4] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) ' ';
                    dstIndex++;
                    srcIndex = srcIndex + 5;
                }
                else if (((data[srcIndex + 0] == (byte) 'a')||(data[srcIndex + 0] == (byte) 'A'))&&
                        ((data[srcIndex + 1] == (byte) 'm')||(data[srcIndex + 1] == (byte) 'M'))&&
                        ((data[srcIndex + 2] == (byte) 'p')||(data[srcIndex + 2] == (byte) 'P'))&&
                        (data[srcIndex + 3] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) '&';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                }
                else if (((data[srcIndex + 0] == (byte) 'l')||(data[srcIndex + 0] == (byte) 'L'))&&
                        ((data[srcIndex + 1] == (byte) 't')||(data[srcIndex + 1] == (byte) 'T'))&&
                        (data[srcIndex + 2] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) '<';
                    dstIndex++;
                    srcIndex = srcIndex + 3;
                }
                else if (((data[srcIndex + 0] == (byte) 'g')||(data[srcIndex + 0] == (byte) 'G'))&&
                        ((data[srcIndex + 1] == (byte) 't')||(data[srcIndex + 1] == (byte) 'T'))&&
                        (data[srcIndex + 2] == (byte) ';'))
                {
                    parsedData[dstIndex] = (byte) '>';
                    dstIndex++;
                    srcIndex = srcIndex + 3;
                }
                else
                {
                    // 通常の一文字転写...
                    parsedData[dstIndex] = data[srcIndex];
                    dstIndex++;
                    srcIndex++;
                }
            }
            else
            {
                // 通常の一文字転写...
                parsedData[dstIndex] = data[srcIndex];
                dstIndex++;
                srcIndex++;
            }
        }
        // プレビューデータの表示
        String previewData = "";
        if (parseMode == PARSE_2chMSG)
        {
            previewData = utf8Converter.parsefromSJ(parsedData, 0, dstIndex);
            int pos = previewData.lastIndexOf('\n');
            previewData = previewData.substring(pos + 1) + "\r\n=====\r\n" + previewData.substring(0, pos);
            parsedData  = null;
            if (useForm == false)
            {
                parent.showDialog(parent.SHOW_INFO, 0, "プレビュー", previewData);
            }
            else
            {
                parent.OpenWriteFormUsingPreviewMode("プレビュー", previewData);
            }
            previewData = null;
        }
        else if (parseMode == PARSE_UTF8)
        {
            try
            {
                previewData = new String(parsedData, 0, dstIndex, "UTF-8");
            }
            catch (Exception e)
            {
                previewData = new String(parsedData);
            }
            parsedData  = null;
            if (useForm == false)
            {
                parent.showDialog(parent.SHOW_INFO, 0, "プレビュー", previewData);
            }
            else
            {
                parent.OpenWriteFormUsingPreviewMode("プレビュー", previewData);
            }
            previewData = null;
        }
        else //  if (parseMode == PARSE_SJIS)
        {
            previewData = utf8Converter.parsefromSJ(parsedData, 0, dstIndex);
            parsedData  = null;
            parent.OpenWriteFormUsingTextEditMode("データ", previewData);
            previewData = null;
        }
        System.gc();
        return;
    }

    /**
     *  スレタイトルを抽出して応答する
     * 
     */
    public String pickupThreadTitle(byte[] data)
    {
        // 超簡単なメッセージの解析...
        byte[] parsedData = new byte[data.length];
        int tokenCount = 0;
        int srcIndex = 0;
        int dstIndex = 0;
        while (srcIndex < data.length)
        {
            // 1レスの最後を検出した場合には抜ける...
            if (data[srcIndex] == (byte) 0x0a)
            {
                break;
            }
            else if (data[srcIndex] == (byte) '<')
            {
                // タグは読み飛ばす...
                srcIndex++;
                if (data[srcIndex] == (byte) '>')
                {
                    tokenCount++;
                    if (tokenCount >= 3)
                    {
                        // 3個目以降の <> は、改行に置換する
                        parsedData[dstIndex] = (byte) '\n';
                        dstIndex++;
                        parsedData[dstIndex] = (byte) '\n';
                    }
                    else
                    {
                        // 3個までの <> は、スペース１個に置換する
                        parsedData[dstIndex] = (byte) ' ';                        
                    }
                    dstIndex++;
                    srcIndex++;
                }
                else if (((data[srcIndex] == (byte) 'b')||(data[srcIndex] == (byte) 'B'))&&
                    ((data[srcIndex + 1] == (byte) 'r')||(data[srcIndex + 1] == (byte) 'R'))&&
                    (data[srcIndex + 2] == (byte) '>'))
                {
                    parsedData[dstIndex] = (byte) '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 3;
                }
                else
                {
                    parsedData[dstIndex] = (byte) ' ';
                    dstIndex++;
                    while ((srcIndex < data.length)&&(data[srcIndex] != (byte) '>'))
                    {
                        srcIndex++;
                    }
                }
            }
            else
            {
                // 通常の一文字転写...
                parsedData[dstIndex] = data[srcIndex];
                dstIndex++;
                srcIndex++;
            }
        }
        
        // スレタイトルを抽出する
        String previewData = utf8Converter.parsefromSJ(parsedData, 0, dstIndex);
        int pos = previewData.lastIndexOf('\n');
        if (pos < 0)
        {
            // タイトルの抽出に失敗...
            return ("");
        }
        return (previewData.substring(pos + 1));
    }

    /**
     *  状態シンボルを取得する
     * 
     */
    private String getStatusSymbol(byte status)
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
     *   スレ番号指定解析処理の実施
     *      ※ ここには、(nick)/(スレ番号) という情報が送られてくる
     * 
     *      - boardNick と (nick) が一致した場合、、、そのまま取得予約
     *      - (nick)が違った場合、 (nick)/a2b.idx を解析し、対象スレがなければ取得予約を追加する
     *      - (nick)が違った場合、
     * 
     * @param threadNumber
     */
    public boolean parseMessageThreadNumber(String threadNumber)
    {
        int pos = threadNumber.indexOf("/");
        if (pos <= 0)
        {
            // '/'の検出失敗、、、終了する
            return (false);
        }
        String threadNick = threadNumber.substring(0, (pos + 1));
        long number       = Long.parseLong(threadNumber.substring(pos + 1));
        if (boardNick.equals(threadNick) == true)
        {
            // 板が一致した、、そのまま取得予約を実施する
            String datFileName = number + ".dat";
            subjectDataHolder dataHolder = null;
            try
            {
                dataHolder = (subjectDataHolder) subjectMap.get(datFileName);
                if (dataHolder != null)
                {
                    // 既に取得済みスレだった、、、取得予約する
                    if (dataHolder.currentStatus >= STATUS_UNKNOWN)
                    {
                        dataHolder.currentStatus = STATUS_RESERVE;
                    }
                    if (parent.getAutoEntryFavorite() == true)
                    {
                        // お気に入りに自動追加する場合...
                        String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                        setResetFavorite(dataHolder, url, threadNick, true);
                    }
                    return (true);
                }

                // 登録されていない、、、データをテーブルに仮格納する
                for (int index = 0; index < subjectTitles; index++)
                {
                    long numnum = getThreadNumber(index);
                    if (numnum == number)
                    {
                        // subject.txtに格納されている...タイトルを切り取って格納する
                        entryThreadDataHolder(index, STATUS_RESERVE, 1, 0);
                        if (parent.getAutoEntryFavorite() == true)
                        {
                            // お気に入りに自動追加する場合...
                            String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                            setResetFavorite(index, url, threadNick, true);
                        }
                        parent.detectMessageThreadNumber(index);
                        return (true);
                    }
                }

                // subject.txtにない...とりあえず、a2b.idxには格納する
                String threadTitleInterim =  "(a2B_INTERIM)" + threadNumber;
                dataHolder                   = null;
                dataHolder                   = new subjectDataHolder(threadTitleInterim.getBytes(), 0, threadTitleInterim.getBytes().length);
                dataHolder.threadNumber      = number;
                dataHolder.numberOfMessage   = 0;
                dataHolder.currentMessage    = 1;
                if (parent.getAutoEntryFavorite() == true)
                {
                    // お気に入りに自動追加する場合...
                    String url = bbsCommunicator.getBoardURL(parent.getCurrentBbsIndex());
                    setResetFavorite(dataHolder, url, threadNick, true);
                }
                subjectMap.put(datFileName, dataHolder);
                subjectVector.addElement(dataHolder);
                dataExtracted = true;

                // このときには、表示モードをデータベースアクセスモードに強制的に切り替える
                useOnlyDatabase = true;
                parent.detectMessageThreadNumber(subjectVector.size() - 1);
            }
            catch (Exception e)
            {
                // 何もしない
                return (false);
            }
            dataHolder.currentStatus     = STATUS_RESERVE;
            return (true);
        }

        if (parent.getAutoEntryFavorite() == true)
        {
            // お気に入りに自動追加する場合...
            if (favorManager.exist(threadNick, number) < 0)
            {
                String url = bbsCommunicator.getBoardURL(threadNick);
                String tempTitle = "(a2B_INTERIM)" + threadNumber;
                favorManager.entryFavorite(tempTitle.getBytes(), url, threadNick, number, 0, 0, STATUS_RESERVE);
            }
        }
        // 現在オープンしていない板だった... (a2b.idxをオープンして追加する)
        return (reserveThread(threadNick, number));
    }

    /**
     *   スレを取得予約する...
     * 
     */
    private boolean reserveThread(String nick, long threadNumber)
    {
        String indexFile = parent.getBaseDirectory() + nick;
        FileConnection connection         = null;
        
        try
        {
            // ディレクトリがオープンできるか？
            connection = (FileConnection) Connector.open(indexFile);
            if (connection.exists() == true)
            {
                if (connection.isDirectory() != true)
                {
                    // ディレクトリではなかった...終了する
                    connection.close();
                    connection = null;
                    return (false);
                }
            }
            else
            {
                // ディレクトリを作成する
                connection.mkdir();
                connection.close();
                connection = null;
                
                // a2b.idxを末尾に追加する
                return (appendA2Bindex(nick, threadNumber));
            }
            connection.close();
            connection = null;
        }
        catch (Exception e)
        {
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
            return (false);
        }
        
        InputStream    in                = null;
        byte[]         buffer            = new byte[indexReadSize + bufferMargin];
        int            dataReadSize      = 0;
        long           checkThreadNumber = 0;
        boolean       isChecking         = false;

        indexFile = indexFile + "a2b.idx";
        try
        {
            connection   = (FileConnection) Connector.open(indexFile, Connector.READ);

            //////  データを最初から size分読み出す...  /////
            in           = connection.openInputStream();
            while (dataReadSize >= 0)
            {
                dataReadSize = in.read(buffer, 0, indexReadSize);

                // a2b.idxの解析...
                for (int index = 0; index < dataReadSize; index++)
                {
                    if (buffer[index] != 10)
                    {
                        if (isChecking == true)
                        {
                            if ((buffer[index] >= '0')&&(buffer[index] <= '9'))
                            {
                                checkThreadNumber = checkThreadNumber * 10 + (buffer[index] - '0');
                            }
                            else
                            {
                                if (threadNumber == checkThreadNumber)
                                {
                                    // スレは登録済みだった、、、この場合には何もしない...
                                    in.close();
                                    connection.close();
                                    buffer = null;                                    
                                    System.gc();
                                    return (true);
                                }
                                isChecking = false;
                            }
                        }
                    }
                    else
                    {
                        // スレ番号の解析モードに切り替える
                        isChecking = true;
                    }
                }
            }
            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " ExCEption  e:" + e.getMessage());
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
            dataReadSize = 0;
            System.gc();
            return (false);
        }
        buffer = null;        
        System.gc();

        // a2b.idxに記録されていない、末尾に記録する。。
        return (appendA2Bindex(nick, threadNumber));
    }

    /**
     *  予約スレを末尾に追加する
     * 
     * @param nick
     * @param threadNumber
     * @return
     */
    private boolean appendA2Bindex(String nick, long threadNumber)
    {
        String         indexFile   = parent.getBaseDirectory() + nick + "a2b.idx";
        FileConnection connection  = null;
        OutputStream   out         = null;
        try 
        {
            // ファイルの書き込み
            connection = (FileConnection) Connector.open(indexFile, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // ファイル作成
                connection.create();
            }
            
            // ファイルの末尾から書き込みを行う...
            long size  =  connection.fileSize();
            out         = connection.openOutputStream(size + bufferMargin);
            if (size == 0)
            {
                // ファイルサイズがゼロの場合、インデックスデータのコメントを出力する
                String comment = "; fileName, max, current, status, 0, 0, (Title)\n";
                out.write(comment.getBytes());
            }
            String outputData = ";\r\n" + threadNumber + ".dat,0,1," + STATUS_RESERVE + ",0,0," + "(a2B_INTERIM)" + threadNumber + "\r\n";
            out.write(outputData.getBytes());
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", indexFile + " Exception  ee:" + e.getMessage());
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
            return (false);
        }
        return (true);
    }

    /**
     *  タイトル名を張り替える...
     * 
     * @param index
     * @param title
     */
    public void updateThreadTitle(int index, byte[] title, int offset, int length)
    {
        // タイトル名を張り替える
        subjectDataHolder dataHolder = getSubjectDataHolder(index);
        if (dataHolder == null)
        {
            return;
        }
        dataHolder.changeTitleName(title, offset, length);
        return;
    }

    /**
     *  テキストファイルにデータ出力する
     * 
     * @param outputFileName  出力ファイル名
     * @param title           タイトル
     * @param message         出力データ
     * @param isAppendMode    上書きモード(true)/追記モード(false)
     * @return
     */
    public boolean outputDataTextFile(String outputFileName, String title, String message, boolean isAppendMode)
    {
        FileConnection connection = null;
        OutputStream  out = null;
        String memoFile = outputFileName;
        try
        {
            // 指定ファイル名が絶対ファイルかどうか？
            if (memoFile.indexOf("file:///") < 0)
            {
                // メモファイル名が絶対パス指定ではない場合、、、
                memoFile = parent.getBaseDirectory() + memoFile;
            }

            // ファイルの書き込み
            connection = (FileConnection) Connector.open(memoFile, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // ファイル作成
                connection.create();
            }
            else
            {
                if (isAppendMode == false)
                {
                    // ファイルが存在した場合には、一度消して作り直す
                    connection.delete();
                    connection.create();
                }
            }
            long fileSize = connection.fileSize();
            out = connection.openOutputStream(fileSize);

            // ファイルに出力する
            String data = "- - - - - -\r\n";
            out.write(data.getBytes());
            data = title  + "\r\n";
            out.write(data.getBytes());
            data = message  + "\r\n";
            out.write(data.getBytes());
            
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
            return (false);
        }
        return (true);        
    }

    /**
     *  迷子になっているdatファイルをさらう
     *  (subject.txtに存在し、かつ現在スレデータクラスで管理されていない datファイルを探す)
     * 
     * @return
     */
    public int collectDatFile()
    {
        int            nofFile   = 0;
        FileConnection connection = null;
        Enumeration    dirEnum    = null;
        String         dirName    = parent.getBaseDirectory() + boardNick;
        byte[]        readBuffer = new byte[readBufferSize + bufferMargin];
        try
        {
            // 指定されたパスから一覧を取得する
            connection = (FileConnection) Connector.open(dirName, Connector.READ);
            dirEnum = connection.list("*", true);
            while (dirEnum.hasMoreElements())
            {
                String name  = (String)dirEnum.nextElement();
                String title = name;
                if ((name.endsWith(".dat") == true)&&((name.indexOf(".bak") < 0)))
                {
                    // datファイルだった!!
                    subjectDataHolder dataHolder = (subjectDataHolder) subjectMap.get(name);
                    if (dataHolder == null)
                    {
                        // 迷子のdatファイル発見！                        
                        ////// スレ番号抽出処理... //////
                        int  index = 0;
                        long threadNumber = 0;
                        char ch = name.charAt(index);
                        while (ch != '.')
                        {
                            if ((ch >= '0')&&(ch <= '9'))
                            {
                                threadNumber = threadNumber * 10 + (ch - '0');
                            }
                            else
                            {
                                break;
                            }
                            index++;
                            ch = name.charAt(index);
                        }

                        long           fileSize = 0;
                        FileConnection  fc = null;
                        try
                        {
                            // ファイルをオープンしてみて、サイズを確認する...
                            fc = (FileConnection) Connector.open(parent.getBaseDirectory() + boardNick + name, Connector.READ_WRITE);
                            fileSize = fc.fileSize();

                            if (fileSize != 0)
                            {
                                // スレタイを読み出してみる...
                                InputStream in = fc.openInputStream();
                                   in.read(readBuffer, 0, readBufferSize);
                                   title = pickupThreadTitle(readBuffer);
                                   in.close();
                            }
                            else
                            {
                                // ファイルサイズがゼロの場合...ファイルを削除し、スレ登録はしない
                                fc.delete();
                            }
                            fc.close();
                        }
                        catch (Exception ee)
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
                            fileSize = 0;
                        }

                        // 管理データとして格納する。。。
                        if (fileSize != 0)
                        {
                            dataHolder                   = new subjectDataHolder(title.getBytes(), 0, (title.getBytes()).length);
                            dataHolder.threadNumber      = threadNumber;
                            dataHolder.numberOfMessage   = 1;
                            dataHolder.currentMessage    = 1;
                            dataHolder.currentStatus     = STATUS_UNKNOWN;
                            subjectMap.put(name, dataHolder);
                            subjectVector.addElement(dataHolder);
                            nofFile++;
                        }
                    }
                }
            }
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", dirName + " Exception eee:" + e.getMessage());
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
            nofFile = -1;
        }
        readBuffer = null;
        dataExtracted = true;
        System.gc();
        return (nofFile);
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
        public byte    entryFavorite   = 0;       // お気に入り登録済みかどうか
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
