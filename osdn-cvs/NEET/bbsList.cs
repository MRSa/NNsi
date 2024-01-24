/*
 *  $Id: bbsList.cs,v 1.1 2005/05/14 07:40:52 mrsa Exp $
 *    --- NNsi��BBS�ꗗ
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
	///  bbsList �̊T�v�̐���
	/// </summary>
	public class bbsList : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button btnEdit;
		private System.Windows.Forms.Button btnNew;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.DataGrid listGrid;

//		private frmBBSselectURL		mFrmBbsSetting  = null;
		private frmNEETsetting		mFrmBbsSetting  = null;
		private bbsDbContainer      mDbContainer	= null;
		private DataSet				mDataSet		= null;
		private DataTable           mDataTable		= null;

//		public bbsList(frmBBSselectURL aFrmBbsSetting, ref bbsDbContainer arDbContainer)
		public bbsList(frmNEETsetting aFrmBbsSetting, ref bbsDbContainer arDbContainer)
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mFrmBbsSetting = aFrmBbsSetting;
			mDbContainer = arDbContainer;
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
			this.btnEdit = new System.Windows.Forms.Button();
			this.btnNew = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.btnSave = new System.Windows.Forms.Button();
			this.listGrid = new System.Windows.Forms.DataGrid();
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).BeginInit();
			this.SuspendLayout();
			// 
			// btnEdit
			// 
			this.btnEdit.Location = new System.Drawing.Point(8, 335);
			this.btnEdit.Name = "btnEdit";
			this.btnEdit.Size = new System.Drawing.Size(88, 23);
			this.btnEdit.TabIndex = 15;
			this.btnEdit.Text = "���R�[�h�ҏW...";
			this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
			// 
			// btnNew
			// 
			this.btnNew.Location = new System.Drawing.Point(112, 335);
			this.btnNew.Name = "btnNew";
			this.btnNew.Size = new System.Drawing.Size(88, 23);
			this.btnNew.TabIndex = 14;
			this.btnNew.Text = "�V�K���R�[�h...";
			this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(408, 335);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(88, 23);
			this.btnDelete.TabIndex = 13;
			this.btnDelete.Text = "�P���폜";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(512, 335);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(96, 23);
			this.btnSave.TabIndex = 12;
			this.btnSave.Text = "�t�@�C���ۑ�";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// listGrid
			// 
			this.listGrid.AllowSorting = false;
			this.listGrid.CaptionText = "BBS�ꗗ";
			this.listGrid.DataMember = "";
			this.listGrid.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.listGrid.Location = new System.Drawing.Point(8, 7);
			this.listGrid.Name = "listGrid";
			this.listGrid.ReadOnly = true;
			this.listGrid.RowHeadersVisible = false;
			this.listGrid.Size = new System.Drawing.Size(600, 320);
			this.listGrid.TabIndex = 11;
			// 
			// bbsList
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(616, 365);
			this.Controls.Add(this.btnEdit);
			this.Controls.Add(this.btnNew);
			this.Controls.Add(this.btnDelete);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.listGrid);
			this.Name = "bbsList";
			this.Text = "NEET (Phase2)";
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).EndInit();
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   �f�[�^���������A��ʕ\������
		/// </summary>
		public void prepareDataset()
		{
			// ���R�[�h�����擾����
			int recordNumber	= mDbContainer.numberOfRecords();

			// �ꗗ�\���p�̗̈揀��
			mDataSet			= new DataSet("BBS�ꗗ");
			mDataTable			= new DataTable();
			DataColumn[]	dc	= new DataColumn[5];

			// �f�[�^���x������������
			mDataTable.TableName = "BBS�ꗗ";
			mDataTable = mDataSet.Tables.Add("m_BBS");
			dc[0] = mDataTable.Columns.Add("No", Type.GetType("System.String"));
			mDataTable.PrimaryKey = dc;
			mDataTable.Columns.Add("BBS����", Type.GetType("System.String"));
			mDataTable.Columns.Add("Use", Type.GetType("System.String"));
			mDataTable.Columns.Add("URL", Type.GetType("System.String"));
			mDataTable.Columns.Add("Lv",  Type.GetType("System.String"));

			// �����R�[�h��o�^����
			for (int loop = 0; loop < recordNumber; loop++)
			{
				bbsRecord data = mDbContainer.getRecordData(loop);
				string name  = data.boardName;
				string url   = data.boardURL;
				string level = data.favoriteLevel;
				string use   = "";
				if (data.useBoardList == true)
				{
					use = "o";
				}
				mDataTable.Rows.Add(new object[]{(loop + 1), name, use, url, level});
			}

			// �e�[�u���̕\���X�^�C��(��̕�)���w�肷��
			DataGridTableStyle  tblStyle;
			DataGridColumnStyle col1, col2, col3, col4, col5;

			tblStyle = new DataGridTableStyle();
			tblStyle.MappingName = "m_BBS";
			tblStyle.AllowSorting = false;

			col1 = new DataGridTextBoxColumn();
			col1.MappingName = "No";
			col1.HeaderText = "#";
			col1.Width = 30;
			tblStyle.GridColumnStyles.Add(col1);

			col2 = new DataGridTextBoxColumn();
			col2.MappingName = "BBS����";
			col2.HeaderText = "����";
			col2.Width = 190;
			tblStyle.GridColumnStyles.Add(col2);

			col3 = new DataGridTextBoxColumn();
			col3.MappingName = "Use";
			col3.HeaderText = "Use";
			col3.Width = 20;
			tblStyle.GridColumnStyles.Add(col3);

			col4 = new DataGridTextBoxColumn();
			col4.MappingName = "URL";
			col4.HeaderText = "URL";
			col4.Width = 302;
			tblStyle.GridColumnStyles.Add(col4);

			col5 = new DataGridTextBoxColumn();
			col5.MappingName = "Lv";
			col5.HeaderText = "Lv";
			col5.Width = 20;
			tblStyle.GridColumnStyles.Add(col5);

			listGrid.TableStyles.Add(tblStyle);
			listGrid.RowHeaderWidth = 15;
			listGrid.AllowSorting = false;
			listGrid.SetDataBinding(mDataSet, "m_BBS");
		}

		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			// ���ݑI�𒆂̃��R�[�h�ԍ����擾����
			int row = listGrid.CurrentRowIndex;

			if (row == 0)
			{
				MessageBox.Show("'�Q�ƃ��O' �̃f�[�^�͕ҏW�ł��܂���B",
								"Warning",
								MessageBoxButtons.OK,
								MessageBoxIcon.Warning);
				// �Ȃɂ������I������
				return;
			}

			// ���R�[�h���Ȃ���ΕҏW���Ȃ�
			if (mDbContainer.numberOfRecords() == 0)
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
		///   ���R�[�h�ҏW�_�C�A���O�𐶐�����
		/// </summary>
		private void editRecord(int aRow)
		{
			// �ҏW�_�C�A���O�𐶐�
			frmBBSedit editDialog = new frmBBSedit(this, ref mDbContainer);

			// �c�a�̐擪���R�[�h��ҏW�_�C�A���O�ɕ\������ݒ�ɂ���
			editDialog.updateScreenData(aRow);

			// �ҏW�_�C�A���O���J��
			editDialog.ShowDialog();
			return;
		}

		private void btnNew_Click(object sender, System.EventArgs e)
		{
			// �V�������R�[�h�I�u�W�F�N�g���擾����
			bbsRecord data      = mDbContainer.newRecordData();
			int       recordNum	= mDbContainer.numberOfRecords();

			string name  = data.boardName;
			string url   = data.boardURL;
			string level = data.favoriteLevel;
			string use   = "";
			if (data.useBoardList == true)
			{
				use = "o";
			}
			mDataTable.Rows.Add(new object[]{recordNum, name, use, url, level});

			// �ǉ��������R�[�h��ҏW����...
			editRecord((recordNum - 1));
		}

		private void btnDelete_Click(object sender, System.EventArgs e)
		{
			int row = listGrid.CurrentRowIndex;

			if (row == 0)
			{
				MessageBox.Show("'�Q�ƃ��O' �͍폜�ł��܂���B",
								"Warning",
								MessageBoxButtons.OK,
								MessageBoxIcon.Warning);
				// �Ȃɂ������I������
				return;
			}

			if (MessageBox.Show("�I�𒆂̃��R�[�h���폜���܂��B",
								"���R�[�h�폜�̊m�F",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// �Ȃɂ������I������
				return;
			}
			// �I�𒆂̃��R�[�h���폜����
			mDbContainer.removeRecord(row);

			// �f�[�^�e�[�u�����X�V����
			refreshDataTable();
			return;
		}

		/// <summary>
		///   �f�[�^�e�[�u����(�\��)���e���X�V����
		/// </summary>
		public void refreshDataTable()
		{
			// �I�𒆂̃��R�[�h���L������
			int row = listGrid.CurrentRowIndex;
			int recordNumber = mDbContainer.numberOfRecords();

			// �f�[�^�e�[�u���̃��R�[�h��S�čX�V����
			mDataTable.Rows.Clear();
			for (int loop = 0; loop < recordNumber; loop++)
			{
				bbsRecord data = mDbContainer.getRecordData(loop);

				string name  = data.boardName;
				string url   = data.boardURL;
				string level = data.favoriteLevel;
				string use   = "";
				if (data.useBoardList == true)
				{
					use = "o";
				}
				mDataTable.Rows.Add(new object[]{(loop + 1), name, use, url, level});
			}

			// �I�𒆃��R�[�h��߂�
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

		private void btnSave_Click(object sender, System.EventArgs e)
		{

			// �f�[�^�o�͂̊m�F
			if (MessageBox.Show("BBS�ꗗDB���t�@�C���o�͂��܂��B",
								"�t�@�C���o�͂̊m�F",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// �Ȃɂ������I������
				return;
			}

			// �{�^���������Ȃ��悤�ɂ���
			btnSave.Enabled = false;

			// �������ޑO�Ƀf�[�^���\�[�g����
			mDbContainer.doSort();

			// �f�[�^�x�[�X�t�@�C���֏o�͂���...
			if (mDbContainer.saveDatabase(false) == false)
			{
				MessageBox.Show("BBS�ꗗDB�̏o�͂Ɏ��s���܂���...�B",
								"�G���[",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
			}
			else
			{
				// �o�͕�
				MessageBox.Show("BBS�ꗗDB���o�͂��܂����B", "DB�o�͊���",
								MessageBoxButtons.OK,
								MessageBoxIcon.Information);
			}

			// �ۑ��{�^����������悤�ɂ���
			btnSave.Enabled = true;
			return;
		}

	}
}

