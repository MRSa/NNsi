/**
 *  画像表示用の画面パッケージです。
 */
package jp.sourceforge.nnsi.a2b.screens.images;
import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;
import javax.microedition.lcdui.game.Sprite;

import jp.sourceforge.nnsi.a2b.frameworks.MidpSceneSelector;
import jp.sourceforge.nnsi.a2b.screens.MidpScreenCanvas;

/**
 *  画像表示用の画面です。
 *
 * @author MRSa
 *
 */
public class PictureScreen extends MidpScreenCanvas
{
    private PictureScreenOperator  operation = null;

    private final int backColor = 0x00ffffff;
    private final int foreColor = 0x00000000;

    private Image imageObject  = null;

    private boolean busyMode     = false;
    private boolean useThumbnail = true;

    private int   offsetX      = 0;     // 表示開始位置（Ｘ座標）
    private int   offsetY      = 0;     // 表示開始位置（Ｙ座標）

    /**
     * コンストラクタでは、必要なオブジェクトを記憶します。
     * @param screenId 画面ID
     * @param object シーンセレクタ
     * @param operator 描画用データクラス
     */
    public PictureScreen(int screenId, MidpSceneSelector object, PictureScreenOperator operator)
    {
        super(screenId, object);
        operation = operator;
    }

    /**
     *  画面の表示を開始します。
     * 
     */
    public void start()
    {
        showPicture(operation.getMessage());
    }

    /**
     *  画像のデータをメモリに展開し、表示します。
     *  @param message タイトルに表示するメッセージ
     * 
     */
    private void showPicture(String message)
    {
        int pos = titleString.indexOf(":");
        if (pos >= 0)
        {
            message = titleString.substring(pos + 1);
        }
        titleString = " 画像展開中... " + message;
        imageObject = null;
        System.gc();
        try
        {
            Image img = operation.getImage(useThumbnail);
            if (img == null)
            {
                // 画像読み込み失敗...
                titleString = "<<< OUT OF MEMORY? >>>";
                System.gc();
                return;
            }

            int width  = screenWidth;
            int height = screenHeight - (screenFont.getHeight() * 1);
            if (img.getHeight() < (height + offsetY))
            {
                offsetY = img.getHeight() - height - 1;
                if (offsetY < 0)
                {
                    offsetY = 0;
                }
                if ((offsetY + height) > img.getHeight())
                {
                    height = img.getHeight() - offsetY;
                }
            }
            if (img.getWidth() < (width + offsetX))
            {
                offsetX = img.getWidth() - width - 1;
                if (offsetX < 0)
                {
                    offsetX = 0;
                }
                if ((offsetX + width) > img.getWidth())
                {
                    width = img.getWidth() - offsetX;
                }
            }
            imageObject = Image.createImage(img, offsetX, offsetY, width, height, Sprite.TRANS_NONE);
            titleString = img.getWidth() + "x" + img.getHeight() + "(" + offsetX + "," + offsetY + ")";
            if (useThumbnail == true)
            {
                titleString = titleString + "T";
            }
            else
            {
                titleString = titleString + "N";
            }
            img = null;
            titleString = titleString + ":" + message;
        }
        catch (OutOfMemoryError e)
        {
            imageObject = null;
            titleString = " ERROR:: Out of Memory...";
        }
        catch (Exception e)
        {
            imageObject = null;
            titleString = " ERROR >> " + e.getMessage();
        }
        System.gc();
        return;
    }

    /**
     * データ表示を終了します
     * 
     */
    public void stop()
    {
        exitScreen();
        return;
    }
    
    /**
     * 画面描画処理
     * 
     * @param g グラフィックスクラス
     * 
     */
    public void paint(Graphics g)
    {
        try
        {
            // タイトル領域の描画
            drawTitleArea(g);

            if (busyMode != true)
            {
                // メインパネルの描画...
                drawMainPanel(g, screenTopX, screenFont.getHeight() + screenTopY, screenWidth, (screenHeight - screenFont.getHeight()));
            }
        }
        catch (Exception e)
        {
            // なぜかヌルポが出るので...
        }
        return;
    }

    /**
     * 画面を抜ける...
     *
     */
    private void exitScreen()
    {
        imageObject = null;
        operation.cancelPictureView();
        offsetX = 0;
        offsetY = 0;
        finishCanvas();
        return;
    }

    /**
     * 情報領域に情報を設定する
     * @param message 設定する情報
     */
    public void setInformation(String message)
    {
        titleString = message;
        busyMode = false;
        return;
    }    

    /**
     * 情報領域の情報を更新します
     * 　@param message 情報領域に表示する文字列
     *   @param mode 負の値の場合には、画像を表示しません
     */
    public void updateInformation(String message, int mode)
    {
        titleString = message;
        if (mode < 0)
        {
            busyMode = true;
        }
        else
        {
            busyMode = false;
        }
        return;        
    }

    /**
     * メインパネル領域に描画する
     * 
     * @param g    グラフィックキャンバス
     * @param topX 左上座標X
     * @param topY 左上座標Y
     * @param width 描画領域幅
     * @param height 描画領域高さ
     */
    private void drawMainPanel(Graphics g, int topX, int topY, int width, int height)
    {
        // フォントサイズ
        int fontHeight = screenFont.getHeight();

        // 枠を表示する
        g.setColor(backColor);
        g.fillRect(topX, topY, width, height);

        // イメージの表示座標を決める
        int x = topX;
        if (imageObject.getWidth() < width)
        {
            x = topX + ((width  - imageObject.getWidth())  / 2);
        }

        int y = topY;
        if (imageObject.getHeight() < height)
        {
            y = y + ((height - imageObject.getHeight()) / 2);
        }

        // イメージを描画する
        g.setColor(foreColor);
        if (imageObject != null)
        {
            g.drawImage(imageObject, x, y, (Graphics.LEFT | Graphics.TOP));
        }
        else
        {
            // イメージがないので ????? を表示する
            g.drawString("?????", (topX + screenFont.stringWidth("□")), (topY + fontHeight * screenLine / 2), (Graphics.LEFT | Graphics.TOP));
        }
        return;
    }


    /**
     *   画面の終了（次の画面へ切り替える）
     * 
     */
    protected void finishCanvas()
    {
        int nextScene = operation.nextSceneToChange(canvasId);
        if (nextScene < 0)
        {
            parent.previousScene();
        }
        else
        {
            parent.changeScene(nextScene, null);
        }
        return;
    }
}
