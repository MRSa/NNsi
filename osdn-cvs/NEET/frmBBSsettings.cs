/*
 *  $Id: frmBBSsettings.cs,v 1.4 2005/07/18 04:33:20 mrsa Exp $
 *    --- 板一覧の取得設定...  (これから作成)
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
	///  frmBBSselectURL の概要の説明
	/// </summary>
	public class frmBBSselectURL : System.Windows.Forms.Form
	{
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.OpenFileDialog openPdbDialog;
		private System.Windows.Forms.GroupBox grpPdbFile;
		private System.Windows.Forms.TextBox fldBBSfile;
		private System.Windows.Forms.Button btnSelection;
		private System.Windows.Forms.Button btnEdit;
		private System.Windows.Forms.Button btnUpdate;
		private System.Windows.Forms.GroupBox grpBBSurl;
		private System.Windows.Forms.RadioButton chkNormalURL;
		private System.Windows.Forms.RadioButton chkOldURL;
		private System.Windows.Forms.RadioButton radioButton2;
		private System.Windows.Forms.TextBox fldOther;

		//private PalmDatabaseAccessor	mDbAccessor = null;
		private string					mParExeFileName;

		public frmBBSselectURL(string aParExeFileName)
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent 呼び出しの後のコンストラクタコードを追加
			//
			mParExeFileName = aParExeFileName;
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
			this.openPdbDialog = new System.Windows.Forms.OpenFileDialog();
			this.grpPdbFile = new System.Windows.Forms.GroupBox();
			this.btnSelection = new System.Windows.Forms.Button();
			this.fldBBSfile = new System.Windows.Forms.TextBox();
			this.btnEdit = new System.Windows.Forms.Button();
			this.btnUpdate = new System.Windows.Forms.Button();
			this.grpBBSurl = new System.Windows.Forms.GroupBox();
			this.fldOther = new System.Windows.Forms.TextBox();
			this.radioButton2 = new System.Windows.Forms.RadioButton();
			this.chkOldURL = new System.Windows.Forms.RadioButton();
			this.chkNormalURL = new System.Windows.Forms.RadioButton();
			this.grpPdbFile.SuspendLayout();
			this.grpBBSurl.SuspendLayout();
			this.SuspendLayout();
			// 
			// grpPdbFile
			// 
			this.grpPdbFile.Controls.Add(this.btnSelection);
			this.grpPdbFile.Controls.Add(this.fldBBSfile);
			this.grpPdbFile.Location = new System.Drawing.Point(8, 144);
			this.grpPdbFile.Name = "grpPdbFile";
			this.grpPdbFile.Size = new System.Drawing.Size(576, 40);
			this.grpPdbFile.TabIndex = 0;
			this.grpPdbFile.TabStop = false;
			this.grpPdbFile.Text = "板一覧のDBファイル";
			// 
			// btnSelection
			// 
			this.btnSelection.Location = new System.Drawing.Point(523, 12);
			this.btnSelection.Name = "btnSelection";
			this.btnSelection.Size = new System.Drawing.Size(48, 23);
			this.btnSelection.TabIndex = 1;
			this.btnSelection.Text = "参照...";
			this.btnSelection.Click += new System.EventHandler(this.btnSelection_Click);
			// 
			// fldBBSfile
			// 
			this.fldBBSfile.Location = new System.Drawing.Point(8, 16);
			this.fldBBSfile.Name = "fldBBSfile";
			this.fldBBSfile.Size = new System.Drawing.Size(512, 19);
			this.fldBBSfile.TabIndex = 0;
			this.fldBBSfile.Text = "C:\\APL\\NNsi\\BBS-NNsi.pdb";
			// 
			// btnEdit
			// 
			this.btnEdit.Location = new System.Drawing.Point(488, 192);
			this.btnEdit.Name = "btnEdit";
			this.btnEdit.Size = new System.Drawing.Size(96, 23);
			this.btnEdit.TabIndex = 1;
			this.btnEdit.Text = "板一覧の編集...";
			this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
			// 
			// btnUpdate
			// 
			this.btnUpdate.Location = new System.Drawing.Point(480, 90);
			this.btnUpdate.Name = "btnUpdate";
			this.btnUpdate.Size = new System.Drawing.Size(88, 24);
			this.btnUpdate.TabIndex = 2;
			this.btnUpdate.Text = "板URLの更新";
			this.btnUpdate.Click += new System.EventHandler(this.btnUpdate_Click);
			// 
			// grpBBSurl
			// 
			this.grpBBSurl.Controls.Add(this.fldOther);
			this.grpBBSurl.Controls.Add(this.radioButton2);
			this.grpBBSurl.Controls.Add(this.chkOldURL);
			this.grpBBSurl.Controls.Add(this.chkNormalURL);
			this.grpBBSurl.Controls.Add(this.btnUpdate);
			this.grpBBSurl.Location = new System.Drawing.Point(8, 8);
			this.grpBBSurl.Name = "grpBBSurl";
			this.grpBBSurl.Size = new System.Drawing.Size(576, 128);
			this.grpBBSurl.TabIndex = 3;
			this.grpBBSurl.TabStop = false;
			this.grpBBSurl.Text = "板一覧の取得先 (URL)";
			// 
			// fldOther
			// 
			this.fldOther.Location = new System.Drawing.Point(78, 93);
			this.fldOther.Name = "fldOther";
			this.fldOther.Size = new System.Drawing.Size(394, 19);
			this.fldOther.TabIndex = 3;
			this.fldOther.Text = "";
			// 
			// radioButton2
			// 
			this.radioButton2.Location = new System.Drawing.Point(24, 88);
			this.radioButton2.Name = "radioButton2";
			this.radioButton2.Size = new System.Drawing.Size(56, 24);
			this.radioButton2.TabIndex = 2;
			this.radioButton2.Text = "その他";
			// 
			// chkOldURL
			// 
			this.chkOldURL.Location = new System.Drawing.Point(24, 52);
			this.chkOldURL.Name = "chkOldURL";
			this.chkOldURL.Size = new System.Drawing.Size(536, 24);
			this.chkOldURL.TabIndex = 1;
			this.chkOldURL.Text = "旧URL    (http://www6.ocn.ne.jp/~mirv/bbstable.html)";
			// 
			// chkNormalURL
			// 
			this.chkNormalURL.Location = new System.Drawing.Point(24, 24);
			this.chkNormalURL.Name = "chkNormalURL";
			this.chkNormalURL.Size = new System.Drawing.Size(536, 24);
			this.chkNormalURL.TabIndex = 0;
			this.chkNormalURL.Text = "標準URL (http://www.ff.iij4u.or.jp/~ch2/bbstable.html)";
			// 
			// frmBBSselectURL
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(592, 221);
			this.Controls.Add(this.grpBBSurl);
			this.Controls.Add(this.btnEdit);
			this.Controls.Add(this.grpPdbFile);
			this.Name = "frmBBSselectURL";
			this.Text = "板一覧の編集設定";
			this.grpPdbFile.ResumeLayout(false);
			this.grpBBSurl.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///
		/// </summary>
		private void btnSelection_Click(object sender, System.EventArgs e)
		{
			openPdbDialog.ShowDialog();
			fldBBSfile.Text = openPdbDialog.FileName;
		}

		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			string targetFileName = fldBBSfile.Text;

			// ＤＢアクセスクラスを生成し、データコンテナクラスに渡す
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(mParExeFileName, targetFileName);

			// データコンテナクラスの初期化
			bbsDbContainer bbsDbContainer = new bbsDbContainer(targetFileName, ref dbAccessor);
			if (bbsDbContainer.initializeDatas() != true)
			{
				// データクラスの初期化に失敗した、、、終了する
				return;
			}

			//// 一覧ダイアログを生成する
			//bbsList listDialog = new bbsList(this, ref bbsDbContainer);
			//
			//// データセットを準備する
			//listDialog.prepareDataset();
			//
			//// 板編集ダイアログを開く
			////  (編集終了まで待つ、その間同じボタンが押されないようにする)
			//btnEdit.Enabled = false;
			//listDialog.ShowDialog();
			//btnEdit.Enabled = true;

			return;

		}

		private void btnUpdate_Click(object sender, System.EventArgs e)
		{
			//

		}
	}
}
