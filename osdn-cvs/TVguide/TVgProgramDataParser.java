import jp.sourceforge.nnsi.a2b.frameworks.IDataProcessing;
import jp.sourceforge.nnsi.a2b.utilities.*;
import java.util.Vector;

/**
 *  TVgProgramDataParser
 * 
 * @author MRSa
 *
 */
public class TVgProgramDataParser
{
    private TVgDataStorage storageDb = null;

    private boolean dataFileForceUpdate = false;
    private boolean dataFileIsReady = false;
    private boolean dataConstructionIsReady = false;
    private String   dataFileName = null;
    private int      dataFileDate = 0;
    private String   dataString = "";
    
    private MidpFileOperations fileUtils = null;
    private MidpA2BhttpCommunication httpComm = null;
    private IDataProcessing dataProcess = null;
    private MidpDateUtils dateUtils = null;

    private Vector tvStationData = null;
    
    /**
     *   コンストラクタでは、データストレージクラスを記憶する
     *
     */
    public TVgProgramDataParser(TVgDataStorage storage, MidpA2BhttpCommunication httpUtil, IDataProcessing destination, MidpDateUtils dateUtil)
    {
        storageDb = storage;
        fileUtils = new MidpFileOperations();
        httpComm = httpUtil;
        dateUtils = dateUtil;
        dataProcess = destination;
    }

    /**
     *  データの開始
     * @return
     */
    public boolean start()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (false);
        }
        return (true);
    }    

    /**
     *   現在表示するテレビ番組表の地域と種別の文字列を応答する
     * @return テレビ番組表の地域と種別、テレビ局の文字列
     */
    public String getTvStationTitle()
    {
        return (storageDb.getAreaName() + " [" + storageDb.getTvTypeString() + "]");
    }

    /**
     *  テレビ局名を表示する
     * @return テレビ局名
     */
    public String getTvStationString()
    {
        return (getTvStationName(storageDb.getTvStation()));
    }

    /**
     *  選択中テレビ局を変更する
     * 
     * @param relativeNumber 変更するテレビ局のオフセット
     */
    public void changeTvStationIndex(int relativeNumber)
    {
        int stationId = storageDb.getTvStation() + relativeNumber;
        if (stationId < 0)
        {
        	stationId = tvStationData.size() - 1;
        }
        if (stationId >= tvStationData.size())
        {
        	stationId = 0;
        }
        storageDb.setTvStation(stationId);
    }

    /**
     *  テレビ局のIdを応答する
     * @return テレビ局Id
     */
    public int getTvStationIndex()
    {
        return (storageDb.getTvStation());
    }
    
    /**
     *  テレビ局数を応答する
     *
     * @return テレビ局数
     */
    public int getNumberOfTvStations()
    {
        return (tvStationData.size());
    }

    
    /**
     *  番組表ファイル名を取得する
     * @param relativeDay (今日からの)相対日
     * @return ファイル名
     */
    public String getFileName(int relativeDay)
    {
        String name = dateUtils.getDateNumber(relativeDay) + "-" + storageDb.getAreaId() + "-" +  storageDb.getTvType() + ".html";
        return (name);
    }

    /**
     *  現在の時刻を応答する
     * @return
     */
    public int getCurrentTime()
    {
        return (dateUtils.getCurrentTime(5));
    }
    
    /**
     * 
     * @param relativeDay (今日からの)相対日
     * @return
     */
    public String getDateDataString(int relativeDay)
    {
        String dateString = dateUtils.getDateDataString(relativeDay);
        String appendMessage = "";
        if (relativeDay == -1)
        {
        	appendMessage = "   (きのう)";
        }
        else if (relativeDay == 0)
        {
        	appendMessage = "   (今日)";
        }
        else if (relativeDay == 1)
        {
        	appendMessage = "   (あした)";
        }
        return (dateString + " " + appendMessage);
    }

    /**
     *  日付データを選択する
     * @param relativeDay (今日からの)相対日
     * @return 選択されたデータ
     */
    public boolean selectedDateData(int relativeDay, boolean forceUpdate)
    {
        dataFileName = storageDb.getBaseDirectory() + getFileName(relativeDay);
        dataFileDate = dateUtils.getDateNumber(relativeDay);
        dataFileForceUpdate = forceUpdate;
        Thread thread = new Thread()
        {
            public void run()
            {
                dataProcess.updateInformation("", 0, false);
                dataFileIsReady = MidpFileOperations.IsExistFile(dataFileName);
                if ((dataFileIsReady == false)||(dataFileForceUpdate == true))
                {                    
                    // データを(通信で)取得する
                    String url = storageDb.getUrlToGet() + dataFileDate + "0500" + "&index=" + storageDb.getTvType();
                    boolean ret = httpComm.GetHttpCommunication(dataFileName, url, null, "", -1, -1, false);
                    dataProcess.finishUpdateData(0, 0, 300);
                    if (ret == false)
                    {
                        dataProcess.updateInformation("  データ通信に失敗!!", 0, false);
                        return;
                    }
                    dataProcess.updateInformation("通信終了 ", 0, true);
                    dataFileIsReady = true;
                }

                // データファイルを解析する
                dataConstructionIsReady = false;
                dataProcess.updateInformation("", 0, false);
                dataConstructionIsReady = parseDataFile(dataFileName);
                if (dataConstructionIsReady == false)
                {
                    dataProcess.updateInformation("失敗 " + dataString, 0, false);
                }
                else
                {
                    dataProcess.updateInformation("解析終了 ", 0, false);               
                }
            };
        };
        try
        {
            thread.start();
            thread.join();
        }
        catch (Exception ex)
        {
            dataString = ex.getMessage() + " " + ex.toString();
        }
        return (true);
    }

    /**
     *  表示データの日付(数値)を取得する
     * @return
     */
    public int getDataDate()
    {
        return (dataFileDate);
    }
    
    /**
     *  データファイルの解析実処理
     * @param fileName
     * @return <code>true</code>:解析成功 / <code>false</code>:解析失敗
     */
    public boolean parseDataFile(String fileName)
    {
        boolean ret = false;
        byte[] dataFile = null;
        try
        {
            dataFile = fileUtils.ReadFileWhole(fileName);
            System.gc();
            tvStationData = null;
            tvStationData = new Vector();
            ret = parseTvStationData(dataFile);
            dataFile = null;
        }
        catch (Exception ex)
        {
            //
            dataFile = null;
            dataString = ex.getMessage() + " " + ex.toString();
            ret = false;
        }
        System.gc();
        return (ret);
    }

    /**
     *  テレビ局名を抽出する
     * @param data バイトデータ
     * @return <code>true</code>:抽出成功 / <code>false</code>:抽出失敗
     */
    private boolean parseTvStationData(byte[] data)
    {
        for (int lp = 0; lp < data.length; lp++)
        {
            if ((data[lp + 0] == '<')&&(data[lp + 1] == 't')&&(data[lp + 2] == 'h')&&
                (data[lp + 3] == 'e')&&(data[lp + 4] == 'a')&&(data[lp + 5] == 'd')&&(data[lp + 6] == '>'))
            {
                // <thead>...
                int next = pickupTvStation(data, (lp + 7));
                if (next < 0)
                {
                    return (false);
                }
                lp = next;
            }
            else if ((data[lp + 0] == '<')&&(data[lp + 1] == 't')&&(data[lp + 2] == 'b')&&
                      (data[lp + 3] == 'o')&&(data[lp + 4] == 'd')&&(data[lp + 5] == 'y')&&(data[lp + 6] == '>'))
            {
                // <tbody>...
                int next = parseTvProgramData(data, (lp + 7));
                if (next < 0)
                {
                    return (false);
                }
                return (true);
            }
        }
        return (true);
    }
    
    /**
     *  テレビ局の番組データを切り出して格納する
     * 
     */
    private int parseTvProgramData(byte[] data, int offset)
    {
        int   dataMinutes = 0;
        for (int lp = offset; lp < data.length; lp++)
        {
            if ((data[lp + 0] == '\"')&&(data[lp + 1] == 's')&&(data[lp + 2] == 'c')&&(data[lp + 3] == 'h')&&
                    (data[lp + 4] == 'e')&&(data[lp + 5] == 'd')&&(data[lp + 6] == 'u')&&(data[lp + 7] == 'l')&&
                (data[lp + 8] == 'e')&&(data[lp + 9] == ' '))
            {
                // '"schedule ' : テレビ局インデックスを抽出する
                int start = lp + 10;
                while (data[start] != '>')
                {
                    start++;
                }
                start++;
                lp = pickupTvProgramData(data, start, dataMinutes);
                String progress = ((lp * 100)/ data.length) + "%" + " " + "(" + lp + " / " + data.length + ")";
                dataProcess.updateInformation(progress, 0, false);
            }
            else if ((data[lp + 0] == 'r')&&(data[lp + 1] == 'o')&&(data[lp + 2] == 'w')&&(data[lp + 3] == 's')&&
                    (data[lp + 4] == 'p')&&(data[lp + 5] == 'a')&&(data[lp + 6] == 'n')&&(data[lp + 7] == '=')&&
                    (data[lp + 8] == '"'))
            {
                // 'rowspan="' を取得... 番組の長さを取得
                int start = lp + 9;
                int end = lp + 9;
                while (data[end] != '"')
                {
                    end++;
                }
                String min = new String(data, start, (end - start));
                dataMinutes = Integer.parseInt(min);
                end++;
                lp = end;
            }
            else if ((data[lp + 0] == '<')&&(data[lp + 1] == '/')&&(data[lp + 2] == 't')&&(data[lp + 3] == 'b')&&
                    (data[lp + 4] == 'o')&&(data[lp + 5] == 'd')&&(data[lp + 6] == 'y')&&(data[lp + 7] == '>'))
            {
                // </tbody>の検出...終了する
                return (lp + 8);
            }
        }
        return (-10);
    }

    /**
     *  １件のテレビ番組データを抽出する
     * @param data
     * @param offset
     * @param hourHead
     * @param stationNumber
     * @return
     */
    private int pickupTvProgramData(byte[] data, int offset, int dataMinutes)
    {
        int returnIndex = 0;
        for (int lp = offset; lp < data.length; lp++)
        {
            if ((data[lp + 0] == '\"')&&(data[lp + 1] == 's')&&(data[lp + 2] == 'c')&&(data[lp + 3] == 'h')&&
                (data[lp + 4] == 'e')&&(data[lp + 5] == 'd')&&(data[lp + 6] == 'u')&&(data[lp + 7] == 'l')&&
                (data[lp + 8] == 'e')&&(data[lp + 9] == ' '))
            {
                // 次の解析データの先頭を検出... ('"schedule ')
                entryTvProgramData(data, offset, lp, dataMinutes);
                return (returnIndex);
            }
            else if ((data[lp + 0] == '\"')&&(data[lp + 1] == 'h')&&(data[lp + 2] == 'o')&&(data[lp + 3] == 'u')&&
                      (data[lp + 4] == 'r')&&(data[lp + 5] == '-')&&(data[lp + 6] == 'h')&&(data[lp + 7] == 'e')&&
                      (data[lp + 8] == 'a')&&(data[lp + 9] == 'd')&&(data[lp + 10] == '\"'))
            {
                // 次の解析データの先頭を検出... ('"hour-head"')
                entryTvProgramData(data, offset, lp, dataMinutes);
                return (lp + 10);
            }
            else if ((data[lp + 0] == '<')&&(data[lp + 1] == 't')&&(data[lp + 2] == 'd')&&(data[lp + 3] == ' '))
            {
                // '<td ' を戻りの先頭に変更する (rowspan取得用...)
                returnIndex = (lp + 3);
            }
            else if ((data[lp + 0] == '<')&&(data[lp + 1] == '/')&&(data[lp + 2] == 't')&&(data[lp + 3] == 'b')&&
                    (data[lp + 4] == 'o')&&(data[lp + 5] == 'd')&&(data[lp + 6] == 'y')&&(data[lp + 7] == '>'))
            {
                // </tbody>の検出...終了する
                return (lp - 1);
            }
        }
        return (data.length);
    }

    /**
     *   データを登録する
     * 
     * @param data
     * @param startIndex
     * @param endIndex
     * @param hourHead
     * @param dataMinutes
     */
    private void entryTvProgramData(byte[] data, int startIndex, int endIndex, int dataMinutes)
    {
        try
        {
            int stationNumberToSet = 0;
            int minimumMinitutes = Integer.MAX_VALUE;
            for (int lp = 0; lp < tvStationData.size(); lp++)
            {
                // どのテレビ局データなのか確認する
                TVgStationProgramData station = (TVgStationProgramData) tvStationData.elementAt(lp);
                int minutes = station.getMinutes();
                if (minutes < minimumMinitutes)
                {
                    minimumMinitutes = minutes;
                    stationNumberToSet = lp;
                }
            }
            TVgStationProgramData station = (TVgStationProgramData) tvStationData.elementAt(stationNumberToSet);
            station.SetStationData(data, startIndex, endIndex, dataMinutes);
        }
        catch (Exception ex)
        {
            //
        }
        System.gc();
        return;
    }
    
    /**
     *  テレビ局名を取得する
     * @param data データバッファ
     * @param offset データバッファの先頭
     * @return 切り出したデータの末尾
     */
    private int pickupTvStation(byte[] data, int offset)
    {
        for (int lp = offset; lp < data.length; lp++)
        {
            // データの末尾を検出する
            if ((data[lp + 0] == '<')&&(data[lp + 1] == '/')&&(data[lp + 2] == 't')&&(data[lp + 3] == 'h')&&
                 (data[lp + 4] == 'e')&&(data[lp + 5] == 'a')&&(data[lp + 6] == 'd')&&(data[lp + 7] == '>'))
            {
                // 
                return (lp + 8);
            }
            
            //
            if ((data[lp + 0] == '<')&&(data[lp + 1] == 't')&&(data[lp + 2] == 'h')&&(data[lp + 3] == ' '))
            {
                lp = pickupTvStationMain(data, (lp + 4));
            }
        }
        return (-10);
    }

    /**
     *  テレビ局名称を抽出する(メイン処理)
     * @param data
     * @param offset
     * @return
     */
    private int pickupTvStationMain(byte[] data, int offset)
    {
        for (int lp = offset; lp < data.length; lp++)
        {
            // データの末尾を検出する
            if ((data[lp + 0] == '<')&&(data[lp + 1] == '/')&&(data[lp + 2] == 't')&&(data[lp + 3] == 'h')&&
                 (data[lp + 4] == '>'))
            { 
                return (lp + 5);
            }

            if ((data[lp + 0] == '<')&&(data[lp + 1] == 'a')&&(data[lp + 2] == ' '))
            {
                int index = lp + 3;
                while (index < data.length)
                {
                    if (data[index] == '>')
                    {
                        index++;
                        while ((data [index] == ' ')||(data [index] == 0x0d)||(data [index] == 0x0a)||(data [index] == '\t'))
                        {
                            // スペース、改行コード、タブは消去する
                            index++;
                        }
                        int start = index;
                        while (index < data.length)
                        {
                            if ((data[index] == ' ')||(data[index] == '<'))
                            {
                                TVgStationProgramData station = new TVgStationProgramData(new String(data, start, (index - start)));
                                tvStationData.addElement(station);
                                return (index);
                            }
                            index++;
                        }
                    }
                    index++;
                }
                lp = index;
            }
        }
        return (data.length);
    }

    /**
     *  ファイルデータが取得済みか？
     * @return
     */
    public boolean isDataFileIsReady()
    {
        return (dataFileIsReady);
    }    

    /**
     *  表示データの解析が終了したか？
     * @return
     */
    public boolean isReadyToConstruct()
    {
        return (dataConstructionIsReady);
    }
    
    /**
     *  データ更新中...
     *
     */
    public void prepareDataToShow()
    {
        // 描画...
        dataProcess.updateInformation(" ", 0, false);     
    }

    /**
     *  テレビ局名を応答する
     * @param itemNumber アイテム番号
     * @return TV局
     */
    public String getTvStationName(int itemNumber)
    {
        return (((TVgStationProgramData) tvStationData.elementAt(itemNumber)).getStationName());
    }

    /**
     *  番組データ数を応答する
     * @return 番組データ数
     */
    public int getNumberOfProgramDatas()
    {
           int station = storageDb.getTvStation();
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.numberOfDatas());
    }

    /**
     *  番組データを応答する
     * @return 番組データ
     */
    public String getProgramDataFromIndex(int station, int index, boolean isRange)
    {
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgramData(index, isRange));
    }

    /**
     *  番組データを応答する
     * @return 番組データ
     */
    public String getProgramData(int index, boolean isRange)
    {
           int station = storageDb.getTvStation();
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgramData(index, isRange));
    }
    
    /**
     *  番組データを応答する (時間指定)
     * @return 番組データ
     */
    public String getProgramDataFromMinutes(int station, int minutes, boolean isRange)
    {
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgram(minutes, isRange));
    }

    /**
     *  テレビ番組Indexを取得する
     * @param station TV局ID
     * @param minutes 分
     * @return テレビ番組Index
     */
    public int getProgramIndex(int station, int minutes)
    {
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgramIndex(minutes));
    }
}
