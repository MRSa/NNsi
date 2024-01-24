import java.io.InputStream;
import java.io.OutputStream;
import java.util.Enumeration;
import java.util.Vector;
import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;

/**
 *  ���[�J�����ځ[��L�[���[�h�Ǘ��N���X
 * @author MRSa
 *
 */
public class a2BNgWordManager
{
    // �I�u�W�F�N�g
    private a2BMain         parent        = null;
    private a2Butf8Conveter utf8Converter = null;
    private a2BsjConverter  sjConverter   = null;

    private final String   ngWordFile    = "NGWORD.CSV";
    private final int      bufferMargin   = 16; 
    
    private Vector          ngWordList    = null;

    public  final int CHECKING_ALL   = 0;  // �X���S�̂Ń`�F�b�N����
    public  final int CHECKING_NAME  = 1;  // ���O���Ń`�F�b�N����
    public  final int CHECKING_EMAIL = 2;  // email���Ń`�F�b�N����

    /**
     * �R���X�g���N�^
     * @param object     �e�N���X
     * @param converter  SJ -> utf8�ϊ��N���X
     */
    public a2BNgWordManager(a2BMain object, a2Butf8Conveter converter, a2BsjConverter converterSJ)
    {
        // �e���L�^����...
        parent = object;
        
        // �R���o�[�^�̓o�^
        utf8Converter = converter;
        sjConverter   = converterSJ;
        
        ngWordList = new Vector();
    }

    /**
     *   NG�L�[���[�h���X�g��W�J����
     *
     */
    public void restore()
    {
        FileConnection connection    = null;
        InputStream    in            = null;

        byte[]         buffer        = null;
        int            dataReadSize  = 0;
        
        // �t�@�C�����𐶐�����
        String fileName = parent.getBaseDirectory() + ngWordFile;
        try
        {
            connection   = (FileConnection) Connector.open(fileName, Connector.READ);
            int size     = (int) connection.fileSize();

            //////  �f�[�^���ŏ����� size���ǂݏo��...  /////
            in           = connection.openInputStream();
            buffer       = new byte[size + bufferMargin];
            dataReadSize = in.read(buffer, 0, size);

            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(ngWord)", fileName + " EXCEption  e:" + e.getMessage());
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                //            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                //
            }
            buffer = null;
        }
        in = null;
        connection = null;

        for (int offset = 0; offset < dataReadSize; offset++)
        {
            int length = 0;
            if ((buffer[offset] == 10)||(buffer[offset] == 0x0a))
            {
                try
                {
                    // 1�s�P�ʂŃf�[�^�𒊏o����...
                    length = pickupData(buffer, (offset + 1), dataReadSize);
                    offset = offset + length;
                }
                catch (Exception e)
                {
                    // ��O����...
                    offset = offset + dataReadSize;
                }
            }
        }
        buffer = null;
        System.gc();
        return;
    }

    /**
     *   NG�L�[���[�h���X�g��ۊǂ���
     *
     */
    public void backup()
    {
        FileConnection connection = null;
        OutputStream  out = null;
        
        // �t�@�C�����𐶐�����
        String fileName = parent.getBaseDirectory() + ngWordFile;
        try 
        {
            // �t�@�C���̏�������
            connection = (FileConnection) Connector.open(fileName, Connector.READ_WRITE);
            if (connection.exists() != true)
            {
                // �t�@�C���쐬
                connection.create();
            }
            else
            {
                // �t�@�C�������݂����ꍇ�ɂ́A��x�����č�蒼��
                connection.delete();
                connection.create();
            }
            out        = connection.openOutputStream();

            // �C���f�b�N�X�f�[�^�̃R�����g���o�͂���
            String comment = ";; 0, 0, 0,(ngWord)\n";
            out.write(comment.getBytes());

            Enumeration cnt = ngWordList.elements();
            while (cnt.hasMoreElements())
            {
                a2BngWord data = (a2BngWord) cnt.nextElement();
                if (data != null)
                {
                    String outputData = data.checkingType + ", 0, 0,";
                    out.write(outputData.getBytes());
                    out.write(data.getWord());
                    outputData = null;
                    outputData = "\n";
                    out.write(outputData.getBytes());
                }
            }
            comment = ";[End]";
            out.write(comment.getBytes());
            out.close();
            connection.close();
        }
        catch (Exception e)
        {
            // ��O�����I�I�I
            parent.showDialog(parent.SHOW_EXCEPTION, 0, "Debug(output NG word)", " Exception  ee:" + e.getMessage());
            try
            {
                if (out != null)
                {
                    out.close();
                }
            }
            catch (Exception e2)
            {
            
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
            
            }
        }
        out = null;
        connection = null;
        System.gc();
        return;        
    }

    /**
     *  �L�[���[�h�̏�����
     *
     */
    public void clean()
    {
        ngWordList = null;
        System.gc();
    }

    /**
     *  NG���[�h����������Ԃ�
     *
     */
    public int numberOfNgWords()
    {
        return (ngWordList.size());
    }


    /**
     *  NG���[�h�̃`�F�b�N�^�C�v����������
     *
     *  @param  index
     */
    public int getNgCheckingType(int index)
    {
        if ((index < 0)||(index >= ngWordList.size()))
        {
            return (-1);
        }
        a2BngWord ngWord = (a2BngWord) ngWordList.elementAt(index);
        if (ngWord == null)
        {
            return (-1);
        }
        return (ngWord.checkingType);
    }
    
    /**
     *  NG���[�h����������
     *
     */
    public String getNgWord(int index)
    {
        if ((index < 0)||(index >= ngWordList.size()))
        {
            return (null);
        }
        a2BngWord ngWord = (a2BngWord) ngWordList.elementAt(index);
        if (ngWord == null)
        {
            return (null);
        }
        return (utf8Converter.parsefromSJ(ngWord.getWord()));
    }

    /**
     *  NG���[�h��o�^����
     *
     */
    public void entryNgWord(String ngWord, int checkType)
    {
        a2BngWord dataHolder    = new a2BngWord(sjConverter.parseToSJ(ngWord), checkType);
        ngWordList.addElement(dataHolder);
        return;
    }
    
    /**
     *   ngword.csv ���� Vector�փf�[�^��W�J����...
     *   (���Ȃ�x�^�x�^...)
     * 
     */
    private int pickupData(byte[] buffer, int offset, int limit)
    {
        int length = 0;

        // �f�[�^���Ȃ��Ƃ�...
        if (offset + 7 > limit)
        {
            length = limit - offset + 1;
            return (length);
        }
        
        // �R�����g�s�������A�A�A�P�s�ǂݔ�΂�
        if (buffer[offset] == 59)
        {
            while (((offset + length) < limit)&&(buffer[offset + length] != 10))
            {
                length++;
            }
            return (length);
        }

        int nameOffset = offset + length;
        int nameLength = 0;

        // checkType�𒊏o����
        int checkType = 0;
        while ((buffer[nameOffset + nameLength] >= '0')&&(buffer[nameOffset + nameLength] <= '9'))
        {
            checkType = checkType * 10 + (buffer[nameOffset + nameLength] - '0');
            nameLength++;
        }        
        while (buffer[nameOffset + nameLength] != 44)
        {
            nameLength++;
        }
        length = (nameOffset + nameLength + 1) - offset;

        // �_�~�[�̈�P
        int dummy1 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy1 = dummy1 * 10 + (buffer[offset + length] - '0');
            length++;
        }
        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;
        
        //�@�_�~�[�̈�Q
        int dummy2 = 0;
        while ((buffer[offset + length] >= '0')&&(buffer[offset + length] <= '9'))
        {
            dummy2 = dummy2 * 10 + (buffer[offset + length] - '0');
            length++;
        }

        // �f�[�^�ǂݔ�΂�...
        while (buffer[offset + length] != 44)
        {
            length++;
        }
        length++;

        // �󔒂�ǂݔ�΂�
        while (buffer[offset + length] == ' ')
        {
            length++;
        }

        // NG���[�h�𒊏o����
        nameOffset = offset + length;
        nameLength = 0;
        while ((buffer[nameOffset + nameLength] != 0)&&(buffer[nameOffset + nameLength] != 0x0d)&&(buffer[nameOffset + nameLength] != 0x0a))
        {
            nameLength++;
        }
        
        length = (nameOffset + nameLength) - offset;
        while (buffer[offset + length] != 10)
        {
            length++;
        }

        //  �f�[�^��Vector�̈�Ɋi�[����
        a2BngWord dataHolder    = new a2BngWord(buffer, nameOffset, nameLength, checkType);
        ngWordList.addElement(dataHolder);

        return (length);
    }

    /**
     *  NG�L�[���[�h�ێ��N���X
     * 
     * @author MRSa
     *
     */
    public class a2BngWord
    {
        private final int MARGIN         = 2;  // ���܂�̈�̊m��
        public  int     checkingType      = CHECKING_ALL;  // �L�[���[�h���
        private byte[]  ngWord            = null;          // �L�[���[�h
//        private int     wordSize          = 0;

        /**
         *  �R���X�g���N�^
         */
        public a2BngWord(byte[] word, int checkType)
        {
            int len = word.length;
            ngWord = new byte[len + MARGIN];
            System.arraycopy(word, 0, ngWord, 0, len);
            ngWord[len]  = 0;
//            wordSize     = len;
            checkingType = checkType;
        }

        /**
         *  �R���X�g���N�^
         */
        public a2BngWord(byte[] word, int offset, int len, int checkType)
        {
            ngWord = new byte[len + MARGIN];
            System.arraycopy(word, offset, ngWord, 0, len);
            ngWord[len] = 0;
//            wordSize = len;
            checkingType = checkType;
        }

        /**
         *  NG���[�h����������
         */
        public byte[] getWord()
        {
            return (ngWord);
        }
    }
}

//--------------------------------------------------------------------------
//  a2BNgWordManager  --- ���[�J�����ځ[��f�[�^�Ǘ�
//
//
//--------------------------------------------------------------------------
//  MRSa (mrsa@sourceforge.jp)
