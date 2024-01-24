/*
 *  $Id: logTokenList.cs,v 1.3 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̋�؂蕶����ꗗ
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
	///  WinForm1 �̊T�v�̐���
	/// </summary>
	public class logTokenList : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.DataGrid listGrid;
		private System.Windows.Forms.Button btnSave;
		private System.Windows.Forms.Button btnDelete;
		private System.Windows.Forms.Button btnNew;
		private System.Windows.Forms.Button btnEdit;

		private frmNEETsetting		mFrmNeetSetting = null;
		private logTokenDbContainer mDbContainer	= null;
		private DataSet				mDataSet		= null;
		private DataTable           mDataTable		= null;


		public logTokenList(frmNEETsetting aFrmNeetSetting, ref logTokenDbContainer arDbContainer)
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mFrmNeetSetting = aFrmNeetSetting;
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
			this.listGrid = new System.Windows.Forms.DataGrid();
			this.btnSave = new System.Windows.Forms.Button();
			this.btnDelete = new System.Windows.Forms.Button();
			this.btnNew = new System.Windows.Forms.Button();
			this.btnEdit = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.listGrid)).BeginInit();
			this.SuspendLayout();
			// 
			// listGrid
			// 
			this.listGrid.AllowSorting = false;
			this.listGrid.CaptionText = "�Q�ƃ��O�p��؂�ݒ�ꗗ";
			this.listGrid.DataMember = "";
			this.listGrid.HeaderForeColor = System.Drawing.SystemColors.ControlText;
			this.listGrid.Location = new System.Drawing.Point(8, 8);
			this.listGrid.Name = "listGrid";
			this.listGrid.ReadOnly = true;
			this.listGrid.RowHeadersVisible = false;
			this.listGrid.Size = new System.Drawing.Size(600, 320);
			this.listGrid.TabIndex = 1;
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(512, 336);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(96, 23);
			this.btnSave.TabIndex = 7;
			this.btnSave.Text = "�t�@�C���ۑ�";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(408, 336);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(88, 23);
			this.btnDelete.TabIndex = 8;
			this.btnDelete.Text = "�P���폜";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// btnNew
			// 
			this.btnNew.Location = new System.Drawing.Point(112, 336);
			this.btnNew.Name = "btnNew";
			this.btnNew.Size = new System.Drawing.Size(88, 23);
			this.btnNew.TabIndex = 9;
			this.btnNew.Text = "�V�K���R�[�h...";
			this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
			// 
			// btnEdit
			// 
			this.btnEdit.Location = new System.Drawing.Point(8, 336);
			this.btnEdit.Name = "btnEdit";
			this.btnEdit.Size = new System.Drawing.Size(88, 23);
			this.btnEdit.TabIndex = 10;
			this.btnEdit.Text = "���R�[�h�ҏW...";
			this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
			// 
			// logTokenList
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(616, 365);
			this.Controls.Add(this.btnEdit);
			this.Controls.Add(this.btnNew);
			this.Controls.Add(this.btnDelete);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.listGrid);
			this.Name = "logTokenList";
			this.Text = "NEET (Phase1)";
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
			mDataSet			= new DataSet("��؂�ꗗ");
			mDataTable			= new DataTable();
			DataColumn[]	dc	= new DataColumn[2];

			// �f�[�^���x������������
			mDataTable.TableName = "��؂�ꗗ";
			mDataTable = mDataSet.Tables.Add("m_��؂�");
			dc[0] = mDataTable.Columns.Add("No", Type.GetType("System.String"));
			mDataTable.PrimaryKey = dc;
			mDataTable.Columns.Add("���O", Type.GetType("System.String"));

			// �����R�[�h��o�^����
			for (int loop = 0; loop < recordNumber; loop++)
			{
				logToken data = mDbContainer.getRecordData(loop);
				string name = data.name;
				mDataTable.Rows.Add(new object[]{(loop + 1), name});
			}

			// �e�[�u���̕\���X�^�C��(��̕�)���w�肷��
			DataGridTableStyle  tblStyle;
			DataGridColumnStyle col1, col2;

			tblStyle = new DataGridTableStyle();
			tblStyle.MappingName = "m_��؂�";
			tblStyle.AllowSorting = false;

			col1 = new DataGridTextBoxColumn();
			col1.MappingName = "No";
			col1.HeaderText = "#";
			col1.Width = 30;
			tblStyle.GridColumnStyles.Add(col1);

			col2 = new DataGridTextBoxColumn();
			col2.MappingName = "���O";
			col2.HeaderText = "���O";
			col2.Width = 570;
			tblStyle.GridColumnStyles.Add(col2);

			listGrid.TableStyles.Add(tblStyle);
			listGrid.RowHeaderWidth = 15;
			listGrid.AllowSorting = false;
			listGrid.SetDataBinding(mDataSet, "m_��؂�");
		}
		
		private void btnNew_Click(object sender, System.EventArgs e)
		{
			// �V�������R�[�h�I�u�W�F�N�g���擾����
			logToken data   = mDbContainer.newRecordData();
			int recordNum	= mDbContainer.numberOfRecords();

			string name = data.name;
			mDataTable.Rows.Add(new object[]{recordNum, name});

			// �ǉ��������R�[�h��ҏW����...
			editRecord((recordNum - 1));

		}

		/// <summary>
		///   ���R�[�h�ҏW�_�C�A���O�𐶐�����
		/// </summary>
		private void editRecord(int aRow)
		{
			// �ҏW�_�C�A���O�𐶐�
			logTokenEdit editDialog = new logTokenEdit(this, ref mDbContainer);

			// �c�a�̐擪���R�[�h��ҏW�_�C�A���O�ɕ\������ݒ�ɂ���
			editDialog.updateScreenData(aRow);

			// �ҏW�_�C�A���O���J��
			editDialog.ShowDialog();
			return;
		}

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
			mDbContainer.removeRecord(row);

			// �f�[�^�e�[�u�����X�V����
			refreshDataTable();
			return;
		}

		/// <summary>
		///   �f�[�^���t�@�C���o��
		/// </summary>
		private void btnSave_Click(object sender, System.EventArgs e)
		{

			// �f�[�^�o�͂̊m�F
			if (MessageBox.Show("��؂�DB���t�@�C���o�͂��܂��B",
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
			if (mDbContainer.saveDatabase(false) == false)
			{
				MessageBox.Show("��؂�DB�̏o�͂Ɏ��s���܂���...�B",
								"�G���[",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
			}
			else
			{
				// �o�͕�
				MessageBox.Show("��؂�DB���o�͂��܂����B", "DB�o�͊���",
								MessageBoxButtons.OK,
								MessageBoxIcon.Information);
			}

			// �ۑ��{�^����������悤�ɂ���
			btnSave.Enabled = true;
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
				logToken data = mDbContainer.getRecordData(loop);
				string   name = data.name;
				mDataTable.Rows.Add(new object[]{(loop + 1), name});
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

		/// <summary>
		///
		/// </summary>
		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			// ���ݑI�𒆂̃��R�[�h�ԍ����擾����
			int row = listGrid.CurrentRowIndex;

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
	}
}
