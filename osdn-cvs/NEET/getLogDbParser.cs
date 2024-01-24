/*
 *  $Id: getLogDbParser.cs,v 1.5 2005/02/26 13:17:49 mrsa Exp $
 *    --- NNsi�̎Q�ƃ��O�f�[�^�x�[�X�ƃe���|�����t�@�C���Ƃ̂��Ƃ�...
 *       (�f�[�^�ϊ���S��)
 *
 *      GetLogDbParser()	�F �R���X�g���N�^
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
	public class GetLogDbParser
	{
		/// <summary>
		///   �萔�̒�`...
		/// </summary>
		const int MAX_THREADFILENAME  	=  32;
		const int MAX_DATAPREFIX      	=  32;
		const int MAX_NICKNAME        	=  16;
		const int MAX_GETLOG_URL      	= 244;
		const int MAX_GETLOG_DATETIME 	=  38;
		const int MAX_TITLE_LABEL     	=  32;

		const string DB_CREATOR			= "NNsi";
		const string DB_TYPE  			= "Data";
		const string DB_NAME			= "LogPermanent-NNsi";
		const int 	 DB_VERSION 		= 100;


		public GetLogDbParser()
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
		public bool inputRecordFromTemporaryFile(string aFileName, ref logRecord entryRecord)
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

				// dataFileName
				fileStream.Seek(0, SeekOrigin.Begin);
				string dataFileName = new string(reader.ReadChars(MAX_THREADFILENAME));
				offset = offset + MAX_THREADFILENAME;
				entryRecord.DataFileName = dataFileName;

				// dataTitlePrefix
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] dataTitlePrefix = reader.ReadBytes(MAX_DATAPREFIX);
				offset = offset + MAX_DATAPREFIX;
				entryRecord.DataTitlePrefix = enc.GetString(dataTitlePrefix);

				// boardName
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] boardName = reader.ReadBytes(MAX_NICKNAME);
				offset = offset + MAX_NICKNAME;
				entryRecord.BoardName = enc.GetString(boardName);

				// dataURL
				fileStream.Seek(offset, SeekOrigin.Begin);
				string dataURL = new string(reader.ReadChars(MAX_GETLOG_URL));
				offset = offset + MAX_GETLOG_URL;
				entryRecord.DataURL = dataURL;

				// getDateTime
				fileStream.Seek(offset, SeekOrigin.Begin);
				string getDateTime = new string(reader.ReadChars(MAX_GETLOG_DATETIME));
				offset = offset + MAX_GETLOG_DATETIME;
				entryRecord.GetDateTime = getDateTime;

				// dataTitle
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte[] dataTitle = reader.ReadBytes(MAX_TITLE_LABEL);
				offset = offset + MAX_TITLE_LABEL;
				entryRecord.DataTitle =  enc.GetString(dataTitle);

				// wwwcMode
				fileStream.Seek(offset, SeekOrigin.Begin);
				int wwwcMode = Convert.ToInt32(reader.ReadByte());
				entryRecord.WwwcMode = wwwcMode;

				// rssData
				int rssData = Convert.ToInt32(reader.ReadByte());
				entryRecord.RssData = rssData;

				// notUse0
				int notUse0 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.NotUse0 = notUse0;


				// notUse1
				int notUse1 = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.NotUse1 = notUse1;

				// lastUpdate
				uint lastUpdate = reader.ReadUInt32();
				entryRecord.LastUpdate = lastUpdate;

				// id
				int id = IPAddress.NetworkToHostOrder(Convert.ToInt32(reader.ReadUInt16()));
				entryRecord.Id = id;

				// state
				int state = Convert.ToInt32(reader.ReadByte());
				entryRecord.State = state;

				// getAfterHours
				int getAfterHours = Convert.ToInt32(reader.ReadByte());
				entryRecord.GetAfterHours = getAfterHours;

				// usable
				int usable = Convert.ToInt32(reader.ReadByte());
				entryRecord.Usable = usable;

				// getDepth
				int getDepth = Convert.ToInt32(reader.ReadByte());
				entryRecord.GetDepth = getDepth;

				// kanjiCode
				int kanjiCode = Convert.ToInt32(reader.ReadByte());
				entryRecord.KanjiCode = kanjiCode;

				// getSubData
				int getSubData = Convert.ToInt32(reader.ReadByte());
				entryRecord.GetSubData = getSubData;

				// tokenId
				uint tokenId = reader.ReadUInt32();
				entryRecord.TokenId = tokenId;

				// subTokenId
				uint subTokenId = reader.ReadUInt32();
				entryRecord.SubTokenId = subTokenId;

				// URL�̕\��
				// MessageBox.Show(
				//				"KanjiCode :" + kanjiCode +
				//				" id :" + id + " tokenId :" + tokenId + "\n\n" +
				//				" URL : " + dataURL);

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
		public bool outputRecordToTemporaryFile(string aOutputFileName, ref logRecord aData)
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

				// dataFileName
				byte[] dataFileName = null;
				fileStream.Seek(0, SeekOrigin.Begin);
				if (aData.DataFileName != null)
				{
					dataFileName = enc.GetBytes(aData.DataFileName);
					len = dataFileName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_THREADFILENAME)
					{
						writer.Write(dataFileName, 0, MAX_THREADFILENAME);
						len = MAX_THREADFILENAME;
					}
					else
					{
						writer.Write(dataFileName, 0, len);
					}
				}
				for (; len < MAX_THREADFILENAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_THREADFILENAME;

				// dataTitlePrefix
				byte[] dataTitlePrefix = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.DataTitlePrefix != null)
				{
					dataTitlePrefix = enc.GetBytes(aData.DataTitlePrefix);
					len = dataTitlePrefix.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_DATAPREFIX)
					{
						writer.Write(dataTitlePrefix, 0, MAX_DATAPREFIX);
						len = MAX_DATAPREFIX;
					}
					else
					{
						writer.Write(dataTitlePrefix, 0, len);
					}
				}
				for (; len < MAX_DATAPREFIX; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_DATAPREFIX;

				// boardName
				byte[] boardName = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.BoardName != null)
				{
					boardName = enc.GetBytes(aData.BoardName);
					len = boardName.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_NICKNAME)
					{
						writer.Write(boardName, 0, MAX_NICKNAME);
						len = MAX_NICKNAME;
					}
					else
					{
						writer.Write(boardName, 0, len);
					}
				}
				for (; len < MAX_NICKNAME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_NICKNAME;

				// dataURL
				byte[] dataURL = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.DataURL != null)
				{
					dataURL = enc.GetBytes(aData.DataURL);
					len = dataURL.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_GETLOG_URL)
					{
						writer.Write(dataURL, 0, MAX_GETLOG_URL);
						len = MAX_GETLOG_URL;
					}
					else
					{
						writer.Write(dataURL, 0, len);
					}
				}
				for (; len < MAX_GETLOG_URL; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_GETLOG_URL;

				// getDateTime
				byte[] getDateTime = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.GetDateTime != null)
				{
					getDateTime = enc.GetBytes(aData.GetDateTime);
					len = getDateTime.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_GETLOG_DATETIME)
					{
						writer.Write(getDateTime, 0, MAX_GETLOG_DATETIME);
						len = MAX_GETLOG_DATETIME;
					}
					else
					{
						writer.Write(getDateTime, 0, len);
					}
				}
				for (; len < MAX_GETLOG_DATETIME; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_GETLOG_DATETIME;

				// dataTitle
				byte[] dataTitle = null;
				fileStream.Seek(offset, SeekOrigin.Begin);
				if (aData.DataTitle != null)
				{
					dataTitle = enc.GetBytes(aData.DataTitle);
					len = dataTitle.Length;
				}
				else
				{
					len = 0;
				}
				if (len > 0)
				{
					if (len > MAX_TITLE_LABEL)
					{
						writer.Write(dataTitle, 0, MAX_TITLE_LABEL);
						len = MAX_TITLE_LABEL;
					}
					else
					{
						writer.Write(dataTitle, 0, len);
					}
				}
				for (; len < MAX_TITLE_LABEL; len++)
				{
					byte tempData = 0;
					writer.Write(tempData);
				}
				offset = offset + MAX_TITLE_LABEL;

				// wwwcMode
				fileStream.Seek(offset, SeekOrigin.Begin);
				byte wwwcMode = (byte) aData.WwwcMode;
				writer.Write(wwwcMode);

				// rssData
				byte rssData = (byte) aData.RssData;
				writer.Write(rssData);

				// notUse0
				UInt16 notUse0 = (UInt16) aData.NotUse0;
				writer.Write(notUse0);

				// notUse1
				UInt16 notUse1 = (UInt16) aData.NotUse1;
				writer.Write(notUse1);

				// lastUpdate
				UInt32 lastUpdate = aData.LastUpdate;
				writer.Write(lastUpdate);

				// id
				UInt16 id = (UInt16) IPAddress.HostToNetworkOrder(aData.Id);
				writer.Write(id);

				// state
				byte state = (byte) aData.State;
				writer.Write(state);

				// getAfterHours
				byte getAfterHours = (byte) aData.GetAfterHours;
				writer.Write(getAfterHours);

				// usable
				byte usable = (byte) aData.Usable;
				writer.Write(usable);

				// getDepth
				byte getDepth = (byte) aData.GetDepth;
				writer.Write(getDepth);

				// kanjiCode
				byte kanjiCode = (byte) aData.KanjiCode;
				writer.Write(kanjiCode);

				// getSubData
				byte getSubData = (byte) aData.GetSubData;
				writer.Write(getSubData);

				// tokenId
				UInt32 tokenId = aData.TokenId;
				writer.Write(tokenId);

				// subTokenId
				UInt32 subTokenId = aData.SubTokenId;
				writer.Write(subTokenId);

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
//    �Q�ƃ��O�f�[�^�x�[�X�̒�`
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//#define MAX_THREADFILENAME          32
//#define MAX_DATAPREFIX              32
//#define MAX_NICKNAME                16
//#define MAX_GETLOG_URL             244
//#define MAX_GETLOG_DATETIME         38
//#define MAX_TITLE_LABEL             32
//
//typedef struct {
//	Char   dataFileName[MAX_THREADFILENAME]; // �f�[�^�t�@�C����
//	Char   dataTitlePrefix[MAX_DATAPREFIX];  // �f�[�^�w�b�_
//	Char   boardName      [MAX_NICKNAME];    // �{�[�h�j�b�N�l�[��(���g�p)
//	Char   dataURL    [MAX_GETLOG_URL];      // �f�[�^�擾��URL
//	Char   getDateTime[MAX_GETLOG_DATETIME]; // �X�V����
//	Char   dataTitle  [MAX_TITLE_LABEL];     // �f�[�^����
//	UInt8  wwwcMode;                         // ���O�擾���Ȃ�
//	UInt8  rssData;                          // RSS�f�[�^
//	UInt16 notUse0;                          // ���g�p�̈�(�p�~)
//	UInt16 notUse1;                          // ���g�p�̈�(�p�~)
//	UInt32 lastUpdate;                       // �ŏI�X�V����
//	UInt16 id;                               // �f�[�^ID
//	UInt8  state;                            // �Q�ƃ��O���
//	UInt8  getAfterHours;                    // �Ď擾���鎞�ԊԊu
//	UInt8  usable;                           // �擾����/���Ȃ�
//	UInt8  getDepth;                         // �擾�K�w�w��
//	UInt8  kanjiCode;                        // �����R�[�h
//	UInt8  getSubData;                       // �u�P���擾�v�Ŏ擾�����f�[�^��
//	UInt32 tokenId;                          // ��؂��ID
//	UInt32 subTokenId;                       // �T�u��؂��ID
//} NNshGetLogDatabase;
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}
