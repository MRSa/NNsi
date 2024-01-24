/*
 *  $Id: NGwordRecords.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi��NG���[�h�f�[�^
 */
using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.IO;

namespace NEET
{
	public class NGwordRecord
	{
		private string mNGword;
		private uint   mCheckArea;
		private uint   mMatchedAction;
		private int    mReserved;

		/// <summary>
		///   �v���p�e�B (NG word)
		/// </summary>
		public string ngWord
		{
			get
			{
				return (mNGword);
			}

			set
			{
				mNGword = value;
			}
		}

		/// <summary>
		///   �v���p�e�B (check Area)
		/// </summary>
		public uint checkArea
		{
			get
			{
				return (mCheckArea);
			}

			set
			{
				mCheckArea = value;
			}
		}

		/// <summary>
		///   �v���p�e�B (Mached Action)
		/// </summary>
		public uint matchedAction
		{
			get
			{
				return (mMatchedAction);
			}

			set
			{
				mMatchedAction = value;
			}
		}

		/// <summary>
		///   �v���p�e�B (reserved)
		/// </summary>
		public int reserved
		{
			get
			{
				return (mReserved);
			}

			set
			{
				mReserved = value;
			}
		}

	}

	/// <summary>
	///  Class �̊T�v�̐���
	/// </summary>
	public class NGwordContainer
	{
		// �u�t�@�C�����J���v�_�C�A���O
		private System.Windows.Forms.OpenFileDialog fileSelectionDialog = null;

		// PalmOS��DB�t�@�C���̃A�N�Z�T(par.exe��wrapper�N���X)
		private PalmDatabaseAccessor mDbAccessor		= null;

		// NG���[�h�c�a�̃��R�[�h�ƈꎞ�t�@�C���̕ϊ�
		private NGwordDbParser 	mDbParser				= null;

		// NG���[�h�c�a�̓��̓t�@�C����
		private string				mDbName				= null;

		// �Q�ƃ��O�c�a�̃I�u�W�F�N�g�Ǘ��p���X�g
		private ArrayList			mDbArrayList		= null;

		public NGwordContainer(string aDbName, ref PalmDatabaseAccessor aAccessor)
		{
			//  �I�u�W�F�N�g�̏�����...
			mDbName			= aDbName;
			mDbAccessor		= aAccessor;
			mDbArrayList	= new ArrayList();
			mDbParser		= new NGwordDbParser();
			fileSelectionDialog = new System.Windows.Forms.OpenFileDialog();
			fileSelectionDialog.AddExtension = true;
		}

		/// <summary>
		///    �f�[�^�R���e�i�N���X������������
		///    (DB�t�@�C������f�[�^��ǂݏo���ăI�u�W�F�N�g�ɓW�J����)
		/// </summary>
		public bool initializeDatas()
		{
			int nofRecords = 0;

			// �c�a�̃w�b�_����ǂ�Ń��R�[�h�����擾����
			if (mDbAccessor.getRecordNumber(out nofRecords) == false)
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
			if (nofRecords == 0)
			{
				// ���R�[�h���Ȃ���������ǂݍ��܂Ȃ�...
				return (true);
			}

			// �ꎞ�t�@�C�����̎w��
			string tempFileName = mDbName + ".temp";

			// ���R�[�h�̂��鐔���ǂݍ���ŃI�u�W�F�N�g�ɕϊ��A�o�^����
			for (int loop = 0; loop < nofRecords; loop++)
			{
				try
				{
					// par.exe���g�p����1���R�[�h���t�@�C���ɓW�J����
					if (mDbAccessor.pickUpRecord(loop, tempFileName) != false)
					{
						// �f�[�^�L�^�p���R�[�h�𐶐�
						NGwordRecord entryRecord = new NGwordRecord();

						// ���R�[�h����͂���
						mDbParser.inputRecordFromTemporaryFile(tempFileName, ref entryRecord);

						// ���R�[�h�����X�g�ɓo�^����
						mDbArrayList.Add(entryRecord);
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
		public int numberOfRecords()
		{
			return (mDbArrayList.Count);
		}

		/// <summary>
		///   ���R�[�h�̏����擾����
		/// </summary>
		public NGwordRecord getRecordData(int aIndexNumber)
		{
			if ((aIndexNumber < 0)&&
				(aIndexNumber >= mDbArrayList.Count))
			{
				return (null);
			}
			return ((NGwordRecord) mDbArrayList[aIndexNumber]);
		}

		/// <summary>
		///    �V�K���R�[�h�̒ǉ�
		/// </summary>
		public NGwordRecord newRecordData()
		{
			// ���O���R�[�h�𐶐�
			NGwordRecord entryRecord = new NGwordRecord();

			// �V�������R�[�h�I�u�W�F�N�g�����X�g�ɓo�^����
			mDbArrayList.Add(entryRecord);

			return (entryRecord);
		}

		/// <summary>
		///   ���R�[�h���P���폜����
		/// </summary>
		public void removeRecord(int aIndexNumber)
		{
			NGwordRecord targetRecord = (NGwordRecord) mDbArrayList[aIndexNumber];

			mDbArrayList.Remove(targetRecord);
			return;
		}

		/// <summary>
		///   ���O���R�[�h�Q��ۑ�����
		/// </summary>
		public bool saveDatabase(bool aOverWrite)
		{
			if (aOverWrite != true)
			{
				try
				{
					fileSelectionDialog.CheckFileExists = false;
					fileSelectionDialog.FileName = mDbName;
					if (fileSelectionDialog.ShowDialog() == DialogResult.Cancel)
					{
						// �t�@�C���o�͂̒��f...
						return (false);
					}
				}
				catch
				{
					// �������Ȃ�...
				}
			}
			else
			{
				// �t�@�C�����͂c�a���Ƃ��Ďw�肳�ꂽ����...
				fileSelectionDialog.FileName = mDbName;
			}

			// �ꎞ�t�@�C�����̎w��
			string tempFileName = mDbName + ".temp";

			// �ۑ�����t�@�C�����̎w��
			string targetFileName = fileSelectionDialog.FileName;

			// �t�@�C�������݂����ꍇ�ɂ́Arename���ĕۊǂ���
			if (File.Exists(targetFileName) == true)
			{
				try
				{
					File.Move(targetFileName, targetFileName + ".BAK");
				}
				catch
				{
					// �G���[�ɂȂ����Ƃ��͉������Ȃ�
				}
			}

			// �f�[�^�x�[�X�t�@�C����V�K�ɍ쐬����
			try
			{
				mDbAccessor.createDatabase(targetFileName,
										  mDbParser.getDbName(),
										  mDbParser.getDbType(),
										  mDbParser.getDbCreatorName(),
										  mDbParser.getDbVersion());
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
			for (int loop = 0; loop < mDbArrayList.Count; loop++)
			{
				NGwordRecord theData = getRecordData(loop);
				try
				{
					// �ꎞ�t�@�C���ɏo�͌�ADB�t�@�C���ɏ����o��
					mDbParser.outputRecordToTemporaryFile(tempFileName, ref theData);
					mDbAccessor.appendRecordToDatabase(targetFileName, tempFileName);
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

//////////////////////////////////////////////////
// // NG-3�\����
//////////////////////////////////////////////////
// typedef struct {
//	Char   ngWord[NGWORD_LEN];
//	UInt8  checkArea;
//	UInt8  matchedAction;
//	UInt16 reserved1;
//} NNshNGwordDatabase;
//////////////////////////////////////////////////

}
