/*
 * @file: util_wifi.h
 *
 * @brief: This file contains various utility functions to interact with the WiFi
 *
 * @author: Ashutosh Singh Parmar
 */


#ifndef COMPONENTS_UTIL_WIFI_UTIL_WIFI_H_
#define COMPONENTS_UTIL_WIFI_UTIL_WIFI_H_

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"

void set_sta_ssid(char *);
void set_sta_password(char *);

void set_ap_ssid(char *);
void set_ap_password(char *);

esp_err_t begin_wifi_sta();
esp_err_t begin_wifi_ap();
void stop_wifi();

uint8_t isStationConnected();

#endif
