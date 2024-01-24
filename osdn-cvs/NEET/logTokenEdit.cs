/*
 *  $Id: logTokenEdit.cs,v 1.3 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの区切り文字列編集
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
	///  WinForm1 の概要の説明
	/// </summary>
	public class logTokenEdit : System.Windows.Forms.Form
	{
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button btnCancelEdit;
		private System.Windows.Forms.Button btnDeleteRecord;
		private System.Windows.Forms.Button btnEntryData;
		private System.Windows.Forms.TextBox fldName;
		private System.Windows.Forms.Label lblName;
		private System.Windows.Forms.GroupBox grpStart;
		private System.Windows.Forms.TextBox fldStartString;
		private System.Windows.Forms.GroupBox grpEnd;
		private System.Windows.Forms.TextBox fldEndString;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.GroupBox grpSeparator;
		private System.Windows.Forms.TextBox fldSeparatorString;
		private System.Windows.Forms.Label lblRecordNumber;

		private logTokenList		mParent			 = null;
		private logTokenDbContainer mDbContainer	 = null;
		private logToken            mEditRecord		 = null;
		private int					mIndexNumber     = 0;
		private int					mMaxRecordNumber = 0;
		private System.Windows.Forms.Label lblCaution;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label lblCaution2;

		public logTokenEdit(logTokenList aParent, ref logTokenDbContainer arDbContainer)
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent 呼び出しの後のコンストラクタコードを追加
			//
			mParent			  = aParent;
			mDbContainer	  = arDbContainer;
			mMaxRecordNumber  = mDbContainer.numberOfRecords();
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
			this.btnCancelEdit = new System.Windows.Forms.Button();
			this.btnDeleteRecord = new System.Windows.Forms.Button();
			this.btnEntryData = new System.Windows.Forms.Button();
			this.fldName = new System.Windows.Forms.TextBox();
			this.lblName = new System.Windows.Forms.Label();
			this.grpStart = new System.Windows.Forms.GroupBox();
			this.fldStartString = new System.Windows.Forms.TextBox();
			this.grpEnd = new System.Windows.Forms.GroupBox();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.fldEndString = new System.Windows.Forms.TextBox();
			this.grpSeparator = new System.Windows.Forms.GroupBox();
			this.fldSeparatorString = new System.Windows.Forms.TextBox();
			this.lblRecordNumber = new System.Windows.Forms.Label();
			this.lblCaution = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.lblCaution2 = new System.Windows.Forms.Label();
			this.grpStart.SuspendLayout();
			this.grpEnd.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.grpSeparator.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnCancelEdit
			// 
			this.btnCancelEdit.Location = new System.Drawing.Point(120, 280);
			this.btnCancelEdit.Name = "btnCancelEdit";
			this.btnCancelEdit.Size = new System.Drawing.Size(104, 32);
			this.btnCancelEdit.TabIndex = 18;
			this.btnCancelEdit.Text = "編集中止";
			this.btnCancelEdit.Click += new System.EventHandler(this.btnCancelEdit_Click);
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(400, 280);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 16;
			this.btnDeleteRecord.Text = "レコード削除";
			this.btnDeleteRecord.Click += new System.EventHandler(this.btnDeleteRecord_Click);
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 280);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 17;
			this.btnEntryData.Text = "データ登録";
			this.btnEntryData.Click += new System.EventHandler(this.btnEntryData_Click);
			// 
			// fldName
			// 
			this.fldName.Location = new System.Drawing.Point(72, 8);
			this.fldName.Name = "fldName";
			this.fldName.Size = new System.Drawing.Size(160, 19);
			this.fldName.TabIndex = 19;
			this.fldName.Text = "(name)";
			// 
			// lblName
			// 
			this.lblName.Location = new System.Drawing.Point(8, 11);
			this.lblName.Name = "lblName";
			this.lblName.Size = new System.Drawing.Size(64, 16);
			this.lblName.TabIndex = 20;
			this.lblName.Text = "区切り名称";
			// 
			// grpStart
			// 
			this.grpStart.Controls.Add(this.fldStartString);
			this.grpStart.Location = new System.Drawing.Point(8, 32);
			this.grpStart.Name = "grpStart";
			this.grpStart.Size = new System.Drawing.Size(496, 48);
			this.grpStart.TabIndex = 21;
			this.grpStart.TabStop = false;
			this.grpStart.Text = "区切り開始文字列";
			// 
			// fldStartString
			// 
			this.fldStartString.Location = new System.Drawing.Point(8, 18);
			this.fldStartString.Name = "fldStartString";
			this.fldStartString.Size = new System.Drawing.Size(480, 19);
			this.fldStartString.TabIndex = 0;
			this.fldStartString.Text = "(開始文字列)";
			// 
			// grpEnd
			// 
			this.grpEnd.Controls.Add(this.groupBox1);
			this.grpEnd.Controls.Add(this.fldEndString);
			this.grpEnd.Location = new System.Drawing.Point(8, 88);
			this.grpEnd.Name = "grpEnd";
			this.grpEnd.Size = new System.Drawing.Size(496, 48);
			this.grpEnd.TabIndex = 22;
			this.grpEnd.TabStop = false;
			this.grpEnd.Text = "区切り終了文字列";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.textBox1);
			this.groupBox1.Location = new System.Drawing.Point(0, 48);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(496, 48);
			this.groupBox1.TabIndex = 23;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "区切り終了文字列";
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(8, 18);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(480, 19);
			this.textBox1.TabIndex = 0;
			this.textBox1.Text = "(開始文字列)";
			// 
			// fldEndString
			// 
			this.fldEndString.Location = new System.Drawing.Point(8, 18);
			this.fldEndString.Name = "fldEndString";
			this.fldEndString.Size = new System.Drawing.Size(480, 19);
			this.fldEndString.TabIndex = 0;
			this.fldEndString.Text = "(終了文字列)";
			// 
			// grpSeparator
			// 
			this.grpSeparator.Controls.Add(this.fldSeparatorString);
			this.grpSeparator.Location = new System.Drawing.Point(8, 144);
			this.grpSeparator.Name = "grpSeparator";
			this.grpSeparator.Size = new System.Drawing.Size(496, 48);
			this.grpSeparator.TabIndex = 23;
			this.grpSeparator.TabStop = false;
			this.grpSeparator.Text = "データ内区切り文字列";
			// 
			// fldSeparatorString
			// 
			this.fldSeparatorString.Location = new System.Drawing.Point(8, 18);
			this.fldSeparatorString.Name = "fldSeparatorString";
			this.fldSeparatorString.Size = new System.Drawing.Size(480, 19);
			this.fldSeparatorString.TabIndex = 0;
			this.fldSeparatorString.Text = "(区切り文字列)";
			// 
			// lblRecordNumber
			// 
			this.lblRecordNumber.Location = new System.Drawing.Point(336, 8);
			this.lblRecordNumber.Name = "lblRecordNumber";
			this.lblRecordNumber.Size = new System.Drawing.Size(168, 23);
			this.lblRecordNumber.TabIndex = 24;
			this.lblRecordNumber.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// lblCaution
			// 
			this.lblCaution.Location = new System.Drawing.Point(16, 216);
			this.lblCaution.Name = "lblCaution";
			this.lblCaution.Size = new System.Drawing.Size(480, 32);
			this.lblCaution.TabIndex = 25;
			this.lblCaution.Text = "現状は、NEETでの区切り文字登録は仮登録です。 必ずNNsiの『参照ログ・データ区切り設定』でデータの登録確認が必要です。 (NN" +  
				"si上の登録操作完了後に使用可能になります。)  ";
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("ＭＳ ゴシック", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.label1.Location = new System.Drawing.Point(11, 196);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(88, 16);
			this.label1.TabIndex = 26;
			this.label1.Text = "※ 注意 ※";
			// 
			// lblCaution2
			// 
			this.lblCaution2.Location = new System.Drawing.Point(16, 248);
			this.lblCaution2.Name = "lblCaution2";
			this.lblCaution2.Size = new System.Drawing.Size(480, 16);
			this.lblCaution2.TabIndex = 27;
			this.lblCaution2.Text = "区切り文字列に漢字を使用した場合、SHIFT JISとして処理します。";
			// 
			// logTokenEdit
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(512, 317);
			this.Controls.Add(this.lblCaution2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.lblCaution);
			this.Controls.Add(this.lblRecordNumber);
			this.Controls.Add(this.grpSeparator);
			this.Controls.Add(this.grpEnd);
			this.Controls.Add(this.grpStart);
			this.Controls.Add(this.lblName);
			this.Controls.Add(this.fldName);
			this.Controls.Add(this.btnCancelEdit);
			this.Controls.Add(this.btnDeleteRecord);
			this.Controls.Add(this.btnEntryData);
			this.Name = "logTokenEdit";
			this.Text = "区切り文字編集";
			this.grpStart.ResumeLayout(false);
			this.grpEnd.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.grpSeparator.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   現在表示中のレコードを記録する
		/// </summary>
		private void updateRecord()
		{
			logToken targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// 新しいレコードオブジェクトを取得する
				targetRecord = mDbContainer.newRecordData();
				mMaxRecordNumber  = mDbContainer.numberOfRecords();
			}
			else
			{
				// 現在の表示中のレコードオブジェクトを取得する
				targetRecord = mDbContainer.getRecordData(mIndexNumber);
			}

			// ログデータを登録する...
			targetRecord.name = fldName.Text;
			targetRecord.startToken = fldStartString.Text;
			targetRecord.endToken   = fldEndString.Text;
			targetRecord.dataToken  = fldSeparatorString.Text;

			// 親の画面データを更新する
			mParent.refreshDataTable();

			return;
		}

		/// <summary>
		///   表示するレコード番号を（外部から）指定されたときの処理
		/// </summary>
		public void updateScreenData(int aIndexNumber)
		{
			// レコード番号の表示
			mIndexNumber = aIndexNumber;
			lblRecordNumber.Text = (aIndexNumber + 1) + " / " + mMaxRecordNumber;

			mEditRecord = mDbContainer.getRecordData(aIndexNumber);

			// フィールドにデータを反映させる
			fldName.Text			= mEditRecord.name;
			fldStartString.Text		= mEditRecord.startToken;
			fldEndString.Text		= mEditRecord.endToken;
			fldSeparatorString.Text	= mEditRecord.dataToken;

			return;
		}

		///  <summary>
		///    編集中止ボタンが押されたときの処理
		///  </summary>
		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// 画面を閉じる
			this.Close();

			return;
		}

		///  <summary>
		///    レコード削除ボタンが押されたときの処理
		///  </summary>
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
			mDbContainer.removeRecord(mIndexNumber);

			// 親画面データを更新する
			mParent.refreshDataTable();

			// 画面を閉じる
			this.Close();

			return;

		}

		///  <summary>
		///    レコード登録する
		///  </summary>
		private void btnEntryData_Click(object sender, System.EventArgs e)
		{
			// 現在表示中のレコードを更新(保存)する...
			updateRecord();

			// 画面を閉じる
			this.Close();
		}
	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ログ区切り文字列データベースの定義  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//
//#define MAX_STRLEN                  32
//#define MAX_DATATOKEN               80
//
// レス区切り用構造体(データベース)
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
