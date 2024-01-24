/*============================================================================*
 *
 *  $Id: sslv2rc4.c,v 1.3 2004/02/07 12:16:55 mrsa Exp $
 *  FILE: 
 *     sslv2rc4.c
 *
 *  Description: 
 *     SSL v2�ʐM�pRC4, RSA�Í����v���O���� (OpenSSL���)
 *
 *===========================================================================*/
#define SSLV2RC4_C
#include "local.h"

#ifdef USE_SSL
#ifdef USE_SSL_V2

/*=========================================================================*/
/* RC4 as implemented from a posting from
 * Newsgroups: sci.crypt
 * From: sterndark@netcom.com (David Sterndark)
 * Subject: RC4 Algorithm revealed.
 * Message-ID: <sternCvKL4B.Hyy@netcom.com>
 * Date: Wed, 14 Sep 1994 06:35:31 GMT
 */
/*=========================================================================*/
/*** OpenSSL�ɕt������RC4�R�[�h ***/
void RC4_SetKey(RC4_KEY *key, UInt16 len, UInt8 *data)
{
    Int32  tmp, id1, id2, i, *d;

    d = &(key->data[0]);
    for (i = 0; i < 256; i++)
    {
        d[i] = i;
    }
    key->x = 0;     
    key->y = 0;     
    id1 = 0;
    id2 = 0; 

#define SK_LOOP(n) {                         \
        tmp = d[(n)];                         \
        id2 = (data[id1] + tmp + id2) & 0xff; \
        if (++id1 == len)                     \
        {                                     \
            id1 = 0;                          \
        }                                     \
        d[(n)] = d[id2];                      \
        d[id2] = tmp;                         \
    }

    for (i = 0; i < 256; i += 4)
    {
        SK_LOOP(i + 0);
        SK_LOOP(i + 1);
        SK_LOOP(i + 2);
        SK_LOOP(i + 3);
    }
    return;
}

/*=========================================================================*/
/* RC4 as implemented from a posting from
 * Newsgroups: sci.crypt
 * From: sterndark@netcom.com (David Sterndark)
 * Subject: RC4 Algorithm revealed.
 * Message-ID: <sternCvKL4B.Hyy@netcom.com>
 * Date: Wed, 14 Sep 1994 06:35:31 GMT
 */
/*=========================================================================*/
/*** OpenSSL�ɕt������RC4�R�[�h ***/
void RC4_Calculate(RC4_KEY *key, UInt32 len, Char *indata, Char *outdata)
{
    Int32 *d, x, y, tx, ty, i;
        
    x = key->x;     
    y = key->y;     
    d = key->data; 

#define LOOP(in,out) {                    \
        x     = ((x+1) & 0xff);            \
        tx    = d[x];                      \
        y     = (tx + y) & 0xff;           \
        d[x]  = ty = d[y];                 \
        d[y]  = tx;                        \
        (out) = d[(tx + ty) & 0xff]^ (in); \
    }

#ifndef RC4_INDEX
    #define RC4_LOOP(a,b,i) LOOP(*((a)++),*((b)++))
#else
    #define RC4_LOOP(a,b,i) LOOP(a[i],b[i])
#endif

    i = (Int32) (len >> 3L);
    if (i)
    {
        for (;;)
        {
            RC4_LOOP(indata, outdata, 0);
            RC4_LOOP(indata, outdata, 1);
            RC4_LOOP(indata, outdata, 2);
            RC4_LOOP(indata, outdata, 3);
            RC4_LOOP(indata, outdata, 4);
            RC4_LOOP(indata, outdata, 5);
            RC4_LOOP(indata, outdata, 6);
            RC4_LOOP(indata, outdata, 7);
#ifdef RC4_INDEX
            indata  += 8;
            outdata += 8;
#endif
            if (--i == 0) break;
        }
    }
    i = (Int32) len & 0x07;
    if (i)
    {
        for (;;)
        {
            RC4_LOOP(indata, outdata, 0); if (--i == 0) break;
            RC4_LOOP(indata, outdata, 1); if (--i == 0) break;
            RC4_LOOP(indata, outdata, 2); if (--i == 0) break;
            RC4_LOOP(indata, outdata, 3); if (--i == 0) break;
            RC4_LOOP(indata, outdata, 4); if (--i == 0) break;
            RC4_LOOP(indata, outdata, 5); if (--i == 0) break;
            RC4_LOOP(indata, outdata, 6); if (--i == 0) break;
        }
    }               
    key->x = x;     
    key->y = y;

    return;
}

/** ���쌠�\��(OpenSSL/SSLeay) **/
/* Copyright (C) 1995-1998 Eric Young (eay@cryptsoft.com)
 * All rights reserved.
 *
 * This package is an SSL implementation written
 * by Eric Young (eay@cryptsoft.com).
 * The implementation was written so as to conform with Netscapes SSL.
 * 
 * This library is free for commercial and non-commercial use as long as
 * the following conditions are aheared to.  The following conditions
 * apply to all code found in this distribution, be it the RC4, RSA,
 * lhash, DES, etc., code; not just the SSL code.  The SSL documentation
 * included with this distribution is covered by the same copyright terms
 * except that the holder is Tim Hudson (tjh@cryptsoft.com).
 * 
 * Copyright remains Eric Young's, and as such any Copyright notices in
 * the code are not to be removed.
 * If this package is used in a product, Eric Young should be given attribution
 * as the author of the parts of the library used.
 * This can be in the form of a textual message at program startup or
 * in documentation (online or textual) provided with the package.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    "This product includes cryptographic software written by
 *     Eric Young (eay@cryptsoft.com)"
 *    The word 'cryptographic' can be left out if the rouines from the library
 *    being used are not cryptographic related :-).
 * 4. If you include any Windows specific code (or a derivative thereof) from 
 *    the apps directory (application code) you must include an acknowledgement:
 *    "This product includes software written by Tim Hudson (tjh@cryptsoft.com)"
 * 
 * THIS SOFTWARE IS PROVIDED BY ERIC YOUNG ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * The licence and distribution terms for any publically available version or
 * derivative of this code cannot be changed.  i.e. this code cannot simply be
 * copied and put under another distribution licence
 * [including the GNU Public Licence.]
 */
#endif // #ifdef USE_SSL_V2
#endif // #ifdef USE_SSL
