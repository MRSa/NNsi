/*
 *  $Id: threadIndexRecord.h,v 1.3 2005/06/14 15:02:57 mrsa Exp $
 *
 *    スレインデックスのデータ...
 *     (共通のデータ保持クラス)
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
	DWORD       currentNumber;  // 現在の板での位置 
	std::string fileName;       // スレファイル名(*.dat)
	std::string threadTitle;    // スレタイトル
	DWORD       resCount;       // スレッドカウント（ローカル）
	DWORD       fileSize;       // スレファイルサイズ
	DWORD       roundDate;      // スレッドを取得した日時（巡回日時）
	DWORD       lastModified;   // スレッドが更新されている日時（サーバ側日時）
	DWORD       readResNumber;  // ”ここまで読んだ”番号
	DWORD       newReceivedRes; // ココから新規受信
	DWORD       reserved;       // 予約(ゼロ固定)
	DWORD       readUnread;     // 未読フラグ
	DWORD       scrollTop;      // スクロール位置
	DWORD       allResCount;    // スレッドカウント（サーバ）
	DWORD       newResCount;    // スレッド新着数
	DWORD       ageSage;        // アイテムの上げ下げ

	DWORD       currentResNumber;  // 現在のメッセージを読んだ位置
};
/*****  [[[ GikoNavi のスレッドインデックスファイルの作成処理 ]]]  *****/
/*****
(*************************************************************************
 *スレッドインデックスファイル(Folder.idx)作成
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

				s := Format('%x', [cnt]) + #1					//番号
					 + Rec.FFileName + #1								//ファイル名
					 + Rec.FTitle + #1									//タイトル
					 + Format('%x', [Rec.FCount]) + #1	//カウント
					 + Format('%x', [0]) + #1						//size
					 + Format('%x', [0]) + #1						//RoundDate
					 + Format('%x', [0]) + #1						//LastModified
					 + Format('%x', [0]) + #1						//Kokomade
					 + Format('%x', [0]) + #1						//NewReceive
					 + '0' + #1					 							//未使用
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

/***** GikoNavi : BoardGroup.pas より... *****/
/*****
	private
		FContext: DWORD;               // プラグインが自由に設定していい値(主にインスタンスが入る)

		FNo: Integer;                  //番号
		FFileName: string;             //スレッドファイル名
		FTitle: string;	               //スレッドタイトル
		FShortTitle: string;           //短いスレッドタイトル（検索用）
		FRoundDate: TDateTime;         //スレッドを取得した日時（巡回日時）
		FLastModified: TDateTime;      //スレッドが更新されている日時（サーバ側日時）
		FCount: Integer;               //スレッドカウント（ローカル）
		FAllResCount: Integer;         //スレッドカウント（サーバ）
		FNewResCount: Integer;         //スレッド新着数
		FSize: Integer;                //スレッドサイズ
		FRound: Boolean;               //巡回フラグ
		FRoundName: string;	           //巡回名
		FIsLogFile: Boolean;           //ログ存在フラグ
		FParentBoard: TBoard;          //親ボード
		FKokomade: Integer;	           //ココまで読んだ番号
		FNewReceive: Integer;          //ココから新規受信
		FNewArrival: Boolean;          //新着
		FUnRead: Boolean;              //未読フラグ
		FScrollTop: Integer;           //スクロール位置
		FDownloadHost: string;         //今のホストと違う場合のホスト
		FAgeSage: TGikoAgeSage;	       //アイテムの上げ下げ
//		FSPID: string;                 //書き込み用SPID

		FUpdate: Boolean;
		FExpand: Boolean;
		FURL					: string;  // このスレをブラウザで表示する際の URL
		FBoardPlugIn	: TBoardPlugIn;    // このスレをサポートするプラグイン
		FFilePath			: string;      // このスレが保存されているパス
*****/
/**
	//リストの上げ下げ
	TGikoAgeSage = (gasNone, gasAge, gasSage, gasNew, gasNull);

	 gasNone : dat落ちとか？
     gasNew  : 新スレ
	 gasAge  : 上がったスレ
	 gasSage : 下がったスレ
     gasNull : ログなし？	
**/
