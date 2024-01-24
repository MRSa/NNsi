import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 *  ローカルあぼーんキーワード管理クラス
 * @author MRSa
 *
 */
public class a2BNgWordManager
{
    // オブジェクト
    private a2BMain         parent        = null;
    private a2Butf8Conveter utf8Converter = null;
    private a2BsjConverter  sjConverter   = null;

    private final String   ngWordFile    = "NGWORD.CSV";
    private final int      bufferMargin   = 16; 
    
    private Vector          ngWordList    = null;

    public  final int CHECKING_ALL   = 0;  // スレ全体でチェックする
    public  final int CHECKING_NAME  = 1;  // 名前欄でチェックする
    public  final int CHECKING_EMAIL = 2;  // email欄でチェックする

    /**
     * コンストラクタ
     * @param object     親クラス
     * @param converter  SJ -> utf8変換クラス
     */
    public a2BNgWordManager(a2BMain object, a2Butf8Conveter converter, a2BsjConverter converterSJ)
    {
        // 親を記録する...
        parent = object;
        
        // コンバータの登録
        utf8Converter = converter;
        sjConverter   = converterSJ;
        
        ngWordList = new Vector();
    }

    /**
     *   NGキーワードリストを展開する
     *
     */
    public void restore()
    {
        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // ファイル名を生成する
        String fileName = parent.getBaseDirectory() + ngWordFile;
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
        }
        catch (Exception e)
        {
            // 例外発生！！！
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(ngWord)", fileName + " EXCEption  e:" + e.getMessage());
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
        in = null;
        connection = null;

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

    /**
     *   NGキーワードリストを保管する
     *
     */
    public void backup()
    {
        FileConnection connection = null;
        OutputStream  out = null;
        
        // ファイル名を生成する
        String fileName = parent.getBaseDirectory() + ngWordFile;
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
            String comment = ";; 0, 0, 0,(ngWord)\n";
            out.write(comment.getBytes());

            Enumeration cnt = ngWordList.elements();
            while (cnt.hasMoreElements())
            {
                a2BngWord data = (a2BngWord) cnt.nextElement();
                if (data != null)
                {
                    String outputData = data.checkingType + ", 0, 0,";
                    out.write(outputData.getBytes());
                    out.write(data.getWord());
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
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(output NG word)", " Exception  ee:" + e.getMessage());
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
        ngWordList = null;
        System.gc();
    }

    /**
     *  NGワード数を応答を返す
     *
     */
    public int numberOfNgWords()
    {
        return (ngWordList.size());
    }


    /**
     *  NGワードのチェックタイプを応答する
     *
     *  @param  index
     */
    public int getNgCheckingType(int index)
    {
        if ((index < 0)||(index >= ngWordList.size()))
        {
            return (-1);
        }
        a2BngWord ngWord = (a2BngWord) ngWordList.elementAt(index);
        if (ngWord == null)
        {
            return (-1);
        }
        return (ngWord.checkingType);
    }
    
    /**
     *  NGワードを応答する
     *
     */
    public String getNgWord(int index)
    {
        if ((index < 0)||(index >= ngWordList.size()))
        {
            return (null);
        }
        a2BngWord ngWord = (a2BngWord) ngWordList.elementAt(index);
        if (ngWord == null)
        {
            return (null);
        }
        return (utf8Converter.parsefromSJ(ngWord.getWord()));
    }

    /**
     *  NGワードを登録する
     *
     */
    public void entryNgWord(String ngWord, int checkType)
    {
        a2BngWord dataHolder    = new a2BngWord(sjConverter.parseToSJ(ngWord), checkType);
        ngWordList.addElement(dataHolder);
        return;
    }
    
    /**
     *   ngword.csv から Vectorへデータを展開する...
     *   (かなりベタベタ...)
     * 
     */
    private int pickupData(byte[] buffer, int offset, int limit)
    {
        int length = 0;

        // データがないとき...
        if (offset + 7 > limit)
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

        // checkTypeを抽出する
        int checkType = 0;
        while ((buffer[nameOffset + nameLength] >= '0')&&(buffer[nameOffset + nameLength] <= '9'))
        {
            checkType = checkType * 10 + (buffer[nameOffset + nameLength] - '0');
            nameLength++;
        }        
        while (buffer[nameOffset + nameLength] != 44)
        {
            nameLength++;
        }
        length = (nameOffset + nameLength + 1) - offset;

        // ダミー領域１
        int dummy1 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy1 = dummy1 * 10 + (buffer[offset + length] - '0');
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

        // 空白を読み飛ばす
        while (buffer[offset + length] == ' ')
        {
            length++;
        }

        // NGワードを抽出する
        nameOffset = offset + length;
        nameLength = 0;
        while ((buffer[nameOffset + nameLength] != 0)&&(buffer[nameOffset + nameLength] != 0x0d)&&(buffer[nameOffset + nameLength] != 0x0a))
        {
            nameLength++;
        }
        
        length = (nameOffset + nameLength) - offset;
        while (buffer[offset + length] != 10)
        {
            length++;
        }

        //  データをVector領域に格納する
        a2BngWord dataHolder    = new a2BngWord(buffer, nameOffset, nameLength, checkType);
        ngWordList.addElement(dataHolder);

        return (length);
    }

    /**
     *  NGキーワード保持クラス
     * 
     * @author MRSa
     *
     */
    public class a2BngWord
    {
        private final int MARGIN         = 2;  // あまり領域の確保
        public  int     checkingType      = CHECKING_ALL;  // キーワード種別
        private byte[]  ngWord            = null;          // キーワード
//        private int     wordSize          = 0;

        /**
         *  コンストラクタ
         */
        public a2BngWord(byte[] word, int checkType)
        {
            int len = word.length;
            ngWord = new byte[len + MARGIN];
            System.arraycopy(word, 0, ngWord, 0, len);
            ngWord[len]  = 0;
//            wordSize     = len;
            checkingType = checkType;
        }

        /**
         *  コンストラクタ
         */
        public a2BngWord(byte[] word, int offset, int len, int checkType)
        {
            ngWord = new byte[len + MARGIN];
            System.arraycopy(word, offset, ngWord, 0, len);
            ngWord[len] = 0;
//            wordSize = len;
            checkingType = checkType;
        }

        /**
         *  NGワードを応答する
         */
        public byte[] getWord()
        {
            return (ngWord);
        }
    }
}

//--------------------------------------------------------------------------
//  a2BNgWordManager  --- ローカルあぼーんデータ管理
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
