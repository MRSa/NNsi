/*
 *  $Id: threadIndexRecord.h,v 1.3 2005/06/14 15:02:57 mrsa Exp $
 *
 *    �X���C���f�b�N�X�̃f�[�^...
 *     (���ʂ̃f�[�^�ێ��N���X)
 *
 *
 */
#pragma once
#include <windows.h>
#include <string>

class threadIndexRecord
{
public:
	threadIndexRecord(void);
	virtual ~threadIndexRecord(void);

public:
	//  setters...
	void setCurrentNumber(DWORD aNumber);
	void setFileName(char *apFileName);
	void setFileName(std::string &arFileName);
	void setThreadTitle(char *apThreadTitle);
	void setThreadTitle(std::string &arThreadTitle);
	void setResCount(DWORD aNumber);
	void setFileSize(DWORD aNumber);
	void setRoundDate(DWORD aNumber);
	void setLastModified(DWORD aNumber);
	void setReadResNumber(DWORD aNumber);
	void setNewReceivedRes(DWORD aNumber);
	void setReserved(DWORD aNumber);
	void setReadUnread(DWORD aNumber);
	void setScrollTop(DWORD aNumber);
	void setAllResCount(DWORD aNumber);
	void setNewResCount(DWORD aNumber);
	void setAgeSage(DWORD aNumber);
	void setCurrentResNumber(DWORD aNumber);

public:
	//  getters..
	void getCurrentNumber(DWORD &arNumber);
	void getFileName(std::string &arFile);
	void getThreadTitle(std::string &arThreadTitle);
	void getResCount(DWORD &arNumber);
	void getFileSize(DWORD &arNumber);
	void getRoundDate(DWORD &arNumber);
	void getLastModified(DWORD &arNumber);
	void getReadResNumber(DWORD &arNumber);
	void getNewReceivedRes(DWORD &arNumber);
	void getReserved(DWORD &arNumber);
	void getReadUnread(DWORD &arNumber);
	void getScrollTop(DWORD &arNumber);
	void getAllResCount(DWORD &arNumber);
	void getNewResCount(DWORD &arNumber);
	void getAgeSage(DWORD &arNumber);
	void getCurrentResNumber(DWORD &arNumber);

private:
	void initializeSelf(void);

private:
	DWORD       currentNumber;  // ���݂̔ł̈ʒu 
	std::string fileName;       // �X���t�@�C����(*.dat)
	std::string threadTitle;    // �X���^�C�g��
	DWORD       resCount;       // �X���b�h�J�E���g�i���[�J���j
	DWORD       fileSize;       // �X���t�@�C���T�C�Y
	DWORD       roundDate;      // �X���b�h���擾���������i��������j
	DWORD       lastModified;   // �X���b�h���X�V����Ă�������i�T�[�o�������j
	DWORD       readResNumber;  // �h�����܂œǂ񂾁h�ԍ�
	DWORD       newReceivedRes; // �R�R����V�K��M
	DWORD       reserved;       // �\��(�[���Œ�)
	DWORD       readUnread;     // ���ǃt���O
	DWORD       scrollTop;      // �X�N���[���ʒu
	DWORD       allResCount;    // �X���b�h�J�E���g�i�T�[�o�j
	DWORD       newResCount;    // �X���b�h�V����
	DWORD       ageSage;        // �A�C�e���̏グ����

	DWORD       currentResNumber;  // ���݂̃��b�Z�[�W��ǂ񂾈ʒu
};
/*****  [[[ GikoNavi �̃X���b�h�C���f�b�N�X�t�@�C���̍쐬���� ]]]  *****/
/*****
(*************************************************************************
 *�X���b�h�C���f�b�N�X�t�@�C��(Folder.idx)�쐬
 *************************************************************************)
procedure TGikoSys.CreateThreadDat(Board: TBoard);
var
	i: integer;
	s: string;
	SubjectList: TStringList;
	sl: TStringList;
	Rec: TSubjectRec;
	FileName: string;
	cnt: Integer;
begin
	if not FileExists(Board.GetSubjectFileName) then Exit;
	FileName := Board.GetFolderIndexFileName;

	SubjectList := TStringList.Create;
	try
		SubjectList.LoadFromFile(Board.GetSubjectFileName);
		sl := TStringList.Create;
		try
			cnt := 1;
			sl.Add(FOLDER_INDEX_VERSION);
			for i := 0 to SubjectList.Count - 1 do begin
				Rec := DivideSubject(SubjectList[i]);

				if (Trim(Rec.FFileName) = '') or (Trim(Rec.FTitle) = '') then
					Continue;

				s := Format('%x', [cnt]) + #1					//�ԍ�
					 + Rec.FFileName + #1								//�t�@�C����
					 + Rec.FTitle + #1									//�^�C�g��
					 + Format('%x', [Rec.FCount]) + #1	//�J�E���g
					 + Format('%x', [0]) + #1						//size
					 + Format('%x', [0]) + #1						//RoundDate
					 + Format('%x', [0]) + #1						//LastModified
					 + Format('%x', [0]) + #1						//Kokomade
					 + Format('%x', [0]) + #1						//NewReceive
					 + '0' + #1					 							//���g�p
					 + Format('%x', [0]) + #1						//UnRead
					 + Format('%x', [0]) + #1						//ScrollTop
					 + Format('%x', [Rec.FCount]) + #1	//AllResCount
					 + Format('%x', [0]) + #1						//NewResCount
					 + Format('%x', [0]);								//AgeSage

				sl.Add(s);
				inc(cnt);
			end;
			sl.SaveToFile(FileName);
		finally
			sl.Free;
		end;
	finally
		SubjectList.Free;
	end;
end;
*****/

/***** GikoNavi : BoardGroup.pas ���... *****/
/*****
	private
		FContext: DWORD;               // �v���O�C�������R�ɐݒ肵�Ă����l(��ɃC���X�^���X������)

		FNo: Integer;                  //�ԍ�
		FFileName: string;             //�X���b�h�t�@�C����
		FTitle: string;	               //�X���b�h�^�C�g��
		FShortTitle: string;           //�Z���X���b�h�^�C�g���i�����p�j
		FRoundDate: TDateTime;         //�X���b�h���擾���������i��������j
		FLastModified: TDateTime;      //�X���b�h���X�V����Ă�������i�T�[�o�������j
		FCount: Integer;               //�X���b�h�J�E���g�i���[�J���j
		FAllResCount: Integer;         //�X���b�h�J�E���g�i�T�[�o�j
		FNewResCount: Integer;         //�X���b�h�V����
		FSize: Integer;                //�X���b�h�T�C�Y
		FRound: Boolean;               //����t���O
		FRoundName: string;	           //����
		FIsLogFile: Boolean;           //���O���݃t���O
		FParentBoard: TBoard;          //�e�{�[�h
		FKokomade: Integer;	           //�R�R�܂œǂ񂾔ԍ�
		FNewReceive: Integer;          //�R�R����V�K��M
		FNewArrival: Boolean;          //�V��
		FUnRead: Boolean;              //���ǃt���O
		FScrollTop: Integer;           //�X�N���[���ʒu
		FDownloadHost: string;         //���̃z�X�g�ƈႤ�ꍇ�̃z�X�g
		FAgeSage: TGikoAgeSage;	       //�A�C�e���̏グ����
//		FSPID: string;                 //�������ݗpSPID

		FUpdate: Boolean;
		FExpand: Boolean;
		FURL					: string;  // ���̃X�����u���E�U�ŕ\������ۂ� URL
		FBoardPlugIn	: TBoardPlugIn;    // ���̃X�����T�|�[�g����v���O�C��
		FFilePath			: string;      // ���̃X�����ۑ�����Ă���p�X
*****/
/**
	//���X�g�̏グ����
	TGikoAgeSage = (gasNone, gasAge, gasSage, gasNew, gasNull);

	 gasNone : dat�����Ƃ��H
     gasNew  : �V�X��
	 gasAge  : �オ�����X��
	 gasSage : ���������X��
     gasNull : ���O�Ȃ��H	
**/
