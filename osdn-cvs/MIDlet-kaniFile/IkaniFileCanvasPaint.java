import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * キャンバスのペイント (インタフェースクラス)
 * @author MRSa
 */
public interface IkaniFileCanvasPaint
{
	public abstract void prepare(Font font, int mode);  // 準備メソッド
	public abstract void setRegion(int topX, int topY, int width, int height);  // 描画範囲の設定メソッド
	public abstract void changeScene();      // 画面切り替えメソッド...
	public abstract void updateData();       // データ更新メソッド
	public abstract void paint(Graphics g);  // ペイントメソッド

	public abstract int  getTopX();          // 左上端X
	public abstract int  getTopY();          // 左上端X
	public abstract int  getWidth();         // 幅
	public abstract int  getHeight();        // 高さ

	public abstract void setInformation(String message);    // 情報表示
	public abstract void updateInformation(String message, int mode); // 情報更新
	public abstract int  getMode();                         // シーン

	// キー入力メソッド
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
