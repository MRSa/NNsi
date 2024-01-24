/*
 *  $Id: NGwordDbParser.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi��NG���[�h��̓G���W��
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
	///  Class �̊T�v�̐���
	/// </summary>
	public class NGwordDbParser
	{
		const int	 NGWORD_LEN		= 40;

		const string DB_CREATOR		= "NNsi";
		const string DB_TYPE		= "Data";
		const string DB_NAME		= "NGWord-NNsi";
		const int 	 DB_VERSION		= 100;

		public NGwordDbParser()
		{
			//
			// TODO: �R���X�g���N�^�̃��W�b�N�������ɒǉ�
			//
		}

		/// <summary>
		///   DB������������
		/// </summary>
		public string getDbName()
		{
			return (DB_NAME);
		}

		/// <summary>
		///   DB�^�C�v����������
		/// </summary>
		public string getDbType()
		{
			return (DB_TYPE);
		}

		/// <summary>
		///   �N���G�[�^������������
		/// </summary>
		public string getDbCreatorName()
		{
			return (DB_CREATOR);
		}

		/// <summary>
		///   DB�o�[�W�����ԍ�����������
		/// </summary>
		public int getDbVersion()
		{
			return (DB_VERSION);
		}

		/// <summary>
		///   �t�@�C������P���R�[�h��ǂݏo��
		/// </summary>
		public bool inputRecordFromTemporaryFile(string aFileName, ref NGwordRecord entryRecord)
		{
			try
			{
				Stream fileStream;
				int offset = 0;
				fileStream = File.OpenRead(aFileName);

				// SHIFT JIS�̃G���R�[�_������
				Encoding enc = Encoding.GetEncoding("Shift_Jis");

				// �o�C�i���f�[�^��ǂݏo��...
				BinaryReader reader;
				reader = new BinaryReader(fileStream);

				// ngWord
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] ngWordData = reader.ReadBytes(NGWORD_LEN);
				offset = offset + NGWORD_LEN;
				entryRecord.ngWord = enc.GetString(ngWordData);

				//  checkArea
				uint checkArea = Convert.ToUInt32(reader.ReadByte());
				entryRecord.checkArea= checkArea;

				//  matchedAction
				uint matchedAction = Convert.ToUInt32(reader.ReadByte());
				entryRecord.matchedAction = matchedAction;

				// reserved1
				int reserved1 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.reserved = reserved1;

				// �t�@�C�����N���[�Y����
				reader.Close();
			}
			catch
			{
				return (false);
			}
			return (true);
		}

		/// <summary>
		///   �t�@�C���ɂP���R�[�h���o�͂���
		/// </summary>
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref NGwordRecord aData)
		{
			try
			{
				Stream fileStream;
				int offset = 0;
				fileStream = File.OpenWrite(aOutputFileName);

				// SHIFT JIS�̃G���R�[�_������
				Encoding enc = Encoding.GetEncoding("Shift_Jis");

				// �o�C�i���f�[�^����������
				BinaryWriter writer;
				writer = new BinaryWriter(fileStream);

				int len = 0;

				// ngWord
				byte[] ngWord = null;
				fileStream.Seek(0, SeekOrigin.Begin);
				if (aData.ngWord != null)
				{
					ngWord = enc.GetBytes(aData.ngWord);
					len = ngWord.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > NGWORD_LEN)
					{
						writer.Write(ngWord, 0, NGWORD_LEN);
						len = NGWORD_LEN;
					}
					else
					{
						writer.Write(ngWord, 0, len);
					}
				}
				for (; len < NGWORD_LEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + NGWORD_LEN;

				// checkArea
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte checkArea = (byte) aData.checkArea;
				writer.Write(checkArea);

				// matchedAction
				byte matchedAction = (byte) aData.matchedAction;
				writer.Write(matchedAction);

				// reserved
				UInt16 reserved = (UInt16) aData.reserved;
				writer.Write(reserved);

				// NNsi�̂c�a�͂S�o�C�g�̗\��̈悪����̂�...
				UInt32 dummyArea = 0;
				writer.Write(dummyArea);

				// �t�@�C�����N���[�Y����
				writer.Close();
			}
			catch
			{
				// �o�͂Ɏ��s...(���������I������)
				return (false);
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
