/**
 * @brief : This file contains declarations for APIs that are used to implement vispr protocol
 *
 * @file : vispr.h
 *
 * @author : Ashutosh Singh parmar
 */
#include <string.h>

#include <stdlib.h>

#include "esp_err.h"

#include "mbedtls/aes.h"
#include "mbedtls/md.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

#include "util_uart.h"

#define VISPR_BROADCAST_ADDRESS "255.255.255.255"
#define VISPR_BROADCAST_PORT 55667

#define PREAMBLE 0XEF
#define END_OF_BROADCAST 0XFE

#define MAX_RTX 10
#define RTX_DELAY 1

typedef struct vispr_talker { char * name; uint16_t uid; uint8_t key[16]; char * topic; uint64_t counter; int socket; struct sockaddr_in destinationAddr; }vispr_talker;

esp_err_t visprTalkerInitialize( char *, uint16_t, char [16], char *, uint64_t );

esp_err_t vispTalkerDestroy();

esp_err_t visprBroadcast( unsigned char *, int);

char generateKey(unsigned char *, unsigned char *);

