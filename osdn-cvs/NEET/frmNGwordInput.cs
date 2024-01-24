/*
 *  $Id: frmNGwordInput.cs,v 1.1 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiのNGワード編集フォーム
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
	///   frmNGwordInput の概要の説明
	/// </summary>
	public class frmNGwordInput : System.Windows.Forms.Form
	{
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Label lblNGword;
		private System.Windows.Forms.TextBox fldNGword;
		private System.Windows.Forms.CheckBox chkReplace;
		private System.Windows.Forms.GroupBox grpAction;
		private System.Windows.Forms.RadioButton chkWhole;
		private System.Windows.Forms.RadioButton chkHeader;
		private System.Windows.Forms.RadioButton grpExceptMsg;
		private System.Windows.Forms.RadioButton grpMsg;
		private System.Windows.Forms.Button btnEntryData;
		private System.Windows.Forms.Button btnCancelEdit;
		private System.Windows.Forms.Button btnDeleteRecord;
		private System.Windows.Forms.Label lblRecordNumber;
		private System.Windows.Forms.Label lblDescription;

		private frmNGwordEdit   mParent          = null;
		private NGwordContainer mContainer       = null;
		private NGwordRecord    mEditRecord      = null;
		private int				mMaxRecordNumber = 0;
		private int             mIndexNumber     = 0;

		/// <summary>
		///   コンストラクタ
		/// </summary>
		public frmNGwordInput(frmNGwordEdit aParent, ref NGwordContainer arDbContainer)
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent 呼び出しの後のコンストラクタコードを追加
			//
			mParent          = aParent;
			mContainer       = arDbContainer;
			mMaxRecordNumber = mContainer.numberOfRecords();
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
			this.lblNGword = new System.Windows.Forms.Label();
			this.fldNGword = new System.Windows.Forms.TextBox();
			this.chkReplace = new System.Windows.Forms.CheckBox();
			this.grpAction = new System.Windows.Forms.GroupBox();
			this.grpMsg = new System.Windows.Forms.RadioButton();
			this.grpExceptMsg = new System.Windows.Forms.RadioButton();
			this.chkHeader = new System.Windows.Forms.RadioButton();
			this.chkWhole = new System.Windows.Forms.RadioButton();
			this.btnEntryData = new System.Windows.Forms.Button();
			this.btnCancelEdit = new System.Windows.Forms.Button();
			this.btnDeleteRecord = new System.Windows.Forms.Button();
			this.lblRecordNumber = new System.Windows.Forms.Label();
			this.lblDescription = new System.Windows.Forms.Label();
			this.grpAction.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblNGword
			// 
			this.lblNGword.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lblNGword.Location = new System.Drawing.Point(8, 8);
			this.lblNGword.Name = "lblNGword";
			this.lblNGword.Size = new System.Drawing.Size(56, 16);
			this.lblNGword.TabIndex = 0;
			this.lblNGword.Text = "NGワード";
			// 
			// fldNGword
			// 
			this.fldNGword.Location = new System.Drawing.Point(64, 5);
			this.fldNGword.Name = "fldNGword";
			this.fldNGword.Size = new System.Drawing.Size(360, 19);
			this.fldNGword.TabIndex = 1;
			this.fldNGword.Text = "(NGワードを入力)";
			// 
			// chkReplace
			// 
			this.chkReplace.Location = new System.Drawing.Point(32, 152);
			this.chkReplace.Name = "chkReplace";
			this.chkReplace.Size = new System.Drawing.Size(296, 24);
			this.chkReplace.TabIndex = 2;
			this.chkReplace.Text = "NGワードを検出した場合、NGワードだけを * に置換する";
			// 
			// grpAction
			// 
			this.grpAction.Controls.Add(this.grpMsg);
			this.grpAction.Controls.Add(this.grpExceptMsg);
			this.grpAction.Controls.Add(this.chkHeader);
			this.grpAction.Controls.Add(this.chkWhole);
			this.grpAction.Location = new System.Drawing.Point(8, 32);
			this.grpAction.Name = "grpAction";
			this.grpAction.Size = new System.Drawing.Size(416, 120);
			this.grpAction.TabIndex = 3;
			this.grpAction.TabStop = false;
			this.grpAction.Text = "NGワードを検出する範囲";
			// 
			// grpMsg
			// 
			this.grpMsg.Location = new System.Drawing.Point(24, 88);
			this.grpMsg.Name = "grpMsg";
			this.grpMsg.Size = new System.Drawing.Size(368, 24);
			this.grpMsg.TabIndex = 3;
			this.grpMsg.Text = "MSG欄 (本文だけでNGワードをチェックする)";
			// 
			// grpExceptMsg
			// 
			this.grpExceptMsg.Location = new System.Drawing.Point(24, 64);
			this.grpExceptMsg.Name = "grpExceptMsg";
			this.grpExceptMsg.Size = new System.Drawing.Size(368, 24);
			this.grpExceptMsg.TabIndex = 2;
			this.grpExceptMsg.Text = "MSG欄以外 (本文以外の部分でNGワードをチェックする)";
			// 
			// chkHeader
			// 
			this.chkHeader.Location = new System.Drawing.Point(24, 40);
			this.chkHeader.Name = "chkHeader";
			this.chkHeader.Size = new System.Drawing.Size(368, 24);
			this.chkHeader.TabIndex = 1;
			this.chkHeader.Text = "名前欄/メール欄 (名前欄とメール欄だけNGワードをチェックする)";
			// 
			// chkWhole
			// 
			this.chkWhole.Checked = true;
			this.chkWhole.Location = new System.Drawing.Point(24, 16);
			this.chkWhole.Name = "chkWhole";
			this.chkWhole.Size = new System.Drawing.Size(368, 24);
			this.chkWhole.TabIndex = 0;
			this.chkWhole.TabStop = true;
			this.chkWhole.Text = "レス全体 (名前欄、メール欄、本文すべてでNGワードをチェックする)";
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 208);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 18;
			this.btnEntryData.Text = "データ登録";
			this.btnEntryData.Click += new System.EventHandler(this.btnEntryData_Click);
			// 
			// btnCancelEdit
			// 
			this.btnCancelEdit.Location = new System.Drawing.Point(120, 208);
			this.btnCancelEdit.Name = "btnCancelEdit";
			this.btnCancelEdit.Size = new System.Drawing.Size(104, 32);
			this.btnCancelEdit.TabIndex = 19;
			this.btnCancelEdit.Text = "編集中止";
			this.btnCancelEdit.Click += new System.EventHandler(this.btnCancelEdit_Click);
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(304, 208);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 20;
			this.btnDeleteRecord.Text = "レコード削除";
			this.btnDeleteRecord.Click += new System.EventHandler(this.btnDeleteRecord_Click);
			// 
			// lblRecordNumber
			// 
			this.lblRecordNumber.Location = new System.Drawing.Point(344, 160);
			this.lblRecordNumber.Name = "lblRecordNumber";
			this.lblRecordNumber.Size = new System.Drawing.Size(80, 16);
			this.lblRecordNumber.TabIndex = 21;
			this.lblRecordNumber.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// lblDescription
			// 
			this.lblDescription.Location = new System.Drawing.Point(48, 178);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.Size = new System.Drawing.Size(313, 16);
			this.lblDescription.TabIndex = 22;
			this.lblDescription.Text = "(通常は、NGワードを検出するとレス全体を表示させません。)";
			// 
			// frmNGwordInput
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(432, 245);
			this.Controls.Add(this.lblDescription);
			this.Controls.Add(this.lblRecordNumber);
			this.Controls.Add(this.btnDeleteRecord);
			this.Controls.Add(this.btnCancelEdit);
			this.Controls.Add(this.btnEntryData);
			this.Controls.Add(this.grpAction);
			this.Controls.Add(this.chkReplace);
			this.Controls.Add(this.fldNGword);
			this.Controls.Add(this.lblNGword);
			this.Name = "frmNGwordInput";
			this.Text = "NGワード編集";
			this.grpAction.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   表示するレコード番号を（外部から）指定されたときの処理
		/// </summary>
		public void updateScreenData(int aIndexNumber)
		{
			// レコード番号の表示
			mIndexNumber = aIndexNumber;
			lblRecordNumber.Text = (aIndexNumber + 1) + " / " + mMaxRecordNumber;

			mEditRecord = mContainer.getRecordData(aIndexNumber);

			// フィールドにデータを反映させる

			// NGワード
			fldNGword.Text			= mEditRecord.ngWord;

			// チェックする領域の指定
			switch (mEditRecord.checkArea)
			{
			  case 1:
				// 名前欄/メール欄をチェックする
				chkWhole.Checked     = false;
				chkHeader.Checked    = true;
				grpExceptMsg.Checked = false;
				grpMsg.Checked       = false;
				break;

			  case 2:
				// MSG欄以外をチェックする
				chkWhole.Checked     = false;
				chkHeader.Checked    = false;
				grpExceptMsg.Checked = true;
				grpMsg.Checked       = false;
				break;

			  case 3:
				// MSG欄だけをチェックする
				chkWhole.Checked     = false;
				chkHeader.Checked    = false;
				grpExceptMsg.Checked = false;
				grpMsg.Checked       = true;
				break;

			  case 0:
			  default:
				// レス全体をチェックする
				chkWhole.Checked     = true;
				chkHeader.Checked    = false;
				grpExceptMsg.Checked = false;
				grpMsg.Checked       = false;
				break;
			}

			// マッチしたときに実施する処理
			if (mEditRecord.matchedAction == 0)
			{
				// レス全体を表示しない
				chkReplace.Checked = false;
			}
			else
			{
				// NGワードだけを * に置換する
				chkReplace.Checked = true;
			}
			return;
		}

		/// <summary>
		///   編集中止ボタンを押した時の処理
		/// </summary>
		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// 画面を閉じる
			this.Close();

			return;
		}
		
		private void btnDeleteRecord_Click(object sender, System.EventArgs e)
		{
			if (MessageBox.Show("表示中のレコードを削除します。",
								"レコード削除の確認",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// なにもせず終了する
				return;
			}

			// 表示中のレコードを削除する
			mContainer.removeRecord(mIndexNumber);

			// 親画面データを更新する
			mParent.refreshDataTable();

			// 画面を閉じる
			this.Close();

			return;
		}
		
		private void btnEntryData_Click(object sender, System.EventArgs e)
		{
			// 現在表示中のレコードを更新(保存)する...
			updateRecord();

			// 画面を閉じる
			this.Close();
		}

		/// <summary>
		///   現在表示中のレコードを記録する
		/// </summary>
		private void updateRecord()
		{
			NGwordRecord targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// 新しいレコードオブジェクトを取得する
				targetRecord      = mContainer.newRecordData();
				mMaxRecordNumber  = mContainer.numberOfRecords();
			}
			else
			{
				// 現在の表示中のレコードオブジェクトを取得する
				targetRecord = mContainer.getRecordData(mIndexNumber);
			}

			// ログデータを登録する...


			// NGワード
			targetRecord.ngWord = fldNGword.Text;

			// チェック領域の設定
			if (chkHeader.Checked == true)
			{
				targetRecord.checkArea = 1;
			}
			else if (grpExceptMsg.Checked == true)
			{
				targetRecord.checkArea = 2;
			}
			else if (grpMsg.Checked == true)
			{
				targetRecord.checkArea = 3;
			}
			else
			{
				targetRecord.checkArea = 0;
			}

			// 置換モードかどうか？
			if (chkReplace.Checked == true)
			{
				// NGワード置換モード
				targetRecord.matchedAction = 1;
			}
			else
			{
				// レス消去モード
				targetRecord.matchedAction = 0;
			}

			// 親の画面データを更新する
			mParent.refreshDataTable();

			return;
		}
	}
}
