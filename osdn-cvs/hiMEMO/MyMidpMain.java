import jp.sourceforge.nnsi.a2b.framework.base.MidpMain;

/**
 *  MIDP�̃��C���N���X
 *  �i�I�u�W�F�N�g�̐����̂݁j
 * 
 * @author MRSa
 *
 */
public class MyMidpMain extends MidpMain
{
    /**
     *  �R���X�g���N�^
     *
     */
    public MyMidpMain()
    {
        // �X�[�p�[�N���X�̃R���X�g���N�^�Ăяo��
        super(new HiMemoFactory());
    }
}