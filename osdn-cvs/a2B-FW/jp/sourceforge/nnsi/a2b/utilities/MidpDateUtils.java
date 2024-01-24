package jp.sourceforge.nnsi.a2b.utilities;

import java.util.Date;


/**
 * 日付関連のユーティリティクラスです。
 * @author MRSa
 *
 */
public class MidpDateUtils
{
    /**
     *   コンストラクタでは、何も処理を行いません
     */
	public MidpDateUtils()
    {
        // 何もしない
    }

	/**
     *   現在の日付を数値に変換する
     * 
     * @param relativeDay 相対日付 (日付を±する値)
     * @return 日付(数値)
     */
    public int getDateNumber(int relativeDay)
    {
        long millisecs = System.currentTimeMillis() + (relativeDay * 24 * 60 * 60 * 1000);
        Date   toGetDate = new Date(millisecs);
        String dateTime = toGetDate.toString();

        String yearStr = dateTime.substring(dateTime.lastIndexOf(' ') + 1);
        int monthTop = dateTime.indexOf(" ");
        int dateTop = dateTime.indexOf(" ", monthTop + 1);
        int timeTop = dateTime.indexOf(" ", dateTop + 1);
        
        String monthStr = dateTime.substring(monthTop + 1, dateTop);
        String dayStr   = dateTime.substring(dateTop + 1, timeTop);

        int year = (Integer.valueOf(yearStr)).intValue();
        int day = (Integer.valueOf(dayStr)).intValue();

        return (year * 10000 + convertMonthStrToInt(monthStr) * 100 + day);
    }

    /**
     *   現在の時刻を数値（分）にする
     * 
     * @param startHour 先頭の時間（午前五時の場合には、5を与える）
     * @return 現在の時刻
     */
    public int getCurrentTime(int startHour)
    {
        long millisecs = System.currentTimeMillis();
        Date   toGetDate = new Date(millisecs);
        String dateTime = toGetDate.toString();

        int monthTop = dateTime.indexOf(" ");
        int dateTop = dateTime.indexOf(" ", monthTop + 1);
        int timeTop = dateTime.indexOf(" ", dateTop + 1);
        int minTop = dateTime.indexOf(":", timeTop + 1);
        int minBottom = dateTime.indexOf(":", minTop + 1);

        String hourStr = dateTime.substring(timeTop + 1, minTop);
        String minStr = dateTime.substring(minTop + 1, minBottom);
        
        int hour = (Integer.valueOf(hourStr)).intValue();
        int min  = (Integer.valueOf(minStr)).intValue();
        
        if (startHour < 0)
        {
        	startHour = 0;
        }
        if (hour < startHour)
        {
            hour = startHour;
        }
        return ((hour - startHour) * 60 + min);
    }

    /**
     *  月の文字列を数字へ変換する
     * @param monthStr 月を示す文字列
     * @return 月(数値)
     */
    public int convertMonthStrToInt(String monthStr)
    {
        int month = 0;
        if (monthStr.indexOf("Jan") >= 0)
        {
            month = 1;
        }
        else if (monthStr.indexOf("Feb") >= 0)
        {
            month = 2;
        }
        else if (monthStr.indexOf("Mar") >= 0)
        {
            month = 3;
        }
        else if (monthStr.indexOf("Apr") >= 0)
        {
            month = 4;
        }
        else if (monthStr.indexOf("May") >= 0)
        {
            month = 5;
        }
        else if (monthStr.indexOf("Jun") >= 0)
        {
            month = 6;
        }
        else if (monthStr.indexOf("Jul") >= 0)
        {
            month = 7;
        }
        else if (monthStr.indexOf("Aug") >= 0)
        {
            month = 8;
        }
        else if (monthStr.indexOf("Sep") >= 0)
        {
            month = 9;
        }
        else if (monthStr.indexOf("Oct") >= 0)
        {
            month = 10;
        }
        else if (monthStr.indexOf("Nov") >= 0)
        {
            month = 11;
        }
        else if (monthStr.indexOf("Dec") >= 0)
        {
            month = 12;
        }
        return (month);
    }    

    /**
     *  現在の日付（年月日）を文字列に変換する
     * @param relativeDay (今日からの)相対日
     * @return 文字列
     */
    public String getDateDataString(int relativeDay)
    {
        int date = getDateNumber(relativeDay);
        int day =  date % 100;
        int month = ((date) / 100) % 100;
        int year  = ((date) / 10000);

        return (year + "年 " + month + "月 " + day + "日");
    }
}
