/*
 *  $Id: frmNGwordEdit.cs,v 1.3 2005/02/26 13:17:49 mrsa Exp $
 *    --- NGワード一覧...
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
	///  frmNGwordEdit の概要の説明
	/// </summary>
	public class frmNGwordEdit : System.Windows.Forms.Form
	{
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button btnNew;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnEdit;
		private System.Windows.Forms.DataGrid listGrid;

		private frmNEETsetting  mParent		= null;
		private NGwordContainer mContainer	= null;
		private DataSet			mDataSet	= null;
		private DataTable       mDataTable	= null;

		public frmNGwordEdit(frmNEETsetting aFrmNeetSetting, ref NGwordContainer arDbContainer)
		{
			//
			// Windows フォームデザイナサポートに必要です。
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent 呼び出しの後のコンストラクタコードを追加
			//
			mParent          = aFrmNeetSetting;
			mContainer       = arDbContainer;
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
			this.btnNew = new System.Windows.Forms.Button();
			this.btnSave = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.listGrid = new System.Windows.Forms.DataGrid();
			this.btnEdit = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).BeginInit();
			this.SuspendLayout();
			// 
			// btnNew
			// 
			this.btnNew.Location = new System.Drawing.Point(112, 328);
			this.btnNew.Name = "btnNew";
			this.btnNew.Size = new System.Drawing.Size(88, 23);
			this.btnNew.TabIndex = 6;
			this.btnNew.Text = "新規レコード...";
			this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(360, 328);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(96, 23);
			this.btnSave.TabIndex = 8;
			this.btnSave.Text = "ファイル保存";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(224, 328);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(88, 23);
			this.btnDelete.TabIndex = 7;
			this.btnDelete.Text = "１件削除";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// listGrid
			// 
			this.listGrid.AllowSorting = false;
			this.listGrid.CaptionText = "登録済みNGワード一覧";
			this.listGrid.DataMember = "";
			this.listGrid.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.listGrid.Location = new System.Drawing.Point(0, 0);
			this.listGrid.Name = "listGrid";
			this.listGrid.ReadOnly = true;
			this.listGrid.RowHeadersVisible = false;
			this.listGrid.Size = new System.Drawing.Size(472, 320);
			this.listGrid.TabIndex = 5;
			// 
			// btnEdit
			// 
			this.btnEdit.Location = new System.Drawing.Point(8, 328);
			this.btnEdit.Name = "btnEdit";
			this.btnEdit.Size = new System.Drawing.Size(88, 23);
			this.btnEdit.TabIndex = 11;
			this.btnEdit.Text = "レコード編集...";
			this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
			// 
			// frmNGwordEdit
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(472, 357);
			this.Controls.Add(this.btnEdit);
			this.Controls.Add(this.btnNew);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.btnDelete);
			this.Controls.Add(this.listGrid);
			this.Name = "frmNGwordEdit";
			this.Text = "NGワード-3編集";
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).EndInit();
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   データを準備し、画面表示する
		/// </summary>
		public void prepareDataset()
		{
			// レコード数を取得する
			int recordNumber	= mContainer.numberOfRecords();

			// 一覧表示用の領域準備
			mDataSet			= new DataSet("NGワード一覧");
			mDataTable			= new DataTable();
			DataColumn[]	dc	= new DataColumn[2];

			// データラベルを準備する
			mDataTable.TableName = "NGワード一覧";
			mDataTable = mDataSet.Tables.Add("m_NGワード");
			dc[0] = mDataTable.Columns.Add("No", Type.GetType("System.String"));
			mDataTable.PrimaryKey = dc;
			mDataTable.Columns.Add("NGワード", Type.GetType("System.String"));

			// 実レコードを登録する
			for (int loop = 0; loop < recordNumber; loop++)
			{
				NGwordRecord data = mContainer.getRecordData(loop);
				string name = data.ngWord;
				mDataTable.Rows.Add(new object[]{(loop + 1), name});
			}

			// テーブルの表示スタイル(列の幅)を指定する
			DataGridTableStyle  tblStyle;
			DataGridColumnStyle col1, col2;

			tblStyle = new DataGridTableStyle();
			tblStyle.MappingName = "m_NGワード";
			tblStyle.AllowSorting = false;

			col1 = new DataGridTextBoxColumn();
			col1.MappingName = "No";
			col1.HeaderText = "#";
			col1.Width = 30;
			tblStyle.GridColumnStyles.Add(col1);

			col2 = new DataGridTextBoxColumn();
			col2.MappingName = "NGワード";
			col2.HeaderText = "NGワード";
			col2.Width = 405;
			tblStyle.GridColumnStyles.Add(col2);

			listGrid.TableStyles.Add(tblStyle);
			listGrid.RowHeaderWidth = 15;
			listGrid.AllowSorting = false;
			listGrid.SetDataBinding(mDataSet, "m_NGワード");

		}

		/// <summary>
		///    レコードの削除
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
			mContainer.removeRecord(row);

			// データテーブルを更新する
			refreshDataTable();
			return;

		}

		/// <summary>
		///   データテーブルの(表示)内容を更新する
		/// </summary>
		public void refreshDataTable()
		{
			// 選択中のレコードを記憶する
			int row = listGrid.CurrentRowIndex;
			int recordNumber = mContainer.numberOfRecords();

			// データテーブルのレコードを全て更新する
			mDataTable.Rows.Clear();
			for (int loop = 0; loop < recordNumber; loop++)
			{
				NGwordRecord data = mContainer.getRecordData(loop);
				string   name = data.ngWord;
				mDataTable.Rows.Add(new object[]{(loop + 1), name});
			}

			// 選択中レコードを戻す
			if (row >= recordNumber)
			{
				row = (recordNumber - 1);
			}
			if (row < 0)
			{
				row = 0;
			}
			listGrid.CurrentRowIndex = row;
			return;
		}

		/// <summary>
		///
		/// </summary>
		private void btnSave_Click(object sender, System.EventArgs e)
		{

			// データ出力の確認
			if (MessageBox.Show("NGワードDBをファイル出力します。",
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
			if (mContainer.saveDatabase(false) == false)
			{
				MessageBox.Show("NGワードDBの出力に失敗しました...。",
								"エラー",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
			}
			else
			{
				// 出力報告
				MessageBox.Show("NGワードDBを出力しました。", "DB出力完了",
								MessageBoxButtons.OK,
								MessageBoxIcon.Information);
			}

			// 保存ボタンを押せるようにする
			btnSave.Enabled = true;
			return;
		}

		/// <summary>
		/// </summary>
		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			// 現在選択中のレコード番号を取得する
			int row = listGrid.CurrentRowIndex;

			// レコードがなければ編集しない
			if (mContainer.numberOfRecords() == 0)
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
		///   レコード編集ダイアログを生成する
		/// </summary>
		private void editRecord(int aRow)
		{
			// 編集ダイアログを生成
			frmNGwordInput editDialog = new frmNGwordInput(this, ref mContainer);

			// ＤＢの先頭レコードを編集ダイアログに表示する設定にする
			editDialog.updateScreenData(aRow);

			// 編集ダイアログを開く
			editDialog.ShowDialog();
			return;
		}


		/// <summary>
		///   
		/// </summary>
		private void btnNew_Click(object sender, System.EventArgs e)
		{
			// 新しいレコードオブジェクトを取得する
			NGwordRecord data   = mContainer.newRecordData();
			int recordNum      	= mContainer.numberOfRecords();

			string name = data.ngWord;
			mDataTable.Rows.Add(new object[]{recordNum, name});

			// 追加したレコードを編集する...
			editRecord((recordNum - 1));

			return;
		}
	}
}
