package jp.sourceforge.nnsi.a2b.utilities;

/**
 *  NNsi���玝���Ă����AHTML�ȈՃp�[�T<br>
 *  html�̃^�O���������x�̊ȒP�ȕ�������H���s���܂��B<br>
 *  
 * @author MRSa
 *
 */
/*
 *  (�ꉞ�A2ch��dat�t�@�C���\���p�̐��`�@�\�������Ă��܂��B)
 */
public class MidpNNsiHtmlParser
{
	/** �f�[�^��SHIFT JIS�R�[�h */
	static public final  int KANJI_SJIS = 0;

	/** �f�[�^��UTF8�R�[�h */
	static public final  int KANJI_UTF8 = 1;

	/** �f�[�^��EUC�R�[�h */
	static public final  int KANJI_EUC  = 2;
	
	static public  final  int RECOMMEND_PARSESIZE = 10240;  // �ꉞ�̃I�X�X���p�[�X�T�C�Y
	static private final  int MINIMUM_TEMPORARY   = 32768;  // �ŏ��m�ۈꎞ�̈�
	static private final int PARSING_2CH    = -1;	       // 2�����˂�X����̓��[�h
    static private final int PARSING_NORMAL = 0;	           // �ʏ�(HTML)��̓��[�h

    private MidpKanjiConverter kanjiConverter = null; // �����R�[�h�ϊ��N���X
    private int messageParsingMode   = PARSING_2CH;   // ���b�Z�[�W��̓��[�h
    private boolean convertHanzen   = false;         // ���p�J�i �� �S�p�J�i�ϊ����s���H

    /**
     *  ���p�J�i �� �S�p�J�i�ϊ��e�[�u��
     *  
     */
	private final short[] Han2ZenTable = {
      // "�B", "�u", "�v", "�A", "�E", "��",
      (short) 0x8142, (short) 0x8175, (short) 0x8176, 
      (short) 0x8141, (short) 0x8145, (short) 0x8392,
      
      // "�@", "�B", "�D", "�F", "�H", "��",
      (short) 0x8340, (short) 0x8342, (short) 0x8344, 
      (short) 0x8346, (short) 0x8348, (short) 0x8383,

      // "��", "��", "�b", "�[", "�A", "�C",
      (short) 0x8385, (short) 0x8387, (short) 0x8362, 
      (short) 0x815b, (short) 0x8341, (short) 0x8343,

      // "�E", "�G", "�I", "�J", "�L", "�N",
      (short) 0x8345, (short) 0x8347, (short) 0x8349, 
      (short) 0x834a, (short) 0x834c, (short) 0x834e,

      // "�P", "�R", "�T", "�V", "�X", "�Z",
      (short) 0x8350, (short) 0x8352, (short) 0x8354, 
      (short) 0x8356, (short) 0x8358, (short) 0x835a,

      // "�\\", "�^", "�`", "�c", "�e", "�g",
      (short) 0x835c, (short) 0x835e, (short) 0x8360, 
      (short) 0x8363, (short) 0x8365, (short) 0x8367,

      // "�i", "�j", "�k", "�l", "�m", "�n",
      (short) 0x8369, (short) 0x836a, (short) 0x836b, 
      (short) 0x836c, (short) 0x836d, (short) 0x836e,

      // "�q", "�t", "�w", "�z", "�}", "�~",
      (short) 0x8371, (short) 0x8374, (short) 0x8377, 
      (short) 0x837a, (short) 0x837d, (short) 0x837e,

      // "��", "��", "��", "��", "��", "��",
      (short) 0x8380, (short) 0x8381, (short) 0x8382, 
      (short) 0x8384, (short) 0x8386, (short) 0x8388,

      // "��", "��", "��", "��", "��", "��",
      (short) 0x8389, (short) 0x838a, (short) 0x838b, 
      (short) 0x838c, (short) 0x838d, (short) 0x838f,

      // "��", "�J", "�K", "",   "",   ""
      (short) 0x8393, (short) 0x814a, (short) 0x814b, 
      (short) 0x0000, (short) 0x0000, (short) 0x0000
    };

    /**
     *  �����R�[�h�ϊ��N���X���L�����܂�
     * 
     *  @param converter �����R�[�h�ϊ��N���X
     *  @see MidpKanjiConverter
     */
	public MidpNNsiHtmlParser(MidpKanjiConverter converter)
	{
        kanjiConverter = converter;
	}

    /**
     *  HTML�̊ȈՓI��͏���<br>
     *  �p�[�X�����̂��߂Ƀ��[�N�̈�Ƃ���32kB(or 2�{)�̈ꎞ�̈���m�ۂ��܂�<br>
     *  <br>
     *  <BR>
     *  <B>�� ���p��̒��� ��</B><BR>
     *  ���x�I�Ȗ����o��̂ŁA���܂�傫�ȃT�C�Y��ϊ����Ȃ���...�B<BR>
     * 
     * @param source �ϊ����f�[�^
     * @param offset �ϊ����J�n����f�[�^�̐擪�ʒu
     * @param size   �ϊ�����f�[�^�̃f�[�^��
     * @param kanjiCode �ϊ����f�[�^�̊����R�[�h(KANJI_SJIS/KANJI_UTF8/KANJI_EUC)
     * @return �ϊ����ʂ̕�����(string�^)
     */
	public String parseHtml(byte[] source, int offset, int size, int kanjiCode)
    {
        messageParsingMode = PARSING_NORMAL;
        int allocateSize = size + size;
        if (allocateSize < MINIMUM_TEMPORARY)
        {
        	allocateSize = MINIMUM_TEMPORARY;
        }
        byte[] convData = new byte[allocateSize];
        String  data = null;
        try
        {
            parseMessage(convData, source, offset, size);
            if (kanjiCode == KANJI_SJIS)
            {
                // SJIS -> UTF8 �ϊ�����
            	data = kanjiConverter.ParseFromSJ(convData);
            }
            else if (kanjiCode == KANJI_EUC)
            {
                // EUC -> UTF8 �ϊ�����
            	data = kanjiConverter.ParseFromEUC(convData, 0, convData.length);
            }
            else // if (kanjiCode == KANJI_UTF8)
            {
                // �����R�[�h�̕ϊ��͍s��Ȃ�
            	data = new String(convData);
            }
            convData = null;
        }
        catch (Exception ex)
        {
            //
        }
        return (data);
	}
	
    /*=========================================================================*/
	/*   Function : ParseMessage�@�@                                           */
	/*                                       (���b�Z�[�W�p�[�X����...NNsi���) */
	/*=========================================================================*/
	private void parseMessage(byte[] dst, byte[] src, int offset, int size)
	{
		int dataStatus = 0;
		int dstIndex  = 0;
		int srcIndex  = offset;
//        int previousIndex = 0;
		
		// ���x�����߃��[�`������... (�P�����Âp�[�X����)
	    while (srcIndex < (offset + size))
	    {
	        if (src[srcIndex] == '&')
	        {
	            // "&gt;" �� '>' �ɒu��
	            if ((src[srcIndex + 1]== 'g')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
	            {
	                dst[dstIndex] = '>';
					dstIndex++;
					srcIndex = srcIndex + 4;
	                continue;
	            }

				// "&lt;" �� '<' �ɒu��
	            if ((src[srcIndex + 1]== 'l')&&(src[srcIndex + 2] == 't')&&(src[srcIndex + 3] == ';'))
	            {
	                dst[dstIndex] = '<';
					dstIndex++;
					srcIndex = srcIndex + 4;
	                continue;
	            }

				// "&quot;" �� '"' �ɒu��
	            if ((src[srcIndex + 1] == 'q')&&(src[srcIndex + 2] == 'u')&&(src[srcIndex + 3] == 'o')&&
					(src[srcIndex + 4] == 't')&&(src[srcIndex + 5] == ';'))
	            {
	                dst[dstIndex] = '"';
					dstIndex++;
					srcIndex = srcIndex + 6;
	                continue;
	            }

				// "&nbsp;" �� ' ' �ɒu�� (AA�\�����[�h�̂��߂ɁA�X�y�[�X1�ɂ���悤�ύX)
	            if ((src[srcIndex + 1] == 'n')&&(src[srcIndex + 2] == 'b')&&(src[srcIndex + 3] == 's')&&
						(src[srcIndex + 4] == 'p')&&(src[srcIndex + 5] == ';'))
		        {
		                dst[dstIndex] = ' ';
						dstIndex++;
						srcIndex = srcIndex + 6;
		                continue;
		        }

				// "&amp;" �� '&' �ɒu��
	            if ((src[srcIndex + 1] == 'a')&&(src[srcIndex + 2] == 'm')&&(src[srcIndex + 3] == 'p')&&
					(src[srcIndex + 4] == ';'))
	            {
	                dst[dstIndex] = '&';
					dstIndex++;
					srcIndex = srcIndex + 5;
	                continue;
	            }

				// ����͂��肦�Ȃ��͂�����...�ꉞ�B
                dst[dstIndex] = '&';
				dstIndex++;
				srcIndex++;
	            continue;
	        }
	        if (src[srcIndex] == '<')
	        {
	            //  "<>" �́A�Z�p���[�^(��Ԃɂ���ĕς��)
	            if (src[srcIndex + 1] == '>')
	            {
    				srcIndex = srcIndex + 2;
	    			if (messageParsingMode == PARSING_NORMAL)
	    			{
	    				// HTML��̓��[�h�́A�X�y�[�X�P�ɕϊ�����...
			            dst[dstIndex] = ' ';
						dstIndex++;
						continue;
	    			}
	                switch (dataStatus)
	                {
	                  case 0:
	                    // ���O���̋�؂�
                        //nameField = kanjiConverter.ParseFromSJ(dst, previousIndex, (dstIndex - previousIndex));
			            dst[dstIndex] = ' ';
						dstIndex++;
			            dst[dstIndex] = ' ';
						dstIndex++;
	                    dataStatus = 1;
//						previousIndex = dstIndex;
	                    break;

	                  case 1:
	                    // e-mail���̋�؂�
                        //emailField = kanjiConverter.ParseFromSJ(dst, previousIndex, (dstIndex - previousIndex));
				        dst[dstIndex] = '\r';
						dstIndex++;
				        dst[dstIndex] = '\n';
						dstIndex++;
	                    dataStatus = 2;
//						previousIndex = dstIndex;
	                    break;

	                  case 2:
	                    // �N�����E���Ԃ���тh�c���̋�؂�
	                    //dateTimeField = kanjiConverte.ParseFromSJr(dst, previousIndex, (dstIndex - previousIndex));
					    //dst[dstIndex] = '\n';
						//dstIndex++;
					    dst[dstIndex] = '\r';
						dstIndex++;
					    dst[dstIndex] = '\n';
					    dstIndex++;
						dataStatus = 3;
//						previousIndex = dstIndex;
						
						srcIndex++;  // �� ���X�̐擪�ɓ����Ă���X�y�[�X����������
	                    break;

	                  case 3:
	                  default:
	                    // ���b�Z�[�W�̏I�[
			            //messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
					    dst[dstIndex] = '\0';
						dstIndex++;
						//previousIndex = dstIndex;
	                    return;
	                    //break;
	                }
	                continue;
	            }

	            //  "<br>" �́A���s�ɒu��
	            if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
					((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
					(src[srcIndex + 3] == '>'))
	            {
	                // �s���ƍs���̃X�y�[�X������Ă݂�ꍇ
					if ((srcIndex != 0)&&((srcIndex -1) == ' '))
	                {
	                    dstIndex--;
	                }
	                if (src[srcIndex + 4] == ' ')
	                {
				        dst[dstIndex] = '\r';
						dstIndex++;
	                	dst[dstIndex] = '\n';
	                	dstIndex++;
	                	srcIndex = srcIndex + 5;
	                    continue;
	                }
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }

	            //  "<p*>" �́A���s2�ɒu��
	            if ((src[srcIndex + 1] == 'p')||(src[srcIndex + 1] == 'P'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 2;
	                while (src[srcIndex] != '>')
	                {
	                    srcIndex++;
	                    if (srcIndex >= (offset + size))
	                    {
	                        // �m�ۂ����̈���z����...
	                        break;
	                    }
	                }
	                srcIndex++;
	                continue;
	            }

	            // <li>�^�O�����s�R�[�h�� - �ɒu������
	            if (((src[srcIndex + 1] == 'l')||(src[srcIndex + 1] == 'L'))&&
					((src[srcIndex + 2] == 'i')||(src[srcIndex + 2] == 'I'))&&
					(src[srcIndex + 3] == '>'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                dst[dstIndex] = '-';
	                dstIndex++;
	                dst[dstIndex] = ' ';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }
	               
	            //  "<hr>" �́A���s === ���s �ɒu��
	            if (((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))&&
					((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))&&
					(src[srcIndex + 3] == '>'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                dst[dstIndex] = '=';
	                dstIndex++;
	                dst[dstIndex] = '=';
	                dstIndex++;
	                dst[dstIndex] = '=';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }

	            //  "<dt>" �́A���s�ɒu��
	            if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
					((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
					(src[srcIndex + 3] == '>'))
	            {
	                if (dst[dstIndex - 1] != '\n')
	                {
				        dst[dstIndex] = '\r';
						dstIndex++;
	                    dst[dstIndex] = '\n';
	                    dstIndex++;
	                }
	                srcIndex = srcIndex + 4;
	                continue;
	            }

	            //  "</tr>" ����� "</td>" �́A���s�ɒu��
	            if (((src[srcIndex + 2] == 't')||(src[srcIndex + 2] == 'T'))&&
					((src[srcIndex + 3] == 'r')||(src[srcIndex + 3] == 'R'))||
					((src[srcIndex + 3] == 'd')||(src[srcIndex + 3] == 'D'))&&
					(src[srcIndex + 1] == '/')&&(src[srcIndex + 4] == '>'))
	            {
	                if (dst[dstIndex - 1] != '\n')
	                {
				        dst[dstIndex] = '\r';
						dstIndex++;
	                    dst[dstIndex] = '\n';
	                    dstIndex++;
	                }
	                srcIndex = srcIndex + 5;
	                continue;
	            }

	            //  "<dd>" �́A���s�Ƌ�2�ɒu��
	            if (((src[srcIndex + 1] == 'd')||(src[srcIndex + 1] == 'D'))&&
					((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D'))&&
					(src[srcIndex + 3] == '>'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                dst[dstIndex] = ' ';
	                dstIndex++;
	                dst[dstIndex] = ' ';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
	                srcIndex = srcIndex + 4;
	                continue;
	            }
	            
	            //  "<h*>" �́A���s + ���s �ɒu��
	            if ((src[srcIndex + 1] == 'h')||(src[srcIndex + 1] == 'H'))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;

	                // TODO: �����F�̐ݒ�...

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 2;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            //  "</h*>" �́A���s + ���s �ɒu��
	            if ((src[srcIndex + 1] == '/')&&((src[srcIndex + 2] == 'h')||(src[srcIndex + 2] == 'H')))
	            {
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;
			        dst[dstIndex] = '\r';
					dstIndex++;
	                dst[dstIndex] = '\n';
	                dstIndex++;

	                // TODO: �����F�̉���...

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            //  "<a*>" �ŁA�A���J�[�F�̌��o
	            if (((src[srcIndex + 1] == 'a')||(src[srcIndex + 1] == 'A'))&&(src[srcIndex + 2] == ' '))
	            {
	                // TODO: �A���J�[�F�̐ݒ�...

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            //  "</a*>" �ŁA�A���J�[�F�̉���
	            if (((src[srcIndex + 2] == 'a')||(src[srcIndex + 2] == 'A'))&&(src[srcIndex + 1] == '/'))
	            {
	                // TODO: �A���J�[�F�̉���

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
 	                continue;
	            }

	            // <EM* �͋����F��
	            if (((src[srcIndex + 1] == 'e')||(src[srcIndex + 1] == 'E'))&&
					((src[srcIndex + 2] == 'm')||(src[srcIndex + 2] == 'M')))
	            {
	                // TODO: �����F�֐؂�ւ�

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            // </em* �͒ʏ�F��
	            if (((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
					((src[srcIndex + 3] == 'm')||(src[srcIndex + 3] == 'M'))&&
					(src[srcIndex + 1] == '/'))
	            {
	                // TODO: �����F�̉���

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            // <tr>, <td> �̓X�y�[�X�P�ɕϊ�
	            if (((src[srcIndex + 1] == 't')||(src[srcIndex + 1] == 'T'))&&
					((src[srcIndex + 2] == 'r')||(src[srcIndex + 2] == 'R'))||
					((src[srcIndex + 2] == 'd')||(src[srcIndex + 2] == 'D')))
	            {
	                dst[dstIndex] = ' ';
	                dstIndex++;

	                // �^�O�͓ǂݔ�΂�
	                srcIndex = srcIndex + 3;
	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                    {
	                    srcIndex++;
	                }
					srcIndex++;
	                continue;
	            }

	            // <BE: �́ABE ID�̂��ߓǂݔ�΂��Ȃ��B(��ʕ\������)
	            if (((src[srcIndex + 1] == 'b')||(src[srcIndex + 1] == 'B'))&&
					((src[srcIndex + 2] == 'e')||(src[srcIndex + 2] == 'E'))&&
					(src[srcIndex + 3] == ':'))
	            {
	                dst[dstIndex] = ' ';
	                dstIndex++;
	                srcIndex++;

	                while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
	                {
	                	dst[dstIndex] = src[srcIndex];
	                	dstIndex++;
	                	srcIndex++;
	                }
	                srcIndex++;
	                continue;
	            }

	            // ���̑��̃^�O�͓ǂݔ�΂�
	            while ((src[srcIndex] != '>')&&(src[srcIndex] != '\0')&&(srcIndex < (offset + size)))
                {
	                srcIndex++;
	            }
	            // �^�O�͊��S�����Ƃ���
	            //dst[dstIndex] = ' ';
	            //dstIndex++;
				srcIndex++;
	            continue;
	        }

	        // ���p�J�i���S�p�J�i�ϊ����������{����ꍇ...
			if ((convertHanzen == true)&&(src[srcIndex] < 0))
			{
                int checkChar = 256 + src[srcIndex];
				if ((checkChar >= 0x81)&&(checkChar <= 0x9f))
				{
					// 2�o�C�g�����Ɣ���
					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;

					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;
					continue;
				}

				if ((checkChar >= 0xe0)&&(checkChar <= 0xef))
				{
					// 2�o�C�g�����Ɣ���
					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;

					dst[dstIndex] = src[srcIndex];
					dstIndex++;
					srcIndex++;
					continue;
				}

				if ((checkChar >= 0xa1)&&(checkChar <= 0xdf))
				{
	                // �J�^�J�i�����Ɣ���A�e�[�u�����g���Ĕ��p�J�i���S�p�ϊ�����
					short code = Han2ZenTable[checkChar - 0xa1];
					byte temp;

					temp = (byte) ((code & 0xff00) >> 8);
					dst[dstIndex] = temp;
					dstIndex++;

					temp = (byte) ((code & 0x00ff));
					dst[dstIndex] = temp;
					dstIndex++;
										
					srcIndex++;
	                continue;
	            }
	        }

			if ((messageParsingMode != PARSING_NORMAL)&&((src[srcIndex] == 0x0d)||(src[srcIndex] == 0x0a)))
			{
				// �e�L�X�g�\�����[�h�̂Ƃ��ɂ́A���s�R�[�h�𕁒ʂ̈ꕶ���]�ʂ���B
	            srcIndex++;
	            dst[dstIndex] = '\r';
				dstIndex++;
	            dst[dstIndex] = '\n';
				dstIndex++;
				continue;
			}
			
	        // �X�y�[�X���A�����Ă����ꍇ�A�P�Ɍ��炷
			if ((src[srcIndex] == ' ')&&(src[srcIndex + 1] == ' '))
	        {
	            srcIndex++;
	            while ((src[srcIndex] == ' ')&&(srcIndex < (offset + size)))
	            {
	                srcIndex++;
	            }
	            srcIndex--;
	        }

			// NULL ����� 0x0a, 0x0d �͖�������
	        if ((src[srcIndex] != '\0')&&(src[srcIndex] != 0x0a)&&(src[srcIndex] != 0x0d)&&(src[srcIndex] != 0x09))
	        {
	            // ���ʂ̈ꕶ���]��
	            dst[dstIndex] = src[srcIndex];
				dstIndex++;
	        }
	        srcIndex++;
	    }
        //  ���X�̏I�[...
		//messageField = new String(dst, previousIndex, (dstIndex - previousIndex));
		dst[dstIndex] = '\0';
		dstIndex++;
	    return;
	}
}
