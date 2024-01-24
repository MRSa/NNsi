/*
 *  $Id: bbsRecord.cs,v 1.2 2005/07/18 04:33:20 mrsa Exp $
 *    --- NNsi��BBS�f�[�^�ݒ� (�P���R�[�h)
 *
 *
 */
using System;

namespace NEET
{
	/// <summary>
	///   BBS�f�[�^ �̊T�v
	///     NNsi��BBS-NNsi.pdb �̓��e�i�P���R�[�h�j���L������N���X
	/// </summary>
	public class bbsRecord : IComparable
	{
		private string mBoardNick;
		private string mBoardURL;
		private string mBoardName;
		private uint   mBbsType;
		private uint   mState;
		private int    mReserved;

		const int    NNSH_BBSSTATE_VISIBLE   = 0x01;
		const int    NNSH_BBSSTATE_INVISIBLE = 0x00;

		const int    NNSH_BBSSTATE_LEVELMASK = 0x0e;
		const int    NNSH_BBSSTATE_NOTFAVOR  = 0xf1;
		const int    NNSH_BBSSTATE_FAVOR_L1  = 0x02;
		const int    NNSH_BBSSTATE_FAVOR_L2  = 0x04;
		const int    NNSH_BBSSTATE_FAVOR_L3  = 0x06;
		const int    NNSH_BBSSTATE_FAVOR_L4  = 0x08;
		const int    NNSH_BBSSTATE_FAVOR     = 0x0e;

		const int    NNSH_BBSSTATE_SUSPEND   = 0x10;

		const int    NNSH_BBSTYPE_2ch                = 0x00; // 2�����˂�
		const int    NNSH_BBSTYPE_MACHIBBS           = 0x01; // �܂�BBS
		const int    NNSH_BBSTYPE_SHITARABA          = 0x02; // �������BBS
		const int    NNSH_BBSTYPE_SHITARABAJBBS_OLD  = 0x03; // (�� JBBS)
		const int    NNSH_BBSTYPE_OTHERBBS           = 0x04; // 2ch�݊�BBS
		const int    NNSH_BBSTYPE_PLUGINSUPPORT      = 0x05; // 2ch�`���ϊ�BBS
		const int    NNSH_BBSTYPE_HTML               = 0x06; // html�`���f�[�^
		const int    NNSH_BBSTYPE_SHITARABAJBBS_RAW  = 0x07; // �������@JBBS
		const int    NNSH_BBSTYPE_PLAINTEXT          = 0x08; // �e�L�X�g�`��
		const int    NNSH_BBSTYPE_2ch_EUC            = 0x09; // 2ch(EUC�`��)
		const int    NNSH_BBSTYPE_OTHERBBS_2         = 0x0a; // 2ch�݊�(����2)

		// <summary>
		//   �j�b�N�l�[��
		// </summary>
		public string boardNick
		{
			get
			{
				return (mBoardNick);
			}

			set
			{
				mBoardNick = value;
			}
		}

		// <summary>
		//   ��URL
		// </summary>
		public string boardURL
		{
			get
			{
				return (mBoardURL);
			}

			set
			{
				mBoardURL = value;
			}
		}

		// <summary>
		//   ����
		// </summary>
		public string boardName
		{
			get
			{
				return (mBoardName);
			}

			set
			{
				mBoardName = value;
			}
		}


		// <summary>
		//   BBS�^�C�v
		// </summary>
		public uint bbsType
		{
			get
			{
				return (mBbsType);
			}

			set
			{
				mBbsType = value;
			}
		}

		// <summary>
		//   BBS���
		// </summary>
		public uint bbsState
		{
			get
			{
				return (mState);
			}

			set
			{
				mState = value;
			}
		}

		// <summary>
		//   �\��̈�
		// </summary>
		public int reserved
		{
			get
			{
				return (mReserved);
			}

			set
			{
				mReserved = value;
			}
		}

		// ���C�ɓ��背�x���i������\���j
		public string favoriteLevel
		{
			get
			{
				switch ((mState)&(NNSH_BBSSTATE_LEVELMASK))
				{
				  case NNSH_BBSSTATE_FAVOR:
					return ("H");
					//break;
				  case NNSH_BBSSTATE_FAVOR_L1:
					return ("L");
					//break;
				  case NNSH_BBSSTATE_FAVOR_L2:
					return ("1");
					//break;
				  case NNSH_BBSSTATE_FAVOR_L3:
					return ("2");
					//break;
				  case NNSH_BBSSTATE_FAVOR_L4:
					return ("3");
					//break;

				  default:
					break;
				}
				return ("-");
			}

		}

		// ���C�ɓ��背�x���i���l�\���j
		public int favoriteLevelNumber
		{
			get
			{
				switch ((mState)&(NNSH_BBSSTATE_LEVELMASK))
				{
				  case NNSH_BBSSTATE_FAVOR_L1:
					return (1);
					//break;

				  case NNSH_BBSSTATE_FAVOR_L2:
					return (2);
					//break;

				  case NNSH_BBSSTATE_FAVOR_L3:
					return (3);
					//break;

				  case NNSH_BBSSTATE_FAVOR_L4:
					return (4);
					//break;

				  case NNSH_BBSSTATE_FAVOR:
					return (5);
					//break;

				  default:
					break;
				}
				return (0);
			}

			set
			{
				// �X�����C�ɓ��背�x�����N���A����
				switch ((mState)&(NNSH_BBSSTATE_LEVELMASK))
				{
				  case NNSH_BBSSTATE_FAVOR:
					mState = mState - NNSH_BBSSTATE_FAVOR;
					break;

				  case NNSH_BBSSTATE_FAVOR_L1:
					mState = mState - NNSH_BBSSTATE_FAVOR_L1;
					break;

				  case NNSH_BBSSTATE_FAVOR_L2:
					mState = mState - NNSH_BBSSTATE_FAVOR_L2;
					break;

				  case NNSH_BBSSTATE_FAVOR_L3:
					mState = mState - NNSH_BBSSTATE_FAVOR_L3;
					break;

				  case NNSH_BBSSTATE_FAVOR_L4:
					mState = mState - NNSH_BBSSTATE_FAVOR_L4;
					break;

				  default:
					break;
				}

				// �ݒ肳�ꂽ�X�����C�ɓ��背�x���𔽉f������
				switch (value)
				{
				  case 1:
					mState = mState + NNSH_BBSSTATE_FAVOR_L1;
					break;

				  case 2:
					mState = mState + NNSH_BBSSTATE_FAVOR_L2;
					break;

				  case 3:
					mState = mState + NNSH_BBSSTATE_FAVOR_L3;
					break;

				  case 4:
					mState = mState + NNSH_BBSSTATE_FAVOR_L4;
					break;

				  case 5:
					mState = mState + NNSH_BBSSTATE_FAVOR;
					break;

				  default:
					// �������Ȃ�...
					break;
				}

			}

		}

		// �擾���~���[�h
		public bool suspendMode
		{
			get
			{
				if (((mState)&(NNSH_BBSSTATE_SUSPEND)) == (NNSH_BBSSTATE_SUSPEND))
				{
					return (true);
				}
				return (false);
			}

			set
			{
				if (((mState)&(NNSH_BBSSTATE_SUSPEND)) == (NNSH_BBSSTATE_SUSPEND))
				{
					mState = mState - (NNSH_BBSSTATE_SUSPEND);
				}

				if (value == true)
				{
					mState = mState + (NNSH_BBSSTATE_SUSPEND);
				}
			}

		}

		// �ꗗ�ɕ\������
		public bool useBoardList
		{
			get
			{
				if ((mState & (NNSH_BBSSTATE_VISIBLE)) == (NNSH_BBSSTATE_VISIBLE))
				{
					return (true);
				}
				return (false);
			}

			set
			{
				if (((mState)&(NNSH_BBSSTATE_VISIBLE)) == (NNSH_BBSSTATE_VISIBLE))
				{
					mState = mState - (NNSH_BBSSTATE_VISIBLE);
				}

				if (value == true)
				{
					mState = mState + (NNSH_BBSSTATE_VISIBLE);
				}
			}
		}

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public bbsRecord()
		{
			//
			// TODO: �R���X�g���N�^�̃��W�b�N�������ɒǉ�
			//
			mBoardNick = "zznewBoard/";
			mBoardURL  = "http://";
			mBoardName = "New Board";
		}

		// �\�[�g�p�̔�r�f�[�^
		public int CompareTo(Object rhs)
		{
			bbsRecord r = (bbsRecord) rhs;
			return (this.boardNick.CompareTo(r.boardNick));
		}
	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ���f�[�^�x�[�X�̒�`  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_URL                     80
//#define MAX_BOARDNAME               40
//#define MAX_NICKNAME                16
//
//// ���(�f�[�^�x�[�X�̍\��)
//typedef struct {
//	Char   boardNick[MAX_NICKNAME];    // �{�[�h�j�b�N�l�[��
//	Char   boardURL [MAX_URL];         // �{�[�hURL
//	Char   boardName[MAX_BOARDNAME];   // �{�[�h��
//	UInt8  bbsType;                    // �{�[�hTYPE
//	UInt8  state;                      // �{�[�h�g�p���
//	Int16  reserved;                   // (�\��̈�F�ȑO�͎g�p���Ă���)
//} NNshBoardDatabase;
//
//
//#define NNSH_BBSSTATE_VISIBLE       0x01         // �\��
//#define NNSH_BBSSTATE_INVISIBLE     0x00         // ��\��
//
//#define NNSH_BBSSTATE_LEVELMASK     0x0e         // �X�����x�����o�}�X�N
//#define NNSH_BBSSTATE_NOTFAVOR      0xf1         // �X�����x���N���A
//#define NNSH_BBSSTATE_FAVOR_L1      0x02         // �X�����x��L1
//#define NNSH_BBSSTATE_FAVOR_L2      0x04         // �X�����x��L2
//#define NNSH_BBSSTATE_FAVOR_L3      0x06         // �X�����x��L3
//#define NNSH_BBSSTATE_FAVOR_L4      0x08         // �X�����x��L4
//#define NNSH_BBSSTATE_FAVOR         0x0e         // �X�����x��HIGH
//
//#define NNSH_BBSSTATE_SUSPEND       0x10   // �u�擾��~�vBBS�ł���
//#define NNSH_BBSSTATE_NOTSUSPEND    0xef   // �u�擾��~�vBBS�ł͂Ȃ�(MASK�l)
//
///** BBS-DB�̃{�[�h�^�C�v **/
//#define NNSH_BBSTYPE_2ch                 0x00    // 2ch(�ʏ탂�[�h)
//#define NNSH_BBSTYPE_MACHIBBS            0x01    // �܂�BBS(���ꃂ�[�h)
//#define NNSH_BBSTYPE_SHITARABA           0x02    // �������BBS
//#define NNSH_BBSTYPE_SHITARABAJBBS_OLD   0x03    // �������@JBBS
//#define NNSH_BBSTYPE_OTHERBBS            0x04    // 2ch�݊�BBS
//#define NNSH_BBSTYPE_PLUGINSUPPORT       0x05    // 2ch�`���ϊ�BBS
//#define NNSH_BBSTYPE_HTML                0x06    // html�`���f�[�^
//#define NNSH_BBSTYPE_SHITARABAJBBS_RAW   0x07    // �������@JBBS(RAWMODE)
//#define NNSH_BBSTYPE_PLAINTEXT           0x08    // �ʏ�̃e�L�X�g�`��
//#define NNSH_BBSTYPE_2ch_EUC             0x09    // 2ch(EUC�`��)
//#define NNSH_BBSTYPE_OTHERBBS_2          0x0a    // 2ch�݊��`��(����2)
//#define NNSH_BBSTYPE_ERROR               0xff    // �{�[�h�^�C�v�G���[
//#define NNSH_BBSTYPE_MASK                0x1f    // �{�[�h�^�C�vMASK
//#define NNSH_BBSTYPE_CHARSETMASK         0xe0    // �{�[�h�̕����R�[�h
//#define NNSH_BBSTYPE_CHAR_SJIS           0x00    // �W�������R�[�h(SHIFT JIS)
//#define NNSH_BBSTYPE_CHAR_EUC            0x20    // EUC�����R�[�h
//#define NNSH_BBSTYPE_CHAR_JIS            0x40    // JIS�����R�[�h(iso-2022-jp)
//#define NNSH_BBSTYPE_CHAR_SHIFT             5    // �E�V�t�g
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}

