/*
 *  $Id: logTokenDbParser.cs,v 1.2 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̋�؂蕶����f�[�^�x�[�X�ƃe���|�����t�@�C���Ƃ̂��Ƃ�...
 *       (�f�[�^�ϊ���S��)
 *
 *      logTokenDbParser()	�F �R���X�g���N�^
 *      getDbName()			�F �c�a���̂��擾����
 *      getDbType()			�F �c�a�^�C�v���擾����
 *      getDbCreatorName()	�F �c�a�̃N���G�[�^�����擾����
 *      getDbVersion()		�F �c�a�̃o�[�W�����ԍ����擾����
 *      inputRecordFromTemporaryFile()
 *							�F �P���R�[�h���t�@�C������I�u�W�F�N�g�ɕϊ�
 *      outputRecordToTemporaryFile()
 *							�F �P���R�[�h���I�u�W�F�N�g����t�@�C���ɕϊ�
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
	///   getLogDb�̃e���|�����t�@�C����́E�o�̓G���W��
	/// </summary>
	public class logTokenDbParser
	{
		/// <summary>
		///   �萔�̒�`...
		/// </summary>
		const int MAX_STRLEN		= 32;
		const int MAX_DATATOKEN		= 80;

		const string DB_CREATOR		= "NNsi";
		const string DB_TYPE		= "Data";
		const string DB_NAME		= "LogToken-NNsi";
		const int 	 DB_VERSION		= 100;

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public logTokenDbParser()
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
		public bool inputRecordFromTemporaryFile(string aFileName, ref logToken entryRecord)
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

				// id
				uint id = reader.ReadUInt32();
				entryRecord.id = id;
				offset = 4;

				// tokenPatternName
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] tokenPatternName = reader.ReadBytes(MAX_STRLEN);
				offset = offset + MAX_STRLEN;
				entryRecord.name = enc.GetString(tokenPatternName);

				// startToken
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] startToken = reader.ReadBytes(MAX_DATATOKEN);
				offset = offset + MAX_DATATOKEN;
				entryRecord.startToken = enc.GetString(startToken);

				// endToken
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] endToken = reader.ReadBytes(MAX_DATATOKEN);
				offset = offset + MAX_DATATOKEN;
				entryRecord.endToken = enc.GetString(endToken);

				// dataToken
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] dataToken = reader.ReadBytes(MAX_DATATOKEN);
				offset = offset + MAX_DATATOKEN;
				entryRecord.dataToken = enc.GetString(dataToken);

				// lastUpdate
				uint lastUpdate = reader.ReadUInt32();
				entryRecord.lastUpdate = lastUpdate;

				//  usable
				int usable = Convert.ToInt32(reader.ReadByte());
				entryRecord.usable = usable;

				// reserved0
				int reserved0 = Convert.ToInt32(reader.ReadByte());
				entryRecord.reserved0 = reserved0;

				// reserved1
				int reserved1 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.reserved1 = reserved1;

				// reserved2
				uint reserved2 = reader.ReadUInt32();
				entryRecord.reserved2 = reserved2;

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
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref logToken aData)
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

				//  id
				UInt32 id = aData.id;
				writer.Write(id);
				offset = 4;

				// tokenPatternName
				byte[] tokenPatternName = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.name != null)
				{
					tokenPatternName = enc.GetBytes(aData.name);
					len = tokenPatternName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_STRLEN)
					{
						writer.Write(tokenPatternName, 0, MAX_STRLEN);
						len = MAX_STRLEN;
					}
					else
					{
						writer.Write(tokenPatternName, 0, len);
					}
				}
				for (; len < MAX_STRLEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_STRLEN;


				// startToken
				byte[] startToken = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.startToken != null)
				{
					startToken = enc.GetBytes(aData.startToken);
					len = startToken.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATATOKEN)
					{
						writer.Write(startToken, 0, MAX_DATATOKEN);
						len = MAX_DATATOKEN;
					}
					else
					{
						writer.Write(startToken, 0, len);
					}
				}
				for (; len < MAX_DATATOKEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATATOKEN;

				// endToken
				byte[] endToken = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.endToken != null)
				{
					endToken = enc.GetBytes(aData.endToken);
					len = endToken.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATATOKEN)
					{
						writer.Write(endToken, 0, MAX_DATATOKEN);
						len = MAX_DATATOKEN;
					}
					else
					{
						writer.Write(endToken, 0, len);
					}
				}
				for (; len < MAX_DATATOKEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATATOKEN;

				// dataToken
				byte[] dataToken = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.dataToken != null)
				{
					dataToken = enc.GetBytes(aData.dataToken);
					len = dataToken.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATATOKEN)
					{
						writer.Write(dataToken, 0, MAX_DATATOKEN);
						len = MAX_DATATOKEN;
					}
					else
					{
						writer.Write(dataToken, 0, len);
					}
				}
				for (; len < MAX_DATATOKEN; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATATOKEN;

				// lastUpdate
				UInt32 lastUpdate = aData.lastUpdate;
				writer.Write(lastUpdate);

				// usable
				byte usable = (byte) aData.usable;
				writer.Write(usable);

				// reserved0
				byte reserved0 = (byte) aData.reserved0;
				writer.Write(reserved0);

				// reserved1
				UInt16 reserved1 = (UInt16) IPAddress.HostToNetworkOrder(aData.reserved1);
				writer.Write(reserved1);

				// reserved2
				UInt32 reserved2 = aData.reserved2;
				writer.Write(reserved2);

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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ���O��؂蕶����f�[�^�x�[�X�̒�`
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_STRLEN                  32
//#define MAX_DATATOKEN               80
//
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

