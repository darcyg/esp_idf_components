/*
 * @file: util_nvs.h
 *
 * @brief: This file contains various utility functions to interact with the NVS storage
 *
 * @author: Ashutosh Singh Parmar
 */

#ifndef COMPONENTS_UTIL_NVS_UTIL_NVS_H_
#define COMPONENTS_UTIL_NVS_UTIL_NVS_H_

#include "nvs_flash.h"

void InitializeNVS();

void EraseNVS();

esp_err_t NVSStoreBytes(const char *, const char *, uint8_t *, uint8_t);

esp_err_t NVSReadBytes(const char *, const char *, uint8_t *, uint8_t *);

esp_err_t NVSStoreInteger32(const char *, const char *, int32_t);

esp_err_t NVSReadInteger32(const char *, const char *, int32_t *);

#endif /* COMPONENTS_UTIL_NVS_UTIL_NVS_H_ */
