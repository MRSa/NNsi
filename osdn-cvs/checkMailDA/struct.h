/*============================================================================*
 *  $Id: struct.h,v 1.3 2004/05/08 15:31:16 mrsa Exp $
 *
 *  FILE: 
 *     struct.h
 *
 *  Description: 
 *     structure definitions for Palm application.
 *
 *===========================================================================*/

/********** My Local structures **********/

// DB �̏��擾�p
typedef struct {
    Char    *nameP;
    UInt16  *attributesP;
    UInt16  *versionP;
    UInt32  *crDateP;
    UInt32  *modDateP;
    UInt32  *bckUpDateP;
    UInt32  *modNumP;
    LocalID *appInfoIDP;
    LocalID *sortInfoIDP;
    UInt32  *typeP;
    UInt32  *creatorP;
} NNshDBInfo;

// NNsi�̃��X�g�A�C�e��ID
typedef struct {
  UInt16  popId;
  UInt16  lstId;
} NNshListItem;

// �o�[�W�������(�V�X�e��Preference�ɋL�^����\����)
typedef struct {
    UInt32  NNsiVersion;
    UInt32  prefSize;
    UInt16  prefType;
    UInt16  debugMode;
} NNshSoftwareVersionInfo;

typedef struct {
    UInt16 up;              // �n�[�h�L�[��
    UInt16 down;            // �n�[�h�L�[��
    UInt16 key1;            // �n�[�h�L�[�P
    UInt16 key2;            // �n�[�h�L�[�Q
    UInt16 key3;            // �n�[�h�L�[�R
    UInt16 key4;            // �n�[�h�L�[�S
} NNshHardkeyControl;

// �V�X�e�����(�N�����Ƀf�[�^�x�[�X����ǂݍ��ޏ��)
typedef struct {
    /** NNsi�ݒ� **/
    UInt16  debugMessageON;            // �f�o�b�O���b�Z�[�W�̕\��
    UInt8   getMessageHeader;          // ���b�Z�[�W�w�b�_���擾
    UInt8   confirmationDisable;       // �m�F�_�C�A���O���ȗ�����
    UInt16  disconnect;                // �I�����ɉ���ؒf
    UInt16  protocol;                  // �ʐM�v���g�R��
    UInt16  portNum;                   // �ʐM�|�[�g�ԍ�
    Int32   timeout;                   // �ʐM�^�C���A�E�g�l
    Char    hostName[BUFSIZE];         // �z�X�g��
    Char    userName[BUFSIZE];         // ���[�U��
    Char    password[BUFSIZE];         // �p�X���[�h
    UInt16  useHardKeyControl;         // �n�[�h�L�[��������{����
    UInt16  bufferSize;                // �o�b�t�@�T�C�Y
    NNshHardkeyControl useKey;         // �n�[�h�L�[�̃L�[�ԍ��i�[�̈�
    UInt32  reserved[DB_RESERVE_AREA]; // �\���̈�
} NNshSavedPref;

//  �S�̂Ŏg�p����O���[�o���̈�
typedef struct
{
    FormType     *currentForm;
    FormType     *previousForm;
    FormType     *prevBusyForm;
    // FormActiveStateType formState;
    ControlType  *selP;
    UInt16        btnId;
    UInt16        netRef;
    UInt16        err;
    UInt16        ret;
    UInt16        size;
    NetSocketRef  socketRef;
    Char          tempBuf[BUFSIZE * 2];
    Char         *tempPtr2;
    Char         *tempPtr;
    Char         *sendBuf;
    Char         *recvBuf;
    UInt8         PADDING[64];
    MD5_CTX       context;
    UInt8         digest[20];
#ifdef USE_OSVERSION
    UInt32        palmOSVersion;
#endif
} NNshWorkingInfo;
