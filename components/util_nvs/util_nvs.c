/*
 * @file: util_wifi.c
 *
 * @brief: This file contains various utility functions to interact with the WiFi
 *
 * @author: Ashutosh Singh Parmar
 */
#include "util_nvs.h"

/*
 * @brief : This function is used to initialize the NVS storage.
 *
 * @param
 * none
 *
 * @return
 * nothing
 */
void InitializeNVS()
{
	ESP_ERROR_CHECK(nvs_flash_init());
}
