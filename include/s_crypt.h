/************************************************************************
 *   IRC - Internet Relay Chat, include/s_crypt.h
 *   Copyright (C) 2001 einride <einride@einride.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * "s_crypt.h". - Headers file.
 *
 *
 */
#ifndef INCLUDED_s_crypt_h
#define INCLUDED_s_crypt_h

#include "config.h"

#ifdef CRYPT_LINKS

#ifndef HAVE_LIBCRYPTO
#error You don`t have OpenSSL -lcrypto properly installed
#endif
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>

#if defined(CRYPT_BLOWFISH128) || defined(CRYPT_BLOWFISH256)
#define CRYPT_BLOWFISH
#ifndef HAVE_BF_CFB64_ENCRYPT
#error You enabled a Blowfish cipher but your crypto library do not support it
#endif
#include <openssl/blowfish.h>
#endif

#ifdef CRYPT_CAST
#ifndef HAVE_CAST_CFB64_ENCRYPT
#error You enabled the CAST cipher but your crypto library do not support it
#endif
#include <openssl/cast.h>
#endif

#ifdef CRYPT_IDEA
#ifndef HAVE_IDEA_CFB64_ENCRYPT
#error You enabled the IDEA cipher but your crypto library do not support it
#endif
#include <openssl/idea.h>
#endif

#ifdef CRYPT_DES
#ifndef HAVE_DES_CFB64_ENCRYPT
#error You enabled the DES cipher but your crypto library do not support it
#endif
#include <openssl/des.h>
#endif

#ifdef CRYPT_3DES
#ifndef HAVE_DES_EDE3_CFB64_ENCRYPT
#error You enabled the Triple DES cipher but your crypto library do not support it
#endif
#include <openssl/des.h>
/* YES that is a double include... I'm lazy 
 * -einride
 */
#endif

#if defined(CRYPT_RC5_8) || defined(CRYPT_RC5_12) || defined(CRYPT_RC5_16)
#define CRYPT_RC5
#ifndef HAVE_RC5_32_CFB64_ENCRYPT
#error You enabled the RC5 cipher but your crypto library do not support it
#endif
#include <openssl/rc5.h>
#endif

struct Client;
struct ConfItem;

/* Function return values */
#define CRYPT_OK               0  /* Everything fine */
#define CRYPT_BADPARAM         1  /* Something wasn't right :) */
#define CRYPT_ENCRYPTED        2  /* Passed data have been encrypted */
#define CRYPT_DECRYPTED        3  /* Passed data have been decrypted */
#define CRYPT_NOT_ENCRYPTED    4  /* Passed data haven't been altered */
#define CRYPT_NOT_DECRYPTED    5  /* Passed data haven't been altered */
#define CRYPT_ERROR            6

/* State flags */
#define CRYPTFLAG_ENCRYPT     1
#define CRYPTFLAG_DECRYPT     2


#define CRYPT_RSASIZE         256 /* RSA Keys must be exact this size */

#define CRYPT_CIPHERNAMELENGTH 6

typedef int (*cipher_encryptproc) (struct Client *, char *, int);
typedef int (*cipher_initproc) (void * state, unsigned char * keydata);

typedef struct {
  char name[CRYPT_CIPHERNAMELENGTH];
  int keysize;
  int state_data_size;
  int priority;
  cipher_initproc init;
  cipher_encryptproc encrypt;
  cipher_encryptproc decrypt;
} CipherDef;

extern CipherDef Ciphers[];

struct CryptData {
  CipherDef * InCipher;
  void * InState;
  unsigned char inkey[64];
  CipherDef * OutCipher;
  void * OutState;
  unsigned char outkey[64];
  RSA * RSAKey;
  int flags;
};



int crypt_selectcipher(char * ciphers);
int crypt_rsa_decode(char * base64text, unsigned char * data, int * length);
int crypt_rsa_encode(RSA * rsakey, unsigned char * data, char * base64text, int length);

int crypt_init();
int crypt_initserver(struct Client * cptr, struct ConfItem * cline, struct ConfItem * nline);
int crypt_encrypt(struct Client * cptr, const char * Data, int Length);
int crypt_decrypt(struct Client * cptr, const char * Data, int Length);
void crypt_free(struct Client * cptr);
#endif
#endif








