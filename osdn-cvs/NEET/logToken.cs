/*
 *  $Id: logToken.cs,v 1.1.1.1 2005/01/30 13:28:25 mrsa Exp $
 *    --- NNsi�̎Q�ƃ��O��؂蕶����ݒ� (�P���R�[�h)
 *
 *
 */
using System;

namespace NEET
{
	/// <summary>
	///   logToken(��؂蕶����) �̊T�v
	///     NNsi��LogToken-NNsi.pdb �̓��e�i�P���R�[�h�j���L������N���X
	/// </summary>
	public class logToken
	{
		private uint   mTokenId;
		private string mTokenPatternName;
		private string mStartToken;
		private string mEndToken;
		private string mDataToken;
		private uint   mLastUpdate;
		private int    mUsable;
		private int    mReserved0;
		private int    mReserved1;
		private uint   mReserved2;

		// <summary>
		//   �v���p�e�B (ID)
		// </summary>
		public uint id
		{
			get
			{
				return (mTokenId);
			}

			set
			{
				mTokenId = value;
			}
		}

		// <summary>
		//   ��؂蕶���̖���
		// </summary>
		public string name
		{
			get
			{
				return (mTokenPatternName);
			}

			set
			{
				mTokenPatternName = value;
			}
		}

		// <summary>
		//   ��؂蕶���̖���
		// </summary>
		public string startToken
		{
			get
			{
				return (mStartToken);
			}

			set
			{
				mStartToken = value;
			}
		}

		// <summary>
		//   ��؂蕶���̖���
		// </summary>
		public string endToken
		{
			get
			{
				return (mEndToken);
			}

			set
			{
				mEndToken = value;
			}
		}

		// <summary>
		//   ��؂蕶���̖���
		// </summary>
		public string dataToken
		{
			get
			{
				return (mDataToken);
			}

			set
			{
				mDataToken = value;
			}
		}

		// <summary>
		//   �ŏI�X�V����
		// </summary>
		public uint lastUpdate
		{
			get
			{
				return (mLastUpdate);
			}

			set
			{
				mLastUpdate = value;
			}
		}

		// <summary>
		//   �g�p����/�g�p���Ȃ��̐ݒ�
		// </summary>
		public int usable
		{
			get
			{
				return (mUsable);
			}

			set
			{
				mUsable = value;
			}
		}

		// <summary>
		//   �\��̈�O
		// </summary>
		public int reserved0
		{
			get
			{
				return (mReserved0);
			}

			set
			{
				mReserved0 = value;
			}
		}

		// <summary>
		//   �\��̈�P
		// </summary>
		public int reserved1
		{
			get
			{
				return (mReserved1);
			}

			set
			{
				mReserved1 = value;
			}
		}

		// <summary>
		//   �\��̈�Q
		// </summary>
		public uint reserved2
		{
			get
			{
				return (mReserved2);
			}

			set
			{
				mReserved2 = value;
			}
		}

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public logToken()
		{
			//
			// TODO: �R���X�g���N�^�̃��W�b�N�������ɒǉ�
			//
		}

	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ���O��؂蕶����f�[�^�x�[�X�̒�`  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//
//#define MAX_STRLEN                  32
//#define MAX_DATATOKEN               80
//
// ���X��؂�p�\����(�f�[�^�x�[�X)
//typedef struct {
//	UInt32 id;                              // �f�[�^ID
//	Char   tokenPatternName[MAX_STRLEN];    // ��؂�p�^�[����
//	Char   startToken[MAX_DATATOKEN];       // �؂�o���J�n������
//	Char   endToken  [MAX_DATATOKEN];       // �؂�o���I��������
//	Char   dataToken [MAX_DATATOKEN];       // �f�[�^�C���f�b�N�X������
//	UInt32 lastUpdate;                      // �ŏI�X�V����
//	UInt8  usable;                          // �g�p����/���Ȃ�
//	UInt8  reserved0;                       // �\��̈�
//	UInt16 reserved1;                       // �\��̈�
//	UInt32 reserved2;                       // �\��̈�
//} NNshLogTokenDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
