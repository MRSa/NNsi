/*
 *  $Id: PalmDatabaseAccessor.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- PalmOS�̃f�[�^�x�[�X���A�N�Z�X����N���X..
 *
 *      [Memo]
 *         par.exe��wrapper.
 *
 */
using System;
using System.Collections;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using System.IO;

namespace NEET
{
	/// <summary>
	///  Class �̊T�v�̐���
	/// </summary>
	public class PalmDatabaseAccessor
	{
		private string mParExe;				// par.exe�̃t�@�C����(��΃p�X)
		private string mDatabaseFileName;   // �f�[�^�x�[�X�t�@�C������

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public PalmDatabaseAccessor(string aParExe, string aFileName)
		{
			//
			// TODO: �R���X�g���N�^�̃��W�b�N�������ɒǉ�
			//
			mParExe           = aParExe;
			mDatabaseFileName = aFileName;
		}

		/// <summary>
		///   1���R�[�h��؂�o���A�t�@�C���ɏo�͂���
		/// </summary>
		public bool pickUpRecord(int aIndexNumber, string aFileName)
		{

			// par.exe�ɓn���������쐬����
			string argument = "x " + mDatabaseFileName +
								" -f " + aFileName + " " + aIndexNumber;
			string outputData = "";

			// par.exe �����s���āA�P�̃��R�[�h���t�@�C���ɒ��o����
			return (executeParExe(argument, out outputData));
		}


		///  <summary>
		///    �f�[�^�x�[�X�̃��R�[�h�����擾����
		///  </summary>
		public bool getRecordNumber(out int aRecords)
		{
			string argument = "h " + mDatabaseFileName;
			string output;
			if (executeParExe(argument, out output) == false)
			{
				// �ǂݍ��ݎ��s�B�B�B�I������
				aRecords = 0;
				return (false);
			}

			try
			{
				//  �ǂݏo�����f�[�^���g���~���O����...
				//  (����[�������... "nrecords : ###" �������Đ��l��؂�o��)
				int len = output.LastIndexOf("nrecords");
				output = output.Remove(0, len);
				Regex numberPattern = new Regex("[0-9]+");	// ���l�̃p�^�[��
				Match number = numberPattern.Match(output);	// �����񂩂琔�l�̃p�^�[���𔲂��o��

				// �؂������������string�^����int�^�ɕϊ�
				aRecords = Convert.ToInt32(number.ToString());
			}
			catch
			{
				// �ϊ��Ɏ��s...�ǂݍ��ݎ��s�ɂ���B
				aRecords = 0;
				return (false);
			}

			// �ǂݍ��ݐ���
			return (true);
		}

		///  <summary>
		///    ������ۂ̃f�[�^�x�[�X���쐬����
		///  </summary>
		public bool createDatabase(string dbNameWithPath, string dbName, string dbType, string dbCreator, int dbVersion)
		{
			string argument = "c -v " + dbVersion + " " + dbNameWithPath + " " + dbName + " " + dbType + " " + dbCreator;
			string result;
			try
			{
				// par.exe�����s����
				executeParExe(argument, out result);
			}
			catch
			{
				return (false);
			}
			return (true);
		}

		///  <summary>
		///    �f�[�^�x�[�X�Ƀ��R�[�h���P�ǉ�����
		///  </summary>
		public bool appendRecordToDatabase(string dbNameWithPath, string recordFile)
		{
			string argument = "a " + dbNameWithPath + " " + recordFile;
			string result;
			try
			{
				// par.exe�����s����
				executeParExe(argument, out result);
			}
			catch
			{
				return (false);
			}
			return (true);
		}

		/// <summary>
		///   par.exe�����s����
		///     (�W���o�͂Ɋi�[�������� result �Ɋi�[���ĉ�������)
		/// </summary>
		private bool executeParExe(string argument, out string result)
		{

			// par.exe �N���̂��߂̏���...
			ProcessStartInfo pInfo       = new ProcessStartInfo();
			pInfo.FileName               = mParExe;     // ���s�R�}���h�̃t���p�X
			pInfo.Arguments              = argument;	// �����̎w��
			pInfo.WindowStyle         	 = ProcessWindowStyle.Minimized; // �ŏ������ċN��
			pInfo.CreateNoWindow         = true;	// �N�����E�B���h�E��\�����Ȃ�
			pInfo.UseShellExecute        = false;	// �N����OS�̃V�F�����g�p���Ȃ�
			pInfo.RedirectStandardInput  = false;	// �W�����͂��֎~����
			pInfo.RedirectStandardOutput = true;	// �W���o�͂��g�p����
			// pInfo.WorkingDirectory = "(��ƃt�H���_)";  // ��ƃt�H���_�̎w��
			try
			{
				// �A�v���P�[�V�������N��
				Process proc = Process.Start(pInfo);

				// �W���o�͂ɏo�͂����f�[�^���擾����
				result = proc.StandardOutput.ReadToEnd();

				// �A�v�����I������܂ő҂�...
				proc.WaitForExit();
			}
			catch
			{
				// ���s�Ɏ��s
				result = "";
				return (false);
			}

			// ���s����
			return (true);
		}
	}
}
