/*
 *  $Id: frmNGwordInput.cs,v 1.1 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi��NG���[�h�ҏW�t�H�[��
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
	///   frmNGwordInput �̊T�v�̐���
	/// </summary>
	public class frmNGwordInput : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
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
		///   �R���X�g���N�^
		/// </summary>
		public frmNGwordInput(frmNGwordEdit aParent, ref NGwordContainer arDbContainer)
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mParent          = aParent;
			mContainer       = arDbContainer;
			mMaxRecordNumber = mContainer.numberOfRecords();
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
			this.lblNGword.Text = "NG���[�h";
			// 
			// fldNGword
			// 
			this.fldNGword.Location = new System.Drawing.Point(64, 5);
			this.fldNGword.Name = "fldNGword";
			this.fldNGword.Size = new System.Drawing.Size(360, 19);
			this.fldNGword.TabIndex = 1;
			this.fldNGword.Text = "(NG���[�h�����)";
			// 
			// chkReplace
			// 
			this.chkReplace.Location = new System.Drawing.Point(32, 152);
			this.chkReplace.Name = "chkReplace";
			this.chkReplace.Size = new System.Drawing.Size(296, 24);
			this.chkReplace.TabIndex = 2;
			this.chkReplace.Text = "NG���[�h�����o�����ꍇ�ANG���[�h������ * �ɒu������";
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
			this.grpAction.Text = "NG���[�h�����o����͈�";
			// 
			// grpMsg
			// 
			this.grpMsg.Location = new System.Drawing.Point(24, 88);
			this.grpMsg.Name = "grpMsg";
			this.grpMsg.Size = new System.Drawing.Size(368, 24);
			this.grpMsg.TabIndex = 3;
			this.grpMsg.Text = "MSG�� (�{��������NG���[�h���`�F�b�N����)";
			// 
			// grpExceptMsg
			// 
			this.grpExceptMsg.Location = new System.Drawing.Point(24, 64);
			this.grpExceptMsg.Name = "grpExceptMsg";
			this.grpExceptMsg.Size = new System.Drawing.Size(368, 24);
			this.grpExceptMsg.TabIndex = 2;
			this.grpExceptMsg.Text = "MSG���ȊO (�{���ȊO�̕�����NG���[�h���`�F�b�N����)";
			// 
			// chkHeader
			// 
			this.chkHeader.Location = new System.Drawing.Point(24, 40);
			this.chkHeader.Name = "chkHeader";
			this.chkHeader.Size = new System.Drawing.Size(368, 24);
			this.chkHeader.TabIndex = 1;
			this.chkHeader.Text = "���O��/���[���� (���O���ƃ��[��������NG���[�h���`�F�b�N����)";
			// 
			// chkWhole
			// 
			this.chkWhole.Checked = true;
			this.chkWhole.Location = new System.Drawing.Point(24, 16);
			this.chkWhole.Name = "chkWhole";
			this.chkWhole.Size = new System.Drawing.Size(368, 24);
			this.chkWhole.TabIndex = 0;
			this.chkWhole.TabStop = true;
			this.chkWhole.Text = "���X�S�� (���O���A���[�����A�{�����ׂĂ�NG���[�h���`�F�b�N����)";
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 208);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 18;
			this.btnEntryData.Text = "�f�[�^�o�^";
			this.btnEntryData.Click += new System.EventHandler(this.btnEntryData_Click);
			// 
			// btnCancelEdit
			// 
			this.btnCancelEdit.Location = new System.Drawing.Point(120, 208);
			this.btnCancelEdit.Name = "btnCancelEdit";
			this.btnCancelEdit.Size = new System.Drawing.Size(104, 32);
			this.btnCancelEdit.TabIndex = 19;
			this.btnCancelEdit.Text = "�ҏW���~";
			this.btnCancelEdit.Click += new System.EventHandler(this.btnCancelEdit_Click);
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(304, 208);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 20;
			this.btnDeleteRecord.Text = "���R�[�h�폜";
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
			this.lblDescription.Text = "(�ʏ�́ANG���[�h�����o����ƃ��X�S�̂�\�������܂���B)";
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
			this.Text = "NG���[�h�ҏW";
			this.grpAction.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   �\�����郌�R�[�h�ԍ����i�O������j�w�肳�ꂽ�Ƃ��̏���
		/// </summary>
		public void updateScreenData(int aIndexNumber)
		{
			// ���R�[�h�ԍ��̕\��
			mIndexNumber = aIndexNumber;
			lblRecordNumber.Text = (aIndexNumber + 1) + " / " + mMaxRecordNumber;

			mEditRecord = mContainer.getRecordData(aIndexNumber);

			// �t�B�[���h�Ƀf�[�^�𔽉f������

			// NG���[�h
			fldNGword.Text			= mEditRecord.ngWord;

			// �`�F�b�N����̈�̎w��
			switch (mEditRecord.checkArea)
			{
			  case 1:
				// ���O��/���[�������`�F�b�N����
				chkWhole.Checked     = false;
				chkHeader.Checked    = true;
				grpExceptMsg.Checked = false;
				grpMsg.Checked       = false;
				break;

			  case 2:
				// MSG���ȊO���`�F�b�N����
				chkWhole.Checked     = false;
				chkHeader.Checked    = false;
				grpExceptMsg.Checked = true;
				grpMsg.Checked       = false;
				break;

			  case 3:
				// MSG���������`�F�b�N����
				chkWhole.Checked     = false;
				chkHeader.Checked    = false;
				grpExceptMsg.Checked = false;
				grpMsg.Checked       = true;
				break;

			  case 0:
			  default:
				// ���X�S�̂��`�F�b�N����
				chkWhole.Checked     = true;
				chkHeader.Checked    = false;
				grpExceptMsg.Checked = false;
				grpMsg.Checked       = false;
				break;
			}

			// �}�b�`�����Ƃ��Ɏ��{���鏈��
			if (mEditRecord.matchedAction == 0)
			{
				// ���X�S�̂�\�����Ȃ�
				chkReplace.Checked = false;
			}
			else
			{
				// NG���[�h������ * �ɒu������
				chkReplace.Checked = true;
			}
			return;
		}

		/// <summary>
		///   �ҏW���~�{�^�������������̏���
		/// </summary>
		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// ��ʂ����
			this.Close();

			return;
		}
		
		private void btnDeleteRecord_Click(object sender, System.EventArgs e)
		{
			if (MessageBox.Show("�\�����̃��R�[�h���폜���܂��B",
								"���R�[�h�폜�̊m�F",
								MessageBoxButtons.OKCancel,
								MessageBoxIcon.Question) == DialogResult.Cancel)
			{
				// �Ȃɂ������I������
				return;
			}

			// �\�����̃��R�[�h���폜����
			mContainer.removeRecord(mIndexNumber);

			// �e��ʃf�[�^���X�V����
			mParent.refreshDataTable();

			// ��ʂ����
			this.Close();

			return;
		}
		
		private void btnEntryData_Click(object sender, System.EventArgs e)
		{
			// ���ݕ\�����̃��R�[�h���X�V(�ۑ�)����...
			updateRecord();

			// ��ʂ����
			this.Close();
		}

		/// <summary>
		///   ���ݕ\�����̃��R�[�h���L�^����
		/// </summary>
		private void updateRecord()
		{
			NGwordRecord targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// �V�������R�[�h�I�u�W�F�N�g���擾����
				targetRecord      = mContainer.newRecordData();
				mMaxRecordNumber  = mContainer.numberOfRecords();
			}
			else
			{
				// ���݂̕\�����̃��R�[�h�I�u�W�F�N�g���擾����
				targetRecord = mContainer.getRecordData(mIndexNumber);
			}

			// ���O�f�[�^��o�^����...


			// NG���[�h
			targetRecord.ngWord = fldNGword.Text;

			// �`�F�b�N�̈�̐ݒ�
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

			// �u�����[�h���ǂ����H
			if (chkReplace.Checked == true)
			{
				// NG���[�h�u�����[�h
				targetRecord.matchedAction = 1;
			}
			else
			{
				// ���X�������[�h
				targetRecord.matchedAction = 0;
			}

			// �e�̉�ʃf�[�^���X�V����
			mParent.refreshDataTable();

			return;
		}
	}
}
