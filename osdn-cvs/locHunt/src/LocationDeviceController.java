import com.jvlite.device.Location;

/**
 *  位置情報デバイスの制御クラス
 * @author MRSa
 *
 */
public class LocationDeviceController
{
    private Location locationDevice = null;
    
    
    /**
     *  コンストラクタ
     *
     */
    public LocationDeviceController()
    {
        // 何もしない。。。
    }

    /**
     *  位置情報取得デバイスが利用可能かチェックする
     * @return  true(利用可能) / false(利用不可)
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
            // 何もしない
        }
        System.gc();
        return (false);
    }
    
    /**
     *  位置情報取得デバイスの準備
     * @return
     */
    public boolean prepare()
    {
        try
        {
            locationDevice = Location.deviceActive();
            if (locationDevice != null)
            {
                // 位置情報取得デバイスの取得成功
                return (true);
            }
        }
        catch (Exception ex)
        {
            // 何もしない
        }
        locationDevice = null;
        return (false);        
    }

    /**
     *  位置情報の取得
     * @return  nullの場合、位置情報取得失敗、それ以外、位置情報オブジェクト
     */
    public CurrentLocation getCurrentLocation()
    {
        try
        {
            int ret = locationDevice.infoRenew();
            
            // ちょっと2回読んでみると違うかな... 圏外対策...
            ret = locationDevice.infoRenew();
            if (ret < 0)
            {
                // データ更新に失敗...null応答する
                return (null);
            }
            ret = locationDevice.getStatus();
            if (ret <= 0)
            {
            	// 情報なし or 位置情報受信デバイスの状態が変
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
            // 何もしない...
        }
        return (null);
    }
    
    /**
     *   位置情報オブジェクト
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
         *  コンストラクタ
         * @param loc  位置情報取得デバイス
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
         *  経度情報を取得する(double型で)
         * @return
         */
        public double getLatitudeDouble(boolean isWorldType)
        {
            try
            {
            	double answer = location.convertDegree(latitude);
            	if (isWorldType == true)
            	{
                	// 東京測地系 ⇒ 世界測地系へ変換
            		double longitudeData = location.convertDegree(longitude);
                    answer = answer - 0.00010695 * answer + 0.000017464 * longitudeData + 0.0046017;
            	}
                return (answer);
            }
            catch (Exception ex)
            {
                // 変換エラー
            }
            return (-1.0);
        }

        /**
         *  経度情報を秒単位で取得する (double型で)
         *  TODO:  ※※ 本来は世界測地系に変換しなければならない ※※
         *  (現状は東京測地系でのみ応答する)
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
         *  緯度情報を秒単位で取得する (double型で)
         *  TODO : ※※ 本来は世界測地系に変換しなければならない ※※
         *  (現状は、東京測地系でのみ応答する)
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
         *  緯度情報を取得する(double型で)
         * @return
         */
        public double getLongitudeDouble(boolean isWorldType)
        {
            try
            {
            	double answer = location.convertDegree(longitude);
            	if (isWorldType == true)
            	{
            		// 東京測地系⇒世界測地系へ変換
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
         *   緯度情報を応答する
         * @return
         */
        public String getLatitude()
        {
            return (latitude);
        }
        
        /**
         *   経度情報を応答する
         */
        public String getLongitude()
        {
            return (longitude);
        }

        /**
         *   郵便番号を応答する
         * @return
         */
        public String getZipCode()
        {
            return (zipCode);
        }
    }
}
