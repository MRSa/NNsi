package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  IMidpA2BhttpCommunicator�N���X�̃T���v�������ł��B<br>
 *  MidpA2BhttpCommunication �N���X�ŗ��p������̂ł��B
 * 
 * @see jp.sourceforge.nnsi.a2b.utilities.IMidpA2BhttpCommunicator
 * @see jp.sourceforge.nnsi.a2b.utilities.MidpA2BhttpCommunication
 * @author MRSa
 */
public class MidpDefaultA2BhttpCommunicator implements IMidpA2BhttpCommunicator
{
    boolean isGet2chAppendMode = false;
	String  userAgent = "Mozilla/3.0(WILLCOM;JRC/WX310J/2;1/1/C128) NetFront/3.3";
	String  cookie = null;

	/**
	 *  2ch�����擾���[�h��ݒ肷��
	 *  
	 *  @param get2chAppendMode 
	 *      <code>true</code>:�Q����񍷕��擾���[�h
	 *      <code>false</code>:�ʏ탂�[�h
	 */
    public MidpDefaultA2BhttpCommunicator(boolean get2chAppendMode)
    {
    	isGet2chAppendMode = get2chAppendMode;
    }

    /**
     *  �v���L�V�ʐM���[�h���m�F����<br>
     *  (�g�p���Ȃ����[�h����������)
     * @return �v���L�V�ʐM���[�h
     */
    public int GetProxyMode()
    {
    	return (IMidpA2BhttpCommunicator.PROXY_DO_NOT_USE_PROXY);
    }
    
    /**
     *  �v���L�V�ʐM���̃z�X�g���ƃ|�[�g�ԍ�����������<br>
     *  (�����������Ȃ�)
     * @return �v���L�V�ʐM���̃z�X�g���F�|�[�g�ԍ�
     */
    public String GetProxyUrl()
    {
    	return ("");
    }
    
	/** �X�������擾�T�C�Y���擾����
     * @return �X�������擾�T�C�Y(30500�Œ�)
     */
	public int GetDivideGetSize()
	{
		return (30500);
	}
	
	/**
	 *   �X�������擾���[�h���g�p���邩�H
	 * 
	 * @return <code>false</code>: <b>�����擾���[�h�͗��p���Ȃ� </b>
	 */
	public boolean GetDivideGetHttp()
	{
		return (false);
	}

    /**
     *  ���[�U�[�G�[�W�F���g�����擾����
     * @param ua ���[�U�[�G�[�W�F���g
     */
	public void SetUserAgent(String ua)
	{
		userAgent = ua;
	}
	
    /**
     *  ���[�U�G�[�W�F���g����������
     * @return ���[�U�G�[�W�F���g��
     */
    public String GetUserAgent()
    {
    	return (userAgent);
    }
    
    
    /**
     *  cookie��ݒ肷��
     * 
     * @param cookieToSet �ݒ肷�� cookie
     */
    public void SetCookie(String cookieToSet)
    {
        cookie = cookieToSet;
    }

    /**
     *  cookie����������
     *  @return cookie
     */
    public String GetCookie()
    {
    	return (cookie);
    }
    
    /**
     *  ����f�[�^���m�F���A�f�[�^�̎�M���p������m�F����
     * 
     * @param rcCode  �����R�[�h
     * @param isDivideReceive �����擾���s
     * @param data �f�[�^
     * @param contentsOffset �f�[�^�ǂݏo���I�t�Z�b�g
     * @param readSize �f�[�^�T�C�Y
     * @return �ǂݔ�΂��T�C�Y (���̏ꍇ�ɂ͒ʐM���f)
     */    
    public int GetHttpDataReceivedFirst(int rcCode, boolean isDivideReceive, byte[] data, int contentsOffset, int readSize)
    {
        if (isGet2chAppendMode == false)
        {
            // 2�����˂�̃X���擾���[�h�ł͂Ȃ��ꍇ...
        	return (contentsOffset);
        }
    	if (isDivideReceive == false)
        {
            // �X�������擾���̂Q��ڈȍ~�̃��N�G�X�g�������ꍇ...
            return (contentsOffset);
        }

        if ((readSize == 1)&&(data[contentsOffset] == (byte) 10))
        {
            // �X���X�V�Ȃ�...
            return (-1);
        }
        if (data[contentsOffset] != (byte) 10)
        {
            // �f�[�^�擾�G���[...
            return (-10);
        }

        // �����擾�����s�I
        return (contentsOffset + 1);
    }
    
    /**
     *  ��M�����f�[�^��ʒm����iAPPENDMODE_STRING���[�h���̂Ƃ��ɌĂяo���j
     * @param data     ��M�f�[�^�i�[�̈�
     * @param offset   ��M�f�[�^�̃I�t�Z�b�g
     * @param dataSize ��M�f�[�^�̃T�C�Y
     */
    public void ReceivedData(byte[] data, int offset, int dataSize)
    {
        return;
    }

    /**
	 *  HTTP�ʐM�̏I����ʒm����
	 * @param responseCode �����R�[�h
	 * @param bytesRead �ǂݍ��񂾃o�C�g��
	 */
	public void CompletedToGetHttpData(int responseCode, int bytesRead)
	{
        return;
	}
}
