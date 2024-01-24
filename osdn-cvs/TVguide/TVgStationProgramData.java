import java.util.Vector;
import jp.sourceforge.nnsi.a2b.utilities.MidpFileOperations;
import jp.sourceforge.nnsi.a2b.utilities.MidpKanjiConverter;

/**
 *  �e���r�ǂ̃f�[�^
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
     *  �R���X�g���N�^�ł́ATV�ǖ���ݒ肵�܂�
     */
    public TVgStationProgramData(String station)
    {
        stationName = station;
        programData = new Vector();
    }

    /**
     *  TV�ǖ����������܂�
     * @return TV�ǖ�
     */
    public String getStationName()
    {
        return (stationName);
    }

    /**
     *  �ԑg�f�[�^��ݒ肵�܂��B
     *  �i�����Ń^�O�����ȗ����Ċi�[���܂��B�j
     * @param hourHead    ���Ԃ̃w�b�_
     * @param data        �ԑg�f�[�^(�o�b�t�@)
     * @param startIndex  �ԑg�f�[�^�̐擪�C���f�b�N�X
     * @param endIndex    �ԑg�f�[�^�̖����C���f�b�N�X
     * @param dataMinutes �ԑg�f�[�^�̒���(�P��:��)
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
                // �^�O����
                if (data[index] == '<')
                {
                    while ((index < endIndex)&&(data[index] != '>'))
                    {
                        if ((data[index + 0] == '\"')&&(data[index + 1] == 't')&&(data[index + 2] == 'i')&&(data[index + 3] == 'm')&&
                                (data[index + 4] == 'e')&&(data[index + 5] == '-')&&(data[index + 6] == 'r')&&(data[index + 7] == 'a')&&
                                (data[index + 8] == 'n')&&(data[index + 9] == 'g')&&(data[index + 10] == 'e'))
                        {
                            // �����f�[�^�̐擪�����o... (time-range) : �ǂݔ�΂�
                            index = index + 11;
                            while (data[index] != '>')
                            {
                                index++;
                            }
                            index++;
                        }
                        else if ((data[index + 0] == 'a')&&(data[index + 1] == 'l')&&(data[index + 2] == 't')&&(data[index + 3] == '=')&&(data[index + 4] == '\"'))
                        {
                        	// alt="..."(��փe�L�X�g)��W�J����
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
                            // �f�[�^�̐擪�����o... ("schedule genre-) : �f�[�^�^�C�v�Ƃ��ċL�^����
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
                    // �X�y�[�X�A���s�R�[�h�A�^�u�͏�������
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
     *  �i�[���Ă���f�[�^���i�ԑg���j���������܂�
     * @return �i�[���Ă���f�[�^��
     */
    public int numberOfDatas()
    {
        return (programData.size());
    }

    /**
     * TV�ԑg�f�[�^���擾����
     * @param index �C���f�b�N�X�ԍ�
     * @return �ԑg�f�[�^
     */
    public String getProgramData(int index, boolean isRangeTime)
    {
        TvProgramData data = (TvProgramData) programData.elementAt(index);
        return (data.getDataContent(isRangeTime));
    }

    /**
     *  TV�ԑg�f�[�^���擾���� (�����w��Ŏ擾)
     * @param startMinutes �J�n����(���P�ʂŁA�ߑO5������̌o�ߕ��j
     * @return �ԑg�f�[�^
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
     *  TV�ԑg�f�[�^�̐擪�C���f�b�N�X���擾���� (�����w��Ŏ擾)
     * @param startMinutes �J�n����(���P�ʂŁA�ߑO5������̌o�ߕ��j
     * @return �擪�C���f�b�N�X
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
     *  �f�[�^��"��"���擾����
     * @return �L�^����Ă��� "��"
     */
    public int getMinutes()
    {
        return (minutes);
    }

    /**
     *  �L���b�V�����Ă����f�[�^�t�@�C����ǂݏo��
     * @param fileName
     * @return <code>true</code>:�ǂݏo������ / <code>false</code>:�ǂݏo�����s
     */
    public boolean constructFromFile(String fileName)
    {
        if (MidpFileOperations.IsExistFile(fileName) == false)
        {
            // �L���b�V���t�@�C�����Ȃ�...�I������
        	return (false);
        }
    	
    	return (false);
    }
    
    /**
     *  �f�[�^���L���b�V������i�t�@�C���Ƀf�[�^�������o���j
     * @param fileName
     */
    public void outputToFile(String fileName)
    {

        return;
    }
    
    /**
     *  �e���r�ԑg�̃f�[�^�i�ꌏ���j
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
         *  �R���X�g���N�^�ł́A�f�[�^�����܂�
         * @param data �ԑg�f�[�^
         * @param hourHead ���Ԃ̃w�b�_
         * @param minutes �ԑg�̒���
         * @param type �f�[�^�^�C�v
         */
        public TvProgramData(String data, int start, int minutes, int type)
        {
            dataContent = data;
            dataMinutes = minutes;
            startMinute = start;
            dataType    = type;
        }

        /**
         *  �ԑg�f�[�^���擾����
         * @return �ԑg�f�[�^
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
         *  �ԑg�f�[�^���擾����
         * @return �ԑg�f�[�^
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
         *   �ԑg�̕������Ԃ��擾����
         * 
         * @return
         */
        public String getProgramRange()
        {
            String data = convertTimeData(startMinute) + "-" + convertTimeData(startMinute + dataMinutes);
            return (data);
        }

        /**
         *  �ԑg�̒������擾����
         * @return �ԑg�̒���(�P��:��)
         */
        public int getDataMinutes()
        {
            return (dataMinutes);
        }
        
        /**
         *  �ԑg�̃f�[�^�^�C�v����������
         * @return �ԑg�f�[�^�^�C�v
         */        
        public int getDataType()
        {
        	return (dataType);
        }
    }
}
