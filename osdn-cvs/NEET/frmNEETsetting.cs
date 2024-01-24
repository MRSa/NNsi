/*
 *  $Id: frmNEETsetting.cs,v 1.10 2005/07/18 04:33:19 mrsa Exp $
 *    --- NNsiのDBファイル、par.exeの場所を指定してもらうための処理
 *
 *
 */
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace NEET
{
	/// <summary>
	///  frmNEETsetting の概要の説明
	///    NNsiのDBファイル、par.exeの場所を入力、提供するクラスです。
	/// </summary>
	public class frmNEETsetting : System.Windows.Forms.Form
	{
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button btnSelectGetLogDB;
		private System.Windows.Forms.TextBox parExeLocation;
		private System.Windows.Forms.Button btnSelectParExe;
		private System.Windows.Forms.GroupBox settings;
		private System.Windows.Forms.Label lblDescription;
		private System.Windows.Forms.OpenFileDialog selectFileDialog;
		private System.Windows.Forms.Button btnEditToken;
		private System.Windows.Forms.Button btnNGword;
		private System.Windows.Forms.Button btnBBS;
		private System.Windows.Forms.Label lblVersion;

		public frmNEETsetting()
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent 呼び出しの後のコンストラクタコードを追加
			//
			//
			//   (今のところ、特に必要なし)
			//
		}

		/// <summary>
		/// 使用されているリソースの後処理を実行します。
		/// </summary>
		protected override void Dispose (bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		#region Windows フォームデザイナで生成されたコード
		/// <summary>
		/// デザイナサポートに必要なメソッドです。このメソッドの内容を
		/// コードエディタで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.btnSelectGetLogDB = new System.Windows.Forms.Button();
			this.parExeLocation = new System.Windows.Forms.TextBox();
			this.btnSelectParExe = new System.Windows.Forms.Button();
			this.selectFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.settings = new System.Windows.Forms.GroupBox();
			this.lblDescription = new System.Windows.Forms.Label();
			this.btnEditToken = new System.Windows.Forms.Button();
			this.btnNGword = new System.Windows.Forms.Button();
			this.btnBBS = new System.Windows.Forms.Button();
			this.lblVersion = new System.Windows.Forms.Label();
			this.settings.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnSelectGetLogDB
			// 
			this.btnSelectGetLogDB.Location = new System.Drawing.Point(40, 32);
			this.btnSelectGetLogDB.Name = "btnSelectGetLogDB";
			this.btnSelectGetLogDB.Size = new System.Drawing.Size(152, 40);
			this.btnSelectGetLogDB.TabIndex = 12;
			this.btnSelectGetLogDB.Text = "参照ログDBの編集...   (LogPermanent-NNsi.pdb)";
			this.btnSelectGetLogDB.Click += new System.EventHandler(this.btnSelectGetLogDB_Click);
			// 
			// parExeLocation
			// 
			this.parExeLocation.Location = new System.Drawing.Point(8, 24);
			this.parExeLocation.Name = "parExeLocation";
			this.parExeLocation.Size = new System.Drawing.Size(312, 19);
			this.parExeLocation.TabIndex = 9;
			this.parExeLocation.Text = "C:\\APL\\par.exe";
			// 
			// btnSelectParExe
			// 
			this.btnSelectParExe.Location = new System.Drawing.Point(328, 22);
			this.btnSelectParExe.Name = "btnSelectParExe";
			this.btnSelectParExe.Size = new System.Drawing.Size(48, 23);
			this.btnSelectParExe.TabIndex = 8;
			this.btnSelectParExe.Text = "参照...";
			this.btnSelectParExe.Click += new System.EventHandler(this.btnSelectParExe_Click);
			// 
			// settings
			// 
			this.settings.Controls.Add(this.btnSelectParExe);
			this.settings.Controls.Add(this.parExeLocation);
			this.settings.Location = new System.Drawing.Point(8, 144);
			this.settings.Name = "settings";
			this.settings.Size = new System.Drawing.Size(392, 56);
			this.settings.TabIndex = 11;
			this.settings.TabStop = false;
			this.settings.Text = "par.exeの場所";
			// 
			// lblDescription
			// 
			this.lblDescription.Font = new System.Drawing.Font("MS UI Gothic", 10F);
			this.lblDescription.Location = new System.Drawing.Point(8, 8);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.Size = new System.Drawing.Size(400, 16);
			this.lblDescription.TabIndex = 16;
			this.lblDescription.Text = "※  par.exeがある場所を設定し、ＤＢ編集ボタンを押してください。  ※";
			this.lblDescription.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// btnEditToken
			// 
			this.btnEditToken.Location = new System.Drawing.Point(40, 88);
			this.btnEditToken.Name = "btnEditToken";
			this.btnEditToken.Size = new System.Drawing.Size(152, 40);
			this.btnEditToken.TabIndex = 17;
			this.btnEditToken.Text = "区切り文字DBの編集...   (LogToken-NNsi.pdb)";
			this.btnEditToken.Click += new System.EventHandler(this.btnEditToken_Click);
			// 
			// btnNGword
			// 
			this.btnNGword.Location = new System.Drawing.Point(216, 88);
			this.btnNGword.Name = "btnNGword";
			this.btnNGword.Size = new System.Drawing.Size(152, 40);
			this.btnNGword.TabIndex = 18;
			this.btnNGword.Text = "NGワードDBの編集...   (NGWord-NNsi.pdb)";
			this.btnNGword.Click += new System.EventHandler(this.btnNGword_Click);
			// 
			// btnBBS
			// 
			this.btnBBS.Location = new System.Drawing.Point(216, 32);
			this.btnBBS.Name = "btnBBS";
			this.btnBBS.Size = new System.Drawing.Size(152, 40);
			this.btnBBS.TabIndex = 19;
			this.btnBBS.Text = "板(BBS)一覧DBの編集...   (BBS-NNsi.pdb)";
			this.btnBBS.Click += new System.EventHandler(this.btnBBS_Click);
			// 
			// lblVersion
			// 
			this.lblVersion.Enabled = false;
			this.lblVersion.Location = new System.Drawing.Point(152, 208);
			this.lblVersion.Name = "lblVersion";
			this.lblVersion.Size = new System.Drawing.Size(248, 16);
			this.lblVersion.TabIndex = 20;
			this.lblVersion.Text = "NEET (Phase2) Version 0.21 Revision000";
			this.lblVersion.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// frmNEETsetting
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(416, 229);
			this.Controls.Add(this.lblVersion);
			this.Controls.Add(this.btnBBS);
			this.Controls.Add(this.btnNGword);
			this.Controls.Add(this.btnEditToken);
			this.Controls.Add(this.settings);
			this.Controls.Add(this.btnSelectGetLogDB);
			this.Controls.Add(this.lblDescription);
			this.Name = "frmNEETsetting";
			this.Text = "NEET  -- a NNsi\'s Expanding and Editing Tool ";
			this.settings.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   par.exe のある場所を選択する (ファイル選択ダイアログを表示)
		/// </summary>
		private void btnSelectParExe_Click(object sender, System.EventArgs e)
		{
			selectFileDialog.ShowDialog();
			parExeLocation.Text = selectFileDialog.FileName;
			return;
		}

		/// <summary>
		///   参照ログＤＢのある場所を選択し、編集を開始する
		/// </summary>
		private void btnSelectGetLogDB_Click(object sender, System.EventArgs e)
		{
			// 参照ログDBのファイル選択ダイアログを表示する
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// キャンセル時には何もしない...
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// 参照ログアクセスクラスを生成し、データコンテナクラスに渡す
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// データコンテナクラスの初期化
			getLogDbContainer logContainer = new getLogDbContainer();
			logContainer.setGetLogAccessor(targetFileName, ref dbAccessor);
			if (logContainer.initializeDatas() != true)
			{
				// データクラスの初期化に失敗した、、、終了する
				return;
			}

			////////////////////////////////////////////////////////////////
			//   一覧ダイアログを生成して開く処理
			////////////////////////////////////////////////////////////////

			// 一覧ダイアログを生成する
			frmLogList listDialog = new frmLogList(this);

			// ログコンテナクラスを一覧ダイアログに渡す
			listDialog.setLogContainer(ref logContainer);

			// データセットを準備する
			listDialog.prepareDataset();

			// 参照ログＤＢ編集ダイアログを開く
			//  (編集終了まで待つ、その間同じボタンが押されないようにする)
			getLogButtonControl(false);
			listDialog.ShowDialog();
			getLogButtonControl(true);

			return;
		}

		/// <summary>
		///   Par.Exeのファイル名を応答する （絶対パス名)
		/// </summary>
		public string getParExeLocation()
		{
			return (parExeLocation.Text);
		}

		///  <summary>
		///	ボタン制御を有効にする
		///  </summary>
		public void getLogButtonControl(bool aTF)
		{
			btnSelectGetLogDB.Enabled = aTF;
		}


		/// <summary>
		///   区切り文字ＤＢの編集...
		/// </summary>
		private void btnEditToken_Click(object sender, System.EventArgs e)
		{
			// 区切り文字DBのファイル選択ダイアログを表示する
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// キャンセルされたときには何もしない。
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// 区切り文字ＤＢアクセスクラスを生成し、データコンテナクラスに渡す
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// データコンテナクラスの初期化
			logTokenDbContainer tokenDbContainer = new logTokenDbContainer(targetFileName, ref dbAccessor);
			if (tokenDbContainer.initializeDatas() != true)
			{
				// データクラスの初期化に失敗した、、、終了する
				return;
			}

			// 一覧ダイアログを生成する
			logTokenList listDialog = new logTokenList(this, ref tokenDbContainer);

			// データセットを準備する
			listDialog.prepareDataset();

			// 区切り文字編集ダイアログを開く
			//  (編集終了まで待つ、その間同じボタンが押されないようにする)
			btnEditToken.Enabled = false;
			listDialog.ShowDialog();
			btnEditToken.Enabled = true;

			return;
		}

		/// <summary>
		/// </summary>
		private void btnBBS_Click(object sender, System.EventArgs e)
		{
			// 区切り文字DBのファイル選択ダイアログを表示する
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// キャンセルされたときには何もしない。
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// BBS
			btnBBS.Enabled = false;

			// 区切り文字ＤＢアクセスクラスを生成し、データコンテナクラスに渡す
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// データコンテナクラスの初期化
			bbsDbContainer bbsContainer = new bbsDbContainer(targetFileName, ref dbAccessor);
			if (bbsContainer.initializeDatas() != true)
			{
				// データクラスの初期化に失敗した、、、終了する
				return;
			}

			// 一覧ダイアログを生成する
			bbsList listDialog = new bbsList(this, ref bbsContainer);

			// データセットを準備する
			listDialog.prepareDataset();

			//frmBBSselectURL bbsSettingDialog = new frmBBSselectURL(parExeLocation.Text);

			listDialog.ShowDialog();
			btnBBS.Enabled = true;
		}

		/// <summary>
		/// </summary>
		private void btnNGword_Click(object sender, System.EventArgs e)
		{
			// NGワードDBのファイル選択ダイアログを表示する
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// キャンセルされたときには何もしない。
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// NGワードＤＢアクセスクラスを生成し、データコンテナクラスに渡す
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// データコンテナクラスの初期化
			NGwordContainer ngWordContainer = new NGwordContainer(targetFileName, ref dbAccessor);
			if (ngWordContainer.initializeDatas() != true)
			{
				// データクラスの初期化に失敗した、、、終了する
				return;
			}

			// 一覧ダイアログを生成する
			frmNGwordEdit listDialog = new frmNGwordEdit(this, ref ngWordContainer);

			// データセットを準備する
			listDialog.prepareDataset();

			// 区切り文字編集ダイアログを開く
			//  (編集終了まで待つ、その間同じボタンが押されないようにする)
			btnNGword.Enabled = false;
			listDialog.ShowDialog();
			btnNGword.Enabled = true;
		}
	}

/*****

#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16
#define MAX_THREADFILENAME          32
#define MAX_GETLOG_URL             244

// 板情報(データベースの構造)
typedef struct {
	Char   boardNick[MAX_NICKNAME];    // ボードニックネーム
	Char   boardURL [MAX_URL];         // ボードURL
	Char   boardName[MAX_BOARDNAME];   // ボード名
	UInt8  bbsType;                    // ボードTYPE
	UInt8  state;                      // ボード使用状態
	Int16  reserved;                   // (予約領域：以前は使用していた)
} NNshBoardDatabase;

// 参照ログ（１つ下ファイル）の構造体
typedef struct {
	Char   dataURL     [MAX_GETLOG_URL];      // データ取得先URL
	Char   dataFileName[MAX_THREADFILENAME];  // データファイル名
	UInt32 lastUpdate;                        // 最終更新時間
	UInt32 parentDbId;                        // 親のレコードID(ユニークID)
	UInt32 logDbId;                           // ログのレコードID(ユニークID)
	UInt32 tokenId;                           // 区切り文字のレコードID(ユニークID)
	UInt32 reserve0;                          // 予約領域
	UInt32 reserve1;                          // 予約領域
	UInt32 reserve2;                          // 予約領域
	UInt32 reserve3;                          // 予約領域
} NNshGetLogSubFileDatabase;

*****/

}
