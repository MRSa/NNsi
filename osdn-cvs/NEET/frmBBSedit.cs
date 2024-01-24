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
	public class frmBBSedit : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.TextBox fldBoardName;
		private System.Windows.Forms.Label lblBBSname;
		private System.Windows.Forms.TextBox fldURL;
		private System.Windows.Forms.Label lblURL;
		private System.Windows.Forms.Label lblNick;
		private System.Windows.Forms.TextBox fldNickName;
		private System.Windows.Forms.GroupBox grpBBStype;
		private System.Windows.Forms.RadioButton radioMachi;
		private System.Windows.Forms.RadioButton radio2ch;
		private System.Windows.Forms.RadioButton radioShitaraba;
		private System.Windows.Forms.RadioButton radioJBBS;
		private System.Windows.Forms.RadioButton radioCompat;
		private System.Windows.Forms.RadioButton radioCompatNi;
		private System.Windows.Forms.RadioButton radio2chEUC;
		private System.Windows.Forms.CheckBox chkGetStop;
		private System.Windows.Forms.GroupBox grpLevel;
		private System.Windows.Forms.RadioButton LevelNormal;
		private System.Windows.Forms.RadioButton Level1;
		private System.Windows.Forms.RadioButton Level2;
		private System.Windows.Forms.RadioButton Level3;
		private System.Windows.Forms.RadioButton Level4;
		private System.Windows.Forms.RadioButton Level5;
		private System.Windows.Forms.Button btnEntryData;
		private System.Windows.Forms.Button btnCancelEdit;
		private System.Windows.Forms.Button btnDeleteRecord;
		private System.Windows.Forms.Label lblCaution;
		private System.Windows.Forms.Label lblNotice;

		private bbsList		        mParent			 = null;
		private bbsDbContainer      mDbContainer	 = null;
		private bbsRecord           mEditRecord		 = null;
		private int					mIndexNumber     = 0;
		private int					mMaxRecordNumber = 0;
		private System.Windows.Forms.Label lblRecordNumber;

		const int    NNSH_BBSSTATE_VISIBLE   = 0x01;
		const int    NNSH_BBSSTATE_INVISIBLE = 0x00;

		const int    NNSH_BBSSTATE_LEVELMASK = 0x0e;
		const int    NNSH_BBSSTATE_NOTFAVOR  = 0xf1;
		const int    NNSH_BBSSTATE_FAVOR_L1  = 0x02;
		const int    NNSH_BBSSTATE_FAVOR_L2  = 0x04;
		const int    NNSH_BBSSTATE_FAVOR_L3  = 0x06;
		const int    NNSH_BBSSTATE_FAVOR_L4  = 0x08;
		const int    NNSH_BBSSTATE_FAVOR     = 0x0e;

		const int    NNSH_BBSSTATE_SUSPEND   = 0x10;

		const int    NNSH_BBSTYPE_2ch                = 0x00; // 2�����˂�
		const int    NNSH_BBSTYPE_MACHIBBS           = 0x01; // �܂�BBS
		const int    NNSH_BBSTYPE_SHITARABA          = 0x02; // �������BBS
		const int    NNSH_BBSTYPE_SHITARABAJBBS_OLD  = 0x03; // (�� JBBS)
		const int    NNSH_BBSTYPE_OTHERBBS           = 0x04; // 2ch�݊�BBS
		const int    NNSH_BBSTYPE_PLUGINSUPPORT      = 0x05; // 2ch�`���ϊ�BBS
		const int    NNSH_BBSTYPE_HTML               = 0x06; // html�`���f�[�^
		const int    NNSH_BBSTYPE_SHITARABAJBBS_RAW  = 0x07; // �������@JBBS
		const int    NNSH_BBSTYPE_PLAINTEXT          = 0x08; // �e�L�X�g�`��
		const int    NNSH_BBSTYPE_2ch_EUC            = 0x09; // 2ch(EUC�`��)
		const int    NNSH_BBSTYPE_OTHERBBS_2         = 0x0a;
		private System.Windows.Forms.CheckBox chkUseBBS; // 2ch�݊�(����2)

		public frmBBSedit(bbsList aParent, ref bbsDbContainer arDbContainer)
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			mParent           = aParent;
			mDbContainer      = arDbContainer;
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
			this.fldBoardName = new System.Windows.Forms.TextBox();
			this.lblBBSname = new System.Windows.Forms.Label();
			this.fldURL = new System.Windows.Forms.TextBox();
			this.lblURL = new System.Windows.Forms.Label();
			this.lblNick = new System.Windows.Forms.Label();
			this.fldNickName = new System.Windows.Forms.TextBox();
			this.grpBBStype = new System.Windows.Forms.GroupBox();
			this.radio2chEUC = new System.Windows.Forms.RadioButton();
			this.radioCompatNi = new System.Windows.Forms.RadioButton();
			this.radioCompat = new System.Windows.Forms.RadioButton();
			this.radioJBBS = new System.Windows.Forms.RadioButton();
			this.radioShitaraba = new System.Windows.Forms.RadioButton();
			this.radio2ch = new System.Windows.Forms.RadioButton();
			this.radioMachi = new System.Windows.Forms.RadioButton();
			this.chkGetStop = new System.Windows.Forms.CheckBox();
			this.grpLevel = new System.Windows.Forms.GroupBox();
			this.Level5 = new System.Windows.Forms.RadioButton();
			this.LevelNormal = new System.Windows.Forms.RadioButton();
			this.Level1 = new System.Windows.Forms.RadioButton();
			this.Level2 = new System.Windows.Forms.RadioButton();
			this.Level3 = new System.Windows.Forms.RadioButton();
			this.Level4 = new System.Windows.Forms.RadioButton();
			this.btnEntryData = new System.Windows.Forms.Button();
			this.btnCancelEdit = new System.Windows.Forms.Button();
			this.btnDeleteRecord = new System.Windows.Forms.Button();
			this.lblCaution = new System.Windows.Forms.Label();
			this.lblNotice = new System.Windows.Forms.Label();
			this.lblRecordNumber = new System.Windows.Forms.Label();
			this.chkUseBBS = new System.Windows.Forms.CheckBox();
			this.grpBBStype.SuspendLayout();
			this.grpLevel.SuspendLayout();
			this.SuspendLayout();
			// 
			// fldBoardName
			// 
			this.fldBoardName.Location = new System.Drawing.Point(74, 8);
			this.fldBoardName.Name = "fldBoardName";
			this.fldBoardName.Size = new System.Drawing.Size(254, 19);
			this.fldBoardName.TabIndex = 0;
			this.fldBoardName.Text = "(����)";
			// 
			// lblBBSname
			// 
			this.lblBBSname.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lblBBSname.Location = new System.Drawing.Point(14, 12);
			this.lblBBSname.Name = "lblBBSname";
			this.lblBBSname.Size = new System.Drawing.Size(56, 12);
			this.lblBBSname.TabIndex = 1;
			this.lblBBSname.Text = "BBS����";
			// 
			// fldURL
			// 
			this.fldURL.Location = new System.Drawing.Point(74, 36);
			this.fldURL.Name = "fldURL";
			this.fldURL.Size = new System.Drawing.Size(526, 19);
			this.fldURL.TabIndex = 2;
			this.fldURL.Text = "(��URL)";
			// 
			// lblURL
			// 
			this.lblURL.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lblURL.Location = new System.Drawing.Point(14, 39);
			this.lblURL.Name = "lblURL";
			this.lblURL.Size = new System.Drawing.Size(56, 12);
			this.lblURL.TabIndex = 3;
			this.lblURL.Text = "URL";
			// 
			// lblNick
			// 
			this.lblNick.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lblNick.Location = new System.Drawing.Point(344, 12);
			this.lblNick.Name = "lblNick";
			this.lblNick.Size = new System.Drawing.Size(56, 12);
			this.lblNick.TabIndex = 4;
			this.lblNick.Text = "NickName";
			// 
			// fldNickName
			// 
			this.fldNickName.Location = new System.Drawing.Point(408, 8);
			this.fldNickName.Name = "fldNickName";
			this.fldNickName.Size = new System.Drawing.Size(192, 19);
			this.fldNickName.TabIndex = 5;
			this.fldNickName.Text = "(��Nick)";
			// 
			// grpBBStype
			// 
			this.grpBBStype.Controls.Add(this.radio2chEUC);
			this.grpBBStype.Controls.Add(this.radioCompatNi);
			this.grpBBStype.Controls.Add(this.radioCompat);
			this.grpBBStype.Controls.Add(this.radioJBBS);
			this.grpBBStype.Controls.Add(this.radioShitaraba);
			this.grpBBStype.Controls.Add(this.radio2ch);
			this.grpBBStype.Controls.Add(this.radioMachi);
			this.grpBBStype.Location = new System.Drawing.Point(8, 64);
			this.grpBBStype.Name = "grpBBStype";
			this.grpBBStype.Size = new System.Drawing.Size(592, 48);
			this.grpBBStype.TabIndex = 6;
			this.grpBBStype.TabStop = false;
			this.grpBBStype.Text = "�^�C�v";
			// 
			// radio2chEUC
			// 
			this.radio2chEUC.Location = new System.Drawing.Point(488, 16);
			this.radio2chEUC.Name = "radio2chEUC";
			this.radio2chEUC.Size = new System.Drawing.Size(80, 24);
			this.radio2chEUC.TabIndex = 6;
			this.radio2chEUC.Text = "2ch (EUC)";
			// 
			// radioCompatNi
			// 
			this.radioCompatNi.Location = new System.Drawing.Point(400, 16);
			this.radioCompatNi.Name = "radioCompatNi";
			this.radioCompatNi.Size = new System.Drawing.Size(88, 24);
			this.radioCompatNi.TabIndex = 5;
			this.radioCompatNi.Text = "2ch�݊�[Ni]";
			// 
			// radioCompat
			// 
			this.radioCompat.Location = new System.Drawing.Point(328, 16);
			this.radioCompat.Name = "radioCompat";
			this.radioCompat.Size = new System.Drawing.Size(72, 24);
			this.radioCompat.TabIndex = 4;
			this.radioCompat.Text = "2ch�݊�";
			// 
			// radioJBBS
			// 
			this.radioJBBS.Location = new System.Drawing.Point(224, 16);
			this.radioJBBS.Name = "radioJBBS";
			this.radioJBBS.TabIndex = 3;
			this.radioJBBS.Text = "������΁�JBBS";
			// 
			// radioShitaraba
			// 
			this.radioShitaraba.Location = new System.Drawing.Point(160, 16);
			this.radioShitaraba.Name = "radioShitaraba";
			this.radioShitaraba.Size = new System.Drawing.Size(64, 24);
			this.radioShitaraba.TabIndex = 2;
			this.radioShitaraba.Text = "�������";
			// 
			// radio2ch
			// 
			this.radio2ch.Location = new System.Drawing.Point(8, 15);
			this.radio2ch.Name = "radio2ch";
			this.radio2ch.Size = new System.Drawing.Size(80, 24);
			this.radio2ch.TabIndex = 1;
			this.radio2ch.Text = "�Q�����˂�";
			// 
			// radioMachi
			// 
			this.radioMachi.Location = new System.Drawing.Point(88, 16);
			this.radioMachi.Name = "radioMachi";
			this.radioMachi.Size = new System.Drawing.Size(72, 24);
			this.radioMachi.TabIndex = 0;
			this.radioMachi.Text = "�܂�BBS";
			// 
			// chkGetStop
			// 
			this.chkGetStop.Location = new System.Drawing.Point(320, 144);
			this.chkGetStop.Name = "chkGetStop";
			this.chkGetStop.Size = new System.Drawing.Size(112, 24);
			this.chkGetStop.TabIndex = 7;
			this.chkGetStop.Text = "�X���擾��~��";
			// 
			// grpLevel
			// 
			this.grpLevel.Controls.Add(this.Level5);
			this.grpLevel.Controls.Add(this.LevelNormal);
			this.grpLevel.Controls.Add(this.Level1);
			this.grpLevel.Controls.Add(this.Level2);
			this.grpLevel.Controls.Add(this.Level3);
			this.grpLevel.Controls.Add(this.Level4);
			this.grpLevel.Location = new System.Drawing.Point(8, 120);
			this.grpLevel.Name = "grpLevel";
			this.grpLevel.Size = new System.Drawing.Size(296, 48);
			this.grpLevel.TabIndex = 8;
			this.grpLevel.TabStop = false;
			this.grpLevel.Text = "�X�����x��";
			// 
			// Level5
			// 
			this.Level5.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Level5.Location = new System.Drawing.Point(248, 16);
			this.Level5.Name = "Level5";
			this.Level5.Size = new System.Drawing.Size(40, 24);
			this.Level5.TabIndex = 10;
			this.Level5.Text = "H";
			// 
			// LevelNormal
			// 
			this.LevelNormal.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.LevelNormal.Location = new System.Drawing.Point(8, 16);
			this.LevelNormal.Name = "LevelNormal";
			this.LevelNormal.Size = new System.Drawing.Size(32, 24);
			this.LevelNormal.TabIndex = 0;
			this.LevelNormal.Text = "-";
			// 
			// Level1
			// 
			this.Level1.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Level1.Location = new System.Drawing.Point(56, 16);
			this.Level1.Name = "Level1";
			this.Level1.Size = new System.Drawing.Size(32, 24);
			this.Level1.TabIndex = 9;
			this.Level1.Text = "L";
			// 
			// Level2
			// 
			this.Level2.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Level2.Location = new System.Drawing.Point(104, 16);
			this.Level2.Name = "Level2";
			this.Level2.Size = new System.Drawing.Size(32, 24);
			this.Level2.TabIndex = 9;
			this.Level2.Text = "1";
			// 
			// Level3
			// 
			this.Level3.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Level3.Location = new System.Drawing.Point(152, 16);
			this.Level3.Name = "Level3";
			this.Level3.Size = new System.Drawing.Size(32, 24);
			this.Level3.TabIndex = 9;
			this.Level3.Text = "2";
			// 
			// Level4
			// 
			this.Level4.Font = new System.Drawing.Font("MS UI Gothic", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.Level4.Location = new System.Drawing.Point(200, 16);
			this.Level4.Name = "Level4";
			this.Level4.Size = new System.Drawing.Size(32, 24);
			this.Level4.TabIndex = 9;
			this.Level4.Text = "3";
			// 
			// btnEntryData
			// 
			this.btnEntryData.Location = new System.Drawing.Point(8, 232);
			this.btnEntryData.Name = "btnEntryData";
			this.btnEntryData.Size = new System.Drawing.Size(96, 32);
			this.btnEntryData.TabIndex = 18;
			this.btnEntryData.Text = "�f�[�^�o�^";
			this.btnEntryData.Click += new System.EventHandler(this.btnEntryData_Click);
			// 
			// btnCancelEdit
			// 
			this.btnCancelEdit.Location = new System.Drawing.Point(128, 232);
			this.btnCancelEdit.Name = "btnCancelEdit";
			this.btnCancelEdit.Size = new System.Drawing.Size(104, 32);
			this.btnCancelEdit.TabIndex = 19;
			this.btnCancelEdit.Text = "�ҏW���~";
			this.btnCancelEdit.Click += new System.EventHandler(this.btnCancelEdit_Click);
			// 
			// btnDeleteRecord
			// 
			this.btnDeleteRecord.Location = new System.Drawing.Point(496, 232);
			this.btnDeleteRecord.Name = "btnDeleteRecord";
			this.btnDeleteRecord.Size = new System.Drawing.Size(104, 32);
			this.btnDeleteRecord.TabIndex = 20;
			this.btnDeleteRecord.Text = "���R�[�h�폜";
			this.btnDeleteRecord.Click += new System.EventHandler(this.btnDeleteRecord_Click);
			// 
			// lblCaution
			// 
			this.lblCaution.Location = new System.Drawing.Point(16, 200);
			this.lblCaution.Name = "lblCaution";
			this.lblCaution.Size = new System.Drawing.Size(584, 16);
			this.lblCaution.TabIndex = 26;
			this.lblCaution.Text = "��Nick�̖����ɂ́A \'/\' ���K�v�ł��B";
			// 
			// lblNotice
			// 
			this.lblNotice.Font = new System.Drawing.Font("�l�r �S�V�b�N", 11.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
			this.lblNotice.Location = new System.Drawing.Point(8, 176);
			this.lblNotice.Name = "lblNotice";
			this.lblNotice.Size = new System.Drawing.Size(88, 16);
			this.lblNotice.TabIndex = 27;
			this.lblNotice.Text = "�� ���� ��";
			// 
			// lblRecordNumber
			// 
			this.lblRecordNumber.Location = new System.Drawing.Point(504, 120);
			this.lblRecordNumber.Name = "lblRecordNumber";
			this.lblRecordNumber.Size = new System.Drawing.Size(96, 16);
			this.lblRecordNumber.TabIndex = 28;
			this.lblRecordNumber.Text = "9999/9999";
			this.lblRecordNumber.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			// 
			// chkUseBBS
			// 
			this.chkUseBBS.Location = new System.Drawing.Point(320, 120);
			this.chkUseBBS.Name = "chkUseBBS";
			this.chkUseBBS.Size = new System.Drawing.Size(168, 24);
			this.chkUseBBS.TabIndex = 29;
			this.chkUseBBS.Text = "BBS�ꗗ(�^�u)�ɕ\������";
			// 
			// frmBBSedit
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(616, 270);
			this.Controls.Add(this.chkUseBBS);
			this.Controls.Add(this.lblRecordNumber);
			this.Controls.Add(this.lblNotice);
			this.Controls.Add(this.lblCaution);
			this.Controls.Add(this.btnDeleteRecord);
			this.Controls.Add(this.btnCancelEdit);
			this.Controls.Add(this.btnEntryData);
			this.Controls.Add(this.grpLevel);
			this.Controls.Add(this.chkGetStop);
			this.Controls.Add(this.grpBBStype);
			this.Controls.Add(this.fldNickName);
			this.Controls.Add(this.lblNick);
			this.Controls.Add(this.lblURL);
			this.Controls.Add(this.fldURL);
			this.Controls.Add(this.lblBBSname);
			this.Controls.Add(this.fldBoardName);
			this.Name = "frmBBSedit";
			this.Text = "BBS�f�[�^�̕ҏW";
			this.grpBBStype.ResumeLayout(false);
			this.grpLevel.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		private void btnCancelEdit_Click(object sender, System.EventArgs e)
		{
			// ��ʂ����
			this.Close();
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
			fldBoardName.Text = mEditRecord.boardName;
			fldNickName.Text  = mEditRecord.boardNick;
			fldURL.Text       = mEditRecord.boardURL;

			// �^�C�v
			switch (mEditRecord.bbsType)
			{
			  case NNSH_BBSTYPE_2ch:
				radio2ch.Checked = true;
				break;

			  case NNSH_BBSTYPE_MACHIBBS:
				radioMachi.Checked = true;
				break;

			  case NNSH_BBSTYPE_SHITARABA:
				radioShitaraba.Checked = true;
				break;

			  case NNSH_BBSTYPE_OTHERBBS:
				radioCompat.Checked = true;
				break;

			  case NNSH_BBSTYPE_SHITARABAJBBS_RAW:
				radioJBBS.Checked = true;
				break;

			  case NNSH_BBSTYPE_OTHERBBS_2:
				radioCompatNi.Checked = true;
				break;

			  case NNSH_BBSTYPE_2ch_EUC:
				radio2chEUC.Checked = true;
				break;

			  default:
				break;
			}

			// �X�����x��
			switch (mEditRecord.favoriteLevelNumber)
			{
			  case 5:
				Level5.Checked = true;
				break;

			  case 4:
				Level4.Checked = true;
				break;

			  case 3:
				Level3.Checked = true;
				break;

			  case 2:
				Level2.Checked = true;
				break;

			  case 1:
				Level1.Checked = true;
				break;

			  default:
				LevelNormal.Checked = true;
				break;
			}

			// use
			if (mEditRecord.useBoardList == true)
			{
				chkUseBBS.Checked = true;
			}

			// �擾��~��...
			if (mEditRecord.suspendMode == true)
			{
				chkGetStop.Checked = true;
			}

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
			mDbContainer.removeRecord(mIndexNumber);

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
			bbsRecord targetRecord = null;

			if (mIndexNumber == mMaxRecordNumber)
			{
				// �V�������R�[�h�I�u�W�F�N�g���擾����
				targetRecord      = mDbContainer.newRecordData();
				mMaxRecordNumber  = mDbContainer.numberOfRecords();
			}
			else
			{
				// ���݂̕\�����̃��R�[�h�I�u�W�F�N�g���擾����
				targetRecord = mDbContainer.getRecordData(mIndexNumber);
			}

			// �f�[�^��o�^����...
			// �t�B�[���h�Ƀf�[�^�𔽉f������
			mEditRecord.boardName = fldBoardName.Text;
			mEditRecord.boardNick = fldNickName.Text;
			mEditRecord.boardURL = fldURL.Text;

			// �^�C�v
			if (radioMachi.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_MACHIBBS;
			}
			if (radioShitaraba.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_SHITARABA;
			}
			if (radioCompat.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_OTHERBBS;
			}
			if (radioJBBS.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_SHITARABAJBBS_RAW;
			}
			if (radioCompatNi.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_OTHERBBS_2;
			}
			if (radio2chEUC.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_2ch_EUC;
			}
			if (radio2ch.Checked == true)
			{
				mEditRecord.bbsType = NNSH_BBSTYPE_2ch;
			}

			// �X�����x��
			if (Level1.Checked == true)
			{
				mEditRecord.favoriteLevelNumber = 1;
			}
			if (Level2.Checked == true)
			{
				mEditRecord.favoriteLevelNumber = 2;
			}
			if (Level3.Checked == true)
			{
				mEditRecord.favoriteLevelNumber = 3;
			}
			if (Level4.Checked == true)
			{
				mEditRecord.favoriteLevelNumber = 4;
			}
			if (Level5.Checked == true)
			{
				mEditRecord.favoriteLevelNumber = 5;
			}
			if (LevelNormal.Checked == true)
			{
				mEditRecord.favoriteLevelNumber = 0;
			}

			// use
			if (chkUseBBS.Checked == true)
			{
				mEditRecord.useBoardList = true;
			}
			else
			{
            	mEditRecord.useBoardList = false;
			}

			// �擾��~��...
			if (chkGetStop.Checked == true)
			{
				mEditRecord.suspendMode = true;
			}
			else
			{
				mEditRecord.suspendMode = false;
			}

			// �e�̉�ʃf�[�^���X�V����
			mParent.refreshDataTable();

			return;
		}

	}
}
