package jp.sourceforge.nnsi.a2b.utilities;

import java.util.Date;


/**
 * ���t�֘A�̃��[�e�B���e�B�N���X�ł��B
 * @author MRSa
 *
 */
public class MidpDateUtils
{
    /**
     *   �R���X�g���N�^�ł́A�����������s���܂���
     */
	public MidpDateUtils()
    {
        // �������Ȃ�
    }

	/**
     *   ���݂̓��t�𐔒l�ɕϊ�����
     * 
     * @param relativeDay ���Γ��t (���t���}����l)
     * @return ���t(���l)
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
     *   ���݂̎����𐔒l�i���j�ɂ���
     * 
     * @param startHour �擪�̎��ԁi�ߑO�܎��̏ꍇ�ɂ́A5��^����j
     * @return ���݂̎���
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
     *  ���̕�����𐔎��֕ϊ�����
     * @param monthStr ��������������
     * @return ��(���l)
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
     *  ���݂̓��t�i�N�����j�𕶎���ɕϊ�����
     * @param relativeDay (���������)���Γ�
     * @return ������
     */
    public String getDateDataString(int relativeDay)
    {
        int date = getDateNumber(relativeDay);
        int day =  date % 100;
        int month = ((date) / 100) % 100;
        int year  = ((date) / 10000);

        return (year + "�N " + month + "�� " + day + "��");
    }
}
