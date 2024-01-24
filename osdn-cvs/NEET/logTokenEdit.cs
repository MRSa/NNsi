/*
 *  $Id: logTokenEdit.cs,v 1.3 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̋�؂蕶����ҏW
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
	public class logTokenEdit : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
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
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mParent			  = aParent;
			mDbContainer	  = arDbContainer;
			mMaxRecordNumber  = mDbContainer.numberOfRecords();
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
			this.btnCancelEdit.Text = "�ҏW���~";
			this.btnCancelEdit.Click += new System.EventHandler(this.btnCancelEdit_Click);
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(400, 280);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 16;
			this.btnDeleteRecord.Text = "���R�[�h�폜";
			this.btnDeleteRecord.Click += new System.EventHandler(this.btnDeleteRecord_Click);
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 280);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 17;
			this.btnEntryData.Text = "�f�[�^�o�^";
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
			this.lblName.Text = "��؂薼��";
			// 
			// grpStart
			// 
			this.grpStart.Controls.Add(this.fldStartString);
			this.grpStart.Location = new System.Drawing.Point(8, 32);
			this.grpStart.Name = "grpStart";
			this.grpStart.Size = new System.Drawing.Size(496, 48);
			this.grpStart.TabIndex = 21;
			this.grpStart.TabStop = false;
			this.grpStart.Text = "��؂�J�n������";
			// 
			// fldStartString
			// 
			this.fldStartString.Location = new System.Drawing.Point(8, 18);
			this.fldStartString.Name = "fldStartString";
			this.fldStartString.Size = new System.Drawing.Size(480, 19);
			this.fldStartString.TabIndex = 0;
			this.fldStartString.Text = "(�J�n������)";
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
			this.grpEnd.Text = "��؂�I��������";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.textBox1);
			this.groupBox1.Location = new System.Drawing.Point(0, 48);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(496, 48);
			this.groupBox1.TabIndex = 23;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "��؂�I��������";
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(8, 18);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(480, 19);
			this.textBox1.TabIndex = 0;
			this.textBox1.Text = "(�J�n������)";
			// 
			// fldEndString
			// 
			this.fldEndString.Location = new System.Drawing.Point(8, 18);
			this.fldEndString.Name = "fldEndString";
			this.fldEndString.Size = new System.Drawing.Size(480, 19);
			this.fldEndString.TabIndex = 0;
			this.fldEndString.Text = "(�I��������)";
			// 
			// grpSeparator
			// 
			this.grpSeparator.Controls.Add(this.fldSeparatorString);
			this.grpSeparator.Location = new System.Drawing.Point(8, 144);
			this.grpSeparator.Name = "grpSeparator";
			this.grpSeparator.Size = new System.Drawing.Size(496, 48);
			this.grpSeparator.TabIndex = 23;
			this.grpSeparator.TabStop = false;
			this.grpSeparator.Text = "�f�[�^����؂蕶����";
			// 
			// fldSeparatorString
			// 
			this.fldSeparatorString.Location = new System.Drawing.Point(8, 18);
			this.fldSeparatorString.Name = "fldSeparatorString";
			this.fldSeparatorString.Size = new System.Drawing.Size(480, 19);
			this.fldSeparatorString.TabIndex = 0;
			this.fldSeparatorString.Text = "(��؂蕶����)";
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
			this.lblCaution.Text = "����́ANEET�ł̋�؂蕶���o�^�͉��o�^�ł��B �K��NNsi�́w�Q�ƃ��O�E�f�[�^��؂�ݒ�x�Ńf�[�^�̓o�^�m�F���K�v�ł��B (NN" +  
				"si��̓o�^���슮����Ɏg�p�\�ɂȂ�܂��B)  ";
			// 
			// label1
			// 
			this.label1.Font = new System.Drawing.Font("�l�r �S�V�b�N", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.label1.Location = new System.Drawing.Point(11, 196);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(88, 16);
			this.label1.TabIndex = 26;
			this.label1.Text = "�� ���� ��";
			// 
			// lblCaution2
			// 
			this.lblCaution2.Location = new System.Drawing.Point(16, 248);
			this.lblCaution2.Name = "lblCaution2";
			this.lblCaution2.Size = new System.Drawing.Size(480, 16);
			this.lblCaution2.TabIndex = 27;
			this.lblCaution2.Text = "��؂蕶����Ɋ������g�p�����ꍇ�ASHIFT JIS�Ƃ��ď������܂��B";
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
			this.Text = "��؂蕶���ҏW";
			this.grpStart.ResumeLayout(false);
			this.grpEnd.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.grpSeparator.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   ���ݕ\�����̃��R�[�h���L�^����
		/// </summary>
		private void updateRecord()
		{
			logToken targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// �V�������R�[�h�I�u�W�F�N�g���擾����
				targetRecord = mDbContainer.newRecordData();
				mMaxRecordNumber  = mDbContainer.numberOfRecords();
			}
			else
			{
				// ���݂̕\�����̃��R�[�h�I�u�W�F�N�g���擾����
				targetRecord = mDbContainer.getRecordData(mIndexNumber);
			}

			// ���O�f�[�^��o�^����...
			targetRecord.name = fldName.Text;
			targetRecord.startToken = fldStartString.Text;
			targetRecord.endToken   = fldEndString.Text;
			targetRecord.dataToken  = fldSeparatorString.Text;

			// �e�̉�ʃf�[�^���X�V����
			mParent.refreshDataTable();

			return;
		}

		/// <summary>
		///   �\�����郌�R�[�h�ԍ����i�O������j�w�肳�ꂽ�Ƃ��̏���
		/// </summary>
		public void updateScreenData(int aIndexNumber)
		{
			// ���R�[�h�ԍ��̕\��
			mIndexNumber = aIndexNumber;
			lblRecordNumber.Text = (aIndexNumber + 1) + " / " + mMaxRecordNumber;

			mEditRecord = mDbContainer.getRecordData(aIndexNumber);

			// �t�B�[���h�Ƀf�[�^�𔽉f������
			fldName.Text			= mEditRecord.name;
			fldStartString.Text		= mEditRecord.startToken;
			fldEndString.Text		= mEditRecord.endToken;
			fldSeparatorString.Text	= mEditRecord.dataToken;

			return;
		}

		///  <summary>
		///    �ҏW���~�{�^���������ꂽ�Ƃ��̏���
		///  </summary>
		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// ��ʂ����
			this.Close();

			return;
		}

		///  <summary>
		///    ���R�[�h�폜�{�^���������ꂽ�Ƃ��̏���
		///  </summary>
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
			mDbContainer.removeRecord(mIndexNumber);

			// �e��ʃf�[�^���X�V����
			mParent.refreshDataTable();

			// ��ʂ����
			this.Close();

			return;

		}

		///  <summary>
		///    ���R�[�h�o�^����
		///  </summary>
		private void btnEntryData_Click(object sender, System.EventArgs e)
		{
			// ���ݕ\�����̃��R�[�h���X�V(�ۑ�)����...
			updateRecord();

			// ��ʂ����
			this.Close();
		}
	}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ���O��؂蕶����f�[�^�x�[�X�̒�`  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//
//#define MAX_STRLEN                  32
//#define MAX_DATATOKEN               80
//
// ���X��؂�p�\����(�f�[�^�x�[�X)
//typedef struct {
//	UInt32 id;                              // �f�[�^ID
//	Char   tokenPatternName[MAX_STRLEN];    // ��؂�p�^�[����
//	Char   startToken[MAX_DATATOKEN];       // �؂�o���J�n������
//	Char   endToken  [MAX_DATATOKEN];       // �؂�o���I��������
//	Char   dataToken [MAX_DATATOKEN];       // �f�[�^�C���f�b�N�X������
//	UInt32 lastUpdate;                      // �ŏI�X�V����
//	UInt8  usable;                          // �g�p����/���Ȃ�
//	UInt8  reserved0;                       // �\��̈�
//	UInt16 reserved1;                       // �\��̈�
//	UInt32 reserved2;                       // �\��̈�
//} NNshLogTokenDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}
