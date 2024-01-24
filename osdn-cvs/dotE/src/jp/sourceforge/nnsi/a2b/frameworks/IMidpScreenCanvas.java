package jp.sourceforge.nnsi.a2b.frameworks;
import javax.microedition.lcdui.Font;
import javax.microedition.lcdui.Graphics;

/**
 * キャンバス用のインタフェースクラス(管理用)
 * 
 * @author MRSa
 */
public interface IMidpScreenCanvas
{

    /**
     *  クラスの準備処理 
     *
     *  @param font 表示するフォントサイズ
     */
    public abstract void prepare(Font font);                                    // 準備メソッド

    /**
     *  描画範囲を設定する
     *
     *  @param topX   左上X座標
     *  @param topY   左上Y座標
     *  @param width  描画幅
     *  @param height 描画高さ
     */
    public abstract void setRegion(int topX, int topY, int width, int height);  // 描画範囲の設定メソッド

    /**
     *  自分の画面ID
     * 
     *  @return 画面ID
     */
    public abstract int  getCanvasId();      // 自分のID

    // 画面更新
    /**
     *  画面(描画)の開始処理
     */
    public abstract void start();            // 画面(描画)の開始処理

    /**
     *  画面(描画)の停止処理
     *
     */
    public abstract void stop();             // 画面(描画)の停止処理

    /**
     * 画面ペイント処理
     * @param g グラフィッククラス
     */
    public abstract void paint(Graphics g);

    // 表示領域
    /**
     *  左上端X座標を応答する
     *  
     *  @return 左上端X座標
     */
    public abstract int  getTopX();          // 左上端X

    /**
     *  左上端Y座標を応答する
     *  
     *  @return 左上端Y座標
     */
    public abstract int  getTopY();          // 左上端Y

    /**
     *  画面描画幅を応答する
     *  
     *  @return 画面描画幅
     */
    public abstract int  getWidth();         // 幅

    /**
     *  画面描画高さを応答する
     *  
     *  @return 画面描画高さ
     */
    public abstract int  getHeight();        // 高さ

    // (動作用)情報表示
    /**
     *  タイトルを設定する
     *  
     *  @param title 表示タイトル
     */
    public abstract void setTitle(String title);                      // タイトル

    /**
     *  情報表示メッセージを設定する
     * 
     * @param message 情報表示メッセージ
     */
    public abstract void setInformation(String message);              // 情報表示

    /**
     *  情報更新メソッド
     * 
     * @param message メッセージ
     * @param mode モード
     */
    public abstract void updateInformation(String message, int mode); // 情報更新

    /**
     *  メニューボタンが押されたときの処理
     */
    public abstract void showMenu();

    /**
     *  終了ボタンが押されたときの処理
     * @return 終了する(true) / 終了しない(false)
     */
    public abstract boolean checkExit();
    
    /**
     * 更新処理の指示
     * @param processingId 処理ID
     */
    public abstract void updateData(int processingId);

    /**
     * 更新処理の終了通知
     * @param processingId 処理ID
     * @param result 処理結果
     */
    public abstract void finishUpdateData(int processingId, int result);
    
    // キーの入力処理

    /**
     *  上キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputUp   (boolean isRepeat);

    /**
     *  下キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputDown (boolean isRepeat);

    /**
     *  左キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputLeft (boolean isRepeat);

    /**
     *  右キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputRight(boolean isRepeat);

    /**
     *  センターキーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputFire (boolean isRepeat);

    // GAMEキー

    /**
     *  GAME Aキーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputGameA(boolean isRepeat);

    /**
     *  GAME Bキーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputGameB(boolean isRepeat);

    /**
     *  GAME Cキーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputGameC(boolean isRepeat);

    /**
     *  GAME Dキーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputGameD(boolean isRepeat);

    /**
     *  ＃キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputPound(boolean isRepeat);

    /**
     *  ＊キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputStar(boolean isRepeat);

    /**
     *  数字キーの入力処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputNumber(int number, boolean isRepeat);

    /**
     *  その他のキー入力の処理
     *  @param isRepeat キーリピートしているか（true:キーリピートしている、false:通常入力）
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean inputOther(int keyCode, boolean isRepeat);

    /**
     *  画面タッチ入力処理
     *  @param x x座標
     *  @param y y座標
     *  @return このメソッドで処理を実行した(true) / 実行していない(false)
     */
    public abstract boolean pointerPressed(int x, int y);
}
