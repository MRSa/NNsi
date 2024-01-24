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

// Sony FileDB Header
typedef struct sonyFileDBHeader {
    UInt32  headerID;
    UInt32  fileAttr;
    UInt32  LADate;
    UInt16  ExAttr;
    UInt16  Reserved;
    Char    Filename[256];
} FileDBHeader;

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
    UInt32  bufferSize;          // ���[�N�̈�Ƃ��Ċm�ۂ���o�b�t�@�T�C�Y
    UInt16  useVFS;              // VFS�V�X�e���̎g�p�ېݒ�
    UInt16  vfsUseCompactFlash;  // VFS�͂b�e���g�p����
    UInt16  currentFont;         // �t�H���gID(Starndard�p)
    UInt16  useARMlet;           // PalmOS5�p�@�\���g�p
    UInt16  sonyHRFont;          // (Sony HR�p)Tiny�t�H���gID
    UInt16  disableSonyHR;       // SONY�n�C���]�`�惂�[�h���g�p���Ȃ�
    UInt16  useEndSaveDialog;    // �I�����ɕۑ��_�C�A���O
    UInt16  autoRestore;         // �O��̕ҏW�t�@�C���𕜋A
    UInt16  convertHanZen;       // ���p�J�i���S�p�J�i�ϊ�
    UInt16  useHardKeyControl;   // �n�[�h�L�[��������{����
    UInt16  notAutoFocus;        // �t�H�[�J�X�������ݒ肵�Ȃ�
    UInt16  silkMax;             // �V���N�G���A���g�傷��
    UInt16  notUseTsPatch;       // TsPatch���g�p���Ȃ�
    NNshHardkeyControl useKey;   // �n�[�h�L�[�̃L�[�ԍ��i�[�̈�
    Char    fileName[MAXLENGTH_FILENAME + MARGIN];  // �t�@�C�����L���̈�
    UInt16  fileLocation;        // �t�@�C���̂��肩
    UInt16  kanjiCode;           // �t�@�C���̊����R�[�h
    UInt16  newLineCode;         // �t�@�C���̉��s�R�[�h
} NNshSavedPref;

typedef struct
{
   UInt16 utf8;
   UInt16 sj;
} NNshUTF8ConvTable;

// UnicodeToJis0208 �e�[�u�����R�[�h
typedef struct
{
    UInt16 size;
    UInt16 table[256];
} UnicodeToJis0208Rec;

// Jis0208ToUnicode �e�[�u�����R�[�h
typedef struct
{
    UInt16 code;
} Jis0208ToUnicodeRec;

//  NNsi�S�̂Ŏg�p����O���[�o���̈�
typedef struct
{
    UInt16             device;
    UInt16             vfsVol;
    FormType          *prevBusyForm;
#ifdef USE_CLIE
    UInt16   updateHR;            // SILK�T�C�Y�ύX�A��ʕ\�����X�V����
    UInt16   hrRef;               // SONY HR�Q�Ɣԍ�
    UInt16   hrVer;               // Sony HR�̃o�[�W�����ԍ�
    UInt16   silkRef;             // CLIE�pSILK�}�l�[�W���̎Q�Ɣԍ�(Temp)
    UInt16   silkVer;             // CLIE�pSILK�}�l�[�W���̃o�[�W�����ԍ�(Temp)
    UInt16   prevHRWidth;         // CLIE�n�C���]�\���p(��ʃ��[�h�L���p)
    UInt16  *jogInfoBack;         // SONY�W���O���o�b�N�A�b�v
    UInt16 **jogAssistSetBack;    // SONY�W���O�A�V�X�g�o�b�N�A�b�v
    UInt16   tempStatus;          // 
    UInt16   notUseHR;            // HR������s��Ȃ�
    UInt16   notUseSilk;          // Silk������s��Ȃ�
    UInt16   sonyJogMask[NUMBER_OF_JOGMASKS * 2 + NUMBER_OF_JOGDATAS]; // JOG���
#endif      // #ifdef USE_CLIE
    EventType dummyEvent;         // �C�x���g�����p�o�b�t�@
    Char      dummyBuffer[MARGIN];
    UInt32   palmOSVersion;       // PalmOS�̃o�[�W�����ԍ�
    UInt16   jumpSelection;       // �W�����v�ԍ�
    UInt32   os5HighDensity;      // OS5���𑜓x
    UInt32   browserCreator;      // Web�u���E�U�N���G�[�^ID
    UInt16   browserLaunchCode;   // Web�u���E�U�N���R�[�h
    UInt16   notifyUpdate;        // VFS��Ԓʒm
    UInt16   autoOpenFile;        // �t�@�C���������I�[�v������
    Char     dirName[MAXLENGTH_FILENAME + MARGIN];  // �f�B���N�g�����L���̈�
    MemHandle fileListH;          // �t�@�C�����ꗗ
    DmOpenRef unicode2jisDB;      // UTF8 -> JIS �ϊ��e�[�u��
    DmOpenRef jis2unicodeDB;      // JIS  -> UTF8�ϊ��e�[�u��
} NNshWorkingInfo;

// kaniEdit�p�N���R�[�h��`
typedef enum {
    kaniEditCmdOpenFileStreamReadOnly = sysAppLaunchCmdCustomBase, // 0x8000
    kaniEditCmdOpenFileStream,                                     // 0x8001
    kaniEditCmdOpenVFSreadOnly,                                    // 0x8002
    kaniEditCmdOpenVFS,                                            // 0x8003
    kaniEditCmdOpenVFSdos,                                         // 0x8004
    kaniEditCmdOpenVFSmac,                                         // 0x8005
    kaniEditCmdOpenVFSjis,                                         // 0x8006
    kaniEditCmdOpenVFSeuc,                                         // 0x8007
    kaniEditCmdOpenVFSutf8                                         // 0x8008
} MyAppCustomActionCodes;

#endif
