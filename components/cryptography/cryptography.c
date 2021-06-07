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
 *
 * @note : If the input data does not 'multiple of 16' bytes then, 0s are padded in the end.
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
 * '1' success
 * '0' failed
 *
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


/**
 * @brief : This API is used to hash a plain text string using MD5 algorithm
 *
 * @params :
 * 1. const unsigned char * text : NULL terminated data string
 * 2. char * buff : Pointer to array where the DIGEST will be stored
 *
 * @returns : char
 * '1' Success
 * '0' Failed
 */
char hashMD5(const unsigned char * text, unsigned char * buff)
{
	unsigned char output[MBEDTLS_MD_MAX_SIZE];

	const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_MD5);

	mbedtls_md_context_t ctx;

	mbedtls_md_init(&ctx);

	int ret = mbedtls_md_init_ctx(&ctx, md_info);
	if (ret != 0)
	{
	    return 0;
	}

	mbedtls_md_starts(&ctx);

	mbedtls_md_update(&ctx, text, strlen((char *)text));

	mbedtls_md_finish(&ctx, output);

	mbedtls_md_free(&ctx);

	//uart0Println("DIGEST");

	//LOG_AS_HEX(output, MBEDTLS_MD_MAX_SIZE);

	memcpy(buff, output, 16);

	return 1;
}




