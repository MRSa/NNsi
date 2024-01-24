/*============================================================================*
 *
 * $Id: struct.h,v 1.140 2012/01/10 16:27:42 mrsa Exp $
 *
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

typedef struct {
    Char    tabName[MAX_BOARDNAME];        // �^�u��
    UInt16  newArrival;                    // �V���m�F�^�u
    UInt16  getList;                       // �ꗗ�擾�^�u
    UInt16  condition;                     // ����
    UInt16  boardNick;                     // �^�u
    UInt16  boardCondition;                // �w��
    UInt16  threadLevel;                   // ���x���ݒ�
    UInt16  threadStatus;                  // �X����Ԏw��
    UInt16  threadCreate;                  // �X���쐬��
    UInt16  stringSet;                     // ������w��
    Char    string1[MINIBUF];              // ����������P
    Char    string2[MINIBUF];              // ����������Q
    Char    string3[MINIBUF];              // ����������R
} NNshCustomTab;

typedef struct {
    UInt16 up;              // �n�[�h�L�[��
    UInt16 down;            // �n�[�h�L�[��
    UInt16 key1;            // �n�[�h�L�[�P
    UInt16 key2;            // �n�[�h�L�[�Q
    UInt16 key3;            // �n�[�h�L�[�R
    UInt16 key4;            // �n�[�h�L�[�S
    UInt16 jogPush;         // �W���O����
    UInt16 jogBack;         // �W���OBack
    UInt16 clieCapture;     // CLIE�L���v�`��(NX)
    UInt16 left;            // �n�[�h�L�[��
    UInt16 right;           // �n�[�h�L�[�E
} NNshHardkeyControl;

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
    UInt8   useNameOneLine;      // ���O�����P�s������
    UInt8   boldMessageNum;      // �Q�Ɖ�ʂŃX���ԍ����y�z�ł�����
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
    UInt16  copyDelReadOnly;     // �Q�ƃ��O�X���ɃR�s�[�����Ƃ��A�����ɍ폜
    UInt8   vfsUseCompactFlash;  // VFS�g�p�\����CF���g�p����
    UInt8   vfsOnAutomatic;      // VFS�g�p�\���Ɏ����I��VFS ON
    UInt16  currentFont;         // �t�H���gID(Starndard�p)
    UInt8   notListReadOnlyNew;  // "���ǂ���"�ɎQ�ƃ��O�͕\�����Ȃ�
    UInt8   notListReadOnly;     // "�擾�ςݑS��"�ɎQ�ƃ��O�͕\�����Ȃ�
    UInt16  redrawAfterConnect;  // �ڑ����ʂ��ĕ`�悷��
    UInt16  insertReplyNum;      // �������ݎ��ɎQ�ƃX���ԍ���}������
    UInt16  useARMlet;           // PalmOS5�p�@�\���g�p
    UInt16  sonyHRFont;          // (Sony HR�p)Tiny�t�H���g(�Q�Ɨp)
    UInt16  sonyHRFontTitle;     // (Sony HR�p)Tiny�t�H���g(�ꗗ�p)
    UInt16  useSonyTinyFont;     // (Sony HR�p)Tiny�t�H���g���g�p����
    UInt16  useSonyTinyFontTitle; // (Sony HR�p)Tiny�t�H���g���g�p����
    UInt8   notOpenSilkWrite;     // �������ݎ��A�����I�ɃV���N���g�債�Ȃ�
    UInt8   notAutoFocus;         // �������ݎ��A�����I�ɏ���Field�Ɉړ����Ȃ�
    UInt16  writeBufferSize;      // �X���ꗗ�ɕ\������^�C�g���̒���
    UInt16  browseMesNum;        // Browser�ŊJ�����b�Z�[�W�ԍ�
    UInt8   startMacroArrivalEnd; // �u�V���m�F�v�I�����Ƀ}�N�������s����
    UInt8   disconnArrivalEnd;   // �u�V���m�F�v�I�����ɉ����ؒf����
    UInt8   autoBeep;            // �u�V���m�F�v�I����Ƀr�[�v����炷
    UInt8   autoOpenNotRead;     // �u�V���m�F�v�I����Ɂu���ǂ���v��
    UInt16  addReturnToList;     // JumpList�Ɂu�ꗗ�֖߂�v��ǉ�
    UInt16  notUseSilk;          // Silk������s��Ȃ�
    UInt16  writeSequence2;      // �������݃V�[�P���XPART2
    UInt16  startTitleRec;       // �\�����^�C�g���ꗗ�̐擪���R�[�h
    UInt16  endTitleRec;         // �\�����^�C�g���ꗗ�̖������R�[�h
    UInt16  currentSelectRec;    // �\�����^�C�g���ꗗ�̑I�����R�[�h
    UInt16  selectedTitleItem;   // �\�����^�C�g���ꗗ�̑I�����X�g�ԍ�
    UInt16  titleDispState;      // �^�C�g���ꗗ�\�����
    UInt16  convertHanZen;       // �Q�Ǝ��A���p�J�i���S�p�J�i�ϊ�
    UInt16  addLineDisconn;      // �Q�Ǝ��A�I�����j���[�ɉ���ؒf
    UInt16  addLineGetPart;      // �Q�Ǝ��A�I�����j���[�ɍ����擾
    UInt16  addJumpTopMsg;       // �Q�Ǝ��A�I�����j���[�ɐ擪
    UInt16  addJumpBottomMsg;    // �Q�Ǝ��A�I�����j���[�ɖ���
    UInt16  addMenuSelAndWeb;    // �Q�Ǝ��A�I�����ɑS�I��+WebBrowser�ŊJ��
    UInt16  addMenuMsg;          // �Q�Ǝ��A�I�����Ƀ��j���[��ǉ� 
    UInt16  addMenuBackRtnMsg;   // �Q�Ǝ��A�I�����Ɂu�P�߂�v��ǉ� 
    UInt16  addMenuGraphView;    // �Q�Ǝ��A�I�����Ɂu�`�惂�[�h�ύX�v��ǉ�
    UInt8   writingReplyMsg;     // Write�{�^�����u���p�J�L�R�v�ɂ���
    UInt8   addMenuFavorite;     // �Q�Ǝ��A�I�����Ɂu���C�ɓ���ؑցv��ǉ�
    UInt8   addMenuGetThreadNum; // �Q�Ǝ��A�I�����Ɂu�X���Ԏw��擾�v��ǉ�
    UInt8   addMenuOutputMemo;   // �Q�Ǝ��A�I�����Ɂu�������֏o�́v��ǉ�
    UInt8   addNgSetting3;       // �Q�Ǝ��A�I�����ɁuNG�ݒ�v��ǉ� 
    UInt8   addDeviceInfo;       // �Q�Ǝ��A�I�����Ƀf�o�C�X����ǉ� 
    UInt8   addBtOnOff;          // �I������Bluetooth on/off��ǉ�
    UInt8   addMenuTitle;        // �ꗗ���A�I�����Ƀ��j���[��ǉ� 
    UInt16  addMenuDeleteMsg;    // �ꗗ���A�I������MSG�폜��ǉ� 
    UInt16  addMenuCopyMsg;      // �ꗗ���A�I�����ɎQ�ƃ��O�փR�s�[��ǉ� 
    UInt16  addMenuGetMode;      // �ꗗ���A�I�����ɃX���擾���[�h�ؑ�
    UInt16  addMenuGraphTitle;   // �ꗗ���A�I�����Ɂu�`�惂�[�h�ύX�v��ǉ�
    UInt16  addMenuMultiSW1;     // �ꗗ���A�I�����ɑ��ړI�X�C�b�`�P��ǉ�
    UInt16  addMenuMultiSW2;     // �ꗗ���A�I�����ɑ��ړI�X�C�b�`�Q��ǉ�
    UInt16  addMenuNNsiEnd;      // �ꗗ���A�I������NNsi�I����ǉ� 
    UInt16  addMenuDeviceInfo;   // �ꗗ���A�I�����Ƀf�o�C�X����ǉ�
    UInt16  disableSonyHR;       // SONY�n�C���]�`�惂�[�h���g�p���Ȃ�
    UInt16  useColor;            // �J���[�\�����g�p����
    UInt8   colorError;          // �G���[���̃J���[
    UInt8   colorOver;           // 1000���̃J���[
    UInt8   colorNotYet;         // ���擾�X���̃J���[
    UInt8   colorNew;            // �V���X���̃J���[
    UInt8   colorUpdate;         // �X�V�X���̃J���[
    UInt8   colorRemain;         // ���ǂ���X���̃J���[
    UInt8   colorAlready;        // �S�ēǂ񂾃X���̃J���[
    UInt8   colorUnknown;        // �s���ȃX���̃J���[
    UInt8   colorBackGround;     // �ꗗ��ʂ̔w�i�F
    UInt8   colorButton;         // �㉺�{�^���̐F
    UInt8   colorViewBG;         // �Q�Ɖ�ʂ̔w�i�F
    UInt8   colorViewFG;         // �Q�Ɖ�ʂ̕����F
    UInt8   rawDisplayMode;           // RAW�\�����[�h
    UInt8   notDelOffline;            // OFFLINE�X���������ɃX���������Ȃ�
    UInt8   sortGoMenuAnchor;         // Go���j���[�A���J�[�𐮗񂳂���
    UInt8   notFocusNumField;         // �X���Q�Ɖ�ʂŃt�H�[�J�X�ړ����Ȃ�
    UInt16  disableUnderlineWrite;    // �������ݎ��A���_�[���C��OFF
    UInt16  usageOfTitleMultiSwitch1; // �X���ꗗ��ʂ̑��ړI�X�C�b�`�P
    UInt16  usageOfTitleMultiSwitch2; // �X���ꗗ��ʂ̑��ړI�X�C�b�`�Q
    UInt16  autoDeleteNotYet;         // �ړ����ɖ��擾������폜����
    UInt16  notCheckBBSURL;           // ��URL�̗L���`�F�b�N�����Ȃ�
    UInt16  displayFavorLevel;        // ���C�ɓ���ɕ\������Œ�X�����x��
    UInt16  newArrivalNotRead;        // �u���ǂ���v�^�u�ł̐V���m�F
    UInt16  getReserveFeature;        // �X���擾�\��@�\���g��
    Char    searchStrMessage[MAX_SEARCH_STRING]; // �X��������
    UInt32  searchMode;               // �������[�h
    UInt16  bbsOverwrite;             // BBS�ꗗ�����������㏑������
    UInt16  useImodeURL;              // �ꗗ�Ńu���E�U�J���Ƃ�i-mode�p
    UInt16  nofRetry;                 // �ʐM�^�C���A�E�g���̃��g���C��
    UInt16  autoUpdateGetError;       // �����擾���s���ɍĎ擾
    UInt16  dispBottom;               // ���b�Z�[�W�𖖔�����\���ł���悤��
    UInt16  blockDispMode;                 // �Q�Ǝ��A���X��A���\��
    Char    multiBtn1Caption[MAX_CAPTION]; // ���ړI�{�^���P�̃{�^�����x��
    Char    multiBtn2Caption[MAX_CAPTION]; // ���ړI�{�^���Q�̃{�^�����x��
    Char    multiBtn3Caption[MAX_CAPTION]; // ���ړI�{�^���R�̃{�^�����x��
    Char    multiBtn4Caption[MAX_CAPTION]; // ���ړI�{�^���S�̃{�^�����x��
    Char    multiBtn5Caption[MAX_CAPTION]; // ���ړI�{�^���T�̃{�^�����x��
    Char    multiBtn6Caption[MAX_CAPTION]; // ���ړI�{�^���U�̃{�^�����x��
    UInt16  multiBtn1Feature;              // ���ړI�{�^���P��Feature
    UInt16  multiBtn2Feature;              // ���ړI�{�^���Q��Feature
    UInt16  multiBtn3Feature;              // ���ړI�{�^���R��Feature
    UInt16  multiBtn4Feature;              // ���ړI�{�^���S��Feature
    UInt16  multiBtn5Feature;              // ���ړI�{�^���T��Feature
    UInt16  multiBtn6Feature;              // ���ړI�{�^���U��Feature

    NNshHardkeyControl useKey;             // �n�[�h�L�[�̑���������
    NNshHardkeyControl ttlFtr;             // �ꗗ��ʂł̃n�[�h�L�[������
    NNshHardkeyControl viewFtr;            // �Q�Ɖ�ʂł̃n�[�h�L�[������

    UInt16  useHardKeyControl;             // �n�[�h�L�[�𐧌䂷��
    UInt16  checkDuplicateThread;          // �ꗗ�\�����ɏd���̊m�F���s��
    UInt16  hideMessage;                   // ��[���邠�ځ[����{
    Char    hideWord1[HIDEBUFSIZE];        // ���ځ[��L�[���[�h�P
    Char    hideWord2[HIDEBUFSIZE];        // ���ځ[��L�[���[�h�Q
    NNshCustomTab custom1;                 // ���[�U�ݒ�P
    NNshCustomTab custom2;                 // ���[�U�ݒ�Q
    NNshCustomTab custom3;                 // ���[�U�ݒ�R
    NNshCustomTab custom4;                 // ���[�U�ݒ�S
    NNshCustomTab custom5;                 // ���[�U�ݒ�T
    UInt8   listMesNumIsNotReadNum;        // �ꗗ��ʂŕ\�����郌�X���͖��ǐ�
    UInt8   writeJogPushDisable;           // �������ݎ�PUSH�{�^���𖳌�
    UInt8   autostartMacro;                // NNsi�N�����Ƀ}�N�����s
    UInt8   enableNewArrivalHtml;          // �܂�BBS/�������@JBBS�̐V���L��
    UInt16  useRegularExpression;          // NG���[�h�Ő��K�\�����g�p����
    UInt16  vfsOnNotDBCheck;               // VFS ON�ł�DB�`�F�b�N���Ȃ�
    UInt16  writeMessageAutoSave;          // �������݉�ʏI�����A���X��ۑ�
    Char    launchPass[PASSSIZE];          // �N�����L�[���[�h�ݒ�
    UInt16  readOnlySelection;             // �Q�ƃ��O�f�B���N�g���I��ԍ�
    UInt16  addMenuDirSelect;              // �ꗗ��ʃ��j���[��Dir�I����ǉ�
    UInt16  use_DAplugin;                  // �V���m�F����DA���s
    UInt16  listAndUpdate;                 // �ꗗ�擾�Ɠ����ɐV���m�F
    UInt16  msgNumLimit;                   // ���b�Z�[�W�ԍ��̍ő吔
    UInt8   notUseTsPatch;                 // TsPatch�@�\���g��Ȃ�
    UInt8   getROLogLevel;                 // �Q�ƃ��O�̎擾���x��
    UInt16  viewMultiBtnFeature;           // �Q�Ɖ�ʍ����̃{�^��Feature
    Char    viewSearchStrHeader[SEARCH_HEADLEN]; // �Q�Ɖ�ʌ������̏����w�b�_
    UInt16  enableSearchStrHeader;         // �Q�Ǝ��Ɍ����p������������w��
    UInt16  viewTitleSelFeature;           // �Q�Ɖ�ʍ����̃{�^��Feature
    Char    oysterUserId[OYSTERID_LEN];    //  2ch���[�UID
    Char    messageSeparator[SEPARATOR_LEN];  // �X���A���\�����̋�؂蕶����
    UInt8   colorMsgHeader;                 // �Q�Ɖ�ʂ̃w�b�_�F
    UInt8   colorMsgHeaderBold;             // �Q�Ɖ�ʂ̃w�b�_�F(����)
    UInt8   colorMsgFooter;                 // �Q�Ɖ�ʂ̃t�b�^�F
    UInt8   colorMsgFooterBold;             // �Q�Ɖ�ʂ̃t�b�^�F(����)
    UInt8   colorMsgInform;                 // �Q�Ɖ�ʂ̏��F
    UInt8   colorMsgInformBold;             // �Q�Ɖ�ʂ̏��F(����)
    UInt8   useColorMessageToken;           // �Q�Ɖ�ʂ̕�����ŐF������
    UInt8   useColorMessageBold;            // �Q�Ɖ�ʂ̕�����ŋ�������
    NNshHardkeyControl getLogFeature;       // �Q�ƃ��O�ꗗ�̃n�[�h�L�[�@�\�ݒ�
    UInt16  getLogListPage;                 // �Q�ƃ��O�ꗗ�̍ŏI�\���y�[�W
    UInt16  getLogListSelection;            // �Q�ƃ��O�ꗗ�̍ŏI�I���A�C�e��
    UInt32  prepareDAforNewArrival;         // �V���m�F�O�N������DA�̃N���G�[�^ID
    UInt8   useGZIP;                        // GZIP���k���g�p���� 
    UInt8   reserve0;                       // �Ƃ肠�����ۊǏꏊ�̊m��... 
    UInt8   preOnDAnewArrival;              // �V���m�F�O��DA���N������
    UInt8   showResPopup;                   // �Q�Ɖ�ʂŁA���X��popup�\������
    UInt16  vfsOnDefault;                   // �W����VFS ON�ݒ�
    UInt16  useBe2chInfo;                   // �������ݎ���Be@2ch�̏����g�p����
    Char    be2chId[BE2chID_LEN];           // Be@2ch��ID
    Char    be2chPw[BE2chPW_LEN];           // Be@2ch�̃p�X���[�h
    UInt16  roundTripDelay;                 // ���񎞑҂����� (ms)
    UInt8   searchPickupMode;               // �i���݌������[�h
    UInt8   getLogSiteListMode;             // �Q�ƃ��O�T�C�g�ꗗ���[�h

    Char              cookieStoreBuffer[NNSH_WRITECOOKIE_BUFFER]; //  Cookie���L������ꏊ

    Char    oysterPassword[OYSTER_LEN];     //  2ch�p�X���[�h
    UInt16  selectedTitleItemForGetLog;     // �Q�ƃ��O�T�C�g�ꗗ���[�h�ł̕\���T�C�g�A�C�e��
    UInt32  reserve2;                       // �Ƃ肠�����ۊǏꏊ�̊m��...
} NNshSavedPref;

// ���(�f�[�^�x�[�X�̍\��)
typedef struct {
    Char   boardNick[MAX_NICKNAME];    // �{�[�h�j�b�N�l�[�� 
    Char   boardURL [MAX_URL];         // �{�[�hURL
    Char   boardName[MAX_BOARDNAME];   // �{�[�h��
    UInt8  bbsType;                    // �{�[�hTYPE
    UInt8  state;                      // �{�[�h�g�p���
    Int16  reserved;                   // (�\��̈�F�ȑO�͎g�p���Ă���)
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
  Char     searchName[MAX_BOARDNAME];  // �����w�肷�镶����
  Char     PageLabel[BBSSEL_PAGEINFO]; // �\�����y�[�W���i�[�̈�
} NNshBBS_Temp_Info;

// ���X��؂�p�\����(�f�[�^�x�[�X)
typedef struct {
    UInt32 id;                              // �f�[�^ID
    Char   tokenPatternName[MAX_STRLEN];    // ��؂�p�^�[����
    Char   startToken[MAX_DATATOKEN];       // �؂�o���J�n������
    Char   endToken  [MAX_DATATOKEN];       // �؂�o���I��������
    Char   dataToken [MAX_DATATOKEN];       // �f�[�^�C���f�b�N�X������
    UInt32 lastUpdate;                      // �ŏI�X�V����
    UInt8  usable;                          // �g�p����/���Ȃ�
    UInt8  reserved0;                       // �\��̈�
    UInt16 reserved1;                       // �\��̈�
    UInt32 reserved2;                       // �\��̈�  
} NNshLogTokenDatabase;

// �\�����̃��b�Z�[�W���(�ꎞ�̈�)
typedef struct
{
    UInt32      nofMsg;                // �ǂݍ��񂾃X���̍ő僁�b�Z�[�W�ԍ�
    UInt16      openMsg;               // ���݊J���Ă��郁�b�Z�[�W�ԍ�
    UInt16      dispMsg;               // �\�����Ă��郁�b�Z�[�W��
    UInt16      blockStartMsg;         // �]�[���\�����[�h�ŁA�]�[���̐擪�̃��b�Z�[�W
    UInt16      nofJump;               // �W�����v��
    UInt16      jumpMsg[JUMPBUF];      // �X���Q�Ɣԍ����߂̐�
    UInt16      histIdx;               // �O��J�������b�Z�[�W�ԍ���
    UInt16      prevMsg[PREVBUF];      // �O��J�������b�Z�[�W�o�b�t�@
    UInt32      fileSize;              // �ǂݍ��ݒ��t�@�C���̃t�@�C����
    UInt16      reserved;              // (�\��)
    UInt16      bbsType;               // BBS�^�C�v
    UInt16      msgStatus;             // ���b�Z�[�W�̉�͏��
    EventType   event;                 // �_�~�[�̃C�x���g�p�f�[�^
    // NNshLogTokenDatabase *tokenInfo;   // ���O�̋�؂���f�[�^
    UInt32     *msgOffset;             //
    Char       *jumpListTitles;        //
    MemHandle   jumpListH;             //
    Char        baseURL[MAX_URL];      // �x�[�XURL
    NNshFileRef fileRef;
    UInt32      fileOffset;            // �Ǎ���ł���f�[�^�̐擪�����offset
    UInt32      fileReadSize;          // ���݃o�b�t�@�ɓǂݍ���ł���f�[�^��
    Char        showTitle  [MAX_THREADNAME]; // �X���^�C�g��(��ʕ\���p)
    Char        threadTitle[MAX_THREADNAME]; // �X������
    Char        threadFile [MAX_THREADFILENAME]; // �X��ID
    Char        boardNick  [MAX_NICKNAME];  // �{�[�h�j�b�N�l�[�� 
    Char        buffer     [MARGIN];        // �f�[�^�o�b�t�@(���ۂ͂����Ƃ���)
} NNshMessageIndex;

// �B�����b�Z�[�W�̃��[�N�̈�\����
typedef struct
{
    UInt16    nofWord;        // ������(���X�g)��
    Char     *wordString;     // ������̗̈�(���X�g�`��)
    MemHandle wordmemH;       // �����񃊃X�g�f�[�^
} NNshWordList;

// Field �𕪊������Ƃ��̗̈��`
typedef struct
{
  UInt16 startX;
  UInt16 startY;
  UInt16 endX;
  UInt16 endY;
} fieldRegion;

// �Q�ƃ��O�ꗗ�̕\�����X�g
typedef struct
{
    UInt16    dataCount;
    UInt16    recNum    [MAX_GETLOG_RECORDS];
    UInt32    updateTime[MAX_GETLOG_RECORDS];
    Char     *title     [MAX_GETLOG_RECORDS];
} NNshDispList;

//  NNsi�S�̂Ŏg�p����O���[�o���̈�
typedef struct
{
    MemHandle   searchTitleH;
    MemHandle   searchBBSH;
    MemHandle   boardIdxH;
    MemHandle   logPrefixH;
    MemHandle   bbsTitleH;
    UInt16      backFormId;
    UInt32      palmOSVersion;       // �f�o�C�X��OS�o�[�W����
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
    UInt16      notifyUpdate;        // Notify�C�x���g�̍X�V����
    UInt16      updateHR;            // SILK�T�C�Y�ύX�A��ʕ\�����X�V����
    UInt16      tempStatus;          //
#ifdef USE_CLIE
    UInt16      hrRef;               // SONY HR�Q�Ɣԍ�
    UInt16      hrVer;               // Sony HR�̃o�[�W�����ԍ�
    UInt16      notUseHR;            // HR������s��Ȃ�
    UInt16      notUseSilk;          // Silk������s��Ȃ�
    UInt16      silkRef;             // CLIE�pSILK�}�l�[�W���̎Q�Ɣԍ�(Temp)
    UInt16      silkVer;             // CLIE�pSILK�}�l�[�W����Version�ԍ�(Temp)
    UInt16     *jogInfoBack;         // SONY�W���O���o�b�N�A�b�v
    UInt16    **jogAssistSetBack;    // SONY�W���O�A�V�X�g�o�b�N�A�b�v
    UInt16      sonyJogMask[(NUMBER_OF_JOGMASKS * (2 + 1)) + MARGIN];
#endif // #ifdef USE_CLIE
#ifdef USE_COLOR
    RGBColorType fgColor;                // �O�i�F
    RGBColorType bgColor;                // �w�i�F
    RGBColorType txColor;                // �e�L�X�g�F
    RGBColorType bgFormColor;            // (�t�H�[��)�w�i�F
    RGBColorType bgDialogColor;          // (�_�C�A���O)�w�i�F
    RGBColorType bgAlertColor;           // (�A���[�g)�w�i�F
    RGBColorType bgObjectColor;          // (�I�u�W�F�N�g)�w�i�F
#endif  // #ifdef USE_COLOR
    UInt32             os5HighDensity;   // ���𑜓x�T�|�[�g�̗L��
    UInt16             jumpSelection;    // �W�����v�I��(+ �ꗗ��ʋ@�\���s�ԍ�)
    EventType          dummyEvent;       // �C�x���g�����p�o�b�t�@
    Char               dummyBuffer[MARGIN];
    FormType          *prevBusyForm;
    Char              *bbsTitles;

    UInt16             nofTitleItems;   // �X���^�C�g����
    Char              *msgListStrings;  // �X���^�C�g���f�[�^�i�[�̈�
    Char              **msgTitleIndex;  // (1�����Ƃ�)�X���^�C�g���擪
    UInt16            *msgListIndex;    // �^�C�g�����X�g���X�����R�[�h�ԍ��ϊ�
    UInt16            *msgTitleStatus;  // (1�����Ƃ�)�X���^�C�g�����
    UInt16            *msgNumbers;      // (1�����Ƃ�)���b�Z�[�W��

    UInt32            be2chLoginDateTime;    //  Be@2ch�̃��O�C�����s����
    Char              *be2chCookie;          //  Be@2ch�̃��O�C����Cookie

//    Char              cookieStoreBuffer[NNSH_WRITECOOKIE_BUFFER]; //  Cookie���L������ꏊ

    Char              *customizedUserAgent;  //  ���[�U�[�G�[�W�F���g��
    Char              *connectedSessionId;   //  2ch�p�Z�b�V����ID
    UInt32            loginDateTime;         //  ���O�C�����{����
//    Char              oysterPassword[OYSTER_LEN];  //  2ch�p�X���[�h

    NNshBBS_Temp_Info *tempInfo;
    NNshMessageIndex  *msgIndex;
    fieldRegion        fieldPos[2];       // �Q�Ɖ�ʂ̊e�̈���
    UInt32             tokenId;           // �Q�Ɖ�ʋ�؂�p�^�[��
    Char              *bookmarkFileName;  // ������ݒ莞�̃X��ID
    Char              *bookmarkNick;      // ������ݒ莞�̃X��Nick
    Char               restoreSetting;    // �ݒ�̕���
    MemHandle          listHandle;        // ���X�g�n���h��
    NNshWordList       hide1;             // NG�ݒ�P���X�g
    NNshWordList       hide2;             // NG�ݒ�Q���X�g
    NNshWordList      *featureList;       // �@�\�ݒ胉�x��
    UInt16             featureLockCnt;    // �@�\�ݒ胉�x���̃��b�N��
#ifdef USE_LOGCHARGE
    NNshDispList      *dispList;          // �Q�ƃ��O�ꗗ
    Char               getLogListUrlNameLabel[MAX_TITLE_LABEL];
#endif // #ifdef USE_LOGCHARGE
#ifdef USE_ADDR_NETLIB
    // NetLib���ɂ͐錾�Ȃ�
#else
    NetSocketAddrType  sockAddr;          // �ʐM��A�h���X
#endif
#ifdef USE_ZLIB
    UInt16             getRetry;          // �Ď擾
    UInt16             useGzip;           // GZIP���k���g�p����(������)
    void             *streamZ;           // zgip�X�g���[��
    Char              *inflateBuf;        // zlib�𓀐�o�b�t�@
#endif // #ifdef USE_ZLIB
    UInt16             work1;             // ���[�N�̈�(���̂P)
    UInt16             work2;             // ���[�N�̈�(���̂Q)
    UInt16             work3;             // ���[�N�̈�(���̂R)
    UInt32            *work4;             // ���[�N�̈�(���̂S)
    NNshSavedPref     *NNsiParam;         // NNsi�ݒ�p�f�[�^�x�[�X
} NNshWorkingInfo;

typedef struct {
    UInt8 importDB:1;                        // DB �𕜌����邩�ǂ���
    UInt8 deleteFile:1;                        // VFS �� DB���폜���邩�ǂ���
    UInt8 fileExist:1;                        // VFS �� DB �����邩�ǂ���
    UInt32 modDateVFS;                        // VFS �� DB �̍X�V����
    UInt32 modDateMEM;                        // MEM �� DB �̍X�V����
    Char dateStrMEM[24];
    Char dateStrVFS[24];
} NNshVFSDBInfo;

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

typedef struct {
  UInt16  popId;
  UInt16  lstId;
} NNshListItem;

typedef struct {
    Char   dataFileName[MAX_THREADFILENAME]; // �f�[�^�t�@�C����
    Char   dataTitlePrefix[MAX_DATAPREFIX];  // �f�[�^�w�b�_
    Char   boardName      [MAX_NICKNAME];    // �{�[�h�j�b�N�l�[��(���g�p) 
    Char   dataURL    [MAX_GETLOG_URL];      // �f�[�^�擾��URL
    Char   getDateTime[MAX_GETLOG_DATETIME]; // �X�V����
    Char   dataTitle  [MAX_TITLE_LABEL];     // �f�[�^����
    UInt8  wwwcMode;                         // ���O�擾���Ȃ�
    UInt8  rssData;                          // RSS�f�[�^
    UInt16 notUse0;                          // ���g�p�̈�(�p�~)
    UInt16 notUse1;                          // ���g�p�̈�(�p�~)
    UInt32 lastUpdate;                       // �ŏI�X�V����
    UInt16 id;                               // �f�[�^ID
    UInt8  state;                            // �Q�ƃ��O���
    UInt8  getAfterHours;                    // �Ď擾���鎞�ԊԊu
    UInt8  usable;                           // �擾����/���Ȃ�
    UInt8  getDepth;                         // �擾�K�w�w��
    UInt8  kanjiCode;                        // �����R�[�h
    UInt8  getSubData;                       // �u�P���擾�v�Ŏ擾�����f�[�^��
    UInt32 tokenId;                          // ��؂��ID
    UInt32 subTokenId;                       // �T�u��؂��ID
} NNshGetLogDatabase;

//
//  �K�w���ɂ���t�@�C���̎擾���
//
typedef struct {
    Char   dataURL     [MAX_GETLOG_URL];      // �f�[�^�擾��URL
    Char   dataFileName[MAX_THREADFILENAME];  // �f�[�^�t�@�C����
    UInt32 lastUpdate;                        // �ŏI�X�V����
    UInt32 parentDbId;                        // �e�̃��R�[�hID(���j�[�NID)
    UInt32 logDbId;                           // ���O�̃��R�[�hID(���j�[�NID)
    UInt32 tokenId;                           // ��؂蕶���̃��R�[�hID(���j�[�NID)
    UInt32 reserve0;                          // �\��̈�
    UInt32 reserve1;                          // �\��̈�
    UInt32 reserve2;                          // �\��̈�
    UInt32 reserve3;                          // �\��̈�
} NNshGetLogSubFileDatabase;


// �u�P�����擾�v�ŁA�t�q�k��͎��ɍ쐬����
//  �e���|�����f�[�^�x�[�X
typedef struct {
    Char  dataURL[MAX_GETLOG_URL + MARGIN];
    Char  titleName[MAX_THREADNAME + MARGIN];
} NNshGetLogTempURLDatabase;


typedef struct {
    Char   ngWord[NGWORD_LEN];
    UInt8  checkArea;
    UInt8  matchedAction;
    UInt16 reserved1;    
} NNshNGwordDatabase;

// UnicodeToJis0208 �e�[�u�����R�[�h
typedef struct
{
    UInt16 size;
    UInt16 table[256];
} UnicodeToJis0208Rec;

#ifdef USE_SSL
#ifdef USE_SSL_V2

/* RC4 */
typedef struct rc4_key_st
{
  Int32 x;
  Int32 y;
  Int32 data[256];
} RC4_KEY;

/* MD5 context. */
typedef struct {
  UInt32 state [4];                                   /* state (ABCD) */
  UInt32 count [2];        /* number of bits, modulo 2^64 (lsb first) */
  UInt8  buffer[64];                                  /* input buffer */
} MD5_CTX;

typedef struct {
    UInt16        state;
    UInt16        netRef;
    NetSocketRef  socketRef;
    UInt8         challenge[SSL2_CHALLENGE_LENGTH + MARGIN];
    UInt8         clientWriteKey[SSL2_RWKEY_LENGTH + MARGIN];
    UInt8         clientReadKey[SSL2_RWKEY_LENGTH + MARGIN];
    UInt8         clientMasterKey[SSL2_MASTERKEY_LENGTH + MARGIN];
    UInt8         PADDING[64];
    MD5_CTX       context;
    UInt8         digest[SSL2_MD5_HASH_LENGTH + MARGIN];
    UInt32        seqNum;
    RC4_KEY       workReadKey;
    RC4_KEY       workWriteKey;
    UInt8        *serverPublicKey;
    UInt16        serverPublicLen;
    UInt8        *connectionId;
    UInt16        connectionIdLen;
    UInt16        bufLen;
    Char         *buffer;
} NNshSSLv2Ref;
#endif
#endif

#ifdef USE_MACRO
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
#endif

// ping�p ICMP�p�P�b�g�\����
typedef struct {
  UInt8  icmp_type;
  UInt8  icmp_code;
  UInt16 icmp_cksum;
  UInt16 icmp_id;
  UInt16 icmp_seq;
  UInt8  icmp_data[32];
} ICMP_packet;

// IP�p�P�b�g�\����(PING�p)
typedef struct {
  UInt8       ip_ver;
  UInt8       ip_tos;
  UInt16      ip_len;  
  UInt16      ip_id;
  UInt16      ip_off;
  UInt8       ip_ttl;
  UInt8       ip_p;
  UInt16      ip_sum;
  NetIPAddr   ip_src;
  NetIPAddr   ip_dst;
  ICMP_packet icmp;
} PING_packet;

#endif
