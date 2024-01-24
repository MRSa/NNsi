import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * �L�����o�X�̃y�C���g (�C���^�t�F�[�X�N���X)
 * @author MRSa
 */
public interface IkaniFileCanvasPaint
{
	public abstract void prepare(Font font, int mode);  // �������\�b�h
	public abstract void setRegion(int topX, int topY, int width, int height);  // �`��͈͂̐ݒ胁�\�b�h
	public abstract void changeScene();      // ��ʐ؂�ւ����\�b�h...
	public abstract void updateData();       // �f�[�^�X�V���\�b�h
	public abstract void paint(Graphics g);  // �y�C���g���\�b�h

	public abstract int  getTopX();          // ����[X
	public abstract int  getTopY();          // ����[X
	public abstract int  getWidth();         // ��
	public abstract int  getHeight();        // ����

	public abstract void setInformation(String message);    // ���\��
	public abstract void updateInformation(String message, int mode); // ���X�V
	public abstract int  getMode();                         // �V�[��

	// �L�[���̓��\�b�h
	public abstract boolean inputBack();
	public abstract boolean inputForward();
	public abstract boolean inputPreviousItem();
	public abstract boolean inputNextItem();
	public abstract boolean inputPreviousPage();
	public abstract boolean inputNextPage();
	public abstract boolean inputDummy();
	public abstract boolean inputSelected(boolean isRepeat);
	public abstract boolean inputCancel(boolean isRepeat);
	public abstract boolean inputOther(boolean isRepeat);
	public abstract boolean inputTop(boolean isRepeat);
	public abstract boolean inputBottom(boolean isRepeat);
	public abstract boolean inputNumber(int number);
	
}
