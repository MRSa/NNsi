import jp.sourceforge.nnsi.a2b.frameworks.MidpMain;

/**
 *  MIDP�̃��C���N���X�ł�<br>
 *  �i�I�u�W�F�N�g�̐����݂̂��s���܂��j<br>
 * 
 * @author MRSa
 *
 */
public class fwSampleMain extends MidpMain
{
    /**
     *  �X�[�p�[�N���X�̃R���X�g���N�^���Ăяo���܂�<br>
     *  (�����Ƀt�@�N�g���N���X�𐶐����ēn���܂�)
     *
     */
    public fwSampleMain()
    {
        // �X�[�p�[�N���X�̃R���X�g���N�^�Ăяo��
        super(new fwSampleFactory());
    }
}
