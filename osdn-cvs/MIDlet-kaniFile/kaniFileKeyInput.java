import javax.microedition.lcdui.Canvas;

/**
 * (Canvas�N���X��)�L�[���͂��ꂽ�Ƃ��̏���
 * @author MRSa
 */
public class kaniFileKeyInput
{
	private IkaniFileCanvasPaint  disp          = null;

	/**
	 * �R���X�g���N�^
	 * @param object
	 */
	kaniFileKeyInput()
	{
		// �������Ȃ�...
	}

	/**
	 * �X�N���[���L�����o�X��ݒ�...
	 * @param object
	 */
	public void setScreenCanvas(IkaniFileCanvasPaint object)
	{
		disp = object;
		return;
	}
	
	/**
	 * ���J�[�\���L�[�����͂��ꂽ
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
	 * �E�J�[�\���L�[�����͂��ꂽ
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
	 * ��J�[�\���L�[�����͂��ꂽ
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
	 * ���J�[�\���L�[�����͂��ꂽ
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
	 * ���{�^�������͂��ꂽ
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
	 * A�{�^�������͂��ꂽ
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
	 * B�{�^�������͂��ꂽ
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
	 * C�{�^�������͂��ꂽ
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
	 * D�{�^�������͂��ꂽ
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
	 * #�{�^�������͂��ꂽ
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
	 * *�{�^�������͂��ꂽ
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
	 * ���̑��̃f�[�^�����͂��ꂽ
	 * @return
	 */
	public boolean inputDummy()
	{
		disp.inputDummy();
		return (false);
	}
	

	/**
	 * �����{�^�������͂��ꂽ
	 * @param isRepeat
	 * @return
	 */
	public int numberInputMode(int keyCode)
	{
        boolean ret = false;
		int number = -10;
		
		// �ʏ탂�[�h���̃L�[���͏���...
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
        	// # �L�[�̓���
        	ret = inputPound(false);
			break;

          case Canvas.KEY_STAR:
        	// * �L�[�̓���
          	ret = inputStar(false);
			break;

		  case -8:
			// Nokia�̃N���A�L�[ (�������킹)
            ret = inputGameC(false);
			break;

		  case -50:
            // Nokia��Func�L�[... (�������킹)
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
            // �������͂��L�����o�X�N���X�֓`�B...
			disp.inputNumber(number);
		}
		return (number);
	}
}
