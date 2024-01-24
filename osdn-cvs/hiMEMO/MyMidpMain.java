import jp.sourceforge.nnsi.a2b.framework.base.MidpMain;

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
        super(new HiMemoFactory());
    }
}
