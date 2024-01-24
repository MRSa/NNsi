/*
 *  $Id: frmNEETsetting.cs,v 1.10 2005/07/18 04:33:19 mrsa Exp $
 *    --- NNsi��DB�t�@�C���Apar.exe�̏ꏊ���w�肵�Ă��炤���߂̏���
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
	///  frmNEETsetting �̊T�v�̐���
	///    NNsi��DB�t�@�C���Apar.exe�̏ꏊ����́A�񋟂���N���X�ł��B
	/// </summary>
	public class frmNEETsetting : System.Windows.Forms.Form
	{
		/// <summary>
		/// �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		private System.ComponentModel.Container components = null;
		private System.Windows.Forms.Button btnSelectGetLogDB;
		private System.Windows.Forms.TextBox parExeLocation;
		private System.Windows.Forms.Button btnSelectParExe;
		private System.Windows.Forms.GroupBox settings;
		private System.Windows.Forms.Label lblDescription;
		private System.Windows.Forms.OpenFileDialog selectFileDialog;
		private System.Windows.Forms.Button btnEditToken;
		private System.Windows.Forms.Button btnNGword;
		private System.Windows.Forms.Button btnBBS;
		private System.Windows.Forms.Label lblVersion;

		public frmNEETsetting()
		{
			//
			// Windows �t�H�[���f�U�C�i�T�|�[�g�ɕK�v�ł��B
			//
			InitializeComponent();

			//
			// TODO: InitializeComponent �Ăяo���̌�̃R���X�g���N�^�R�[�h��ǉ�
			//
			//
			//   (���̂Ƃ���A���ɕK�v�Ȃ�)
			//
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
			this.btnSelectGetLogDB = new System.Windows.Forms.Button();
			this.parExeLocation = new System.Windows.Forms.TextBox();
			this.btnSelectParExe = new System.Windows.Forms.Button();
			this.selectFileDialog = new System.Windows.Forms.OpenFileDialog();
			this.settings = new System.Windows.Forms.GroupBox();
			this.lblDescription = new System.Windows.Forms.Label();
			this.btnEditToken = new System.Windows.Forms.Button();
			this.btnNGword = new System.Windows.Forms.Button();
			this.btnBBS = new System.Windows.Forms.Button();
			this.lblVersion = new System.Windows.Forms.Label();
			this.settings.SuspendLayout();
			this.SuspendLayout();
			// 
			// btnSelectGetLogDB
			// 
			this.btnSelectGetLogDB.Location = new System.Drawing.Point(40, 32);
			this.btnSelectGetLogDB.Name = "btnSelectGetLogDB";
			this.btnSelectGetLogDB.Size = new System.Drawing.Size(152, 40);
			this.btnSelectGetLogDB.TabIndex = 12;
			this.btnSelectGetLogDB.Text = "�Q�ƃ��ODB�̕ҏW...   (LogPermanent-NNsi.pdb)";
			this.btnSelectGetLogDB.Click += new System.EventHandler(this.btnSelectGetLogDB_Click);
			// 
			// parExeLocation
			// 
			this.parExeLocation.Location = new System.Drawing.Point(8, 24);
			this.parExeLocation.Name = "parExeLocation";
			this.parExeLocation.Size = new System.Drawing.Size(312, 19);
			this.parExeLocation.TabIndex = 9;
			this.parExeLocation.Text = "C:\\APL\\par.exe";
			// 
			// btnSelectParExe
			// 
			this.btnSelectParExe.Location = new System.Drawing.Point(328, 22);
			this.btnSelectParExe.Name = "btnSelectParExe";
			this.btnSelectParExe.Size = new System.Drawing.Size(48, 23);
			this.btnSelectParExe.TabIndex = 8;
			this.btnSelectParExe.Text = "�Q��...";
			this.btnSelectParExe.Click += new System.EventHandler(this.btnSelectParExe_Click);
			// 
			// settings
			// 
			this.settings.Controls.Add(this.btnSelectParExe);
			this.settings.Controls.Add(this.parExeLocation);
			this.settings.Location = new System.Drawing.Point(8, 144);
			this.settings.Name = "settings";
			this.settings.Size = new System.Drawing.Size(392, 56);
			this.settings.TabIndex = 11;
			this.settings.TabStop = false;
			this.settings.Text = "par.exe�̏ꏊ";
			// 
			// lblDescription
			// 
			this.lblDescription.Font = new System.Drawing.Font("MS UI Gothic", 10F);
			this.lblDescription.Location = new System.Drawing.Point(8, 8);
			this.lblDescription.Name = "lblDescription";
			this.lblDescription.Size = new System.Drawing.Size(400, 16);
			this.lblDescription.TabIndex = 16;
			this.lblDescription.Text = "��  par.exe������ꏊ��ݒ肵�A�c�a�ҏW�{�^���������Ă��������B  ��";
			this.lblDescription.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// btnEditToken
			// 
			this.btnEditToken.Location = new System.Drawing.Point(40, 88);
			this.btnEditToken.Name = "btnEditToken";
			this.btnEditToken.Size = new System.Drawing.Size(152, 40);
			this.btnEditToken.TabIndex = 17;
			this.btnEditToken.Text = "��؂蕶��DB�̕ҏW...   (LogToken-NNsi.pdb)";
			this.btnEditToken.Click += new System.EventHandler(this.btnEditToken_Click);
			// 
			// btnNGword
			// 
			this.btnNGword.Location = new System.Drawing.Point(216, 88);
			this.btnNGword.Name = "btnNGword";
			this.btnNGword.Size = new System.Drawing.Size(152, 40);
			this.btnNGword.TabIndex = 18;
			this.btnNGword.Text = "NG���[�hDB�̕ҏW...   (NGWord-NNsi.pdb)";
			this.btnNGword.Click += new System.EventHandler(this.btnNGword_Click);
			// 
			// btnBBS
			// 
			this.btnBBS.Location = new System.Drawing.Point(216, 32);
			this.btnBBS.Name = "btnBBS";
			this.btnBBS.Size = new System.Drawing.Size(152, 40);
			this.btnBBS.TabIndex = 19;
			this.btnBBS.Text = "��(BBS)�ꗗDB�̕ҏW...   (BBS-NNsi.pdb)";
			this.btnBBS.Click += new System.EventHandler(this.btnBBS_Click);
			// 
			// lblVersion
			// 
			this.lblVersion.Enabled = false;
			this.lblVersion.Location = new System.Drawing.Point(152, 208);
			this.lblVersion.Name = "lblVersion";
			this.lblVersion.Size = new System.Drawing.Size(248, 16);
			this.lblVersion.TabIndex = 20;
			this.lblVersion.Text = "NEET (Phase2) Version 0.21 Revision000";
			this.lblVersion.TextAlign = System.Drawing.ContentAlignment.BottomRight;
			// 
			// frmNEETsetting
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 12);
			this.ClientSize = new System.Drawing.Size(416, 229);
			this.Controls.Add(this.lblVersion);
			this.Controls.Add(this.btnBBS);
			this.Controls.Add(this.btnNGword);
			this.Controls.Add(this.btnEditToken);
			this.Controls.Add(this.settings);
			this.Controls.Add(this.btnSelectGetLogDB);
			this.Controls.Add(this.lblDescription);
			this.Name = "frmNEETsetting";
			this.Text = "NEET  -- a NNsi\'s Expanding and Editing Tool ";
			this.settings.ResumeLayout(false);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///   par.exe �̂���ꏊ��I������ (�t�@�C���I���_�C�A���O��\��)
		/// </summary>
		private void btnSelectParExe_Click(object sender, System.EventArgs e)
		{
			selectFileDialog.ShowDialog();
			parExeLocation.Text = selectFileDialog.FileName;
			return;
		}

		/// <summary>
		///   �Q�ƃ��O�c�a�̂���ꏊ��I�����A�ҏW���J�n����
		/// </summary>
		private void btnSelectGetLogDB_Click(object sender, System.EventArgs e)
		{
			// �Q�ƃ��ODB�̃t�@�C���I���_�C�A���O��\������
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// �L�����Z�����ɂ͉������Ȃ�...
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// �Q�ƃ��O�A�N�Z�X�N���X�𐶐����A�f�[�^�R���e�i�N���X�ɓn��
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// �f�[�^�R���e�i�N���X�̏�����
			getLogDbContainer logContainer = new getLogDbContainer();
			logContainer.setGetLogAccessor(targetFileName, ref dbAccessor);
			if (logContainer.initializeDatas() != true)
			{
				// �f�[�^�N���X�̏������Ɏ��s�����A�A�A�I������
				return;
			}

			////////////////////////////////////////////////////////////////
			//   �ꗗ�_�C�A���O�𐶐����ĊJ������
			////////////////////////////////////////////////////////////////

			// �ꗗ�_�C�A���O�𐶐�����
			frmLogList listDialog = new frmLogList(this);

			// ���O�R���e�i�N���X���ꗗ�_�C�A���O�ɓn��
			listDialog.setLogContainer(ref logContainer);

			// �f�[�^�Z�b�g����������
			listDialog.prepareDataset();

			// �Q�ƃ��O�c�a�ҏW�_�C�A���O���J��
			//  (�ҏW�I���܂ő҂A���̊ԓ����{�^����������Ȃ��悤�ɂ���)
			getLogButtonControl(false);
			listDialog.ShowDialog();
			getLogButtonControl(true);

			return;
		}

		/// <summary>
		///   Par.Exe�̃t�@�C�������������� �i��΃p�X��)
		/// </summary>
		public string getParExeLocation()
		{
			return (parExeLocation.Text);
		}

		///  <summary>
		///	�{�^�������L���ɂ���
		///  </summary>
		public void getLogButtonControl(bool aTF)
		{
			btnSelectGetLogDB.Enabled = aTF;
		}


		/// <summary>
		///   ��؂蕶���c�a�̕ҏW...
		/// </summary>
		private void btnEditToken_Click(object sender, System.EventArgs e)
		{
			// ��؂蕶��DB�̃t�@�C���I���_�C�A���O��\������
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// �L�����Z�����ꂽ�Ƃ��ɂ͉������Ȃ��B
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// ��؂蕶���c�a�A�N�Z�X�N���X�𐶐����A�f�[�^�R���e�i�N���X�ɓn��
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// �f�[�^�R���e�i�N���X�̏�����
			logTokenDbContainer tokenDbContainer = new logTokenDbContainer(targetFileName, ref dbAccessor);
			if (tokenDbContainer.initializeDatas() != true)
			{
				// �f�[�^�N���X�̏������Ɏ��s�����A�A�A�I������
				return;
			}

			// �ꗗ�_�C�A���O�𐶐�����
			logTokenList listDialog = new logTokenList(this, ref tokenDbContainer);

			// �f�[�^�Z�b�g����������
			listDialog.prepareDataset();

			// ��؂蕶���ҏW�_�C�A���O���J��
			//  (�ҏW�I���܂ő҂A���̊ԓ����{�^����������Ȃ��悤�ɂ���)
			btnEditToken.Enabled = false;
			listDialog.ShowDialog();
			btnEditToken.Enabled = true;

			return;
		}

		/// <summary>
		/// </summary>
		private void btnBBS_Click(object sender, System.EventArgs e)
		{
			// ��؂蕶��DB�̃t�@�C���I���_�C�A���O��\������
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// �L�����Z�����ꂽ�Ƃ��ɂ͉������Ȃ��B
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// BBS
			btnBBS.Enabled = false;

			// ��؂蕶���c�a�A�N�Z�X�N���X�𐶐����A�f�[�^�R���e�i�N���X�ɓn��
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// �f�[�^�R���e�i�N���X�̏�����
			bbsDbContainer bbsContainer = new bbsDbContainer(targetFileName, ref dbAccessor);
			if (bbsContainer.initializeDatas() != true)
			{
				// �f�[�^�N���X�̏������Ɏ��s�����A�A�A�I������
				return;
			}

			// �ꗗ�_�C�A���O�𐶐�����
			bbsList listDialog = new bbsList(this, ref bbsContainer);

			// �f�[�^�Z�b�g����������
			listDialog.prepareDataset();

			//frmBBSselectURL bbsSettingDialog = new frmBBSselectURL(parExeLocation.Text);

			listDialog.ShowDialog();
			btnBBS.Enabled = true;
		}

		/// <summary>
		/// </summary>
		private void btnNGword_Click(object sender, System.EventArgs e)
		{
			// NG���[�hDB�̃t�@�C���I���_�C�A���O��\������
			if (selectFileDialog.ShowDialog() == DialogResult.Cancel)
			{
				// �L�����Z�����ꂽ�Ƃ��ɂ͉������Ȃ��B
				return;
			}
			string targetFileName = selectFileDialog.FileName;

			// NG���[�h�c�a�A�N�Z�X�N���X�𐶐����A�f�[�^�R���e�i�N���X�ɓn��
			PalmDatabaseAccessor dbAccessor = new PalmDatabaseAccessor(parExeLocation.Text, targetFileName);

			// �f�[�^�R���e�i�N���X�̏�����
			NGwordContainer ngWordContainer = new NGwordContainer(targetFileName, ref dbAccessor);
			if (ngWordContainer.initializeDatas() != true)
			{
				// �f�[�^�N���X�̏������Ɏ��s�����A�A�A�I������
				return;
			}

			// �ꗗ�_�C�A���O�𐶐�����
			frmNGwordEdit listDialog = new frmNGwordEdit(this, ref ngWordContainer);

			// �f�[�^�Z�b�g����������
			listDialog.prepareDataset();

			// ��؂蕶���ҏW�_�C�A���O���J��
			//  (�ҏW�I���܂ő҂A���̊ԓ����{�^����������Ȃ��悤�ɂ���)
			btnNGword.Enabled = false;
			listDialog.ShowDialog();
			btnNGword.Enabled = true;
		}
	}

/*****

#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16
#define MAX_THREADFILENAME          32
#define MAX_GETLOG_URL             244

// ���(�f�[�^�x�[�X�̍\��)
typedef struct {
	Char   boardNick[MAX_NICKNAME];    // �{�[�h�j�b�N�l�[��
	Char   boardURL [MAX_URL];         // �{�[�hURL
	Char   boardName[MAX_BOARDNAME];   // �{�[�h��
	UInt8  bbsType;                    // �{�[�hTYPE
	UInt8  state;                      // �{�[�h�g�p���
	Int16  reserved;                   // (�\��̈�F�ȑO�͎g�p���Ă���)
} NNshBoardDatabase;

// �Q�ƃ��O�i�P���t�@�C���j�̍\����
typedef struct {
	Char   dataURL     [MAX_GETLOG_URL];      // �f�[�^�擾��URL
	Char   dataFileName[MAX_THREADFILENAME];  // �f�[�^�t�@�C����
	UInt32 lastUpdate;                        // �ŏI�X�V����
	UInt32 parentDbId;                        // �e�̃��R�[�hID(���j�[�NID)
	UInt32 logDbId;                           // ���O�̃��R�[�hID(���j�[�NID)
	UInt32 tokenId;                           // ��؂蕶���̃��R�[�hID(���j�[�NID)
	UInt32 reserve0;                          // �\��̈�
	UInt32 reserve1;                          // �\��̈�
	UInt32 reserve2;                          // �\��̈�
	UInt32 reserve3;                          // �\��̈�
} NNshGetLogSubFileDatabase;

*****/

}
