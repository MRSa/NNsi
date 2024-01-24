import com.jvlite.device.Camera;
import javax.microedition.lcdui.Image;
import java.io.InputStream;

/**
 *   カメラデバイスの制御クラス
 * 
 * @author MRSa
 *
 */
public class CameraDeviceController
{
    private Camera  cameraInstance = null;
    private int     brightness = Camera.BRIGHTNESS_DEFAULT;  // 輝度 (デフォルトで固定)
    private int     continuous = Camera.CONTINUOUS_DEFAULT;  // 連射モード (デフォルトで固定)
    private int     effect     = Camera.EFFECT_DEFAULT;      // ピクチャエフェクト
    private int     flash      = Camera.FLASH_DEFAULT;       // フラッシュモード
    private int     mode       = Camera.MODE_DEFAULT;        // 撮影モード
    private int     pict       = Camera.PICT_DEFAULT;        // ピクト表示
    private int     quality    = Camera.QUALITY_DEFAULT;     // 撮影画質 (デフォルトで固定)
    private int     size       = Camera.SIZE_DEFAULT;        // 撮影サイズ
    private int     sound      = Camera.SOUND_DEFAULT;       // シャッター音 (デフォルトで固定)
    private int     startMode  = Camera.USER_START;          // 手動シャッターモード (固定)
    private int     style      = Camera.STYLE_DEFAULT;       // 撮影スタイル (デフォルトで固定) Vert/Horiz
    private int     zoom       = Camera.ZOOM_DEFAULT;        // ズーム設定 (固定)

    /**  撮影サイズ **/
    static public final int PICTURESIZE_QQVGA = 0;
    static public final int PICTURESIZE_QVGA  = 1;
    static public final int PICTURESIZE_VGA   = 2;
    static public final int PICTURESIZE_SXGA  = 3;
    
    /**  撮影モード **/
    static public final int PICTUREMODE_AUTO    = 10;
    static public final int PICTUREMODE_INDOOR  = 11;
    static public final int PICTUREMODE_LIGHT   = 12;
    static public final int PICTUREMODE_FINE    = 13;
    static public final int PICTUREMODE_CLOUDY  = 14;
    static public final int PICTUREMODE_NIGHT   = 15;

    /** ピクチャエフェクト **/
    static public final int PICTUREEFFECT_OFF   = 20;
    static public final int PICTUREEFFECT_SEPIA = 21;
    static public final int PICTUREEFFECT_NEGA  = 22;
    static public final int PICTUREEFFECT_MONO  = 23;
    static public final int PICTUREEFFECT_SKIN  = 24;
    
    /** フラッシュモード **/
    static public final int FLASHMODE_ON        = 30;
    static public final int FLASHMODE_OFF       = 31;
    static public final int FLASHMODE_AUTO      = 32;
    static public final int FLASHMODE_LIGHT     = 33;
    
    /**
     *  コンストラクタ
     */
    public CameraDeviceController()
    {
        //
    }

    /**
     *  カメラデバイスが利用可能かチェックする
     * @return  true(利用可能) / false(利用不可)
     */
    static public boolean isAvailableCameraDevice()
    {
        try
        {
            Camera device = Camera.getCameraInstance();
            if (device != null)
            {
                device.clearCameraInstance();
                System.gc();
                return (true);
            }
        }
        catch (Exception ex)
        {
            // 何もしない
        }
        System.gc();
        return (false);
    }

    /**
     *   カメラデバイスの利用準備
     * @return
     */
    public boolean prepare()
    {
        try
        {
            cameraInstance = Camera.getCameraInstance();
            if (cameraInstance != null)
            {
                // カメラを初期化する
                cameraInstance.setDefault();
                return (true);
            }
        }
        catch (Exception ex)
        {
            // 何もしない
        }
        cameraInstance = null;
        return (false);
    }
    
    /**
     *  カメラのパラメータを設定する
     * @param isReset カメラパラメータを初期化する(true)
     */
    public void setupCameraParameter(boolean isReset)
    {
        if (cameraInstance == null)
        {
            // prepare() が呼ばれて、準備がされてない場合には、何もしない
            return;
        }
        if (isReset == true)
        {
            // カメラのパラメータを初期化する
            cameraInstance.setDefault();
            return;
        }

        // カメラのパラメータ値をがっつり設定する
        cameraInstance.setStartMode(startMode);
        cameraInstance.setSize(size);
        cameraInstance.setStyle(style);
        cameraInstance.setMode(mode);
        cameraInstance.setEffect(effect);
        cameraInstance.setContinuous(continuous);
        cameraInstance.setQuality(quality);
        cameraInstance.setSound(sound);
        cameraInstance.setFlash(flash);
        cameraInstance.setPict(pict);
        cameraInstance.setBrightness(brightness);
        cameraInstance.setZoom(zoom);
        return;
    }
        
    /**
     *   撮影サイズを設定
     *
     */
    public void setPictureSize(int pictureSize)
    {
        switch (pictureSize)
        {
          case PICTURESIZE_QQVGA:
            size = Camera.SIZE_QQVGA;
            break;
          case PICTURESIZE_QVGA:
            size = Camera.SIZE_QVGA;
            break;
          case PICTURESIZE_VGA:
            size = Camera.SIZE_VGA;
            break;
          case PICTURESIZE_SXGA:
            size = Camera.SIZE_SXGA;
            break;
          default:
            size = Camera.SIZE_DEFAULT;
            break;
        }
        return;
    }

    /**
     *  撮影モードを設定
     * @param pictureMode
     */
    public void setPictureMode(int pictureMode)
    {
        switch (pictureMode)
        {
          case PICTUREMODE_AUTO:
            mode = Camera.MODE_AUTO;
            break;
          case PICTUREMODE_INDOOR:
            mode = Camera.MODE_INDOOR;
            break;
          case PICTUREMODE_LIGHT:
            mode = Camera.MODE_LIGHT;
            break;
          case PICTUREMODE_FINE:
            mode = Camera.MODE_FINE;
            break;
          case PICTUREMODE_CLOUDY:
            mode = Camera.MODE_CLOUDY;
            break;
          case PICTUREMODE_NIGHT:
            mode = Camera.MODE_NIGHT;
            break;
          default:
            mode = Camera.MODE_DEFAULT;
            break;
        }
        return;
    }

    /**
     *   ピクチャエフェクトを設定
     *
     */
    public void setPictureEffect(int pictureEffect)
    {
        switch (pictureEffect)
        {
          case PICTUREEFFECT_OFF:
            effect = Camera.EFFECT_OFF;
            break;
          case PICTUREEFFECT_SEPIA:
            effect = Camera.EFFECT_SEPIA;
            break;
          case PICTUREEFFECT_NEGA:
            effect = Camera.EFFECT_NEGA;
            break;
          case PICTUREEFFECT_MONO:
            effect = Camera.EFFECT_MONO;
            break;
          case PICTUREEFFECT_SKIN:
            effect = Camera.EFFECT_DEFAULT;
            break;
          default:
            effect = Camera.EFFECT_DEFAULT;
            break;
        }
        return;
    }    

    /**
     *  フラッシュモードを設定する
     * @param flashMode
     */
    public void setFlashMode(int flashMode)
    {
        switch (flashMode)
        {
          case FLASHMODE_ON:
            flash = Camera.FLASH_ON;
            break;
          case FLASHMODE_OFF:
            flash = Camera.FLASH_OFF;
            break;
          case FLASHMODE_AUTO:
            flash = Camera.FLASH_AUTO;
            break;
          case FLASHMODE_LIGHT:
            flash = Camera.FLASH_LIGHT;
            break;
          default:
            flash = Camera.FLASH_DEFAULT;
            break;
        }
        return;
    }

    /**
     *  ピクト表示モードを設定
     *
     */
    public void setPictMode(boolean isDisplay)
    {
        if (isDisplay == true)
        {
            pict = Camera.PICT_ON;
        }
        else
        {
            pict = Camera.PICT_OFF;
        }
        return;
    }
    
    /**
     *  カメラパラメータの実行
     * @return
     */
    public boolean doCapture()
    {
        if (cameraInstance == null)
        {
            // カメラデバイスが用意されていない...終了する
            return (false);
        }
        
        int ret = 0;
        int num = 0;
        try
        {
            ret = cameraInstance.captureStart();
            if (ret != Camera.CAPTURE_OK)
            {
                // 撮影失敗
                return (false);
            }
            num = cameraInstance.getNumberOfImages();
            if (num <= 0)
            {
                // 画像枚数がないぞ...終了する
                return (false);
            }
        }
        catch (Exception ex)
        {
            // 例外発生...
            return (false);
        }
        return (true);
    }

    /**
     *  イメージデータを取得する (取得できなければnull)
     * @return
     */
    public Image getImage()
    {
        try
        {
            return (cameraInstance.getImage(0));
        }
        catch (Exception ex)
        {
            //
        }
        return (null);
    }
    
    /**
     *  イメージデータを取得する (取得できなければnull)
     * @return
     */
    public InputStream getInputStream()
    {
        try
        {
            return (cameraInstance.getInputStream(0));
        }
        catch (Exception ex)
        {
            //
        }
        return (null);        
    }

    /**
     *  撮影済みデータのサイズを応答する
     * @return  データサイズ
     */
    public int getDataLength()
    {
        int length = -1;
        try
        {
            length = cameraInstance.getDataLength(0);
        }
        catch (Exception ex)
        {
            //
            length = -1;
        }
        return (length);
    }
    
    
    /**
     *   カメラをリセットする
     */
    public void reset()
    {
        try
        {
            // カメラをリセットする
            cameraInstance.clearCameraInstance();
        }
        catch (Exception ex)
        {
            // なにもしない
        }
        return;
    }

}
