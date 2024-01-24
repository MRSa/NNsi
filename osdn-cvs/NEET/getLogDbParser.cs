/*
 *  $Id: getLogDbParser.cs,v 1.5 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの参照ログデータベースとテンポラリファイルとのやりとり...
 *       (データ変換を担当)
 *
 *      GetLogDbParser()	： コンストラクタ
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
	public class GetLogDbParser
	{
		/// <summary>
		///   定数の定義...
		/// </summary>
		const int MAX_THREADFILENAME  	=  32;
		const int MAX_DATAPREFIX      	=  32;
		const int MAX_NICKNAME        	=  16;
		const int MAX_GETLOG_URL      	= 244;
		const int MAX_GETLOG_DATETIME 	=  38;
		const int MAX_TITLE_LABEL     	=  32;

		const string DB_CREATOR			= "NNsi";
		const string DB_TYPE  			= "Data";
		const string DB_NAME			= "LogPermanent-NNsi";
		const int 	 DB_VERSION 		= 100;


		public GetLogDbParser()
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
		public bool inputRecordFromTemporaryFile(string aFileName, ref logRecord entryRecord)
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

				// dataFileName
				fileStream.Seek(0, SeekOrigin.Begin);
				string dataFileName = new string(reader.ReadChars(MAX_THREADFILENAME));
				offset = offset + MAX_THREADFILENAME;
				entryRecord.DataFileName = dataFileName;

				// dataTitlePrefix
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] dataTitlePrefix = reader.ReadBytes(MAX_DATAPREFIX);
				offset = offset + MAX_DATAPREFIX;
				entryRecord.DataTitlePrefix = enc.GetString(dataTitlePrefix);

				// boardName
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardName = reader.ReadBytes(MAX_NICKNAME);
				offset = offset + MAX_NICKNAME;
				entryRecord.BoardName = enc.GetString(boardName);

				// dataURL
				fileStream.Seek(offset, SeekOrigin.Begin);
				string dataURL = new string(reader.ReadChars(MAX_GETLOG_URL));
				offset = offset + MAX_GETLOG_URL;
				entryRecord.DataURL = dataURL;

				// getDateTime
				fileStream.Seek(offset, SeekOrigin.Begin);
				string getDateTime = new string(reader.ReadChars(MAX_GETLOG_DATETIME));
				offset = offset + MAX_GETLOG_DATETIME;
				entryRecord.GetDateTime = getDateTime;

				// dataTitle
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] dataTitle = reader.ReadBytes(MAX_TITLE_LABEL);
				offset = offset + MAX_TITLE_LABEL;
				entryRecord.DataTitle =  enc.GetString(dataTitle);

				// wwwcMode
				fileStream.Seek(offset, SeekOrigin.Begin);
				int wwwcMode = Convert.ToInt32(reader.ReadByte());
				entryRecord.WwwcMode = wwwcMode;

				// rssData
				int rssData = Convert.ToInt32(reader.ReadByte());
				entryRecord.RssData = rssData;

				// notUse0
				int notUse0 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.NotUse0 = notUse0;


				// notUse1
				int notUse1 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.NotUse1 = notUse1;

				// lastUpdate
				uint lastUpdate = reader.ReadUInt32();
				entryRecord.LastUpdate = lastUpdate;

				// id
				int id = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.Id = id;

				// state
				int state = Convert.ToInt32(reader.ReadByte());
				entryRecord.State = state;

				// getAfterHours
				int getAfterHours = Convert.ToInt32(reader.ReadByte());
				entryRecord.GetAfterHours = getAfterHours;

				// usable
				int usable = Convert.ToInt32(reader.ReadByte());
				entryRecord.Usable = usable;

				// getDepth
				int getDepth = Convert.ToInt32(reader.ReadByte());
				entryRecord.GetDepth = getDepth;

				// kanjiCode
				int kanjiCode = Convert.ToInt32(reader.ReadByte());
				entryRecord.KanjiCode = kanjiCode;

				// getSubData
				int getSubData = Convert.ToInt32(reader.ReadByte());
				entryRecord.GetSubData = getSubData;

				// tokenId
				uint tokenId = reader.ReadUInt32();
				entryRecord.TokenId = tokenId;

				// subTokenId
				uint subTokenId = reader.ReadUInt32();
				entryRecord.SubTokenId = subTokenId;

				// URLの表示
				// MessageBox.Show(
				//				"KanjiCode :" + kanjiCode +
				//				" id :" + id + " tokenId :" + tokenId + "\n\n" +
				//				" URL : " + dataURL);

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
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref logRecord aData)
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

				// dataFileName
				byte[] dataFileName = null;
				fileStream.Seek(0, SeekOrigin.Begin);
				if (aData.DataFileName != null)
				{
					dataFileName = enc.GetBytes(aData.DataFileName);
					len = dataFileName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_THREADFILENAME)
					{
						writer.Write(dataFileName, 0, MAX_THREADFILENAME);
						len = MAX_THREADFILENAME;
					}
					else
					{
						writer.Write(dataFileName, 0, len);
					}
				}
				for (; len < MAX_THREADFILENAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_THREADFILENAME;

				// dataTitlePrefix
				byte[] dataTitlePrefix = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.DataTitlePrefix != null)
				{
					dataTitlePrefix = enc.GetBytes(aData.DataTitlePrefix);
					len = dataTitlePrefix.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATAPREFIX)
					{
						writer.Write(dataTitlePrefix, 0, MAX_DATAPREFIX);
						len = MAX_DATAPREFIX;
					}
					else
					{
						writer.Write(dataTitlePrefix, 0, len);
					}
				}
				for (; len < MAX_DATAPREFIX; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATAPREFIX;

				// boardName
				byte[] boardName = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.BoardName != null)
				{
					boardName = enc.GetBytes(aData.BoardName);
					len = boardName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_NICKNAME)
					{
						writer.Write(boardName, 0, MAX_NICKNAME);
						len = MAX_NICKNAME;
					}
					else
					{
						writer.Write(boardName, 0, len);
					}
				}
				for (; len < MAX_NICKNAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_NICKNAME;

				// dataURL
				byte[] dataURL = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.DataURL != null)
				{
					dataURL = enc.GetBytes(aData.DataURL);
					len = dataURL.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_GETLOG_URL)
					{
						writer.Write(dataURL, 0, MAX_GETLOG_URL);
						len = MAX_GETLOG_URL;
					}
					else
					{
						writer.Write(dataURL, 0, len);
					}
				}
				for (; len < MAX_GETLOG_URL; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_GETLOG_URL;

				// getDateTime
				byte[] getDateTime = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.GetDateTime != null)
				{
					getDateTime = enc.GetBytes(aData.GetDateTime);
					len = getDateTime.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_GETLOG_DATETIME)
					{
						writer.Write(getDateTime, 0, MAX_GETLOG_DATETIME);
						len = MAX_GETLOG_DATETIME;
					}
					else
					{
						writer.Write(getDateTime, 0, len);
					}
				}
				for (; len < MAX_GETLOG_DATETIME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_GETLOG_DATETIME;

				// dataTitle
				byte[] dataTitle = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.DataTitle != null)
				{
					dataTitle = enc.GetBytes(aData.DataTitle);
					len = dataTitle.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_TITLE_LABEL)
					{
						writer.Write(dataTitle, 0, MAX_TITLE_LABEL);
						len = MAX_TITLE_LABEL;
					}
					else
					{
						writer.Write(dataTitle, 0, len);
					}
				}
				for (; len < MAX_TITLE_LABEL; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_TITLE_LABEL;

				// wwwcMode
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte wwwcMode = (byte) aData.WwwcMode;
				writer.Write(wwwcMode);

				// rssData
				byte rssData = (byte) aData.RssData;
				writer.Write(rssData);

				// notUse0
				UInt16 notUse0 = (UInt16) aData.NotUse0;
				writer.Write(notUse0);

				// notUse1
				UInt16 notUse1 = (UInt16) aData.NotUse1;
				writer.Write(notUse1);

				// lastUpdate
				UInt32 lastUpdate = aData.LastUpdate;
				writer.Write(lastUpdate);

				// id
				UInt16 id = (UInt16) IPAddress.HostToNetworkOrder(aData.Id);
				writer.Write(id);

				// state
				byte state = (byte) aData.State;
				writer.Write(state);

				// getAfterHours
				byte getAfterHours = (byte) aData.GetAfterHours;
				writer.Write(getAfterHours);

				// usable
				byte usable = (byte) aData.Usable;
				writer.Write(usable);

				// getDepth
				byte getDepth = (byte) aData.GetDepth;
				writer.Write(getDepth);

				// kanjiCode
				byte kanjiCode = (byte) aData.KanjiCode;
				writer.Write(kanjiCode);

				// getSubData
				byte getSubData = (byte) aData.GetSubData;
				writer.Write(getSubData);

				// tokenId
				UInt32 tokenId = aData.TokenId;
				writer.Write(tokenId);

				// subTokenId
				UInt32 subTokenId = aData.SubTokenId;
				writer.Write(subTokenId);

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
