/*
 *  $Id: logToken.cs,v 1.1.1.1 2005/01/30 13:28:25 mrsa Exp $
 *    --- NNsiの参照ログ区切り文字列設定 (１レコード)
 *
 *
 */
using System;

namespace NEET
{
	/// <summary>
	///   logToken(区切り文字列) の概要
	///     NNsiのLogToken-NNsi.pdb の内容（１レコード）を記憶するクラス
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
		//   プロパティ (ID)
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
		//   区切り文字の名称
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
		//   区切り文字の名称
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
		//   区切り文字の名称
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
		//   区切り文字の名称
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
		//   最終更新時刻
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
		//   使用する/使用しないの設定
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
		//   予約領域０
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
		//   予約領域１
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
		//   予約領域２
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
		///   コンストラクタ
		/// </summary>
		public logToken()
		{
			//
			// TODO: コンストラクタのロジックをここに追加
			//
		}

	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ログ区切り文字列データベースの定義  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//
//#define MAX_STRLEN                  32
//#define MAX_DATATOKEN               80
//
// レス区切り用構造体(データベース)
//typedef struct {
//	UInt32 id;                              // データID
//	Char   tokenPatternName[MAX_STRLEN];    // 区切りパターン名
//	Char   startToken[MAX_DATATOKEN];       // 切り出し開始文字列
//	Char   endToken  [MAX_DATATOKEN];       // 切り出し終了文字列
//	Char   dataToken [MAX_DATATOKEN];       // データインデックス文字列
//	UInt32 lastUpdate;                      // 最終更新時間
//	UInt8  usable;                          // 使用する/しない
//	UInt8  reserved0;                       // 予約領域
//	UInt16 reserved1;                       // 予約領域
//	UInt32 reserved2;                       // 予約領域
//} NNshLogTokenDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
