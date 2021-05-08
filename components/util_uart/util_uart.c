/*
 * @file: util_uart.c
 *
 * @brief: This file contains various utility functions to interact with the UART port
 *
 * @author: Ashutosh Singh Parmar
 */

#include "util_uart.h"

static char STRING0[UART0_RX_BUFFER_SIZE];
static char * empty_string0="";
uint16_t ind0=0;

static char STRING2[UART2_RX_BUFFER_SIZE];
static char * empty_string2="";
uint16_t ind2=0;

/*
 * @brief: This function initializes UART0 port for communication.
 *
 * @param:
 * 1. int _baud - baud rate.
 *
 * @return: esp_err_t
 * ESP_OK - if everything initialization is successful.
 * Error code otherwise.
 */
esp_err_t uart0_begin(int _baud)
{
	esp_err_t _err;

	uart_config_t uart0_config = {
	        .baud_rate = _baud,
	        .data_bits = UART_DATA_8_BITS,
	        .parity    = UART_PARITY_DISABLE,
	        .stop_bits = UART_STOP_BITS_1,
	        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	        .source_clk = UART_SCLK_APB,
	};

	_err=uart_driver_install(UART_NUM_0, UART0_RX_BUFFER_SIZE * 2, 0, 0, NULL, 0);
	if(_err!=ESP_OK) return _err;

	_err=uart_param_config(UART_NUM_0, &uart0_config);
	if(_err!=ESP_OK) return _err;

	_err=uart_set_pin(UART_NUM_0, 1, 3, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	return _err;
}

/*
 * @brief: This function initializes UART2 port for communication.
 *
 * @param:
 * 1. int _baud - baud rate.
 *
 * @return: esp_err_t
 * ESP_OK - if everything initialization is successful.
 * Error code otherwise.
 */
esp_err_t uart2_begin(int _baud)
{
	esp_err_t _err;

	uart_config_t uart2_config = {
	        .baud_rate = _baud,
	        .data_bits = UART_DATA_8_BITS,
	        .parity    = UART_PARITY_DISABLE,
	        .stop_bits = UART_STOP_BITS_1,
	        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	        .source_clk = UART_SCLK_APB,
	};

	_err=uart_driver_install(UART_NUM_2, UART2_RX_BUFFER_SIZE * 2, 0, 0, NULL, 0);
	if(_err!=ESP_OK) return _err;

	_err=uart_param_config(UART_NUM_2, &uart2_config);
	if(_err!=ESP_OK) return _err;

	_err=uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

	return _err;
}

/*
 * @brief: This function is used to delete the driver for UART0 port.
 *
 * @param:
 * NONE
 *
 * @return:
 * NOTHING
 */
void uart0End(void)
{
	uart_driver_delete(UART_NUM_0);
}

/*
 * @brief: This function is used to delete the driver for UART2 port.
 *
 * @param:
 * NONE
 *
 * @return:
 * NOTHING
 */
void uart2End(void)
{
	uart_driver_delete(UART_NUM_2);
}

/*
 * @brief: This function is used to send one byte to UART0 port.
 *
 * @param:
 * 1. char byt: The byte to send.
 *
 * @return: nothing.
 */
void uart0Send(char byt)
{
	uart_write_bytes(UART_NUM_0, &byt, 1);
}

/*
 * @brief: This function is used to send one byte to UART2 port.
 *
 * @param:
 * 1. char byt: The byte to send.
 *
 * @return: nothing.
 */
void uart2Send(char byt)
{
	uart_write_bytes(UART_NUM_2, &byt, 1);
}

/*
 * @brief: This function is used to send bytes to UART0 port.
 *
 * @param:
 * 1. char * byts: Pointer to byte string.
 * 2. len : Number of bytes in the string.
 *
 * @return: nothing.
 */
void uart0SendBytes(char * byts, uint16_t len)
{
	uart_write_bytes(UART_NUM_0, byts, len);
}

/*
 * @brief: This function is used to send bytes to UART2 port.
 *
 * @param:
 * 1. char * byts: Pointer to byte string.
 * 2. len : Number of bytes in the string.
 *
 * @return: nothing.
 */
void uart2SendBytes(char * byts, uint16_t len)
{
	uart_write_bytes(UART_NUM_2, byts, len);
}

/*
 * @brief: This function is used to send a '\0' terminated string to UART0 port.
 *
 * @param:
 * 1. char * str: Pointer to character string.
 *
 * @return: nothing.
 */
void uart0Print(char * str)
{
	uart0SendBytes(str, strlen(str));
}

/*
 * @brief: This function is used to send a '\0' terminated string to UART2 port.
 *
 * @param:
 * 1. char * str: Pointer to character string.
 *
 * @return: nothing.
 */
void uart2Print(char * str)
{
	uart2SendBytes(str, strlen(str));
}

/*
 * @brief: This function is used to send a '\0' terminated string to UART0 port. Newline character is automatically appended.
 *
 * @param:
 * 1. char * str: Pointer to character string.
 *
 * @return: nothing.
 */
void uart0Println(char * str)
{
	uart0SendBytes(str, strlen(str));
	uart0Send('\n');
}

/*
 * @brief: This function is used to send a '\0' terminated string to UART2 port. Newline character is automatically appended.
 *
 * @param:
 * 1. char * str: Pointer to character string.
 *
 * @return: nothing.
 */
void uart2Println(char * str)
{
	uart2SendBytes(str, strlen(str));
	uart2Send('\n');
}

/*
 * @brief: This function is used to read one byte from UART0 receive buffer.
 *
 * @param:
 * NONE
 *
 * @return: uart_char (a custom structure)
 */
uart_char uart0Read(void)
{
	uart_char ch = { 0, 0};
	uint16_t len=uart_read_bytes(UART_NUM_0, &(ch.character), 1, 20 / portTICK_RATE_MS);
	if(len==1)
	{
		ch.flag=1;
	}
	return ch;
}

/*
 * @brief: This function is used to read one byte from UART2 receive buffer.
 *
 * @param:
 * NONE
 *
 * @return: uart_char (a custom structure)
 */
uart_char uart2Read(void)
{
	uart_char ch = { 0, 0};
	uint16_t len=uart_read_bytes(UART_NUM_2, &(ch.character), 1, 20 / portTICK_RATE_MS);
	if(len==1)
	{
		ch.flag=1;
	}
	return ch;
}

/*
 * @brief: This function is used to read a series of bytes until a particular byte, from UART0 port.
 *
 * @param:
 * 1. char delimiter : the delimiter byte
 *
 * @return: char *
 * Pointer to read bytes.
 */
char * uart0ReadBytesUntil(char delimiter)
{
	if(ind0 < (UART0_RX_BUFFER_SIZE-1)){
		uart_char CH=uart0Read();
		if(CH.flag){
			if(CH.character!=delimiter){
				STRING0[ind0++]=CH.character;
				return empty_string0;
			}
			else{
				STRING0[ind0]='\0';
				ind0=0;
				return STRING0;
			}
		}
		else
			return empty_string0;
	}
	else{
		STRING0[UART0_RX_BUFFER_SIZE-1]='\0';
		ind0=0;
		return STRING0;
	}
}

/*
 *  @brief: This function is used to read a series of bytes until a particular byte, from UART2 port.
 *
 *  @param:
 *  1. char delimiter : the delimiter byte
 *
 *  @return:
 *  Pointer to read bytes.
 */
char * uart2ReadBytesUntil(char delimiter)
{
	if(ind2< (UART2_RX_BUFFER_SIZE-1)){
		uart_char CH=uart2Read();
		if(CH.flag){
			if(CH.character!=delimiter){
				STRING2[ind2++]=CH.character;
				return empty_string2;
			}
			else{
				STRING2[ind2]='\0';
				ind2=0;
				return STRING2;
			}
		}
		else
			return empty_string2;
	}
	else{
		STRING2[UART2_RX_BUFFER_SIZE-1]='\0';
		ind2=0;
		return STRING2;
	}
}

/*
 * @brief: This function resets the storage array associated with UART0 port.
 *
 * @param:
 * NONE
 *
 * @return:
 * NOTHING
 */
void uart0InputReset(void){
	memset(STRING0, 0, UART0_RX_BUFFER_SIZE);
	ind0=0;
}

/*
 * @brief: This function resets the storage array associated with UART2 port.
 *
 * @param:
 * NONE
 *
 * @return:
 * NOTHING
 */
void uart2InputReset(void){
	memset(STRING2, 0, UART2_RX_BUFFER_SIZE);
	ind2=0;
}

/*
 * @brief: This function is used to print an integer to UART0 port.
 *
 * @param:
 * 1. int num: The integer to send
 *
 * @return:
 * NOTHING
 */
void uart0PrintInteger(int num)
{
	char buff[UART_MAX_INTEGER_DIGITS+1]={0};
	itoa(num, buff, 10);
	uart0Print(buff);
}

/*
 * @brief: This function is used to print an integer to UART2 port.
 *
 * @param:
 * 1. int num: The integer to send
 *
 * @return:
 * NOTHING
 */
void uart2PrintInteger(int num)
{
	char buff[UART_MAX_INTEGER_DIGITS+1]={0};
	itoa(num, buff, 10);
	uart2Print(buff);
}


