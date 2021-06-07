/*
 * @file: cryptography.h
 *
 * @brief: This file contains declarations of functions to perform cryptographic operations
 *
 * @author: Ashutosh Singh Parmar
 */
#ifndef COMPONENTS_CRYPTOGRAPHY_CRYPTOGRAPHY_H_
#define COMPONENTS_CRYPTOGRAPHY_CRYPTOGRAPHY_H_

#include <mbedtls/aes.h>
#include <mbedtls/md.h>
#include <string.h>

char encryptAES_ECB(const char *, const char *, uint32_t, char *, uint32_t *);

char decryptAES_ECB(const char *, const char *, uint32_t, char *, uint32_t);

char hashMD5(const unsigned char *, unsigned char *);

#endif /* COMPONENTS_CRYPTOGRAPHY_CRYPTOGRAPHY_H_ */
