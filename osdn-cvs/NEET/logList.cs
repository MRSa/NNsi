/*
 *  $Id: logList.cs,v 1.5 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsiの参照ログデータベースの一覧表示...
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
	///   ログ一覧
	/// </summary>
	public class frmLogList : System.Windows.Forms.Form
	{
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.DataGrid listGrid;
		private System.Windows.Forms.Button btnEdit;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.Button btnNew;

		private int                 mMaxRecordNumber;   // DBのレコード数
		private getLogDbContainer	mLogDataContainer;	// ログコンテナクラス
		private bool                mEnabledLogData;    // ログデータが有効
		private DataSet				mGetLogDataSet = null;
		private DataTable           mDataTable = null;
		private frmNEETsetting		mFrmNeetSetting = null;

		public frmLogList(frmNEETsetting aFrmNeetSetting)
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent 呼び出しの後のコンストラクタコードを追加
			//
			mEnabledLogData = false;
			mFrmNeetSetting = aFrmNeetSetting;
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
			this.listGrid = new System.Windows.Forms.DataGrid();
			this.btnEdit = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.btnSave = new System.Windows.Forms.Button();
			this.btnNew = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).BeginInit();
			this.SuspendLayout();
			// 
			// listGrid
			// 
			this.listGrid.AllowSorting = false;
			this.listGrid.CaptionText = "登録済み参照ログ一覧";
			this.listGrid.DataMember = "";
			this.listGrid.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.listGrid.Location = new System.Drawing.Point(8, 8);
			this.listGrid.Name = "listGrid";
			this.listGrid.ReadOnly = true;
			this.listGrid.RowHeadersVisible = false;
			this.listGrid.Size = new System.Drawing.Size(600, 320);
			this.listGrid.TabIndex = 0;
			// 
			// btnEdit
			// 
			this.btnEdit.Location = new System.Drawing.Point(8, 336);
			this.btnEdit.Name = "btnEdit";
			this.btnEdit.Size = new System.Drawing.Size(88, 23);
			this.btnEdit.TabIndex = 1;
			this.btnEdit.Text = "レコード編集...";
			this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(216, 336);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(88, 23);
			this.btnDelete.TabIndex = 3;
			this.btnDelete.Text = "１件削除";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(512, 336);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(96, 23);
			this.btnSave.TabIndex = 4;
			this.btnSave.Text = "ファイル保存";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnNew
			// 
			this.btnNew.Location = new System.Drawing.Point(104, 336);
			this.btnNew.Name = "btnNew";
			this.btnNew.Size = new System.Drawing.Size(88, 23);
			this.btnNew.TabIndex = 2;
			this.btnNew.Text = "新規レコード...";
			this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
			// 
			// frmLogList
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(616, 365);
			this.Controls.Add(this.btnNew);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.btnDelete);
			this.Controls.Add(this.btnEdit);
			this.Controls.Add(this.listGrid);
			this.Name = "frmLogList";
			this.Text = "NEET  -- a NNsi\'s Expanding and Editing Tool";
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).EndInit();
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///  ログコンテナクラスを設定する (初期設定...)
		/// </summary>
		public void setLogContainer(ref getLogDbContainer arLogDataContainer)
		{
			mLogDataContainer = arLogDataContainer;
			mEnabledLogData   = true;
			mMaxRecordNumber  = mLogDataContainer.numberOfGetLogRecords();
		}

		/// <summary>
		///   データを準備し、画面表示する
		/// </summary>
		public void prepareDataset()
		{
			if (mEnabledLogData != true)
			{
				// データをロードしていなければ、終了する
				return;
			}

			// 一覧表示用の領域準備
			mGetLogDataSet   = new DataSet("参照ログデータ一覧");
			mDataTable       = new DataTable();
			DataColumn[]  dc = new DataColumn[4];

			// データラベルを準備する
			mDataTable.TableName = "参照ログデータ一覧";
			mDataTable = mGetLogDataSet.Tables.Add("m_参照ログ");
			dc[0] = mDataTable.Columns.Add("No", Type.GetType("System.String"));
			mDataTable.PrimaryKey = dc;
			mDataTable.Columns.Add("名前", Type.GetType("System.String"));
			mDataTable.Columns.Add("URL", Type.GetType("System.String"));
			mDataTable.Columns.Add("Lv", Type.GetType("System.String"));

			// 実レコードを登録する
			for (int loop = 0; loop < mMaxRecordNumber; loop++)
			{
				logRecord data = mLogDataContainer.getRecordData(loop);
				string title = data.DataTitle;
				string url   = data.DataURL;
				mDataTable.Rows.Add(new object[]{(loop + 1), title, url, data.Usable});
			}

			// テーブルの表示スタイル(列の幅)を指定する
			DataGridTableStyle  tblStyle;
			DataGridColumnStyle col1, col2, col3, col4;

			tblStyle = new DataGridTableStyle();
			tblStyle.MappingName = "m_参照ログ";
			tblStyle.AllowSorting = false;

			col1 = new DataGridTextBoxColumn();
			col1.MappingName = "No";
			col1.HeaderText = "#";
			col1.Width = 20;
			tblStyle.GridColumnStyles.Add(col1);

			col2 = new DataGridTextBoxColumn();
			col2.MappingName = "名前";
			col2.HeaderText = "名前";
			col2.Width = 190;
			tblStyle.GridColumnStyles.Add(col2);

			col3 = new DataGridTextBoxColumn();
			col3.MappingName = "URL";
			col3.HeaderText = "取得先URL";
			col3.Width = 335;
			tblStyle.GridColumnStyles.Add(col3);

			col4 = new DataGridTextBoxColumn();
			col4.MappingName = "Lv";
			col4.HeaderText = "Lv";
			col4.Width = 20;
			tblStyle.GridColumnStyles.Add(col4);

			listGrid.TableStyles.Add(tblStyle);
			listGrid.RowHeaderWidth = 15;
			listGrid.AllowSorting = false;
			listGrid.SetDataBinding(mGetLogDataSet, "m_参照ログ");
		}


		/// <summary>
		///   レコードの編集
		/// </summary>
		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			// 現在選択中のレコード番号を取得する
			int row = listGrid.CurrentRowIndex;

			if (mLogDataContainer.numberOfGetLogRecords() == 0)
			{
				return;
			}

			// レコードがなければ編集はしない
			if (row < 0)
			{
				return;
			}

			// 編集ダイアログを生成し開く
			editRecord(row);

			return;
		}

		/// <summary>
		///   新規レコードの追加
		/// </summary>
		private void btnNew_Click(object sender, System.EventArgs e)
		{
			// 新しいレコードオブジェクトを取得する
			logRecord data   = mLogDataContainer.newRecordData();
			mMaxRecordNumber = mLogDataContainer.numberOfGetLogRecords();

			string title = data.DataTitle;
			string url   = data.DataURL;
			mDataTable.Rows.Add(new object[]{mMaxRecordNumber, title, url, data.Usable});

			// 追加したレコードを編集する...
			editRecord((mMaxRecordNumber - 1));
		}

		/// <summary>
		///   データをファイル出力
		/// </summary>
		private void btnSave_Click(object sender, System.EventArgs e)
		{
			// 出力データベースファイル名を設定する
			mLogDataContainer.selectLogDatabaseFileNameToSave();

			// データ出力の確認
			if (MessageBox.Show("参照ログDBをファイル出力します。",
								"ファイル出力の確認",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// なにもせず終了する
				return;
			}

			// ボタンを押せないようにする
			btnSave.Enabled = false;

			// データベースファイルへ出力する...
			if (mLogDataContainer.saveGetLogDatabase() == false)
			{
				MessageBox.Show("参照ログDBの出力に失敗しました...。",
								"エラー",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
			}
			else
			{
				// 出力報告
				MessageBox.Show("参照ログDBを出力しました。", "DB出力完了",
								MessageBoxButtons.OK,
								MessageBoxIcon.Information);
			}

			// 保存ボタンを押せるようにする
			btnSave.Enabled = true;
			return;
		}

		/// <summary>
		///   レコード編集ダイアログを生成する
		/// </summary>
		private void editRecord(int aRow)
		{
			// 編集ダイアログを生成
			WinForm editDialog = new WinForm(this, ref mLogDataContainer);

			// 編集ダイアログ上にあるボタンを有効にする
			editDialog.activateEditFormButtons();

			// ＤＢの先頭レコードを編集ダイアログに表示する設定にする
			editDialog.updateScreenData(aRow);

			// 編集ダイアログを開く
			//  (編集中はボタンを無効にする)
			controlButtons(false);
			editDialog.ShowDialog();
			controlButtons(true);

			return;
		}


		/// <summary>
		///   データテーブルの(表示)内容を更新する
		/// </summary>
		public void refreshDataTable()
		{
			// 選択中のレコードを記憶する
			int row = listGrid.CurrentRowIndex;

			mMaxRecordNumber = mLogDataContainer.numberOfGetLogRecords();

			// データテーブルのレコードを全て更新する
			mDataTable.Rows.Clear();
			for (int loop = 0; loop < mMaxRecordNumber; loop++)
			{
				logRecord data = mLogDataContainer.getRecordData(loop);
				string title = data.DataTitle;
				string url   = data.DataURL;
				mDataTable.Rows.Add(new object[]{(loop + 1), title, url, data.Usable});
			}

			// 選択中レコードを戻す
			if (row >= mMaxRecordNumber)
			{
				row = (mMaxRecordNumber - 1);
			}
			if (row < 0)
			{
				row = 0;
			}
			listGrid.CurrentRowIndex = row;
			return;
		}

		/// <summary>
		///   レコードを１件削除する
		/// </summary>
		private void btnDelete_Click(object sender, System.EventArgs e)
		{
			if (MessageBox.Show("選択中のレコードを削除します。",
								"レコード削除の確認",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// なにもせず終了する
				return;
			}
			// 選択中のレコードを削除する
			int row = listGrid.CurrentRowIndex;
			mLogDataContainer.removeRecord(row);

			// データテーブルを更新する
			refreshDataTable();
			return;
		}


		/// <summary>
		///   ボタンの制御を行う
		/// </summary>
		private void controlButtons(bool aData)
		{
			btnDelete.Enabled = aData;
			btnNew.Enabled    = aData;
			btnEdit.Enabled   = aData;
			btnSave.Enabled   = aData;

			return;
		}
	}
}
