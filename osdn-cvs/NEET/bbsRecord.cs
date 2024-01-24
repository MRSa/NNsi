/*
 *  $Id: bbsRecord.cs,v 1.2 2005/07/18 04:33:20 mrsa Exp $
 *    --- NNsiのBBSデータ設定 (１レコード)
 *
 *
 */
using System;

namespace NEET
{
	/// <summary>
	///   BBSデータ の概要
	///     NNsiのBBS-NNsi.pdb の内容（１レコード）を記憶するクラス
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

		const int    NNSH_BBSTYPE_2ch                = 0x00; // 2ちゃんねる
		const int    NNSH_BBSTYPE_MACHIBBS           = 0x01; // まちBBS
		const int    NNSH_BBSTYPE_SHITARABA          = 0x02; // したらばBBS
		const int    NNSH_BBSTYPE_SHITARABAJBBS_OLD  = 0x03; // (旧 JBBS)
		const int    NNSH_BBSTYPE_OTHERBBS           = 0x04; // 2ch互換BBS
		const int    NNSH_BBSTYPE_PLUGINSUPPORT      = 0x05; // 2ch形式変換BBS
		const int    NNSH_BBSTYPE_HTML               = 0x06; // html形式データ
		const int    NNSH_BBSTYPE_SHITARABAJBBS_RAW  = 0x07; // したらば@JBBS
		const int    NNSH_BBSTYPE_PLAINTEXT          = 0x08; // テキスト形式
		const int    NNSH_BBSTYPE_2ch_EUC            = 0x09; // 2ch(EUC形式)
		const int    NNSH_BBSTYPE_OTHERBBS_2         = 0x0a; // 2ch互換(その2)

		// <summary>
		//   板ニックネーム
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
		//   板URL
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
		//   板名称
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
		//   BBSタイプ
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
		//   BBS状態
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
		//   予約領域
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

		// お気に入りレベル（文字列表現）
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

		// お気に入りレベル（数値表現）
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
				// スレお気に入りレベルをクリアする
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

				// 設定されたスレお気に入りレベルを反映させる
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
					// 何もしない...
					break;
				}

			}

		}

		// 取得中止モード
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

		// 板一覧に表示する
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
		///   コンストラクタ
		/// </summary>
		public bbsRecord()
		{
			//
			// TODO: コンストラクタのロジックをここに追加
			//
			mBoardNick = "zznewBoard/";
			mBoardURL  = "http://";
			mBoardName = "New Board";
		}

		// ソート用の比較データ
		public int CompareTo(Object rhs)
		{
			bbsRecord r = (bbsRecord) rhs;
			return (this.boardNick.CompareTo(r.boardNick));
		}
	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    板情報データベースの定義  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_URL                     80
//#define MAX_BOARDNAME               40
//#define MAX_NICKNAME                16
//
//// 板情報(データベースの構造)
//typedef struct {
//	Char   boardNick[MAX_NICKNAME];    // ボードニックネーム
//	Char   boardURL [MAX_URL];         // ボードURL
//	Char   boardName[MAX_BOARDNAME];   // ボード名
//	UInt8  bbsType;                    // ボードTYPE
//	UInt8  state;                      // ボード使用状態
//	Int16  reserved;                   // (予約領域：以前は使用していた)
//} NNshBoardDatabase;
//
//
//#define NNSH_BBSSTATE_VISIBLE       0x01         // 表示
//#define NNSH_BBSSTATE_INVISIBLE     0x00         // 非表示
//
//#define NNSH_BBSSTATE_LEVELMASK     0x0e         // スレレベル抽出マスク
//#define NNSH_BBSSTATE_NOTFAVOR      0xf1         // スレレベルクリア
//#define NNSH_BBSSTATE_FAVOR_L1      0x02         // スレレベルL1
//#define NNSH_BBSSTATE_FAVOR_L2      0x04         // スレレベルL2
//#define NNSH_BBSSTATE_FAVOR_L3      0x06         // スレレベルL3
//#define NNSH_BBSSTATE_FAVOR_L4      0x08         // スレレベルL4
//#define NNSH_BBSSTATE_FAVOR         0x0e         // スレレベルHIGH
//
//#define NNSH_BBSSTATE_SUSPEND       0x10   // 「取得停止」BBSである
//#define NNSH_BBSSTATE_NOTSUSPEND    0xef   // 「取得停止」BBSではない(MASK値)
//
///** BBS-DBのボードタイプ **/
//#define NNSH_BBSTYPE_2ch                 0x00    // 2ch(通常モード)
//#define NNSH_BBSTYPE_MACHIBBS            0x01    // まちBBS(特殊モード)
//#define NNSH_BBSTYPE_SHITARABA           0x02    // したらばBBS
//#define NNSH_BBSTYPE_SHITARABAJBBS_OLD   0x03    // したらば@JBBS
//#define NNSH_BBSTYPE_OTHERBBS            0x04    // 2ch互換BBS
//#define NNSH_BBSTYPE_PLUGINSUPPORT       0x05    // 2ch形式変換BBS
//#define NNSH_BBSTYPE_HTML                0x06    // html形式データ
//#define NNSH_BBSTYPE_SHITARABAJBBS_RAW   0x07    // したらば@JBBS(RAWMODE)
//#define NNSH_BBSTYPE_PLAINTEXT           0x08    // 通常のテキスト形式
//#define NNSH_BBSTYPE_2ch_EUC             0x09    // 2ch(EUC形式)
//#define NNSH_BBSTYPE_OTHERBBS_2          0x0a    // 2ch互換形式(その2)
//#define NNSH_BBSTYPE_ERROR               0xff    // ボードタイプエラー
//#define NNSH_BBSTYPE_MASK                0x1f    // ボードタイプMASK
//#define NNSH_BBSTYPE_CHARSETMASK         0xe0    // ボードの文字コード
//#define NNSH_BBSTYPE_CHAR_SJIS           0x00    // 標準文字コード(SHIFT JIS)
//#define NNSH_BBSTYPE_CHAR_EUC            0x20    // EUC漢字コード
//#define NNSH_BBSTYPE_CHAR_JIS            0x40    // JIS漢字コード(iso-2022-jp)
//#define NNSH_BBSTYPE_CHAR_SHIFT             5    // 右シフト
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}

