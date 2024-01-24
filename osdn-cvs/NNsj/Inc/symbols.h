/*============================================================================*
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for NNsh.
 *
 *===========================================================================*/

//////////////////////////////////////////////////////////////////////////////
/***  DATABASE VERSIONS  ***/
#define SOFT_CREATOR_ID       'NNsj'
#define DATA_CREATOR_ID       'NNsi'
#define SOFT_DBTYPE_ID        'Data'
#define DATA_DBTYPE_ID        'Mcro'

#define SOFT_PREFID           100
#define SOFT_PREFVERSION      100

#define NNSH_PREFERID         100
#define NNSH_PREFERTYPE       100

#define DBNAME_MACROSCRIPT     "Macro-NNsi"
#define DBVERSION_MACROSCRIPT  100

//////////////////////////////////////////////////////////////////////////////

#define DBNAME_SETTING        "NNsj-Setting"
#define DBVERSION_SETTING     100
#define DBSIZE_SETTING        32

//////////////////////////////////////////////////////////////////////////////
//
#define NNSJ_NNSIEXT_SELECTMACRO  "<NNsi:NNsiExt type=\"SelectMacro\"/>"

/** buffer size(small) **/
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12
#define MAX_URL                     80
#define MAX_FILELIST               256

#define NNSH_BUSYWIN_MAXLEN        32

/** �ꎞ�m�ۗ̈�̕�(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4
#define FREE_SPACE_MARGIN         2048

/** ��Ԓl **/
#define NNSH_NOT_EFFECTIVE           0
#define NNSH_UPDATE_DISPLAY          1
#define NNSH_SILK_CONTROL            1

#define NNSH_DISABLE                 0
#define NNSH_ENABLE                  1


/** �f�o�C�X�� **/
#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002

// COPY type
#define NNSH_COPY_PALM_TO_VFS       0
#define NNSH_COPY_VFS_TO_VFS        1
#define NNSH_COPY_VFS_TO_PALM       2
#define NNSH_COPY_PALM_TO_PALM      4

#define NNSH_FILEMODE_CREATE        1
#define NNSH_FILEMODE_READWRITE     2
#define NNSH_FILEMODE_READONLY      4
#define NNSH_FILEMODE_TEMPFILE      8
#define NNSH_FILEMODE_APPEND        16

#define FILEMGR_STATE_DISABLED      0
#define FILEMGR_STATE_OPENED_VFS    1
#define FILEMGR_STATE_OPENED_STREAM 2


/** VFS�t�@�C�����̍ő�T�C�Y  **/
#define MAXLENGTH_FILENAME          256

// for FILE NAMEs
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define DATAFILE_SUFFIX             ".dat"

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont

/** �f�[�^�x�[�X�̃L�[�^�C�v(������������) **/
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2

/** HTTP���b�Z�[�W�쐬���Ɏw�� **/
#define HTTP_SENDTYPE_GET                10
#define HTTP_SENDTYPE_GET_NOTMONA        11
#define HTTP_SENDTYPE_POST               20
#define HTTP_SENDTYPE_POST_OTHERBBS      30
#define HTTP_SENDTYPE_POST_MACHIBBS      40
#define HTTP_SENDTYPE_POST_SHITARABAJBBS 80
#define HTTP_SENDTYPE_POST_SHITARABA     90
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETSUBJECT_START       0
#define HTTP_GETSUBJECT_PART        0

/** �������� **/
#define NNSH_SEARCH_FORWARD         1
#define NNSH_SEARCH_BACKWARD       -1

/** �t�H�[��ID(�����Ǘ��p) **/
#define NNSH_FRMID_MAIN             10
#define NNSH_FRMID_CONFIG_NNSH      20

/** �_�C�A���O�\�����x�� **/
#define NNSH_LEVEL_ERROR            0x0000
#define NNSH_LEVEL_WARN             0x0001
#define NNSH_LEVEL_INFO             0x0002
#define NNSH_LEVEL_CONFIRM          0x0004
#define NNSH_LEVEL_DEBUG            0x0010



// �_�C�A���O���ȗ�����ݒ�
#define NNSH_OMITDIALOG_NOTHING      0x00
#define NNSH_OMITDIALOG_CONFIRM      0x01
#define NNSH_OMITDIALOG_WARNING      0x02
#define NNSH_OMITDIALOG_INFORMATION  0x04
#define NNSH_OMITDIALOG_ALLOMIT      ((NNSH_OMITDIALOG_CONFIRM)|(NNSH_OMITDIALOG_WARNING)|(NNSH_OMITDIALOG_INFORMATION))

#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)

#define NNSH_NGSET_SEPARATOR        ','

// VFS���(�ύX)�l
#define NNSH_VFS_UNMOUNTED   1
#define NNSH_VFS_MOUNTED     2

#define NNSH_MODE_NORMAL            0
#define NNSH_MODE_INSERT            1
#define NNSH_MODE_DELETE            2

// ���C�����
#define NNSH_STEP_PAGEUP           -1
#define NNSH_STEP_PAGEDOWN          1
#define NNSH_STEP_REDRAW            0
#define NNSH_STEP_UPDATE           10
#define NNSH_STEP_TO_BOTTOM       100
#define NNSH_STEP_TO_TOP          NNSH_STEP_UPDATE

#define NNSH_DISP_NOTHING          0x0000   // �Ȃ�
#define NNSH_DISP_UPPERLIMIT       0x0001   // ���
#define NNSH_DISP_HALFWAY          0x0002   // �r��
#define NNSH_DISP_ALL              0x0004   // �S�\��
#define NNSH_DISP_LOWERLIMIT       0x0008   // ����


// ���M���O���x��
#define NNSH_MACROLOGLEVEL_FATAL    0
#define NNSH_MACROLOGLEVEL_ERROR    1
#define NNSH_MACROLOGLEVEL_WARN     2
#define NNSH_MACROLOGLEVEL_INFO     3
#define NNSH_MACROLOGLEVEL_DEBUG    9

// op�R�[�h(���\�[�X�ƈ�v����K�v����)
#define NNSH_MACROCMD_MACROEND 0   // �}�N���I��
#define NNSH_MACROCMD_MOVE     1   // ���
#define NNSH_MACROCMD_ADD      2   // ���Z
#define NNSH_MACROCMD_SUB      3   // ���Z
#define NNSH_MACROCMD_MESSAGE  4   // ���O�o��
#define NNSH_MACROCMD_EXECUTE  5   // �@�\�̎��s
#define NNSH_MACROCMD_CMP      6   // ���l�̔�r
#define NNSH_MACROCMD_JMP      7   // �������W�����v
#define NNSH_MACROCMD_JZ       8   // ��v�����ꍇ�W�����v
#define NNSH_MACROCMD_JNZ      9   // ��v���Ȃ������ꍇ�W�����v
#define NNSH_MACROCMD_BEEP    10   // �r�[�v����炷
#define NNSH_MACROCMD_OPERATE 11   // NNsi�ݒ�̒l�ݒ�
#define NNSH_MACROCMD_BBSTYPE 12   // BBS�^�C�v����
#define NNSH_MACROCMD_STRSTR  13   // �����񂪊܂܂�邩�ǂ���
#define NNSH_MACROCMD_STRCMP  14   // �����񂪈�v���邩�ǂ���
#define NNSH_MACROCMD_SETTABNUM 15 // �^�u�Ɋ܂܂�鐔����
#define NNSH_MACROCMD_GETMESNUM 16 // �X���Ɋ܂܂�郁�b�Z�[�W������
#define NNSH_MACROCMD_MESSTATUS 17 // �X����Ԃ���
#define NNSH_MACROCMD_AND     18   // �_����
#define NNSH_MACROCMD_OR      19   // �_���a
#define NNSH_MACROCMD_XOR     20   // �r���I�_���a
#define NNSH_MACROCMD_IMPORT  21   // NNsi�ݒ�C���|�[�g
#define NNSH_MACROCMD_EXPORT  22   // NNsi�ݒ�G�N�X�|�[�g
#define NNSH_MACROCMD_STATUS  23   // NNsi�ݒ�̒l�擾
#define NNSH_MACROCMD_LAUNCHDA 24  // DA�N��
#define NNSH_MACROCMD_CLIPCOPY  25  // �N���b�v�{�[�h�ɃR�s�[
#define NNSH_MACROCMD_CLIPADD   26  // �N���b�v�{�[�h�ɒǉ�
#define NNSH_MACROCMD_CLIPINSTR 27  // �N���b�v�{�[�h�̕�����Ɋ܂܂�邩�m�F
#define NNSH_MACROCMD_TITLECLIP 28  // �X���^�C�g�����N���b�v�{�[�h�ɃR�s�[
#define NNSH_MACROCMD_SET_FINDBBSSTR 29 // �^�u�����p������̐ݒ�
#define NNSH_MACROCMD_SET_FINDTHREADSTR  30 // �X���^�C�����p������̐ݒ�
#define NNSH_MACROCMD_UPDATE_RESNUM  31 // �X���̃��X�ԍ����X�V����
#define NNSH_MACROCMD_JPL       32   // �l�����̏ꍇ�ɂ̓W�����v(1.03)
#define NNSH_MACROCMD_JMI       33   // �l�����̏ꍇ�ɂ̓W�����v(1.03)
#define NNSH_MACROCMD_OUTPUTTITLE 34 // �X���^�C�g�������O�o��(1.03)
#define NNSH_MACROCMD_OUTPUTCLIP 35  // �N���b�v�{�[�h�̓��e�����O�o��(1.03)
#define NNSH_MACROCMD_REMARK    36   // ���ߕ�(�������Ȃ�) (1.03)
#define NNSH_MACROCMD_PUSH      37   // �X�^�b�N��PUSH(1.04)
#define NNSH_MACROCMD_POP       38   // �X�^�b�N��POP(1.04)
#define NNSH_MACROCMD_GETRECNUM 39   // ���R�[�h�ԍ����擾(1.04)
#define NNSH_MACROCMD_FINDTHREADSTRCOPY 40 // �X��������������N���b�v�{�[�h��(1.04)
#define NNSH_MACROCMD_INPUTDIALOG   41     // ���������́A�N���b�v�{�[�h�փR�s�[ (1.05)
#define NNSH_MACROCMD_OUTPUTDIALOG  42     // ��������o�͂��� (1.05)
#define NNSH_MACROCMD_CONFIRMDIALOG 43     // �m�F�_�C�A���O��\������ (1.05)
#define NNSH_MACROCMD_MESATTRIBUTE  44     // �X���̑������� (1.06)
#define NNSH_MACROCMD_JUMPLAST      45     // �X���̍ŉ��s�̏ꍇ�ɂ̓W�����v���� (1.06)


// �f�[�^�̑���֘A�w��(NNsi�ݒ�̕ύX�Ƃ�)
#define NNSH_MACROOPCMD_DISABLE            0xffff  // (�}�N������Ȃ�)
#define NNSH_MACROOPCMD_AUTOSTARTMACRO     0xfffe  // (�}�N���������sSET)
#define NNSH_MACROOPCMD_NEWARRIVAL_LINE    0xfffd  // (�V���m�F��̉���ؒf)
#define NNSH_MACROOPCMD_NEWARRIVAL_DA      0xfffc  // (�V���m�F���DA�N��)
#define NNSH_MACROOPCMD_NEWARRIVAL_ROLOG   0xfffb  // (�Q�Ɛ�p���O�擾���x��)
#define NNSH_MACROOPCMD_NEWARRIVAL_BEEP    0xfffa  // (�V���m�F��̃r�[�v��)
#define NNSH_MACROOPCMD_COPY_AND_DELETE    0xfff9  // (�Q�ƃ��O�ɃR�s�[���폜��)

#define NNSH_MACROCMD_VIEWTITLE  "�}�N�����e"
#define NNSH_MACROLOG_VIEWTITLE  "�}�N�����s����"

#define MAX_MACRO_STEP         256   // �ҏW�\�ȃ}�N���T�C�Y�̍ő�

#define MAX_HALT_OPERATION     100   // �@�\���s�̐���
#define MAX_LOGVIEW_BUFFER      96   //
#define MAX_LOGOUTPUT_BUFFER    80   //
#define MAX_FUNCCHAR_BUF        20   //
#define TEXTBUFSIZE            256

#define MAX_LOGRECLEN               40
#define MAX_STRLEN                  32

#define FILE_XML_NNSISET            "NNsiSet.xml"

// �}�N���̃��W�X�^�֘A
#define NOF_REGISTER             16  // ���W�X�^�̐�
#define NNSH_MACROOP_NOSPECIFY 0x00  // �w��Ȃ�
#define NNSH_MACROOP_IMMEDIATE 0x01  // ���l�w��

#define NNSH_MACROOP_GR_OFFSET 0xe0  // ���W�X�^�I�t�Z�b�g
#define NNSH_MACROOP_GR0       0xe0  // GR0
#define NNSH_MACROOP_GR1       0xe1  // GR1
#define NNSH_MACROOP_GR2       0xe2  // GR2
#define NNSH_MACROOP_GR3       0xe3  // GR3
#define NNSH_MACROOP_GR4       0xe4  // GR4
#define NNSH_MACROOP_GR5       0xe5  // GR5
#define NNSH_MACROOP_GR6       0xe6  // GR6
#define NNSH_MACROOP_GR7       0xe7  // GR7
#define NNSH_MACROOP_GR8       0xe8  // GR8
#define NNSH_MACROOP_GR9       0xe9  // GR9
#define NNSH_MACROOP_GRa       0xea  // GRa
#define NNSH_MACROOP_GRb       0xeb  // GRb
#define NNSH_MACROOP_GRc       0xec  // GRc
#define NNSH_MACROOP_GRd       0xed  // GRd
#define NNSH_MACROOP_GRe       0xee  // GRe
#define NNSH_MACROOP_GRf       0xef  // GRf
#define NNSH_MACROOP_GR_MAX    0xef  // ���W�X�^�ő�l
#define NNSH_MACROOP_BUF0      0xf0  // �e�L�X�g�o�b�t�@
