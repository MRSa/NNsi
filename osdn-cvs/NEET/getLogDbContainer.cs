/*
 *  $Id: getLogDbContainer.cs,v 1.6 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̎Q�ƃ��O�f�[�^�x�[�X�̋L�^�ꏊ...
 *
 *    getLogDbContainer()				�F �R���X�g���N�^
 *    setGetLogAccessor()				�F PalmDB�̃A�N�Z�T���擾����
 *    initializeDatas()					�F DB�t�@�C������I�u�W�F�N�g�ɓW�J
 *    numberOfGetLogRecords()			�F ���݂̎Q�ƃ��R�[�h�����擾
 *    getRecordData()					�F �I�u�W�F�N�g������
 *    newRecordData()					�F �V�K�̃I�u�W�F�N�g������
 *    selectLogDatabaseFileNameToSave()	�F �ۑ��p�c�a�t�@�C���������
 *    saveGetLogDatabase()				�F ���R�[�h��DB�t�@�C���ɕۑ�
 *
 */
using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;
using System.Net;

namespace NEET
{
	/// <summary>
	///  ���O�f�[�^�̃R���e�i�̐���
	/// </summary>
	public class getLogDbContainer : System.Windows.Forms.UserControl
	{
		/// <summary>
		///   �K�v�ȃf�U�C�i�ϐ��ł��B
		/// </summary>
		private System.ComponentModel.Container     components = null;
		private System.Windows.Forms.Label          label1;
		private System.Windows.Forms.OpenFileDialog fileSelectionDialog;


		// PalmOS��DB�t�@�C���̃A�N�Z�T(par.exe��wrapper�N���X)
		private PalmDatabaseAccessor mLogUrlDb             = null;

		// �Q�ƃ��O�c�a�̃��R�[�h�ƈꎞ�t�@�C���̕ϊ�
		private GetLogDbParser	mGetLogDbParser = null;

		// �Q�ƃ��O�c�a�̃��R�[�h��
		private int                  mNumberOfLogDbRecords = 0;

		// �Q�ƃ��O�c�a�̓��̓t�@�C����
		private string               mLogUrlDbName         = null;

		// �Q�ƃ��O�c�a�̏o�̓t�@�C����
		private string               mDbFileNameToSave     = "";

		// �Q�ƃ��O�c�a�̃I�u�W�F�N�g�Ǘ��p���X�g
		private ArrayList		mGetLogDatabaseArrayList = null;

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public getLogDbContainer()
		{
			// ���̌Ăяo���́CWindows.Forms �f�U�C�i�ŕK�v�ł��B
			InitializeComponent();

			//  �I�u�W�F�N�g�̏�����...
			mGetLogDatabaseArrayList = new ArrayList();
			mGetLogDbParser          = new GetLogDbParser();
		}

		/// <summary>
		///   �g�p����Ă��郊�\�[�X�̌㏈�������s���܂��B
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
					components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region �R���|�[�l���g �f�U�C�i�Ő������ꂽ�R�[�h
		/// <summary>
		/// �f�U�C�i�T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e�� 
		/// �R�[�h�G�f�B�^�ŕύX���Ȃ��ł��������B
		/// </summary>
		private void InitializeComponent()
		{
			this.label1 = new System.Windows.Forms.Label();
			this.fileSelectionDialog = new System.Windows.Forms.OpenFileDialog();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(0, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(184, 16);
			this.label1.TabIndex = 0;
			this.label1.Text = "�Q�ƃ��O�̃��R�[�h�R���e�i�N���X";
			// 
			// getLogDbContainer
			// 
			this.Controls.Add(this.label1);
			this.Name = "getLogDbContainer";
			this.Size = new System.Drawing.Size(168, 32);
			this.ResumeLayout(false);
		}
		#endregion

		/// <summary>
		///	  �Q�ƃ��O�c�a�̃A�N�Z�X�N���X��ݒ肷��
		/// </summary>
		public void setGetLogAccessor(string aDbName, ref PalmDatabaseAccessor aLogUrlDb)
		{
			mNumberOfLogDbRecords   = 0;
			mLogUrlDb               = aLogUrlDb;
			mLogUrlDbName           = aDbName;
			mDbFileNameToSave       = mLogUrlDbName;
			return;
		}

		/// <summary>
		///    �f�[�^�R���e�i�N���X������������
		///    (DB�t�@�C������f�[�^��ǂݏo���ăI�u�W�F�N�g�ɓW�J����)
		/// </summary>
		public bool initializeDatas()
		{
			// �c�a�̃w�b�_����ǂ�Ń��R�[�h�����擾����
			if (mLogUrlDb.getRecordNumber(out mNumberOfLogDbRecords) == false)
			{
				// DB�̓ǂݍ��ݎ��s�B�B�B(���������I������)
				MessageBox.Show("DB�t�@�C�� �܂��� par.exe�̏ꏊ��" +
								"�������w�肳��Ă��Ȃ��悤�ł��B\n" +
								"(�t�@�C���ݒ�̌����������肢���܂��B)",
								"Access Failure",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
				return (false);
			}

			// ���R�[�h���̊m�F���s���A���R�[�h�����f�[�^��ǂݍ���
			if (mNumberOfLogDbRecords == 0)
			{
				// ���R�[�h���Ȃ���������ǂݍ��܂Ȃ�...
				return (true);
			}

			// �ꎞ�t�@�C�����̎w��
			string tempFileName = mLogUrlDbName + ".temp";

			// ���R�[�h�̂��鐔���ǂݍ���ŃI�u�W�F�N�g�ɕϊ��A�o�^����
			for (int loop = 0; loop < mNumberOfLogDbRecords; loop++)
			{
				try
				{
					// par.exe���g�p����1���R�[�h���t�@�C���ɓW�J����
					if (mLogUrlDb.pickUpRecord(loop, tempFileName) != false)
					{
						// ���O���R�[�h�𐶐�
						logRecord entryRecord = new logRecord();

						// ���R�[�h����͂���
						mGetLogDbParser.inputRecordFromTemporaryFile(tempFileName, ref entryRecord);

						// ���R�[�h�����X�g�ɓo�^����
						mGetLogDatabaseArrayList.Add(entryRecord);
					}
				}
				catch
				{
					// �ϊ��Ɏ��s���Ă��������Ȃ�...
				}
			}

			try
			{
				// �ꎞ�t�@�C�����폜����
				File.Delete(tempFileName);
			}
			catch
			{
				// �폜���s...�ł��������Ȃ�...
			}
			return (true);
		}

		/// <summary>
		///   �Q�ƃ��O�c�a�̃��R�[�h����Ԃ�
		/// </summary>
		public int numberOfGetLogRecords()
		{
			return (mGetLogDatabaseArrayList.Count);
		}

		/// <summary>
		///   ���R�[�h�̏����擾����
		/// </summary>
		public logRecord getRecordData(int aIndexNumber)
		{
			if ((aIndexNumber < 0)&&
				(aIndexNumber >= mGetLogDatabaseArrayList.Count))
			{
				return (null);
			}
			return ((logRecord) mGetLogDatabaseArrayList[aIndexNumber]);
		}

		/// <summary>
		///    �V�K���R�[�h�̒ǉ�
		/// </summary>
		public logRecord newRecordData()
		{
			// ���O���R�[�h�𐶐�
			logRecord entryRecord = new logRecord();

			// �V�������R�[�h�I�u�W�F�N�g�����X�g�ɓo�^����
			mGetLogDatabaseArrayList.Add(entryRecord);

			return (entryRecord);
		}

		/// <summary>
		///   ���R�[�h���P���폜����
		/// </summary>
		public void removeRecord(int aIndexNumber)
		{
			logRecord targetRecord = (logRecord) mGetLogDatabaseArrayList[aIndexNumber];

			mGetLogDatabaseArrayList.Remove(targetRecord);
			return;
		}

		/// <summary>
		///   �ۑ��p�̃t�@�C�������w�肷��
		/// </summary>
		public void selectLogDatabaseFileNameToSave()
		{
			try
			{
				fileSelectionDialog.CheckFileExists = false;
				fileSelectionDialog.FileName = mDbFileNameToSave;
				fileSelectionDialog.ShowDialog();
				mDbFileNameToSave = fileSelectionDialog.FileName;
			}
			catch
			{
				// �������Ȃ�...
			}

		}

		/// <summary>
		///   ���O���R�[�h�Q��ۑ�����
		/// </summary>
		public bool saveGetLogDatabase()
		{
			// �ꎞ�t�@�C�����̎w��
			string tempFileName = mLogUrlDbName + ".temp";

			// �t�@�C�������݂����ꍇ�ɂ́Arename���ĕۊǂ���
			if (File.Exists(mDbFileNameToSave) == true)
			{
				try
				{
					File.Move(mDbFileNameToSave, mDbFileNameToSave + ".BAK");
				}
				catch
				{
					// �G���[�ɂȂ����Ƃ��͉������Ȃ�
				}
			}

			// �f�[�^�x�[�X�t�@�C����V�K�ɍ쐬����
			try
			{
				mLogUrlDb.createDatabase(mDbFileNameToSave,
										 mGetLogDbParser.getDbName(),
										 mGetLogDbParser.getDbType(),
										 mGetLogDbParser.getDbCreatorName(),
										 mGetLogDbParser.getDbVersion());
			}
			catch
			{
				// DB�t�@�C���V�K�쐬�Ɏ��s...
				MessageBox.Show("DB�t�@�C���̍쐬�Ɏ��s���܂����B",
								"�t�@�C���쐬���s",
								MessageBoxButtons.OK,
								MessageBoxIcon.Error);
				return (false);
			}

			// ���[�v���āA�ێ����Ă���I�u�W�F�N�g�S�āi�S���R�[�h�j���o��
			for (int loop = 0; loop < mGetLogDatabaseArrayList.Count; loop++)
			{
				logRecord theData = getRecordData(loop);
				try
				{
					// �ꎞ�t�@�C���ɏo�͌�ADB�t�@�C���ɏ����o��
					mGetLogDbParser.outputRecordToTemporaryFile(tempFileName, ref theData);
					mLogUrlDb.appendRecordToDatabase(mDbFileNameToSave, tempFileName);
				}
				catch
				{
					// �o�͎��s�A�A�A�ł��������Ȃ�...
				}
			}
			try
			{
				// �ꎞ�t�@�C�����폜����
				File.Delete(tempFileName);
			}
			catch
			{
				// �폜���s...�ł��������Ȃ�...
			}
			return (true);
		}
	}
}

