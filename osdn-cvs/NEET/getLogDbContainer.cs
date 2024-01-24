/*
 *  $Id: getLogDbContainer.cs,v 1.6 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの参照ログデータベースの記録場所...
 *
 *    getLogDbContainer()				： コンストラクタ
 *    setGetLogAccessor()				： PalmDBのアクセサを取得する
 *    initializeDatas()					： DBファイルからオブジェクトに展開
 *    numberOfGetLogRecords()			： 現在の参照レコード数を取得
 *    getRecordData()					： オブジェクトを応答
 *    newRecordData()					： 新規のオブジェクトを応答
 *    selectLogDatabaseFileNameToSave()	： 保存用ＤＢファイル名を入力
 *    saveGetLogDatabase()				： レコードをDBファイルに保存
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
	///  ログデータのコンテナの説明
	/// </summary>
	public class getLogDbContainer : System.Windows.Forms.UserControl
	{
		/// <summary>
		///   必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container     components = null;
		private System.Windows.Forms.Label          label1;
		private System.Windows.Forms.OpenFileDialog fileSelectionDialog;


		// PalmOSのDBファイルのアクセサ(par.exeのwrapperクラス)
		private PalmDatabaseAccessor mLogUrlDb             = null;

		// 参照ログＤＢのレコードと一時ファイルの変換
		private GetLogDbParser	mGetLogDbParser = null;

		// 参照ログＤＢのレコード数
		private int                  mNumberOfLogDbRecords = 0;

		// 参照ログＤＢの入力ファイル名
		private string               mLogUrlDbName         = null;

		// 参照ログＤＢの出力ファイル名
		private string               mDbFileNameToSave     = "";

		// 参照ログＤＢのオブジェクト管理用リスト
		private ArrayList		mGetLogDatabaseArrayList = null;

		/// <summary>
		///   コンストラクタ
		/// </summary>
		public getLogDbContainer()
		{
			// この呼び出しは，Windows.Forms デザイナで必要です。
			InitializeComponent();

			//  オブジェクトの初期化...
			mGetLogDatabaseArrayList = new ArrayList();
			mGetLogDbParser          = new GetLogDbParser();
		}

		/// <summary>
		///   使用されているリソースの後処理を実行します。
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
					components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region コンポーネント デザイナで生成されたコード
		/// <summary>
		/// デザイナサポートに必要なメソッドです。このメソッドの内容を 
		/// コードエディタで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.label1 = new System.Windows.Forms.Label();
			this.fileSelectionDialog = new System.Windows.Forms.OpenFileDialog();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(0, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(184, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "参照ログのレコードコンテナクラス";
			// 
			// getLogDbContainer
			// 
			this.Controls.Add(this.label1);
			this.Name = "getLogDbContainer";
			this.Size = new System.Drawing.Size(168, 32);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///	  参照ログＤＢのアクセスクラスを設定する
		/// </summary>
		public void setGetLogAccessor(string aDbName, ref PalmDatabaseAccessor aLogUrlDb)
		{
			mNumberOfLogDbRecords   = 0;
			mLogUrlDb               = aLogUrlDb;
			mLogUrlDbName           = aDbName;
			mDbFileNameToSave       = mLogUrlDbName;
			return;
		}

		/// <summary>
		///    データコンテナクラスを初期化する
		///    (DBファイルからデータを読み出してオブジェクトに展開する)
		/// </summary>
		public bool initializeDatas()
		{
			// ＤＢのヘッダ情報を読んでレコード数を取得する
			if (mLogUrlDb.getRecordNumber(out mNumberOfLogDbRecords) == false)
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
			if (mNumberOfLogDbRecords == 0)
			{
				// レコードがなかったから読み込まない...
				return (true);
			}

			// 一時ファイル名の指定
			string tempFileName = mLogUrlDbName + ".temp";

			// レコードのある数分読み込んでオブジェクトに変換、登録する
			for (int loop = 0; loop < mNumberOfLogDbRecords; loop++)
			{
				try
				{
					// par.exeを使用して1レコードをファイルに展開する
					if (mLogUrlDb.pickUpRecord(loop, tempFileName) != false)
					{
						// ログレコードを生成
						logRecord entryRecord = new logRecord();

						// レコードを解析する
						mGetLogDbParser.inputRecordFromTemporaryFile(tempFileName, ref entryRecord);

						// レコードをリストに登録する
						mGetLogDatabaseArrayList.Add(entryRecord);
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
		public int numberOfGetLogRecords()
		{
			return (mGetLogDatabaseArrayList.Count);
		}

		/// <summary>
		///   レコードの情報を取得する
		/// </summary>
		public logRecord getRecordData(int aIndexNumber)
		{
			if ((aIndexNumber < 0)&&
				(aIndexNumber >= mGetLogDatabaseArrayList.Count))
			{
				return (null);
			}
			return ((logRecord) mGetLogDatabaseArrayList[aIndexNumber]);
		}

		/// <summary>
		///    新規レコードの追加
		/// </summary>
		public logRecord newRecordData()
		{
			// ログレコードを生成
			logRecord entryRecord = new logRecord();

			// 新しいレコードオブジェクトをリストに登録する
			mGetLogDatabaseArrayList.Add(entryRecord);

			return (entryRecord);
		}

		/// <summary>
		///   レコードを１件削除する
		/// </summary>
		public void removeRecord(int aIndexNumber)
		{
			logRecord targetRecord = (logRecord) mGetLogDatabaseArrayList[aIndexNumber];

			mGetLogDatabaseArrayList.Remove(targetRecord);
			return;
		}

		/// <summary>
		///   保存用のファイル名を指定する
		/// </summary>
		public void selectLogDatabaseFileNameToSave()
		{
			try
			{
				fileSelectionDialog.CheckFileExists = false;
				fileSelectionDialog.FileName = mDbFileNameToSave;
				fileSelectionDialog.ShowDialog();
				mDbFileNameToSave = fileSelectionDialog.FileName;
			}
			catch
			{
				// 何もしない...
			}

		}

		/// <summary>
		///   ログレコード群を保存する
		/// </summary>
		public bool saveGetLogDatabase()
		{
			// 一時ファイル名の指定
			string tempFileName = mLogUrlDbName + ".temp";

			// ファイルが存在した場合には、renameして保管する
			if (File.Exists(mDbFileNameToSave) == true)
			{
				try
				{
					File.Move(mDbFileNameToSave, mDbFileNameToSave + ".BAK");
				}
				catch
				{
					// エラーになったときは何もしない
				}
			}

			// データベースファイルを新規に作成する
			try
			{
				mLogUrlDb.createDatabase(mDbFileNameToSave,
										 mGetLogDbParser.getDbName(),
										 mGetLogDbParser.getDbType(),
										 mGetLogDbParser.getDbCreatorName(),
										 mGetLogDbParser.getDbVersion());
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
			for (int loop = 0; loop < mGetLogDatabaseArrayList.Count; loop++)
			{
				logRecord theData = getRecordData(loop);
				try
				{
					// 一時ファイルに出力後、DBファイルに書き出す
					mGetLogDbParser.outputRecordToTemporaryFile(tempFileName, ref theData);
					mLogUrlDb.appendRecordToDatabase(mDbFileNameToSave, tempFileName);
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

