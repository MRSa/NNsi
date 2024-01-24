/*
 *  $Id: logTokenDbParser.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの区切り文字列データベースとテンポラリファイルとのやりとり...
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
	public class logTokenDbParser
	{
		/// <summary>
		///   定数の定義...
		/// </summary>
		const int MAX_STRLEN		= 32;
		const int MAX_DATATOKEN		= 80;

		const string DB_CREATOR		= "NNsi";
		const string DB_TYPE		= "Data";
		const string DB_NAME		= "LogToken-NNsi";
		const int 	 DB_VERSION		= 100;

		/// <summary>
		///   コンストラクタ
		/// </summary>
		public logTokenDbParser()
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
		public bool inputRecordFromTemporaryFile(string aFileName, ref logToken entryRecord)
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

				// id
				uint id = reader.ReadUInt32();
				entryRecord.id = id;
				offset = 4;

				// tokenPatternName
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] tokenPatternName = reader.ReadBytes(MAX_STRLEN);
				offset = offset + MAX_STRLEN;
				entryRecord.name = enc.GetString(tokenPatternName);

				// startToken
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] startToken = reader.ReadBytes(MAX_DATATOKEN);
				offset = offset + MAX_DATATOKEN;
				entryRecord.startToken = enc.GetString(startToken);

				// endToken
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] endToken = reader.ReadBytes(MAX_DATATOKEN);
				offset = offset + MAX_DATATOKEN;
				entryRecord.endToken = enc.GetString(endToken);

				// dataToken
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] dataToken = reader.ReadBytes(MAX_DATATOKEN);
				offset = offset + MAX_DATATOKEN;
				entryRecord.dataToken = enc.GetString(dataToken);

				// lastUpdate
				uint lastUpdate = reader.ReadUInt32();
				entryRecord.lastUpdate = lastUpdate;

				//  usable
				int usable = Convert.ToInt32(reader.ReadByte());
				entryRecord.usable = usable;

				// reserved0
				int reserved0 = Convert.ToInt32(reader.ReadByte());
				entryRecord.reserved0 = reserved0;

				// reserved1
				int reserved1 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.reserved1 = reserved1;

				// reserved2
				uint reserved2 = reader.ReadUInt32();
				entryRecord.reserved2 = reserved2;

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
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref logToken aData)
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

				//  id
				UInt32 id = aData.id;
				writer.Write(id);
				offset = 4;

				// tokenPatternName
				byte[] tokenPatternName = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.name != null)
				{
					tokenPatternName = enc.GetBytes(aData.name);
					len = tokenPatternName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_STRLEN)
					{
						writer.Write(tokenPatternName, 0, MAX_STRLEN);
						len = MAX_STRLEN;
					}
					else
					{
						writer.Write(tokenPatternName, 0, len);
					}
				}
				for (; len < MAX_STRLEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_STRLEN;


				// startToken
				byte[] startToken = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.startToken != null)
				{
					startToken = enc.GetBytes(aData.startToken);
					len = startToken.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATATOKEN)
					{
						writer.Write(startToken, 0, MAX_DATATOKEN);
						len = MAX_DATATOKEN;
					}
					else
					{
						writer.Write(startToken, 0, len);
					}
				}
				for (; len < MAX_DATATOKEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATATOKEN;

				// endToken
				byte[] endToken = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.endToken != null)
				{
					endToken = enc.GetBytes(aData.endToken);
					len = endToken.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATATOKEN)
					{
						writer.Write(endToken, 0, MAX_DATATOKEN);
						len = MAX_DATATOKEN;
					}
					else
					{
						writer.Write(endToken, 0, len);
					}
				}
				for (; len < MAX_DATATOKEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATATOKEN;

				// dataToken
				byte[] dataToken = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.dataToken != null)
				{
					dataToken = enc.GetBytes(aData.dataToken);
					len = dataToken.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATATOKEN)
					{
						writer.Write(dataToken, 0, MAX_DATATOKEN);
						len = MAX_DATATOKEN;
					}
					else
					{
						writer.Write(dataToken, 0, len);
					}
				}
				for (; len < MAX_DATATOKEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATATOKEN;

				// lastUpdate
				UInt32 lastUpdate = aData.lastUpdate;
				writer.Write(lastUpdate);

				// usable
				byte usable = (byte) aData.usable;
				writer.Write(usable);

				// reserved0
				byte reserved0 = (byte) aData.reserved0;
				writer.Write(reserved0);

				// reserved1
				UInt16 reserved1 = (UInt16) IPAddress.HostToNetworkOrder(aData.reserved1);
				writer.Write(reserved1);

				// reserved2
				UInt32 reserved2 = aData.reserved2;
				writer.Write(reserved2);

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
//    ログ区切り文字列データベースの定義
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_STRLEN                  32
//#define MAX_DATATOKEN               80
//
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

