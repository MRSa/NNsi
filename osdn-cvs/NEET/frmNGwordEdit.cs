/*
 *  $Id: frmNGwordEdit.cs,v 1.3 2005/02/26 13:17:49 mrsa Exp $
 *    --- NG���[�h�ꗗ...
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
	///  frmNGwordEdit �̊T�v�̐���
	/// </summary>
	public class frmNGwordEdit : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
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
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mParent          = aFrmNeetSetting;
			mContainer       = arDbContainer;
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
			this.btnNew.Text = "�V�K���R�[�h...";
			this.btnNew.Click += new System.EventHandler(this.btnNew_Click);
			// 
			// btnSave
			// 
			this.btnSave.Location = new System.Drawing.Point(360, 328);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(96, 23);
			this.btnSave.TabIndex = 8;
			this.btnSave.Text = "�t�@�C���ۑ�";
			this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnDelete
			// 
			this.btnDelete.Location = new System.Drawing.Point(224, 328);
			this.btnDelete.Name = "btnDelete";
			this.btnDelete.Size = new System.Drawing.Size(88, 23);
			this.btnDelete.TabIndex = 7;
			this.btnDelete.Text = "�P���폜";
			this.btnDelete.Click += new System.EventHandler(this.btnDelete_Click);
			// 
			// listGrid
			// 
			this.listGrid.AllowSorting = false;
			this.listGrid.CaptionText = "�o�^�ς�NG���[�h�ꗗ";
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
			this.btnEdit.Text = "���R�[�h�ҏW...";
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
			this.Text = "NG���[�h-3�ҏW";
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
			int recordNumber	= mContainer.numberOfRecords();

			// �ꗗ�\���p�̗̈揀��
			mDataSet			= new DataSet("NG���[�h�ꗗ");
			mDataTable			= new DataTable();
			DataColumn[]	dc	= new DataColumn[2];

			// �f�[�^���x������������
			mDataTable.TableName = "NG���[�h�ꗗ";
			mDataTable = mDataSet.Tables.Add("m_NG���[�h");
			dc[0] = mDataTable.Columns.Add("No", Type.GetType("System.String"));
			mDataTable.PrimaryKey = dc;
			mDataTable.Columns.Add("NG���[�h", Type.GetType("System.String"));

			// �����R�[�h��o�^����
			for (int loop = 0; loop < recordNumber; loop++)
			{
				NGwordRecord data = mContainer.getRecordData(loop);
				string name = data.ngWord;
				mDataTable.Rows.Add(new object[]{(loop + 1), name});
			}

			// �e�[�u���̕\���X�^�C��(��̕�)���w�肷��
			DataGridTableStyle  tblStyle;
			DataGridColumnStyle col1, col2;

			tblStyle = new DataGridTableStyle();
			tblStyle.MappingName = "m_NG���[�h";
			tblStyle.AllowSorting = false;

			col1 = new DataGridTextBoxColumn();
			col1.MappingName = "No";
			col1.HeaderText = "#";
			col1.Width = 30;
			tblStyle.GridColumnStyles.Add(col1);

			col2 = new DataGridTextBoxColumn();
			col2.MappingName = "NG���[�h";
			col2.HeaderText = "NG���[�h";
			col2.Width = 405;
			tblStyle.GridColumnStyles.Add(col2);

			listGrid.TableStyles.Add(tblStyle);
			listGrid.RowHeaderWidth = 15;
			listGrid.AllowSorting = false;
			listGrid.SetDataBinding(mDataSet, "m_NG���[�h");

		}

		/// <summary>
		///    ���R�[�h�̍폜
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
			mContainer.removeRecord(row);

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
			int recordNumber = mContainer.numberOfRecords();

			// �f�[�^�e�[�u���̃��R�[�h��S�čX�V����
			mDataTable.Rows.Clear();
			for (int loop = 0; loop < recordNumber; loop++)
			{
				NGwordRecord data = mContainer.getRecordData(loop);
				string   name = data.ngWord;
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
		private void btnSave_Click(object sender, System.EventArgs e)
		{

			// �f�[�^�o�͂̊m�F
			if (MessageBox.Show("NG���[�hDB���t�@�C���o�͂��܂��B",
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
			if (mContainer.saveDatabase(false) == false)
			{
				MessageBox.Show("NG���[�hDB�̏o�͂Ɏ��s���܂���...�B",
								"�G���[",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
			}
			else
			{
				// �o�͕�
				MessageBox.Show("NG���[�hDB���o�͂��܂����B", "DB�o�͊���",
								MessageBoxButtons.OK,
								MessageBoxIcon.Information);
			}

			// �ۑ��{�^����������悤�ɂ���
			btnSave.Enabled = true;
			return;
		}

		/// <summary>
		/// </summary>
		private void btnEdit_Click(object sender, System.EventArgs e)
		{
			// ���ݑI�𒆂̃��R�[�h�ԍ����擾����
			int row = listGrid.CurrentRowIndex;

			// ���R�[�h���Ȃ���ΕҏW���Ȃ�
			if (mContainer.numberOfRecords() == 0)
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
			frmNGwordInput editDialog = new frmNGwordInput(this, ref mContainer);

			// �c�a�̐擪���R�[�h��ҏW�_�C�A���O�ɕ\������ݒ�ɂ���
			editDialog.updateScreenData(aRow);

			// �ҏW�_�C�A���O���J��
			editDialog.ShowDialog();
			return;
		}


		/// <summary>
		///   
		/// </summary>
		private void btnNew_Click(object sender, System.EventArgs e)
		{
			// �V�������R�[�h�I�u�W�F�N�g���擾����
			NGwordRecord data   = mContainer.newRecordData();
			int recordNum      	= mContainer.numberOfRecords();

			string name = data.ngWord;
			mDataTable.Rows.Add(new object[]{recordNum, name});

			// �ǉ��������R�[�h��ҏW����...
			editRecord((recordNum - 1));

			return;
		}
	}
}
