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
     *   �R���X�g���N�^�ł́A�f�[�^�X�g���[�W�N���X���L������
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
     *  �f�[�^�̊J�n
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
     *   ���ݕ\������e���r�ԑg�\�̒n��Ǝ�ʂ̕��������������
     * @return �e���r�ԑg�\�̒n��Ǝ�ʁA�e���r�ǂ̕�����
     */
    public String getTvStationTitle()
    {
        return (storageDb.getAreaName() + " [" + storageDb.getTvTypeString() + "]");
    }

    /**
     *  �e���r�ǖ���\������
     * @return �e���r�ǖ�
     */
    public String getTvStationString()
    {
        return (getTvStationName(storageDb.getTvStation()));
    }

    /**
     *  �I�𒆃e���r�ǂ�ύX����
     * 
     * @param relativeNumber �ύX����e���r�ǂ̃I�t�Z�b�g
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
     *  �e���r�ǂ�Id����������
     * @return �e���r��Id
     */
    public int getTvStationIndex()
    {
        return (storageDb.getTvStation());
    }
    
    /**
     *  �e���r�ǐ�����������
     *
     * @return �e���r�ǐ�
     */
    public int getNumberOfTvStations()
    {
        return (tvStationData.size());
    }

    
    /**
     *  �ԑg�\�t�@�C�������擾����
     * @param relativeDay (���������)���Γ�
     * @return �t�@�C����
     */
    public String getFileName(int relativeDay)
    {
        String name = dateUtils.getDateNumber(relativeDay) + "-" + storageDb.getAreaId() + "-" +  storageDb.getTvType() + ".html";
        return (name);
    }

    /**
     *  ���݂̎�������������
     * @return
     */
    public int getCurrentTime()
    {
        return (dateUtils.getCurrentTime(5));
    }
    
    /**
     * 
     * @param relativeDay (���������)���Γ�
     * @return
     */
    public String getDateDataString(int relativeDay)
    {
        String dateString = dateUtils.getDateDataString(relativeDay);
        String appendMessage = "";
        if (relativeDay == -1)
        {
        	appendMessage = "   (���̂�)";
        }
        else if (relativeDay == 0)
        {
        	appendMessage = "   (����)";
        }
        else if (relativeDay == 1)
        {
        	appendMessage = "   (������)";
        }
        return (dateString + " " + appendMessage);
    }

    /**
     *  ���t�f�[�^��I������
     * @param relativeDay (���������)���Γ�
     * @return �I�����ꂽ�f�[�^
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
                    // �f�[�^��(�ʐM��)�擾����
                    String url = storageDb.getUrlToGet() + dataFileDate + "0500" + "&index=" + storageDb.getTvType();
                    boolean ret = httpComm.GetHttpCommunication(dataFileName, url, null, "", -1, -1, false);
                    dataProcess.finishUpdateData(0, 0, 300);
                    if (ret == false)
                    {
                        dataProcess.updateInformation("  �f�[�^�ʐM�Ɏ��s!!", 0, false);
                        return;
                    }
                    dataProcess.updateInformation("�ʐM�I�� ", 0, true);
                    dataFileIsReady = true;
                }

                // �f�[�^�t�@�C������͂���
                dataConstructionIsReady = false;
                dataProcess.updateInformation("", 0, false);
                dataConstructionIsReady = parseDataFile(dataFileName);
                if (dataConstructionIsReady == false)
                {
                    dataProcess.updateInformation("���s " + dataString, 0, false);
                }
                else
                {
                    dataProcess.updateInformation("��͏I�� ", 0, false);               
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
     *  �\���f�[�^�̓��t(���l)���擾����
     * @return
     */
    public int getDataDate()
    {
        return (dataFileDate);
    }
    
    /**
     *  �f�[�^�t�@�C���̉�͎�����
     * @param fileName
     * @return <code>true</code>:��͐��� / <code>false</code>:��͎��s
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
     *  �e���r�ǖ��𒊏o����
     * @param data �o�C�g�f�[�^
     * @return <code>true</code>:���o���� / <code>false</code>:���o���s
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
     *  �e���r�ǂ̔ԑg�f�[�^��؂�o���Ċi�[����
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
                // '"schedule ' : �e���r�ǃC���f�b�N�X�𒊏o����
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
                // 'rowspan="' ���擾... �ԑg�̒������擾
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
                // </tbody>�̌��o...�I������
                return (lp + 8);
            }
        }
        return (-10);
    }

    /**
     *  �P���̃e���r�ԑg�f�[�^�𒊏o����
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
                // ���̉�̓f�[�^�̐擪�����o... ('"schedule ')
                entryTvProgramData(data, offset, lp, dataMinutes);
                return (returnIndex);
            }
            else if ((data[lp + 0] == '\"')&&(data[lp + 1] == 'h')&&(data[lp + 2] == 'o')&&(data[lp + 3] == 'u')&&
                      (data[lp + 4] == 'r')&&(data[lp + 5] == '-')&&(data[lp + 6] == 'h')&&(data[lp + 7] == 'e')&&
                      (data[lp + 8] == 'a')&&(data[lp + 9] == 'd')&&(data[lp + 10] == '\"'))
            {
                // ���̉�̓f�[�^�̐擪�����o... ('"hour-head"')
                entryTvProgramData(data, offset, lp, dataMinutes);
                return (lp + 10);
            }
            else if ((data[lp + 0] == '<')&&(data[lp + 1] == 't')&&(data[lp + 2] == 'd')&&(data[lp + 3] == ' '))
            {
                // '<td ' ��߂�̐擪�ɕύX���� (rowspan�擾�p...)
                returnIndex = (lp + 3);
            }
            else if ((data[lp + 0] == '<')&&(data[lp + 1] == '/')&&(data[lp + 2] == 't')&&(data[lp + 3] == 'b')&&
                    (data[lp + 4] == 'o')&&(data[lp + 5] == 'd')&&(data[lp + 6] == 'y')&&(data[lp + 7] == '>'))
            {
                // </tbody>�̌��o...�I������
                return (lp - 1);
            }
        }
        return (data.length);
    }

    /**
     *   �f�[�^��o�^����
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
                // �ǂ̃e���r�ǃf�[�^�Ȃ̂��m�F����
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
     *  �e���r�ǖ����擾����
     * @param data �f�[�^�o�b�t�@
     * @param offset �f�[�^�o�b�t�@�̐擪
     * @return �؂�o�����f�[�^�̖���
     */
    private int pickupTvStation(byte[] data, int offset)
    {
        for (int lp = offset; lp < data.length; lp++)
        {
            // �f�[�^�̖��������o����
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
     *  �e���r�ǖ��̂𒊏o����(���C������)
     * @param data
     * @param offset
     * @return
     */
    private int pickupTvStationMain(byte[] data, int offset)
    {
        for (int lp = offset; lp < data.length; lp++)
        {
            // �f�[�^�̖��������o����
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
                            // �X�y�[�X�A���s�R�[�h�A�^�u�͏�������
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
     *  �t�@�C���f�[�^���擾�ς݂��H
     * @return
     */
    public boolean isDataFileIsReady()
    {
        return (dataFileIsReady);
    }    

    /**
     *  �\���f�[�^�̉�͂��I���������H
     * @return
     */
    public boolean isReadyToConstruct()
    {
        return (dataConstructionIsReady);
    }
    
    /**
     *  �f�[�^�X�V��...
     *
     */
    public void prepareDataToShow()
    {
        // �`��...
        dataProcess.updateInformation(" ", 0, false);     
    }

    /**
     *  �e���r�ǖ�����������
     * @param itemNumber �A�C�e���ԍ�
     * @return TV��
     */
    public String getTvStationName(int itemNumber)
    {
        return (((TVgStationProgramData) tvStationData.elementAt(itemNumber)).getStationName());
    }

    /**
     *  �ԑg�f�[�^������������
     * @return �ԑg�f�[�^��
     */
    public int getNumberOfProgramDatas()
    {
           int station = storageDb.getTvStation();
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.numberOfDatas());
    }

    /**
     *  �ԑg�f�[�^����������
     * @return �ԑg�f�[�^
     */
    public String getProgramDataFromIndex(int station, int index, boolean isRange)
    {
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgramData(index, isRange));
    }

    /**
     *  �ԑg�f�[�^����������
     * @return �ԑg�f�[�^
     */
    public String getProgramData(int index, boolean isRange)
    {
           int station = storageDb.getTvStation();
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgramData(index, isRange));
    }
    
    /**
     *  �ԑg�f�[�^���������� (���Ԏw��)
     * @return �ԑg�f�[�^
     */
    public String getProgramDataFromMinutes(int station, int minutes, boolean isRange)
    {
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgram(minutes, isRange));
    }

    /**
     *  �e���r�ԑgIndex���擾����
     * @param station TV��ID
     * @param minutes ��
     * @return �e���r�ԑgIndex
     */
    public int getProgramIndex(int station, int minutes)
    {
           TVgStationProgramData programData = (TVgStationProgramData) tvStationData.elementAt(station);
           return (programData.getProgramIndex(minutes));
    }
}
