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

typedef struct {
    UInt32  NNsiVersion;
    UInt32  prefSize;
    UInt16  prefType;
    UInt16  debugMode;
} NNshSoftwareVersionInfo;

// �V�X�e�����(�N�����Ƀf�[�^�x�[�X����ǂݍ��ޏ��A�A�A��������...)
typedef struct {
    /** NNsi�ݒ� **/
    Char    handleName[BUFSIZE]; // �R�e�n����
    Char    bbs_URL   [MAX_URL]; // BBS�ꗗ�擾�p�t�q�k
    Char    proxyURL  [MAX_URL]; // Proxy�̃z�X�g��
    Int32   netTimeout;          // �ʐM�^�C���A�E�g�l
    UInt16  getAllThread;        // �S�ẴX�������擾
    UInt16  confirmationDisable; // �m�F�_�C�A���O���ȗ�����
    UInt16  openAutomatic;       // �X���擾�㎩���I�ɃI�[�v��
    UInt16  writeAlwaysSage;     // sage��������
    UInt16  useBookmark;         // ������@�\���g��
    UInt16  searchCaseless;      // ���������p�啶���������𖳎�����
    UInt16  offChkLaunch;        // NNsi�N������OFFLINE�X������
    UInt16  disableUnderline;    // �X���Q�Ǝ���UNDERLINE OFF
    UInt16  debugMessageON;      // �f�o�b�O���b�Z�[�W�̕\��
    UInt16  printNofMessage;     // �X���ꗗ�ŃX������\��
    UInt16  boldMessageNum;      // �Q�Ɖ�ʂŃX���ԍ����y�z�ł�����
    UInt16  useVFS;              // VFS�V�X�e���̎g�p�ېݒ�
    UInt16  bufferSize;          // ���[�N�̈�Ƃ��Ċm�ۂ���o�b�t�@�T�C�Y
    UInt16  partGetSize;         // ���b�Z�[�W������M�T�C�Y
    UInt16  enablePartGet;       // �����_�E�����[�h
    UInt16  lastFrmID;           // �I�����ɕ\�����Ă������
    UInt16  lastBBS;             // �I�����ɕ\�����Ă���BBS��
    UInt16  openMsgIndex;        // �\�����郁�b�Z�[�WINDEX(�ԍ�)
    UInt16  bookMsgIndex;        // Bookmark���b�Z�[�WINDEX
    UInt16  notCursor;           // �Q�Ǝ��J�[�\���\�����Ȃ�
    UInt16  bookMsgNumber;       // Bookmark���b�Z�[�W�ԍ�
    UInt16  openThreadIndex;     // �\������X�����X�g�ԍ�
    UInt16  useFixedHandle;      // �R�e�n���@�\���g�p����
    UInt16  disconnectNNsiEnd;   // NNsi�I�����ɉ���ؒf
    UInt16  useProxy;            // Proxy�o�R�ł̃A�N�Z�X
    UInt16  proxyPort;           // Proxy�̃|�[�g�ԍ�
    UInt16  useCookieWrite;      // ��M����Cookie���g�p���ď�������
    UInt16  copyDelReadOnly;     // �Q�Ɛ�p�X���ɃR�s�[�����Ƃ��A�����ɍ폜
    UInt16  vfsOnAutomatic;      // VFS�g�p�\���Ɏ����I��VFS ON
    UInt16  currentFont;         // �t�H���gID(Starndard�p)
    UInt16  notListReadOnly;     // "�擾�ςݑS��"�ɂ͎Q�Ɛ�p���O�͕\�����Ȃ�
    UInt16  redrawAfterConnect;  // �ڑ����ʂ��ĕ`�悷��
    UInt16  insertReplyNum;      // �������ݎ��ɎQ�ƃX���ԍ���}������
    UInt16  useARMlet;           // PalmOS5�p�@�\���g�p
    UInt16  sonyHRFont;          // (Sony HR�p)Tiny�t�H���gID
    UInt16  useSonyTinyFont;     // (Sony HR�p)Tiny�t�H���g���g�p����
    UInt16  notAutoFocus;        // �������ݎ��A�����I�ɏ���Field�Ɉړ����Ȃ�
    UInt16  titleListSize;       // �X���ꗗ�ɕ\������^�C�g���̒���
    UInt16  browseMesNum;        // Browser�ŊJ�����b�Z�[�W�ԍ�
    UInt16  disconnArrivalEnd;   // �u�V���m�F�v�I�����ɉ����ؒf����
    UInt16  autoOpenNotRead;     // �u�V���m�F�v�I����Ɂu���ǂ���v��
    UInt16  jogBackBtnAsGo;      // Jog Back�{�^����Go�{�^���Ɠ�����
    UInt16  addReturnToList;     // JumpList�Ɂu�ꗗ�֖߂�v��ǉ�
    UInt16  jogBackBtnAsMenu;    // �ꗗ�\����Jog Back�{�^���ł�Menu�\��
    UInt16  notUseSilk;          // Silk������s��Ȃ�
} NNshSavedPref;

// ���(�f�[�^�x�[�X�̍\��)
typedef struct {
    Char   boardNick[MAX_NICKNAME];    // �{�[�h�j�b�N�l�[�� 
    Char   boardURL [MAX_URL];         // �{�[�hURL
    Char   boardName[MAX_BOARDNAME];   // �{�[�h��
    UInt8  bbsType;                    // �{�[�hTYPE
    UInt8  state;                      // �{�[�h�g�p���
    Int16  threadCnt;                  // (�擾�ς�)�X���^�C�g����
} NNshBoardDatabase;

// �X�����(�f�[�^�x�[�X�̍\��)
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

// �X���i�[�f�B���N�g�����(�f�[�^�x�[�X�̍\��)
typedef struct {
    UInt16   dirIndex;                         // �f�B���N�g��Index(ID)
    Char     boardNick[MAX_NICKNAME];          // �{�[�h�j�b�N�l�[�� 
    Char     dirName[MAX_DIRNAME];             // �f�B���N�g������
    UInt16   previousIndex;                    // �ЂƂ�̃f�B���N�g��Index
    UInt32   depth;                            // �f�B���N�g���̐[�x
    UInt32   reserved;                         // �\���P
} NNshDirectoryDatabase;

// �C���f�b�N�X���(�ꎞ�̈�)
typedef struct {
    UInt16   bbsIndex;                 // �g�p����BBS��INDEX(�����ɂ͐擪)
} NNshBBSRelation;

// BBS�g�p���(�ꎞ�̈�ABBS�I����ʂŎg�p)
typedef struct
{
  Int16    currentPage;                // ���ݕ\�����Ă���y�[�W�ԍ�
  Int16    maxPage;                    // �ő�̃y�[�W�ԍ�
  UInt16   nofRecords;                 // BBS���̃��R�[�h��
  UInt16   nofRow;                     // 1�y�[�W������ɕ\���\��BBS��
  UInt16  *checkBoxState;              // �`�F�b�N�{�b�N�X�̐�(�SBBS����)
  Char    *BBSName_Temp;               // �a�a�r�����X�g(��ʂɕ\�����鐔��)
  UInt32   BBSName_size;               // �a�a�r�����X�g�̑傫��
  Char     PageLabel[BBSSEL_PAGEINFO]; // �\�����y�[�W���i�[�̈�
} NNshBBS_Temp_Info;

// �\�����̃��b�Z�[�W���(�ꎞ�̈�)
typedef struct
{
    UInt32      nofMsg;                // �ǂݍ��񂾃X���̍ő僁�b�Z�[�W�ԍ�
    UInt16      openMsg;               // ���݊J���Ă��郁�b�Z�[�W�ԍ�
    UInt16      nofJump;               // �W�����v��
    UInt16      jumpMsg[JUMPBUF];      // �X���Q�Ɣԍ����߂̐�
    UInt16      histIdx;               // �O��J�������b�Z�[�W�ԍ���
    UInt16      prevMsg[PREVBUF];      // �O��J�������b�Z�[�W�o�b�t�@
    UInt32      fileSize;              // �ǂݍ��ݒ��t�@�C���̃t�@�C����
    UInt32      msgOffset[NNSH_MESSAGE_MAXNUMBER]; // �X���C���f�b�N�X
    UInt16      reserved;              // (�\��)
    UInt16      bbsType;               // BBS�^�C�v
    EventType   event;                 // �_�~�[�̃C�x���g�p�f�[�^
    Char       *jumpListTitles;        //
    MemHandle   jumpListH;             //
    NNshFileRef fileRef;
    UInt32      fileOffset;            // �Ǎ���ł���f�[�^�̐擪�����offset
    UInt32      fileReadSize;          // ���݃o�b�t�@�ɓǂݍ���ł���f�[�^��
    Char        threadTitle[MAX_THREADNAME]; // �X������
    Char        boardNick  [MAX_NICKNAME];  // �{�[�h�j�b�N�l�[�� 
    Char        buffer     [MARGIN];        // �f�[�^�o�b�t�@(���ۂ͂����Ƃ���)
} NNshMessageIndex;

//  NNsi�S�̂Ŏg�p����O���[�o���̈�
typedef struct
{
    MemHandle   searchTitleH;
    MemHandle   threadIdxH;
    MemHandle   boardIdxH;
    MemHandle   logPrefixH;
    MemHandle   bbsTitleH;
    MemHandle   threadTitleH;
    MemHandle   msgTTLStringH;
    Int16       tappedPrev;
    UInt16      updateDatabaseInfo;  // DB���A�b�v�f�[�g���ꂽ���
    UInt16      useBBS;
    UInt16      netRef;              // �l�b�g���[�N�Q�Ɣԍ�
    UInt16      device;
    UInt16      vfsVol;
    UInt16      browserLaunchCode;   // WebBrowser��Launch Code
    UInt32      browserCreator;      // WebBrowser��CreatorID
    UInt16      prevHRWidth;         // �O���t�B�b�N���[�h�\���p(���Mode�L��)
    UInt16      totalLine;           // �O���t�B�b�N���[�h�\���p(�S�s��)
    UInt16      nofPage;             // �O���t�B�b�N���[�h�\���p(�S�y�[�W��)
    UInt16      currentPage;         // �O���t�B�b�N���[�h�\���p(�\�����y�[�W)
    UInt16      prevHRLines;         // CLIE�n�C���]�\���p(�\���\�s���L����)
#ifdef USE_CLIE
    UInt16      updateHR;            // SILK�T�C�Y�ύX�A��ʕ\�����X�V����
    UInt16      hrRef;               // SONY HR�Q�Ɣԍ�
    UInt16      hrVer;               // Sony HR�̃o�[�W�����ԍ�
    UInt16      notUseSilk;          // Silk������s��Ȃ�
    UInt16      silkRef;             // CLIE�pSILK�}�l�[�W���̎Q�Ɣԍ�(Temp)
    UInt16      silkVer;             // CLIE�pSILK�}�l�[�W����Version�ԍ�(Temp)
    UInt16     *jogInfoBack;         // SONY�W���O���o�b�N�A�b�v
    UInt16    **jogAssistSetBack;    // SONY�W���O�A�V�X�g�o�b�N�A�b�v
    UInt16      tempStatus;          //
#endif // #ifdef USE_CLIE
    UInt16             jumpSelection; // �W�����v�I��
    EventType          dummyEvent;    // �C�x���g�����p�o�b�t�@
    Char               dummyBuffer[MARGIN];
    FormType          *prevBusyForm;
    Char              *bbsTitles;
    UInt16             threadCount;  // �X���ꗗ��ʂɕ\������Ă�X��(LIST)��
    Char              *threadTitles;
    NNshBBS_Temp_Info *tempInfo;
    NNshMessageIndex  *msgIndex;
} NNshWorkingInfo;

#endif
