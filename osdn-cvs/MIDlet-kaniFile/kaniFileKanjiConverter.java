
public class kaniFileKanjiConverter
{
	private final   int MARGIN   = 8;        // �o�b�t�@�̗]�T
    private boolean useSystemSJ = false;   // �V�X�e���� UTF8 �� Shift JIS���T�|�[�g����Ă��邩
    private boolean useSystemEUC = false;  // �V�X�e���� UTF8 �� EUC���T�|�[�g����Ă��邩

    /**
     *  �R���X�g���N�^
     *
     */
    public kaniFileKanjiConverter()
    {
        // �V�X�e���ŃT�|�[�g���Ă���G���R�[�h�𒲂ׂ�
        useSystemEUC = checkEncode("EUC-JP");
        useSystemSJ  = checkEncode("SJIS");
    }

    /**
     *   UTF8�ϊ��̃��C������
     * 
     */
    public byte[] parseToSJ(String data)
    {
        if (useSystemSJ == true)
        {
            try
            {
                return (data.getBytes("SJIS"));
            }
            catch (Exception e)
            {
                return (data.getBytes());
            }
        }
        if (useSystemEUC == true)
        {
            byte[] eucLen = null;
            byte[] sjLen  = null;
            try
            {
                eucLen = data.getBytes("EUC-JP");
                sjLen = new byte[eucLen.length + 2];
                StrCopySJ(sjLen, 0, eucLen, 0);
                eucLen = null;
                return (sjLen);
            }
            catch (Exception e)
            {
                eucLen = null;
                sjLen  = null;
                return (data.getBytes());
            }
        }
        // "��"���������������B�B�B
        return (data.getBytes());
    }

    /**
     *   �������SHIFT JIS�ɕϊ����A�܂�URL�G���R�[�h���ĉ�������
     * 
     * 
     */
    public String parseToSJAndUrlEncode(String data)
    {
        // �ʂ�ۃ`�F�b�N
        if (data == null)
        {
            return ("");
        }
        return (urlEncode(parseToSJ(data)));
    }

    /**
     *   ������(�o�C�g��)��URL�G���R�[�h���ĉ�������
     * 
     * 
     */
    public String urlEncode(byte[] target)
    {
        String buffer = "";
        for (int loop = 0; loop < target.length; loop++)
        {
            byte convData = target[loop];

            if (convData == ' ')
            {
                buffer = buffer + "+";
            }
            else if (convData == '\n')
            {
                buffer = buffer + "%0D%0A";
            }
            else if (convData == '.')
            {
                buffer = buffer + ".";
            }
            else if (convData == '_')
            {
                buffer = buffer + "_";
            }
            else if (convData == '-')
            {
                buffer = buffer + "-";
            }
            else if (convData == '*')
            {
                buffer = buffer + "*";
            }
            else if ((convData >= '0')&&(convData <= '9'))
            {
                int targetData = convData - '0';
                buffer = buffer + targetData;
            }
            else if ((convData >= 'A')&&(convData <= 'Z'))
            {
                String targetData = new String(target, loop, 1);
                buffer = buffer + targetData;
                targetData = null;
            }
            else if ((convData >= 'a')&&(convData <= 'z'))
            {
                String targetData = new String(target, loop, 1);
                buffer = buffer + targetData;
                targetData = null;
            }
            else
            {
                int targetInt = convData;
                if (targetInt < 0)
                {
                    targetInt = targetInt + 256;
                }
                if ((targetInt != 0)&&(targetInt != 0x0d))
                {
                    if (targetInt < 16)
                    {
                        buffer = buffer + "%0" + (Integer.toHexString(targetInt)).toUpperCase();                        
                    }
                    else
                    {
                        buffer = buffer + "%" + (Integer.toHexString(targetInt)).toUpperCase();
                    }
                }
            }
        }
        return (buffer);        
    }

	/**
	 * �o�C�g�f�[�^��UTF8��String�^�֕ϊ����鏈��
	 * 
	 * @param   data �o�C�g�f�[�^
	 * @return  UTF8�ϊ��ς݂�String�^�f�[�^
	 */
	public String parsefromSJ(byte[]data)
	{
		if (data == null)
		{
			return ("");
		}
		return (parsefromSJ(data, 0, data.length));
	}

	/**
	 * �o�C�g�f�[�^(EUC)��UTF8��String�^�֕ϊ����鏈��
	 * 
	 * @param data    �o�C�g�f�[�^
	 * @param offset  �o�C�g�f�[�^�̒��ŕϊ����J�n����ʒu
	 * @param length  �ϊ�����o�C�g��
	 * @return        UTF8�ϊ��ς݂�String�^�f�[�^
	 */
	public String parsefromEUC(byte[] data, int offset, int length)
	{
		// �ϊ����W�b�N��I������
		if (useSystemEUC == true)
		{
			// EUC-JP�ŕϊ����ė��p����
			return (parseToUtf8UsingEUC(data, offset, length));
		}
		else if (useSystemSJ == true)
		{
			// EUC-JP����xSHIFT JIS�֕ϊ�����
			byte[] tempBuffer = new byte[(length * 2) + MARGIN];
			StrCopyEUCtoSJ(tempBuffer, 0, data, offset, length);
			return (parseToUtf8UsingSJ(tempBuffer, 0, length));
		}

		// �ϊ����Ȃ�...
		return (new String(data, offset, length));
	}
	
	/**
	 * �o�C�g�f�[�^��UTF8��String�^�֕ϊ����鏈��
	 * 
	 * @param data    �o�C�g�f�[�^
	 * @param offset  �o�C�g�f�[�^�̒��ŕϊ����J�n����ʒu
	 * @param length  �ϊ�����o�C�g��
	 * @return        UTF8�ϊ��ς݂�String�^�f�[�^
	 */
	public String parsefromSJ(byte[] data, int offset, int length)
	{
		// �ϊ����W�b�N��I������
		if (useSystemSJ == true)
		{
			// SHIFT JIS�����̂܂ܗ��p����
			return (parseToUtf8UsingSJ(data, offset, length));
		}
		else if (useSystemEUC == true)
		{
			// EUC-JP�ŕϊ����ė��p����
			return (parseToUtf8UsingEUC(data, offset, length));
		}

		// �ϊ����Ȃ�...
		return (new String(data, offset, length));
	}

	/**
	 *=========================================================================*
	 *   Function : ConvertSJtoEUC                                             *
	 *                        �����R�[�h��SHIFT JIS����EUC�ɕϊ�����(NNsi���) *
	 *=========================================================================*
	 **/
	public int StrCopySJtoEUC(byte[] dst, int dstOffset, byte[] src, int srcOffset, int length)
	{
        int srcIndex = 0;
		int dstIndex = 0;
		
	    while ((srcIndex < length)&&(src[srcIndex + srcOffset] != '\0'))
	    {
			int checkChar;
			if (src[srcIndex + srcOffset] < 0)
			{
				checkChar = 256 + src[srcIndex + srcOffset];
			}
			else
			{
				checkChar = src[srcIndex + srcOffset];
			}
			
			// ���p�A���t�@�x�b�g�R�[�h�͂��̂܂ܕ\������
	        if (checkChar <= 0x80)
	        {
                dst[dstIndex + dstOffset] = src[srcIndex + srcOffset];
				dstIndex++;
				srcIndex++;
				continue;
	        }

			// ���p�J�i�R�[�h��EUC�����R�[�h�ɕϊ�����
            if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
	        {
	            dst[dstIndex + dstOffset] = (byte) 0x8e;
	            dstIndex++;
	            dst[dstIndex + dstOffset] = src[srcIndex + srcOffset];
	            dstIndex++;
				srcIndex++;
				continue;
            }

			// JIS�����R�[�h�֕ϊ����鏈��...
		    int  upper, lower;

		    // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
			upper = checkChar;
			if (src[srcIndex + srcOffset + 1] < 0)
			{
				lower = 256 + src[srcIndex + srcOffset + 1];
			}
			else
			{
				lower = src[srcIndex + srcOffset + 1];
			}

		    if (upper <= 0x9f)
		    {
		        if (lower < 0x9f)
		        {
		            upper = (upper << 1) - 0xe1;
		        }
		        else
		        {
		            upper = (upper << 1) - 0xe0;
		        }
		    }
		    else
		    {
		        if (lower < 0x9f)
		        {
		            upper = ((upper - 0xb0) << 1) - 1;
		        }
		        else
		        {
		            upper = ((upper - 0xb0) << 1);
		        }
		    }
		    if (lower < 0x7f)
		    {
		        lower = lower - 0x1f;
		    }
		    else
		    {
		        if (lower < 0x9f)
		        {
		            lower = lower - 0x20;
		        }
		        else
		        {
		            lower = lower - 0x7e;
		        }
		    }

			// JIS -> EUC�̊����R�[�h�ϊ������{...
			upper = (upper & 0x000000ff)|(0x80);
			lower = (lower & 0x000000ff)|(0x80);
			
			dst[dstIndex + dstOffset] = (byte) upper;
			dstIndex++;
			dst[dstIndex + dstOffset] = (byte) lower;
			dstIndex++;
			srcIndex = srcIndex + 2;
	    }
		dst[dstIndex + dstOffset] = 0;
		dstIndex++;
	    return (dstIndex);
	}

	/*=========================================================================*/
	/*   Function : StrCopySJ                                                  */
	/*                          EUC�����R�[�h��SHIFT JIS�ɕϊ����ĕ�����R�s�[ */
	/*=========================================================================*/
	public void StrCopyEUCtoSJ(byte[] dst, int dstOffset, byte[] src, int srcOffset, int length)
	{
		int dstIndex  = 0;
		int srcIndex  = 0;
        int matchData = 0;
	    while ((srcIndex < length)&&((dstIndex + dstOffset) < dst.length)&&(src[srcOffset + srcIndex] != 0))
	    {
			matchData = src[srcOffset + srcIndex];
			if (matchData < 0)
			{
				matchData = matchData + 256;
			}
			
			if (matchData == 0x8e)
		    {
		        // ���p�J�i�R�[�h
				srcIndex++;
		        dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
		        dstIndex++;
				continue;
		    }
		    if ((matchData >= ((0x80)|(0x21)))&&(matchData <= ((0x80)|(0x7e))))
		    {
		        // EUC�����R�[�h�Ɣ���AJIS�����R�[�h�Ɉ�x�ϊ����Ă���SHIFT JIS�ɕϊ�
				srcIndex++;
				byte[] temp = new byte[2];
		        temp[0] = (byte) ((matchData)&(0x7f));
		        int data = src[srcOffset + srcIndex];
				srcIndex++;
				if (data < 0)
				{
					data = data + 256;
				}
		        temp[1] = (byte) ((data)&(0x7f));
		        ConvertJIStoSJ(dst, (dstOffset + dstIndex), temp, 0);
				dstIndex = dstIndex + 2;
				continue;
		    }
			dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
			dstIndex++;
			srcIndex++;
	    }
		if (((srcIndex + srcOffset) < src.length)&&((dstIndex + dstOffset) < dst.length))
		{
			dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
		}
	    return;
	}

    /**
     *  �G���R�[�h�`�F�b�N�̃��C������...
     *  
     */
    private boolean checkEncode(String enc)
    {
        try
        {
            // ���߂��Ɋ�����ϊ����Ă݂�...
            String text = "��";
            text.getBytes(enc);
            return (true);
        }
        catch (Exception e)
        {
            // �ϊ��̓T�|�[�g���Ă��Ȃ������I�I
        }
        return (false);
    }
    /*=======================================================================*/
    /*   Function : StrCopySJ                                                */
    /*                        EUC�����R�[�h��SHIFT JIS�ɕϊ����ĕ�����R�s�[ */
    /*=======================================================================*/
    private void StrCopySJ(byte[] dst, int dstOffset, byte[] src, int srcOffset)
    {
        int dstIndex  = 0;
        int srcIndex  = 0;
        int matchData = 0;
        while (((srcIndex + srcOffset) < src.length)&&((dstIndex + dstOffset) < dst.length)&&(src[srcOffset + srcIndex] != 0))
        {
            matchData = src[srcOffset + srcIndex];
            if (matchData < 0)
            {
                matchData = matchData + 256;
            }
            
            if (matchData == 0x8e)
            {
                // ���p�J�i�R�[�h
                srcIndex++;
                dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
                dstIndex++;
                continue;
            }
            if ((matchData >= ((0x80)|(0x21)))&&(matchData <= ((0x80)|(0x7e))))
            {
                // EUC�����R�[�h�Ɣ���AJIS�����R�[�h�Ɉ�x�ϊ����Ă���SHIFT JIS�ɕϊ�
                srcIndex++;
                byte[] temp = new byte[2];
                temp[0] = (byte) ((matchData)&(0x7f));
                int data = src[srcOffset + srcIndex];
                srcIndex++;
                if (data < 0)
                {
                    data = data + 256;
                }
                temp[1] = (byte) ((data)&(0x7f));
                ConvertJIStoSJ(dst, (dstOffset + dstIndex), temp, 0);
                dstIndex = dstIndex + 2;
                continue;
            }
            dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
            dstIndex++;
            srcIndex++;
        }
        if (((srcIndex + srcOffset) < src.length)&&((dstIndex + dstOffset) < dst.length))
        {
            dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
        }
        return;
    }

    /*=======================================================================*/
    /*   Function : ConvertJIStoSJ                                           */
    /*                          JIS�����R�[�h��SHIFT JIS�����R�[�h�ɕϊ����� */
    /*=======================================================================*/
    private void ConvertJIStoSJ(byte[] dst, int dstOffset, byte[] src, int srcOffset)
    {
        int  upper, lower;

        // ���8�r�b�g/����8�r�b�g��ϐ��ɃR�s�[����
        upper = (int) src[srcOffset];
        lower = (int) src[srcOffset + 1];

        // byte -> int �ϊ��Œl�ϊ�
        if (upper < 0)
        {
            upper = 256 + upper;
        }
        if (lower < 0)
        {
            lower = 256 + lower;
        }

        // ���������C����ɂ��A���S���Y�����T(p.110)�̃R�[�h���g�킹�Ă�������
        if ((upper & 1) != 0)
        {
            if (lower < 0x60)
            {
                lower = lower + 0x1f;
            }
            else
            {
                lower = lower + 0x20;
            }
        }
        else
        {
            lower = lower + 0x7e;
        }   
        if (upper < 0x5f)
        {
            upper = (upper + 0xe1) >> 1;
        }
        else
        {
            upper = (((upper + 0x61) >> 1)|(0x80));
        }

        dst[dstOffset]     = (byte) (upper & (0xff));
        dst[dstOffset + 1] = (byte) (lower & (0xff));
        return;
    }

    /**
	 *  SHIFT-JIS => UTF8 �̕ϊ�����
	 *  (System�� SJ=>utf8���T�|�[�g���Ă���ꍇ)
	 * 
	 * @param data
	 * @param offset
	 * @param length
	 * @return
	 */
	private String parseToUtf8UsingSJ(byte[] data, int offset, int length)
	{
		String buffer = null;
		int index = 0;
		while ((index < length)&&(data[index + offset] != '\0'))
		{
			index++;
		}
		try
		{
			buffer = new String(data, offset, index, "SJIS");
		}
		catch (Exception e)
		{
			buffer = new String(data, offset, length);
		}		
		return (buffer);
	}

	/**
	 *  SHIFT-JIS => UTF8 �̕ϊ����� 
	 *  (System�� euc -> utf8���T�|�[�g���Ă���ꍇ�ASJ->euc�����{���Ă���utf8�ϊ����s��)
	 * 
	 * @param data
	 * @param offset
	 * @param length
	 * @return
	 */
	private String parseToUtf8UsingEUC(byte[] data, int offset, int length)
	{
		String buffer = null;
		byte[] tempBuffer = new byte[(length * 2) + MARGIN];
		int parsedLength  = StrCopySJtoEUC(tempBuffer, 0, data, offset, length);

		try
		{
			buffer = new String(tempBuffer, 0, parsedLength, "EUC-JP");
		}
		catch (Exception e)
		{
			buffer = new String(tempBuffer, 0, parsedLength);
		}
		tempBuffer = null;
		return (buffer);
	}

}
