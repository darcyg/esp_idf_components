/**
 * @brief : This file contains APIs that are used to implement vispr protocol
 *
 * @file : vispr.c
 *
 * @author : Ashutosh Singh parmar
 */
#include "vispr.h"

static vispr_talker talker = {.socket=-1,};

#define LOG_AS_HEX(pt, len) { for(unsigned int k=0; k<len; k++){ uart0PrintHex( *(pt+k) ); uart0Send(' '); } uart0Println(""); }


/**
 * @brief : This API is used to hash a plain text string using MD5 algorithm
 *
 * @params :
 * 1. const unsigned char * text : NULL terminated data string
 * 2. int len : The length of input data
 * 3. unsigned char * buff : Pointer to array where the DIGEST will be stored
 *
 * @returns : char
 * '1' Success
 * '0' Failed
 */
static char utilHashMD5(const unsigned char * text, int len, unsigned char * buff)
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

	mbedtls_md_update(&ctx, text, len );

	mbedtls_md_finish(&ctx, output);

	mbedtls_md_free(&ctx);

	memcpy(buff, output, 16);

	return 1;
}


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
static char utilEncryptAES_ECB(const char * key, const char * data, uint32_t data_len, char * buff, uint32_t * buff_len)
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
 * @brief : This API is used to create a 'vispr' broadcaster
 *
 * @param :
 * 1. uint16_t uid : The 2 byte UID of device
 * 2. char key[16] : 128 bits shared key
 * 3. char * topic : NULL terminated topic string (should have length between 5 and 100 characters)
 * 4. uint64_t counter : The starting counter for broadcast
 *
 * @returns : esp_err_t
 * ESP_OK if everything if socket is created successfully
 */
esp_err_t visprTalkerInitialize( char * name, uint16_t uid, char key[16], char * topic, uint64_t counter )
{
	// If a socket a vispr socket already exists then, exit with error code
	if( talker.socket != -1 ) return ESP_FAIL;

	// creating a socket
	int sock = socket(AF_INET, SOCK_DGRAM, 0);

	if(sock < 0)
	{
		return ESP_FAIL;
	}

	char * n = (char *)calloc( strlen(name)+1, sizeof(char) );
	memcpy(n, name, strlen(name));

	char * t = (char *)calloc( strlen(topic)+1, sizeof(char) );
	memcpy(t, topic, strlen(topic));

	vispr_talker temp_talker = {
			.name = n,
			.uid = uid,
			.topic = t,
			.counter = counter,
			.socket = sock,
	};
	memcpy(temp_talker.key, key, 16);

	// saving all the information in the global variable
	talker = temp_talker;

	// If the operation was not successful then, return with error code and close the socket
	int broadcastEnable = 1;
	if( setsockopt(talker.socket, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable) ) != 0)
	{
		shutdown(talker.socket, 0);
		close(talker.socket);

		talker.socket = -1;

		free(talker.name);
		free(talker.topic);

		return ESP_FAIL;
	}

	// Writing destination address
	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = inet_addr(VISPR_BROADCAST_ADDRESS);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(VISPR_BROADCAST_PORT);
	talker.destinationAddr = dest_addr;

	return ESP_OK;
}

/**
 * @brief : This API is used to destroy the talker object and shut down its operation
 *
 * @param:
 * NONE
 *
 * @returns: esp_err_t
 * ESP_OK
 */
esp_err_t vispTalkerDestroy()
{
	if( talker.socket == -1)
		return ESP_OK;

	shutdown(talker.socket, 0);
	close(talker.socket);

	talker.socket = -1;

	free(talker.name);
	free(talker.topic);

	return ESP_OK;
}

/**
 * @brief : This is a utility API used to generate 128 bits MAC code for the broadcast
 *
 * @params :
 * 1. char * msg : Pointer to message string
 * 2. int msgLen : Length of message string
 * 3. char * buff : Pointer to buffer where MAC code will be stored
 *
 * @return : char
 * 1 success
 * 0 failed
 */
static char utilGenerateMAC(char * msg, int msgLen, char * buff)
{
	if(talker.socket == -1)
		return 0;

	unsigned char temp[365];
	int i = 0;

	// Writing the flag byte
	temp[ i++ ] = 0X00;

	// Writing the UID bytes
	temp[ i++ ] = talker.uid & 0xff;
	temp[ i++ ] = (talker.uid >> 8 ) & 0xff;

	// Writing the counter
	for(; i<=10; i++ )
	{
		temp[ i ] = (talker.counter >> ((i-3)*8)) & 0xff;
	}

	// Writing the topic
	memcpy( (temp + i), talker.topic, strlen(talker.topic) );
	i += strlen(talker.topic);

	// Writing the message
	memcpy( (temp + i), msg, msgLen);
	i += msgLen;

	char digest[16];
	if( !utilHashMD5(temp, i, (unsigned char *)digest)) return 0;

	char mac[16];
	uint32_t k = 16;
	if( !utilEncryptAES_ECB( (const char *)talker.key, digest, 16, mac, &k)) return 0;

	memcpy( buff, mac, 16);

	return 1;
}

/**
 * @brief : This API is used to broadcast vispr messages.
 *
 * @params:
 * 1. unsigned char * msg : The message string
 * 2. int len : Length of the message string
 *
 * @returns: esp_err_t
 * ESP_OK success
 * ESP_FAIL failed
 */
esp_err_t visprBroadcast( unsigned char * msg, int len)
{
	if(talker.socket == -1 || len > 255)
		return ESP_FAIL;

	// buffer for holding the data
	char buff[500];
	unsigned int i = 0;

	// Writing the PREAMBLE byte into buffer
	buff[ i++ ] = PREAMBLE;

	// Writing the FLAG byte into buffer
	buff[ i++ ] = 0X00;

	// Writing the UID bytes
	buff[ i++ ] = talker.uid & 0xff;
	buff[ i++ ] = (talker.uid >> 8 ) & 0xff;

	// Generating and writing the MAC code
	char mac[16];
	if( !utilGenerateMAC( (char *)msg, len, mac) ) return 0;

	memcpy((buff+i), mac, 16);
	i += 16;

	// Writing the broadcast counter
	for(; i<=27; i++ )
	{
		buff[ i ] = (talker.counter >> ((i-20)*8)) & 0xff;
	}

	// Writing the lengths
	uint8_t topicLen = strlen(talker.topic);
	buff[ i++ ] = topicLen;
	buff[ i++ ] = len;

	// Writing the topic
	memcpy( (buff+i), talker.topic, topicLen);
	i += topicLen;

	// Writing the message
	memcpy( (buff+i), msg, len);
	i += len;

	buff[ i++ ] = END_OF_BROADCAST;

	int r = (9/RAND_MAX)*rand() + 1;

	talker.counter = talker.counter + r;

	for( uint8_t rtx = 1; rtx<= MAX_RTX; rtx++)
	{
		sendto(talker.socket, buff, i, 0, (struct sockaddr *)&talker.destinationAddr, sizeof(talker.destinationAddr));
		vTaskDelay( RTX_DELAY );
	}

	return ESP_OK;
}

/**
 * @brief : This API is used to generate 128 bits key from plain text. It uses MD5 has for this purpose
 *
 * @params :
 * 1. char * text : NULL terminated plain text string
 * 3. char buff[16] : The buffer where the 128 bits digest will be stored
 *
 * @returns : char
 * '1' Success
 * '0' Failed
 */
char generateKey( unsigned char * text, unsigned char * buff)
{
	return utilHashMD5( text, strlen((char *)text), buff);
}

