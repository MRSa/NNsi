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
#define SOFT_PREFID           100
#define SOFT_PREFVERSION      203

#define NNSH_PREFERID         200
#define NNSH_PREFERTYPE       105

#define SOFT_DBTYPE_ID        'Data'
#define OFFLINE_DBTYPE_ID     'GIKO'

#define DBNAME_BBSLIST        "BBS-NNsi"
#define DBVERSION_BBSLIST     160

#define DBNAME_SUBJECT        "Subject-NNsi"
#define DBVERSION_SUBJECT     160

#define DBNAME_SETTING        "Settings-NNsi"
#define DBVERSION_SETTING     101
#define DBSIZE_SETTING        32

#define DBNAME_DIRINDEX       "DirIndex-NNsi"
#define DBVERSION_DIRINDEX    100

//////////////////////////////////////////////////////////////////////////////

#define RSCID_createThreadIndexSub_ARM     1     // ARMlet�֐��ԍ����̂P
#define RSCID_parseMessage_ARM             2     // ARMlet�֐��ԍ����̂Q

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define MINIBUF                     48
#define BUFSIZE                    128
#define BIGBUF                     640
#define PREVBUF                     12

// JUMPBUF�̒l��ύX�����ꍇ�ɂ́AARMlet���̓����V���{����ύX���邱��
#define JUMPBUF                     12  // ���ӁFARMlet�ɓ����V���{����`����

/** �ꎞ�m�ۗ̈�̕�(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4

/** BBS-DB�̕�����T�C�Y **/
#define MAX_URL                     80
#define MAX_BOARDNAME               40
#define MAX_NICKNAME                16
#define LIMIT_BOARDNAME             20

/** Thread-DB �̕�����T�C�Y **/
#define MAX_THREADFILENAME          32
#define MAX_THREADNAME              72
#define MAX_DIRNAME                 40
#define LIMIT_TITLENAME_DEFAULT     36
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
#define URL_BBSTABLE_OLD            "http://www6.ocn.ne.jp/~mirv/bbstable.html"
#define URL_BBSTABLE                "http://www.ff.iij4u.or.jp/~ch2/bbstable.html"
#define URL_PREFIX                  "test/read.cgi/"
#define URL_PREFIX_MACHIBBS         "bbs/read.pl?BBS="
#define FILE_THREADLIST             "subject.txt"

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont

#define NNSH_NOTENTRY_THREAD        0xffff

/** OFFLINE LOG(for GIKO Shippo) **/
#define OFFLINE_THREAD_NAME         "�Q�Ɛ�p���O"
#define OFFLINE_THREAD_NICK         "!GikoShippo/"
#define OFFLINE_THREAD_URL          "file://"

#define FAVORITE_THREAD_NAME        "���C�ɓ���"
#define GETALL_THREAD_NAME          "�擾�ςݑS��"
#define NOTREAD_THREAD_NAME         "���ǂ���"

#define NNSH_NOF_SPECIAL_BBS        4   // ����Ȕ��̐�
#define NNSH_SELBBS_FAVORITE        0   // ���C�ɓ���X��
#define NNSH_SELBBS_GETALL          1   // �擾�ςݑS��
#define NNSH_SELBBS_NOTREAD         2   // ���ǂ���
#define NNSH_SELBBS_OFFLINE         3   // OFFLINE�X��

// for NaNaShi FILE NAMEs
#define FILE_WRITEMSG               "write.txt"
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define LOGDATAFILE_PREFIX          "/PALM/Programs/2ch/"
#define LOGDIR_FILE                 "/PALM/Programs/2ch/logdir.txt"
#define DATAFILE_SUFFIX             ".dat"

//  time diference between from 'Jan/01/1904' to 'Jan/01/1970'.
#define TIME_CONVERT_1904_1970      (2082844800 - 32400)    // JST
#define TIME_CONVERT_1904_1970_UTC  (2082844800)            // UTC

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002
#define NNSH_VFS_USEOFFLINE         0x0004
#define NNSH_VFS_DBBACKUP           0x0008

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
#define NNSH_BBSTYPE_ERROR          0xff         // �{�[�h�^�C�v�G���[

/** �ꗗ�\����BBS�ꗗ�����C�ɓ��肩 **/
#define NNSH_FAVORLIST_MODE         1
#define NNSH_BBSLIST_MODE           0 


// �f�[�^�x�[�X�̃L�[�^�C�v(������������)
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2

// HTTP���b�Z�[�W�쐬���Ɏw��
#define HTTP_SENDTYPE_GET           10
#define HTTP_SENDTYPE_POST          20
#define HTTP_SENDTYPE_POST_MACHIBBS 40
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETSUBJECT_START       0
#define HTTP_GETSUBJECT_PART        0

// ��������
#define NNSH_SEARCH_FORWARD         1
#define NNSH_SEARCH_BACKWARD       -1

// �������݃��b�Z�[�W�m�F�R�}���h
#define NNSH_WRITECHK_SPID          0
#define NNSH_WRITECHK_REPLY         1

// ���b�Z�[�W�̑���
#define NNSH_MSGATTR_NOTSPECIFY    0xff   // �u���C�ɓ���v�w��͎w�肵�Ȃ�
#define NNSH_MSGATTR_NOTFAVOR      0x7f   // �u���C�ɓ���v�X���łȂ�(MASK�l)
#define NNSH_MSGATTR_FAVOR         0x80   // �u���C�ɓ���v�X���ł���
#define NNSH_MSGATTR_NOTERROR      0xbf   // �u�G���[�����v�X���łȂ�(MASK�l)
#define NNSH_MSGATTR_ERROR         0x40   // �u�G���[�����v�X���ł���


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

#define NNSH_MSGSTATE_NOTMODIFIED   10
#define NNSH_MSGSTATE_ERROR         20

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

#define NNSH_FIELD_SEPARATOR        "<>"
#define NNSH_FIELD_SEPARATOR_MACHI  ","
#define NNSH_RECORD_SEPARATOR       "\x0a"
#define NNSH_SUBJTITLE_END          "("

#define NNSH_MACHITOKEN_START       "\x0d\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a\x0d\x0a"
#define NNSH_MACHITOKEN_MSGSTART    "���O�F"
#define NNSH_MACHINAME_SUFFIX       "[�܂�BBS]"

/** **/
#define NNSH_SYMBOL_WRITE_DONE      "�������݂܂����B"

#define NNSH_DEVICE_NORMAL          0
#define NNSH_DEVICE_HANDERA         1

#define NNSH_JUMPMSG_LEN            10
#define NNSH_JUMPMSG_HEAD           ">>"
#define NNSH_JUMPMSG_BACK           "�P�߂�"
#define NNSH_JUMPMSG_TO_LIST        "�ꗗ��"
#define NNSH_JUMPSEL_TO_LIST         0
#define NNSH_JUMPSEL_BACK_USELIST    1
#define NNSH_JUMPSEL_BACK            0

#define NNSH_JUMPMSG_OPENBBS        "��I��"
#define NNSH_JUMPMSG_OPENMENU       "���j���["
#define NNSH_JUMPMSG_OPENNEW        "�V���m�F"
#define NNSH_JUMPMSG_OPENLIST       "�ꗗ�擾"
#define NNSH_JUMPMSG_OPENGET        "�X���擾"
#define NNSH_JUMPMSG_OPENINFO       "�X�����"
#define NNSH_JUMPSEL_NUMLIST        6  // ���X�g�A�C�e����
#define NNSH_JUMPSEL_OPENBBS        0  // ��I��
#define NNSH_JUMPSEL_OPENMENU       1  // ���j���[
#define NNSH_JUMPSEL_OPENNEW        2  // �V���m�F
#define NNSH_JUMPSEL_OPENLIST       3  // �ꗗ�擾
#define NNSH_JUMPSEL_OPENGET        4  // �X���擾
#define NNSH_JUMPSEL_OPENINFO       5  // �X�����

