import com.jvlite.device.Location;

/**
 *  �ʒu���f�o�C�X�̐���N���X
 * @author MRSa
 *
 */
public class LocationDeviceController
{
    private Location locationDevice = null;
    
    
    /**
     *  �R���X�g���N�^
     *
     */
    public LocationDeviceController()
    {
        // �������Ȃ��B�B�B
    }

    /**
     *  �ʒu���擾�f�o�C�X�����p�\���`�F�b�N����
     * @return  true(���p�\) / false(���p�s��)
     */
    static public boolean isAvailableLocationDevice()
    {
        try
        {
            Location device = Location.deviceActive();
            if (device != null)
            {
                device.deviceDeactive();
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
     *  �ʒu���擾�f�o�C�X�̏���
     * @return
     */
    public boolean prepare()
    {
        try
        {
            locationDevice = Location.deviceActive();
            if (locationDevice != null)
            {
                // �ʒu���擾�f�o�C�X�̎擾����
                return (true);
            }
        }
        catch (Exception ex)
        {
            // �������Ȃ�
        }
        locationDevice = null;
        return (false);        
    }

    /**
     *  �ʒu���̎擾
     * @return  null�̏ꍇ�A�ʒu���擾���s�A����ȊO�A�ʒu���I�u�W�F�N�g
     */
    public CurrentLocation getCurrentLocation()
    {
        try
        {
            int ret = locationDevice.infoRenew();
            
            // �������2��ǂ�ł݂�ƈႤ����... ���O�΍�...
            ret = locationDevice.infoRenew();
            if (ret < 0)
            {
                // �f�[�^�X�V�Ɏ��s...null��������
                return (null);
            }
            ret = locationDevice.getStatus();
            if (ret <= 0)
            {
            	// ���Ȃ� or �ʒu����M�f�o�C�X�̏�Ԃ���
            	return (null);
            }
            String latitude  = locationDevice.getLatitude();
            String longitude = locationDevice.getLongitude();
            String zipCode   = locationDevice.getZipCode(Location.ALL);
            CurrentLocation location = new CurrentLocation(locationDevice, latitude, longitude, zipCode);
            return (location);
        }
        catch (Exception ex)
        {
            // �������Ȃ�...
        }
        return (null);
    }
    
    /**
     *   �ʒu���I�u�W�F�N�g
     * @author MRSa
     *
     */
    public class CurrentLocation
    {
        Location location = null;
        String latitude   = null;
        String longitude  = null;
        String zipCode    = null;
        
        /**
         *  �R���X�g���N�^
         * @param loc  �ʒu���擾�f�o�C�X
         * @param lat  latitude
         * @param lon  longitude
         * @param zip  zipCode
         */
        protected CurrentLocation(Location loc, String lat, String lon, String zip)
        {
            location  = loc;
            latitude  = lat;
            longitude = lon;
            zipCode   = zip;
        }

        /**
         *  �o�x�����擾����(double�^��)
         * @return
         */
        public double getLatitudeDouble(boolean isWorldType)
        {
            try
            {
            	double answer = location.convertDegree(latitude);
            	if (isWorldType == true)
            	{
                	// �������n�n �� ���E���n�n�֕ϊ�
            		double longitudeData = location.convertDegree(longitude);
                    answer = answer - 0.00010695 * answer + 0.000017464 * longitudeData + 0.0046017;
            	}
                return (answer);
            }
            catch (Exception ex)
            {
                // �ϊ��G���[
            }
            return (-1.0);
        }

        /**
         *  �o�x����b�P�ʂŎ擾���� (double�^��)
         *  TODO:  ���� �{���͐��E���n�n�ɕϊ����Ȃ���΂Ȃ�Ȃ� ����
         *  (����͓������n�n�ł̂݉�������)
         * @return
         */        
        public double getLatitudeSeconds(boolean isWorld)
        {
        	double answer = 0.0;
        	String target = latitude;
        	String degree = target;
        	int index = degree.indexOf(',');
        	if (index > 0)
        	{
//        		answer = Double.parseDouble(degree.substring(0, (index - 1))) * 3600.0;
        		answer = Double.parseDouble(degree.substring(0, (index))) * 3600.0;
        	}
        	degree = target;
        	int index1 = degree.indexOf(',', (index + 1));
        	if ((index1 > 0)&&(index1 > index))
        	{
//        		answer = answer + Double.parseDouble(degree.substring((index + 1), (index1 - 1))) * 60.0;
        		answer = answer + Double.parseDouble(degree.substring((index + 1), (index1))) * 60.0;
                degree = target;
                answer = answer + Double.parseDouble(degree.substring((index1 + 1)));
        	}
            return (answer);
        }

        
        /**
         *  �ܓx����b�P�ʂŎ擾���� (double�^��)
         *  TODO : ���� �{���͐��E���n�n�ɕϊ����Ȃ���΂Ȃ�Ȃ� ����
         *  (����́A�������n�n�ł̂݉�������)
         * @return
         */        
        public double getLongitudeSeconds(boolean isWorld)
        {
        	double answer = 0.0;
        	String target = longitude;
        	String degree = target;
        	int index = degree.indexOf(',');
        	if (index > 0)
        	{
//        		answer = Double.parseDouble(degree.substring(0, (index - 1))) * 3600.0;
        		answer = Double.parseDouble(degree.substring(0, (index))) * 3600.0;
        	}
        	degree = target;
        	int index1 = degree.indexOf(',', (index + 1));
        	if ((index1 > 0)&&(index1 > index))
        	{
//        		answer = answer + Double.parseDouble(degree.substring((index + 1), (index1 - 1))) * 60.0;
        		answer = answer + Double.parseDouble(degree.substring((index + 1), (index1))) * 60.0;
                degree = target;
                answer = answer + Double.parseDouble(degree.substring((index1 + 1)));
        	}
            return (answer);
        }        
        
        /**
         *  �ܓx�����擾����(double�^��)
         * @return
         */
        public double getLongitudeDouble(boolean isWorldType)
        {
            try
            {
            	double answer = location.convertDegree(longitude);
            	if (isWorldType == true)
            	{
            		// �������n�n�ː��E���n�n�֕ϊ�
            		double latitudeData = location.convertDegree(latitude);
            		answer = answer - 0.000046038 * latitudeData - 0.000083043 * answer + 0.010040;
            	}
                return (answer);
            }
            catch (Exception ex)
            {
                
            }
            return (-1.0);
        }

        /**
         *   �ܓx������������
         * @return
         */
        public String getLatitude()
        {
            return (latitude);
        }
        
        /**
         *   �o�x������������
         */
        public String getLongitude()
        {
            return (longitude);
        }

        /**
         *   �X�֔ԍ�����������
         * @return
         */
        public String getZipCode()
        {
            return (zipCode);
        }
    }
}
