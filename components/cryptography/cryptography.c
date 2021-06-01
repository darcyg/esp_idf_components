/*
 * @file: cryptography.h
 *
 * @brief: This file contains functions to perform cryptographic operations
 *
 * @author: Ashutosh Singh Parmar
 */
#include "cryptography.h"

/**
 * @brief : This function is used to encrypt data using AES-128 algorithm
 *
 * @param :
 * 1. const char * key : Pointer to key string
 * 2. const char * data : Pointer to plain data string
 * 3. uint32_t data_len : Number of plain data bytes
 * 4. char * buff : Pointer to buffer where encrypted data will be stored
 * 5. uint32_t * buff_len : Pointer to variable containing size of output buff, number of bytes in encrypted data will be stored in this variable
 *
 * @return : char
 */
char encryptAES_ECB(const char * key, const char * data, uint32_t data_len, char * buff, uint32_t * buff_len)
{
	// if there is no data then, return with failed code
	if( !data_len ) return 0;

	// getting the number of padding bytes
	uint8_t padding = (data_len%16) ? (16 - (data_len % 16)) : 0;

	// if the output buffer is not of sufficient size then, return with failed code
	if( *(buff_len) < data_len + padding ) return 0;

	mbedtls_aes_context aes;
	mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );

	for( uint32_t i=0; i<data_len; )
	{
		if( (i + 16) >= data_len )
		{
			char temp[16] = {0};
			memcpy(temp, (data+i), (16-padding));
			if( mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)temp, (unsigned char *)(buff+i) ) != 0 ) return 0;
		}
		else
		{
			if( mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)(data+i), (unsigned char *)(buff+i) ) != 0 ) return 0;
		}
		i += 16;
		*(buff_len) = i;
	}

	mbedtls_aes_free( &aes );

	return 1;
}

/**
 * @brief : This function is used to decrypt data using AES-128 algorithm
 *
 * @param :
 * 1. const char * key : Pointer to key string
 * 2. const char * data : Pointer to encrypted data string
 * 3. uint32_t data_len : Number of encrypted data bytes
 * 4. char * buff : Pointer to buffer where un-encrypted data will be stored
 * 5. uint32_t buff_len : Size of output buff
 *
 * @return : char
 */
char decryptAES_ECB(const char * key, const char * data, uint32_t data_len, char * buff, uint32_t buff_len)
{
	if( data_len % 16 != 0 || data_len > buff_len )return 0;

	mbedtls_aes_context aes;
	mbedtls_aes_setkey_dec( &aes, (const unsigned char*) key, strlen(key) * 8 );

	for(uint32_t i=0; i<(data_len/16); i++)
	{
		if( mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*)(data+ i*16), (unsigned char *)(buff+ i*16) ) != 0 ) return 0;
	}
	return 1;
}




