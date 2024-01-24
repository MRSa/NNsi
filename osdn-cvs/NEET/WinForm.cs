/*
 *  $Id: WinForm.cs,v 1.8 2005/07/18 04:33:20 mrsa Exp $
 *    --- NNsiの参照ログ設定の１レコードを編集するための処理
 *
 *
 */
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;

namespace NEET
{
	/// <summary>
	///   WinForm の概要の説明
	///    --- WinFormは、NNsiの参照ログ設定の１レコードを編集するための処理を
	///       行います。
	/// </summary>
	public class WinForm : System.Windows.Forms.Form
	{
		/// <summary>
		///   必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.GroupBox getLogINFO;
		private System.Windows.Forms.TextBox fldTargetName;
		private System.Windows.Forms.Label lblName;
		private System.Windows.Forms.Label lblHeader;
		private System.Windows.Forms.TextBox fldHeader;
		private System.Windows.Forms.TextBox fldUrl;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button btnOpenUrl;
		private System.Windows.Forms.Label lblLogLevel;
		private System.Windows.Forms.NumericUpDown numLevel;
		private System.Windows.Forms.CheckBox chkRSS;
		private System.Windows.Forms.CheckBox chkGetLogLayer;
		private System.Windows.Forms.NumericUpDown numUpdateCheck;
		private System.Windows.Forms.GroupBox box2;
		private System.Windows.Forms.Label lblHour;
		private System.Windows.Forms.Button btnTokenSelect;
		private System.Windows.Forms.Button btnTokenSelectLevel2;
		private System.Windows.Forms.GroupBox selKanji;
		private System.Windows.Forms.RadioButton selShiftJIS;
		private System.Windows.Forms.RadioButton selEUC;
		private System.Windows.Forms.RadioButton selJIS;
		private System.Windows.Forms.Label lblRecordInfo;
		private System.Windows.Forms.Button btnDeleteRecord;
		private System.Windows.Forms.Button btnEntryData;
		private System.Windows.Forms.Button btnCancelEdit;


		private int					mIndexNumber;		// 表示中のIndex番号
		private int                 mMaxRecordNumber;   // DBのレコード数
		private getLogDbContainer	mLogDataContainer;	// ログコンテナクラス
		private frmLogList			mParent = null;			// ログリストクラス
		private bool                mEnabledLogData;    // ログデータが有効
		private logRecord           mCurrentData;
		private System.Windows.Forms.ComboBox cmbLogGetMode;       // 現在のレコードデータ


		/// <summary>
		///   コンストラクタ
		/// </summary>
		public WinForm(frmLogList aParent,
					   ref getLogDbContainer arLogDataContainer)
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

        	// 初期化処理...
			mIndexNumber      = 0;
			mEnabledLogData   = false;
			mParent           = aParent;
			mLogDataContainer = arLogDataContainer;
			mEnabledLogData   = true;
			mMaxRecordNumber  = mLogDataContainer.numberOfGetLogRecords();
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
			this.getLogINFO = new System.Windows.Forms.GroupBox();
			this.cmbLogGetMode = new System.Windows.Forms.ComboBox();
			this.lblRecordInfo = new System.Windows.Forms.Label();
			this.selKanji = new System.Windows.Forms.GroupBox();
			this.selJIS = new System.Windows.Forms.RadioButton();
			this.selEUC = new System.Windows.Forms.RadioButton();
			this.selShiftJIS = new System.Windows.Forms.RadioButton();
			this.btnTokenSelectLevel2 = new System.Windows.Forms.Button();
			this.btnTokenSelect = new System.Windows.Forms.Button();
			this.box2 = new System.Windows.Forms.GroupBox();
			this.numUpdateCheck = new System.Windows.Forms.NumericUpDown();
			this.lblHour = new System.Windows.Forms.Label();
			this.chkGetLogLayer = new System.Windows.Forms.CheckBox();
			this.chkRSS = new System.Windows.Forms.CheckBox();
			this.numLevel = new System.Windows.Forms.NumericUpDown();
			this.lblLogLevel = new System.Windows.Forms.Label();
			this.btnOpenUrl = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.fldUrl = new System.Windows.Forms.TextBox();
			this.fldHeader = new System.Windows.Forms.TextBox();
			this.lblHeader = new System.Windows.Forms.Label();
			this.lblName = new System.Windows.Forms.Label();
			this.fldTargetName = new System.Windows.Forms.TextBox();
			this.btnDeleteRecord = new System.Windows.Forms.Button();
			this.btnEntryData = new System.Windows.Forms.Button();
			this.btnCancelEdit = new System.Windows.Forms.Button();
			this.getLogINFO.SuspendLayout();
			this.selKanji.SuspendLayout();
			this.box2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.numUpdateCheck)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numLevel)).BeginInit();
			this.SuspendLayout();
			// 
			// getLogINFO
			// 
			this.getLogINFO.Controls.Add(this.cmbLogGetMode);
			this.getLogINFO.Controls.Add(this.lblRecordInfo);
			this.getLogINFO.Controls.Add(this.selKanji);
			this.getLogINFO.Controls.Add(this.btnTokenSelectLevel2);
			this.getLogINFO.Controls.Add(this.btnTokenSelect);
			this.getLogINFO.Controls.Add(this.box2);
			this.getLogINFO.Controls.Add(this.chkGetLogLayer);
			this.getLogINFO.Controls.Add(this.chkRSS);
			this.getLogINFO.Controls.Add(this.numLevel);
			this.getLogINFO.Controls.Add(this.lblLogLevel);
			this.getLogINFO.Controls.Add(this.btnOpenUrl);
			this.getLogINFO.Controls.Add(this.label1);
			this.getLogINFO.Controls.Add(this.fldUrl);
			this.getLogINFO.Controls.Add(this.fldHeader);
			this.getLogINFO.Controls.Add(this.lblHeader);
			this.getLogINFO.Controls.Add(this.lblName);
			this.getLogINFO.Controls.Add(this.fldTargetName);
			this.getLogINFO.Enabled = false;
			this.getLogINFO.Location = new System.Drawing.Point(8, 8);
			this.getLogINFO.Name = "getLogINFO";
			this.getLogINFO.Size = new System.Drawing.Size(496, 224);
			this.getLogINFO.TabIndex = 9;
			this.getLogINFO.TabStop = false;
			this.getLogINFO.Text = "参照ログ取得設定";
			// 
			// cmbLogGetMode
			// 
			this.cmbLogGetMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbLogGetMode.Items.AddRange(new object[] {
						"通常ログ取得モード",
						"ログ取得なしモード (WWWCライクな更新チェッカーとして使用する場合)",
						"取得ログ加工モード (保存ログ容量を抑えたい場合)"});
			this.cmbLogGetMode.Location = new System.Drawing.Point(8, 80);
			this.cmbLogGetMode.Name = "cmbLogGetMode";
			this.cmbLogGetMode.Size = new System.Drawing.Size(408, 20);
			this.cmbLogGetMode.TabIndex = 19;
			// 
			// lblRecordInfo
			// 
			this.lblRecordInfo.Location = new System.Drawing.Point(416, 18);
			this.lblRecordInfo.Name = "lblRecordInfo";
			this.lblRecordInfo.Size = new System.Drawing.Size(72, 16);
			this.lblRecordInfo.TabIndex = 18;
			this.lblRecordInfo.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// selKanji
			// 
			this.selKanji.Controls.Add(this.selJIS);
			this.selKanji.Controls.Add(this.selEUC);
			this.selKanji.Controls.Add(this.selShiftJIS);
			this.selKanji.Location = new System.Drawing.Point(128, 152);
			this.selKanji.Name = "selKanji";
			this.selKanji.Size = new System.Drawing.Size(232, 40);
			this.selKanji.TabIndex = 17;
			this.selKanji.TabStop = false;
			this.selKanji.Text = "漢字コード (初期値)";
			// 
			// selJIS
			// 
			this.selJIS.Enabled = false;
			this.selJIS.Location = new System.Drawing.Point(176, 16);
			this.selJIS.Name = "selJIS";
			this.selJIS.Size = new System.Drawing.Size(40, 16);
			this.selJIS.TabIndex = 2;
			this.selJIS.Text = "JIS";
			// 
			// selEUC
			// 
			this.selEUC.Enabled = false;
			this.selEUC.Location = new System.Drawing.Point(112, 16);
			this.selEUC.Name = "selEUC";
			this.selEUC.Size = new System.Drawing.Size(48, 16);
			this.selEUC.TabIndex = 1;
			this.selEUC.Text = "EUC";
			// 
			// selShiftJIS
			// 
			this.selShiftJIS.Checked = true;
			this.selShiftJIS.Enabled = false;
			this.selShiftJIS.Location = new System.Drawing.Point(24, 16);
			this.selShiftJIS.Name = "selShiftJIS";
			this.selShiftJIS.Size = new System.Drawing.Size(72, 16);
			this.selShiftJIS.TabIndex = 0;
			this.selShiftJIS.TabStop = true;
			this.selShiftJIS.Text = "シフトＪＩＳ";
			// 
			// btnTokenSelectLevel2
			// 
			this.btnTokenSelectLevel2.Enabled = false;
			this.btnTokenSelectLevel2.Location = new System.Drawing.Point(256, 194);
			this.btnTokenSelectLevel2.Name = "btnTokenSelectLevel2";
			this.btnTokenSelectLevel2.Size = new System.Drawing.Size(232, 23);
			this.btnTokenSelectLevel2.TabIndex = 16;
			this.btnTokenSelectLevel2.Text = "区切りパターン選択 (下の階層)";
			// 
			// btnTokenSelect
			// 
			this.btnTokenSelect.Enabled = false;
			this.btnTokenSelect.Location = new System.Drawing.Point(8, 195);
			this.btnTokenSelect.Name = "btnTokenSelect";
			this.btnTokenSelect.Size = new System.Drawing.Size(232, 23);
			this.btnTokenSelect.TabIndex = 15;
			this.btnTokenSelect.Text = "区切りパターン選択 (指定URL)";
			// 
			// box2
			// 
			this.box2.Controls.Add(this.numUpdateCheck);
			this.box2.Controls.Add(this.lblHour);
			this.box2.Enabled = false;
			this.box2.Location = new System.Drawing.Point(368, 152);
			this.box2.Name = "box2";
			this.box2.Size = new System.Drawing.Size(120, 40);
			this.box2.TabIndex = 14;
			this.box2.TabStop = false;
			this.box2.Text = "更新確認する時間";
			// 
			// numUpdateCheck
			// 
			this.numUpdateCheck.Enabled = false;
			this.numUpdateCheck.Location = new System.Drawing.Point(17, 16);
			this.numUpdateCheck.Maximum = new System.Decimal(new int[] {
						255,
						0,
						0,
						0});
			this.numUpdateCheck.Name = "numUpdateCheck";
			this.numUpdateCheck.Size = new System.Drawing.Size(56, 19);
			this.numUpdateCheck.TabIndex = 13;
			// 
			// lblHour
			// 
			this.lblHour.Enabled = false;
			this.lblHour.Location = new System.Drawing.Point(77, 19);
			this.lblHour.Name = "lblHour";
			this.lblHour.Size = new System.Drawing.Size(32, 16);
			this.lblHour.TabIndex = 15;
			this.lblHour.Text = "時間";
			// 
			// chkGetLogLayer
			// 
			this.chkGetLogLayer.Enabled = false;
			this.chkGetLogLayer.Location = new System.Drawing.Point(8, 128);
			this.chkGetLogLayer.Name = "chkGetLogLayer";
			this.chkGetLogLayer.Size = new System.Drawing.Size(408, 24);
			this.chkGetLogLayer.TabIndex = 12;
			this.chkGetLogLayer.Text = "リンクをたどってログ取得する (１階層のみ)";
			// 
			// chkRSS
			// 
			this.chkRSS.Enabled = false;
			this.chkRSS.Location = new System.Drawing.Point(8, 104);
			this.chkRSS.Name = "chkRSS";
			this.chkRSS.Size = new System.Drawing.Size(408, 24);
			this.chkRSS.TabIndex = 11;
			this.chkRSS.Text = "指定したURLはRDFファイル (RSSリーダとして使用する場合)";
			// 
			// numLevel
			// 
			this.numLevel.Enabled = false;
			this.numLevel.Location = new System.Drawing.Point(75, 167);
			this.numLevel.Maximum = new System.Decimal(new int[] {
						15,
						0,
						0,
						0});
			this.numLevel.Name = "numLevel";
			this.numLevel.Size = new System.Drawing.Size(40, 19);
			this.numLevel.TabIndex = 9;
			// 
			// lblLogLevel
			// 
			this.lblLogLevel.Enabled = false;
			this.lblLogLevel.Location = new System.Drawing.Point(11, 167);
			this.lblLogLevel.Name = "lblLogLevel";
			this.lblLogLevel.Size = new System.Drawing.Size(80, 16);
			this.lblLogLevel.TabIndex = 7;
			this.lblLogLevel.Text = "ログレベル：";
			// 
			// btnOpenUrl
			// 
			this.btnOpenUrl.Enabled = false;
			this.btnOpenUrl.Font = new System.Drawing.Font("MS UI Gothic", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btnOpenUrl.Location = new System.Drawing.Point(424, 80);
			this.btnOpenUrl.Name = "btnOpenUrl";
			this.btnOpenUrl.Size = new System.Drawing.Size(64, 24);
			this.btnOpenUrl.TabIndex = 6;
			this.btnOpenUrl.Text = "試しに開く";
			this.btnOpenUrl.Click += new System.EventHandler(this.btnOpenUrl_Click);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 40);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(39, 16);
			this.label1.TabIndex = 5;
			this.label1.Text = "URL：";
			// 
			// fldUrl
			// 
			this.fldUrl.Enabled = false;
			this.fldUrl.Location = new System.Drawing.Point(8, 56);
			this.fldUrl.Name = "fldUrl";
			this.fldUrl.Size = new System.Drawing.Size(480, 19);
			this.fldUrl.TabIndex = 4;
			this.fldUrl.Text = "(取得先URL)";
			// 
			// fldHeader
			// 
			this.fldHeader.Enabled = false;
			this.fldHeader.Location = new System.Drawing.Point(288, 16);
			this.fldHeader.Name = "fldHeader";
			this.fldHeader.Size = new System.Drawing.Size(112, 19);
			this.fldHeader.TabIndex = 3;
			this.fldHeader.Text = "(スレタイヘッダ)";
			// 
			// lblHeader
			// 
			this.lblHeader.Location = new System.Drawing.Point(216, 20);
			this.lblHeader.Name = "lblHeader";
			this.lblHeader.Size = new System.Drawing.Size(80, 16);
			this.lblHeader.TabIndex = 2;
			this.lblHeader.Text = "タイトルヘッダ： ";
			// 
			// lblName
			// 
			this.lblName.Location = new System.Drawing.Point(8, 20);
			this.lblName.Name = "lblName";
			this.lblName.Size = new System.Drawing.Size(40, 16);
			this.lblName.TabIndex = 1;
			this.lblName.Text = "名前：";
			// 
			// fldTargetName
			// 
			this.fldTargetName.Enabled = false;
			this.fldTargetName.Location = new System.Drawing.Point(48, 17);
			this.fldTargetName.Name = "fldTargetName";
			this.fldTargetName.Size = new System.Drawing.Size(160, 19);
			this.fldTargetName.TabIndex = 0;
			this.fldTargetName.Text = "(取得先名称)";
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(400, 240);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 11;
			this.btnDeleteRecord.Text = "レコード削除";
			this.btnDeleteRecord.Click += new System.EventHandler(this.btnDeleteRecord_Click);
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 240);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 12;
			this.btnEntryData.Text = "データ登録";
			this.btnEntryData.Click += new System.EventHandler(this.btnEntryData_Click);
			// 
			// btnCancelEdit
			// 
			this.btnCancelEdit.Location = new System.Drawing.Point(120, 240);
			this.btnCancelEdit.Name = "btnCancelEdit";
			this.btnCancelEdit.Size = new System.Drawing.Size(104, 32);
			this.btnCancelEdit.TabIndex = 15;
			this.btnCancelEdit.Text = "編集中止";
			this.btnCancelEdit.Click += new System.EventHandler(this.btnCancelEdit_Click);
			// 
			// WinForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(512, 277);
			this.Controls.Add(this.btnCancelEdit);
			this.Controls.Add(this.btnDeleteRecord);
			this.Controls.Add(this.getLogINFO);
			this.Controls.Add(this.btnEntryData);
			this.MaximizeBox = false;
			this.Name = "WinForm";
			this.Text = "参照ログ取得設定の編集";
			this.getLogINFO.ResumeLayout(false);
			this.selKanji.ResumeLayout(false);
			this.box2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.numUpdateCheck)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numLevel)).EndInit();
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		/// アプリケーションのメインエントリポイントです。
		/// </summary>
		[STAThread]
		static void Main()
		{
			// ファイル位置の設定画面を開く
			Application.Run(new frmNEETsetting());
		}

		/// <summary>
		///   画面にあるボタン群を有効にする
		/// </summary>
		public void activateEditFormButtons()
		{
			//  まだログコンテナクラスをもらっていないときには、ボタンは
			// 有効にしない。
			if (mEnabledLogData == false)
			{
				return;
			}

			getLogINFO.Enabled = true;
			fldTargetName.Enabled = true;
			lblName.Enabled = true;
			lblHeader.Enabled = true;
			fldHeader.Enabled = true;
			fldUrl.Enabled = true;
			label1.Enabled = true;
			btnOpenUrl.Enabled = true;
			lblLogLevel.Enabled = true;
			numLevel.Enabled = true;
			chkRSS.Enabled = true;
			chkGetLogLayer.Enabled = true;
			numUpdateCheck.Enabled = true;
			box2.Enabled = true;
			lblHour.Enabled = true;
			selKanji.Enabled = true;
			selShiftJIS.Enabled = true;
			selEUC.Enabled = true;
			selJIS.Enabled = true;
			lblRecordInfo.Enabled = true;
			btnDeleteRecord.Enabled = true;

			// そのうち対応するボタン群...
			btnTokenSelect.Enabled = false;		  // 区切り設定-1は未対応
			btnTokenSelectLevel2.Enabled = false; // 区切り設定-2は未対応
		 }

		/// <summary>
		///   表示するレコード番号を（外部から）指定されたときの処理
		/// </summary>
		public void updateScreenData(int aIndexNumber)
		{
			// インデックス番号をメンバ変数に格納する
			mIndexNumber = aIndexNumber;

			// レコード番号の表示
			lblRecordInfo.Text = (mIndexNumber + 1) + " / " + mMaxRecordNumber;

			////////////////////////////////////////////////////////////////
			//  レコードを取得し、画面表示する
			////////////////////////////////////////////////////////////////
			mCurrentData = mLogDataContainer.getRecordData(mIndexNumber);

			fldUrl.Text = mCurrentData.DataURL;
			fldTargetName.Text = mCurrentData.DataTitle;
			fldHeader.Text = mCurrentData.DataTitlePrefix;
			numLevel.Value = mCurrentData.Usable;
			numUpdateCheck.Value = mCurrentData.GetAfterHours;

			cmbLogGetMode.SelectedIndex = mCurrentData.WwwcMode;

			if (mCurrentData.RssData != 0)
			{
				chkRSS.Checked = true;
			}
			else
			{
				chkRSS.Checked = false;
			}
			if (mCurrentData.GetDepth != 0)
			{
				chkGetLogLayer.Checked = true;
			}
			else
			{
				chkGetLogLayer.Checked = false;
			}
			if (mCurrentData.KanjiCode == 0)
			{
				selShiftJIS.Checked = true;
				selEUC.Checked = false;
				selJIS.Checked = false;
			}
			if (mCurrentData.KanjiCode == 1)
			{
				selShiftJIS.Checked = false;
				selEUC.Checked = true;
				selJIS.Checked = false;
			}
			if (mCurrentData.KanjiCode == 2)
			{
				selShiftJIS.Checked = false;
				selEUC.Checked = false;
				selJIS.Checked = true;
			}
			if (mCurrentData.KanjiCode == 3)
			{
				selShiftJIS.Checked = true;
				selEUC.Checked = false;
				selJIS.Checked = false;
				chkRSS.Checked = true;
			}

			return;
		}

		/// <summary>
		///   現在表示中のレコードを記録する
		/// </summary>
		private void updateRecord()
		{
			logRecord targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// 新しいレコードオブジェクトを取得する
				targetRecord = mLogDataContainer.newRecordData();
				mMaxRecordNumber  = mLogDataContainer.numberOfGetLogRecords();
			}
			else
			{
				// 現在の表示中のレコードオブジェクトを取得する
				targetRecord = mLogDataContainer.getRecordData(mIndexNumber);
			}

			// 表示中画面からデータを取得し、オブジェクトを更新する
			targetRecord.DataURL = fldUrl.Text;
			targetRecord.DataTitle = fldTargetName.Text;
			targetRecord.DataTitlePrefix = fldHeader.Text;
			targetRecord.Usable = (int) numLevel.Value;
			targetRecord.GetAfterHours = (int) numUpdateCheck.Value;

			targetRecord.WwwcMode =	cmbLogGetMode.SelectedIndex;

			if (chkRSS.Checked == true)
			{
				targetRecord.RssData = 1;
			}
			else
			{
				targetRecord.RssData = 0;
			}

			if (chkGetLogLayer.Checked == true)
			{
				targetRecord.GetDepth = 1;
			}
			else
			{
				targetRecord.GetDepth = 0;
			}

			if (selJIS.Checked == true)
			{
				targetRecord.KanjiCode = 2;
			}

			if (selEUC.Checked == true)
			{
				targetRecord.KanjiCode = 1;
			}

			if (selShiftJIS.Checked == true)
			{
				targetRecord.KanjiCode = 0;
			}

			// 親の画面データを更新する
			mParent.refreshDataTable();
			return;
		}

		/// <summary>
		///   URLを試しに開いてみる...
		/// </summary>
		private void btnOpenUrl_Click(object sender, System.EventArgs e)
		{
			try
			{
				// 記入されたURLを開く...
				Process.Start(fldUrl.Text);
			}
			catch
			{
				// URLが妥当なものではなかった、なにもしません。
				// (面倒くさいので、httpプロトコルが指定されたかどうかの
				//  チェックもしない。。 *.txt を指定すると、メモ帳が
				//  立ち上がるはず...
				MessageBox.Show("Wrong URL? \n URL → " + fldUrl.Text + "\n");
				return;
			}
			return;
		}

		/// <summary>
		///   表示中レコードを削除し、ダイアログを閉じる
		/// </summary>
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
			mLogDataContainer.removeRecord(mIndexNumber);

			// 親画面データを更新する
			mParent.refreshDataTable();

			// 画面を閉じる
			this.Close();

			return;
		}


		/// <summary>
		///   表示中レコードを登録し、ダイアログを閉じる
		/// </summary>
		private void btnEntryData_Click(object sender, System.EventArgs e)
		{
			// 現在表示中のレコードを更新(保存)する...
			updateRecord();

			// 画面を閉じる
			this.Close();

			return;
		}

		/// <summary>
		///   表示中レコードの編集中止
		/// </summary>
		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// 画面を閉じる
			this.Close();

			return;
		}
	}
}

