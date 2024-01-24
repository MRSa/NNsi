import java.util.Vector;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpKanjiConverter;

/**
 *  テレビ局のデータ
 * 
 * @author MRSa
 *
 */
public class TVgStationProgramData
{
    private final int MARGIN = 16;
    private int minutes = 0;
    private String stationName = null;
    Vector programData = null;
    
    /**
     *  コンストラクタでは、TV局名を設定します
     */
    public TVgStationProgramData(String station)
    {
        stationName = station;
        programData = new Vector();
    }

    /**
     *  TV局名を応答します
     * @return TV局名
     */
    public String getStationName()
    {
        return (stationName);
    }

    /**
     *  番組データを設定します。
     *  （ここでタグ情報を省略して格納します。）
     * @param hourHead    時間のヘッダ
     * @param data        番組データ(バッファ)
     * @param startIndex  番組データの先頭インデックス
     * @param endIndex    番組データの末尾インデックス
     * @param dataMinutes 番組データの長さ(単位:分)
     */
    public void SetStationData(byte[] data, int startIndex, int endIndex, int dataMinutes)
    {
        int dstIndex = 0;
        byte[] entryData = null;
        try
        {
            int dataType = -1;
        	int limitSize = endIndex - startIndex;
            entryData = new byte[limitSize + MARGIN];
            for (int index = startIndex; index < endIndex; index++)
            {
                // タグ消し
                if (data[index] == '<')
                {
                    while ((index < endIndex)&&(data[index] != '>'))
                    {
                        if ((data[index + 0] == '\"')&&(data[index + 1] == 't')&&(data[index + 2] == 'i')&&(data[index + 3] == 'm')&&
                                (data[index + 4] == 'e')&&(data[index + 5] == '-')&&(data[index + 6] == 'r')&&(data[index + 7] == 'a')&&
                                (data[index + 8] == 'n')&&(data[index + 9] == 'g')&&(data[index + 10] == 'e'))
                        {
                            // 時刻データの先頭を検出... (time-range) : 読み飛ばす
                            index = index + 11;
                            while (data[index] != '>')
                            {
                                index++;
                            }
                            index++;
                        }
                        else if ((data[index + 0] == 'a')&&(data[index + 1] == 'l')&&(data[index + 2] == 't')&&(data[index + 3] == '=')&&(data[index + 4] == '\"'))
                        {
                        	// alt="..."(代替テキスト)を展開する
                            index = index + 5;
                            while (data[index] != '\"')
                            {
                            	entryData[dstIndex] = data[index];
                            	dstIndex++;
                            	index++;
                            }
                        }
                        else if ((data[index + 0] == '\"')&&(data[index + 1] == 's')&&(data[index + 2] == 'c')&&(data[index + 3] == 'h')&&
                                (data[index + 4] == 'e')&&(data[index + 5] == 'd')&&(data[index + 6] == 'u')&&(data[index + 7] == 'l')&&
                                (data[index + 8] == 'e')&&(data[index + 9] == ' ')&&(data[index + 10] == 'g')&&(data[index + 11] == 'e')&&
                                (data[index + 12] == 'n')&&(data[index + 13] == 'r')&&(data[index + 14] == 'e')&&(data[index + 15] == '-'))
                        {
                            // データの先頭を検出... ("schedule genre-) : データタイプとして記録する
                        	int start = index + 16;
                            index = index + 16;
                            while (data[index] != '"')
                            {
                                index++;
                            }
                            dataType = Integer.parseInt(new String(data, start, (index - start)));
                        }
                        index++;
                    }
                    entryData[dstIndex] = ' ';
                    dstIndex++;
                    continue;
                }
                if ((data [index] == ' ')||(data [index] == 0x0d)||(data [index] == 0x0a)||(data [index] == '\t'))
                {
                    // スペース、改行コード、タブは消去する
                }
                else
                {
                    entryData[dstIndex] = data[index];
                    dstIndex++;
                }
            }
            TvProgramData content = new TvProgramData(new String(entryData, 0, dstIndex), minutes, dataMinutes, dataType);
            programData.addElement(content);
        }
        catch (Exception ex)
        {
            programData.addElement(ex.getMessage());
        }
        minutes = minutes + dataMinutes;
        System.gc();
        return;
    }

    /**
     *  格納しているデータ数（番組数）を応答します
     * @return 格納しているデータ数
     */
    public int numberOfDatas()
    {
        return (programData.size());
    }

    /**
     * TV番組データを取得する
     * @param index インデックス番号
     * @return 番組データ
     */
    public String getProgramData(int index, boolean isRangeTime)
    {
        TvProgramData data = (TvProgramData) programData.elementAt(index);
        return (data.getDataContent(isRangeTime));
    }

    /**
     *  TV番組データを取得する (時刻指定で取得)
     * @param startMinutes 開始時刻(分単位で、午前5時からの経過分）
     * @return 番組データ
     */
    public String getProgram(int startMinutes, boolean isRangeTime)
    {
        int min = 0;
        for (int lp = 0; lp < programData.size(); lp++)
        {
            TvProgramData data = (TvProgramData) programData.elementAt(lp);
            min = min + data.getDataMinutes();
            if (min > startMinutes)
            {
                return (data.getDataContent(isRangeTime));
            }            
        }
        return ("");
    }

    /**
     *  TV番組データの先頭インデックスを取得する (時刻指定で取得)
     * @param startMinutes 開始時刻(分単位で、午前5時からの経過分）
     * @return 先頭インデックス
     */
    public int getProgramIndex(int startMinutes)
    {
        int min = 0;
        for (int lp = 0; lp < programData.size(); lp++)
        {
            TvProgramData data = (TvProgramData) programData.elementAt(lp);
            min = min + data.getDataMinutes();
            if (min > startMinutes)
            {
                return (lp);
            }            
        }
        return (0);
    }

    /**
     *  データの"分"を取得する
     * @return 記録されている "分"
     */
    public int getMinutes()
    {
        return (minutes);
    }

    /**
     *  キャッシュしていたデータファイルを読み出す
     * @param fileName
     * @return <code>true</code>:読み出し成功 / <code>false</code>:読み出し失敗
     */
    public boolean constructFromFile(String fileName)
    {
        if (MidpFileOperations.IsExistFile(fileName) == false)
        {
            // キャッシュファイルがない...終了する
        	return (false);
        }
    	
    	return (false);
    }
    
    /**
     *  データをキャッシュする（ファイルにデータを書き出す）
     * @param fileName
     */
    public void outputToFile(String fileName)
    {

        return;
    }
    
    /**
     *  テレビ番組のデータ（一件分）
     * 
     * @author MRSa
     */
    public class TvProgramData
    {
        private String dataContent = null;
        private int   dataMinutes = 0;
        private int   startMinute = 0;
        private int   dataType    = -1;

        /**
         *  コンストラクタでは、データを入れます
         * @param data 番組データ
         * @param hourHead 時間のヘッダ
         * @param minutes 番組の長さ
         * @param type データタイプ
         */
        public TvProgramData(String data, int start, int minutes, int type)
        {
            dataContent = data;
            dataMinutes = minutes;
            startMinute = start;
            dataType    = type;
        }

        /**
         *  番組データを取得する
         * @return 番組データ
         */
        private String convertTimeData(int minutes)
        {
            String appendH = ""; 
            String appendM = "";
            int hour = (minutes / 60 + 5) % 24;
            if (hour < 10)
            {
                appendH = "0";
            }
            int min  = minutes % 60;
            if (min < 10)
            {
                appendM = "0";
            }
            return (appendH + hour + ":" + appendM + min);
        }

        /**
         *  番組データを取得する
         * @return 番組データ
         */
        public String getDataContent(boolean isRange)
        {
            String data = "";
            if (isRange == false)
            {
                data = convertTimeData(startMinute); 
            }
            else
            {
                data = getProgramRange();
            }
            return (data + " " + dataContent);
        }

        /**
         *   番組の放送時間を取得する
         * 
         * @return
         */
        public String getProgramRange()
        {
            String data = convertTimeData(startMinute) + "-" + convertTimeData(startMinute + dataMinutes);
            return (data);
        }

        /**
         *  番組の長さを取得する
         * @return 番組の長さ(単位:分)
         */
        public int getDataMinutes()
        {
            return (dataMinutes);
        }
        
        /**
         *  番組のデータタイプを応答する
         * @return 番組データタイプ
         */        
        public int getDataType()
        {
        	return (dataType);
        }
    }
}
