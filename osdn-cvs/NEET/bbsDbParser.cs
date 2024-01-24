/*
 *  $Id: bbsDbParser.cs,v 1.1 2005/05/14 07:40:52 mrsa Exp $
 *    --- NNsiの板一覧データベースとテンポラリファイルとのやりとり...
 *       (データ変換を担当)
 *
 *      logTokenDbParser()	： コンストラクタ
 *      getDbName()			： ＤＢ名称を取得する
 *      getDbType()			： ＤＢタイプを取得する
 *      getDbCreatorName()	： ＤＢのクリエータ名を取得する
 *      getDbVersion()		： ＤＢのバージョン番号を取得する
 *      inputRecordFromTemporaryFile()
 *							： １レコードをファイルからオブジェクトに変換
 *      outputRecordToTemporaryFile()
 *							： １レコードをオブジェクトからファイルに変換
 *
 */
using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;
using System.Net;

namespace NEET
{
	/// <summary>
	///   getLogDbのテンポラリファイル解析・出力エンジン
	/// </summary>
	public class  bbsDbParser
	{
		/// <summary>
		///   定数の定義...
		/// </summary>
		const int MAX_URL			= 80;
		const int MAX_BOARDNAME		= 40;
		const int MAX_NICKNAME		= 16;

		const string DB_CREATOR		= "NNsi";
		const string DB_TYPE		= "Data";
		const string DB_NAME		= "BBS-NNsi";
		const int 	 DB_VERSION		= 160;

		/// <summary>
		///   コンストラクタ
		/// </summary>
		public bbsDbParser()
		{
			//
			// TODO: コンストラクタのロジックをここに追加
			//
		}

		/// <summary>
		///   DB名を応答する
		/// </summary>
		public string getDbName()
		{
			return (DB_NAME);
		}

		/// <summary>
		///   DBタイプを応答する
		/// </summary>
		public string getDbType()
		{
			return (DB_TYPE);
		}

		/// <summary>
		///   クリエータ名を応答する
		/// </summary>
		public string getDbCreatorName()
		{
			return (DB_CREATOR);
		}

		/// <summary>
		///   DBバージョン番号を応答する
		/// </summary>
		public int getDbVersion()
		{
			return (DB_VERSION);
		}

		/// <summary>
		///   ファイルから１レコードを読み出す
		/// </summary>
		public bool inputRecordFromTemporaryFile(string aFileName, ref bbsRecord entryRecord)
		{
			try
			{
				Stream fileStream;
				int offset = 0;
				fileStream = File.OpenRead(aFileName);

				// SHIFT JISのエンコーダを準備
				Encoding enc = Encoding.GetEncoding("Shift_Jis");

				// バイナリデータを読み出す...
				BinaryReader reader;
				reader = new BinaryReader(fileStream);

				// boardNick
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardNick = reader.ReadBytes(MAX_NICKNAME);
				offset = offset + MAX_NICKNAME;
				entryRecord.boardNick = enc.GetString(boardNick);

				// boardURL
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardURL = reader.ReadBytes(MAX_URL);
				offset = offset + MAX_URL;
				entryRecord.boardURL = enc.GetString(boardURL);

				// boardName
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardName = reader.ReadBytes(MAX_BOARDNAME);
				offset = offset + MAX_BOARDNAME;
				entryRecord.boardName = enc.GetString(boardName);


				//  bbsType
				uint bbsType = Convert.ToUInt32(reader.ReadByte());
				entryRecord.bbsType = bbsType;

				// state
				uint bbsState = Convert.ToUInt32(reader.ReadByte());
				entryRecord.bbsState = bbsState;

				// reserved
				int reserved = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.reserved = reserved;


				// ファイルをクローズする
				reader.Close();
			}
			catch
			{
				return (false);
			}
			return (true);
		}

		/// <summary>
		///   ファイルに１レコードを出力する
		/// </summary>
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref bbsRecord aData)
		{
			try
			{
				Stream fileStream;
				int offset = 0;
				fileStream = File.OpenWrite(aOutputFileName);

				// SHIFT JISのエンコーダを準備
				Encoding enc = Encoding.GetEncoding("Shift_Jis");

				// バイナリデータを書く準備
				BinaryWriter writer;
				writer = new BinaryWriter(fileStream);

				int len = 0;

				// boardNick
				byte[] boardNick = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.boardNick != null)
				{
					boardNick = enc.GetBytes(aData.boardNick);
					len = boardNick.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_NICKNAME)
					{
						writer.Write(boardNick, 0, MAX_NICKNAME);
						len = MAX_NICKNAME;
					}
					else
					{
						writer.Write(boardNick, 0, len);
					}
				}
				for (; len < MAX_NICKNAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_NICKNAME;


				// boardURL
				byte[] boardURL = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.boardURL != null)
				{
					boardURL = enc.GetBytes(aData.boardURL);
					len = boardURL.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_URL)
					{
						writer.Write(boardURL, 0, MAX_URL);
						len = MAX_URL;
					}
					else
					{
						writer.Write(boardURL, 0, len);
					}
				}
				for (; len < MAX_URL; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_URL;

				// boardName
				byte[] boardName = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.boardName != null)
				{
					boardName = enc.GetBytes(aData.boardName);
					len = boardName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_BOARDNAME)
					{
						writer.Write(boardName, 0, MAX_BOARDNAME);
						len = MAX_BOARDNAME;
					}
					else
					{
						writer.Write(boardName, 0, len);
					}
				}
				for (; len < MAX_BOARDNAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_BOARDNAME;


				// bbsType
				byte bbsType = (byte) aData.bbsType;
				writer.Write(bbsType);

				// bbsState
				byte bbsState = (byte) aData.bbsState;
				writer.Write(bbsState);

				// reserved
				UInt16 reserved = (UInt16) IPAddress.HostToNetworkOrder(aData.reserved);
				writer.Write(reserved);

				// NNsiのＤＢは４バイトの予約領域があるので...
				UInt32 dummyArea = 0;
				writer.Write(dummyArea);

				// ファイルをクローズする
				writer.Close();
			}
			catch
			{
				// 出力に失敗...(何もせず終了する)
				return (false);
			}
			return (true);
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
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}

