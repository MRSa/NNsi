import java.io.InputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;


public class a2BMessageParser
{
    private a2Butf8Conveter      stringConverter = null;

    private boolean convertHanzen   = true;               // 半角カナ→全角カナ変換を実施する

    private byte[]            parsedBuffer = null;         // 解析済みレスデータ格納領域
    private int[]             bufferIndex = null;          // レスのインデックス(レスの先頭アドレス)
    private int               numberOfIndex = 0;           // 解析したレスの数

    private String            threadBaseTitle = null;      // スレタイのベースタイトル
    private byte[]            threadMessageTitle = null;  // スレタイの解析後タイトル
    private int               threadMessageLength = 0;     // スレタイの解析後タイトル長さ

    private String            readFileName = null;         // スレファイル名
    private long              dataFileSize  = 0;           // スレファイルのファイルサイズ

    private byte[]            buff = null;                // スレ表示用データバッファ
    private int               currentFileOffset = -1;      // 現在読み込んでいるスレ表示用データバッファの先頭アドレス
    private int               currentReadSize   = 0;       // 現在読み込んでいるスレ表示用データバッファのデータサイズ
    
    private String             nameField  = "";
    private String             emailField = "";
    //private String             dateTimeField = null;
    //private String             messageField = null;

    // マジックナンバー
    private int               messageLimit = 1005;            // スレメッセージ解析数の最大
    private int               readBufferSize = (10240 * 2);   // データバッファの最大領域
    
    private int               separatorSize  = (1024 * 3);    // テキストデータモードのデータ区切り検索開始
    private int               separatorLimit = (1024 * 4);    // テキストデータモードのデータ区切り制限

/**
    // 半角カナ/全角カナ変換テーブル
    private final short[] Han2ZenTable = {
      // "。", "「", "」", "、", "・", "ヲ",
      (short) 0x8142, (short) 0x8175, (short) 0x8176, 
      (short) 0x8141, (short) 0x8145, (short) 0x8392,
      
      // "ァ", "ィ", "ゥ", "ェ", "ォ", "ャ",
      (short) 0x8340, (short) 0x8342, (short) 0x8344, 
      (short) 0x8346, (short) 0x8348, (short) 0x8383,

      // "ュ", "ョ", "ッ", "ー", "ア", "イ",
      (short) 0x8385, (short) 0x8387, (short) 0x8362, 
      (short) 0x815b, (short) 0x8341, (short) 0x8343,

      // "ウ", "エ", "オ", "カ", "キ", "ク",
      (short) 0x8345, (short) 0x8347, (short) 0x8349, 
      (short) 0x834a, (short) 0x834c, (short) 0x834e,

      // "ケ", "コ", "サ", "シ", "ス", "セ",
      (short) 0x8350, (short) 0x8352, (short) 0x8354, 
      (short) 0x8356, (short) 0x8358, (short) 0x835a,

      // "ソ\", "タ", "チ", "ツ", "テ", "ト",
      (short) 0x835c, (short) 0x835e, (short) 0x8360, 
      (short) 0x8363, (short) 0x8365, (short) 0x8367,

      // "ナ", "ニ", "ヌ", "ネ", "ノ", "ハ",
      (short) 0x8369, (short) 0x836a, (short) 0x836b, 
      (short) 0x836c, (short) 0x836d, (short) 0x836e,

      // "ヒ", "フ", "ヘ", "ホ", "マ", "ミ",
      (short) 0x8371, (short) 0x8374, (short) 0x8377, 
      (short) 0x837a, (short) 0x837d, (short) 0x837e,

      // "ム", "メ", "モ", "ヤ", "ユ", "ヨ",
      (short) 0x8380, (short) 0x8381, (short) 0x8382, 
      (short) 0x8384, (short) 0x8386, (short) 0x8388,

      // "ラ", "リ", "ル", "レ", "ロ", "ワ",
      (short) 0x8389, (short) 0x838a, (short) 0x838b, 
      (short) 0x838c, (short) 0x838d, (short) 0x838f,

      // "ン", "゛", "゜", "",   "",   ""
      (short) 0x8393, (short) 0x814a, (short) 0x814b, 
      (short) 0x0000, (short) 0x0000, (short) 0x0000
    };
**/
    private final int  PARSING_NORMAL      = 0;
    private final int  PARSING_RAW         = 1;
//    private final int  PARSING_HTML        = 2;
    private int        messageParsingMode   = PARSING_NORMAL;
    
    // コンストラクタ
    public a2BMessageParser(a2Butf8Conveter utf8Converter) 
    {
        // 文字列変換クラスを記憶する
        stringConverter = utf8Converter;
    }

    // メッセージのクリア (このタイミングで強制的にガベコレを実行...)
    public void leaveMessage()
    {
        // 確保した領域を開放する...
        buff = null;
        readFileName = null;
        parsedBuffer = null;
        bufferIndex = null;
        threadBaseTitle = null;
        threadMessageTitle = null;

        // ガベコレの実施
        System.gc();
    }

    // ファイル名の設定処理
    public void setFileName(String fileName)
    {
        // ファイル名を設定する
        readFileName = fileName;        
    }

    // ファイル名を取得する
    public String getFileName()
    {
        return (readFileName);
    }

    // ファイルサイズを応答する
    public long getFileSize()
    {
        return (dataFileSize);
    }
    
    // 半角カナ -> 全角カナの変換を実施するかどうか
    public void setConvertHanZen(boolean setting)
    {
        convertHanzen = setting;
        return;
    }

    // テキストパースモードかどうかの設定
    public void setMessageParsingMode(int parseMode)
    {
        messageParsingMode   = parseMode;
        return;
    }
    
    // メッセージ解析の処理
    public boolean prepareMessage()
    {
        // ファイル名が設定されていなければエラー
        if (readFileName.length() == 0)
        {
            return (false);
        }
        FileConnection     dataFileConnection = null;

        // 読み込み用データバッファの確保
        buff = new byte[readBufferSize + 60];

        // スレインデックス格納領域の確保
        bufferIndex = new int[(messageLimit + 1)];
        for (int loop = 0; loop <= messageLimit; loop++)
        {
            bufferIndex[loop] = 0;
        }
        numberOfIndex = 0;

        // ファイルのオープン、ファイルサイズの特定...
        dataFileSize = 0;
        try
        {
            dataFileConnection = (FileConnection) Connector.open(readFileName, Connector.READ);
            dataFileSize = dataFileConnection.fileSize();
        }
        catch (Exception e)
        {
            // ファイルサイズの特定失敗...
            return (false);
        }

        // 名前欄およびEmail欄を初期化
        nameField = "";        
        emailField = "";

        // こそっとガベコレ実施...
        System.gc();

        // ファイル内にあるスレデータのレコードインデックスを作成する
        InputStream  in = null;
        try
        {
            boolean firstTime = true;
            int offset = 0;
            int readSize = 1;
            in = dataFileConnection.openInputStream();
            while (readSize > 0)
            {
                readSize = in.read(buff, 0, readBufferSize);
                if (readSize > 0)
                {
                    if (messageParsingMode != PARSING_NORMAL)
                    {
                        // HTML/TEXT用のスレインデックス検索。。。
                        createThreadIndexSub_TextParsing(offset, buff, readSize);                        
                    }
                    else
                    {
                        // 通常のスレインデックス検索。。。
                        createThreadIndexSub(offset, buff, readSize);
                    }
                    offset = offset + readSize;

                    // 初回だった場合には、スレタイトルを抽出する
                    if (firstTime == true)
                    {
                        pickupThreadTitle(buff, readSize);
                        firstTime = false;
                    }
                }
            }
            in.close();
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

            // ファイルがオープンしっぱなしだったらクローズする
            try
            {
                if (dataFileConnection.isOpen() == true)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception e3)
            {
                // 例外が発生したときは何もしない
            }
            // なんだか解析に失敗...
            return (false);
        }

        // ファイルデータはまだ読み込まれていない状態にする
        currentFileOffset = -1;
        currentReadSize = 0;

        // ファイルがオープンしっぱなしだったらクローズする
        try
        {
            if (dataFileConnection.isOpen() == true)
            {
                dataFileConnection.close();
            }
            dataFileConnection = null;
        }
        catch (Exception e)
        {
            // 例外が発生したときは何もしない
        }

        return (true);
    }

    // レス数の最大数を応答する
    public int getNumberOfMessages()
    {
        return (numberOfIndex);        
    }

    // スレタイの設定処理
    public String decideThreadTitle()
    {
        if (threadMessageTitle != null)
        {
//            threadBaseTitle = new String(threadMessageTitle);
            threadBaseTitle = stringConverter.parsefromSJ(threadMessageTitle);
        }
        return ("[1:" + numberOfIndex + "] " + threadBaseTitle);
    }

    // スレタイトルを指定された場合...
    public void setThreadBaseTitle(String title)
    {
        threadBaseTitle = title;
        return;
    }

    /**
     * スレタイトルを応答する
     * 
     * @return
     */
    public String getTitle()
    {
        return (threadBaseTitle);
    }

    // レスデータを取得する
    public byte[] getMessage(int number, int kanjiMode)
    {
        parsedBuffer = null;
        System.gc();

        // 取得するよう指定されたレス番号を確認し、範囲外なら表示しない
        if ((numberOfIndex < 1)||(number < 1)||(number > numberOfIndex))
        {
            parsedBuffer = new byte[4];
            parsedBuffer[0] = '-';
            parsedBuffer[1] = '-';
            parsedBuffer[2] = '-';
            parsedBuffer[3] = '\0';
            return (parsedBuffer);
        }

        int offset = 0;
        int parseSize = 0;
        if (number == 1)
        {
            offset = 0;
            parseSize = bufferIndex[0];
        }
        else
        {
            offset = bufferIndex[(number - 2)];
            parseSize = bufferIndex[(number - 1)] - bufferIndex[(number - 2)];
        }

        // レスデータが現在読み込んでいるデータバッファ内に含まれていない...
        if ((offset < currentFileOffset)||((offset + parseSize) > (currentFileOffset + currentReadSize)))
        {
            // データファイルを読み出す...
            if (loadFromFile(offset, parseSize) != true)
            {
                parsedBuffer = new byte[4];
                parsedBuffer[0] = 'x';
                parsedBuffer[1] = 'x';
                parsedBuffer[2] = 'x';
                parsedBuffer[3] = '\0';
                return (parsedBuffer);
            }
        }

        // ファイルが途中で切れていた場合...
        if ((offset + parseSize) > (currentFileOffset + currentReadSize))
        {
            parseSize = (currentFileOffset + currentReadSize) - offset;
        }
        
        // オフセットを調整する
        if (offset >= currentFileOffset)
        {
            offset = offset - currentFileOffset;
        }
        
        // データを加工する(RAWモード)
        if (messageParsingMode == PARSING_RAW)
        {
            if (kanjiMode == a2BsubjectDataParser.PARSE_EUC)
            {
            	// EUC 漢字モード (EUCからSJへ変換する)
                parsedBuffer = new byte[(parseSize * 2) + 60];
                stringConverter.StrCopyEUCtoSJ(parsedBuffer, 0, buff, offset, parseSize);
                return (parsedBuffer);
            }
            parsedBuffer = new byte[parseSize + 2];
            System.arraycopy(buff, offset, parsedBuffer, 0, parseSize);
            return (parsedBuffer);
        }
        
        // データを加工する(通常/HTML解析モード)
        byte[] originBuffer = null;
        if (kanjiMode == a2BsubjectDataParser.PARSE_EUC)
        {
        	// EUC 漢字モード (EUCからSJへ変換する)
            originBuffer = new byte[(parseSize * 2) + 60];
            stringConverter.StrCopyEUCtoSJ(originBuffer, 0, buff, offset, parseSize);
            parsedBuffer = new byte[(parseSize * 2) + 60];
            parseMessage(parsedBuffer, originBuffer, 0, parseSize, number);
            originBuffer = null;
            return (parsedBuffer);
        }
        // Shift JIS漢字モード or UTF8漢字モード
        parsedBuffer = new byte[(parseSize * 2) + 60];
        parseMessage(parsedBuffer, buff, offset, parseSize, number);
        return (parsedBuffer);
    }    

    // 特定のレス内に含まれるアンカー数を取得する
    public int getNumberOfAnchors(int number)
    {
        return (0);        
    }
    
    // レスに含まれるアンカー番号を取得する
    public int getAnchorNumber(int index)
    {
        return (0);
    }

    // あぼーん実施
    public void doAbone()
    {
        nameField  = "* * *";
        emailField = "";
    }

    // 名前欄を取得
    public String getNameField()
    {
        return (nameField);
    }

    // eMail欄を取得
    public String getEmailField()
    {
        return (emailField);
    }

    /*
     *   スレタイトルを取得する
     * 
     */    
    public byte[] getThreadTitle()
    {
        return (threadMessageTitle);
    }

    /*
     *   スレタイトル長を取得する
     * 
     */
    public int getThreadTitleLength()
    {
        return (threadMessageLength);
    }
    
    /*
     *   スレタイトルを抽出する
     * 
     */
    private void pickupThreadTitle(byte[] buff, int readSize)
    {
        // インデックスがなかった場合には抽出しない
        if (numberOfIndex < 1)
        {
            return;
        }
        
        for (int index = bufferIndex[0]; index > 0; index--)
        {
            if (buff[index] == '>')
            {
                index++;
                int length = (bufferIndex[0] - index);
                
                threadMessageTitle = new byte[length + 4];
                int loopCnt = 0;
                while (loopCnt < length)
                {
                    threadMessageTitle[loopCnt] = buff[index];
                    index++;
                    loopCnt++;
                    if (buff[index] == 0x0a)
                    {
                        // 改行コードで区切る...
                        break;
                    }
                }
                threadMessageTitle[length] = 0;
                threadMessageLength = loopCnt;
                index = -1;
                break;
            }
        }
        return;
    }

    // ファイルからデータを読み出す...
    private boolean loadFromFile(int offset, int parseSize)
    {
        FileConnection  dataFileConnection = null;
        int start = 0;
        if (((offset + parseSize) < readBufferSize)||
             (offset < (readBufferSize / 2))||
             (dataFileSize < readBufferSize))
        {
            // データの先頭から読み出す...
            start = 0;
        }
        else if (offset > (dataFileSize - readBufferSize))
        {
            // データの末尾部分を読み出す...
            start = (int) dataFileSize - readBufferSize;
        }
        else
        {
            // データの真ん中を読み出す...
            start = offset - (readBufferSize / 2);
        }

        // ファイルのオープン、ファイルサイズの特定...
        try
        {
            dataFileConnection = (FileConnection) Connector.open(readFileName, Connector.READ);
        }
        catch (Exception e)
        {
            // ファイルオープン失敗...
            System.gc();
            return (false);
        }

        //////  データをstartから readBufferSize分読み出す...  /////
        InputStream  in = null;
        try
        {
            in = dataFileConnection.openInputStream();

            // ファイルの先頭まで読み出し...
            if (start != 0)
            {
                //in.skip(start);

                // in.skip(start)の処理があまりにも遅かったので、、、カラ読み処理。                
                int startOffset = start;
                while (startOffset > 0)
                {
                    int readSize = 0;
                    if (startOffset > readBufferSize)
                    {
                        readSize = in.read(buff, 0, readBufferSize);
                    }
                    else
                    {
                        readSize = in.read(buff, 0, startOffset);
                    }
                    startOffset = startOffset - readSize;
                }
            }
            currentReadSize = in.read(buff, 0, readBufferSize);
            in.close();
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

            // なんだか解析に失敗...
            currentReadSize = 0;

            // ファイルがオープンしっぱなしだったらクローズする
            try
            {
                if (dataFileConnection.isOpen() == true)
                {
                    dataFileConnection.close();
                }
                dataFileConnection = null;
            }
            catch (Exception ez)
            {
                // 例外が発生したときは何もしない
            }
            System.gc();
            return (false);
        }

        // ファイルの先頭を設定する
        currentFileOffset = start;

        // ファイルがオープンしっぱなしだったらクローズする
        try
        {
            if (dataFileConnection.isOpen() == true)
            {
                dataFileConnection.close();
            }
            dataFileConnection = null;
        }
        catch (Exception e)
        {
            // 例外が発生したときは何もしない
        }
        System.gc();
        return (true);
    }
    
    /*=========================================================================*/
    /*   Function : ParseMessage　　                                           */
    /*                                       (メッセージパース処理...NNsiより) */
    /*=========================================================================*/
    private void parseMessage(byte[] dst, byte[] src, int offset, int size, int number)
    {
        int dataStatus = 0;
        int dstIndex  = 0;
        int srcIndex  = offset;
        int previousIndex = 0;

        // 超遅い解釈ルーチンかも... (１文字づつパースする)
        while (srcIndex < (offset + size))
        {
            if (src[srcIndex] == '&')
            {
                // "&gt;" を '>' に置換
                if ((src[srcIndex + 1]== 'g')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
                {
                    dst[dstIndex] = '>';
                    dstIndex++;
                    srcIndex = srcIndex + 4;

                    // TODO ここにレス参照番号のpick up ロジックを入れる
                    continue;
                }

                // "&lt;" を '<' に置換
                if ((src[srcIndex + 1]== 'l')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
                {
                    dst[dstIndex] = '<';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }

                // "&quot;" を '"' に置換
                if ((src[srcIndex + 1] == 'q')&&(src[srcIndex + 2] == 'u')&&(src[srcIndex + 3] == 'o')&&
                    (src[srcIndex + 4] == 't')&&(src[srcIndex + 5] == ';'))
                {
                    dst[dstIndex] = '"';
                    dstIndex++;
                    srcIndex = srcIndex + 6;
                    continue;
                }

                // "&nbsp;" を '    ' に置換 (AA表示モードのために、スペース1個にするよう変更)
                if ((src[srcIndex + 1] == 'n')&&(src[srcIndex + 2] == 'b')&&(src[srcIndex + 3] == 's')&&
                        (src[srcIndex + 4] == 'p')&&(src[srcIndex + 5] == ';'))
                {
                        dst[dstIndex] = ' ';
                        dstIndex++;
/*
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dst[dstIndex] = ' ';
                        dstIndex++;
*/
                        srcIndex = srcIndex + 6;
                        continue;
                }

                // "&amp;" を '&' に置換
                if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'm')&&(src[srcIndex + 3] == 'p')&&
                    (src[srcIndex + 4] == ';'))
                {
                    dst[dstIndex] = '&';
                    dstIndex++;
                    srcIndex = srcIndex + 5;
                    continue;
                }
                // "&alpha;" を 'α' に置換
                if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'l')&&
                    (src[srcIndex + 3] == 'p')&&(src[srcIndex + 4] == 'h')&&
                    (src[srcIndex + 5] == 'a')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xbf;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(TAG_ALPHA);
                    continue;
                }
                 // "&beta;" を 'β' に置換
                if ((src[srcIndex + 1] == 'b')&&(src[srcIndex + 2] == 'e')&&
                    (src[srcIndex + 3] == 't')&&(src[srcIndex + 4] == 'a')&&
                    (src[srcIndex + 5] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xc0;
                    dstIndex++;
                    srcIndex = srcIndex + 6;    // StrLen(TAG_BETA);
                    continue;
                }
                // "&hellip;" を '･･･' に置換
                if ((src[srcIndex + 1] == 'h')&&(src[srcIndex + 2] == 'e')&&
                    (src[srcIndex + 3] == 'l')&&(src[srcIndex + 4] == 'l')&&
                    (src[srcIndex + 5] == 'i')&&(src[srcIndex + 6] == 'p')&&
                    (src[srcIndex + 7] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0x63;
                    dstIndex++;
                    srcIndex = srcIndex + 8;    // StrLen(TAG_...)
                    continue;
                }
                // "&rarr;" を '→' に置換
                if ((src[srcIndex + 1] == 'r')&&(src[srcIndex + 2] == 'a')&&
                    (src[srcIndex + 3] == 'r')&&(src[srcIndex + 4] == 'r')&&
                    (src[srcIndex + 5] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xa8;
                    dstIndex++;
                    srcIndex = srcIndex + 6;    // StrLen(→);
                    continue;
                }
                // "&acute;" を '´' に置換
                if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'c')&&
                    (src[srcIndex + 3] == 'u')&&(src[srcIndex + 4] == 't')&&
                    (src[srcIndex + 5] == 'e')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0x4c;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(´);
                    continue;
                }
                 // "&omega;" を 'ω' に置換
                if ((src[srcIndex + 1] == 'o')&&(src[srcIndex + 2] == 'm')&&
                    (src[srcIndex + 3] == 'e')&&(src[srcIndex + 4] == 'g')&&
                    (src[srcIndex + 5] == 'a')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xd6;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(´);
                    continue;
                }
                 // "&rdquo;" を '”' に置換
                if ((src[srcIndex + 1] == 'r')&&(src[srcIndex + 2] == 'd')&&
                    (src[srcIndex + 3] == 'q')&&(src[srcIndex + 4] == 'u')&&
                    (src[srcIndex + 5] == 'o')&&(src[srcIndex + 6] == ';'))
                {
                    dst[dstIndex] = (byte) 0x81;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0x68;
                    dstIndex++;
                    srcIndex = srcIndex + 7;    // StrLen(´);
                    continue;
                }
                //  "&Pi;" を 'Π' に置換
                if ((src[srcIndex + 1] == 'P')&&(src[srcIndex + 2] == 'i')&&
                    (src[srcIndex + 3] == ';'))
                {
                    dst[dstIndex] = (byte) 0x83;
                    dstIndex++;
                    dst[dstIndex] = (byte) 0xae;
                    dstIndex++;
                    srcIndex = srcIndex + 4;    // StrLen(´);
                    continue;
                }

                // これはありえないはずだが...一応。
                dst[dstIndex] = '&';
                dstIndex++;
                srcIndex++;
                continue;
            }
            if (src[srcIndex] == '<')
            {
                //  "<>" は、セパレータ(状態によって変わる)
                if (src[srcIndex + 1] == '>')
                {
                    srcIndex = srcIndex + 2;
                    if (messageParsingMode != PARSING_NORMAL)
                    {
                        // スレ解析モードは、スペース１つに変換する...
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        continue;
                    }
                    switch (dataStatus)
                    {
                      case 0:
                        // 名前欄の区切り
                        nameField = stringConverter.parsefromSJ(dst, previousIndex, (dstIndex - previousIndex));
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dst[dstIndex] = ' ';
                        dstIndex++;
                        dataStatus = 1;
                        previousIndex = dstIndex;
                        break;

                      case 1:
                        // e-mail欄の区切り
                        emailField = stringConverter.parsefromSJ(dst, previousIndex, (dstIndex - previousIndex));
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                        dataStatus = 2;
                        previousIndex = dstIndex;
                        break;

                      case 2:
                        // 年月日・時間およびＩＤ欄の区切り
                        //dateTimeField = new String(dst, previousIndex, (dstIndex - previousIndex));
                        //dst[dstIndex] = '\n';
                        //dstIndex++;
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                        dataStatus = 3;
                        previousIndex = dstIndex;

                        // 先頭にあるスペース...
                        if (src[srcIndex] == ' ')
                        {
                            srcIndex++;  // ← レスの先頭に入っているスペースを除去する
                        }
                        break;

                      case 3:
                      default:
                        // メッセージの終端
                        //messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
                        dst[dstIndex] = '\0';
                        dstIndex++;
                        //previousIndex = dstIndex;
                        return;
                        //break;
                    }
                    continue;
                }

                //  "<br>" は、改行に置換
                if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
                    ((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    // 行末と行頭のスペースを削ってみる場合
                    if ((srcIndex != 0)&&((srcIndex -1) == ' '))
                    {
                        dstIndex--;
                    }
                    if (src[srcIndex + 4] == ' ')
                    {
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                        srcIndex = srcIndex + 5;
                        continue;
                    }
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }

                //  "<p*>" は、改行2つに置換
                if ((src[srcIndex + 1] == 'p')||(src[srcIndex + 1] == 'P'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 2;
                    while (src[srcIndex] != '>')
                    {
                        srcIndex++;
                        if (srcIndex >= (offset + size))
                        {
                            // 確保した領域を越えた...
                            break;
                        }
                    }
                    srcIndex++;
                    continue;
                }

                // <li>タグを改行コードと - に置換する
                if (((src[srcIndex + 1] == 'l')||(src[srcIndex + 1] == 'L'))&&
                    ((src[srcIndex + 2] == 'i')||(src[srcIndex + 2] == 'I'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '-';
                    dstIndex++;
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }
                   
                //  "<hr>" は、改行 === 改行 に置換
                if (((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))&&
                    ((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '=';
                    dstIndex++;
                    dst[dstIndex] = '=';
                    dstIndex++;
                    dst[dstIndex] = '=';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }

                //  "<dt>" は、改行に置換
                if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
                    ((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    if (dst[dstIndex - 1] != '\n')
                    {
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                    }
                    srcIndex = srcIndex + 4;
                    continue;
                }

                //  "</tr>" および "</td>" は、改行に置換
                if (((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
                    ((src[srcIndex + 3] == 'r')||(src[srcIndex + 3] == 'R'))||
                    ((src[srcIndex + 3] == 'd')||(src[srcIndex + 3] == 'D'))&&
                    (src[srcIndex + 1] == '/')&&(src[srcIndex + 4] == '>'))
                {
                    if (dst[dstIndex - 1] != '\n')
                    {
                        dst[dstIndex] = '\r';
                        dstIndex++;
                        dst[dstIndex] = '\n';
                        dstIndex++;
                    }
                    srcIndex = srcIndex + 5;
                    continue;
                }

                //  "<dd>" は、改行と空白2つに置換
                if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
                    ((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D'))&&
                    (src[srcIndex + 3] == '>'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    srcIndex = srcIndex + 4;
                    continue;
                }
                
                //  "<h*>" は、改行 + 改行 に置換
                if ((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;

                    // TODO: 強調色の設定...

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 2;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                //  "</h*>" は、改行 + 改行 に置換
                if ((src[srcIndex + 1] == '/')&&((src[srcIndex + 2] == 'h')||(src[srcIndex + 2] == 'H')))
                {
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;
                    dst[dstIndex] = '\r';
                    dstIndex++;
                    dst[dstIndex] = '\n';
                    dstIndex++;

                    // TODO: 強調色の解除...

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                //  "<a*>" で、アンカー色の検出
                if (((src[srcIndex + 1] == 'a')||(src[srcIndex + 1] == 'A'))&&(src[srcIndex + 2] == ' '))
                {
                    // TODO: アンカー色の設定...

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                //  "</a*>" で、アンカー色の解除
                if (((src[srcIndex + 2] == 'a')||(src[srcIndex + 2] == 'A'))&&(src[srcIndex + 1] == '/'))
                {
                    // TODO: アンカー色の解除

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                     continue;
                }

                // <EM* は強調色へ
                if (((src[srcIndex + 1] == 'e')||(src[srcIndex + 1] == 'E'))&&
                    ((src[srcIndex + 2] == 'm')||(src[srcIndex + 2] == 'M')))
                {
                    // TODO: 強調色へ切り替え

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // </em* は通常色へ
                if (((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
                    ((src[srcIndex + 3] == 'm')||(src[srcIndex + 3] == 'M'))&&
                    (src[srcIndex + 1] == '/'))
                {
                    // TODO: 強調色の解除

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // <tr>, <td> はスペース１つに変換
                if (((src[srcIndex + 1] == 't')||(src[srcIndex + 1] == 'T'))&&
                    ((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))||
                    ((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D')))
                {
                    dst[dstIndex] = ' ';
                    dstIndex++;

                    // タグは読み飛ばす
                    srcIndex = srcIndex + 3;
                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // <BE: は、BE IDのため読み飛ばさない。(画面表示する)
                if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
                    ((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
                    (src[srcIndex + 3] == ':'))
                {
                    dst[dstIndex] = ' ';
                    dstIndex++;
                    srcIndex++;

                    while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
                        dst[dstIndex] = src[srcIndex];
                        dstIndex++;
                        srcIndex++;
                    }
                    srcIndex++;
                    continue;
                }

                // その他のタグは読み飛ばす
                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                {
                    srcIndex++;
                }
                // タグは完全無視とする
                //dst[dstIndex] = ' ';
                //dstIndex++;
                srcIndex++;
                continue;
            }

            // 半角カナ→全角カナ変換処理を実施する場合...
            if ((convertHanzen == true)&&(src[srcIndex] < 0))
            {
                int checkChar = 256 + src[srcIndex];
                if ((checkChar >= 0x81)&&(checkChar <= 0x9f))
                {
                    // 2バイト文字と判定
                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;

                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;
                    continue;
                }

                if ((checkChar >= 0xe0)&&(checkChar <= 0xef))
                {
                    // 2バイト文字と判定
                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;

                    dst[dstIndex] = src[srcIndex];
                    dstIndex++;
                    srcIndex++;
                    continue;
                }
                // 半角カナ → 全角カナ変換を無効化する。
/**
                if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
                {
                    // カタカナ文字と判定、テーブルを使って半角カナ→全角変換する
                    short code = Han2ZenTable[checkChar - 0xa1];
                    byte temp;

                    temp = (byte) ((code & 0xff00) >> 8);
                    dst[dstIndex] = temp;
                    dstIndex++;

                    temp = (byte) ((code & 0x00ff));
                    dst[dstIndex] = temp;
                    dstIndex++;
                                        
                    srcIndex++;
                    continue;
                }
**/
            }

            if ((messageParsingMode != PARSING_NORMAL)&&((src[srcIndex] == 0x0d)||(src[srcIndex] == 0x0a)))
            {
                // テキスト表示モードのときには、改行コードを普通の一文字転写する。
                srcIndex++;
                dst[dstIndex] = '\r';
                dstIndex++;
                dst[dstIndex] = '\n';
                dstIndex++;
                continue;
            }
            
            // スペースが連続していた場合、１つに減らす
            if ((src[srcIndex] == ' ')&&(src[srcIndex + 1] == ' '))
            {
                srcIndex++;
                while ((src[srcIndex] == ' ')&&(srcIndex < (offset + size)))
                {
                    srcIndex++;
                }
                srcIndex--;
            }

            // NULL および 0x0a, 0x0d は無視する
            if ((src[srcIndex] != '\0')&&(src[srcIndex] != 0x0a)&&(src[srcIndex] != 0x0d)&&(src[srcIndex] != 0x09))
            {
                // 普通の一文字転写
                dst[dstIndex] = src[srcIndex];
                dstIndex++;
            }
            srcIndex++;
        }
        //  レスの終端...
        //messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
        dst[dstIndex] = '\0';
        dstIndex++;
        return;
    }

    /*=========================================================================*/
    /*   Function : CreateThreadIndexSub                                       */
    /*                                                                         */
    /*                                   (インデックス取得の実処理...NNsiより) */
    /*=========================================================================*/
    private void createThreadIndexSub(int offset, byte[] buffer, int bufferLength)
    {

        // 現在のメッセージ数を取得する
        int index = 0;
        while (index < bufferLength)
        {
            // メッセージの区切りを探す
            while ((buffer[index] != '\n')&&(buffer[index] != '\0'))
            {
                index++;
            }
            if (buffer[index] != '\0')
            {
                index++;
                if (numberOfIndex < messageLimit)
                {
                    bufferIndex[numberOfIndex] = (offset + index);
                    numberOfIndex++;
                }
                else
                {
                    // 格納エリア不足...何もせず応答する
                    return;
                }
            }
            else
            {
                // 区切りがない(データの末尾？ 壊れている？)、とりあえず次にすすむ
                index++;
            }
        }
        // とりあえず、末尾に末尾を入れる
        bufferIndex[numberOfIndex] = (offset + index);
        return;
    }

    /*=========================================================================*/
    /*   Function : CreateThreadIndexSub                                       */
    /*                                                                         */
    /*                                   (インデックス取得の実処理...NNsiより) */
    /*=========================================================================*/
    private void createThreadIndexSub_TextParsing(int offset, byte[] buffer, int bufferLength)
    {
        // 現在のメッセージ数を取得する
        int index = 0;
        while (index < bufferLength)
        {
            int before = index;
            index = index + separatorSize;

            // メッセージの区切りを探す (なければlimitで切る)
            while ((index < (before + separatorLimit))&&
                    (index < bufferLength)&&
                    (buffer[index] != '\n')&&
                    (buffer[index] != '\r')&&
                    (buffer[index] != '\0'))
            {
                index++;
            }
            if ((index < bufferLength)&&(buffer[index] != '\0'))
            {
                index++;
                if (numberOfIndex < messageLimit)
                {
                    bufferIndex[numberOfIndex] = (offset + index);
                    numberOfIndex++;
                }
                else
                {
                    // 格納エリア不足...何もせず応答する
                    return;
                }
            }
        }

        // とりあえず、末尾に末尾を入れる
        bufferIndex[numberOfIndex] = (offset + index);
        numberOfIndex++;
        return;
    }
}

//--------------------------------------------------------------------------
//   a2BMessageParser  --- スレメッセージパーサ 
//                                        (datファイルの解析とデータ保持)
//
//
//--------------------------------------------------------------------------
//   MRSa (mrsa@sourceforge.jp)
