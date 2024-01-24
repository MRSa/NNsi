/*
 *  $Id: logRecord.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̎Q�ƃ��O�f�[�^�ݒ� (�P���R�[�h)
 *
 *
 */
using System;

namespace NEET
{
	/// <summary>
	///  �Q�ƃ��O�f�[�^�̊T�v�̐���
	/// </summary>
	public class logRecord
	{
		/// <summary>
		///   �����o�ϐ�
		/// </summary>
		private string mDataFileName;
		private string mDataTitlePrefix;
		private string mBoardName;
		private string mDataURL;
		private string mGetDateTime;
		private string mDataTitle;
		private int    mWwwcMode;
		private int    mRssData;
		private int    mNotUse0;
		private int    mNotUse1;
		private uint   mLastUpdate;
		private int    mId;
		private int    mState;
		private int    mGetAfterHours;
		private int    mUsable;
		private int    mGetDepth;
		private int    mKanjiCode;
		private int    mGetSubData;
		private uint   mTokenId;
		private uint   mSubTokenId;

		/// <summary>
		///   �f�[�^�t�@�C����
		/// </summary>
		public string DataFileName
		{
			get
			{
				return (mDataFileName);
			}

			set
			{
				mDataFileName = value;
			}
		}

		/// <summary>
		///   �X���^�C�̃w�b�_
		/// </summary>
		public string DataTitlePrefix
		{
			get
			{
				return (mDataTitlePrefix);
			}

			set
			{
				mDataTitlePrefix = value;
			}
		}

		/// <summary>
		///   �̖���
		/// </summary>
		public string BoardName
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

		/// <summary>
		///   URL
		/// </summary>
		public string DataURL
		{
			get
			{
				return (mDataURL);
			}

			set
			{
				mDataURL = value;
			}
		}

		/// <summary>
		///   �擾���� (If-Modified-Since �Ŏ擾��������)
		/// </summary>
		public string GetDateTime
		{
			get
			{
				return (mGetDateTime);
			}

			set
			{
				mGetDateTime = value;
			}
		}

		/// <summary>
		///   �f�[�^�^�C�g��
		/// </summary>
		public string DataTitle
		{
			get
			{
				return (mDataTitle);
			}

			set
			{
				mDataTitle = value;
			}
		}

		/// <summary>
		///   WWWC���[�h...�@
		/// </summary>
		public int WwwcMode
		{
			get
			{
				return (mWwwcMode);
			}

			set
			{
				mWwwcMode = value;
			}
		}

		/// <summary>
		///   RSS���[�h...�@
		/// </summary>
		public int RssData
		{
			get
			{
				return (mRssData);
			}

			set
			{
				mRssData = value;
			}
		}

		/// <summary>
		///   �O��擾�����@
		/// </summary>
		public uint LastUpdate
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

		/// <summary>
		///   �f�[�^ID�@
		/// </summary>
		public int Id
		{
			get
			{
				return (mId);
			}

			set
			{
				mId = value;
			}
		}

		/// <summary>
		///   ��ԁ@
		/// </summary>
		public int State
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

		/// <summary>
		///   n���Ԍ�Ď擾�@
		/// </summary>
		public int GetAfterHours
		{
			get
			{
				return (mGetAfterHours);
			}

			set
			{
				mGetAfterHours = value;
			}
		}

		/// <summary>
		///   �g�p����/���Ȃ��@
		/// </summary>
		public int Usable
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

		/// <summary>
		///   �P�����擾����@
		/// </summary>
		public int GetDepth
		{
			get
			{
				return (mGetDepth);
			}

			set
			{
				mGetDepth = value;
			}
		}

		/// <summary>
		///   �����R�[�h�@
		/// </summary>
		public int KanjiCode
		{
			get
			{
				return (mKanjiCode);
			}

			set
			{
				mKanjiCode = value;
			}
		}

		/// <summary>
		///   �u�P�����擾�v�Ŏ擾�����f�[�^���@
		/// </summary>
		public int GetSubData
		{
			get
			{
				return (mGetSubData);
			}

			set
			{
				mGetSubData = value;
			}
		}


		/// <summary>
		///   �g�[�N���̂h�c
		/// </summary>
		public uint TokenId
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

		/// <summary>
		///   �g�[�N���̂h�c (�P���擾)
		/// </summary>
		public uint SubTokenId
		{
			get
			{
				return (mSubTokenId);
			}

			set
			{
				mSubTokenId = value;
			}
		}

		/// <summary>
		///   �g�p���Ȃ��f�[�^�i���̂P�j
		/// </summary>
		public int NotUse0
		{
			get
			{
				return (mNotUse0);
			}

			set
			{
				mNotUse0 = value;
			}
		}

		/// <summary>
		///   �g�p���Ȃ��f�[�^�i���̂Q�j
		/// </summary>
		public int NotUse1
		{
			get
			{
				return (mNotUse1);
			}

			set
			{
				mNotUse1 = value;
			}
		}

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public logRecord()
		{

		}



	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    �Q�ƃ��O�f�[�^�x�[�X�̒�`
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_THREADFILENAME          32
//#define MAX_DATAPREFIX              32
//#define MAX_NICKNAME                16
//#define MAX_GETLOG_URL             244
//#define MAX_GETLOG_DATETIME         38
//#define MAX_TITLE_LABEL             32
//
//
//typedef struct {
//	Char   dataFileName[MAX_THREADFILENAME]; // �f�[�^�t�@�C����
//	Char   dataTitlePrefix[MAX_DATAPREFIX];  // �f�[�^�w�b�_
//	Char   boardName      [MAX_NICKNAME];    // �{�[�h�j�b�N�l�[��(���g�p)
//	Char   dataURL    [MAX_GETLOG_URL];      // �f�[�^�擾��URL
//	Char   getDateTime[MAX_GETLOG_DATETIME]; // �X�V����
//	Char   dataTitle  [MAX_TITLE_LABEL];     // �f�[�^����
//	UInt8  wwwcMode;                         // ���O�擾���Ȃ�
//	UInt8  rssData;                          // RSS�f�[�^
//	UInt16 notUse0;                          // ���g�p�̈�(�p�~)
//	UInt16 notUse1;                          // ���g�p�̈�(�p�~)
//	UInt32 lastUpdate;                       // �ŏI�X�V����
//	UInt16 id;                               // �f�[�^ID
//	UInt8  state;                            // �Q�ƃ��O���
//	UInt8  getAfterHours;                    // �Ď擾���鎞�ԊԊu
//	UInt8  usable;                           // �擾����/���Ȃ�
//	UInt8  getDepth;                         // �擾�K�w�w��
//	UInt8  kanjiCode;                        // �����R�[�h
//	UInt8  getSubData;                       // �u�P���擾�v�Ŏ擾�����f�[�^��
//	UInt32 tokenId;                          // ��؂��ID
//	UInt32 subTokenId;                       // �T�u��؂��ID
//} NNshGetLogDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
