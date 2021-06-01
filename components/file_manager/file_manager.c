/*
 * @file: file_manager.c
 *
 * @brief: This file contains file handling functions
 *
 * @author: Ashutosh Singh Parmar
 */
#include "file_manager.h"

static uint8_t mounted = 0;

/**
 * @brief : This function is used to mount the SPIFFS storage
 *
 * @params :
 * 1. char * name : The name of the storage
 *
 * @return : esp_err_t
 * ESP_OK success
 * ESP_FAIL failed
 */
esp_err_t mount_spiffs(char * name)
{
	esp_vfs_spiffs_conf_t fs_conf = {
		      .base_path = name,
		      .partition_label = NULL,
		      .max_files = 5,
		      .format_if_mount_failed = true
	};

	esp_err_t err = esp_vfs_spiffs_register(&fs_conf);

	if( err == ESP_OK )
		mounted = 1;

	return err;
}

/**
 * @brief : This function is used to get the size of a specified file
 *
 * @params :
 * 1. char * filename: Name of the file
 *
 * @returns : int64_t
 * The length of the file
 * -1 if something went wrong
 */
int64_t get_file_size(char * filename)
{
	if(!mounted) return -1;

	int64_t length=-1;
	FILE * fp=fopen(filename,"r");

	//failed to open the specified file, return negative number
	if(fp==NULL) return length;

	if(!fseek(fp, 0, SEEK_END))
		//get the size of file in bytes
		length=ftell(fp);

	fclose(fp);

	return length;
}

/**
 * @brief : This function is used to read a specified file into a character array
 *
 * @params :
 * 1. char * filename: Name of the file
 *
 * @returns : char *
 * Pointer to string containing file content
 * NULL if something went wrong
 */
char * read_file(char * filename)
{
	if(!mounted) return NULL;

	char * fl=NULL;
	int64_t length = -1;

	FILE * fp=fopen(filename,"r");

	//failed to open the specified file, return NULL pointer
	if(fp == NULL) return NULL;

	//get the size of file in bytes
	if( !fseek(fp, 0, SEEK_END) )
	{
		length=ftell(fp);
		if(length==-1)
		{
			fclose(fp);
			return NULL;
		}
	}
	else
	{
		fclose(fp);
		return NULL;
	}

	// create a buffer of size of file + 1 for termination character
	fl=( char *)calloc( (length+1), sizeof( char ) );

	// if the buffer could not be created then, return NULL pointer
	if( fl==NULL )
	{
		fclose(fp);
		return NULL;
	}

	//memset(fl, 0, (length+1) );

	if( fseek(fp, 0, SEEK_SET)!=0 )
	{
		free(fl);
		fclose(fp);
		return NULL;
	}

	// if data could not be read then, return NULL pointer
	if( fread(fl, 1, length, fp) != length )
	{
		free(fl);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	return fl;
}

/**
 * @brief : This function is used to read a specified file into a character array
 *
 * @params :
 * 1. char * filename: Name of the file
 * 2. char * text : Content to be written into the file
 *
 * @returns : esp_err_t
 * ESP_OK - success
 * ESP_FAIL - failed
 */
esp_err_t write_to_file(char * filename, char * text)
{
	if( !mounted ) return ESP_FAIL;

	//open the file in write mode
	FILE * fp=fopen(filename, "w");

	if(fp != NULL)
	{
		int err=fputs(text, fp);

		if( err != EOF)
		{
			// FILE IS SUCCESSFULLY WRITTEN TO
			fclose(fp);
			return ESP_OK;
		}
		else
		{
			fclose(fp);
			return ESP_FAIL;
		}
	}
	else
	{
		return ESP_FAIL;
	}
}
