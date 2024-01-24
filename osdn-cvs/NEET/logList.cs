/*
 *  $Id: logList.cs,v 1.5 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̎Q�ƃ��O�f�[�^�x�[�X�̈ꗗ�\��...
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
	///   ���O�ꗗ
	/// </summary>
	public class frmLogList : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.DataGrid listGrid;
		private System.Windows.Forms.Button btnEdit;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.Button btnNew;

		private int                 mMaxRecordNumber;   // DB�̃��R�[�h��
		private getLogDbContainer	mLogDataContainer;	// ���O�R���e�i�N���X
		private bool                mEnabledLogData;    // ���O�f�[�^���L��
		private DataSet				mGetLogDataSet = null;
		private DataTable           mDataTable = null;
		private frmNEETsetting		mFrmNeetSetting = null;

		public frmLogList(frmNEETsetting aFrmNeetSetting)
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mEnabledLogData = false;
			mFrmNeetSetting = aFrmNeetSetting;
		}

		/// <summary>
		/// �g�p����Ă��郊�\�[�X�̌㏈�������s���܂��B
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

		#region Windows �t�H�[���f�U�C�i�Ő������ꂽ�R�[�h
		/// <summary>
		/// �f�U�C�i�T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h�G�f�B�^�ŕύX���Ȃ��ł��������B
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
			this.listGrid.CaptionText = "�o�^�ςݎQ�ƃ��O�ꗗ";
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
			this.btnEdit.Text = "���R�[�h�ҏW...";
			this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(216, 336);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(88, 23);
			this.btnDelete.TabIndex = 3;
			this.btnDelete.Text = "�P���폜";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(512, 336);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(96, 23);
			this.btnSave.TabIndex = 4;
			this.btnSave.Text = "�t�@�C���ۑ�";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnNew
			// 
			this.btnNew.Location = new System.Drawing.Point(104, 336);
			this.btnNew.Name = "btnNew";
			this.btnNew.Size = new System.Drawing.Size(88, 23);
			this.btnNew.TabIndex = 2;
			this.btnNew.Text = "�V�K���R�[�h...";
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
		///  ���O�R���e�i�N���X��ݒ肷�� (�����ݒ�...)
		/// </summary>
		public void setLogContainer(ref getLogDbContainer arLogDataContainer)
		{
			mLogDataContainer = arLogDataContainer;
			mEnabledLogData   = true;
			mMaxRecordNumber  = mLogDataContainer.numberOfGetLogRecords();
		}

		/// <summary>
		///   �f�[�^���������A��ʕ\������
		/// </summary>
		public void prepareDataset()
		{
			if (mEnabledLogData != true)
			{
				// �f�[�^�����[�h���Ă��Ȃ���΁A�I������
				return;
			}

			// �ꗗ�\���p�̗̈揀��
			mGetLogDataSet   = new DataSet("�Q�ƃ��O�f�[�^�ꗗ");
			mDataTable       = new DataTable();
			DataColumn[]  dc = new DataColumn[4];

			// �f�[�^���x������������
			mDataTable.TableName = "�Q�ƃ��O�f�[�^�ꗗ";
			mDataTable = mGetLogDataSet.Tables.Add("m_�Q�ƃ��O");
			dc[0] = mDataTable.Columns.Add("No", Type.GetType("System.String"));
			mDataTable.PrimaryKey = dc;
			mDataTable.Columns.Add("���O", Type.GetType("System.String"));
			mDataTable.Columns.Add("URL", Type.GetType("System.String"));
			mDataTable.Columns.Add("Lv", Type.GetType("System.String"));

			// �����R�[�h��o�^����
			for (int loop = 0; loop < mMaxRecordNumber; loop++)
			{
				logRecord data = mLogDataContainer.getRecordData(loop);
				string title = data.DataTitle;
				string url   = data.DataURL;
				mDataTable.Rows.Add(new object[]{(loop + 1), title, url, data.Usable});
			}

			// �e�[�u���̕\���X�^�C��(��̕�)���w�肷��
			DataGridTableStyle  tblStyle;
			DataGridColumnStyle col1, col2, col3, col4;

			tblStyle = new DataGridTableStyle();
			tblStyle.MappingName = "m_�Q�ƃ��O";
			tblStyle.AllowSorting = false;

			col1 = new DataGridTextBoxColumn();
			col1.MappingName = "No";
			col1.HeaderText = "#";
			col1.Width = 20;
			tblStyle.GridColumnStyles.Add(col1);

			col2 = new DataGridTextBoxColumn();
			col2.MappingName = "���O";
			col2.HeaderText = "���O";
			col2.Width = 190;
			tblStyle.GridColumnStyles.Add(col2);

			col3 = new DataGridTextBoxColumn();
			col3.MappingName = "URL";
			col3.HeaderText = "�擾��URL";
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
			listGrid.SetDataBinding(mGetLogDataSet, "m_�Q�ƃ��O");
		}


		/// <summary>
		///   ���R�[�h�̕ҏW
		/// </summary>
		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			// ���ݑI�𒆂̃��R�[�h�ԍ����擾����
			int row = listGrid.CurrentRowIndex;

			if (mLogDataContainer.numberOfGetLogRecords() == 0)
			{
				return;
			}

			// ���R�[�h���Ȃ���ΕҏW�͂��Ȃ�
			if (row < 0)
			{
				return;
			}

			// �ҏW�_�C�A���O�𐶐����J��
			editRecord(row);

			return;
		}

		/// <summary>
		///   �V�K���R�[�h�̒ǉ�
		/// </summary>
		private void btnNew_Click(object sender, System.EventArgs e)
		{
			// �V�������R�[�h�I�u�W�F�N�g���擾����
			logRecord data   = mLogDataContainer.newRecordData();
			mMaxRecordNumber = mLogDataContainer.numberOfGetLogRecords();

			string title = data.DataTitle;
			string url   = data.DataURL;
			mDataTable.Rows.Add(new object[]{mMaxRecordNumber, title, url, data.Usable});

			// �ǉ��������R�[�h��ҏW����...
			editRecord((mMaxRecordNumber - 1));
		}

		/// <summary>
		///   �f�[�^���t�@�C���o��
		/// </summary>
		private void btnSave_Click(object sender, System.EventArgs e)
		{
			// �o�̓f�[�^�x�[�X�t�@�C������ݒ肷��
			mLogDataContainer.selectLogDatabaseFileNameToSave();

			// �f�[�^�o�͂̊m�F
			if (MessageBox.Show("�Q�ƃ��ODB���t�@�C���o�͂��܂��B",
								"�t�@�C���o�͂̊m�F",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// �Ȃɂ������I������
				return;
			}

			// �{�^���������Ȃ��悤�ɂ���
			btnSave.Enabled = false;

			// �f�[�^�x�[�X�t�@�C���֏o�͂���...
			if (mLogDataContainer.saveGetLogDatabase() == false)
			{
				MessageBox.Show("�Q�ƃ��ODB�̏o�͂Ɏ��s���܂���...�B",
								"�G���[",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
			}
			else
			{
				// �o�͕�
				MessageBox.Show("�Q�ƃ��ODB���o�͂��܂����B", "DB�o�͊���",
								MessageBoxButtons.OK,
								MessageBoxIcon.Information);
			}

			// �ۑ��{�^����������悤�ɂ���
			btnSave.Enabled = true;
			return;
		}

		/// <summary>
		///   ���R�[�h�ҏW�_�C�A���O�𐶐�����
		/// </summary>
		private void editRecord(int aRow)
		{
			// �ҏW�_�C�A���O�𐶐�
			WinForm editDialog = new WinForm(this, ref mLogDataContainer);

			// �ҏW�_�C�A���O��ɂ���{�^����L���ɂ���
			editDialog.activateEditFormButtons();

			// �c�a�̐擪���R�[�h��ҏW�_�C�A���O�ɕ\������ݒ�ɂ���
			editDialog.updateScreenData(aRow);

			// �ҏW�_�C�A���O���J��
			//  (�ҏW���̓{�^���𖳌��ɂ���)
			controlButtons(false);
			editDialog.ShowDialog();
			controlButtons(true);

			return;
		}


		/// <summary>
		///   �f�[�^�e�[�u����(�\��)���e���X�V����
		/// </summary>
		public void refreshDataTable()
		{
			// �I�𒆂̃��R�[�h���L������
			int row = listGrid.CurrentRowIndex;

			mMaxRecordNumber = mLogDataContainer.numberOfGetLogRecords();

			// �f�[�^�e�[�u���̃��R�[�h��S�čX�V����
			mDataTable.Rows.Clear();
			for (int loop = 0; loop < mMaxRecordNumber; loop++)
			{
				logRecord data = mLogDataContainer.getRecordData(loop);
				string title = data.DataTitle;
				string url   = data.DataURL;
				mDataTable.Rows.Add(new object[]{(loop + 1), title, url, data.Usable});
			}

			// �I�𒆃��R�[�h��߂�
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
		///   ���R�[�h���P���폜����
		/// </summary>
		private void btnDelete_Click(object sender, System.EventArgs e)
		{
			if (MessageBox.Show("�I�𒆂̃��R�[�h���폜���܂��B",
								"���R�[�h�폜�̊m�F",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// �Ȃɂ������I������
				return;
			}
			// �I�𒆂̃��R�[�h���폜����
			int row = listGrid.CurrentRowIndex;
			mLogDataContainer.removeRecord(row);

			// �f�[�^�e�[�u�����X�V����
			refreshDataTable();
			return;
		}


		/// <summary>
		///   �{�^���̐�����s��
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
