/*
 *  $Id: NGwordDbParser.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiのNGワード解析エンジン
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
	///  Class の概要の説明
	/// </summary>
	public class NGwordDbParser
	{
		const int	 NGWORD_LEN		= 40;

		const string DB_CREATOR		= "NNsi";
		const string DB_TYPE		= "Data";
		const string DB_NAME		= "NGWord-NNsi";
		const int 	 DB_VERSION		= 100;

		public NGwordDbParser()
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
		public bool inputRecordFromTemporaryFile(string aFileName, ref NGwordRecord entryRecord)
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

				// ngWord
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] ngWordData = reader.ReadBytes(NGWORD_LEN);
				offset = offset + NGWORD_LEN;
				entryRecord.ngWord = enc.GetString(ngWordData);

				//  checkArea
				uint checkArea = Convert.ToUInt32(reader.ReadByte());
				entryRecord.checkArea= checkArea;

				//  matchedAction
				uint matchedAction = Convert.ToUInt32(reader.ReadByte());
				entryRecord.matchedAction = matchedAction;

				// reserved1
				int reserved1 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.reserved = reserved1;

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
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref NGwordRecord aData)
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

				// ngWord
				byte[] ngWord = null;
				fileStream.Seek(0, SeekOrigin.Begin);
				if (aData.ngWord != null)
				{
					ngWord = enc.GetBytes(aData.ngWord);
					len = ngWord.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > NGWORD_LEN)
					{
						writer.Write(ngWord, 0, NGWORD_LEN);
						len = NGWORD_LEN;
					}
					else
					{
						writer.Write(ngWord, 0, len);
					}
				}
				for (; len < NGWORD_LEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + NGWORD_LEN;

				// checkArea
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte checkArea = (byte) aData.checkArea;
				writer.Write(checkArea);

				// matchedAction
				byte matchedAction = (byte) aData.matchedAction;
				writer.Write(matchedAction);

				// reserved
				UInt16 reserved = (UInt16) aData.reserved;
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

//////////////////////////////////////////////////
// // NG-3構造体
//////////////////////////////////////////////////
// typedef struct {
//	Char   ngWord[NGWORD_LEN];
//	UInt8  checkArea;
//	UInt8  matchedAction;
//	UInt16 reserved1;
//} NNshNGwordDatabase;
//////////////////////////////////////////////////

}
