/*
 *  $Id: logRecord.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの参照ログデータ設定 (１レコード)
 *
 *
 */
using System;

namespace NEET
{
	/// <summary>
	///  参照ログデータの概要の説明
	/// </summary>
	public class logRecord
	{
		/// <summary>
		///   メンバ変数
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
		///   データファイル名
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
		///   スレタイのヘッダ
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
		///   板の名称
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
		///   取得時刻 (If-Modified-Since で取得した時間)
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
		///   データタイトル
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
		///   WWWCモード...　
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
		///   RSSモード...　
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
		///   前回取得時刻　
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
		///   データID　
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
		///   状態　
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
		///   n時間後再取得　
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
		///   使用する/しない　
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
		///   １つ下も取得する　
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
		///   漢字コード　
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
		///   「１つ下も取得」で取得したデータ数　
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
		///   トークンのＩＤ
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
		///   トークンのＩＤ (１つ下取得)
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
		///   使用しないデータ（その１）
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
		///   使用しないデータ（その２）
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
		///   コンストラクタ
		/// </summary>
		public logRecord()
		{

		}



	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    参照ログデータベースの定義
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
//	Char   dataFileName[MAX_THREADFILENAME]; // データファイル名
//	Char   dataTitlePrefix[MAX_DATAPREFIX];  // データヘッダ
//	Char   boardName      [MAX_NICKNAME];    // ボードニックネーム(未使用)
//	Char   dataURL    [MAX_GETLOG_URL];      // データ取得先URL
//	Char   getDateTime[MAX_GETLOG_DATETIME]; // 更新時刻
//	Char   dataTitle  [MAX_TITLE_LABEL];     // データ名称
//	UInt8  wwwcMode;                         // ログ取得しない
//	UInt8  rssData;                          // RSSデータ
//	UInt16 notUse0;                          // 未使用領域(廃止)
//	UInt16 notUse1;                          // 未使用領域(廃止)
//	UInt32 lastUpdate;                       // 最終更新時間
//	UInt16 id;                               // データID
//	UInt8  state;                            // 参照ログ状態
//	UInt8  getAfterHours;                    // 再取得する時間間隔
//	UInt8  usable;                           // 取得する/しない
//	UInt8  getDepth;                         // 取得階層指定
//	UInt8  kanjiCode;                        // 漢字コード
//	UInt8  getSubData;                       // 「１つ下取得」で取得したデータ数
//	UInt32 tokenId;                          // 区切りのID
//	UInt32 subTokenId;                       // サブ区切りのID
//} NNshGetLogDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
