import com.jvlite.device.Camera;
import javax.microedition.lcdui.Image;
import java.io.InputStream;

/**
 *   �J�����f�o�C�X�̐���N���X
 * 
 * @author MRSa
 *
 */
public class CameraDeviceController
{
    private Camera  cameraInstance = null;
    private int     brightness = Camera.BRIGHTNESS_DEFAULT;  // �P�x (�f�t�H���g�ŌŒ�)
    private int     continuous = Camera.CONTINUOUS_DEFAULT;  // �A�˃��[�h (�f�t�H���g�ŌŒ�)
    private int     effect     = Camera.EFFECT_DEFAULT;      // �s�N�`���G�t�F�N�g
    private int     flash      = Camera.FLASH_DEFAULT;       // �t���b�V�����[�h
    private int     mode       = Camera.MODE_DEFAULT;        // �B�e���[�h
    private int     pict       = Camera.PICT_DEFAULT;        // �s�N�g�\��
    private int     quality    = Camera.QUALITY_DEFAULT;     // �B�e�掿 (�f�t�H���g�ŌŒ�)
    private int     size       = Camera.SIZE_DEFAULT;        // �B�e�T�C�Y
    private int     sound      = Camera.SOUND_DEFAULT;       // �V���b�^�[�� (�f�t�H���g�ŌŒ�)
    private int     startMode  = Camera.USER_START;          // �蓮�V���b�^�[���[�h (�Œ�)
    private int     style      = Camera.STYLE_DEFAULT;       // �B�e�X�^�C�� (�f�t�H���g�ŌŒ�) Vert/Horiz
    private int     zoom       = Camera.ZOOM_DEFAULT;        // �Y�[���ݒ� (�Œ�)

    /**  �B�e�T�C�Y **/
    static public final int PICTURESIZE_QQVGA = 0;
    static public final int PICTURESIZE_QVGA  = 1;
    static public final int PICTURESIZE_VGA   = 2;
    static public final int PICTURESIZE_SXGA  = 3;
    
    /**  �B�e���[�h **/
    static public final int PICTUREMODE_AUTO    = 10;
    static public final int PICTUREMODE_INDOOR  = 11;
    static public final int PICTUREMODE_LIGHT   = 12;
    static public final int PICTUREMODE_FINE    = 13;
    static public final int PICTUREMODE_CLOUDY  = 14;
    static public final int PICTUREMODE_NIGHT   = 15;

    /** �s�N�`���G�t�F�N�g **/
    static public final int PICTUREEFFECT_OFF   = 20;
    static public final int PICTUREEFFECT_SEPIA = 21;
    static public final int PICTUREEFFECT_NEGA  = 22;
    static public final int PICTUREEFFECT_MONO  = 23;
    static public final int PICTUREEFFECT_SKIN  = 24;
    
    /** �t���b�V�����[�h **/
    static public final int FLASHMODE_ON        = 30;
    static public final int FLASHMODE_OFF       = 31;
    static public final int FLASHMODE_AUTO      = 32;
    static public final int FLASHMODE_LIGHT     = 33;
    
    /**
     *  �R���X�g���N�^
     */
    public CameraDeviceController()
    {
        //
    }

    /**
     *  �J�����f�o�C�X�����p�\���`�F�b�N����
     * @return  true(���p�\) / false(���p�s��)
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
            // �������Ȃ�
        }
        System.gc();
        return (false);
    }

    /**
     *   �J�����f�o�C�X�̗��p����
     * @return
     */
    public boolean prepare()
    {
        try
        {
            cameraInstance = Camera.getCameraInstance();
            if (cameraInstance != null)
            {
                // �J����������������
                cameraInstance.setDefault();
                return (true);
            }
        }
        catch (Exception ex)
        {
            // �������Ȃ�
        }
        cameraInstance = null;
        return (false);
    }
    
    /**
     *  �J�����̃p�����[�^��ݒ肷��
     * @param isReset �J�����p�����[�^������������(true)
     */
    public void setupCameraParameter(boolean isReset)
    {
        if (cameraInstance == null)
        {
            // prepare() ���Ă΂�āA����������ĂȂ��ꍇ�ɂ́A�������Ȃ�
            return;
        }
        if (isReset == true)
        {
            // �J�����̃p�����[�^������������
            cameraInstance.setDefault();
            return;
        }

        // �J�����̃p�����[�^�l��������ݒ肷��
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
     *   �B�e�T�C�Y��ݒ�
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
     *  �B�e���[�h��ݒ�
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
     *   �s�N�`���G�t�F�N�g��ݒ�
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
     *  �t���b�V�����[�h��ݒ肷��
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
     *  �s�N�g�\�����[�h��ݒ�
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
     *  �J�����p�����[�^�̎��s
     * @return
     */
    public boolean doCapture()
    {
        if (cameraInstance == null)
        {
            // �J�����f�o�C�X���p�ӂ���Ă��Ȃ�...�I������
            return (false);
        }
        
        int ret = 0;
        int num = 0;
        try
        {
            ret = cameraInstance.captureStart();
            if (ret != Camera.CAPTURE_OK)
            {
                // �B�e���s
                return (false);
            }
            num = cameraInstance.getNumberOfImages();
            if (num <= 0)
            {
                // �摜�������Ȃ���...�I������
                return (false);
            }
        }
        catch (Exception ex)
        {
            // ��O����...
            return (false);
        }
        return (true);
    }

    /**
     *  �C���[�W�f�[�^���擾���� (�擾�ł��Ȃ����null)
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
     *  �C���[�W�f�[�^���擾���� (�擾�ł��Ȃ����null)
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
     *  �B�e�ς݃f�[�^�̃T�C�Y����������
     * @return  �f�[�^�T�C�Y
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
     *   �J���������Z�b�g����
     */
    public void reset()
    {
        try
        {
            // �J���������Z�b�g����
            cameraInstance.clearCameraInstance();
        }
        catch (Exception ex)
        {
            // �Ȃɂ����Ȃ�
        }
        return;
    }

}
