/*
 *  $Id: PalmDatabaseAccessor.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- PalmOSのデータベースをアクセスするクラス..
 *
 *      [Memo]
 *         par.exeのwrapper.
 *
 */
using System;
using System.Collections;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;

namespace NEET
{
	/// <summary>
	///  Class の概要の説明
	/// </summary>
	public class PalmDatabaseAccessor
	{
		private string mParExe;				// par.exeのファイル名(絶対パス)
		private string mDatabaseFileName;   // データベースファイル名称

		/// <summary>
		///   コンストラクタ
		/// </summary>
		public PalmDatabaseAccessor(string aParExe, string aFileName)
		{
			//
			// TODO: コンストラクタのロジックをここに追加
			//
			mParExe           = aParExe;
			mDatabaseFileName = aFileName;
		}

		/// <summary>
		///   1レコードを切り出し、ファイルに出力する
		/// </summary>
		public bool pickUpRecord(int aIndexNumber, string aFileName)
		{

			// par.exeに渡す引数を作成する
			string argument = "x " + mDatabaseFileName +
								" -f " + aFileName + " " + aIndexNumber;
			string outputData = "";

			// par.exe を実行して、１つのレコードをファイルに抽出する
			return (executeParExe(argument, out outputData));
		}


		///  <summary>
		///    データベースのレコード数を取得する
		///  </summary>
		public bool getRecordNumber(out int aRecords)
		{
			string argument = "h " + mDatabaseFileName;
			string output;
			if (executeParExe(argument, out output) == false)
			{
				// 読み込み失敗。。。終了する
				aRecords = 0;
				return (false);
			}

			try
			{
				//  読み出したデータをトリミングする...
				//  (ちょー無理やり... "nrecords : ###" を見つけて数値を切り出す)
				int len = output.LastIndexOf("nrecords");
				output = output.Remove(0, len);
				Regex numberPattern = new Regex("[0-9]+");	// 数値のパターン
				Match number = numberPattern.Match(output);	// 文字列から数値のパターンを抜き出す

				// 切り取った文字列をstring型からint型に変換
				aRecords = Convert.ToInt32(number.ToString());
			}
			catch
			{
				// 変換に失敗...読み込み失敗にする。
				aRecords = 0;
				return (false);
			}

			// 読み込み成功
			return (true);
		}

		///  <summary>
		///    からっぽのデータベースを作成する
		///  </summary>
		public bool createDatabase(string dbNameWithPath, string dbName, string dbType, string dbCreator, int dbVersion)
		{
			string argument = "c -v " + dbVersion + " " + dbNameWithPath + " " + dbName + " " + dbType + " " + dbCreator;
			string result;
			try
			{
				// par.exeを実行する
				executeParExe(argument, out result);
			}
			catch
			{
				return (false);
			}
			return (true);
		}

		///  <summary>
		///    データベースにレコードを１つ追加する
		///  </summary>
		public bool appendRecordToDatabase(string dbNameWithPath, string recordFile)
		{
			string argument = "a " + dbNameWithPath + " " + recordFile;
			string result;
			try
			{
				// par.exeを実行する
				executeParExe(argument, out result);
			}
			catch
			{
				return (false);
			}
			return (true);
		}

		/// <summary>
		///   par.exeを実行する
		///     (標準出力に格納される情報を result に格納して応答する)
		/// </summary>
		private bool executeParExe(string argument, out string result)
		{

			// par.exe 起動のための準備...
			ProcessStartInfo pInfo       = new ProcessStartInfo();
			pInfo.FileName               = mParExe;     // 実行コマンドのフルパス
			pInfo.Arguments              = argument;	// 引数の指定
			pInfo.WindowStyle         	 = ProcessWindowStyle.Minimized; // 最小化して起動
			pInfo.CreateNoWindow         = true;	// 起動時ウィンドウを表示しない
			pInfo.UseShellExecute        = false;	// 起動時OSのシェルを使用しない
			pInfo.RedirectStandardInput  = false;	// 標準入力を禁止する
			pInfo.RedirectStandardOutput = true;	// 標準出力を使用する
			// pInfo.WorkingDirectory = "(作業フォルダ)";  // 作業フォルダの指定
			try
			{
				// アプリケーションを起動
				Process proc = Process.Start(pInfo);

				// 標準出力に出力されるデータを取得する
				result = proc.StandardOutput.ReadToEnd();

				// アプリが終了するまで待つ...
				proc.WaitForExit();
			}
			catch
			{
				// 実行に失敗
				result = "";
				return (false);
			}

			// 実行成功
			return (true);
		}
	}
}
