/*
 * @file: util_wifi.c
 *
 * @brief: This file contains various utility functions to interact with the WiFi
 *
 * @author: Ashutosh Singh Parmar
 */
#include "util_nvs.h"

/*
 * @brief : This API is used to initialize the NVS storage.
 *
 * @param
 * NONE
 *
 * @return
 * NOTHING
 */
void InitializeNVS()
{
	ESP_ERROR_CHECK(nvs_flash_init());
}

/**
 * @brief : This API  is used to store data in NVS Storage.
 *
 * @param
 * 1. const char * namespace : The NVS Namespace where data has to be stored.
 * 2. const char * key : The key string.
 * 3. uint8_t * value : Pointer to string of 8 bit values that are to be stored in NVS storage.
 * 4. uint8_t len : The number of values in NVS Storage.
 *
 * @returns - esp_err_t
 * ESP_OK : success
 * ESP_FAIL : failed
 */
esp_err_t NVSStoreBytes(const char * namespace, const char * key, uint8_t * value, uint8_t len)
{
	esp_err_t _err;
	nvs_handle_t nvs_st;

	//open the NVS storage
	_err=nvs_open(namespace, NVS_READWRITE, &nvs_st);
	// if the nvs storage could not be opened, return with error code
	if( _err != ESP_OK ) return ESP_FAIL;

	// storing operation
	_err=nvs_set_blob(nvs_st, key, value, (size_t)len);
	// return with error code if something goes wrong in storing operation
	if( _err != ESP_OK ){
		//close the NVS storage before exit
		nvs_close(nvs_st);
		return ESP_FAIL;
	}

	// committing to memory
	_err = nvs_commit(nvs_st);
	nvs_close(nvs_st);

	if(_err == ESP_OK ) return _err;
	else return ESP_FAIL;
}

/**
 * @brief : This API is used to read a string of 8 bit values from NVS Storage.
 *
 * @param :
 * 1. const char * namespace : The NVS Namespace where data is stored.
 * 2. const char * key : The key string.
 * 3. uint8_t * value : Pointer to array where retrieved data will be stored.
 * 4. uint8_t * max_length : Pointer to variable containing size of the destination array. If the read operation
 * is successful then, number of read bytes is also stored in this variable.
 *
 * @return - esp_err_t
 * ESP_OK : success
 * ESP_FAIL : failed
 */
esp_err_t NVSReadBytes(const char * namespace, const char * key, uint8_t * value, uint8_t * max_length)
{
	//ESP error code holder
	esp_err_t _err;

	nvs_handle_t nvs_st;
	size_t s;

	//open the NVS storage
	_err=nvs_open(namespace, NVS_READWRITE, &nvs_st);
	//if there was error in opening the storage name space then, return with error code
	if(_err != ESP_OK) return ESP_FAIL;

	//get the required number of bytes for key's value
	_err=nvs_get_blob(nvs_st, key, NULL, &s);
	if(_err != ESP_OK || ( (uint8_t)s>(*max_length) )){
		//return with error code if, failed to read or the input variable is too small for the data
		nvs_close(nvs_st);
		return ESP_FAIL;
	}
	else *max_length = (uint8_t)s;

	//get the actual data into the key array
	_err=nvs_get_blob(nvs_st, key, value, &s);
	nvs_close(nvs_st);

	if( _err == ESP_OK ) return _err;
	else return ESP_FAIL;

	return ESP_OK;
}

/**
 * @brief : This API is used to store a 32 bit signed integer in NVS_Storage
 *
 * @param:
 * 1. const char * namespace : The namespace where integer has to stored.
 * 2. const char * key : The key value.
 * 3. int32_t value : The integer to be stored.
 *
 * @return - esp_err_t
 * ESP_OK : success
 * ESP_FAIL : failed
 */
esp_err_t NVSStoreInteger32(const char * namespace, const char * key, int32_t value)
{
	//ESP error code holder
	esp_err_t _err;
	nvs_handle_t nvs_st;

	//open the NVS storage
	_err=nvs_open(namespace, NVS_READWRITE, &nvs_st);
	//if there was error in opening the storage name space then, return with error code
	if(_err != ESP_OK) return ESP_FAIL;

	//get the required number of bytes for key's value
	_err=nvs_set_i32(nvs_st, key, value);
	if(_err != ESP_OK ){
		//failed to read or the input variable is too small for the data
		nvs_close(nvs_st);
		return ESP_FAIL;
	}

	//get the actual data into the key array
	_err=nvs_commit(nvs_st);
	nvs_close(nvs_st);

	if( _err == ESP_OK ) return _err;
	else return ESP_FAIL;
}

/**
 * @brief : This API is used to read a 32 bit signed integer from NVS_Storage
 *
 * @param:
 * 1. const char * namespace : The namespace where integer is stored.
 * 2. const char * key : The key value.
 * 3. int32_t * value : Pointer to variable where retrieved integer will be stored.
 *
 * @return - esp_err_t
 * ESP_OK : success
 * ESP_FAIL : failed
 */
esp_err_t NVSReadInteger32(const char * namespace, const char * key, int32_t * value)
{
	//ESP error code holder
	esp_err_t _err;
	nvs_handle_t nvs_st;

	//open the NVS storage
	_err=nvs_open(namespace, NVS_READWRITE, &nvs_st);
	//if there was error in opening the storage name space then, return with error code
	if(_err != ESP_OK) return ESP_FAIL;

	//get the required number of bytes for key's value
	_err=nvs_get_i32(nvs_st, key, value);
	nvs_close(nvs_st);

	if(_err == ESP_OK ) return _err;
	else return ESP_FAIL;
}
