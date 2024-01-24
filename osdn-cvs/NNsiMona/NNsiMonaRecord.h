/*!
 *  $Id: NNsiMonaRecord.h,v 1.1.1.1 2005/05/07 07:51:09 mrsa Exp $
 *     NNsi�̃��R�[�h�\���̒�`�t�@�C��...
 *
 */
#ifndef __NNsiMona_RECORD_H_
#define __NNsiMona_RECORD_H_

#include <CPalmRec.h>
#include <CPString.h>

#define NNsiMona_OBJECT_VERSION 0x4D500100  //MP 1.0

#define MAX_MEMO_SIZE   MAX_RECORD_SIZE


///////////////  NNsi Version 1.23 Revision000 ��� {  ///////////////
#pragma pack(2)

/** PalmTypes.h **/
typedef char          Char;     //  8�r�b�g�����t���f�[�^
typedef unsigned char UInt8;    //  8�r�b�g�����Ȃ��f�[�^
typedef WORD          Int16;    // 16�r�b�g�����t���f�[�^
typedef WORD          UInt16;   // 16�r�b�g�����Ȃ��f�[�^
typedef DWORD         Int32;    // 32�r�b�g�����t���f�[�^
typedef DWORD         UInt32;   // 32�r�b�g�����Ȃ��f�[�^

/** ���̑��̐ݒ�l **/
#define MARGIN                       4

/** BBS-DB�̕�����T�C�Y **/
#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16

/** Thread-DB �̕�����T�C�Y **/
#define MAX_THREADFILENAME          32
#define MAX_THREADNAME              72


// ���(�f�[�^�x�[�X�̃��R�[�h�\��) : BBS-NNsi.pdb
typedef struct {
    Char   boardNick[MAX_NICKNAME];    // �{�[�h�j�b�N�l�[�� 
    Char   boardURL [MAX_URL];         // �{�[�hURL
    Char   boardName[MAX_BOARDNAME];   // �{�[�h��
    UInt8  bbsType;                    // �{�[�hTYPE
    UInt8  state;                      // �{�[�h�g�p���
    Int16  reserved;                   // (�\��̈�F�ȑO�͎g�p���Ă���)
} NNshBoardDatabase;

// �X�����(�f�[�^�x�[�X�̃��R�[�h�\��) : Subject-NNsi.pdb
typedef struct {
    Char   threadFileName[MAX_THREADFILENAME]; // �{�[�h��
    Char   threadTitle   [MAX_THREADNAME];     // �X����
    Char   boardNick     [MAX_NICKNAME];       // �{�[�h�j�b�N�l�[�� 
    UInt16 dirIndex;                           // �X���̃f�B���N�g��Index�ԍ�
    UInt16 reserved;                           // (���g�p)
    UInt8  state;                              // ���b�Z�[�W�擾��
    UInt8  msgAttribute;                       // ���b�Z�[�W�̑���
    UInt8  msgState;                           // ���b�Z�[�W�̋L�^��
    UInt8  bbsType;                            // BBS(���b�Z�[�W)�̎擾�`��
    UInt16 maxLoc;                             // �ő僁�b�Z�[�W�ԍ�
    UInt16 currentLoc;                         // �\�����b�Z�[�W�ԍ�
    UInt32 fileSize;                           // �t�@�C���T�C�Y
} NNshSubjectDatabase;

#pragma pack()
///////////////  } NNsi Version 1.23 Revision000 ���  ///////////////



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



