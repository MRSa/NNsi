import java.util.Date;
import java.io.InputStream;
import javax.microedition.lcdui.ChoiceGroup;
import javax.microedition.lcdui.TextField;
import jp.sourceforge.nnsi.a2b.screens.informations.*;
import jp.sourceforge.nnsi.a2b.screens.selections.*;
import jp.sourceforge.nnsi.a2b.frameworks.*;
import jp.sourceforge.nnsi.a2b.forms.*;
import jp.sourceforge.nnsi.a2b.utilities.*;

/**
 * �A�v���P�[�V�����̃f�[�^���i�[����N���X
 * 
 * @author MRSa
 *
 */
public class LocHuntSceneStorage implements ISelectionScreenStorage, IInformationScreenStorage, IDataInputStorage, IFileSelectionStorage
{
    private final int  MaxDataInputLength   = 2048;

    /***** �����f�[�^(��������) *****/
    private TextField    inputDataField     = null;
    private TextField    inputNumberField   = null;
    private String       informationMessage = null;
    private int          itemSelectionIndex = -1;

    private String       memoData           = "";
    private String       memoDataNumeric    = "";
    /***** �����f�[�^(�����܂�) *****/

    /***** ���̑��A�N�Z�X�N���X *****/
    private MidpResourceFileUtils resUtils    = null;
    private MidpFileOperations fileOperation  = null;
//    private MidpKanjiConverter kanjiConverter = null;
    private LocHuntDataStorage storageDb = null;
    
    private boolean   isFirstLocation = false;     // ����̃��P�[�V����...
    private double    startLatitude   = 510.0;
    private double    startLongitude  = 510.0;
    private double    leftLatitude    = 510.0;
    private double    leftLongitude   = 510.0;
    private double    rightLatitude   = 510.0;
    private double    rightLongitude  = 510.0;
    private double    upLatitude      = 510.0;
    private double    upLongitude     = 510.0;
    private double    downLatitude    = 510.0;
    private double    downLongitude   = 510.0;
    
    private boolean   pictureSaveAction = false;
    
    /**
     *  �R���X�g���N�^
     *
     *  @param dataStorage �f�[�^�L�^�N���X
     *  @param resourceUtils ���\�[�X�A�N�Z�X�N���X
     *  @param fileUtils �t�@�C���A�N�Z�X�N���X
     */
    public LocHuntSceneStorage(LocHuntDataStorage dataStorage, MidpResourceFileUtils resourceUtils, MidpFileOperations fileUtils, MidpKanjiConverter knjConv, MidpFileHttpCommunication httpComm, IDataProcessing dataProcess)
    {
        storageDb = dataStorage;
        fileOperation = fileUtils;
//        kanjiConverter = knjConv;
        resUtils = resourceUtils;
    }

    /**------------------------------------------------------------**
     **
     **  ��������ۊǃf�[�^�̓ǂݏ���...
     **
     **
     **------------------------------------------------------------**/
    /**
     * �f�B���N�g���̉���
     * @return �f�B���N�g����
     */
    public String getDefaultDirectory()
    {
        return (storageDb.getBaseDirectory());
    }

    /**
     *  �I������Ă���i�t�@�C���j������������
     *  
     */
    public  int getSelectedNameCount()
    {
        if (storageDb.getBaseDirectory() == null)
        {
            return (0);
        }
        return (1);
    }

    /**
     *  �f�B���N�g���̉���
     * @return �f�B���N�g����
     */
    public String getSelectedName(int index)
    {
        return (storageDb.getBaseDirectory());
    }
    
    /**
     *   �f�B���N�g���̉���
     * @param �f�B���N�g����
     */
    public void setSelectedName(String name)
    {
        storageDb.setBaseDirectory(name);
    }

    /**
     *  �I�𒆂̃f�B���N�g�������N���A����
     * 
     */
    public void clearSelectedName()
    {
        storageDb.setBaseDirectory(null);
        return;
    }
    
    /**
     *  �ݒ肵���f�B���N�g�������m�肷��
     *  (�������Ȃ�)
     * 
     */
    public void confirmSelectedName()
    {
        return;
    }
    
    /**------------------------------------------------------------**
     **
     **  ������������f�[�^�̓ǂݏ���...
     **
     **
     **------------------------------------------------------------**/
    /**
     * �\�����郁�b�Z�[�W����������
     * 
     * @return
     */
    public String getInformationMessageToShow()
    {
        String message = "";
        try
        {
            message = resUtils.getResourceText("welcome.txt");
        }
        catch (Exception ex)
        {
            //
        }
        return (message);
    }

    /**
     *   �{�^��������������
     *   
     */
    public int getNumberOfButtons(int screenId)
    {
        return (1);
    }

    /**
     * ���b�Z�[�W���󂯕t����ꂽ���ǂ����ݒ肷��
     * @param buttonId �����ꂽ�{�^����ID
     */
    public void setButtonId(int buttonId)
    {
        // �{�^���������ꂽ�I�I�i��ʑJ�ځI�j
        
    }

    /**
     *  ���\����ʂ̃{�^�����x��������
     * 
     */
    public String getInformationButtonLabel(int buttonId)
    {
        return("Dir�I��");
    }

    /**
     *  �f�[�^���͂��L�����Z�����ꂽ�Ƃ�
     *  
     */
    public void cancelEntry()
    {
        inputDataField = null;
        inputNumberField = null;
        System.gc();
        return;
    }

    /**
     *  �f�[�^���͂��n�j���ꂽ�Ƃ�
     *  
     */
    public void dataEntry()
    {
        // �����f�[�^
        String ddd  = inputDataField.getString();
        if (ddd.length() > 0)
        {
//            String data = kanjiConverter.UrlEncode(kanjiConverter.ParseToEuc(ddd));
            memoData = ddd;    // ����͊����̈�...(�s�����̈�ɕύX���Ă��ǂ�����)
        }
        else
        {
        	memoData = "";
        }

        // ���������f�[�^
        ddd  = inputNumberField.getString();
        if (ddd.length() > 0)
        {
//          String data = kanjiConverter.UrlEncode(kanjiConverter.ParseToEuc(ddd));
        	memoDataNumeric = ddd;    // ����͊����̈�...(�s�����̈�ɕύX���Ă��ǂ�����)
        }
        else
        {
        	memoDataNumeric = "";
        }

        inputDataField = null;
        inputNumberField = null;
        System.gc();
        return;
    }

    /**
     *  �f�[�^���͂̂��߂̏���
     *  
     */
    public void prepare()
    {
        //  �������Ȃ�
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public int nextSceneToChange(int screenId)
    {
        if (screenId == LocHuntSceneDB.WELCOME_SCREEN)
        {
            return (LocHuntSceneDB.DIR_SCREEN);
        }
        else if (screenId == LocHuntSceneDB.DIR_SCREEN)
        {
            return (LocHuntSceneDB.DEFAULT_SCREEN);
        }
        return (IDataInputStorage.SCENE_TO_PREVIOUS);
    }

    /**
     *  ����ʂ̐؂�ւ�
     * 
     */
    public String nextSceneTitleToChange(int screenId)
    {
        return ("");
    }

    /**
     *  �f�[�^���͏I��
     *  
     */
    public void finish()
    {
        // �������Ȃ�
    }

    /**
     *  �f�[�^���̓t�B�[���h�̌����擾����
     *  
     */
    public int getNumberOfDataInputField()
    {
        return (2);
    }

    /**
     *  �f�[�^���̓t�B�[���h���擾����
     *  
     */
    public TextField getDataInputField(int index)
    {
    	if (index == 0)
    	{
            inputDataField = new TextField("����", memoData, MaxDataInputLength, TextField.ANY);
            return (inputDataField);
    	}
    	else
    	{
    		inputNumberField = new TextField("(���l)", memoDataNumeric, MaxDataInputLength, TextField.NUMERIC);    		
    	}
        return (inputNumberField);
    }

    /**
     *  �I�v�V�����̌����擾����
     *  
     */
    public int getNumberOfOptionGroup()
    {
        return (0);
    }

    /**
     *   �I�v�V�������擾����
     */
    public ChoiceGroup getOptionField(int index)
    {
        return (null);
    }

    /**
     *  �I�����̐����擾����
     *  
     */
    public int numberOfSelections()
    {
        return (0);
    }

    /**
     *  �I�����̕�������擾����
     *  
     */
    public String getSelectionItem(int itemNumber)
    {
        return ("");
    }
    
    /**
     *  �I��p�̃K�C�h��������擾����
     *  
     */
    public String getSelectionGuidance()
    {
        return ("Menu��Dir�m��");
    }

    /**
     *  �^�C�g����\�����邩�ǂ����H
     *  
     */
    public boolean isShowTitle()
    {
        return (true);
    }

    /**
     *   start���A�A�C�e�������������邩�H
     * 
     */
    public boolean isItemInitialize()
    {
        return (true);
    }

    /**
     *  �{�^���̐����擾����
     *  
     */
    public int getNumberOfSelectionButtons()
    {
        return (1);
    }

    /**
     *  �{�^���̃��x�����擾����
     *  
     */
    public String getSelectionButtonLabel(int buttonId)
    {
        return ("DIR�I��");
    }

    /**
     *  �w�i���݂��Ⴂ�̐F�ŕ\�����邩�H
     *  
     */
    public boolean getSelectionBackColorMode()
    {
        return (false);
    }

    
    public void setSelectedItem(int itemId)
    {
        itemSelectionIndex = itemId;
    }

    public int getSelectedItem(int itemNumber)
    {
        if (itemNumber != 0)
        {
            return (-1);
        }
        return (itemSelectionIndex);
    }
    
    public boolean isSelectedItemMulti()
    {
        return (false);
    }

    public int getNumberOfSelectedItems()
    {
        if (itemSelectionIndex < 0)
        {
            return (0);
        }
        return (1);
    }

    // �I�����ꂽ�{�^��ID����������
    public boolean setSelectedCommandButtonId(int buttonId)
    {
        if (buttonId == ISelectionScreenStorage.BUTTON_CANCELED)
        {
            resetSelectionItems();
        }
        return (false);
    }

    // �A�C�e���I�����N���A����
    public void resetSelectionItems()
    {
        itemSelectionIndex = -1;
        return;
    }

    // ���j���[�{�^���������ꂽ
    public boolean triggeredMenu()
    {    
        // ��ʂ�؂�ւ���...
        return (true);
    }

    /**
     *  ��񃁃b�Z�[�W��ݒ肷��
     * 
     * @param message
     */
    public void setInformationMessage(String message)
    {
        informationMessage = message;
    }

    /**
     *  ��񃁃b�Z�[�W����������
     * 
     * @return
     */
    public String getInformationMessage()
    {
        return (informationMessage);
    }    

    /**
     *  �ʒu���f�[�^���o��(�ǋL)����
     *  (���E���n�n�ŁA����CSV�`���Ńf�[�^���o�͂���, Super Mapple Digital�Ŏ捞�ł���悤��...)
     *  
     *   �t�H�[�}�b�g : latitude,longitude,fileName,"memoData1", memoData2
     */
    public boolean outputLocationData(String fileName, LocationDeviceController.CurrentLocation location)
    {
    	boolean ret = false;
        try
        {
        	// ���E���n�n�ŁA����CSV�`���Ńf�[�^���o�͂���
            double latitude =location.getLatitudeDouble(true);
            double longitude = location.getLongitudeDouble(true);

            String data = "" + latitude + "," + longitude + "," + fileName + ",";
            if (memoData.length() > 0)
            {
                // �B�e�������o�͂��� �i���͂���Ă����ꍇ�̂ݏo�́j
                data = data + "\"" + memoData + "\"";
            }
            data = data + ",";
            if (memoDataNumeric.length() > 0)
            {
                // �B�e����(�����f�[�^)���o�͂��� �i���͂���Ă����ꍇ�̂ݏo�́j
                data = data + memoDataNumeric;
            }
            // �ǋL���[�h�Ńe�L�X�g�t�@�C���o�͂��s��
            fileOperation.outputDataTextFile(storageDb.getBaseDirectory() + "locHuntLog.csv", null, null, data, true);
        }
        catch (Exception ex)
        {
        	// 
        	ret = false;
        }
        return (ret);
    }

    /**
     *  �t�����f�[�^���o�͂���
     */
    public boolean outputInformationData(String fileNameHeader, LocationDeviceController.CurrentLocation location, boolean linkImage, int width, int height)
    {
    	boolean ret = false;
    	
        try
        {
            // �摜�t�@�C�����o�͂ł����ꍇ...����ƘA�g����f�[�^�t�@�C�����o�͂���
            Date  currentDate = new Date();
            String data;
            data = "<html><head><title>";
            data = data + fileNameHeader;
            if (linkImage == true)
            {
                data = data + ".jpg";
            }
            else
            {
                data = data + ".html";
            }
            data = data + "</title></head>\r\n<body>";
            if (memoData.length() > 0)
            {
                // �B�e�������o�͂��� �i���͂���Ă����ꍇ�j
                data = data + memoData;
            }
            if (memoDataNumeric.length() > 0)
            {
                // �B�e����(�����f�[�^)���o�͂��� �i���͂���Ă����ꍇ�j
                data = data + "\r\n " + memoDataNumeric;
            }
            data = data + "<hr>\r\n";
            data = data + "�����F<br>" + currentDate.toString();
            data = data + "<br>\r\n";
            if (location != null)
            {
                // �ʒu�����o�͂��� �i�擾�ł��Ă����ꍇ�ꍇ�j
                data = data + "�X�֔ԍ� : " + location.getZipCode() + "<br>\r\n";
                data = data + "�ܓx : " + location.getLatitude() + " " + "�o�x : " + location.getLongitude();
                data = data + "<hr>\r\n";
            }
            // �摜�t�@�C���ւ̃����N
            if (linkImage == true)
            {
                data = data + "<P align=\"center\"><IMG src=\"" + fileNameHeader + ".jpg\"" + " alt =\"" + fileNameHeader + ".jpg\"";
                if ((width > 0)&&(height > 0))
                {
                    // �\������摜�f�[�^�̃T�C�Y���w�肳��Ă����ꍇ�A�ݒ肷��
                    data = data + " height=\"" + height + "\" width=\"" + width + "\"";
                }
                data = data + "></P>";
                data = data + "<hr>\r\n";
            }
            if (location != null)
            {
                // google maps�ւ̃����N (�}�b�v�f�[�^��\������) // Zoom Level 15 ���ǂ�����... (14�ł��ǂ��H)
                double latitude =location.getLatitudeDouble(true);
                double longitude = location.getLongitudeDouble(true);
                data = data + "<a href=\"http://maps.google.com/staticmap?" + "center=" + latitude + "," + longitude +
                              "&maptype=mobile" + "&zoom=15&size=240x240" +
                              "&markers=" + latitude + "," + longitude + ",red" +
                              "&key=ABQIAAAAy2Fi9hJaQOg1ZfoEosnyWxS1zhnEwl123lCDx5CEbHiJbEqVOxTf6Oe45-Kkc7sNyF9fr9_fXAl6_w" +
                              "\">" + "�ʒu�m�F(Google Maps)</a><br>\r\n";
                data = data + "<a href=\"http://mobile.yuubinbangou.net/search.php?kw=" + location.getZipCode() + "" +
                              "\">" + "�ꏊ�m�F(���X�֔ԍ�����)</a><br>\r\n";
                /*     // XML�ŉ�������邽�߁A���p�ł��Ȃ�...
                data = data + "<a href=\"http://api.knecht.jp/reverse_geocoding/api/?lat=" + latitude + "&lng=" + longitude +
                              "\">" + "�Z���m�F(api.knecht.jp)</a><br>\r\n";
                */
                data = data + "<a href=\"http://www.chizumaru.com/czm/currentadr.aspx?x=" + location.getLongitudeSeconds(false) + "&y=" + location.getLatitudeSeconds(false) +
                              "\">" + "�Z���m�F(������)</a><br>\r\n";
            }
            // �ulocHunt�֖߂�v�^�O�𖄂ߍ��ށB
            data = data + "<object id=\"app\" declare=\"declare\" classid=\"x-oma-application:java-ams\">";
            data = data + "\r\n" + "  <param name=\"AMS-Filename\" value=\"http://nnsi.sourceforge.jp/archives/midp/locHunt/locHunt.jad\"/>";
            data = data + "\r\n" + "  <param name=\"MIDlet-Name\" value=\""    + resUtils.getAppProperty("MIDlet-Name") + "\"/>";
            data = data + "\r\n" + "  <param name=\"MIDlet-Version\" value=\"" + resUtils.getAppProperty("MIDlet-Version") + "\"/>";
            data = data + "\r\n" + "  <param name=\"MIDlet-Vendor\" value=\""  + resUtils.getAppProperty("MIDlet-Vendor") + "\"/>";
            data = data + "\r\n" + "<param name=\"AMS-Startup\" value=\"download-confirm\"/>";
            data = data + "\r\n" + "</object><a href=\"#app\">(" + resUtils.getAppProperty("MIDlet-Name") + "�֖߂�)</a>";

            // �^�O�̖�����ݒ肷��
            data = data + "\r\n</body></html>";
            fileOperation.outputDataTextFile(storageDb.getBaseDirectory() + fileNameHeader + ".html", "", "", data, false);
        }
        catch (Exception ex)
        {
        	// 
        	ret = false;
        }
        return (ret);
    }
    
    /**
     *   �摜�f�[�^���o�͂���
     * @param fileNameHeader
     * @param data
     * @return
     */
    public boolean outputImageData(String fileNameHeader, InputStream inputStream)
    {
    	boolean ret = false;
        try
        {        	
            if (inputStream == null)
            {
                // �ʒu���̎擾
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();

                // �摜�f�[�^���擾�ł��Ȃ�... �t�����̂ݏo�͂���
                outputLocationData(fileNameHeader + ".html", location);
                ret = outputInformationData(fileNameHeader, location, false, 0, 0);            	
                return (ret);
            }
     
            // �摜�f�[�^�̏o��
            ret = fileOperation.outputRawDataToFile(storageDb.getBaseDirectory() + fileNameHeader + ".jpg", inputStream, false);
//            inputStream.close();    // �K�v�ł���Ύ��s���悤...
            if (ret == true)
            {
                // �ʒu���̎擾 (�ʒu�����擾�ł��Ȃ��Ă��摜�t�@�C�����o�͂���������...)
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();

                // �t�������o�͂���
                int width = 120;     // HTML�ŕ\������摜�f�[�^�̃T�C�Y (��)
                int height = 160;    // HTML�ŕ\������摜�f�[�^�̃T�C�Y (����)
                outputLocationData(fileNameHeader + ".jpg", location);
                outputInformationData(fileNameHeader, location, true, width, height);
            }
        }
        catch (Exception ex)
        {
            //
        }
        return (ret);
    }
    
    
    /**
     *  �ʒu�����N���A����
     *
     */
    public void clearLocation()
    {
        leftLatitude   = 510.0;
        leftLongitude  = 510.0;
        rightLatitude  = 510.0;
        rightLongitude = 510.0;
        upLatitude     = 510.0;
        upLongitude    = 510.0;
        downLatitude   = 510.0;
        downLongitude  = 510.0;
    }
    
    /**
     *  �ʒu�����L�^����
     * @param keyCode
     * @return
     */
    public boolean setLocation(int keyCode)
    {
    	boolean ret = false;
    	try
    	{
            if (keyCode == javax.microedition.lcdui.Canvas.LEFT)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                leftLatitude = location.getLatitudeDouble(true);
                leftLongitude = location.getLongitudeDouble(true);
                ret = true;
            }
            else if (keyCode == javax.microedition.lcdui.Canvas.RIGHT)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                rightLatitude = location.getLatitudeDouble(true);
                rightLongitude = location.getLongitudeDouble(true);
                ret = true;
            }
            else if (keyCode == javax.microedition.lcdui.Canvas.UP)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                upLatitude = location.getLatitudeDouble(true);
                upLongitude = location.getLongitudeDouble(true);
                ret = true;
            }
            else if (keyCode == javax.microedition.lcdui.Canvas.DOWN)
            {
                LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
                downLatitude = location.getLatitudeDouble(true);
                downLongitude = location.getLongitudeDouble(true);
                ret = true;        	
            }
    	}
    	catch (Exception ex)
    	{
    		ret = false;
    	}
    	return (ret);
    }
    
    /**
     *   �����̈ʒu�������ɋL������
     *
     */
    public void setStartLocation(boolean forceMemory)
    {
    	if ((forceMemory == false)&&(isFirstLocation == true))
    	{
    		return;
    	}
    	try
    	{
            // �ʒu���̎擾�ƋL��
            LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
            startLatitude = location.getLatitudeDouble(true);
            startLongitude = location.getLongitudeDouble(true);
            isFirstLocation = true;
    	}
    	catch (Exception ex)
    	{
    		// 
    	}    	
    }
    
    /**
     *  Google Static Map�̂t�q�k����������
     * @param width              �摜�̕�
     * @param height             �摜�̍���
     * @param isCurrentLocation  ���݂̈ʒu���T�|�[�g���邩�H
     * @return
     */
    public String getGoogleMapStaticMapUrl(int width, int height, boolean isCurrentLocation)
    {
    	double latitude = 0.0;
    	double longitude = 0.0;
    	try
    	{
            // �ʒu���̎擾�ƋL��
            LocationDeviceController.CurrentLocation location = storageDb.getCurrentLocation();            
            latitude = location.getLatitudeDouble(true);
            longitude = location.getLongitudeDouble(true);
    	}
    	catch (Exception ex)
    	{
    		// 
    	}
    	if (startLatitude > 500)
    	{
    		startLatitude  = latitude;
    		startLongitude = longitude;
    	}

    	String data = "http://maps.google.com/staticmap?" +
                      "&maptype=mobile" + "&size=" + width + "x" + height + 
                      "&markers=" + startLatitude + "," + startLongitude + ",blueb";
        if (upLatitude < 500)
        {
            data = data + "|" + upLatitude + "," + upLongitude + ",redu";
        }
        if (downLatitude < 500)
        {
            data = data + "|" + downLatitude + "," + downLongitude + ",redd";
        }
        if (leftLatitude < 500)
        {
            data = data + "|" + leftLatitude + "," + leftLongitude + ",redl";
        }
        if (rightLatitude < 500)
        {
            data = data + "|" + rightLatitude + "," + rightLongitude + ",redr";
        }
        if ((isCurrentLocation == true)&&(latitude < 500))
        {
            data = data + "|" + latitude + "," + longitude + ",greene";
        }
        data = data + "&key=ABQIAAAAy2Fi9hJaQOg1ZfoEosnyWxS1zhnEwl123lCDx5CEbHiJbEqVOxTf6Oe45-Kkc7sNyF9fr9_fXAl6_w";
        return (data);
    }    

    /**
     *  �u�f�[�^��ۑ����邩�H�v��ݒ肷��
     * @param data
     */
    public void setSaveAction(boolean data)
    {
    	pictureSaveAction = data;
    }

    /**
     *  �u�f�[�^��ۑ����邩�H�v����������
     * @return
     */
    public boolean getSaveAction()
    {
    	return (pictureSaveAction);
    }
}
