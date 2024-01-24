/**
 *  a2B Framework�̊O���ɂ���A���[�e�B���e�B�p�b�P�[�W�ł��B<br>
 * �ia2B Framework�Ƃ͐؂藣���āj�Ɨ��ŗ��p�\�ɂȂ��Ă��܂��B
 */
package jp.sourceforge.nnsi.a2b.utilities;

/**
 *   MidpA2BhttpCommunication �Ŏg�p����A�C���^�t�F�[�X�N���X�ł��B<br>
 *  ���[�U�[�G�[�W�F���g�̃J�X�^�}�C�Y�ȂǁA���̃C���^�t�F�[�X�����������N���X�ōs���Ă��������B
 * 
 * @see MidpA2BhttpCommunication
 * @author MRSa
 */
public interface IMidpA2BhttpCommunicator
{
    /** ��M�f�[�^���t�@�C���ɋL�^���Ȃ� */
	static public  final int APPENDMODE_STRING       = 2;  // �f�[�^���t�@�C���Ɋi�[���Ȃ�

	/** ��M�f�[�^���A�w�肵���t�@�C���ɒǋL���� */
	static public  final int APPENDMODE_APPENDFILE   = 1;  // �t�@�C���ɒǋL

	/** ��M�f�[�^���A�w�肵���t�@�C���ɏ㏑���L�^���� */
	static public  final int APPENDMODE_NEWFILE      = 0;  // �t�@�C����V���ɍ쐬���ċL�^����

    /** Proxy�ʐM���[�h���g�p���Ȃ� */
	static public final int PROXY_DO_NOT_USE_PROXY = 0;
	
	/** Proxy�ʐM���[�h (1: WX310�ʐM�p�������g�p����) */
    static public final int PROXY_WORKAROUND_WX310 = 1;

    /** Proxy�ʐM���[�h (2: �v���L�V�ʐM�����s����) */
    static public final int PROXY_DO_USE_PROXY     = 2;

    /**
     *  �v���L�V�ʐM���[�h���m�F����
     * @return �v���L�V�ʐM���[�h
     */
    public abstract int GetProxyMode();
    
    /**
     *   �v���L�V�ʐM���̃z�X�g���ƃ|�[�g�ԍ�����������
     * @return �v���L�V�ʐM���̃z�X�g���F�|�[�g�ԍ�
     */
    public abstract String GetProxyUrl();
    
	/** �X�������擾�T�C�Y���擾����
     * @return �X�������擾�T�C�Y
     */
	public abstract int GetDivideGetSize();
	
	/**
	 *   �X�������擾���[�h���g�p���邩�H
	 * 
	 * @return
	 * �@   <code>true</code>:�����擾���[�h�𗘗p����
	 *      <code>false</code>:�����擾���[�h�͗��p���Ȃ�
	 */
	public abstract boolean GetDivideGetHttp();

    /**
     *   ���[�U�G�[�W�F���g����������
     * 
     * @return ���[�U�G�[�W�F���g��
     */
    public abstract String GetUserAgent();

    /**
     *   Cookie����������
     * 
     * @return cookie
     */
    public abstract String GetCookie();
    
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
    public abstract int GetHttpDataReceivedFirst(int rcCode, boolean isDivideReceive, byte[] data, int contentsOffset, int readSize);

    /**
     * ��M�����f�[�^��ʒm����iAPPENDMODE_STRING���[�h���̂Ƃ��ɌĂяo���j
     * @param data     ��M�f�[�^�i�[�̈�
     * @param offset   ��M�f�[�^�̃I�t�Z�b�g
     * @param dataSize ��M�f�[�^�̃T�C�Y
     */
    public abstract void ReceivedData(byte[] data, int offset, int dataSize);

    /**
	 *   HTTP�ʐM�̏I����ʒm����
	 * @param responseCode �����R�[�h
	 * @param bytesRead �ǂݍ��񂾃o�C�g��
	 */
	public abstract void CompletedToGetHttpData(int responseCode, int bytesRead);
}
