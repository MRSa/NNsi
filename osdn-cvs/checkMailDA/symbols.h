/*============================================================================*
 *  $Id: symbols.h,v 1.3 2004/05/08 15:31:16 mrsa Exp $
 *
 *  FILE: 
 *     symbols.h
 *
 *  Description: 
 *     symbol definitions for Palm application.
 *
 *===========================================================================*/

/***  �f�[�^�x�[�X�ݒ�  **/
#define SOFT_CREATOR_ID          'cMDA'
#define SOFT_DBTYPE_ID           'Data'
#define DBNAME_SETTING        "cMDA-DB"
#define DBVERSION_SETTING           100
#define DBSIZE_SETTING               32
#define DB_RESERVE_AREA             256

/** ���b�Z�[�W�w�b�_�̎擾��  **/
#define GET_MESSAGE_HEADER_NUM        3

/***  �o�b�t�@�T�C�Y ***/
#define MINIBUF                      48
#define BUFSIZE                     128
#define MARGIN                        4
#define WORKBUF_DEFAULT            4096
#define BUSYWIN_MAXLEN               32
#define DIALOG_BUFFER              1024

/** �f�[�^�x�[�X�̃L�[�^�C�v(������������) **/
#define NNSH_KEYTYPE_CHAR             0
#define NNSH_KEYTYPE_UINT32           1
#define NNSH_KEYTYPE_UINT16           2

/** �ʐM�v���g�R���̎w�� **/
#define CHECKPROTOCOL_APOP            0
#define CHECKPROTOCOL_POP3            1

/** �^�C���A�E�g���Ԃ̐ݒ� **/
#define NNSH_GETMAIL_TIMEOUT         15

/** �G���[�R�[�h **/
#define NNSH_ERRORCODE_FAILURECONNECT (~errNone -  5)

/** ���b�Z�[�W�\�� **/
#define MSG_ERROR                     "�G���[���������܂����B\n(�ݒ���m�F���Ă��������B)"
#define MSG_NOMAIL                    "���[���͂���܂���B"
#define MSG_MAILARRIVAL               "�ʂ̃��[��������܂��B"
#define MSG_DISCONNECT                "����ؒf��..."
#define MSG_PROCESS_GETHEADER_SEND    "Sending mail header..."
#define MSG_PROCESS_GETHEADER_RECV    "Receiving mail header..."
#define MSG_PROCESS_PARSING           "Parsing mail header..."


//////////////////////////////////////////////////////////////////////////////
//   RFC1321���A�A�A���̂܂�܂�����Ă��܂���(w�B
//////////////////////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------
   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. 
  All rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 ---------------------------------------------------------------------------*/

/*
 * MD5.H - header file for MD5C.C
 */

/* MD5 context. */
typedef struct {
  UInt32 state [4];                                   /* state (ABCD) */
  UInt32 count [2];        /* number of bits, modulo 2^64 (lsb first) */
  UInt8  buffer[64];                                  /* input buffer */
} MD5_CTX;


// from SonyChars.h
#ifndef vchrJogPushedDown
  #define vchrJogPushedDown     (0x1704)
#endif
#ifndef vchrJogPushedUp
  #define vchrJogPushedUp       (0x1703)
#endif
#ifndef vchrJogUp
  #define vchrJogUp             (0x1700)
#endif
#ifndef vchrJogDown
  #define vchrJogDown           (0x1701)
#endif
#ifndef vchrJogPush
  #define vchrJogPush           (0x1705)
#endif
#ifndef vchrJogRelease
  #define vchrJogRelease        (0x1706)
#endif
#ifndef vchrJogBack
  #define vchrJogBack           (0x1707)
#endif

#ifndef vchrJogLeft
  #define vchrJogLeft           (0x1708)
#endif

#ifndef vchrJogRight
  #define vchrJogRight          (0x1709)
#endif
