/*
 *  $Id: WinForm.cs,v 1.8 2005/07/18 04:33:20 mrsa Exp $
 *    --- NNsi�̎Q�ƃ��O�ݒ�̂P���R�[�h��ҏW���邽�߂̏���
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
	///   WinForm �̊T�v�̐���
	///    --- WinForm�́ANNsi�̎Q�ƃ��O�ݒ�̂P���R�[�h��ҏW���邽�߂̏�����
	///       �s���܂��B
	/// </summary>
	public class WinForm : System.Windows.Forms.Form
	{
		/// <summary>
		///   �K�v�ȃf�U�C�i�ϐ��ł��B
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


		private int					mIndexNumber;		// �\������Index�ԍ�
		private int                 mMaxRecordNumber;   // DB�̃��R�[�h��
		private getLogDbContainer	mLogDataContainer;	// ���O�R���e�i�N���X
		private frmLogList			mParent = null;			// ���O���X�g�N���X
		private bool                mEnabledLogData;    // ���O�f�[�^���L��
		private logRecord           mCurrentData;
		private System.Windows.Forms.ComboBox cmbLogGetMode;       // ���݂̃��R�[�h�f�[�^


		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public WinForm(frmLogList aParent,
					   ref getLogDbContainer arLogDataContainer)
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

        	// ����������...
			mIndexNumber      = 0;
			mEnabledLogData   = false;
			mParent           = aParent;
			mLogDataContainer = arLogDataContainer;
			mEnabledLogData   = true;
			mMaxRecordNumber  = mLogDataContainer.numberOfGetLogRecords();
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
			this.getLogINFO.Text = "�Q�ƃ��O�擾�ݒ�";
			// 
			// cmbLogGetMode
			// 
			this.cmbLogGetMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.cmbLogGetMode.Items.AddRange(new object[] {
						"�ʏ탍�O�擾���[�h",
						"���O�擾�Ȃ����[�h (WWWC���C�N�ȍX�V�`�F�b�J�[�Ƃ��Ďg�p����ꍇ)",
						"�擾���O���H���[�h (�ۑ����O�e�ʂ�}�������ꍇ)"});
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
			this.selKanji.Text = "�����R�[�h (�����l)";
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
			this.selShiftJIS.Text = "�V�t�g�i�h�r";
			// 
			// btnTokenSelectLevel2
			// 
			this.btnTokenSelectLevel2.Enabled = false;
			this.btnTokenSelectLevel2.Location = new System.Drawing.Point(256, 194);
			this.btnTokenSelectLevel2.Name = "btnTokenSelectLevel2";
			this.btnTokenSelectLevel2.Size = new System.Drawing.Size(232, 23);
			this.btnTokenSelectLevel2.TabIndex = 16;
			this.btnTokenSelectLevel2.Text = "��؂�p�^�[���I�� (���̊K�w)";
			// 
			// btnTokenSelect
			// 
			this.btnTokenSelect.Enabled = false;
			this.btnTokenSelect.Location = new System.Drawing.Point(8, 195);
			this.btnTokenSelect.Name = "btnTokenSelect";
			this.btnTokenSelect.Size = new System.Drawing.Size(232, 23);
			this.btnTokenSelect.TabIndex = 15;
			this.btnTokenSelect.Text = "��؂�p�^�[���I�� (�w��URL)";
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
			this.box2.Text = "�X�V�m�F���鎞��";
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
			this.lblHour.Text = "����";
			// 
			// chkGetLogLayer
			// 
			this.chkGetLogLayer.Enabled = false;
			this.chkGetLogLayer.Location = new System.Drawing.Point(8, 128);
			this.chkGetLogLayer.Name = "chkGetLogLayer";
			this.chkGetLogLayer.Size = new System.Drawing.Size(408, 24);
			this.chkGetLogLayer.TabIndex = 12;
			this.chkGetLogLayer.Text = "�����N�����ǂ��ă��O�擾���� (�P�K�w�̂�)";
			// 
			// chkRSS
			// 
			this.chkRSS.Enabled = false;
			this.chkRSS.Location = new System.Drawing.Point(8, 104);
			this.chkRSS.Name = "chkRSS";
			this.chkRSS.Size = new System.Drawing.Size(408, 24);
			this.chkRSS.TabIndex = 11;
			this.chkRSS.Text = "�w�肵��URL��RDF�t�@�C�� (RSS���[�_�Ƃ��Ďg�p����ꍇ)";
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
			this.lblLogLevel.Text = "���O���x���F";
			// 
			// btnOpenUrl
			// 
			this.btnOpenUrl.Enabled = false;
			this.btnOpenUrl.Font = new System.Drawing.Font("MS UI Gothic", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.btnOpenUrl.Location = new System.Drawing.Point(424, 80);
			this.btnOpenUrl.Name = "btnOpenUrl";
			this.btnOpenUrl.Size = new System.Drawing.Size(64, 24);
			this.btnOpenUrl.TabIndex = 6;
			this.btnOpenUrl.Text = "�����ɊJ��";
			this.btnOpenUrl.Click += new System.EventHandler(this.btnOpenUrl_Click);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(8, 40);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(39, 16);
			this.label1.TabIndex = 5;
			this.label1.Text = "URL�F";
			// 
			// fldUrl
			// 
			this.fldUrl.Enabled = false;
			this.fldUrl.Location = new System.Drawing.Point(8, 56);
			this.fldUrl.Name = "fldUrl";
			this.fldUrl.Size = new System.Drawing.Size(480, 19);
			this.fldUrl.TabIndex = 4;
			this.fldUrl.Text = "(�擾��URL)";
			// 
			// fldHeader
			// 
			this.fldHeader.Enabled = false;
			this.fldHeader.Location = new System.Drawing.Point(288, 16);
			this.fldHeader.Name = "fldHeader";
			this.fldHeader.Size = new System.Drawing.Size(112, 19);
			this.fldHeader.TabIndex = 3;
			this.fldHeader.Text = "(�X���^�C�w�b�_)";
			// 
			// lblHeader
			// 
			this.lblHeader.Location = new System.Drawing.Point(216, 20);
			this.lblHeader.Name = "lblHeader";
			this.lblHeader.Size = new System.Drawing.Size(80, 16);
			this.lblHeader.TabIndex = 2;
			this.lblHeader.Text = "�^�C�g���w�b�_�F ";
			// 
			// lblName
			// 
			this.lblName.Location = new System.Drawing.Point(8, 20);
			this.lblName.Name = "lblName";
			this.lblName.Size = new System.Drawing.Size(40, 16);
			this.lblName.TabIndex = 1;
			this.lblName.Text = "���O�F";
			// 
			// fldTargetName
			// 
			this.fldTargetName.Enabled = false;
			this.fldTargetName.Location = new System.Drawing.Point(48, 17);
			this.fldTargetName.Name = "fldTargetName";
			this.fldTargetName.Size = new System.Drawing.Size(160, 19);
			this.fldTargetName.TabIndex = 0;
			this.fldTargetName.Text = "(�擾�於��)";
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(400, 240);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 11;
			this.btnDeleteRecord.Text = "���R�[�h�폜";
			this.btnDeleteRecord.Click += new System.EventHandler(this.btnDeleteRecord_Click);
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 240);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 12;
			this.btnEntryData.Text = "�f�[�^�o�^";
			this.btnEntryData.Click += new System.EventHandler(this.btnEntryData_Click);
			// 
			// btnCancelEdit
			// 
			this.btnCancelEdit.Location = new System.Drawing.Point(120, 240);
			this.btnCancelEdit.Name = "btnCancelEdit";
			this.btnCancelEdit.Size = new System.Drawing.Size(104, 32);
			this.btnCancelEdit.TabIndex = 15;
			this.btnCancelEdit.Text = "�ҏW���~";
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
			this.Text = "�Q�ƃ��O�擾�ݒ�̕ҏW";
			this.getLogINFO.ResumeLayout(false);
			this.selKanji.ResumeLayout(false);
			this.box2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.numUpdateCheck)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numLevel)).EndInit();
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		/// �A�v���P�[�V�����̃��C���G���g���|�C���g�ł��B
		/// </summary>
		[STAThread]
		static void Main()
		{
			// �t�@�C���ʒu�̐ݒ��ʂ��J��
			Application.Run(new frmNEETsetting());
		}

		/// <summary>
		///   ��ʂɂ���{�^���Q��L���ɂ���
		/// </summary>
		public void activateEditFormButtons()
		{
			//  �܂����O�R���e�i�N���X��������Ă��Ȃ��Ƃ��ɂ́A�{�^����
			// �L���ɂ��Ȃ��B
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

			// ���̂����Ή�����{�^���Q...
			btnTokenSelect.Enabled = false;		  // ��؂�ݒ�-1�͖��Ή�
			btnTokenSelectLevel2.Enabled = false; // ��؂�ݒ�-2�͖��Ή�
		 }

		/// <summary>
		///   �\�����郌�R�[�h�ԍ����i�O������j�w�肳�ꂽ�Ƃ��̏���
		/// </summary>
		public void updateScreenData(int aIndexNumber)
		{
			// �C���f�b�N�X�ԍ��������o�ϐ��Ɋi�[����
			mIndexNumber = aIndexNumber;

			// ���R�[�h�ԍ��̕\��
			lblRecordInfo.Text = (mIndexNumber + 1) + " / " + mMaxRecordNumber;

			////////////////////////////////////////////////////////////////
			//  ���R�[�h���擾���A��ʕ\������
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
		///   ���ݕ\�����̃��R�[�h���L�^����
		/// </summary>
		private void updateRecord()
		{
			logRecord targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// �V�������R�[�h�I�u�W�F�N�g���擾����
				targetRecord = mLogDataContainer.newRecordData();
				mMaxRecordNumber  = mLogDataContainer.numberOfGetLogRecords();
			}
			else
			{
				// ���݂̕\�����̃��R�[�h�I�u�W�F�N�g���擾����
				targetRecord = mLogDataContainer.getRecordData(mIndexNumber);
			}

			// �\������ʂ���f�[�^���擾���A�I�u�W�F�N�g���X�V����
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

			// �e�̉�ʃf�[�^���X�V����
			mParent.refreshDataTable();
			return;
		}

		/// <summary>
		///   URL�������ɊJ���Ă݂�...
		/// </summary>
		private void btnOpenUrl_Click(object sender, System.EventArgs e)
		{
			try
			{
				// �L�����ꂽURL���J��...
				Process.Start(fldUrl.Text);
			}
			catch
			{
				// URL���Ó��Ȃ��̂ł͂Ȃ������A�Ȃɂ����܂���B
				// (�ʓ|�������̂ŁAhttp�v���g�R�����w�肳�ꂽ���ǂ�����
				//  �`�F�b�N�����Ȃ��B�B *.txt ���w�肷��ƁA��������
				//  �����オ��͂�...
				MessageBox.Show("Wrong URL? \n URL �� " + fldUrl.Text + "\n");
				return;
			}
			return;
		}

		/// <summary>
		///   �\�������R�[�h���폜���A�_�C�A���O�����
		/// </summary>
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
			mLogDataContainer.removeRecord(mIndexNumber);

			// �e��ʃf�[�^���X�V����
			mParent.refreshDataTable();

			// ��ʂ����
			this.Close();

			return;
		}


		/// <summary>
		///   �\�������R�[�h��o�^���A�_�C�A���O�����
		/// </summary>
		private void btnEntryData_Click(object sender, System.EventArgs e)
		{
			// ���ݕ\�����̃��R�[�h���X�V(�ۑ�)����...
			updateRecord();

			// ��ʂ����
			this.Close();

			return;
		}

		/// <summary>
		///   �\�������R�[�h�̕ҏW���~
		/// </summary>
		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// ��ʂ����
			this.Close();

			return;
		}
	}
}

