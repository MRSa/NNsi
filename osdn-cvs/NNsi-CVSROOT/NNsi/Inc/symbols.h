/*============================================================================*
 *
 * $Id: symbols.h,v 1.236 2012/01/10 16:27:42 mrsa Exp $
 *
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
#define DBVERSION_SETTING     121
#define DBSIZE_SETTING        32
#define DBVERSION_XML_SETTING 100

#define DBNAME_DIRINDEX       "DirIndex-NNsi"
#define DBVERSION_DIRINDEX    100

#define DBNAME_GETRESERVE     "GetReserve-NNsi"
#define DBVERSION_GETRESERVE  100

#define DBNAME_PERMANENTLOG    "LogPermanent-NNsi"
#define DBVERSION_PERMANENTLOG 100

#define DBNAME_TEMPORARYLOG    "LogTemporary-NNsi"
#define DBVERSION_TEMPORARYLOG 100

#define DBNAME_TEMPURL         "TempURL-NNsi"
#define DBVERSION_TEMPURL      100

#define DBNAME_NGWORD          "NGWord-NNsi"
#define DBVERSION_NGWORD       100

#define DBNAME_MACROSCRIPT     "Macro-NNsi"
#define DBVERSION_MACROSCRIPT  100

#define DBNAME_RUNONCEMACRO    "RunOnce-NNsi"
#define DBNAME_MACRORESULT     "MacroResult-NNsi"
#define DBVERSION_MACRO        100

#define DBNAME_LOGTOKEN        "LogToken-NNsi"
#define DBVERSION_LOGTOKEN     100

#define NNSH_CREATORID_PREPARE_NEWARRIVAL 'mlDA'

#define DBSOFT_CREATOR_ID      'NNsj'
#define DATA_DBTYPE_ID         'Mcro'

//////////////////////////////////////////////////////////////////////////////

#define RSCID_createThreadIndexSub_ARM     1     // ARMlet�֐��ԍ����̂P
#define RSCID_parseMessage_ARM             2     // ARMlet�֐��ԍ����̂Q

//////////////////////////////////////////////////////////////////////////////

/** buffer size(small) **/
#define TINYBUF                      8
#define MINIBUF                     48
#define BUFSIZE                    128
#define HIDEBUFSIZE                128
#define PASSSIZE                     8
#define BIGBUF                     640
#define PREVBUF                     12
#define JUMPBUF                     16
#define NGWORD_LEN                  40
#define OYSTERID_LEN                64
#define OYSTER_LEN                  64
#define BE2chID_LEN                 64
#define BE2chPW_LEN                 32
#define SEARCH_HEADLEN              16
#define MAX_GETLOG_RECORDS        2048
#define MAX_MACRO_DBFILES          260

#define MAX_TIMEOUT_VALUE          999

#define MAX_SELECTLOGRECORD         80
#define MAX_LOGRECLEN               40
#define MAX_STRLEN                  32
#define MAX_DBNAMELEN               32

/** �ꎞ�m�ۗ̈�̕�(5120 < x < 61000) **/
#define NNSH_WORKBUF_MIN          5120
#define NNSH_WORKBUF_MAX         61000
#define NNSH_WORKBUF_DEFAULT     32750
#define MARGIN                       4
#define FREE_SPACE_MARGIN         2048

/** �������ݎ��Ɋm�ۂ���o�b�t�@�T�C�Y **/
#define NNSH_WRITEBUF_MIN         4096
#define NNSH_WRITEBUF_MAX        16384
#define NNSH_WRITEBUF_DEFAULT     8192

#define NNSH_WRITECOOKIE_BUFFER   2560

#define NNSH_WRITELBL_MAX           26
#define NNSH_WRITELBL_HEAD        "��"

#define NNSH_NEWTHREAD_MAX          32
#define NNSH_NEWTHREAD_LBLHEAD    "�X������:"

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
#define LIMIT_TITLE_HEADER_SIZE    (MAX_THREADNAME - MARGIN)
#define MAX_DEPTH                   10
#define MAX_LISTLENGTH              24

/** GetData-DB�̕�����T�C�Y **/
#define MAX_DATAPREFIX              32
#define MAX_DATATOKEN               80
#define MAX_TITLE_LABEL             32
#define MAX_GETLOG_URL             244
#define MAX_GETLOG_DATETIME         38
#define MAX_URL_CNT                 66
#define MAX_URLLIST_CNT             80
#define MAX_GETLOG_REGETHOUR      0xff

/** URL�ꗗ�̕\���T�C�Y **/
#define MAX_NAMELABEL               48
#define MAX_URL_LIST                24

/** �^�C�g������у��x���̗̈�T�C�Y **/
#define BBSSEL_PAGEINFO             12
#define NNSH_MSGTITLE_BUFSIZE       10
#define SEPARATOR_LEN               24

/** VFS�t�@�C�����̍ő�T�C�Y  **/
#define MAXLENGTH_FILENAME          256

/** �����_�E�����[�h���ɑz�肷��HTTP�w�b�_�T�C�Y **/
#define NNSH_HTTPGET_HEADERMAX      400
#define NNSH_DOWNLOADSIZE_MIN      2048

/** �ʐM�^�C���A�E�g�����l(tick) **/
#define NNSH_NET_TIMEOUT           2500

/** ���b�Z�[�W�̍ő僌�X�ԍ� **/
#define NNSH_MESSAGE_MAXNUMBER     1005
#define NNSH_MESSAGE_MAXLIMIT      25000
#define NNSH_MESSAGE_LIMIT         1000

/** BBS URL **/
#define URL_BBSTABLE             "http://menu.2ch.net/bbstable.html"
#define URL_BBSTABLE_OLD         "http://www.ff.iij4u.or.jp/~ch2/bbstable.html"

#define URL_PREFIX                     "test/read.cgi/"
#define URL_PREFIX_IMODE               "test/r.i/"
#define URL_PREFIX_MOBILE              "http://c.2ch.net/test/-/"
//#define URL_PREFIX_MACHIBBS            "bbs/read.pl?BBS="
//#define URL_PREFIX_MACHIBBS_IMODE      "bbs/read.pl?IMODE=TRUE&BBS="
#define URL_PREFIX_MACHIBBS            "bbs/read.cgi?BBS="
#define URL_PREFIX_MACHIBBS_IMODE      "bbs/read.cgi?IMODE=TRUE&BBS="
#define URL_PREFIX_SHITARABA           "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABA_IMODE     "cgi-bin/read.cgi?bbs="
#define URL_PREFIX_SHITARABAJBBS       "bbs/read.cgi?BBS="
#define URL_PREFIX_SHITARABAJBBS_IMODE "bbs/i.cgi?BBS="
#define FILE_THREADLIST                "subject.txt"

#define URL_PROTOCOL_USESSL            "https://"

#define URL_BE2ch_PROFILE              "http://be.2ch.net/test/p.php"
#define URL_PREFIX_BE2ch_PROFILE       "?i="

#define MAX_REDIRECT_COUNT          3

/** default font ID **/
#define NNSH_DEFAULT_FONT           stdFont
#define NNSH_DEFAULT_FONT_HR        stdFont

#define NNSH_NOTENTRY_THREAD        0xffff
#define NNSH_NOTENTRY_BBS           0xffff
#define NNSH_DATABASE_BLANK         0xffff
#define NNSH_DATABASE_UNKNOWN       0xfffe
#define NNSH_DATABASE_PENDING       0xfffd

/** OFFLINE LOG(for GIKO Shippo) **/
#define OFFLINE_THREAD_NAME         "�Q�ƃ��O"
#define OFFLINE_THREAD_NICK         "!GikoShippo/"
#define OFFLINE_THREAD_URL          "file://"

#define FAVORITE_THREAD_NAME        "���C�ɓ���"
#define GETALL_THREAD_NAME          "�擾�ςݑS��"
#define NOTREAD_THREAD_NAME         "���ǂ���"
#define CUSTOM1_THREAD_NAME         "�擾�G���["
#define CUSTOM2_THREAD_NAME         "Palm�֘A"
#define CUSTOM3_THREAD_NAME         "�V�K(3���ȓ�)"
#define CUSTOM4_THREAD_NAME         "�ő咴"
#define CUSTOM5_THREAD_NAME         "�擾�ۗ���"

#define CUSTOM1_THREAD_HEAD         "1>"
#define CUSTOM2_THREAD_HEAD         "2>"
#define CUSTOM3_THREAD_HEAD         "3>"
#define CUSTOM4_THREAD_HEAD         "4>"
#define CUSTOM5_THREAD_HEAD         "5>"


#define NNSH_NOF_SPECIAL_BBS        9   // ����Ȕ��̐�
#define NNSH_SELBBS_NOTREAD         0   // ���ǂ���
#define NNSH_SELBBS_GETALL          1   // �擾�ςݑS��
#define NNSH_SELBBS_FAVORITE        2   // ���C�ɓ���X��
#define NNSH_SELBBS_CUSTOM1         3   // CUSTOM1
#define NNSH_SELBBS_CUSTOM2         4   // CUSTOM2
#define NNSH_SELBBS_CUSTOM3         5   // CUSTOM3
#define NNSH_SELBBS_CUSTOM4         6   // CUSTOM4
#define NNSH_SELBBS_CUSTOM5         7   // CUSTOM5
#define NNSH_SELBBS_OFFLINE         8   // OFFLINE�X��

// for NaNaShi FILE NAMEs
#define FILE_WRITEMSG               "write.txt"
#define FILE_WRITENAME              "writeName.txt"
#define FILE_WRITEMAIL              "writeMail.txt"
#define FILE_RECVMSG                "recv.txt"
#define FILE_SENDMSG                "send.txt"
#define FILE_XML_NNSISET            "NNsiSet.xml"
#define FILE_LOGCHARGE_PREFIX       "NNsiLog"
#define FILE_LOGCHARGE_SUFFIX       ".htm"
#define FILE_TRIMLOG_PREFIX         "TrimLog"
#define FILE_TRIMLOG_SUFFIX         ".htm"

#define NNSH_HTTP_CONTENT_LENGTH    "Content-Length:"
#define NNSH_HTTP_CONTENT_GZIPED    "Content-Encoding: gzip"     // by Nickle
#define NNSH_HTTP_CONTENT_XGZIPED   "Content-Encoding: x-gzip"

#define DATAFILE_PREFIX             "/PALM/Programs/NNsh/"
#define LOGDATAFILE_PREFIX          "/PALM/Programs/2ch/"
#define LOGDIR_FILE                 "/PALM/Programs/2ch/logdir.txt"
#define DATAFILE_SUFFIX             ".dat"
#define CGIFILE_SUFFIX              ".cgi"

//  time diference between from 'Jan/01/1904' to 'Jan/01/1970'.
#define TIME_CONVERT_1904_1970      (2082844800 - 32400)
#define TIME_CONVERT_1904_1970_UTC  (2082844800)
#define TIME_24HOUR_SECOND          ((Int32) 24 * ((Int32) 60 * (Int32) 60))

#define TIME_CONVERT_1970_2005      (1104505200)

/** VFS SETTINGS **/
#define NNSH_NOTSUPPORT_VFS         0xf000
#define NNSH_VFS_DISABLE            0x0000
#define NNSH_VFS_ENABLE             0x0001
#define NNSH_VFS_WORKAROUND         0x0002
#define NNSH_VFS_USEOFFLINE         0x0004
#define NNSH_VFS_DBBACKUP           0x0008
#define NNSH_VFS_DBIMPORT           0x0010
#define NNSH_VFS_DIROFFLINE         0x0020
#define NNSH_VFS_USE_CF             0x0040

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

#define NNSH_NGCHECK_ALLAREA        0
#define NNSH_NGCHECK_NAME_MAIL      1
#define NNSH_NGCHECK_NOTMESSAGE     2
#define NNSH_NGCHECK_MESSAGE        3

#define NNSH_COPYSTATUS_SEARCHTOP   0
#define NNSH_COPYSTATUS_COPYING     1
#define NNSH_COPYSTATUS_COPYDONE    2

#define NNSH_GETLOGDB_CHECKURL          0x0000  
#define NNSH_GETLOGDB_CHECKPARENT       0x0001
#define NNSH_GETLOGDB_CHECKTOKEN        0x0002
#define NNSH_GETLOGDB_CHECKDATE         0x0004
#define NNSH_GETLOGDB_CHECK_PARENT_DATE 0x0005
#define NNSH_GETLOGDB_FORCE_DELETE      0x0008

#define NNSH_GETLOGDBMAX_SUBDATA    20000
#define NNSH_GETLOGDBMAX_SITEDATA   190

/** BBS-DB�̕\�����(�I����ʂŕ\�����邩/���Ȃ���) **/
#define NNSH_BBSSTATE_VISIBLE       0x01         // �\��
#define NNSH_BBSSTATE_INVISIBLE     0x00         // ��\��
#define NNSH_BBSSTATE_VISIBLEMASK   0xfe         // �\���E��\���p�}�X�N

#define NNSH_BBSSTATE_LEVELMASK     0x0e         // �X�����x�����o�}�X�N
#define NNSH_BBSSTATE_NOTFAVOR      0xf1         // �X�����x���N���A
#define NNSH_BBSSTATE_FAVOR_L1      0x02         // �X�����x��L1
#define NNSH_BBSSTATE_FAVOR_L2      0x04         // �X�����x��L2
#define NNSH_BBSSTATE_FAVOR_L3      0x06         // �X�����x��L3
#define NNSH_BBSSTATE_FAVOR_L4      0x08         // �X�����x��L4
#define NNSH_BBSSTATE_FAVOR         0x0e         // �X�����x��HIGH

#define NNSH_BBSSTATE_SUSPEND       0x10   // �u�擾��~�vBBS�ł���
#define NNSH_BBSSTATE_NOTSUSPEND    0xef   // �u�擾��~�vBBS�ł͂Ȃ�(MASK�l)

/** BBS-DB�̃{�[�h�^�C�v **/
#define NNSH_BBSTYPE_2ch                 0x00    // 2ch(�ʏ탂�[�h)
#define NNSH_BBSTYPE_MACHIBBS            0x01    // �܂�BBS(���ꃂ�[�h)
#define NNSH_BBSTYPE_SHITARABA           0x02    // �������BBS
#define NNSH_BBSTYPE_SHITARABAJBBS_OLD   0x03    // �������@JBBS
#define NNSH_BBSTYPE_OTHERBBS            0x04    // 2ch�݊�BBS
#define NNSH_BBSTYPE_PLUGINSUPPORT       0x05    // 2ch�`���ϊ�BBS
#define NNSH_BBSTYPE_HTML                0x06    // html�`���f�[�^
#define NNSH_BBSTYPE_SHITARABAJBBS_RAW   0x07    // �������@JBBS(RAWMODE)
#define NNSH_BBSTYPE_PLAINTEXT           0x08    // �ʏ�̃e�L�X�g�`��
#define NNSH_BBSTYPE_2ch_EUC             0x09    // 2ch(EUC�`��)
#define NNSH_BBSTYPE_OTHERBBS_2          0x0a    // 2ch�݊��`��(����2)
#define NNSH_BBSTYPE_ERROR               0xff    // �{�[�h�^�C�v�G���[
#define NNSH_BBSTYPE_MASK                0x1f    // �{�[�h�^�C�vMASK
#define NNSH_BBSTYPE_CHARSETMASK         0xe0    // �{�[�h�̕����R�[�h
#define NNSH_BBSTYPE_CHAR_SJIS           0x00    // �W�������R�[�h(SHIFT JIS)
#define NNSH_BBSTYPE_CHAR_EUC            0x20    // EUC�����R�[�h
#define NNSH_BBSTYPE_CHAR_JIS            0x40    // JIS�����R�[�h(iso-2022-jp)
#define NNSH_BBSTYPE_CHAR_SHIFT             5    // �E�V�t�g

/** �ꗗ�\����BBS�ꗗ�����C�ɓ��肩 **/
#define NNSH_FAVORLIST_MODE         1
#define NNSH_BBSLIST_MODE           0 

// �f�[�^�x�[�X�̃L�[�^�C�v(������������)
#define NNSH_KEYTYPE_CHAR           0
#define NNSH_KEYTYPE_UINT32         1
#define NNSH_KEYTYPE_UINT16         2
#define NNSH_KEYTYPE_SUBJECT        3

// HTTP���b�Z�[�W�쐬���Ɏw��
#define HTTP_SENDTYPE_GET                    10
#define HTTP_SENDTYPE_GET_NOTMONA            11
#define HTTP_SENDTYPE_POST                   20
#define HTTP_SENDTYPE_POST_SEQUENCE2         21
#define HTTP_SENDTYPE_POST_2chLOGIN          24
#define HTTP_SENDTYPE_POST_OTHERMETHOD       25
#define HTTP_SENDTYPE_POST_Be2chLOGIN        27
#define HTTP_SENDTYPE_POST_OTHERBBS          30
#define HTTP_SENDTYPE_POST_MACHIBBS          40
#define HTTP_SENDTYPE_POST_SHITARABAJBBS     80
#define HTTP_SENDTYPE_POST_SHITARABA         90
#define HTTP_SENDTYPE_MASK               0x00ff
#define HTTP_RANGE_OMIT             0xffffffff
#define HTTP_GETSUBJECT_LEN         4096
#define HTTP_GETLOG_LEN             1536
#define HTTP_GETLOG_START           0
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
#define NNSH_MSGATTR_NGWORDSET     0x04   //  NG���[�h�ݒ� ON
#define NNSH_MSGATTR_CLRNGWORD     0xfb   //  NG���[�h�ݒ� OFF (�N���A)

#define NNSH_ATTRLABEL_TITLE       ", Level:" 
#define NNSH_ATTRLABEL_NOTFAVOR    "-"    // �u���C�ɓ���v�ł͂Ȃ�
#define NNSH_ATTRLABEL_FAVOR_L1    "L"    // �u���C�ɓ���v���x��(��)
#define NNSH_ATTRLABEL_FAVOR_L2    "1"    // �u���C�ɓ���v���x��(����)
#define NNSH_ATTRLABEL_FAVOR_L3    "2"    // �u���C�ɓ���v���x��(��)
#define NNSH_ATTRLABEL_FAVOR_L4    "3"    // �u���C�ɓ���v���x��(����)
#define NNSH_ATTRLABEL_FAVOR       "H"    // �u���C�ɓ���v���x��(��)

#define NUMBER_OF_JOGMASKS          4

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
#define NNSH_DB_UPDATE_LOGSUBDB     0x0010
#define NNSH_DB_UPDATE_LOGCHARGE    0x0020
#define NNSH_DB_UPDATE_LOGTOKEN     0x0040

#define NNSH_DB_MASK_BBS            0xfffe
#define NNSH_DB_MASK_SUBJECT        0xfffd
#define NNSH_DB_MASK_SETTING        0xfffb
#define NNSH_DB_MASK_DIRINDEX       0xfff7

//////////////////////////////////////////////////////////////////////////////

/********** HTML��ԉ�͗p ********/
#define HTTP_ANCHORSTATE_NORMAL     101
#define HTTP_ANCHORSTATE_ANCHOR     102
#define HTTP_ANCHORSTATE_LABEL      103
#define HTTP_ANCHORSTATE_KANJI      104

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
#define NNSH_SUBJSTATUS_DELETE_OFFLINE  20

#define NNSH_MSGSTATE_NOTMODIFIED   10
#define NNSH_MSGSTATE_ERROR         20

#define NNSH_GETLOGSTATE_SELECTED    0x0002
#define NNSH_GETLOGSTATE_UPDATE      0x0001
#define NNSH_GETLOGSTATE_DATETIME    0x0004
#define NNSH_GETLOGSTATE_NOTMODIFIED 0x0000
#define NNSH_GETLOGSTATE_MASK_UPDATE 0x0001
#define NNSH_GETLOGSTATE_MASK_SELECT 0x0002

#define NNSH_ERRORCODE_PARTGET        (~errNone - 12)
#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)
#define NNSH_ERRORCODE_THREADNOTHING  (~errNone - 88)
#define NNSH_ERRORCODE_WRONGVFSMODE   (~errNone - 10)
#define NNSH_ERRORCODE_COPYFAIL       (~errNone - 11)
#define NNSH_ERRORCODE_NOTFOUND       (~errNone - 12)
#define NNSH_ERRORCODE_NOTACCEPTEDMSG (~errNone + 100)

/**********  ���b�Z�[�W�p�[�X�p  *********/
#define MSGSTATUS_NAME              1
#define MSGSTATUS_EMAIL             2
#define MSGSTATUS_DATE              3
#define MSGSTATUS_NORMAL            4

/**********  BBS���L�X����  **************/
#define NNSH_BBSLIST_AGAIN         -1            // �X�������ēx�m�F����


/*********** �V���m�F���{�R�}���h ********/
#define NNSH_NEWARRIVAL_NORMAL      0            // �ʏ�擾���[�h
#define NNSH_NEWARRIVAL_ONLYMESNUM  1            // �X���Ԏw��\��̂��̂̂�

/////////////////////////////////////////////////////////////////////////////

/*** DATA TOKENS ***/
#define DATA_SEPARATOR              "<>"
#define DATA_NEWLINE                "<br>"

/** for BBS NAME PARSER **/
#define HTTP_TOKEN_ST_ANCHOR        "<A"
#define HTTP_TOKEN_ED_ANCHOR        "</A>"
#define HTTP_TOKEN_END              ">"


#define NNSH_KANJICODE_SHIFTJIS     0x00
#define NNSH_KANJICODE_EUC          0x01
#define NNSH_KANJICODE_JIS          0x02
#define NNSH_KANJICODE_UTF8         0x03
#define NNSH_KANJICODE_NOT_TEXT     0xf0

#define NNSH_CHARCOLOR_ESCAPE       '\x1b'
#define NNSH_CHARCOLOR_NORMAL       '\x09'
#define NNSH_CHARCOLOR_HEADER       '\x0b'
#define NNSH_CHARCOLOR_BOLDHEADER   '\x0f'
#define NNSH_CHARCOLOR_FOOTER       '\x0c'
#define NNSH_CHARCOLOR_INFORM       '\x0e'
#define NNSH_CHARCOLOR_ANCHOR       '\x08'
#define NNSH_CHARCOLOR_BOLDDATA     '\x11'

#define NNSH_HTTP_HEADER_ENDTOKEN   "\x0d\x0a\x0d\0a"
#define NNSH_NGSET_SEPARATOR        ','
#define NNSH_FIELD_SEPARATOR        "<>"
#define NNSH_FIELD_SEPARATOR_MACHI  ","
#define NNSH_RECORD_SEPARATOR       "\x0a"
#define NNSH_SUBJTITLE_END          "("

#define NNSH_JBBSTOKEN_START        "\x0a<dt>"
#define NNSH_JBBSTOKEN_END          "<br><br>\x0a"
#define NNSH_JBBSTOKEN_MSGSTART     "\xcc\xbe\xc1\xb0\xa1\xa7" // ���O�F
#ifdef OLD_MACHIBBS_TOKEN
#define NNSH_MACHITOKEN_START       "\x0d\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a\x0d\x0a"
#else
#define NNSH_MACHITOKEN_START       "\x0a<dt>"
#define NNSH_MACHITOKEN_END         "<br><br>\x0a"
#endif
#define NNSH_MACHITOKEN_BEGINCOMMENT "<!--"
#define NNSH_JBBSTOKEN_RAW_START    "\x0d\x0a"
#define NNSH_JBBSTOKEN_RAW_START_RES "<>"
#define NNSH_JBBSTOKEN_RAW_END_RES  "\x0a"
#define NNSH_MACHITOKEN_MSGSTART2   "<dt>"
#define NNSH_MACHITOKEN_MSGSTART    "���O�F"
#define NNSH_MACHINAME_SUFFIX       "[�܂�BBS]"
#define NNSH_SHITARABA_SUFFIX       "[�������]"
#define NNSH_SHITARABAJBBS_SUFFIX   "[�������@JBBS]"

#define NNSH_TOKEN_BEFORE_NUMBER     0x01    // �g�[�N���̑O�ɐ���
#define NNSH_TOKEN_BEFORE_LETTER     0x02    // �g�[�N���̑O�ɕ���
#define NNSH_TOKEN_AFTER_NUMBER      0x10    // �g�[�N���̌�ɐ���
#define NNSH_TOKEN_AFTER_LETTER      0x20    // �g�[�N���̌�ɕ���

/** **/
#define NNSH_SYMBOL_WRITE_DONE      "�������݂܂����B"
#define NNSH_SYMBOL_WRITE_NG        "�d�q�q�n�q"

#define NNSH_DEVICE_NORMAL         0x0000     // �W��Palm
#define NNSH_DEVICE_HANDERA        0x0001     // Handera 330
#define NNSH_DEVICE_DIASUPPORT     0x0002     // Dynamic Input Area�T�|�[�g�@
#define NNSH_DEVICE_GARMIN         0x0003     // Garmin iQUE 3600

#define NNSH_VFS_USEMEDIA_ANY       0
#define NNSH_VFS_USEMEDIA_CF        1
#define NNSH_VFS_USEMEDIA_MS        2
#define NNSH_VFS_USEMEDIA_SD        3
#define NNSH_VFS_USEMEDIA_SM        4
#define NNSH_VFS_USEMEDIA_RD        5
#define NNSH_VFS_USEMEDIA_DoC       6

#define NNSH_JUMPMSG_LEN            14
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
#define NNSH_JUMPMSG_DEVICEINFO     "�f�o�C�X���"
#define NNSH_JUMPMSG_OPENLINK       "�����N���J��"
#define NNSH_JUMPMSG_EXECA2DA       "a2DA(AA�Q��)"
#define NNSH_JUMPMSG_BTOOTH_ON      "Bt On"
#define NNSH_JUMPMSG_BTOOTH_OFF     "Bt Off"
#define NNSH_JUMPMSG_MEMOURL        "MemoURL�֏o��"
#define NNSH_JUMPMSG_TOGGLERAWMODE  "RAW/�ʏ�ؑ�"
#define NNSH_JUMPMSG_SELECTSEPA     "��؂�ύX"
#define NNSH_JUMPMSG_EDITSEPA       "��؂�ҏW"
#define NNSH_JUMPMSG_OPENLINK_WEBDA "�����N��Q��"
#define NNSH_JUMPMSG_TOGGLE_NGWORD  "NG�ؑ�"
#define NNSH_JUMPMSG_BACK_AND_DEL   "�폜��ꗗ��"
#define NNSH_JUMPMSG_GET_THREADNUM  "�X���Ԏw��"
#define NNSH_JUMPMSG_ENTRY_NGWORD   "NG�o�^"
#define NNSH_JUMPMSG_REPLYMSG       "�Q�ƃJ�L�R"

#define NNSH_NOF_JUMPMSG_EXT        28
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
#define NNSH_JUMPSEL_DEVICEINFO     0x800d
#define NNSH_JUMPSEL_OPENLINK       0x800e
#define NNSH_JUMPSEL_EXECA2DA       0x800f
#define NNSH_JUMPSEL_BTOOTH_ON      0x8011
#define NNSH_JUMPSEL_BTOOTH_OFF     0x8012
#define NNSH_JUMPSEL_MEMOURL        0x8013
#define NNSH_JUMPSEL_TOGGLERAWMODE  0x8014
#define NNSH_JUMPSEL_SELECTSEPA     0x8015
#define NNSH_JUMPSEL_EDITSEPA       0x8016
#define NNSH_JUMPSEL_OPENLINK_WEBDA 0x8017
#define NNSH_JUMPSEL_TOGGLE_NGWORD  0x8018
#define NNSH_JUMPSEL_BACK_AND_DEL   0x8019
#define NNSH_JUMPSEL_GET_THREADNUM  0x801a
#define NNSH_JUMPSEL_ENTRY_NGWORD   0x801b
#define NNSH_JUMPSEL_REPLYMSG       0x801c


#define NNSH_JUMPMSG_OPENBBS        "��I��"
#define NNSH_JUMPMSG_OPENBBS_GETLOG "�T�C�g�I��"
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
#define NNSH_JUMPMSG_SHOWDEVINFO    "�f�o�C�X���"
#define NNSH_JUMPMSG_DIRSELECTION   "Dir�I��(chdir)"
#define NNSH_JUMPMSG_CHANGELOGLOC   "���O�ʒu�ύX"
#define NNSH_JUMPMSG_CHANGELOGDIR   "Dir�ύX(move)"
#define NNSH_JUMPMSG_BT_ON          "Bt On"
#define NNSH_JUMPMSG_BT_OFF         "Bt Off"
#define NNSH_JUMPMSG_OPENGETLOGLIST "�Q�ƃ��O�ꗗ"

#define NNSH_ITEMLEN_JUMPLIST       14      // ���X�g�A�C�e��������
#define NNSH_JUMPSEL_NUMLIST        21      // ���X�g�A�C�e����
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
#define NNSH_JUMPSEL_SHOWDEVINFO    0x900e  // �f�o�C�X���
#define NNSH_JUMPSEL_DIRSELECTION   0x900f  // �f�B���N�g���I��
#define NNSH_JUMPSEL_CHANGELOGLOC   0x9010  // ���O�ʒu�ύX
#define NNSH_JUMPSEL_CHANGELOGDIR   0x9011  // �f�B���N�g���ύX
#define NNSH_JUMPSEL_BT_ON          0x9012  // Bt On
#define NNSH_JUMPSEL_BT_OFF         0x9013  // Bt Off
#define NNSH_JUMPSEL_OPENGETLOGLIST 0x9014  // �Q�ƃ��O�ꗗ��

#define NNSH_ITEMLEN_GETLOGJUMP     14      // ���X�g�A�C�e��������
#define NNSH_NOF_JUMPMSG_GETLOG     16
#define NNSH_JUMPSEL_GETLOGRETURN   0xA001
#define NNSH_JUMPSEL_GETLOG_OPEN    0xA002
#define NNSH_JUMPSEL_GETLOGBROWSER  0xA003
#define NNSH_JUMPSEL_GETLOG_MEMOURL 0xA004
#define NNSH_JUMPSEL_GETLOG_CHECK   0xA005
#define NNSH_JUMPSEL_GETLOG_OPENMNU 0xA006
#define NNSH_JUMPSEL_GETLOG_LEVEL   0xA007
#define NNSH_JUMPSEL_GETLOG_DISCONN 0xA008
#define NNSH_JUMPSEL_GETLOG_BT_ON   0xA009
#define NNSH_JUMPSEL_GETLOG_BT_OFF  0xA00A
#define NNSH_JUMPSEL_GETLOG_NNSIEND 0xA00B
#define NNSH_JUMPSEL_GETLOG_EDIT    0xA00C  // ������
#define NNSH_JUMPSEL_GETLOG_NEW     0xA00D  // 
#define NNSH_JUMPSEL_GETLOG_SEPA    0xA00E  // 
#define NNSH_JUMPSEL_GETLOG_WWWINFO 0xA00F
#define NNSH_JUMPSEL_GETLOG_DELETE_READ 0xA010

#define NNSH_JUMPMSG_GETLOGRETURN   "�X���ꗗ��"
#define NNSH_JUMPMSG_GETLOG_OPEN    "�J��"
#define NNSH_JUMPMSG_GETLOGBROWSER  "�u���E�U�ŊJ��"
#define NNSH_JUMPMSG_GETLOG_MEMOURL "MemoURL�ɑ���"
#define NNSH_JUMPMSG_GETLOG_CHECK   "�V���m�F"
#define NNSH_JUMPMSG_GETLOG_OPENMNU "���j���["
#define NNSH_JUMPMSG_GETLOG_LEVEL   "Level�ύX"
#define NNSH_JUMPMSG_GETLOG_DISCONN "����ؒf"
#define NNSH_JUMPMSG_GETLOG_BT_ON   "Bt On"
#define NNSH_JUMPMSG_GETLOG_BT_OFF  "Bt Off"
#define NNSH_JUMPMSG_GETLOG_NNSIEND "NNsi�I��"

#define NNSH_JUMPMSG_GETLOG_EDIT    "�ҏW"
#define NNSH_JUMPMSG_GETLOG_NEW     "�V�K�o�^"
#define NNSH_JUMPMSG_GETLOG_SEPA    "��ؕҏW"
#define NNSH_JUMPMSG_GETLOG_WWWINFO "�T�C�g���"
#define NNSH_JUMPMSG_GETLOG_DELETE_READ "���ǋL���폜"

#define NNSH_STEP_ITEMUP           -1
#define NNSH_STEP_ITEMDOWN          1
#define NNSH_STEP_PAGEUP           -1
#define NNSH_STEP_PAGEDOWN          1
#define NNSH_STEP_REDRAW            0
#define NNSH_STEP_UPDATE           10
#define NNSH_STEP_TO_BOTTOM       100
#define NNSH_STEP_TO_TOP          NNSH_STEP_UPDATE
#define NNSH_STEP_NEWOPEN         200

#define NNSH_ITEM_LASTITEM         0xffff
#define NNSH_ITEM_BOTTOMITEM       0xfffd

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

#define NNSH_LIMIT_COPYBUFFER      2000
#define NNSH_LIMIT_MEMOPAD         4000
#define NNSH_LIMIT_RETRY           5
#define NNSH_MEMOBUFFER_MERGIN     96

#define NNSH_BUSYWIN_MAXLEN        32
#define NNSH_MAX_OMIT_LENGTH       10

#define NNSH_WRITE_DELAYTIME       7
#define NNSH_NEWTHREAD_DELAYTIME   2
#define NNSH_WRITE_DELAYSECOND     45

#define NNSH_MSGTOKENSTATE_NONE    0
#define NNSH_MSGTOKENSTATE_MESSAGE 1
#define NNSH_MSGTOKENSTATE_END     2

#define NNSH_INPUTWIN_SEARCH       "TITLE����"
#define NNSH_INPUTWIN_GETMESNUM    "�X���擾(�X���Ԏw��)"
#define NNSH_INPUTWIN_MODIFYTITLE  "�X���^�C�g���ύX"
#define NNSH_INPUTWIN_NGWORD1      "NG������P�̓o�^"
#define NNSH_INPUTWIN_NGWORD2      "NG������Q�̓o�^"
#define NNSH_INPUTWIN_SETPASS      "NNsi�N�����[�h�ݒ�"
#define NNSH_INPUTWIN_ENTPASS      "Register..."
#define NNSH_INPUTWIN_RESOLVE      "Host Name"

#define NNSH_DIALOG_USE_SEARCH      10
#define NNSH_DIALOG_USE_GETMESNUM   20
#define NNSH_DIALOG_USE_MODIFYTITLE 40
#define NNSH_DIALOG_USE_SEARCH_TITLE 50
#define NNSH_DIALOG_USE_SEARCH_MESSAGE 60
#define NNSH_DIALOG_USE_SETNGWORD   80
#define NNSH_DIALOG_USE_PASS       100
#define NNSH_DIALOG_USE_RESOLVE    110
#define NNSH_DIALOG_USE_INPUTURL   120
#define NNSH_DIALOG_USE_MACROINPUT 130
#define NNSH_DIALOG_USE_OTHER      150

// �_�C�A���O���ȗ�����ݒ�
#define NNSH_OMITDIALOG_NOTHING      0x00
#define NNSH_OMITDIALOG_CONFIRM      0x01
#define NNSH_OMITDIALOG_WARNING      0x02
#define NNSH_OMITDIALOG_INFORMATION  0x04
#define NNSH_OMITDIALOG_ALLOMIT      ((NNSH_OMITDIALOG_CONFIRM)|(NNSH_OMITDIALOG_WARNING)|(NNSH_OMITDIALOG_INFORMATION))

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
#define NNSH_SWITCHUSAGE_MACHINEW   0x0b  // �܂�BBS���V���m�F����   (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_USEPLUGIN  0x0c  // �V���m�F��Ƀ��[���m�F  (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_LISTUPDATE 0x0d  // �ꗗ�擾�ƃX���X�V      (�ꗗ��ʗp)
#define NNSH_SWITCHUSAGE_NOREADONLY 0x0e  // �擾�ςݑS�Ăɂ͎Q�ƃ��O�Ȃ�
#define NNSH_SWITCHUSAGE_NORDOLY_NW 0x0f  // ���ǂ���ɂ͎Q�ƃ��O�Ȃ�
#define NNSH_SWITCHUSAGE_DISCONN_NA 0x10  // �V���m�F��ɉ����ؒf����
#define NNSH_SWITCHUSAGE_PREEXEC_NA 0x11  // �V���m�F�O��DA�����s����
#define NNSH_SWITCHUSAGE_PR_NOTREAD 0x12  // �ꗗ�ł͖��ǐ���\������
#define NNSH_SWITCHUSAGE_GETLOGURL  0x13  // �ꗗ�ɕ\������͎̂Q�ƃ��O�ŁA�T�C�g��


// ���ړI�{�^���̏����ݒ�l
#define NNSH_SWITCH1USAGE_DEFAULT   NNSH_SWITCHUSAGE_GETPART    // �X���擾���[�h
#define NNSH_SWITCH2USAGE_DEFAULT   NNSH_SWITCHUSAGE_USEPROXY   // Proxy�o�R�ł̃A�N�Z�X

#define NNSH_SUBJSTATUSMSG_NOT_YET  "���擾"
#define NNSH_SUBJSTATUSMSG_NEW      "�V�K�擾"
#define NNSH_SUBJSTATUSMSG_UPDATE   "�X�V�擾"
#define NNSH_SUBJSTATUSMSG_REMAIN   "���ǂ���"
#define NNSH_SUBJSTATUSMSG_ALREADY  "����"
#define NNSH_SUBJSTATUSMSG_UNKNOWN  "��ԕs��"

/** �X�������� **/
#define MAX_SEARCH_STRING 64

#define NNSH_VIEW_SEARCHSTRING_HEADER  "ID:"  // �����p�����񒊏o�w�b�_

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
#define NNSH_FRMID_THREAD        0x1200    // �ꗗ���
#define NNSH_FRMID_MESSAGE       0x1201    // �Q�Ɖ��
#define NNSH_FRMID_DUMMY         0x1202    // �_�~�[���
#define NNSH_FRMID_WRITE_MESSAGE 0x1203    // �������݉��
#define NNSH_FRMID_GETLOGLIST    0x1204    // �Q�ƃ��O�ꗗ���


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
#define JOGBACK_FEATURE_DEFAULT      MULTIBTN_FEATURE_SELMENU
#define CAPTURE_FEATURE_DEFAULT      MULTIBTN_FEATURE_INFO

#define LEFTBTN_FEATURE_DEFAULT      MULTIBTN_FEATURE_PREVPAGE
#define RIGHTBTN_FEATURE_DEFAULT     MULTIBTN_FEATURE_NEXTPAGE

#define MULTIBTN_FEATURE_MASK        0x00ff
#define MULTIBTN_FEATURE             0x7600

// �ꗗ��ʂ̋@�\�ݒ�i�@�\�ԍ��j
//   �� �ݒ�p�^�C�g���́A���\�[�X�Œ�`(������Ɠ������Ă���K�v������)
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
#define MULTIBTN_FEATURE_COPYGIKO   17  // "�Q�ƃ��O��COPY"
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
#define MULTIBTN_FEATURE_NNSHSET    28  // "NNsi�ݒ�(�T��)"
#define MULTIBTN_FEATURE_NNSHSET1   29  // "NNsi�ݒ�-1"
#define MULTIBTN_FEATURE_NNSHSET2   30  // "NNsi�ݒ�-2"
#define MULTIBTN_FEATURE_NNSHSET3   31  // "NNsi�ݒ�-3"
#define MULTIBTN_FEATURE_NNSHSET4   32  // "NNsi�ݒ�-4"
#define MULTIBTN_FEATURE_NNSHSET5   33  // "NNsi�ݒ�-5"
#define MULTIBTN_FEATURE_NNSHSET6   34  // "NNsi�ݒ�-6"
#define MULTIBTN_FEATURE_NNSHSET7   35  // "NNsi�ݒ�-7"
#define MULTIBTN_FEATURE_NNSHSET8   36  // "NNsi�ݒ�-8"
#define MULTIBTN_FEATURE_NNSHSET9   37  // "NNsi�ݒ�-9"
#define MULTIBTN_FEATURE_VERSION    38  // "�o�[�W����"
#define MULTIBTN_FEATURE_SELECTNEXT 39  // "�P��"
#define MULTIBTN_FEATURE_SELECTPREV 40  // "�P��"
#define MULTIBTN_FEATURE_PREVPAGE   41  // "�O�y�[�W"
#define MULTIBTN_FEATURE_NEXTPAGE   42  // "��y�[�W"
#define MULTIBTN_FEATURE_NNSIEND    43  // "NNsi�I��"
#define MULTIBTN_FEATURE_SETNGWORD1 44  // "NG�ݒ�"
#define MULTIBTN_FEATURE_SETNGWORD2 45  // "NG�ݒ�"
#define MULTIBTN_FEATURE_USER1TAB   46  // "���[�U1�^�u"
#define MULTIBTN_FEATURE_USER2TAB   47  // "���[�U2�^�u"
#define MULTIBTN_FEATURE_DEVICEINFO 48  // "�f�o�C�X���"
#define MULTIBTN_FEATURE_GOTODUMMY  49  // "�_�~�[��ʂ�"
#define MULTIBTN_FEATURE_NEWTHREAD  50  // "�X������"
#define MULTIBTN_FEATURE_USER3TAB   51  // "���[�U3�^�u"
#define MULTIBTN_FEATURE_USER4TAB   52  // "���[�U4�^�u"
#define MULTIBTN_FEATURE_USER5TAB   53  // "���[�U5�^�u"
#define MULTIBTN_FEATURE_DIRSELECT  54  // "Dir�I��"
#define MULTIBTN_FEATURE_TABINFO    55  // "�^�u���"
#define MULTIBTN_FEATURE_MOVELOGLOC 56  // "���O�Ǘ��ύX"
#define MULTIBTN_FEATURE_MOVELOGDIR 57  // "Dir�ύX"
#define MULTIBTN_FEATURE_TO_NOTREAD 58  // "'���ǂ���'��"
#define MULTIBTN_FEATURE_TO_GETALL  59  // "'�擾�ςݑS��'��"
#define MULTIBTN_FEATURE_LOGCHARGE  60  // "�Q�ƃ��O�擾"
#define MULTIBTN_FEATURE_OPENURL    61  // "URL���擾"
#define MULTIBTN_FEATURE_SETNGWORD3 62  // "NG�ݒ�"
#define MULTIBTN_FEATURE_SETLOGCHRG 63  // "�Q�ƃ��O�擾�ݒ�"
#define MULTIBTN_FEATURE_BT_ON      64  // "Bt-On"
#define MULTIBTN_FEATURE_BT_OFF     65  // "Bt-Off"
#define MULTIBTN_FEATURE_TO_OFFLINE 66  // '�Q�ƃ��O'�� 
#define MULTIBTN_FEATURE_TO_FAVOR   67  // '���C�ɓ���'��
#define MULTIBTN_FEATURE_TO_USER1   68  // '���[�U1'��
#define MULTIBTN_FEATURE_TO_USER2   69  // '���[�U2'��
#define MULTIBTN_FEATURE_TO_USER3   70  // '���[�U3'��
#define MULTIBTN_FEATURE_TO_USER4   71  // '���[�U4'��
#define MULTIBTN_FEATURE_TO_USER5   72  // '���[�U5'��
#define MULTIBTN_FEATURE_MACRO_EXEC 73  // MACRO���s
#define MULTIBTN_FEATURE_MACRO_VIEW 74  // MACRO�\��
#define MULTIBTN_FEATURE_MACRO_LOG  75  // MACRO���O�\��
#define MULTIBTN_FEATURE_MACRO_SET  76  // MACRO�ݒ�
#define MULTIBTN_FEATURE_SELECT_BBS 77  // ��I��
#define MULTIBTN_FEATURE_OYSTERLOGIN 78 // '�����O�C��'
#define MULTIBTN_FEATURE_USAGE      79  // ����w���v
#define MULTIBTN_FEATURE_SETTOKEN   80  // ��؂�ݒ�
#define MULTIBTN_FEATURE_OPENGETLOGLIST 81 // �Q�ƃ��O�擾�ꗗ��ʂ�
#define MULTIBTN_FEATURE_SEARCHBBS  82     // ����������
#define MULTIBTN_FEATURE_NEXTSEARCHBBS  83 // ������������
#define MULTIBTN_FEATURE_BEAMURL    84     // URL��Beam����
#define MULTIBTN_FEATURE_UPDATERESNUM 85   // �擾�ς݃��X�ԍ����X�V����
#define MULTIBTN_FEATURE_OPENWEBDA  86     // webDA�ŊJ��
#define MULTIBTN_FEATURE_SELECT_MACRO_EXEC 87  // �}�N���I�����s
#define MULTIBTN_FEATURE_BE_LOGIN   88     // Be@2ch���O�C��
#define MULTIBTN_FEATURE_LIST_GETLOG 89    // �X���ԗ\��ꗗ

#define HARDKEY_FEATURE_MASK         0x00ff
#define HARDKEY_FEATURE              0x0000

// �n�[�h�L�[�̐���ݒ�
#define HARDKEY_FEATURE_DONOTHING    0  // NNsi�Ő��䂵�Ȃ�
#define HARDKEY_FEATURE_CONTROLNNSI  1  // NNsi�Ő��䂷��
#define HARDKEY_FEATURE_TRANSJOGPUSH 2  // JOG PUSH�Ɠ����ɂ���
#define HARDKEY_FEATURE_TRANSJOGBACK 3  // JOG BACK�Ɠ����ɂ���

#define NNSH_BLOCKDISP_NOFMSG        5

#define NNSH_VFSDB_SETTING           0
#define NNSH_VFSDB_SUBJECT           1
#define NNSH_VFSDB_BBSLIST           2

#define NNSH_CONDITION_AND           0x0000  // AND����
#define NNSH_CONDITION_OR            0x0001  // OR����
#define NNSH_CONDITION_ALL           0x0003  // (�S����)

#define NNSH_CONDITION_NOTREADONLY   0x0004  // �Q�ƃ��O�͏��O(�r�b�g)

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

#define NNSH_LEVEL_ERROR            0x0000
#define NNSH_LEVEL_WARN             0x0001
#define NNSH_LEVEL_INFO             0x0002
#define NNSH_LEVEL_CONFIRM          0x0004
#define NNSH_LEVEL_DEBUG            0x0010

#define NNSH_TITLE_HARDKEYSET_TITLE  "�ꗗ��ʃn�[�h�L�[�ݒ�"
#define NNSH_HARDKEYCONTROL_TITLE    "�n�[�h�L�[����ݒ�"
#define NNSH_TITLE_HARDKEYVIEW_TITLE "�Q�Ɖ�ʃn�[�h�L�[�ݒ�"
#define NNSH_TITLE_GETLOGLIST_TITLE  "�Q�ƃ��O�ꗗ��ʃn�[�h�L�[�ݒ�"

#define MULTIVIEWBTN_FEATURE_MASK    0x00ff
#define MULTIVIEWBTN_FEATURE         0x7300

#define GETLOGBTN_FEATURE_MASK       0x00ff
#define GETLOGBTN_FEATURE            0x0000

#define UPBTN_VIEWFEATURE_DEFAULT    MULTIVIEWBTN_FEATURE_PAGEUP
#define DOWNBTN_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_PAGEDOWN

#define JOGPUSH_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_GETPART
#define JOGBACK_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_BACKTOLIST
#define CAPTURE_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_INFO

#define LEFTBTN_VIEWFEATURE_DEFAULT  MULTIVIEWBTN_FEATURE_PREVIOUS
#define RIGHTBTN_VIEWFEATURE_DEFAULT MULTIVIEWBTN_FEATURE_NEXT

#define NNSH_DEFAULT_VIEW_LVLBTNFEATURE   MULTIVIEWBTN_FEATURE_INFO
#define NNSH_DEFAULT_VIEW_TITLESELFEATURE MULTIVIEWBTN_FEATURE_BACKTOLIST


// �Q�Ɖ�ʂ̋@�\�ݒ�i�@�\�ԍ��j
//   �� �ݒ�p�^�C�g���́A���\�[�X�Œ�`(������Ɠ������Ă���K�v������)
#define MULTIVIEWBTN_FEATURE_INFO        0  // "�X�����"
#define MULTIVIEWBTN_FEATURE_BACKTOLIST  1  // "�ꗗ��ʂ�"
#define MULTIVIEWBTN_FEATURE_WRITE       2  // "��������"
#define MULTIVIEWBTN_FEATURE_GETPART     3  // "�����擾"
#define MULTIVIEWBTN_FEATURE_GETMESNUM   4  // "�X���Ԏw��擾"
#define MULTIVIEWBTN_FEATURE_GOTOTOP     5  // "�擪MSG��"
#define MULTIVIEWBTN_FEATURE_PREVIOUS    6  // "�OMSG��"
#define MULTIVIEWBTN_FEATURE_PAGEUP      7  // "�O�X�N���[��"
#define MULTIVIEWBTN_FEATURE_PAGEDOWN    8  // "��X�N���[��"
#define MULTIVIEWBTN_FEATURE_NEXT        9  // "��MSG��"
#define MULTIVIEWBTN_FEATURE_GOTOBOTTOM 10  // "����MSG��"
#define MULTIVIEWBTN_FEATURE_JUMP       11  // "Go"
#define MULTIVIEWBTN_FEATURE_RETURN     12  // "�P�߂�"
#define MULTIVIEWBTN_FEATURE_COPY       13  // "������R�s�["
#define MULTIVIEWBTN_FEATURE_SELECTALL  14  // "�����񂷂ׂđI��"
#define MULTIVIEWBTN_FEATURE_OPENWEB    15  // "�������Web"
#define MULTIVIEWBTN_FEATURE_SELANDWEB  16  // "�I��+Web"
#define MULTIVIEWBTN_FEATURE_OUTPUTMEMO 17  // "�������o��"
#define MULTIVIEWBTN_FEATURE_SEARCH     18  // "����"
#define MULTIVIEWBTN_FEATURE_SEARCHNEXT 19  // "������"
#define MULTIVIEWBTN_FEATURE_SEARCHPREV 20  // "�O����"
#define MULTIVIEWBTN_FEATURE_SEARCHTOP  21  // "�擪���猟��"
#define MULTIVIEWBTN_FEATURE_SEARCHBTM  22  // "�������猟��"
#define MULTIVIEWBTN_FEATURE_CHANGEFONT 23  // "�t�H���g�ύX"
#define MULTIVIEWBTN_FEATURE_DRAWMODE   24  // "�`�惂�[�h�ύX"
#define MULTIVIEWBTN_FEATURE_ROTATE     25  // "��ʂ̉�](HE)"
#define MULTIVIEWBTN_FEATURE_DISCONNECT 26  // "����ؒf"
#define MULTIVIEWBTN_FEATURE_SETNETWORK 27  // "Network�ݒ�"
#define MULTIVIEWBTN_FEATURE_BOOKMARK   28  // "������ݒ�"
#define MULTIVIEWBTN_FEATURE_DEVICEINFO 29  // "�f�o�C�X���"
#define MULTIVIEWBTN_FEATURE_PAGEDOWN10 30  // "10���X��"
#define MULTIVIEWBTN_FEATURE_PAGEUP10   31  // "10���X�O"
#define MULTIVIEWBTN_FEATURE_GOTODUMMY  32  // "�_�~�[��ʂ�"
#define MULTIVIEWBTN_FEATURE_OPENURL    33  // "�����N���J��"
#define MULTIVIEWBTN_FEATURE_EXECA2DA   34  // "A2DA�N��"
#define MULTIVIEWBTN_FEATURE_BT_ON      35  // "Bt On"
#define MULTIVIEWBTN_FEATURE_BT_OFF     36  // "Bt Off"
#define MULTIVIEWBTN_FEATURE_USAGE      37  // "����w���v"
#define MULTIVIEWBTN_FEATURE_MEMOURL    38  // "MemoURL�֑���"
#define MULTIVIEWBTN_FEATURE_CHANGERAW  39  // "RAW���[�h�ؑ�"
#define MULTIVIEWBTN_FEATURE_CHGSEPA    40  // "��؂蕶���ύX"
#define MULTIVIEWBTN_FEATURE_EDITSEPA   41  // "��؂蕶���ҏW"
#define MULTIVIEWBTN_FEATURE_OPENURL_HTTP 42 // "URL���J��(MSG���璊�o)"
#define MULTIVIEWBTN_FEATURE_MEMOURLDA  43  // "MemoURL��(DA�o�R��)����"
#define MULTIVIEWBTN_FEATURE_OPENWEBDA  44  // "webDA�Ō���"
#define MULTIVIEWBTN_FEATURE_GETIMAGE   45  // "�C���[�W(�t�@�C��)���擾"
#define MULTIVIEWBTN_FEATURE_DELETEBACK 46  // "�폜��ꗗ�֖߂�"
#define MULTIVIEWBTN_FEATURE_VIEWBEPROF 47  //�@"Be�v���t�Q��"
#define MULTIVIEWBTN_FEATURE_LVLUPDATE  48  //  "�X�����x���X�V"
#define MULTIVIEWBTN_FEATURE_PICKUP_NG  49  //  "NG�o�^"
#define MULTIVIEWBTN_FEATURE_ENTRY_NG   50  //  "NG�ݒ�"
#define MULTIVIEWBTN_FEATURE_REPLYMSG   51  //  "���p�J�L�R"

// Notify�̍X�V����
#define NNSH_VFS_UNMOUNTED              100
#define NNSH_VFS_UNMOUNTED_WRITE        101
#define NNSH_VFS_UNMOUNTED_MESSAGE      102
#define NNSH_VFS_UNMOUNTED_OTHER        103
#define NNSH_VFS_MOUNTED                110

#define NNSH_DISP_RESIZED               200


// NG�`�F�b�N���g�p����E���Ȃ�
#define NNSH_USE_NGWORD1AND2            0x0001
#define NNSH_USE_NGWORD3                0x0010
#define NNSH_USE_NGWORD_EACHTHREAD      0x0020
#define NNSH_RESETMASK_NGWORD3          0xffef
#define NNSH_USE_NGWORD_CLEAREACHTHREAD 0xffdf

//
#define NNSJ_NNSIEXT_SELECTMACRO  "<NNsi:NNsiExt type=\"SelectMacro\"/>"

// a2DA�Ƃ̘A�g�@�\�p��`
#define A2DA_NNSIEXT_AADATASTART  "<NNsi:NNsiExt type=\"AA\">"
#define A2DA_NNSIEXT_ENDDATA      "</NNsi:NNsiExt>"
#define A2DA_NNSIEXT_SRC_STREAM   "<NNsi:Source>stream</NNsi:Source>"
#define A2DA_NNSIEXT_DATANAME     "<NNsi:Name>"
#define A2DA_NNSIEXT_ENDDATANAME  "</NNsi:Name>"
#define A2DA_NNSIEXT_NNSICREATOR  "<NNsi:Creator>NNsi</NNsi:Creator>"

#define nnDA_NNSIEXT_VIEWSTART    "<NNsi:NNsiExt type=\"VIEW\">"
#define nnDA_NNSIEXT_ENDVIEW      "</NNsi:NNsiExt>"
#define nnDA_NNSIEXT_INFONAME     "<NNsi:Info>"
#define nnDA_NNSIEXT_ENDINFONAME  "</NNsi:Info>"

#define nnDA_NNSIEXT_FILE         "FILE:"
#define nnDA_NNSIEXT_HELPLIST     "HELP:LIST"
#define nnDA_NNSIEXT_HELPVIEW     "HELP:VIEW"
#define nnDA_NNSIEXT_HELPFAVORTAB "HELP:FAVORTAB"
#define nnDA_NNSIEXT_HELPNNSISET  "HELP:NNSISET"
#define nnDA_NNSIEXT_HELPGETLOG   "HELP:GETLOG"
#define nnDA_NNSIEXT_FILERECVFILE "FILE:NNsi:recv.txt"
#define nnDA_NNSIEXT_FILESENDFILE "FILE:NNsi:send.txt"
#define nnDA_NNSIEXT_VIEWMEM      "MEM:"
#define nnDA_NNSIEXT_VIEWMEM_FOLD "FOLDMEM:"
#define nnDA_NNSIEXT_FOLDHTMLFILE "FOLDHTMLFILE:NNsi:"
#define nnDA_NNSIEXT_SHOWJPEG     "SHOWJPEG:"
#define nnDA_NNSIEXT_VFSFILE      "VFSFILE:"
#define nnDA_NNSIEXT_VFSFILE_FOLD "FOLDVFS:"

// NNsi�ԒʐM�p��`
#define NNSI_EXCHANGEINFO_SUFFIX  "nsi"

// �X���ԍ�
#define NNSI_NNSIEXT_THREADNUMBER_START "<NNsi:NNsiExt type=\"THREAD\">"
#define NNSI_NNSIEXT_THREADNUMBER_END   "</NNsi:NNsiExt>"

// URL
#define NNSI_NNSIEXT_URL_START          "<NNsi:NNsiExt type=\"URL\">"
#define NNSI_NNSIEXT_URL_END            "</NNsi:NNsiExt>"


// �Q�ƃ��O�ꗗ��ʂ̋@�\�ꗗ
#define NNSH_GETLOG_FEATURE_NONE           0xffff   // �@�\�Ȃ�
#define NNSH_GETLOG_FEATURE_CLOSE_GETLOG        0   // �ꗗ��ʂ�
#define NNSH_GETLOG_FEATURE_CHECK_NEWARRIVAL    1   // �V���m�F
#define NNSH_GETLOG_FEATURE_EDITITEM            2   // �T�C�g�ڍ�
#define NNSH_GETLOG_FEATURE_PREVITEM            3   // �P��
#define NNSH_GETLOG_FEATURE_NEXTITEM            4   // �P��
#define NNSH_GETLOG_FEATURE_PREVPAGE            5   // �O�y�[�W
#define NNSH_GETLOG_FEATURE_NEXTPAGE            6   // ��y�[�W
#define NNSH_GETLOG_FEATURE_TOP                 7   // �擪
#define NNSH_GETLOG_FEATURE_BOTTOM              8   // ����
#define NNSH_GETLOG_FEATURE_CREATELIST          9   // �ꗗ�č\�z
#define NNSH_GETLOG_FEATURE_EDIT_SEPARATOR     10   // ��ؕҏW
#define NNSH_GETLOG_FEATURE_ENTRY_NEWSITE      11   // �T�C�g�o�^
#define NNSH_GETLOG_FEATURE_OPEN_GETLEVEL      12   // Level�ύX
#define NNSH_GETLOG_FEATURE_REDRAW             13   // �ĕ`��
#define NNSH_GETLOG_FEATURE_OPENWEB            14   // �u���E�U�ŊJ��
#define NNSH_GETLOG_FEATURE_BT_ON              15   // Bt On
#define NNSH_GETLOG_FEATURE_BT_OFF             16   // Bt Off
#define NNSH_GETLOG_FEATURE_DISCONNECT         17   // ����ؒf
#define NNSH_GETLOG_FEATURE_NETCONFIG          18   // Network�ݒ�
#define NNSH_GETLOG_FEATURE_DEVICEINFO         19   // �f�o�C�X���
#define NNSH_GETLOG_FEATURE_OPEN_DUMMY         20   // �_�~�[��ʂ�
#define NNSH_GETLOG_FEATURE_OPEN_NNSISET       21   // NNsi�ݒ��ʂ�
#define NNSH_GETLOG_FEATURE_SHOWVERSION        22   // �o�[�W�������
#define NNSH_GETLOG_FEATURE_HELP               23   // �������
#define NNSH_GETLOG_FEATURE_NNSIEND            24   // NNsi�I��
#define NNSH_GETLOG_FEATURE_OPENMENU           25   // �I�����j���[
#define NNSH_GETLOG_FEATURE_OPEN_NNSI          26   // NNsi�ŊJ��
#define NNSH_GETLOG_FEATURE_OPEN_MEMOURL       27   // MemoURL�ŊJ��
#define NNSH_GETLOG_FEATURE_OPEN_GETLOGMENU    28   // ���j���[���J��
#define NNSH_GETLOG_FEATURE_FORCE_DISCONNECT   29   // ����ؒf-2
#define NNSH_GETLOG_FEATURE_CONFIG_GETLOG      30   // �Q�ƃ��O�ꗗ�ݒ�
#define NNSH_GETLOG_FEATURE_SITE_INFORMATION   31   // �T�C�g���
#define NNSH_GETLOG_FEATURE_BEAM_URL           32   // URL��BEAM����
#define NNSH_GETLOG_FEATURE_OPENWEBDA          33   // URL��webDA�ŊJ��
#define NNSH_GETLOG_FEATURE_DELETELOG          34   // ���O���ꊇ�폜
#define NNSH_GETLOG_FEATURE_DELETELOG_READ     35   // ���ǃ��O���ꊇ�폜

#define NNSH_GETLOG_NEWARRIVAL_LOG         0x8000   // ���ǃ��O����
#define NNSH_GETLOG_NEWARRIVAL_CLEAR_MASK  0x7fff   // ���ǃ��O��OFF

// SSL���[�h��`
#define NNSH_SSLMODE_NOTSSL             0
#define NNSH_SSLMODE_SSLV3              1
#define NNSH_SSLMODE_SSL_V2SPECIAL      2

#ifdef USE_SSL
#ifdef USE_SSL_V2

//
#define NNSHSSL_STATE_MASK             0x0FFF
#define NNSHSSL_STATE_CONNECT          0x1000
#define NNSHSSL_STATE_ACCEPT           0x2000
#define NNSHSSL_STATE_INIT        (NNSHSSL_STATE_CONNECT|NNSHSSL_STATE_ACCEPT)
#define NNSHSSL_STATE_BEFORE           0x4000

#define NNSHSSL_STATE_OK               0x03
#define NNSHSSL_STATE_RENEGOTIATE     (0x04|NNSHSSL_STATE_INIT)

/* Protocol Version Codes */
#define SSL2_VERSION                   0x0002
#define SSL2_VERSION_MAJOR             0x00
#define SSL2_VERSION_MINOR             0x02
/* #define SSL2_CLIENT_VERSION          0x0002 */
/* #define SSL2_SERVER_VERSION          0x0002 */

#define NNSHSSL2_MAX_RECORD_LENGTH_3_BYTE_HEADER  16383 /* 2^14-1 */

/* SSLv2 */
/* client */
#define SSL2_ST_SEND_CLIENT_HELLO            (0x10|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_GET_SERVER_HELLO             (0x20|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_SEND_CLIENT_MASTER_KEY       (0x30|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_CREATE_ENCRYPTION_KEY        (0x31|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_SEND_CLIENT_FINISHED         (0x40|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_GET_SERVER_VERIFY            (0x60|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_GET_SERVER_FINISHED          (0x70|NNSHSSL_STATE_CONNECT)
#define SSL2_ST_START_ENCRYPTION             (0x80|NNSHSSL_STATE_CONNECT)

/* Protocol Message Codes */
#define SSL2_MT_ERROR                         0
#define SSL2_MT_CLIENT_HELLO                  1
#define SSL2_MT_CLIENT_MASTER_KEY             2
#define SSL2_MT_CLIENT_FINISHED               3
#define SSL2_MT_SERVER_HELLO                  4
#define SSL2_MT_SERVER_VERIFY                 5
#define SSL2_MT_SERVER_FINISHED               6
#define SSL2_MT_REQUEST_CERTIFICATE           7
#define SSL2_MT_CLIENT_CERTIFICATE            8

//#define SSL2_CHALLENGE_LENGTH                32
#define SSL2_CHALLENGE_LENGTH                16
#define SSL2_MD5_HASH_LENGTH                 16
#define SSL2_RWKEY_LENGTH                    SSL2_MD5_HASH_LENGTH
#define SSL2_MASTERKEY_LENGTH                16
// #define SSL2_MASTERKEY_LENGTH             128
// #define SSL2_SSL_SESSION_ID_LENGTH         16
#define SSL2_MAX_SSL_SESSION_ID_LENGTH       32
#endif  // #ifdef USE_SSL_V2
#endif  // #ifdef USE_SSL

#define NNSH_GETLOGMSG_VIEWTITLE "�X���ԍ��w��擾�ꗗ"
#define NNSH_GETLOGMSG_NONE      "(�Ȃ�)"

#ifdef USE_MACRO

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
#define NNSH_MACROCMD_UPDATE_RESNUM  31 // �X���̃��X�ԍ����X�V����(1.03)
#define NNSH_MACROCMD_JPL       32   // �l�����̏ꍇ�ɂ̓W�����v(1.03)
#define NNSH_MACROCMD_JMI       33   // �l�����̏ꍇ�ɂ̓W�����v(1.03)
#define NNSH_MACROCMD_OUTPUTTITLE 34 // �X���^�C�g�������O�o��(1.03)
#define NNSH_MACROCMD_OUTPUTCLIP 35  // �N���b�v�{�[�h�̓��e�����O�o��(1.03)
#define NNSH_MACROCMD_REMARK    36   // ���ߕ�(�������Ȃ�) (1.03)
#define NNSH_MACROCMD_PUSH      37   // �X�^�b�N��PUSH(1.04)
#define NNSH_MACROCMD_POP       38   // �X�^�b�N��POP(1.04)
#define NNSH_MACROCMD_GETRECNUM 39   // ���R�[�h�ԍ����擾(1.04)
#define NNSH_MACROCMD_FINDTHREADSTRCOPY 40 // �X��������������N���b�v�{�[�h��(1.04)
#define NNSH_MACROCMD_INPUTDIALOG  41      // ���������́A�N���b�v�{�[�h�փR�s�[ (1.05)
#define NNSH_MACROCMD_OUTPUTDIALOG  42     // ��������o�͂��� (1.05)
#define NNSH_MACROCMD_CONFIRMDIALOG 43     // �m�F�_�C�A���O��\������ (1.05)
#define NNSH_MACROCMD_MESATTRIBUTE  44     // �X���̑������� (1.06)
#define NNSH_MACROCMD_JUMPLAST      45     // �X���̍ŉ��s�̏ꍇ�ɂ̓W�����v���� (1.06)


// �f�[�^�̑���֘A�w��(NNsi�ݒ�̕ύX�Ƃ�)
#define NNSH_MACROOPCMD_DISABLE            0xffff  // (�}�N������Ȃ�)
#define NNSH_MACROOPCMD_AUTOSTARTMACRO     0xfffe  // (�}�N���������sSET)
#define NNSH_MACROOPCMD_NEWARRIVAL_LINE    0xfffd  // (�V���m�F��̉���ؒf)
#define NNSH_MACROOPCMD_NEWARRIVAL_DA      0xfffc  // (�V���m�F���DA�N��)
#define NNSH_MACROOPCMD_NEWARRIVAL_ROLOG   0xfffb  // (�Q�ƃ��O�擾���x��)
#define NNSH_MACROOPCMD_NEWARRIVAL_BEEP    0xfffa  // (�V���m�F��̃r�[�v��)
#define NNSH_MACROOPCMD_COPY_AND_DELETE    0xfff9  // (�Q�ƃ��O�ɃR�s�[���폜��)

#define NNSH_MACROCMD_VIEWTITLE  "�}�N�����e"
#define NNSH_MACROLOG_VIEWTITLE  "�}�N�����s����"

#define MAX_HALT_OPERATION     100   // �@�\���s�̐���
#define MAX_LOGVIEW_BUFFER      80   //
#define MAX_LOGOUTPUT_BUFFER    72   //
#define MAX_FUNCCHAR_BUF        20   //
#define TEXTBUFSIZE            256

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

#endif  // #ifdef USE_MACRO

#define  ICMP_ECHOREPLY       0
#define  ICMP_DEST_UNREACH    3
#define  ICMP_SOURCE_QUENCH   4
#define  ICMP_REDIRECT        5
#define  ICMP_ECHO            8
#define  ICMP_TIME_EXCEEDED  11
#define  ICMP_PARAMETERPROB  12
#define  ICMP_TIMESTAMP      13
#define  ICMP_TIMESTAMPREPLY 14
#define  ICMP_INFO_REQUEST   15
#define  ICMP_INFO_REPLY     16
#define  ICMP_MASKREQ        17
#define  ICMP_MASKREPLY      18
 
#define  ICMP_NET_UNREACH     0
#define  ICMP_HOST_UNREACH    1
#define  ICMP_PROT_UNREACH    2
#define  ICMP_PORT_UNREACH    3
#define  ICMP_FRAG_NEEDED     4
#define  ICMP_SR_FAILED       5
#define  ICMP_NET_UNKNOWN     6
#define  ICMP_HOST_UNKNOWN    7
#define  ICMP_HOST_ISOLATED   8
#define  ICMP_NET_UNR_TOS    11
#define  ICMP_HOST_UNR_TOS   12
#define  ICMP_PKT_FILTERED   13
#define  ICMP_PREC_VIOLATION 14
#define  ICMP_PREC_CUTOFF    15
