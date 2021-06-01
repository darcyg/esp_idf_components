/*
 * @file: file_manager.h
 *
 * @brief: This file contains declarations of file handling functions
 *
 * @author: Ashutosh Singh Parmar
 */

#ifndef COMPONENTS_FILE_MANAGER_FILE_MANAGER_H_
#define COMPONENTS_FILE_MANAGER_FILE_MANAGER_H_

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#include "/Users/ashu/projects/esp/esp-idf/components/spiffs/include/esp_spiffs.h"

esp_err_t mount_spiffs(char *);

int64_t get_file_size(char *);

char * read_file(char *);

esp_err_t write_to_file(char *, char *);

#endif /* COMPONENTS_FILE_MANAGER_FILE_MANAGER_H_ */
