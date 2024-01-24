/*
 *  $Id: bbsDbParser.cs,v 1.1 2005/05/14 07:40:52 mrsa Exp $
 *    --- NNsi�̔ꗗ�f�[�^�x�[�X�ƃe���|�����t�@�C���Ƃ̂��Ƃ�...
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
	public class  bbsDbParser
	{
		/// <summary>
		///   �萔�̒�`...
		/// </summary>
		const int MAX_URL			= 80;
		const int MAX_BOARDNAME		= 40;
		const int MAX_NICKNAME		= 16;

		const string DB_CREATOR		= "NNsi";
		const string DB_TYPE		= "Data";
		const string DB_NAME		= "BBS-NNsi";
		const int 	 DB_VERSION		= 160;

		/// <summary>
		///   �R���X�g���N�^
		/// </summary>
		public bbsDbParser()
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
		public bool inputRecordFromTemporaryFile(string aFileName, ref bbsRecord entryRecord)
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

				// boardNick
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardNick = reader.ReadBytes(MAX_NICKNAME);
				offset = offset + MAX_NICKNAME;
				entryRecord.boardNick = enc.GetString(boardNick);

				// boardURL
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardURL = reader.ReadBytes(MAX_URL);
				offset = offset + MAX_URL;
				entryRecord.boardURL = enc.GetString(boardURL);

				// boardName
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardName = reader.ReadBytes(MAX_BOARDNAME);
				offset = offset + MAX_BOARDNAME;
				entryRecord.boardName = enc.GetString(boardName);


				//  bbsType
				uint bbsType = Convert.ToUInt32(reader.ReadByte());
				entryRecord.bbsType = bbsType;

				// state
				uint bbsState = Convert.ToUInt32(reader.ReadByte());
				entryRecord.bbsState = bbsState;

				// reserved
				int reserved = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.reserved = reserved;


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
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref bbsRecord aData)
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

				// boardNick
				byte[] boardNick = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.boardNick != null)
				{
					boardNick = enc.GetBytes(aData.boardNick);
					len = boardNick.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_NICKNAME)
					{
						writer.Write(boardNick, 0, MAX_NICKNAME);
						len = MAX_NICKNAME;
					}
					else
					{
						writer.Write(boardNick, 0, len);
					}
				}
				for (; len < MAX_NICKNAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_NICKNAME;


				// boardURL
				byte[] boardURL = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.boardURL != null)
				{
					boardURL = enc.GetBytes(aData.boardURL);
					len = boardURL.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_URL)
					{
						writer.Write(boardURL, 0, MAX_URL);
						len = MAX_URL;
					}
					else
					{
						writer.Write(boardURL, 0, len);
					}
				}
				for (; len < MAX_URL; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_URL;

				// boardName
				byte[] boardName = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.boardName != null)
				{
					boardName = enc.GetBytes(aData.boardName);
					len = boardName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_BOARDNAME)
					{
						writer.Write(boardName, 0, MAX_BOARDNAME);
						len = MAX_BOARDNAME;
					}
					else
					{
						writer.Write(boardName, 0, len);
					}
				}
				for (; len < MAX_BOARDNAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_BOARDNAME;


				// bbsType
				byte bbsType = (byte) aData.bbsType;
				writer.Write(bbsType);

				// bbsState
				byte bbsState = (byte) aData.bbsState;
				writer.Write(bbsState);

				// reserved
				UInt16 reserved = (UInt16) IPAddress.HostToNetworkOrder(aData.reserved);
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//    ���f�[�^�x�[�X�̒�`  (NNsi)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_URL                     80
//#define MAX_BOARDNAME               40
//#define MAX_NICKNAME                16
//
//// ���(�f�[�^�x�[�X�̍\��)
//typedef struct {
//	Char   boardNick[MAX_NICKNAME];    // �{�[�h�j�b�N�l�[��
//	Char   boardURL [MAX_URL];         // �{�[�hURL
//	Char   boardName[MAX_BOARDNAME];   // �{�[�h��
//	UInt8  bbsType;                    // �{�[�hTYPE
//	UInt8  state;                      // �{�[�h�g�p���
//	Int16  reserved;                   // (�\��̈�F�ȑO�͎g�p���Ă���)
//} NNshBoardDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

}

