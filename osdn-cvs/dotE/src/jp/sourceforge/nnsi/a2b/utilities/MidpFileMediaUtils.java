package jp.sourceforge.nnsi.a2b.utilities;
import java.io.InputStream;

import javax.microedition.io.Connector;
import javax.microedition.io.file.FileConnection;
import javax.microedition.lcdui.Image;
import javax.microedition.media.Manager;
import javax.microedition.media.Player;
import javax.microedition.media.control.VolumeControl;

/**
 * メディア制御関連ユーティリティ<br>
 * <ul>
 *   <li>画像の表示(JPEGのサムネール表示)</li>
 *   <li>画像の表示</li>
 *   <li>サウンドの再生・停止</li>
 * </ul>
 * が可能です。<br><br>
 * ただし、<em>実際に動作するかどうかは、端末のサポートや残りメモリ容量に依存します。</em>
 * 
 * @author MRSa
 */
public class MidpFileMediaUtils
{
    private final int TEMP_READ_SIZE = 32768;
    private Player      musicPlayer = null;

    /**
     * コンストラクタでは何も処理をしません
     *
     */
    public MidpFileMediaUtils()
    {
        // なにもしない...
    }

    /**
     *  イメージファイルを読み出し、メモリに展開します<BR>
     *  このとき、JPEGのサムネール情報がある場合には、その情報を利用して<br>
     *  画像データをメモリに展開できるか試します。<BR>
     *  <br>
     *  サムネール画像を使用することで、通常開けない大きさの画像ファイルの
     *  内容も参照できる、かもしれません。
     *  
     * @param fileName ファイル名
     * @return         イメージデータ
     */
    public Image LoadJpegThumbnailImageFromFile(String fileName)
    {
        Image          image      = null;
        FileConnection connection = null;
        InputStream    in         = null;
        byte[]         buffer     = null;
        try
        {
            ///// 一度仮読み、、、サムネール画像があるか探す
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);

            // データを最初から size分読み出す...
            in      = connection.openInputStream();
            int startThumbnailOffset = 0;
            int endThumbnailOffset = 0;
            int dataReadSize = 0;
            int currentReadSize = 0;
            byte[] tempBuffer = new byte[TEMP_READ_SIZE + 8];
            tempBuffer[0] = 0x00;
            while (currentReadSize >= 0)
            {
                int index = 0;
                currentReadSize = in.read(tempBuffer, 1, TEMP_READ_SIZE);
                if (dataReadSize == 0)
                {
                    // 先頭マーカを飛ばす
                    index = index + 3;
                }
                while (index < currentReadSize)
                {
                    if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd8))
                    {
                        startThumbnailOffset = index + dataReadSize;
                    }
                    if ((tempBuffer[index] == (byte) 0xff)&&(tempBuffer[index + 1] == (byte) 0xd9))
                    {
                        endThumbnailOffset = index + dataReadSize;
                        if (startThumbnailOffset != 0)
                        {
                            currentReadSize = -1;
                            break;
                        }
                    }
                    index++;
                }
                if ((currentReadSize >= 0)&&(index == currentReadSize))
                {
                    tempBuffer[0] = tempBuffer[currentReadSize - 1];
                }
                dataReadSize = dataReadSize + currentReadSize;
            }
            // ファイルのクローズ
            in.close();
            connection.close();
            in = null;
            connection = null;
            tempBuffer = null;

            ///// 一度仮読み、、、サムネール画像があるか探す
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);

            // データを最初から size分読み出す...
            in      = connection.openInputStream();
            int bufferSize = endThumbnailOffset - startThumbnailOffset + 10;
            if (bufferSize < 0)
            {
                bufferSize = (int) connection.fileSize();
            }
            if (startThumbnailOffset < 0)
            {
                startThumbnailOffset = 0;
            }
            buffer = new byte[bufferSize + 4];

            // ファイルの先頭まで読み出し...
            if (startThumbnailOffset != 0)
            {
                //in.skip(startThumbnailOffset);

                // in.skip(startThumbnailOffset)の処理があまりにも遅かったので、、、カラ読み処理。                
                int startOffset = startThumbnailOffset - 1;
                while (startOffset > 0)
                {
                    int readSize = 0;
                    if (startOffset > bufferSize)
                    {
                        readSize = in.read(buffer, 0, bufferSize);
                    }
                    else
                    {
                        readSize = in.read(buffer, 0, startOffset);
                    }
                    startOffset = startOffset - readSize;
                }
            }
            
            // データの読み出し処理...
            currentReadSize = in.read(buffer, 0, bufferSize);
            
            // ファイルのクローズ
            in.close();
            connection.close();

            // 壁紙データとしてイメージに変換...
            image = Image.createImage(buffer, 0, bufferSize);
            
            // バッファをクリアする
            buffer = null;

        }
        catch (OutOfMemoryError e)
        {
            // メモリオーバフロー例外発生！！！
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            // 読み込み失敗、データを破棄する
            buffer  = null;
            image   = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (in != null)
                {
                    in.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            // 読み込み失敗、データを破棄する
            buffer  = null;
            image   = null;
        }
        System.gc();
        if (image == null)
        {
            // サムネールの取得に失敗した場合には。。。
            return (LoadImageFromFile(fileName));
        }
        return (image);
    }

    /**
     *  イメージファイルを読み出し、メモリに展開して応答します
     * 
     * @param fileName ファイル名
     * @return         イメージデータ
     */
    public Image LoadImageFromFile(String fileName)
    {
        Image          image      = null;
        FileConnection connection = null;
        try
        {
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);
            InputStream in = connection.openInputStream();
            image = Image.createImage(in);
            in.close();
            connection.close();
        }
        catch (OutOfMemoryError e)
        {
            // メモリオーバフロー例外発生！！！
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            // 読み込み失敗、データを破棄する
            image = null;
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
            // 読み込み失敗、データを破棄する
            image   = null;
        }
        System.gc();
        return (image);
    }

    /**
     *  サウンドを鳴らします。(ただ鳴らすだけです。)
     * 
     * @param name ファイル名
     * @param playType プレイ形式(テキストデータ)
     */
    public void PlaySound(String name, String playType)
    {
        if (name.indexOf("file://") >= 0)
        {
            // ファイルから読み出す
        	PlaySoundFromFile(name, playType);
        }
        else
        {
            // リソースから読み出す
        	PlaySoundFromResource(name, playType);
        }
        return;
    }
    
    /**
     * サウンドファイルを読み出し、サウンドを鳴らします。(ただ鳴らすだけです。)
     * 
     * @param fileName ファイル名
     * @param playType プレイ形式(テキストデータ)
     */
    public void PlaySoundFromFile(String fileName, String playType)
    {
        FileConnection connection = null;
        try
        {
            // ファイルのオープン
            connection  = (FileConnection) Connector.open(fileName, Connector.READ);
            InputStream in = connection.openInputStream();

            // サウンドを流す...
            musicPlayer = null;
            musicPlayer = Manager.createPlayer(in, playType);
            musicPlayer.start();
            in.close();
            connection.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
            try
            {
                if (connection != null)
                {
                    connection.close();
                }
            }
            catch (Exception e2)
            {
                // (例外発生時には何もしない)
            }
        }
        System.gc();
        return;
    }

    /**
     * リソースからサウンドファイルを読み出し、サウンドを鳴らします。(ただ鳴らすだけです。)
     * 
     * @param name リソース(ファイル)名
     * @param playType プレイ形式(テキストデータ)
     */
    public void PlaySoundFromResource(String name, String playType)
    {
        try
        {
            // ファイルのオープン
		    InputStream in = (this.getClass()).getResourceAsStream(name);

            // サウンドを流す...
            musicPlayer = null;
            musicPlayer = Manager.createPlayer(in, playType);
            musicPlayer.start();
            in.close();
        }
        catch (Exception e)
        {
            // 例外発生！！！
        }
        System.gc();
        return;
    }

    /**
     *  サウンド再生の停止を行う
     *
     */
    public void StopSound()
    {
        if (musicPlayer == null)
        {
        	return;
        }
        try
        {
        	musicPlayer.stop();
        	musicPlayer.close();
        }
        catch (Exception ex)
        {
        	// 何もしない
        }
        musicPlayer = null;
        System.gc();
        return;
    }

    /**
     *  ボリュームを切り替える (相対指定)
     * @param upDown 変更するボリュームサイズ（相対指定、現在のボリュームから加減する量を指定する）
     */
    public void SetVolume(int upDown)
    {
    	if (musicPlayer == null)
    	{
    		return;
    	}
    	VolumeControl cnt = (VolumeControl) musicPlayer.getControl("VolumeControl");
    	int level = cnt.getLevel() + upDown;
    	if (level < 0)
    	{
    		level = 0;
    	}
    	else if (level > 100)
    	{
    		level = 100;
    	}
    	cnt.setLevel(level);
    }

    /**
     *  ミュート状態の切替
     * @param releaseMuting trueなら無理やり鳴らす
     */
    public void ToggleMute(boolean releaseMuting)
    {
    	if (musicPlayer == null)
    	{
    		return;
    	}
    	VolumeControl cnt = (VolumeControl) musicPlayer.getControl("VolumeControl");
    	boolean isMuting = cnt.isMuted();
    	if ((isMuting == true)||(releaseMuting == true))
    	{
    		isMuting = false;
    	}
    	else
    	{
    		isMuting = true;
    	}
    	cnt.setMute(isMuting);
    	return;
    }
}
