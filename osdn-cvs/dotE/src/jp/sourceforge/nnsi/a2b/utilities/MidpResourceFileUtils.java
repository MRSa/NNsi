package jp.sourceforge.nnsi.a2b.utilities;
import javax.microedition.midlet.MIDlet;
import java.io.InputStream;
import java.lang.Class;

/**
 *  ���\�[�X�t�@�C���A�N�Z�X���[�e�B���e�B
 *  <br>
 *  �[�����ɂ���f�[�^��ǂݏo���Ƃ��ɗ��p�\�ȃ��[�e�B���e�B�ł��B
 * 
 * @author MRSa
 *
 */
public class MidpResourceFileUtils
{
	private MIDlet parent = null;
    private final int TEMP_BUFFER = 8192;
	
    /**
     * ���[�e�B���e�B�N���X�̎g�p����
     *  @param object MIDlet�N���X
     */
	public MidpResourceFileUtils(MIDlet object)
    {
        parent = object;
    }

    /**
     *  jad�t�@�C���Ŏw�肳��Ă���l���擾����
     * 
     * @param key jad�t�@�C���ɏ�����Ă���L�[���[�h
     * @return �v���p�e�B�̒l
     */	
	public String getAppProperty(String key)
	{
    	// jad�t�@�C���Ńf�B���N�g���̏ꏊ���w�肳��Ă��邩�H
    	return (parent.getAppProperty(key));
	}

    /**
     *  ���\�[�X�ɂ���e�L�X�g�f�[�^���ꊇ�œǂݏo��
     * 
     * @param name ���\�[�X�e�L�X�g�t�@�C����
     * @return     �ǂݍ��񂾃e�L�X�g������
     */
    public String getResourceText(String name)
    {
        String resourceData = null;
		try
		{
		    Class c = this.getClass();
		    InputStream is = c.getResourceAsStream(name);
			byte[] data = new byte[TEMP_BUFFER];
			int actual = is.read(data, 0, TEMP_BUFFER);
			resourceData = new String(data, 0, actual);
			is.close();
			data = null;
		}
		catch (Exception ex)
		{
            //
			resourceData = ex.getMessage();
		}
		return (resourceData);
    }
	
    /**
     *  �[���̓����t�@�C�����w�肵�Ă����ꍇ�Ɏw��p�X�𒲐�����
     *  <br>
     *  (WX320K�Łu�u���E�U�ŊJ���v�����s����ꍇ�ɕK�v�I)
     * 
     * @param orgName  �ʏ�̃t�@�C���ꏊ
     * @return �ϊ���̃t�@�C���ۊǏꏊ
     */
	static public String convertInternalFileLocation(String orgName)
	{
        // �ϊ��Ώۂ��H
		if (orgName.indexOf("file:///") == 0)
        {
            // �ϊ��ΏۂłȂ����߁A���̂܂ܐ܂�Ԃ�
            return (orgName);
		}

        // �v���b�g�t�H�[�����m�F����
		String platForm = System.getProperty("microedition.platform");
        String newName = orgName;
		if (platForm.indexOf("WX320K") >= 0)
        {
            // WX320K�̎��ɂ́A�f�B���N�g���ʒu��␳����B
			//  (8�́A'file:///' �̕������؂��邽��)
            newName = "file://localhost/D:/" + orgName.substring(8);
        }
		return (newName);
    }
}
