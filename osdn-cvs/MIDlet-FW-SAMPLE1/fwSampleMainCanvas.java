import javax.microedition.lcdui.Graphics;
import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 *  サンプル画面
 * 
 * @author MRSa
 */
public class fwSampleMainCanvas extends MidpScreenCanvas
{
    private String message = "";  // 表示するメッセージを設定する

    /**
     *  コンストラクタ
     *  @param screenId
     *  @param object
     */
    public fwSampleMainCanvas(int screenId, MidpSceneSelector object)
    {
        super(screenId, object); 
    }

    /**
     *  画面が切り替えられたときに呼ばれる処理
     */
    public void start()
    {
        // タイトル文字列を設定する
        titleString = "サンプルアプリ!!";

        // 表示メッセージを設定する
        message = "Hello World!";
        return;
    }

    /**
     *  画面を表示
     *  @param g グラフィッククラス
     */
    public void paint(Graphics g)
    {
        // グラフィック状態の確認
        if (g == null)
        {
            return;
        }

        // タイトルと画面全体のクリアは親（継承元）の機能を使う
        super.paint(g);
        
        g.setColor(0x00000000);  // 黒色

        // 文字列の描画 （１行目はタイトル行のため、その次の行に文字列を表示する）
        g.drawString(message, screenTopX, screenTopY + screenFont.getHeight(), (Graphics.LEFT | Graphics.TOP));

        return;
    }
}
