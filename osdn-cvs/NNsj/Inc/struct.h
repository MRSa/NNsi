/*============================================================================*
 *  FILE: 
 *     struct.h
 *
 *  Description: 
 *     �\���̒�`
 *
 *===========================================================================*/

/********** My Local structures **********/
#ifdef STRUCT_H
#else
#define STRUCT_H

// �t�@�C���Q�Ǝq
typedef struct {
  UInt16   fileLocation;
  FileRef  vfsRef;
  FileHand streamRef;
} NNshFileRef;

// ������(�J���}��؂�)�̃��X�g�\��
typedef struct
{
    UInt16    nofWord;        // ������(���X�g)��
    Char     *wordString;     // ������̗̈�(���X�g�`��)
    MemHandle wordmemH;       // �����񃊃X�g�f�[�^
} NNshWordList;

// DB �̏��擾�p
typedef struct {
    Char *nameP;
    UInt16 *attributesP;
    UInt16 *versionP;
    UInt32 *crDateP;
    UInt32 *modDateP;
    UInt32 *bckUpDateP;
    UInt32 *modNumP;
    LocalID *appInfoIDP;
    LocalID *sortInfoIDP;
    UInt32 *typeP;
    UInt32 *creatorP;
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
    Char  up;              // �n�[�h�L�[��
    Char  down;            // �n�[�h�L�[��
    Char  key1;            // �n�[�h�L�[�P
    Char  key2;            // �n�[�h�L�[�Q
    Char  key3;            // �n�[�h�L�[�R
    Char  key4;            // �n�[�h�L�[�S
} NNshHardkeyControl;

// �V�X�e�����(�N�����Ƀf�[�^�x�[�X����ǂݍ��ޏ��A�A�A��������...)
typedef struct {
    /** NNsi�ݒ� **/
    UInt16  confirmationDisable; // �m�F�_�C�A���O���ȗ�����
    UInt16  debugMessageON;      // �f�o�b�O���b�Z�[�W�̕\��
    UInt16  bufferSize;          // ���[�N�̈�Ƃ��Ċm�ۂ���o�b�t�@�T�C�Y
    Int32   netTimeout;          // �ʐM�^�C���A�E�g�l
    UInt16  nofRetry;            // �ʐM���g���C��
    UInt16  useVFS;              // VFS�V�X�e���̎g�p�ېݒ�
    UInt16  vfsUseCompactFlash;  // VFS�͂b�e���g�p����
    UInt16  lastFrmID;           // �I�����ɕ\�����Ă������
    UInt16  vfsOnAutomatic;      // VFS�g�p�\���Ɏ����I��VFS ON
    UInt16  currentFont;         // �t�H���gID(Starndard�p)
    UInt16  useARMlet;           // PalmOS5�p�@�\���g�p
    UInt16  sonyHRFont;          // (Sony HR�p)Tiny�t�H���gID
    UInt16  disableSonyHR;       // SONY�n�C���]�`�惂�[�h���g�p���Ȃ�
    UInt16  useSonyTinyFont;     // (Sony HR�p)Tiny�t�H���g���g�p����
    UInt16  useProxy;            // Proxy�o�R�ł̃A�N�Z�X
    UInt16  redrawAfterConnect;  // �ڑ����ʂ��ĕ`�悷��
    Char    fileName[MAXLENGTH_FILENAME + MARGIN];  // �t�@�C�����L���̈�
    UInt16  convertHanZen;       // ���p�J�i���S�p�J�i�ϊ�
    Char    proxyURL[MAX_URL];   // Proxy�̃z�X�g��
    UInt16  proxyPort;           // Proxy�̃|�[�g�ԍ�
    UInt16  useHardKeyControl;   // �n�[�h�L�[��������{����
    NNshHardkeyControl useKey;   // �n�[�h�L�[�̃L�[�ԍ��i�[�̈�
} NNshSavedPref;


typedef struct {
    UInt32  dateTime;
    UInt8   logLevel;
    UInt8   reserved0;
    Char    logData[MAX_LOGRECLEN];
    UInt16  errCode;
} NNshMacroResult;

typedef struct {
    UInt16 operator;
    Char   data[MAX_STRLEN];
} NNshMacroString;

typedef struct {
    UInt16 operator;
    UInt16 subCommand;
    UInt16 data1;
    UInt16 data2;
} NNshMacroNumber;

// �}�N��DB�̃��R�[�h�\��
typedef struct {
    UInt16  seqNum;
    UInt16  opCode;
    UInt8   dst;
    UInt8   src;
    union {
       NNshMacroString strData;    // �����f�[�^�̂Ƃ�
       NNshMacroNumber numData;    // ���l�f�[�^�̂Ƃ�
    } MacroData;
} NNshMacroRecord;

//  NNsi�S�̂Ŏg�p����O���[�o���̈�
typedef struct
{
    UInt16             device;
    UInt16             vfsVol;
    UInt16             netRef;
    FormType          *prevBusyForm;
    NNshWordList      *featureList;       // �@�\�ݒ胉�x��
    NNshMacroRecord   *dataRec;           // �}�N���f�[�^�̃��R�[�h
#ifdef USE_CLIE
    UInt16   updateHR;            // SILK�T�C�Y�ύX�A��ʕ\�����X�V����
    UInt16   hrRef;               // SONY HR�Q�Ɣԍ�
    UInt16   hrVer;               // Sony HR�̃o�[�W�����ԍ�
    UInt16   silkRef;             // CLIE�pSILK�}�l�[�W���̎Q�Ɣԍ�(Temp)
    UInt16   silkVer;             // CLIE�pSILK�}�l�[�W���̃o�[�W�����ԍ�(Temp)
    UInt16   prevHRWidth;         // CLIE�n�C���]�\���p(��ʃ��[�h�L���p)
    UInt16   totalLine;           // CLIE�n�C���]�\���p(�S�s��)
    UInt16   nofPage;             // CLIE�n�C���]�\���p(�S�y�[�W��)
    UInt16   currentPage;         // CLIE�n�C���]�\���p(�\�����y�[�W)
    UInt16   prevHRLines;         // CLIE�n�C���]�\���p(�\���\�s���̋L����)
    UInt16  *jogInfoBack;         // SONY�W���O���o�b�N�A�b�v
    UInt16 **jogAssistSetBack;    // SONY�W���O�A�V�X�g�o�b�N�A�b�v
    UInt16   tempStatus;          // 
    UInt16   notUseHR;            // HR������s��Ȃ�
    UInt16   notUseSilk;          // Silk������s��Ȃ�

#endif      // #ifdef USE_CLIE
    EventType dummyEvent;         // �C�x���g�����p�o�b�t�@
    Char      dummyBuffer[MARGIN];
    UInt32   palmOSVersion;       // PalmOS�̃o�[�W�����ԍ�
    MemHandle fileListH;          // �t�@�C�����ꗗ
    UInt16   jumpSelection;       // �W�����v�ԍ�
    UInt32   os5HighDensity;      // OS5���𑜓x
    UInt32   browserCreator;      // Web�u���E�U�N���G�[�^ID
    UInt16   browserLaunchCode;   // Web�u���E�U�N���R�[�h
    UInt16   notifyUpdate;        // VFS��Ԓʒm
    UInt16   maxScriptLine;       // �X�N���v�g�f�[�^�i�[�T�C�Y�̍ő�
    UInt16   currentScriptLine;   // �X�N���v�g�f�[�^�̃��C����
    Char    *scriptArea;          // �X�N���v�g�f�[�^�i�[�̈�
    UInt16   pageTopLine;         // �X�N���v�g�f�[�^�y�[�WTop(�[���X�^�[�g)
    UInt16   currentCursor;       // �X�N���v�g�f�[�^�̌��ݎQ�Ƃ��Ă���f�[�^�ʒu
    Boolean  editFlag;            // �ҏW�t���O
    UInt16   editMode;            // �ҏW���[�h
    Char     infoArea[MINIBUF];   // ���̈�Ƀf�t�H���g�\���������
} NNshWorkingInfo;

#endif
