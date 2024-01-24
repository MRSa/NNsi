/*============================================================================*
 *  $Id: struct.h,v 1.2 2004/09/13 12:10:28 mrsa Exp $
 *
 *  FILE: 
 *     struct.h
 *
 *  Description: 
 *     structure definitions for Palm application.
 *
 *===========================================================================*/

/********** My Local structures **********/

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

//  �S�̂Ŏg�p����O���[�o���̈�
typedef struct
{
    FormType     *currentForm;
    FormType     *previousForm;
    FormType     *prevBusyForm;
    UInt16        netRef;
    UInt16        err;
    UInt16        ret;
    UInt16        loopCount;
    NetSocketRef  socketRef;
    UInt16        sslRef;
    SslLib       *sslTemplate;
    SslContext   *sslContext;
    Char         *topPtr;
    Char         *tempPtr;
    Char         *tempPtr2;
    Char         *tempPtr3;
    Char         *sendBuf;
    Char         *recvBuf;
    Char         *workBuf;
    UInt16        sendMethod;
    UInt32        palmOSVersion;
    Char          reqURL    [BUFSIZE * 2];
    Char          reqAddress[BUFSIZE * 2];
#ifdef USE_REFERER
    Char          referer   [BUFSIZE];
#endif
    Char          tempBuf   [BUFSIZE];
    Char          tempBuf2  [BUFSIZE];
    Char          showBuf   [MINIBUF + MARGIN];
    Char          cookie    [BUFSIZE + MARGIN];
    Char          hostName  [BUFSIZE];       // �z�X�g��
    UInt16        portNum;                   // �ʐM�|�[�g�ԍ�
    Int32         timeout;                   // �ʐM�^�C���A�E�g�l
    UInt16        isChecking;                // �v���A�h���X
    UInt16        kanjiCode;                 // �\�������R�[�h
    UInt16        fieldLen;                  // �\���f�[�^�T�C�Y
    MemHandle     memH;
    // UInt32     bufferSize;
#ifdef USE_WIDEWINDOW
    Coord         x;
    Coord         y;
    Coord         diffX;
    Coord         diffY;
    RectangleType r;
    UInt32        silkVer;
#endif
} NNshWorkingInfo;
