/*============================================================================*
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for NNsh.
 *
 *===========================================================================*/
///// NNsiMona��p��` { /////
#define DBSIZE_BBSLIST    140
#define DBSIZE_SUBJECT    136
#define DBSIZE_DIRINDEX    68
#define DBSIZE_MARGIN       4
///// } NNsiMona��p��` /////
//////////////////////////////////////////////////////////////////////////////
/***  DATABASE VERSIONS  ***/
#define SOFT_PREFID           100
#define SOFT_PREFVERSION      204

#define NNSH_PREFERID         200
#define NNSH_PREFERTYPE       105

#define SOFT_DBTYPE_ID        'Data'
#define OFFLINE_DBTYPE_ID     'GIKO'

#define DBNAME_BBSLIST        "BBS-NNsi"
#define DBVERSION_BBSLIST     160

#define DBNAME_SUBJECT        "Subject-NNsi"
#define DBVERSION_SUBJECT     160

#define DBNAME_SETTING        "Settings-NNsi"
#define DBVERSION_SETTING     111
#define DBSIZE_SETTING        32

#define DBNAME_DIRINDEX       "DirIndex-NNsi"
#define DBVERSION_DIRINDEX    100

//////////////////////////////////////////////////////////////////////////////

#define RSCID_createThreadIndexSub_ARM     1     // ARMlet�֐��ԍ����̂P
#define RSCID_parseMessage_ARM             2     // ARMlet�֐��ԍ����̂Q

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define TINYBUF                      8
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12
#define JUMPBUF                     16

/** �ꎞ�m�ۗ̈�̕�(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4

/** �������ݎ��Ɋm�ۂ���o�b�t�@�T�C�Y **/
#define NNSH_WRITEBUF_MIN         4096
#define NNSH_WRITEBUF_MAX        61000
#define NNSH_WRITEBUF_DEFAULT     6144

/** BBS-DB�̕�����T�C�Y **/
#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16
#define LIMIT_BOARDNAME             20

/** Thread-DB �̕�����T�C�Y **/
#define MAX_THREADFILENAME          32
#define MAX_THREADNAME              72
#define MAX_DIRNAME                 40
#define LIMIT_TITLENAME_DEFAULT     72
#define LIMIT_TITLENAME_MINIMUM     12

/** �^�C�g������у��x���̗̈�T�C�Y **/
#define BBSSEL_PAGEINFO             12
#define NNSH_MSGTITLE_BUFSIZE       10

/** VFS�t�@�C�����̍ő�T�C�Y  **/
#define MAXLENGTH_FILENAME          256

/** �����_�E�����[�h���ɑz�肷��HTTP�w�b�_�T�C�Y **/
#define NNSH_HTTPGET_HEADERMAX      400
#define NNSH_DOWNLOADSIZE_MIN      2048

/** �ʐM�^�C���A�E�g�����l(tick) **/
#define NNSH_NET_TIMEOUT           2500

/** ���b�Z�[�W�̍ő僌�X�ԍ� **/
#define NNSH_MESSAGE_MAXNUMBER     1005
#define NNSH_MESSAGE_LIMIT         1000

/** BBS URL **/
#define URL_BBSTABLE_OLD         "http://www6.ocn.ne.jp/~mirv/bbstable.html"
#define URL_BBSTABLE             "http://www.ff.iij4u.or.jp/~ch2/bbstable.html"

#define URL_PREFIX                     "test/read.cgi/"
#define URL_PREFIX_IMODE               "test/r.i/"
#define URL_PREFIX_MACHIBBS            "bbs/read.pl?BBS="
#define URL_PREFIX_MACHIBBS_IMODE      "bbs/read.pl?IMODE=TRUE&BBS="
#define URL_PREFIX_SHITARABA           "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABA_IMODE     "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABAJBBS       "bbs/read.cgi?BBS="
#define URL_PREFIX_SHITARABAJBBS_IMODE "bbs/i.cgi?BBS="
#define FILE_THREADLIST                "subject.txt"

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont

#define NNSH_NOTENTRY_THREAD        0xffff
#define NNSH_NOTENTRY_BBS           0xffff

/** OFFLINE LOG(for GIKO Shippo) **/
#define OFFLINE_THREAD_NAME         "�Q�Ɛ�p���O"
#define OFFLINE_THREAD_NICK         "!GikoShippo/"
#define OFFLINE_THREAD_URL          "file://"

#define FAVORITE_THREAD_NAME        "���C�ɓ���"
#define GETALL_THREAD_NAME          "�擾�ςݑS��"
#define NOTREAD_THREAD_NAME         "���ǂ���"
#define CUSTOM1_THREAD_NAME         "�擾�G���["
#define CUSTOM2_THREAD_NAME         "Palm�֘A"

#define NNSH_NOF_SPECIAL_BBS        6   // ����Ȕ��̐�
#define NNSH_SELBBS_FAVORITE        0   // ���C�ɓ���X��
#define NNSH_SELBBS_GETALL          1   // �擾�ςݑS��
#define NNSH_SELBBS_NOTREAD         2   // ���ǂ���
#define NNSH_SELBBS_CUSTOM1         3   // CUSTOM1
#define NNSH_SELBBS_CUSTOM2         4   // CUSTOM2
#define NNSH_SELBBS_OFFLINE         5   // OFFLINE�X��

// for NaNaShi FILE NAMEs
#define FILE_WRITEMSG               "write.txt"
#define FILE_WRITENAME              "writeName.txt"
#define FILE_WRITEMAIL              "writeMail.txt"
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define LOGDATAFILE_PREFIX          "/PALM/Programs/2ch/"
#define LOGDIR_FILE                 "/PALM/Programs/2ch/logdir.txt"
#define DATAFILE_SUFFIX             ".dat"

//  time diference between from 'Jan/01/1904' to 'Jan/01/1970'.
#define TIME_CONVERT_1904_1970      (2082844800 - 32400)
#define TIME_CONVERT_1904_1970_UTC  (2082844800)

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002
#define NNSH_VFS_USEOFFLINE         0x0004
#define NNSH_VFS_DBBACKUP           0x0008
#define NNSH_VFS_DBIMPORT           0x0010

#define NNSH_FILEMODE_CREATE        1
#define NNSH_FILEMODE_READWRITE     2
#define NNSH_FILEMODE_READONLY      4
#define NNSH_FILEMODE_TEMPFILE      8
#define NNSH_FILEMODE_APPEND        16

#define FILEMGR_STATE_DISABLED      0
#define FILEMGR_STATE_OPENED_VFS    1
#define FILEMGR_STATE_OPENED_STREAM 2

/** BBS-DB�̕\�����(�I����ʂŕ\�����邩/���Ȃ���) **/
#define NNSH_BBSSTATE_VISIBLE       1            // �\��
#define NNSH_BBSSTATE_INVISIBLE     0            // ��\��

/** BBS-DB�̃{�[�h�^�C�v **/
#define NNSH_BBSTYPE_2ch            0x00         // 2ch(�ʏ탂�[�h)
#define NNSH_BBSTYPE_MACHIBBS       0x01         // �܂�BBS(���ꃂ�[�h)
#define NNSH_BBSTYPE_SHITARABA      0x02         // �������BBS
#define NNSH_BBSTYPE_SHITARABAJBBS  0x03         // �������@JBBS
#define NNSH_BBSTYPE_ERROR          0xff         // �{�[�h�^�C�v�G���[

/** �ꗗ�\����BBS�ꗗ�����C�ɓ��肩 **/
#define NNSH_FAVORLIST_MODE         1
#define NNSH_BBSLIST_MODE           0 


// �f�[�^�x�[�X�̃L�[�^�C�v(������������)
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2
#define NNSH_KEYTYPE_SUBJECT        3

// HTTP���b�Z�[�W�쐬���Ɏw��
#define HTTP_SENDTYPE_GET                10
#define HTTP_SENDTYPE_GET_NOTMONA        11
#define HTTP_SENDTYPE_POST               20
#define HTTP_SENDTYPE_POST_MACHIBBS      40
#define HTTP_SENDTYPE_POST_SHITARABAJBBS 80
#define HTTP_SENDTYPE_POST_SHITARABA     90
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETSUBJECT_START       0
#define HTTP_GETSUBJECT_PART        0

// ��������
#define NNSH_SEARCH_ALL             0
#define NNSH_SEARCH_FORWARD         1
#define NNSH_SEARCH_BACKWARD       -1

// �������݃��b�Z�[�W�m�F�R�}���h
#define NNSH_WRITECHK_SPID          0
#define NNSH_WRITECHK_REPLY         1

// ���b�Z�[�W�̑���
#define NNSH_MSGATTR_NOTSPECIFY    0xff   // �u���C�ɓ���v�w��͎w�肵�Ȃ�
#define NNSH_MSGATTR_NOTFAVOR      0x4f   // �u���C�ɓ���v�X���łȂ�(MASK�l)
#define NNSH_MSGATTR_FAVOR_L1      0x10   // �u���C�ɓ���v���x��(��)
#define NNSH_MSGATTR_FAVOR_L2      0x20   // �u���C�ɓ���v���x��(����)
#define NNSH_MSGATTR_FAVOR_L3      0x30   // �u���C�ɓ���v���x��(��)
#define NNSH_MSGATTR_FAVOR_L4      0x80   // �u���C�ɓ���v���x��(����)
#define NNSH_MSGATTR_FAVOR         0xb0   // �u���C�ɓ���v�X��(��)
#define NNSH_MSGATTR_NOTERROR      0xbf   // �u�G���[�����v�X���łȂ�(MASK�l)
#define NNSH_MSGATTR_ERROR         0x40   // �u�G���[�����v�X���ł���
#define NNSH_MSGATTR_GETRESERVE    0x08   // �u�擾�\��v�X���ł���
#define NNSH_MSGATTR_NOTRESERVE    0xf7   // �u�擾�\��v�X�����N���A����(MASK)

#define NNSH_ATTRLABEL_TITLE       ", Level:" 
#define NNSH_ATTRLABEL_NOTFAVOR    "-"    // �u���C�ɓ���v�ł͂Ȃ�
#define NNSH_ATTRLABEL_FAVOR_L1    "L"    // �u���C�ɓ���v���x��(��)
#define NNSH_ATTRLABEL_FAVOR_L2    "1"    // �u���C�ɓ���v���x��(����)
#define NNSH_ATTRLABEL_FAVOR_L3    "2"    // �u���C�ɓ���v���x��(��)
#define NNSH_ATTRLABEL_FAVOR_L4    "3"    // �u���C�ɓ���v���x��(����)
#define NNSH_ATTRLABEL_FAVOR       "H"    // �u���C�ɓ���v���x��(��)


// �ꎞ���(���݁A�W���O�����񂵏�Ԃ��L������)
#define NNSH_TEMPTYPE_PUSHEDJOGUP   16
#define NNSH_TEMPTYPE_PUSHEDJOGDOWN  8
#define NNSH_TEMPTYPE_CLEARSTATE     0

//  ���̑��̏�Ԓl
#define NNSH_NOT_EFFECTIVE          0
#define NNSH_UPDATE_DISPLAY         1
#define NNSH_SILK_CONTROL           1

#define NNSH_DISABLE                0
#define NNSH_ENABLE                 1

// NNsi�̃f�[�^�x�[�X�X�V���
#define NNSH_DB_UPDATE_BBS          0x0001
#define NNSH_DB_UPDATE_SUBJECT      0x0002
#define NNSH_DB_UPDATE_SETTING      0x0004
#define NNSH_DB_UPDATE_DIRINDEX     0x0008

#define NNSH_DB_MASK_BBS            0xfffe
#define NNSH_DB_MASK_SUBJECT        0xfffd
#define NNSH_DB_MASK_SETTING        0xfffb
#define NNSH_DB_MASK_DIRINDEX       0xfff7

//////////////////////////////////////////////////////////////////////////////

/********** HTML��ԉ�͗p ********/
#define HTTP_ANCHORSTATE_NORMAL     101
#define HTTP_ANCHORSTATE_ANCHOR     102
#define HTTP_ANCHORSTATE_LABEL      103

/**********  �X���ǂݏo����ԗp  *********/
#define NNSH_SUBJSTATUS_NOT_YET      0
#define NNSH_SUBJSTATUS_NEW          1
#define NNSH_SUBJSTATUS_UPDATE       2
#define NNSH_SUBJSTATUS_REMAIN       3
#define NNSH_SUBJSTATUS_ALREADY      4
#define NNSH_SUBJSTATUS_DELETE       5
#define NNSH_SUBJSTATUS_UNKNOWN      6
#define NNSH_SUBJSTATUS_GETERROR     7
#define NNSH_SUBJSTATUS_OVER         8

#define NNSH_MSGSTATE_NOTMODIFIED   10
#define NNSH_MSGSTATE_ERROR         20

#define NNSH_ERRORCODE_PARTGET      (~errNone - 12)

/**********  ���b�Z�[�W�p�[�X�p  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/**********  BBS���L�X����  **************/
#define NNSH_BBSLIST_AGAIN         -1            // �X�������ēx�m�F����

///////////////////////////////////////////////////////////////////////////////

/*** DATA TOKENS ***/
#define DATA_SEPARATOR              "<>"
#define DATA_NEWLINE                "<br>"

/** for BBS NAME PARSER **/
#define HTTP_TOKEN_ST_ANCHOR        "<A"
#define HTTP_TOKEN_ED_ANCHOR        "</A>"
#define HTTP_TOKEN_END              ">"


#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01

#define NNSH_FIELD_SEPARATOR        "<>"
#define NNSH_FIELD_SEPARATOR_MACHI  ","
#define NNSH_RECORD_SEPARATOR       "\x0a"
#define NNSH_SUBJTITLE_END          "("

#define NNSH_JBBSTOKEN_START        "\x0a<dt>"
#define NNSH_JBBSTOKEN_END          "<br><br>\x0a"
#define NNSH_JBBSTOKEN_MSGSTART     "\xcc\xbe\xc1\xb0\xa1\xa7" // ���O�F
#define NNSH_MACHITOKEN_START       "\x0d\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a\x0d\x0a"
#define NNSH_MACHITOKEN_MSGSTART    "���O�F"
#define NNSH_MACHINAME_SUFFIX       "[�܂�BBS]"
#define NNSH_SHITARABA_SUFFIX       "[�������]"
#define NNSH_SHITARABAJBBS_SUFFIX   "[�������@JBBS]"

/** **/
#define NNSH_SYMBOL_WRITE_DONE      "�������݂܂����B"
#define NNSH_SYMBOL_WRITE_NG        "�d�q�q�n�q"

#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1

#define NNSH_JUMPMSG_LEN            10
#define NNSH_JUMPMSG_HEAD           ">>"

#define NNSH_JUMPMSG_BACK           "�P�߂�"
#define NNSH_JUMPMSG_TO_LIST        "�ꗗ��"
#define NNSH_JUMPMSG_OPEN_MENU      "���j���["
#define NNSH_JUMPMSG_UPDATE         "�����擾"
#define NNSH_JUMPMSG_HANGUP         "����ؒf"
#define NNSH_JUMPMSG_TOP            "�擪��"
#define NNSH_JUMPMSG_BOTTOM         "������"
#define NNSH_JUMPMSG_OPENWEB        "�S�I�����Web"
#define NNSH_JUMPMSG_CHANGEVIEW     "�`�惂�[�h�ύX"
#define NNSH_JUMPMSG_FAVORITE       "�X�����"
#define NNSH_JUMPMSG_OUTPUTMEMO     "��������"
#define NNSH_JUMPMSG_BOOKMARK       "������ݒ�"

#define NNSH_NOF_JUMPMSG_EXT        12
#define NNSH_JUMPSEL_BACK           0x8001
#define NNSH_JUMPSEL_TO_LIST        0x8002
#define NNSH_JUMPSEL_OPEN_MENU      0x8003
#define NNSH_JUMPSEL_UPDATE         0x8004
#define NNSH_JUMPSEL_HANGUP         0x8005
#define NNSH_JUMPSEL_TOP            0x8006
#define NNSH_JUMPSEL_BOTTOM         0x8007
#define NNSH_JUMPSEL_OPENWEB        0x8008
#define NNSH_JUMPSEL_CHANGEVIEW     0x8009
#define NNSH_JUMPSEL_FAVORITE       0x800a
#define NNSH_JUMPSEL_OUTPUTMEMO     0x800b
#define NNSH_JUMPSEL_BOOKMARK       0x800c

#define NNSH_JUMPMSG_OPENBBS        "��I��"
#define NNSH_JUMPMSG_OPENMENU       "���j���["
#define NNSH_JUMPMSG_OPENNEW        "�V���m�F"
#define NNSH_JUMPMSG_OPENLIST       "�ꗗ�擾"
#define NNSH_JUMPMSG_OPENGET        "�X���擾"
#define NNSH_JUMPMSG_OPENINFO       "�X�����"
#define NNSH_JUMPMSG_OPENMES        "�X���Q��"
#define NNSH_JUMPMSG_DISCONNECT     "����ؒf"
#define NNSH_JUMPMSG_COPYMSG        "�Q�ƂփR�s�["
#define NNSH_JUMPMSG_DELMSG         "�X���폜"
#define NNSH_JUMPMSG_GRAPHMODE      "�`�惂�[�h�ύX"
#define NNSH_JUMPMSG_MULTISW1       "���ړISW1"
#define NNSH_JUMPMSG_MULTISW2       "���ړISW2"
#define NNSH_JUMPMSG_NNSIEND        "NNsi�I��"
#define NNSH_ITEMLEN_JUMPLIST       14      // ���X�g�A�C�e��������
#define NNSH_JUMPSEL_NUMLIST        14      // ���X�g�A�C�e����
#define NNSH_JUMPSEL_OPENBBS        0x9000  // ��I��
#define NNSH_JUMPSEL_OPENMENU       0x9001  // ���j���[
#define NNSH_JUMPSEL_DISCONNECT     0x9002  // ����ؒf
#define NNSH_JUMPSEL_OPENNEW        0x9003  // �V���m�F
#define NNSH_JUMPSEL_OPENLIST       0x9004  // �ꗗ�擾
#define NNSH_JUMPSEL_OPENGET        0x9005  // �X���擾
#define NNSH_JUMPSEL_OPENMES        0x9006  // �X���Q��
#define NNSH_JUMPSEL_OPENINFO       0x9007  // �X�����
#define NNSH_JUMPSEL_COPYMSG        0x9008  // �Q�ƂփR�s�[
#define NNSH_JUMPSEL_DELMSG         0x9009  // �X���폜
#define NNSH_JUMPSEL_GRAPHMODE      0x900a  // �`�惂�[�h�ύX
#define NNSH_JUMPSEL_MULTISW1       0x900b  // ���ړISW1
#define NNSH_JUMPSEL_MULTISW2       0x900c  // ���ړISW2
#define NNSH_JUMPSEL_NNSIEND        0x900d  // NNsi�I��

#define NNSH_STEP_PAGEUP           -1
#define NNSH_STEP_PAGEDOWN          1
#define NNSH_STEP_REDRAW            0
#define NNSH_STEP_UPDATE           10

#define NNSH_ITEM_LASTITEM         0xffff

#define NNSH_DISP_NOTHING          0x0000   // �Ȃ�
#define NNSH_DISP_UPPERLIMIT       0x0001   // ���
#define NNSH_DISP_HALFWAY          0x0002   // �r��
#define NNSH_DISP_ALL              0x0004   // �S�\��
#define NNSH_DISP_LOWERLIMIT       0x0008   // ����

#define NNSH_DISPMSG_NOTHING       "�Ȃ�"
#define NNSH_DISPMSG_UPPERLIMIT    "���"
#define NNSH_DISPMSG_HALFWAY       "�r��"
#define NNSH_DISPMSG_ALL           "�S�\��"
#define NNSH_DISPMSG_LOWERLIMIT    "����"
#define NNSH_DISPMSG_UNKNOWN       "?????"

#define NNSH_MSGNUM_UNKNOWN        0xffff
#define NNSH_MSGNUM_NOTDISPLAY     0x0000

#define NNSH_LIMIT_COPYBUFFER      2047
#define NNSH_LIMIT_MEMOPAD         4000
#define NNSH_LIMIT_RETRY           5
#define NNSH_MEMOBUFFER_MERGIN     96

#define NNSH_BUSYWIN_MAXLEN        32

#define NNSH_WRITE_DELAYTIME       7

#define NNSH_INPUTWIN_SEARCH       "TITLE����"
#define NNSH_INPUTWIN_GETMESNUM    "�X���擾(�X���Ԏw��)"
#define NNSH_INPUTWIN_MODIFYTITLE  "�X���^�C�g���ύX"
#define NNSH_INPUTWIN_NGWORD1      "NG������P�̓o�^"
#define NNSH_INPUTWIN_NGWORD2      "NG������Q�̓o�^"

#define NNSH_DIALOG_USE_SEARCH      10
#define NNSH_DIALOG_USE_GETMESNUM   20
#define NNSH_DIALOG_USE_MODIFYTITLE 40
#define NNSH_DIALOG_USE_SEARCH_MESSAGE 60
#define NNSH_DIALOG_USE_SETNGWORD   80



// ���ړI�{�^���֘A�̐ݒ�

#define NNSH_SWITCHUSAGE_FORMMASK   0x3f00
#define NNSH_SWITCHUSAGE_FUNCMASK   0x00ff

#define NNSH_SWITCHUSAGE_TITLE      0x2100

#define NNSH_SWITCHUSAGE_NOUSE      0x00  // ���ړI�X�C�b�`���g�p���Ȃ�
#define NNSH_SWITCHUSAGE_GETPART    0x01  // �X���擾���[�h(�ꊇ/����)
#define NNSH_SWITCHUSAGE_HANZEN     0x02  // ���p�J�i���S�p�J�i�ϊ�
#define NNSH_SWITCHUSAGE_USEPROXY   0x03  // Proxy�o�R�ŃA�N�Z�X
#define NNSH_SWITCHUSAGE_CONFIRM    0x04  // �m�F�_�C�A���O�̏ȗ�

#define NNSH_SWITCHUSAGE_SUBJECT    0x05  // �X���ꗗ�S�擾�؂�ւ�  (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_TITLEDISP  0x06  // �X���ꗗ�`�惂�[�h�ؑ�  (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_MSGNUMBER  0x07  // �X���ꗗ��MSG�ԍ�       (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_GETRESERVE 0x08  // �X���擾�ۗ����[�h      (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_IMODEURL   0x09  // i-mode URL�g�p���[�h    (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_BLOCKDISP  0x0a  // �]�[���\�����[�h        (�ꗗ��ʗp)

// ���ړI�{�^���̏����ݒ�l
#define NNSH_SWITCH1USAGE_DEFAULT   NNSH_SWITCHUSAGE_GETPART    // �X���擾���[�h
#define NNSH_SWITCH2USAGE_DEFAULT   NNSH_SWITCHUSAGE_USEPROXY   // Proxy�o�R�ł̃A�N�Z�X

#define NNSH_SWITCHMSG_NOUSE        "(�g�p���Ȃ�)"   // ���ړI�X�C�b�`���g�p���Ȃ�
#define NNSH_SWITCHMSG_GETPART      "����/�ꊇ"      // �X���擾���[�h(�ꊇ/����)
#define NNSH_SWITCHMSG_HANZEN       "���p�J�i�ϊ�"   // ���p�J�i���S�p�J�i�ϊ�
#define NNSH_SWITCHMSG_USEPROXY     "Proxy�g�p"      // Proxy�o�R�ŃA�N�Z�X
#define NNSH_SWITCHMSG_CONFIRM      "�m�F���ȗ�"     // �m�F�_�C�A���O�̏ȗ�

#define NNSH_SWITCHMSG_SUBJECT      "�ꗗ�S�擾"     // �X���ꗗ�S�擾�؂�ւ�
#define NNSH_SWITCHMSG_TITLEDISP    "�`�惂�[�h"     // �X���ꗗ�`�惂�[�h�ؑ�
#define NNSH_SWITCHMSG_MSGNUMBER    "�ꗗ�ɔԍ�"     // �X���ꗗ��MSG�ԍ�
#define NNSH_SWITCHMSG_GETRESERVE   "�擾�ۗ�"       // �X���擾�ۗ�
#define NNSH_SWITCHMSG_IMODEURL     "i-mode URL"     // i-mode URL�g�p
#define NNSH_SWITCHMSG_BLOCKDISP    "�]�[���\��"     // �]�[���\�����[�h

#define NNSH_SUBJSTATUSMSG_NOT_YET  "���擾"
#define NNSH_SUBJSTATUSMSG_NEW      "�V�K�擾"
#define NNSH_SUBJSTATUSMSG_UPDATE   "�X�V�擾"
#define NNSH_SUBJSTATUSMSG_REMAIN   "���ǂ���"
#define NNSH_SUBJSTATUSMSG_ALREADY  "����"
#define NNSH_SUBJSTATUSMSG_UNKNOWN  "��ԕs��"

/** �X�������� **/
#define MAX_SEARCH_STRING 64

/** �X�����������@ **/
#define NNSH_SEARCH_METHOD_FROM_TOP    0 //�擪���猟��
#define NNSH_SEARCH_METHOD_NEXT        1 //������
#define NNSH_SEARCH_METHOD_PREV        2 //�O����
#define NNSH_SEARCH_METHOD_FROM_BOTTOM 3 //�������猟��

/** ���b�Z�[�W�擾���@ **/
#define GET_MESSAGE_TYPE_TOP    1 // ���������ւ̌����p
#define GET_MESSAGE_TYPE_HALF   2 // �X���Q�Ɨp
#define GET_MESSAGE_TYPE_BOTTOM 3 // �擪�����ւ̌����p


// FORM ID LIST
#define NNSH_FRMID_THREAD        0x1200
#define NNSH_FRMID_MESSAGE       0x1201


// ���ړI�{�^���̐ݒ�
#define MAX_CAPTION                  6
#define LIMIT_CAPTION                4

#define MULTIBTN1_CAPTION_DEFAULT    "�Q��"
#define MULTIBTN1_FEATURE_DEFAULT    MULTIBTN_FEATURE_OPEN

#define MULTIBTN2_CAPTION_DEFAULT    "�擾"
#define MULTIBTN2_FEATURE_DEFAULT    MULTIBTN_FEATURE_GET

#define MULTIBTN3_CAPTION_DEFAULT    "�폜"
#define MULTIBTN3_FEATURE_DEFAULT    MULTIBTN_FEATURE_DELETE

#define MULTIBTN4_CAPTION_DEFAULT    "�ؒf"
#define MULTIBTN4_FEATURE_DEFAULT    MULTIBTN_FEATURE_DISCONN

#define MULTIBTN5_CAPTION_DEFAULT    "�ꗗ"
#define MULTIBTN5_FEATURE_DEFAULT    MULTIBTN_FEATURE_GETLIST

#define MULTIBTN6_CAPTION_DEFAULT    "�V��"
#define MULTIBTN6_FEATURE_DEFAULT    MULTIBTN_FEATURE_NEWARRIVAL

#define UPBTN_FEATURE_DEFAULT        MULTIBTN_FEATURE_SELECTPREV
#define DOWNBTN_FEATURE_DEFAULT      MULTIBTN_FEATURE_SELECTNEXT

#define JOGPUSH_FEATURE_DEFAULT      MULTIBTN_FEATURE_OPEN
#define JOGBACK_FEATURE_DEFAULT      MULTIBTN_FEATURE_INFO

#define MULTIBTN_FEATURE_MASK        0x00ff
#define MULTIBTN_FEATURE             0x7600

// ���̒�`�́A���\�[�X�̃��X�g(�̂�����Ă��鏇��)�ƂP�΂P�Ή����K�v�I
#define MULTIBTN_FEATURE_INFO        0  // "���"
#define MULTIBTN_FEATURE_OPEN        1  // "�Q��"
#define MULTIBTN_FEATURE_GET         2  // "�X���擾"
#define MULTIBTN_FEATURE_ALLGET      3  // "�V�K(��)�擾"
#define MULTIBTN_FEATURE_PARTGET     4  // "�����擾"
#define MULTIBTN_FEATURE_DELETE      5  // "�X���폜"
#define MULTIBTN_FEATURE_SORT        6  // "�X������"
#define MULTIBTN_FEATURE_MODTITLE    7  // "�X���^�C�ύX"
#define MULTIBTN_FEATURE_OPENWEB     8  // "�u���E�U�ŊJ��"
#define MULTIBTN_FEATURE_MESNUM      9  // "�X���Ԏw��擾"
#define MULTIBTN_FEATURE_SEARCH     10  // "�X���^�C����"
#define MULTIBTN_FEATURE_NEXT       11  // "������"
#define MULTIBTN_FEATURE_PREV       12  // "�O����"
#define MULTIBTN_FEATURE_TOP        13  // "�擪�ֈړ�"
#define MULTIBTN_FEATURE_BOTTOM     14  // "�����ֈړ�"
#define MULTIBTN_FEATURE_GETBBS     15  // "BBS�ꗗ�擾"
#define MULTIBTN_FEATURE_USEBBS     16  // "�g�pBBS�I��"
#define MULTIBTN_FEATURE_COPYGIKO   17  // "�Q�Ɛ�p��COPY"
#define MULTIBTN_FEATURE_DELNOTGET  18  // "���擾�ꗗ�폜"
#define MULTIBTN_FEATURE_DBCOPY     19  // "DB��VFS��COPY"
#define MULTIBTN_FEATURE_GRAPHMODE  20  // "�`�惂�[�h�ύX"
#define MULTIBTN_FEATURE_ROTATE     21  // "��ʂ̉�](HE)"
#define MULTIBTN_FEATURE_FONT       22  // "�t�H���g�ύX"
#define MULTIBTN_FEATURE_NETWORK    23  // "Network�ݒ�"
#define MULTIBTN_FEATURE_SELMENU    24  // "�I�����j���["
#define MULTIBTN_FEATURE_DISCONN    25  // "����ؒf"
#define MULTIBTN_FEATURE_GETLIST    26  // "�X���ꗗ�擾"
#define MULTIBTN_FEATURE_NEWARRIVAL 27  // "�V���m�F"
#define MULTIBTN_FEATURE_NNSHSET1   28  // "NNsi�ݒ�"
#define MULTIBTN_FEATURE_NNSHSET2   29  // "NNsi�ݒ�-2"
#define MULTIBTN_FEATURE_NNSHSET3   30  // "NNsi�ݒ�-3"
#define MULTIBTN_FEATURE_NNSHSET4   31  // "NNsi�ݒ�-4"
#define MULTIBTN_FEATURE_NNSHSET5   32  // "NNsi�ݒ�-5"
#define MULTIBTN_FEATURE_NNSHSET6   33  // "NNsi�ݒ�-6"
#define MULTIBTN_FEATURE_NNSHSET7   34  // "NNsi�ݒ�-7"
#define MULTIBTN_FEATURE_VERSION    35  // "�o�[�W����"
#define MULTIBTN_FEATURE_SELECTNEXT 36  // "�P��"
#define MULTIBTN_FEATURE_SELECTPREV 37  // "�P��"
#define MULTIBTN_FEATURE_PREVPAGE   38  // "�O�y�[�W"
#define MULTIBTN_FEATURE_NEXTPAGE   39  // "��y�[�W"
#define MULTIBTN_FEATURE_NNSIEND    40  // "NNsi�I��"
#define MULTIBTN_FEATURE_NGWORD1    41  // "NG1�ݒ�"
#define MULTIBTN_FEATURE_NGWORD2    42  // "NG2�ݒ�"
#define MULTIBTN_FEATURE_USER1TAB   43  // "���[�U1�^�u"
#define MULTIBTN_FEATURE_USER2TAB   44  // "���[�U2�^�u"

#define NNSH_BLOCKDISP_NOFMSG        5

#define NNSH_VFSDB_SETTING           0
#define NNSH_VFSDB_SUBJECT           1
#define NNSH_VFSDB_BBSLIST           2

#define NNSH_CONDITION_AND           0x0000  // AND����
#define NNSH_CONDITION_OR            0x0001  // OR����
#define NNSH_CONDITION_ALL           0x0003  // (�S����)

#define NNSH_CONDITION_NOTREADONLY   0x0004  // �Q�Ɛ�p���O�͏��O(�r�b�g)

#define NNSH_CONDITION_LOGLOC_ALL    0x0018  // 
#define NNSH_CONDITION_LOGLOC_PALM   0x0008  // Palm�����O
#define NNSH_CONDITION_LOGLOC_VFS    0x0010  // VFS�����O

#define NNSH_CONDITION_GETERR_ALL    0x0060  // 
#define NNSH_CONDITION_GETERR_NONE   0x0020  //  �G���[�Ȃ�
#define NNSH_CONDITION_GETERR_ERROR  0x0040  //  �擾�G���[

#define NNSH_CONDITION_GETRSV_ALL    0x0180  // 
#define NNSH_CONDITION_GETRSV_RSV    0x0080  //  �ۗ����X��
#define NNSH_CONDITION_GETRSV_NONE   0x0100  //  ��ۗ��X��

#define NNSH_THREADLEVEL_MASK        0x00ff  // �X�����x���}�X�N
#define NNSH_THREADCOND_MASK         0x7f00  // �X����ԃ}�X�N
#define NNSH_THREADCOND_SHIFT        8       // �X����ԃ}�X�N

#define NNSH_THREADLEVEL_UPPER       0       // �X�����x�� �h�ȏ�h
#define NNSH_THREADLEVEL_LOWER       1       // �X�����x�� �h�ȉ��h
#define NNSH_THREADLEVEL_ELSE        2       // �X�����x�� �h�ȊO�h
#define NNSH_THREADLEVEL_MATCH       3       // �X�����x�� �h����h

#define NNSH_BOARD_MATCH             0       // �ݒ� �h���h
#define NNSH_BOARD_ELSE              1       // �ݒ� �h�ȊO�h

// �d���X���̃`�F�b�N���@
#define NNSH_DUP_CHECK_HIGH          1       // ���������g����������
#define NNSH_DUP_CHECK_SLOW          2       // �������͎g��Ȃ����ᑬ��
#define NNSH_DUP_NOCHECK             3       // �d���`�F�b�N���s��Ȃ�

#define NNSH_STRING_SELECTION        0x0100  // 
#define NNSH_STRING_SETMASK          0x00ff  // 
#define NNSH_STRING_OR               0x0001  // 
#define NNSH_STRING_CUSTOM2_DEFAULT1 "Palm"
#define NNSH_STRING_CUSTOM2_DEFAULT2 "palm"
#define NNSH_STRING_CUSTOM2_DEFAULT3 "CLIE"
