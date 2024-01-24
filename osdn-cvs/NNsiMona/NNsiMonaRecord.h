/*!
 *  $Id: NNsiMonaRecord.h,v 1.1.1.1 2005/05/07 07:51:09 mrsa Exp $
 *     NNsiのレコード構造の定義ファイル...
 *
 */
#ifndef __NNsiMona_RECORD_H_
#define __NNsiMona_RECORD_H_

#include <CPalmRec.h>
#include <CPString.h>

#define NNsiMona_OBJECT_VERSION 0x4D500100  //MP 1.0

#define MAX_MEMO_SIZE   MAX_RECORD_SIZE


///////////////  NNsi Version 1.23 Revision000 より {  ///////////////
#pragma pack(2)

/** PalmTypes.h **/
typedef char          Char;     //  8ビット符号付きデータ
typedef unsigned char UInt8;    //  8ビット符号なしデータ
typedef WORD          Int16;    // 16ビット符号付きデータ
typedef WORD          UInt16;   // 16ビット符号なしデータ
typedef DWORD         Int32;    // 32ビット符号付きデータ
typedef DWORD         UInt32;   // 32ビット符号なしデータ

/** その他の設定値 **/
#define MARGIN                       4

/** BBS-DBの文字列サイズ **/
#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16

/** Thread-DB の文字列サイズ **/
#define MAX_THREADFILENAME          32
#define MAX_THREADNAME              72


// 板情報(データベースのレコード構造) : BBS-NNsi.pdb
typedef struct {
    Char   boardNick[MAX_NICKNAME];    // ボードニックネーム 
    Char   boardURL [MAX_URL];         // ボードURL
    Char   boardName[MAX_BOARDNAME];   // ボード名
    UInt8  bbsType;                    // ボードTYPE
    UInt8  state;                      // ボード使用状態
    Int16  reserved;                   // (予約領域：以前は使用していた)
} NNshBoardDatabase;

// スレ情報(データベースのレコード構造) : Subject-NNsi.pdb
typedef struct {
    Char   threadFileName[MAX_THREADFILENAME]; // ボード名
    Char   threadTitle   [MAX_THREADNAME];     // スレ名
    Char   boardNick     [MAX_NICKNAME];       // ボードニックネーム 
    UInt16 dirIndex;                           // スレのディレクトリIndex番号
    UInt16 reserved;                           // (未使用)
    UInt8  state;                              // メッセージ取得状況
    UInt8  msgAttribute;                       // メッセージの属性
    UInt8  msgState;                           // メッセージの記録先
    UInt8  bbsType;                            // BBS(メッセージ)の取得形式
    UInt16 maxLoc;                             // 最大メッセージ番号
    UInt16 currentLoc;                         // 表示メッセージ番号
    UInt32 fileSize;                           // ファイルサイズ
} NNshSubjectDatabase;

#pragma pack()
///////////////  } NNsi Version 1.23 Revision000 より  ///////////////



typedef struct MemoRecord {
    DWORD dwRecordID;
    DWORD dwStatus;
    DWORD dwPosition;
    char  szMemo[MAX_MEMO_SIZE];
	DWORD dwPrivate;
	DWORD dwCategoryID;
} MEMORECORD;


//
//  Field ID's
//
#define mpFLDRecordID		0
#define mpFLDStatus			1
#define mpFLDPosition		2
#define mpFLDMemo			3
#define mpFLDPrivate		4
#define mpFLDCategoryID		5
#define mpFLDCount			6


// TODO What is this?
#define mpSCHEMAResource  64


// Default record position for records added on PC
#define mpDefaultPosition (DWORD)0x7fffffff

typedef struct NNsiMonaSchema {
    DWORD lResourceID;
    DWORD lFieldsPerRow;
    DWORD lRecordIdPos;
    DWORD lRecordStatusPos;
    DWORD lPlacementPos;
    WORD wFieldCounts;
    WORD wFieldArray[mpFLDCount];
} MEMOSCHEMA;



class CNNsiMonaRecord 
{
public:
    CNNsiMonaRecord(void);
    virtual ~CNNsiMonaRecord();

public:
    DWORD m_dwRecordID;
    DWORD m_dwStatus;
    DWORD m_dwPosition;
    CPString m_csMemo;
	DWORD m_dwPrivate;
	DWORD m_dwCategoryID;

public:
    virtual void Reset(void);

private:

};
#endif



