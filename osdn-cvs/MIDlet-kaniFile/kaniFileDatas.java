import javax.microedition.rms.RecordStore;

/**
 * �A�v���P�[�V�����̃f�[�^���i�[����N���X
 * 
 * @author MRSa
 *
 */
public class kaniFileDatas
{
	private final byte[] PREF_VERSION = { (byte) 0, (byte) 1};  // preference DB�̃o�[�W����...
	
	private final int    VERSION_RECORDID     = 1;               // preference DB�̃o�[�W�����i�[���R�[�h
	private final int    BASEDIR_RECORDID     = 2;               // preference DB�̃x�[�X�f�B���N�g���i�[���R�[�h

	private String        startDirectory       = null;            // �f�B���N�g�������L�^����

	/**
     * �R���X�g���N�^
     *
     */
	kaniFileDatas()
	{
		// �Ƃ肠�����A�Ȃɂ����Ȃ�
	}

	/**
	 * ���\�[�X�̏���...
	 *
	 */
	public void prepare()
	{
		boolean     isReset            = false;
		byte[]       versionInfo       = null;
		byte[]       baseDirectory     = null;
		RecordStore  prefDB             = null;
		try
		{
			// ���R�[�h�X�g�A���I�[�v������
			prefDB = RecordStore.openRecordStore("kaniFile-Settings", true);
			if (prefDB.getNumRecords() != 0)
			{
				// �f�[�^�x�[�X�̃o�[�W�����m�F...
				versionInfo = prefDB.getRecord(VERSION_RECORDID);
				if (versionInfo.length == PREF_VERSION.length)
				{
					for (int loop = 0; ((loop < versionInfo.length)&&(isReset == false)); loop++)
					{
						if (versionInfo[loop] != PREF_VERSION[loop])
						{
							// �o�[�W�������Ⴂ�A�A�A���Z�b�g����
							isReset = true;
							loop = versionInfo.length;
						}
					}
				}

   				// �f�B���N�g����ǂݏo��
    			baseDirectory = prefDB.getRecord(BASEDIR_RECORDID);
    			if ((isReset != true)&&(baseDirectory.length != 0))
    			{
    				// �f�B���N�g���𔽉f������
    				startDirectory = new String(baseDirectory);
    				isReset = false;
    			}
    			else
    			{
    				// �f�B���N�g�����ǂݏo���Ȃ�����...
    				isReset = true;
    			}
			}
			
			// ���R�[�h�����
			prefDB.closeRecordStore();			
		}
		catch (Exception e)
		{
			// ���R�[�h�X�g�A�����ĂȂ���Ε���
			try
			{
				if (prefDB != null)
				{
					prefDB.closeRecordStore();
				}
			}
			catch (Exception e2)
			{
				// �������Ȃ�...
			}
			isReset = true;
		}
		if (isReset == true)
		{
			// �o�[�W�����������Z�b�g����
			startDirectory = "file:///";
		}
		return;
	}

	/**
	 * �f�B���N�g���̉���
	 * @return
	 */
	public String getDirectory()
	{
		return (startDirectory);
	}
	
	/**
	 * �f�B���N�g���̋L�^
	 * @param dirName
	 */
	public void setDirectory(String dirName)
	{
		startDirectory = dirName;
		return;
	}
	
	/**
	 * ���\�[�X��ۊǂ���...
	 *
	 */
	public void store()
	{
		RecordStore  prefDB = null;
		try
		{
			// ���R�[�h�X�g�A���I�[�v������
			prefDB = RecordStore.openRecordStore("kaniFile-Settings", true);

			// 1���R�[�h��...�o�[�W�����ԍ�
			int nofRecord = prefDB.getNumRecords();
			if (nofRecord >= VERSION_RECORDID)
			{
				// �o�[�W�������̏�������(�X�V)...
				prefDB.setRecord(VERSION_RECORDID, PREF_VERSION, 0, PREF_VERSION.length);
			}
			else
			{
				// �o�[�W�������̏�������(�ǉ�)...
				prefDB.addRecord(PREF_VERSION, 0, PREF_VERSION.length);					
			}

			// 2���R�[�h��...�x�[�X�f�B���N�g��
			int size = (startDirectory.getBytes()).length;
			if (nofRecord >= BASEDIR_RECORDID)
			{
				// �x�[�X�f�B���N�g�����̏�������(�X�V)...
				prefDB.setRecord(BASEDIR_RECORDID, (startDirectory).getBytes(), 0, size);
			}
			else
			{
				// �x�[�X�f�B���N�g�����̏�������(�ǉ�)...
				prefDB.addRecord((startDirectory).getBytes(), 0, size);					
			}
			prefDB.closeRecordStore();
		}
		catch (Exception e)
		{
			// ���R�[�h�X�g�A�����ĂȂ���Ε���
			try
			{
				if (prefDB != null)
				{
					prefDB.closeRecordStore();
				}
			}
			catch (Exception e2)
			{
				// �������Ȃ�...
			}
		}
		return;
	}

	/**
	 *  �f�[�^�� int�^����o�C�g��֕ϊ�����
	 * 
	 * 
	 */
/*
	private void serializeData(byte[] buffer, int offset, int convId)
	{
		buffer[offset + 0] = (byte)((convId >> 24)&(0xff));
		buffer[offset + 1] = (byte)((convId >> 16)&(0xff));
		buffer[offset + 2] = (byte)((convId >>  8)&(0xff));
		buffer[offset + 3] = (byte)((convId      )&(0xff));
		return;
	}
*/

	/**
	 *  �o�C�g��f�[�^��int�^�֕ϊ�����
	 * 
	 * 
	 */
/*
	private int deSerializeData(byte[] buffer, int offset)
	{
		return ((((buffer[offset + 0]&(0xff))<< 24) + ((buffer[offset + 1]&(0xff))<< 16) +
                 ((buffer[offset + 2]&(0xff))<<  8) + ((buffer[offset + 3]&(0xff)))));
	}	
*/

}
