/*
 * @file: util_uart.h
 *
 * @brief: This is the public interface to the util_uart library
 *
 * @author: Ashutosh Singh Parmar
 */

#ifndef COMPONENTS_UTIL_UART_UTIL_UART_H_
#define COMPONENTS_UTIL_UART_UTIL_UART_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "driver/uart.h"

#define UART_MAX_INTEGER_DIGITS 20

#define UART0_RX_BUFFER_SIZE 200
#define UART2_RX_BUFFER_SIZE 200

#define UART0_TASK_STACK_SIZE 2048
#define UART2_TASK_STACK_SIZE 2048

typedef struct uart_char { char character; char flag; }uart_char;

esp_err_t uart0_begin(int);

esp_err_t uart2_begin(int);

void uart0Send(char);

void uart2Send(char);

void uart0SendBytes(char *, uint16_t);

void uart0Print(char *);

void uart2Print(char *);

void uart0Println(char *);

void uart2SendBytes(char *, uint16_t);

void uart2Println(char *);

uart_char uart0Read(void);

uart_char uart2Read(void);

char * uart0ReadBytesUntil(char);

char * uart2ReadBytesUntil(char);

void uart0InputReset(void);

void uart2InputReset(void);

void uart0PrintInteger(int );

void uart2PrintInteger(int );

void uart0PrintHex(int );

void uart2PrintHex(int );

#endif /* COMPONENTS_UTIL_UART_UTIL_UART_H_ */
