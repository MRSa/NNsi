import jp.sourceforge.nnsi.a2b.frameworks.MidpMain;

/**
 *  MIDPのメインクラス
 *  （オブジェクトの生成のみ）
 * 
 * @author MRSa
 *
 */
public class MyMidpMain extends MidpMain
{
    /**
     *  コンストラクタ
     *
     */
    public MyMidpMain()
    {
        // スーパークラスのコンストラクタ呼び出し
        super(new TheCountFactory());
    }
}
