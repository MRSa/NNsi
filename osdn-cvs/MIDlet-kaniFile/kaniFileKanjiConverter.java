
public class kaniFileKanjiConverter
{
	private final   int MARGIN   = 8;        // バッファの余裕
    private boolean useSystemSJ = false;   // システムで UTF8 → Shift JISがサポートされているか
    private boolean useSystemEUC = false;  // システムで UTF8 → EUCがサポートされているか

    /**
     *  コンストラクタ
     *
     */
    public kaniFileKanjiConverter()
    {
        // システムでサポートしているエンコードを調べる
        useSystemEUC = checkEncode("EUC-JP");
        useSystemSJ  = checkEncode("SJIS");
    }

    /**
     *   UTF8変換のメイン処理
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
        // "生"文字列を応答する。。。
        return (data.getBytes());
    }

    /**
     *   文字列をSHIFT JISに変換し、またURLエンコードして応答する
     * 
     * 
     */
    public String parseToSJAndUrlEncode(String data)
    {
        // ぬるぽチェック
        if (data == null)
        {
            return ("");
        }
        return (urlEncode(parseToSJ(data)));
    }

    /**
     *   文字列(バイト列)をURLエンコードして応答する
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
	 * バイトデータをUTF8のString型へ変換する処理
	 * 
	 * @param   data バイトデータ
	 * @return  UTF8変換済みのString型データ
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
	 * バイトデータ(EUC)をUTF8のString型へ変換する処理
	 * 
	 * @param data    バイトデータ
	 * @param offset  バイトデータの中で変換を開始する位置
	 * @param length  変換するバイト数
	 * @return        UTF8変換済みのString型データ
	 */
	public String parsefromEUC(byte[] data, int offset, int length)
	{
		// 変換ロジックを選択する
		if (useSystemEUC == true)
		{
			// EUC-JPで変換して利用する
			return (parseToUtf8UsingEUC(data, offset, length));
		}
		else if (useSystemSJ == true)
		{
			// EUC-JPを一度SHIFT JISへ変換する
			byte[] tempBuffer = new byte[(length * 2) + MARGIN];
			StrCopyEUCtoSJ(tempBuffer, 0, data, offset, length);
			return (parseToUtf8UsingSJ(tempBuffer, 0, length));
		}

		// 変換しない...
		return (new String(data, offset, length));
	}
	
	/**
	 * バイトデータをUTF8のString型へ変換する処理
	 * 
	 * @param data    バイトデータ
	 * @param offset  バイトデータの中で変換を開始する位置
	 * @param length  変換するバイト数
	 * @return        UTF8変換済みのString型データ
	 */
	public String parsefromSJ(byte[] data, int offset, int length)
	{
		// 変換ロジックを選択する
		if (useSystemSJ == true)
		{
			// SHIFT JISをそのまま利用する
			return (parseToUtf8UsingSJ(data, offset, length));
		}
		else if (useSystemEUC == true)
		{
			// EUC-JPで変換して利用する
			return (parseToUtf8UsingEUC(data, offset, length));
		}

		// 変換しない...
		return (new String(data, offset, length));
	}

	/**
	 *=========================================================================*
	 *   Function : ConvertSJtoEUC                                             *
	 *                        漢字コードをSHIFT JISからEUCに変換する(NNsiより) *
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
			
			// 半角アルファベットコードはそのまま表示する
	        if (checkChar <= 0x80)
	        {
                dst[dstIndex + dstOffset] = src[srcIndex + srcOffset];
				dstIndex++;
				srcIndex++;
				continue;
	        }

			// 半角カナコードはEUC漢字コードに変換する
            if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
	        {
	            dst[dstIndex + dstOffset] = (byte) 0x8e;
	            dstIndex++;
	            dst[dstIndex + dstOffset] = src[srcIndex + srcOffset];
	            dstIndex++;
				srcIndex++;
				continue;
            }

			// JIS漢字コードへ変換する処理...
		    int  upper, lower;

		    // 上位8ビット/下位8ビットを変数にコピーする
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

			// JIS -> EUCの漢字コード変換を実施...
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
	/*                          EUC漢字コードをSHIFT JISに変換して文字列コピー */
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
		        // 半角カナコード
				srcIndex++;
		        dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
		        dstIndex++;
				continue;
		    }
		    if ((matchData >= ((0x80)|(0x21)))&&(matchData <= ((0x80)|(0x7e))))
		    {
		        // EUC漢字コードと判定、JIS漢字コードに一度変換してからSHIFT JISに変換
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
     *  エンコードチェックのメイン処理...
     *  
     */
    private boolean checkEncode(String enc)
    {
        try
        {
            // ためしに漢字を変換してみる...
            String text = "漢";
            text.getBytes(enc);
            return (true);
        }
        catch (Exception e)
        {
            // 変換はサポートしていなかった！！
        }
        return (false);
    }
    /*=======================================================================*/
    /*   Function : StrCopySJ                                                */
    /*                        EUC漢字コードをSHIFT JISに変換して文字列コピー */
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
                // 半角カナコード
                srcIndex++;
                dst[dstOffset + dstIndex] = src[srcOffset + srcIndex];
                dstIndex++;
                continue;
            }
            if ((matchData >= ((0x80)|(0x21)))&&(matchData <= ((0x80)|(0x7e))))
            {
                // EUC漢字コードと判定、JIS漢字コードに一度変換してからSHIFT JISに変換
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
    /*                          JIS漢字コードをSHIFT JIS漢字コードに変換する */
    /*=======================================================================*/
    private void ConvertJIStoSJ(byte[] dst, int dstOffset, byte[] src, int srcOffset)
    {
        int  upper, lower;

        // 上位8ビット/下位8ビットを変数にコピーする
        upper = (int) src[srcOffset];
        lower = (int) src[srcOffset + 1];

        // byte -> int 変換で値変換
        if (upper < 0)
        {
            upper = 256 + upper;
        }
        if (lower < 0)
        {
            lower = 256 + lower;
        }

        // 奥村さんのC言語によるアルゴリズム辞典(p.110)のコードを使わせていただく
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
	 *  SHIFT-JIS => UTF8 の変換処理
	 *  (Systemが SJ=>utf8をサポートしている場合)
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
	 *  SHIFT-JIS => UTF8 の変換処理 
	 *  (Systemが euc -> utf8をサポートしている場合、SJ->eucを実施してからutf8変換を行う)
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
