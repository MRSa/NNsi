/*
 *  $Id: logTokenDbContainer.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの区切り文字データベースの記録場所...
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
	///  区切り文字列DBのコンテナ
	/// </summary>
	public class logTokenDbContainer
	{
		// 「ファイルを開く」ダイアログ
		private System.Windows.Forms.OpenFileDialog fileSelectionDialog = null;

		// PalmOSのDBファイルのアクセサ(par.exeのwrapperクラス)
		private PalmDatabaseAccessor mDbAccessor		= null;

		// 区切り文字ＤＢのレコードと一時ファイルの変換
		private logTokenDbParser 	mDbParser			= null;

		// 区切り文字ＤＢの入力ファイル名
		private string				mDbName				= null;

		// 参照ログＤＢのオブジェクト管理用リスト
		private ArrayList			mDbArrayList		= null;

		/// <summary>
		///   コンストラクタ
		/// </summary>
		public logTokenDbContainer(string aDbName, ref PalmDatabaseAccessor aAccessor)
		{
			//  オブジェクトの初期化...
			mDbName			= aDbName;
			mDbAccessor		= aAccessor;
			mDbArrayList	= new ArrayList();
			mDbParser		= new logTokenDbParser();
			fileSelectionDialog = new System.Windows.Forms.OpenFileDialog();
			fileSelectionDialog.AddExtension = true;
			
		}

		/// <summary>
		///    データコンテナクラスを初期化する
		///    (DBファイルからデータを読み出してオブジェクトに展開する)
		/// </summary>
		public bool initializeDatas()
		{
			int nofRecords = 0;

			// ＤＢのヘッダ情報を読んでレコード数を取得する
			if (mDbAccessor.getRecordNumber(out nofRecords) == false)
			{
				// DBの読み込み失敗。。。(何もせず終了する)
				MessageBox.Show("DBファイル または par.exeの場所が" +
								"正しく指定されていないようです。\n" +
								"(ファイル設定の見直しをお願いします。)",
								"Access Failure",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
				return (false);
			}

			// レコード数の確認を行い、レコード数分データを読み込む
			if (nofRecords == 0)
			{
				// レコードがなかったから読み込まない...
				return (true);
			}

			// 一時ファイル名の指定
			string tempFileName = mDbName + ".temp";

			// レコードのある数分読み込んでオブジェクトに変換、登録する
			for (int loop = 0; loop < nofRecords; loop++)
			{
				try
				{
					// par.exeを使用して1レコードをファイルに展開する
					if (mDbAccessor.pickUpRecord(loop, tempFileName) != false)
					{
						// データ記録用レコードを生成
						logToken entryRecord = new logToken();

						// レコードを解析する
						mDbParser.inputRecordFromTemporaryFile(tempFileName, ref entryRecord);

						// レコードをリストに登録する
						mDbArrayList.Add(entryRecord);
					}
				}
				catch
				{
					// 変換に失敗しても何もしない...
				}
			}

			try
			{
				// 一時ファイルを削除する
				File.Delete(tempFileName);
			}
			catch
			{
				// 削除失敗...でも何もしない...
			}
			return (true);
		}

		/// <summary>
		///   参照ログＤＢのレコード数を返す
		/// </summary>
		public int numberOfRecords()
		{
			return (mDbArrayList.Count);
		}

		/// <summary>
		///   レコードの情報を取得する
		/// </summary>
		public logToken getRecordData(int aIndexNumber)
		{
			if ((aIndexNumber < 0)&&
				(aIndexNumber >= mDbArrayList.Count))
			{
				return (null);
			}
			return ((logToken) mDbArrayList[aIndexNumber]);
		}

		/// <summary>
		///    新規レコードの追加
		/// </summary>
		public logToken newRecordData()
		{
			// ログレコードを生成
			logToken entryRecord = new logToken();

			// 新しいレコードオブジェクトをリストに登録する
			mDbArrayList.Add(entryRecord);

			return (entryRecord);
		}

		/// <summary>
		///   レコードを１件削除する
		/// </summary>
		public void removeRecord(int aIndexNumber)
		{
			logToken targetRecord = (logToken) mDbArrayList[aIndexNumber];

			mDbArrayList.Remove(targetRecord);
			return;
		}

		/// <summary>
		///   ログレコード群を保存する
		/// </summary>
		public bool saveDatabase(bool aOverWrite)
		{
			if (aOverWrite != true)
			{
				try
				{
					fileSelectionDialog.CheckFileExists = false;
					fileSelectionDialog.FileName = mDbName;
					if (fileSelectionDialog.ShowDialog() == DialogResult.Cancel)
					{
						// ファイル出力の中断...
						return (false);
					}
				}
				catch
				{
					// 何もしない...
				}
			}
			else
			{
				// ファイル名はＤＢ名として指定されたもの...
				fileSelectionDialog.FileName = mDbName;
			}

			// 一時ファイル名の指定
			string tempFileName = mDbName + ".temp";

			// 保存するファイル名の指定
			string targetFileName = fileSelectionDialog.FileName;

			// ファイルが存在した場合には、renameして保管する
			if (File.Exists(targetFileName) == true)
			{
				try
				{
					File.Move(targetFileName, targetFileName + ".BAK");
				}
				catch
				{
					// エラーになったときは何もしない
				}
			}

			// データベースファイルを新規に作成する
			try
			{
				mDbAccessor.createDatabase(targetFileName,
										  mDbParser.getDbName(),
										  mDbParser.getDbType(),
										  mDbParser.getDbCreatorName(),
										  mDbParser.getDbVersion());
			}
			catch
			{
				// DBファイル新規作成に失敗...
				MessageBox.Show("DBファイルの作成に失敗しました。",
								"ファイル作成失敗",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
				return (false);
			}

			// ループして、保持しているオブジェクト全て（全レコード）を出力
			for (int loop = 0; loop < mDbArrayList.Count; loop++)
			{
				logToken theData = getRecordData(loop);
				try
				{
					// 一時ファイルに出力後、DBファイルに書き出す
					mDbParser.outputRecordToTemporaryFile(tempFileName, ref theData);
					mDbAccessor.appendRecordToDatabase(targetFileName, tempFileName);
				}
				catch
				{
					// 出力失敗、、、でも何もしない...
				}
			}
			try
			{
				// 一時ファイルを削除する
				File.Delete(tempFileName);
			}
			catch
			{
				// 削除失敗...でも何もしない...
			}
			return (true);
		}
	}
}

