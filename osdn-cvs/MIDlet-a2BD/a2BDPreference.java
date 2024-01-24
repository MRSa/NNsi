/**
 *  �ݒ�f�[�^�ێ��N���X
 * @author MRSa
 *
 */
public class a2BDPreference
{
    // �f�[�^�X�g���[�W���ƃo�[�W����
    private final String DATASTORAGE_NAME     = "a2BD";
    private final int    DATASTORAGE_VERSION  = 101;

    /***** �f�[�^�x�[�X���R�[�h�ԍ�(��������) *****/
    private final int    VERSION_RECORDID     = 1;    // preference DB�̃o�[�W�����i�[���R�[�h
    private final int    PARAMS_RECORDID      = 2;
    private final int    USERAGENT_RECORDID   = 3;
    private final int    DATA1_RECORDID       = 4;
    private final int    DATA2_RECORDID       = 5;
    /***** �f�[�^�x�[�X���R�[�h�ԍ�(�����܂�) *****/
    
    /***** ���R�[�h�A�N�Z�T *****/
    private MidpRecordAccessor dbAccessor = new MidpRecordAccessor();

    /***** �ۊǃf�[�^(��������) *****/
    private String             data1           = "http://menu.2ch.net/bbstable.html";
	private String             data2           = "";
    private String             userAgent       = "Mozilla/3.0(WILLCOM;SANYO/WX310SA/2;1/1/C128) NetFront/3.3";
	private boolean           wx310WorkAround = false;
    private boolean           divideGetHttp   = false;
    private boolean           finishAuto      = false;
    /***** �ۊǃf�[�^(�����܂�) *****/

    /**
     *  �R���X�g���N�^
     *
     */
    public a2BDPreference()
    {
    	
    }

	/**
	 *  �f�[�^�����X�g�A����B�B�B
     *
	 */
	public void restore()
	{
        int versionNumber = 0;
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        if (dbAccessor.startReadRecordIntArray(VERSION_RECORDID) > 0)
        {
            versionNumber = dbAccessor.readRecordIntArray(0);
        }
        try
        {
            if (versionNumber == DATASTORAGE_VERSION)
            {
                int intData = 0;
                if (dbAccessor.startReadRecordIntArray(PARAMS_RECORDID) > 0)
                {
                	intData = dbAccessor.readRecordIntArray(0);
                	wx310WorkAround = (intData != 0) ? true : false;

                	intData = dbAccessor.readRecordIntArray(1);
                	divideGetHttp = (intData != 0) ? true : false;

                	intData = dbAccessor.readRecordIntArray(2);
                	finishAuto = (intData != 0) ? true : false;
                }
                dbAccessor.finishRecordIntArray();

                // �o�[�W������v�A���R�[�h��ǂݍ���
                byte[] byteData = null;

                byteData = dbAccessor.readRecord(USERAGENT_RECORDID);
                if (byteData != null)
                {
                	userAgent = new String(byteData);
                }
                byteData = null;

                byteData = dbAccessor.readRecord(DATA1_RECORDID);
                if (byteData != null)
                {
                    data1 = new String(byteData);
                }
                byteData = null;

                byteData = dbAccessor.readRecord(DATA2_RECORDID);
                if (byteData != null)
                {
                    data2 = new String(byteData);
                }
                byteData = null;

            }
        }
        catch (Exception e)
        {
            // ��O�����A�����l�ɖ߂�
        	data1           = "http://menu.2ch.net/bbstable.html";
        	data2           = "";
            userAgent       = "Mozilla/3.0(WILLCOM;SANYO/WX310SA/2;1/1/C128) NetFront/3.3";
        	wx310WorkAround = false;
            divideGetHttp   = false;
            finishAuto      = false;
        }
        dbAccessor.closeDatabase();
	    System.gc();
	}	
	
	/**
	 *  �f�[�^���o�b�N�A�b�v����
	 *
	 */
	public void backup()
	{
    	
    	// DB���I�[�v��������
        if (dbAccessor.openDatabase(DATASTORAGE_NAME) == false)
        {
            return;
        }
        
        // �o�[�W�����ԍ��̋L�^
        dbAccessor.startCacheIntArray(1);
        dbAccessor.cacheRecordIntArray(0, DATASTORAGE_VERSION);
        dbAccessor.writeRecordIntArray(VERSION_RECORDID);

        try
        {

            // �ݒ�f�[�^(true/false)�̏��L�^
            dbAccessor.startCacheIntArray(3);    // 3���̃f�[�^���L�^����B�B�B
            int intData = 0;
            intData = (wx310WorkAround == true) ? 1: 0;
            dbAccessor.cacheRecordIntArray(0, intData);

            intData = (divideGetHttp == true) ? 1: 0;
            dbAccessor.cacheRecordIntArray(1, intData);

            intData = (finishAuto == true)? 1: 0;
            dbAccessor.cacheRecordIntArray(2, intData);

            dbAccessor.writeRecordIntArray(PARAMS_RECORDID);

            if ((userAgent != null)&&(userAgent.length() > 1))
            {
                dbAccessor.writeRecord(USERAGENT_RECORDID, userAgent.getBytes(), 0, (userAgent.getBytes()).length);
            }

            if ((data1 != null)&&(data1.length() > 1))
            {
                dbAccessor.writeRecord(DATA1_RECORDID, data1.getBytes(), 0, (data1.getBytes()).length);
            }
            
            if ((data2 != null)&&(data2.length() > 1))
            {
                dbAccessor.writeRecord(DATA2_RECORDID, data2.getBytes(), 0, (data2.getBytes()).length);
            }
        }
        catch (Exception ex)
        {
        	// ��O�����A�A�A�������Ȃ�
        }
        
        // DB���N���[�Y����
        dbAccessor.closeDatabase();
        
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////// getter
    
    public String decideFileName(String url, String baseFileName)
    {
    	int pos = 0;
    	pos = baseFileName.lastIndexOf('/');
    	if (baseFileName.length() > (pos + 1))
    	{
            // ������ / �ŏI����Ă��Ȃ�... data1�����̂܂܉�������B
    		return (baseFileName);
    	}

    	// ������ / �ŏI����Ă���... URL����t�@�C�����𒊏o����    	
        pos = url.lastIndexOf('/');
        if (url.length() > (pos + 1))
        {
            String fileName = url.substring(pos + 1);
            return (baseFileName + fileName);
        }	
    	return (baseFileName + "temp.html");
    }
	
    public String getData1()
    {
        return (data1);
    }

    public String getData2()
    {
    	return (data2);
    }

    public String getUserAgent()
    {
    	return (userAgent);
    }
	
    public boolean getWX310workaroundFlag()
    {
    	return (wx310WorkAround);
    }
    
    public boolean getDivideGetHttp()
    {
    	return (divideGetHttp);
    }

    public boolean getAutoFinish()
    {
    	return (finishAuto);
    }

	/////////////////////////////////////////////////////////////////////////////////////////// setter

    public void setData1(String data)
    {
    	data1 = data;
    }
	
    public void setData2(String data)
    {
    	data2 = data;
    }

    public void setUserAgent(String data)
    {
    	userAgent = data;
    }
	
    public void setWX310workaroundFlag(boolean data)
    {
    	wx310WorkAround = data;
    }
    
    public void setDivideGetHttp(boolean data)
    {
    	divideGetHttp = data;
    }

    public void setAutoFinish(boolean data)
    {
    	finishAuto = data;
    }
}
