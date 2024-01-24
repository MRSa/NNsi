package jp.sourceforge.nnsi.a2b.frameworks;

/**
 *  フォームクラスのベースインタフェースです<br>
 *  （データ入力フォームにアクセスするときに使用します。）
 *   
 * @author MRSa
 *
 */
public interface IMidpForm
{
    /**
     *  画面ID(フォームID)の取得
     *  @return 画面ID
     */
    public abstract int getFormId();

    /**
     *  画面が切替えられたときの処理
     */
    public abstract void start();

    /**
     *  画面タイトルを設定する
     *  @param title　設定する画面タイトル
     */
    public abstract void setTitleString(String title);

    /**
     *  OKボタンを用意するかを設定する
     *  @param isShow 用意する(true) / 用意しない(false) 
     */
    public abstract void setOkButton(boolean isShow);

    /**
     *   Cancelボタンを用意するか？
     *   @param isShow 用意する(true) / 用意しない(false)
     */
    public abstract void setCancelButton(boolean isShow);
}
