import javax.microedition.rms.RecordStore;

/**
 *  �f�[�^�X�g���[�W(���R�[�h�X�g�A)�A�N�Z�T
 *  <br>
 *  �[���̃��R�[�h�X�g�A�ɑ΂��ǂݏ������s�����[�e�B���e�B�N���X�ł��B
 *  ���l�^�̃f�[�^�ǂݏ������i�o�C�g��ɕϊ����āj�P���R�[�h�ɂ܂Ƃ�
 *  ���s���邱�Ƃ��ł��܂��B
 * 
 * @author MRSa
 *
 */
public class MidpRecordAccessor
{
    private final int INTARRAY_NOTUSED = -1;
	private String   dbStorageName = null;
    private boolean createIfNecessary = true;
    private int      authMode = RecordStore.AUTHMODE_PRIVATE;
    private boolean writable = true;

    private RecordStore  prefDB = null;
    private byte[]      intArrayRecord = null;
    private int         isIntArrayUse  = INTARRAY_NOTUSED;
    
    /**
	 *  �R���X�g���N�^�ł̏����͂���܂���B
     * 
     */
    public MidpRecordAccessor()
    {
        //
    }

    /**
     *  �f�[�^�X�g���[�W���I�[�v������
     *  
     * @param storageName �f�[�^�X�g���[�W��
     * @return �I�[�v������ OK:true, NG:false
     */
    public boolean openDatabase(String storageName)
    {
        dbStorageName = storageName;
        boolean ret = false;
        try
        {
            // ���R�[�h�X�g�A���I�[�v������
            prefDB = RecordStore.openRecordStore(dbStorageName, createIfNecessary, authMode, writable);
            ret = true;
        }
        catch (Exception ex)
        {
            ret = false;        	
        }
        return (ret);        
    }

    /**
     *  �f�[�^�X�g���[�W���I�[�v������
     *  
     * @param storageName �f�[�^�X�g���[�W��
     * @param isCreate �Ђ悤������΃X�g���[�W���쐬����
     * @param athMode AUTHMODE
     * @param isWrite �������݋���
     * @return �I�[�v������ OK:true, NG:false
     */
    public boolean openDatabase(String storageName, boolean isCreate, int athMode, boolean isWrite)
    {
        dbStorageName = storageName;
        createIfNecessary = isCreate;
        boolean ret = false;
        try
        {
            // ���R�[�h�X�g�A���I�[�v������
            prefDB = RecordStore.openRecordStore(storageName, isCreate, athMode, isWrite);
            ret = true;
        }
        catch (Exception ex)
        {
            ret = false;        	
        }
        return (ret);        
    }
    
    /**
     *  (�ʃA�v����)�f�[�^�X�g���[�W���I�[�v������
     *  
     * @param recordStoreName ���R�[�h�X�g�A����
     * @param vendorName �x���_����
     * @param suiteName �X�[�g����(MIDlet suite name)
     * @return �I�[�v������ OK:true, NG:false
     */
    public boolean openDatabase(String recordStoreName, String vendorName, String suiteName)
    {
        boolean ret = false;
        try
        {
            // ���R�[�h�X�g�A���I�[�v������
            prefDB = RecordStore.openRecordStore(recordStoreName, vendorName, suiteName);
            ret = true;
        }
        catch (Exception ex)
        {
            ret = false;        	
        }
        return (ret);        
    }
    
    /**
     *  �f�[�^�X�g���[�W���N���[�Y����
     * 
     * @return ����(true)/���s(false)
     */
    public boolean closeDatabase()
    {
    	boolean ret = false;
    	try
    	{
    		if (prefDB != null)
    		{
    			prefDB.closeRecordStore();
    			prefDB = null;
    		}
    		System.gc();
    		ret = true;
    	}
    	catch (Exception ex)
    	{
    	    //
    	}
    	return (ret);
    }

    /**
     *  ���R�[�h�Ƀf�[�^����������
     *  �i���R�[�h�ԍ����w�肵�ď������ށj
     * 
     * @param recordNumber ���R�[�h�ԍ�
     * @param data     �������ރf�[�^�̃o�b�t�@
     * @param offset   �������ރf�[�^�̐擪�C���f�b�N�X
     * @param length   �������݃f�[�^��
     * @return ����(true)/���s(false)
     */
    public boolean writeRecord(int recordNumber, byte[] data, int offset, int length)
    {
        boolean ret = false;
    	if (prefDB == null)
        {
            return (false);
        }

        try
        {
            int nofRecord = prefDB.getNumRecords();
            if (nofRecord >= recordNumber)
            {
                // �f�[�^�̏�������(�X�V)
                prefDB.setRecord(recordNumber, data, offset, length);
            }
            else
            {
                // �f�[�^�̒ǋL
                prefDB.addRecord(data, offset, length);
            }
            ret = true;
        }
        catch (Exception ex)
        {
        	ret = false;
        }
        return (ret);
    }

    /**
     *  ���R�[�h�f�[�^�̓ǂݏo��
     * 
     * @param recordNumber �ǂݏo�����R�[�h�ԍ�(0�X�^�[�g)
     * @return ���R�[�h��� (null�̂Ƃ��ɂ͓ǂݏo�����s)
     */
    public byte[] readRecord(int recordNumber)
    {
    	if (prefDB == null)
    	{
    		return (null);
    	}
    	try
    	{
            int nofRecord = prefDB.getNumRecords();
            if (nofRecord < recordNumber)
            {
                return (null);
            }
            return (prefDB.getRecord(recordNumber));
    	}
    	catch (Exception ex)
    	{
    		
    	}
    	return (null);
    }

    /**
     *  int�^�f�[�^�̏����o���̎葱���J�n
     * 
     * @param size �����o��int�^�f�[�^�̌�
     * @return ����OK(true) / �������s(false)
     */
    public boolean startCacheIntArray(int size)
    {
        // �T�C�Y�w�肪�Ԉ���Ă����ꍇ�ɂ́A���������I������
        intArrayRecord = null;
    	if (size <= 0)
        {
            isIntArrayUse  = INTARRAY_NOTUSED;
        	return (false);
        }
    	
    	// int�^�f�[�^��byte�^�f�[�^�ɕϊ�����
    	intArrayRecord = new byte[size * 4];
    	isIntArrayUse = size;
        return (true);    	
    }

    /**
     *  int�^�f�[�^�z������R�[�h�ɓf���o���O�Ɉ�U�L���b�V������
     * 
     * @param index �i�[����f�[�^�̔z��ԍ�(0�X�^�[�g)
     * @param data ���f�[�^
     * @return ����(true) ���s(false)
     */
    public boolean cacheRecordIntArray(int index, int data)
    {
    	if ((isIntArrayUse < 0)||(index >= isIntArrayUse))
    	{
    		// �̈斢�m�ۂ��邢�͊m�ۗ̈�T�C�Y�I�[�o�[
    		return (false);
    	}
	    serializeData(intArrayRecord, (index * 4), data);
	    return (true);
    }
    
    /**
     * int�^�f�[�^����C�ɏ����o��
     * @param recordNumber �������ރ��R�[�h�ԍ�
     * @return �����o������(true) / �����o�����s(false)
     */
    public boolean writeRecordIntArray(int recordNumber)
    {
    	if (isIntArrayUse <= 0)
    	{
    		// �̈斢�m��
    		return (false);
    	}
    	// �f�[�^�̏����o��
    	boolean ret = writeRecord(recordNumber, intArrayRecord, 0, (isIntArrayUse * 4));
    	isIntArrayUse = INTARRAY_NOTUSED;
    	intArrayRecord = null;
    	System.gc();
        return (ret);
    }
    
    /**
     *  int�^���R�[�h�f�[�^��ǂݏo���ď�������
     * 
     * @param recordNumber �ǂݍ��ރ��R�[�h�ԍ�
     * @return �ǂݏo����int�^�f�[�^��
     */
    public int startReadRecordIntArray(int recordNumber)
    {
    	intArrayRecord = null;
        intArrayRecord = readRecord(recordNumber);
        if (intArrayRecord == null)
        {
        	isIntArrayUse = INTARRAY_NOTUSED;
        	return (INTARRAY_NOTUSED);
        }
        isIntArrayUse = (intArrayRecord.length / 4);
        return (isIntArrayUse);
    }

    /**
     *  int�^�f�[�^��ǂݏo��
     * 
     * @param index �ǂݏo���f�[�^�̔z��ԍ�(0�X�^�[�g)
     * @return int�^�f�[�^�̒l
     */
    public int readRecordIntArray(int index)
    {
    	if ((isIntArrayUse <= 0)||(index >= isIntArrayUse))
    	{
    		// �̈斢�m�ۂ��邢�͊m�ۗ̈�T�C�Y�I�[�o�[
    		return (INTARRAY_NOTUSED);
    	}
    	return (deSerializeData(intArrayRecord, (index * 4)));
    }

    /**
     *   int�^�f�[�^�̓ǂݏo�����I������
     * 
     */
    public void finishRecordIntArray()
    {
    	isIntArrayUse = INTARRAY_NOTUSED;
    	intArrayRecord = null;
    	System.gc();
    	return;
    }   
    
    /**
     *  �f�[�^�� int�^����o�C�g��֕ϊ�����
     *  @param buffer �ϊ���f�[�^�i�[�̈�
     *  @param offset �ϊ���f�[�^�i�[�̈�ւ̊i�[�ʒu�I�t�Z�b�g
     *  @param convId �ϊ�����int�^�f�[�^
     */
    private void serializeData(byte[] buffer, int offset, int convId)
    {
        buffer[offset + 0] = (byte)((convId >> 24)&(0xff));
        buffer[offset + 1] = (byte)((convId >> 16)&(0xff));
        buffer[offset + 2] = (byte)((convId >>  8)&(0xff));
        buffer[offset + 3] = (byte)((convId      )&(0xff));
        return;
    }

    /**
     *  �o�C�g��f�[�^��int�^�֕ϊ�����
     *  
     *  @param buffer �o�C�g��f�[�^
     *  @param osset �ϊ����s���f�[�^�̐擪�C���f�b�N�X�ԍ�
     *  @return �ϊ�����(int�^�f�[�^)
     * 
     */
    private int deSerializeData(byte[] buffer, int offset)
    {
        return ((((buffer[offset + 0]&(0xff))<< 24) + ((buffer[offset + 1]&(0xff))<< 16) +
                 ((buffer[offset + 2]&(0xff))<<  8) + ((buffer[offset + 3]&(0xff)))));
    }
}
