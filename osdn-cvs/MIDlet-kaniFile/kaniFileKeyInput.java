import javax.microedition.lcdui.Canvas;

/**
 * (Canvasクラスで)キー入力されたときの処理
 * @author MRSa
 */
public class kaniFileKeyInput
{
	private IkaniFileCanvasPaint  disp          = null;

	/**
	 * コンストラクタ
	 * @param object
	 */
	kaniFileKeyInput()
	{
		// 何もしない...
	}

	/**
	 * スクリーンキャンバスを設定...
	 * @param object
	 */
	public void setScreenCanvas(IkaniFileCanvasPaint object)
	{
		disp = object;
		return;
	}
	
	/**
	 * 左カーソルキーが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputLeft(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputPreviousPage());
	}

	/**
	 * 右カーソルキーが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputRight(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputNextPage());
	}

	/**
	 * 上カーソルキーが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputUp(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputPreviousItem());
	}

	/**
	 * 下カーソルキーが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputDown(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputNextItem());
	}

	/**
	 * 中ボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputFire(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputSelected(isRepeat));
	}

	/**
	 * Aボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputGameA(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputBack());
	}

	/**
	 * Bボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputGameB(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputForward());
	}

	/**
	 * Cボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputGameC(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputCancel(isRepeat));
	}

	/**
	 * Dボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputGameD(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputOther(isRepeat));
	}

	/**
	 * #ボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputPound(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputBottom(isRepeat));
	}

	/**
	 * *ボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public boolean inputStar(boolean isRepeat)
	{
		if (disp == null)
		{
			return (false);
		}
		return (disp.inputTop(isRepeat));
	}
	
	/**
	 * その他のデータが入力された
	 * @return
	 */
	public boolean inputDummy()
	{
		disp.inputDummy();
		return (false);
	}
	

	/**
	 * 数字ボタンが入力された
	 * @param isRepeat
	 * @return
	 */
	public int numberInputMode(int keyCode)
	{
        boolean ret = false;
		int number = -10;
		
		// 通常モード時のキー入力処理...
		switch (keyCode)
		{
          case Canvas.KEY_NUM0:
        	number = 0;
			break;

		  case Canvas.KEY_NUM1:
			number = 1;
			break;

		  case Canvas.KEY_NUM2:
			number = 2;
			break;

		  case Canvas.KEY_NUM3:
			number = 3;
			break;

		  case Canvas.KEY_NUM4:
			number = 4;
			break;

		  case Canvas.KEY_NUM5:
			number = 5;
		    break;

		  case Canvas.KEY_NUM6:
            number = 6;
            break;

		  case Canvas.KEY_NUM7:
			number = 7;
			break;

		  case Canvas.KEY_NUM8:
			number = 8;
			break;

		  case Canvas.KEY_NUM9:
			number = 9;
			break;
			
          case Canvas.KEY_POUND:
        	// # キーの入力
        	ret = inputPound(false);
			break;

          case Canvas.KEY_STAR:
        	// * キーの入力
          	ret = inputStar(false);
			break;

		  case -8:
			// Nokiaのクリアキー (現物合わせ)
            ret = inputGameC(false);
			break;

		  case -50:
            // NokiaのFuncキー... (現物合わせ)
	        ret = inputGameD(false);
            break;

		  default:
			ret = false;
            break;
		}
		if (ret == true)
		{
			number = -1;
		}
		if (number >= 0)
		{
            // 数字入力をキャンバスクラスへ伝達...
			disp.inputNumber(number);
		}
		return (number);
	}
}
