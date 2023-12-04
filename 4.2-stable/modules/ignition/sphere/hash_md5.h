/*********************************************************************
* Filename:   md5.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding MD5 implementation.
*********************************************************************/

#ifndef MD5_H
#define MD5_H

/*************************** HEADER FILES ***************************/
#include <stddef.h>

/****************************** MACROS ******************************/
#define MD5_BLOCK_SIZE 16               // MD5 outputs a 16 byte digest

/**************************** DATA TYPES ****************************/
typedef unsigned char BYTE_MD5;             // 8-bit byte
typedef unsigned int  WORD_MD5;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
   BYTE_MD5 data[64];
   WORD_MD5 datalen;
   unsigned long long bitlen;
   WORD_MD5 state[4];
} MD5_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void md5_init(MD5_CTX *ctx);
void md5_update(MD5_CTX *ctx, const BYTE_MD5 data[], size_t len);
void md5_final(MD5_CTX *ctx, BYTE_MD5 hash[]);

#endif   // MD5_H
