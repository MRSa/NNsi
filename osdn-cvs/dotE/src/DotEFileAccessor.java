import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;

/**
 *   Bitmapファイルの読み出し・書き込みロジック
 *   
 *   
 * 
 * @author MRSa
 *
 */
public class DotEFileAccessor
{
	private MidpFileOperations fileAccessor = null;
    private DotEDataStorage  dataStorage = null;

    private String            dataFileName = null;	
    
    // 暫定仕様... 32x32版 24bitClor BITMAPのヘッダ。(面倒くさくなった...)
    private final byte[] bmp32x32Header = { 
  		  0x42, 0x4D, 0x36, 0x0C, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x36, 0x00, 0x00, 0x00, 0x28, 
  		  0x00, 0x00, 0x00, 0x20, 0x00, 
  		  0x00, 0x00, 0x20, 0x00, 0x00, 
  		  0x00, 0x01, 0x00, 0x18, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x0C, 0x00, 0x00, 0x00, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x00, 0x00, 0x00, 0x00, 0x00, 
  		  0x00, 0x00, 0x00, 0x00};

    /**
     *  コンストラクタ
     *  
     */
    public DotEFileAccessor()
    {
    	//
    	fileAccessor = new MidpFileOperations();
    }

    /**
     *  クラスの準備
     */
    public boolean prepare(DotEDataStorage dataSt)
    {
    	dataStorage = dataSt;
    	if ((fileAccessor == null)||(dataStorage == null))
    	{
    		return (false);
    	}
    	return (true);
    }
    
    /**
     *  データを書き込む
     *  
     *  @param targetFileName 出力ファイル名
     */
    public void saveToFile(String targetFileName)
    {
    	dataFileName = targetFileName;
        Thread thread = new Thread()
        {
            public void run()
            {
                //
            	outputBitmapData();
            }
        };
        
        try
        {
            thread.start();  // 画面の切り替え処理...
            thread.join();   // 終了するまで待つ...
        }
        catch (Exception ex)
        {
            // 何もしない...
        }

        // あとしまつ
        thread = null;
        System.gc();
        return;
    }

    /**
     *   ファイルに保存したビットマップデータを読み出す
     *   
     *   @param targetFileName  読み出すファイル名
     */
    public void loadToFile(String targetFileName)
    {
    	dataFileName = targetFileName;
        Thread thread = new Thread()
        {
            public void run()
            {
                //
            	readBitmapData();
            }
        };
        
        try
        {
            thread.start();  // 画面の切り替え処理...
            thread.join();   // 終了するまで待つ...
        }
        catch (Exception ex)
        {
            // 何もしない...
        }

        // あとしまつ
        thread = null;
        System.gc();
        return;
    }

    
    /**
     *  ビットマップデータを出力する
     * @return 出力成功・失敗
     */
    public boolean outputBitmapData()
    {
    	byte[] buffer = new byte[3126];
    	System.arraycopy(bmp32x32Header, 0, buffer, 0, bmp32x32Header.length);
        int areaIndex = bmp32x32Header.length;
        
        // 面倒だから...32x32にしてしまう。(データを格納する)
        for (int y = 31; y >=0 ; y--)
        {
            for (int x = 0; x < 32; x++)
            {
            	int data = dataStorage.getData(x, y);
            	byte red =   (byte) (((data & 0x00ff0000) >> 16) & 0x000000ff);
            	byte green = (byte) (((data & 0x0000ff00) >> 8)  & 0x000000ff);
            	byte blue =  (byte) (data & 0x000000ff);
            	buffer[areaIndex] = blue;
            	areaIndex++;
            	buffer[areaIndex] = green;
            	areaIndex++;
            	buffer[areaIndex] = red;
            	areaIndex++;
            }
        }

        // ファイルに吐き出す
        return (fileAccessor.outputRawDataToFile(dataFileName, buffer, false));
    }

    /**
     *  保存しているデータを読み出す
     * @return  読み出し成功・失敗
     */
    public boolean readBitmapData()
    {
    	try
    	{
    	    byte[] buffer = fileAccessor.ReadFileWhole(dataFileName);
    	    int index = bmp32x32Header.length;

    	    // 面倒だから...32x32にしてしまう。(データを格納する)
            for (int y = 31; y >=0 ; y--)
            {
                for (int x = 0; x < 32; x++)
                {
                    byte blue = buffer[index];
                    index++;
                    byte green = buffer[index];
                    index++;
                    byte red = buffer[index];
                    index++;

                    int data = (((red & 0xff)<< 16) | ((green & 0xff) << 8) | (blue & 0xff));
                    dataStorage.setData(x, y, data);
                }
            }
    	}
    	catch (Exception ex)
    	{
    		// なにもしない
    		return (false);
    	}
    	return (false);
    }
}
