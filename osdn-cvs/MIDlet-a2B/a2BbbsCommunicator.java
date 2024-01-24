import java.io.InputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 *  BBSデータ管理クラス...
 * 
 * @author MRSa
 *
 */
public class a2BbbsCommunicator
{
    // オブジェクト
    private a2BMain         parent        = null;
    private a2Butf8Conveter utf8Converter = null;

    // 各種マジックナンバー
    private final int   BBS_MAX_CATEGORY   = 60;   // 管理可能な板カテゴリ数
    private final int   BBS_MAX_ARRAY      = 96;   // カテゴリに含まれる板の最大数
    private final int   MARGIN             = 4;    // データバッファ

    private final String FILE_BBSTABLE      = "bbstable.html";                      // 板一覧のファイル名

    // 板数、板カテゴリ数の管理...
    private int          bbsCategoryCount   = 0;
    private int          bbsCount           = 0;

    private bbsCategoryHolder[]  bbsCategoryList     = null;
    private bbsDataHolder[]      bbsInformationArray = null;
    private int                  bbsInformationData  = 0;

    /**
     *   コンストラクタ
     *
     */
    public a2BbbsCommunicator(a2BMain object, a2Butf8Conveter converter)
    {
        // 親を記録する...
        parent = object;
        utf8Converter = converter;

        // 板カテゴリテーブルの準備...
        bbsCategoryList = new bbsCategoryHolder[BBS_MAX_CATEGORY];

        // 板テーブルの準備...
        bbsInformationArray = new bbsDataHolder[BBS_MAX_ARRAY];
    }

    /**
     *   板一覧データベースを作成(準備)する
     *   
     */
    public void prepareBbsDatabase()
    {
        Thread thread = new Thread()
        {
            public void run()
            {
                // 板一覧データベースの構築
                updateBbsDatabase();
            }
        };
        thread.start();
        return;
    }

    /**
     * 板nickを抽出する
     * @param boardUrl
     * @return
     */
    public String getBoardNickName(String boardUrl)
    {

        // 取得先および取得ファイル名を生成する
//        int pos = gettingLogDataURL.lastIndexOf('/', gettingLogDataURL.length() - 2);

        // 取得先および取得ファイル名を生成する
        int pos = boardUrl.lastIndexOf('/', boardUrl.length() - 2);

        // 板ニックネームを決定する
        return (boardUrl.substring(pos + 1));
    }    
    
    /**
     *   (板Nickディレクトリが存在しなければ)ディレクトリを作成する
     *  
     */
    public boolean checkBoardNickName(String nickName)
    {
        boolean ret = true;
        FileConnection fc = null;
        String directory = parent.getBaseDirectory() + nickName;
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
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION(checkNick)  e:" + e.getMessage());
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
     *   板カテゴリ名称を取得する
     *   
     */
    public String getCategory(int index)
    {
        if ((index < 0)&&(index >= bbsCategoryCount))
        {
            index = 0;
        }
        return (utf8Converter.parsefromSJ(bbsCategoryList[index].categoryName, 0, bbsCategoryList[index].categoryName.length));
    }

    /**
     *   板カテゴリに含まれる板の先頭index番号を取得する
     *   
     */
    public int getCategoryIndex(int index)
    {
        if ((index < 0)&&(index >= bbsCategoryCount))
        {
            index = 0;
        }
        return (bbsCategoryList[index].categoryIndex);
    }

    /**
     *   登録済み板数を応答する
     *   
     */
    public int getNumberOfBoardName()
    {
        return (bbsCount);
    }

    /**
     *  登録済みカテゴリ数を応答する
     *  
     */
    public int getNumberOfCategory()
    {
        return (bbsCategoryCount);
    }

    /**
     *   現在の板名称を取得する
     *   
     */
    public String getBoardName(int index)
    {
        int categoryIndex          = parent.getCurrentCategoryIndex();
        int categoryIndexOffset    = bbsCategoryList[categoryIndex].categoryIndex;
        int categoryIndexMaxOffset = bbsCategoryList[categoryIndex + 1].categoryIndex;
        if ((categoryIndexOffset > index)||(categoryIndexMaxOffset <= index))
        {
            return (".");
        }
        if ((bbsInformationArray == null)||(bbsInformationArray[index - categoryIndexOffset] == null))
        {
            return (" ");
        }

        return (utf8Converter.parsefromSJ(bbsInformationArray[index - categoryIndexOffset].boardName, 0, bbsInformationArray[index - categoryIndexOffset].nameLength));
    }

    /**
     *  現在の板のURLを取得する
     * 
     */
    public String getBoardURL(int index)
    {
        int categoryIndex = parent.getCurrentCategoryIndex();
        int categoryIndexOffset = bbsCategoryList[categoryIndex].categoryIndex;
        int categoryIndexMaxOffset = bbsCategoryList[categoryIndex + 1].categoryIndex;
        if ((categoryIndexOffset > index)||(categoryIndexMaxOffset <= index))
        {
            return ("Unknown");
        }
        return (new String(bbsInformationArray[(index - categoryIndexOffset)].boardUrl, 0, bbsInformationArray[(index - categoryIndexOffset)].urlLength));
    }
    
    /**
     * 板Nickから、URLを取得する...
     * 
     * @param boardNick
     * @return
     */
    public String getBoardURL(String boardNick)
    {
        int currentBbsCategoryIndex = parent.getCurrentCategoryIndex();
        String currentBbsNick       = getBoardNickName(getBoardURL(parent.getCurrentBbsIndex()));
        String url                  = "";

        // 板カテゴリを先頭から全てナメる。。。
        for (int categoryLoop = 0; categoryLoop < bbsCategoryCount; categoryLoop++)
        {
            //
            int bbsIndex = checkBoardNick(categoryLoop, boardNick);
            if (bbsIndex >= 0)
            {
                refreshBoardInformationCache(categoryLoop, false, boardNick);
                int categoryIndexOffset    = bbsCategoryList[categoryLoop].categoryIndex;
                url = new String(bbsInformationArray[(bbsIndex - categoryIndexOffset)].boardUrl, 0, bbsInformationArray[(bbsIndex - categoryIndexOffset)].urlLength);
                break;
            }
        }
        refreshBoardInformationCache(currentBbsCategoryIndex, false, currentBbsNick);
        return (url);
    }

    /**
     * 板カテゴリの切り替え...
     * 
     * @param checkOnly
     */
    private void clearBoardInformation(boolean checkOnly)
    {
        if (checkOnly == true)
        {
            // チェックだけのときには何もしない
            return;
        }
        for (int loop = 0; loop < bbsInformationData; loop++)
        {
            bbsInformationArray[loop] = null;
        }
        bbsInformationData = 0;
        
        return;
    }

    /**
     * 板情報を格納する
     * 
     * @param checkOnly
     * @param buffer
     * @param startOffset
     * @param endSize
     * @param bbsIndex
     * @return
     */
    private int enterBoardInformationArray(boolean checkOnly, String boardNick, byte[] buffer, int startOffset, int endSize, int bbsIndex)
    {
        // URL検出!
        int loop  = startOffset;
        int index = 0;
        while ((index < endSize)&&(buffer[startOffset + index] != '>')&&(buffer[startOffset + index] != 34)&&(buffer[startOffset + index] != 39))
        {
            index++;
        }
        // URLを取り込む！
        if (index >= endSize)
        {
            // 想定外のデータ...
            return (-1);
        }

        // 板nickをチェックするモードだった場合...
        if (checkOnly == true)
        {
            String url = new String(buffer, startOffset, index);
            String nick = getBoardNickName(url);
            if (nick.equals(boardNick) == true)
            {
                // 板nickの一致を検出!!
                url  = null;
                nick = null;
                return (0);
            }
            url  = null;
            nick = null;
            return (-100);
        }

        bbsInformationArray[bbsInformationData] = new bbsDataHolder();
        
        // URLの抽出 (コピー)
        bbsInformationArray[bbsInformationData].setBoardUrl(buffer, startOffset, index);
        loop = loop + index;

        // アンカー分飛ばす...
        while ((loop < (startOffset + endSize))&&(buffer[loop] != '>'))
        {
            loop++;
        }
        loop++;

        // ボード名称を検出する
        index = 0;
        while ((index < endSize)&&(buffer[loop + index] != '<'))
        {
            index++;
        }
        bbsInformationArray[bbsInformationData].boardName = null;
        bbsInformationArray[bbsInformationData].setBoardName(buffer, loop, index);
        loop = loop + index;

        bbsInformationArray[bbsInformationData].boardIndex = bbsIndex;

        if (bbsInformationData < (BBS_MAX_ARRAY - 1))
        {
            bbsInformationData++;
        }
        return (loop);
    }

    /**
     *   板情報ファイル(bbstabl.html)の一部（カテゴリ毎）をメモリ内にキャッシュする
     *
     *   
     */
    public int refreshBoardInformationCache(int categoryIndex, boolean checkOnly, String boardNick)
    {
        FileConnection  bbsFileConnection  = null;
        int          size      = 0;
        InputStream  in        = null;
        byte[]       buffer   = null;
//        int categoryIndex = parent.getCurrentCategoryIndex();
        int dataSize = bbsCategoryList[categoryIndex + 1].fileOffset - bbsCategoryList[categoryIndex].fileOffset;
        if (dataSize < 0)
        {
            // データサイズが異常...終了する
            return (-100);
        }
        int bbsIndex = bbsCategoryList[categoryIndex].categoryIndex;
    
        try
        {
            bbsFileConnection = (FileConnection) Connector.open((parent.getBaseDirectory() + FILE_BBSTABLE), Connector.READ);

            size = (int) bbsFileConnection.fileSize();
            if (size < dataSize)
            {
                bbsFileConnection.close();
                return (-101);
            }

            // 板情報を初期化(正確に言うと、一部は残っている...)する...
            clearBoardInformation(checkOnly);
            
            //////  データを最初から dataSize分読み出す...  /////
            in = bbsFileConnection.openInputStream();

            // データバッファを用意する
            buffer = new byte[dataSize + MARGIN];
            
            // ファイルの先頭まで読み出し...
            int startOffset = bbsCategoryList[categoryIndex].fileOffset;
            while (startOffset > 0)
            {
                int readSize = 0;
                if (startOffset > dataSize)
                {
                    readSize = in.read(buffer, 0, dataSize);
                }
                else
                {
                    readSize = in.read(buffer, 0, startOffset);
                }
                startOffset = startOffset - readSize;
            }
            size = in.read(buffer, 0, dataSize);
            in.close();

            bbsFileConnection.close();

            for (int loop = 0; loop < size; loop++)
            {
                if ((buffer[loop] == 'h')&&(buffer[loop + 1] == 't')&&(buffer[loop + 2] == 't')&&(buffer[loop + 3] == 'p'))
                {
                    // 板情報を格納する...
                    int parsed = enterBoardInformationArray(checkOnly, boardNick, buffer, loop, (size - loop), bbsIndex);
                    if (parsed < 0)
                    {
                        loop++;
                        bbsIndex++;
                        continue;
                    }
                    if (parsed == 0)
                    {
                        // アイテム検出...
                        buffer = null;
                        System.gc();
                        return (bbsIndex);
                    }
                    loop = parsed;
                    bbsIndex++;
                }
            }
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXception  e:" + e.getMessage());
            try
            {
                if (bbsFileConnection != null)
                {
                    bbsFileConnection.close();
                }
            }
            catch (Exception e2)
            {
            
            }
            return (-102);
        }

        // ガベコレ実施。
        System.gc();
        return (-1);
    }

    /**
     *  板nick指定で所属する板へ移動する
     * 
     * @param boardUrl
     * @return
     */
    public boolean moveToBoard(String boardNick)
    {
        // 板カテゴリを先頭から全てナメる。。。
        for (int categoryLoop = 0; categoryLoop < bbsCategoryCount; categoryLoop++)
        {
            //
            int bbsIndex = checkBoardNick(categoryLoop, boardNick);
            if (bbsIndex >= 0)
            {
                // 大変だ！ 板が見つかった！！ (カテゴリと、板インデックスを変更する)
                parent.setCurrentBbsCategoryIndex(categoryLoop);
                parent.setCurrentBbsIndex(bbsIndex);
                return (true);
            }
        }
        return (false);
    }
    
    /**
     * 板urlが指定したカテゴリに含まれるか検索する...
     * @param index
     * @param boardNick
     * @return
     */
    private int checkBoardNick(int index, String boardNick)
    {
        return (refreshBoardInformationCache(index, true, boardNick));
    }

    /**
     *   カテゴリ内板一覧データベースの（無理やり）構築処理
     *   
     */
    private void parseBbsDatabase(byte[] buffer, int bufferSize, int offset)
    {
        int entryBbsIndex = 0;
        int loop;

        // カテゴリ、ボードURL、ボード名を抽出する。
        bbsCategoryCount = 0;
        for (loop = 0; loop < bufferSize; loop++)
        {
            // 板一覧DB作成開始...
            if ((buffer[loop] == (byte) 0x81)&&(buffer[loop + 1] == 121)&&(buffer[loop + 2] == 60)&&
                (buffer[loop + 3] == 66)&&(buffer[loop + 4] == 62))
            {
                // 板一覧カテゴリ発見！
                loop = loop + 5;
                int index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != 60))
                {
                    index++;
                }

                // 板カテゴリを登録する
                bbsCategoryList[bbsCategoryCount] = new bbsCategoryHolder(entryBbsIndex, (loop + index + 1));
                bbsCategoryList[bbsCategoryCount].setText(buffer, loop, index);
                bbsCategoryCount++;
                if (bbsCategoryCount >= BBS_MAX_CATEGORY)
                {
                    return;
                }
                loop = loop + index;
                continue;
            }
            if ((buffer[loop] == 'h')&&(buffer[loop + 1] == 't')&&(buffer[loop + 2] == 't')&&(buffer[loop + 3] == 'p'))
            {
                // URL検出!
                int index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '>')&&(buffer[loop + index] != 34)&&(buffer[loop + index] != 39))
                {
                    index++;
                }
                // URL名を取り込む！
                loop = loop + index;
                if (buffer[loop] != '>')
                {
                    // 想定外のデータ...
                    continue;
                }
                loop++;

                // ボード名称を検出する
                index = 0;
                while ((index < (bufferSize - loop))&&(buffer[loop + index] != '<'))
                {
                    index++;
                }
                loop = loop + index;

                // 板情報を登録する。
                if (bbsCategoryCount > 0)
                {
                    bbsCount++;
                    entryBbsIndex++;
                }
                continue;
            }
        }
        return;
    }

    /**
     *   板一覧データベースを作成する...
     *   
     */
    private void updateBbsDatabase()
    {
        FileConnection  bbsFileConnection  = null;
        InputStream     in                 = null;

        String          fileName           = parent.getBaseDirectory() + FILE_BBSTABLE;    
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
//            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug", "EXCEPTION  e:" + e.getMessage());
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
            buffer = null;
            return;
        }
        buffer = null;

        // BBS一覧データベースの準備完了報告
        parent.completedPrepareBbsDatabase();
        return;
    }
    
    /**
     *     板カテゴリ情報を保持するクラス
     *
     */
    private class bbsCategoryHolder
    {
        public byte[] categoryName  = null;  // カテゴリ名称(メモリ使用量削減のためbyte[]で保管)
        public int    categoryIndex = 0;      // カテゴリインデックス番号
        public int    fileOffset    = 0;      // bbstable.html内でデータがある位置 (オフセット(バイト数))

        // コンストラクタ
        public bbsCategoryHolder(int indexNumber, int offset)
        {
            categoryIndex = indexNumber;
            fileOffset    = offset;
        }

        // カテゴリ名称の登録
        public void setText(byte[] buffer, int offset, int length)
        {
            categoryName = null;
            categoryName = new byte[length + 2];
            System.arraycopy(buffer, offset, categoryName, 0, length);
            categoryName[length] = 0;
            return;
        }
    }

    /**
     *     板情報を保持するクラス
     *
     */
    private class bbsDataHolder
    {
        // 板名称(メモリ使用量削減のためbyte[]で保管)
        public byte[] boardName = null;
        public int    nameLength = 0;

        // 板URL(メモリ使用量削減のためbyte[]で保管)
        public byte[] boardUrl  = null;
        public int   urlLength = 0;
        
        // 板インデックス番号
        public int   boardIndex = 0;

/*
         // メモ： 将来の拡張イメージ...
        public final byte BBSTYPE_2ch = 1;     // 板タイプは2ちゃんねる
        private byte bbsType = BBSTYPE_2ch;    // この板タイプ
*/

        // コンストラクタ
        public bbsDataHolder()
        {
            // 何もしない...
        }

        /**
         *  板名称の記録 (読み出しは直接行う)
         * 
         */
        public void setBoardName(byte[] buffer, int offset, int length)
        {
            boardName = null;
            boardName = new byte[length + 2];
            System.arraycopy(buffer, offset, boardName, 0, length);
            boardName[length] = 0;
            nameLength = length;
            return;
        }        

        /**
         *  板URLの記録 (読み出しは直接行う)
         * 
         */
        public void setBoardUrl(byte[] buffer, int offset, int length)
        {
            boardUrl = null;
            boardUrl = new byte[length + 2];
            System.arraycopy(buffer, offset, boardUrl, 0, length);
            boardUrl[length] = 0;
            urlLength = length;
            return;
        }        
    }
}

//
//  <<< 備忘録 >>>
//  板DBの構造 (144bytes × 板数) ： ファイルにするかも...
//     - 板カテゴリID      ： int
//     - 板Nick             : byte[] (16bytes)
//     - 板URL              : byte[] (80bytes)
//     - 板名称             : byte[] (40bytes)
//     - BBSタイプ          : byte
//     - スレレベル         : byte
//     - スレ状態           : byte
//     - 予約               : byte
//
//  板カテゴリDBの構造 (52bytes × 板カテゴリ数)
//     - 板カテゴリID       : int
//     - 板カテゴリ名称     : byte[] (40bytes)
//     - 親板カテゴリID     : int
//     - 板DBの有無         : byte
//     - 予約               : byte
//     - 予約               : byte
//     - 予約               : bytes
//
//
//--------------------------------------------------------------------------
//  a2BbbsDatabase  --- BBSコミュニケータ...
//
//        - 板との通信
//        - 板一覧の管理
//--------------------------------------------------------------------------
//   MRSa (mrsa@sourceforge.jp)
