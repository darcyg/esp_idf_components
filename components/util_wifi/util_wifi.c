/*
 * @file: util_wifi.c
 *
 * @brief: This file contains various utility functions to interact with the WiFi
 *
 * @author: Ashutosh Singh Parmar
 */

#include "util_wifi.h"

static uint8_t tcpipInitialized = 0;

/*
 * @brief: This function sets up the network interface.
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK - if network interface is initialized successfully or already running
 *
 */
static esp_err_t initialize_tcpip(void)
{
	if( !tcpipInitialized )
	{
		ESP_ERROR_CHECK(esp_netif_init());
		tcpipInitialized = 1;

	}
	return ESP_OK;
}

static uint8_t eventLoop = 0;

/*
 * @brief: This function sets up the default event loop.
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK - if network event loop is initialized successfully or is already running
 *
 */
static esp_err_t create_default_event_loop()
{
	if( !eventLoop )
	{
		ESP_ERROR_CHECK(esp_event_loop_create_default());
		eventLoop = 1;

	}
	return ESP_OK;
}

/*
 * @brief: This function deletes the default event loop.
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK - if the default event loop is deinitialized or already down
 *
 */
static esp_err_t delete_default_event_loop()
{
	if( eventLoop )
	{
		ESP_ERROR_CHECK(esp_event_loop_delete_default());
		eventLoop = 0;

	}
	return ESP_OK;

}

static esp_netif_t * sta_netif_obj = NULL;

/*
 * @brief: This function registers network interface handlers for station mode operation
 *
 * @param:
 * none
 *
 * @return:
 * nothing
 */
static void register_sta_handlers()
{
	sta_netif_obj = esp_netif_create_default_wifi_sta();
}

/*
 * @brief: This function unregisters network interface handlers for station mode operation
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK if station mode handlers are un-registered successfully
 */
static esp_err_t unregister_sta_handlers()
{
	if( sta_netif_obj != NULL )
	{
		ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers( sta_netif_obj ));
		esp_netif_destroy( sta_netif_obj );
		sta_netif_obj = NULL;
	}
	return ESP_OK;
}

static esp_netif_t * ap_netif_obj = NULL;

/*
 * @brief: This function registers network interface handlers for access point mode operation
 *
 * @param:
 * none
 *
 * @return:
 * nothing
 */
static void register_ap_handlers()
{
	ap_netif_obj = esp_netif_create_default_wifi_ap();
}

/*
 * @brief: This function unregisters network interface handlers for access point mode operation
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK if access point mode handlers are un-registered successfully
 */
static esp_err_t unregister_ap_handlers()
{
	if( ap_netif_obj != NULL )
	{
		ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers( ap_netif_obj ));
		esp_netif_destroy( ap_netif_obj );
		ap_netif_obj = NULL;
	}

	return ESP_OK;
}

/*
 * @brief : This variable holds maximum number of station that can connect to chip when in access point mode.
 */
static uint8_t ESP_AP_MAX_STA_CONN = 3;

/*
 * @brief : This variable holds the WiFi channel number.
 */
static uint8_t ESP_AP_WIFI_CHANNEL = 11;

/*
 * @brief : This variable holds the current WiFi mode.
 */
static wifi_mode_t wifi_current_mode = WIFI_MODE_NULL;

static esp_event_handler_instance_t wifi_event_handler;

static esp_event_handler_instance_t network_event_handler;

/*
 * @brief : This variable holds the whether WiFi is connected to an access point or not.
 */
static uint8_t wifi_sta_connected = 0;

/*
 * @brief : SSID of access point to connect to when in station mode
 */
char sta_ssid[32];

/*
 * @brief : password of access point to connect to when in station mode
 */
char sta_password[64];

/*
 * @brief : SSID of chip when in softAP mode
 */
char ap_ssid[32];

/*
 * @brief : password of chip when in softAP mode
 */
char ap_password[64];

esp_netif_ip_info_t sta_ip_info;

/*
 * @brief: This function is used to set SSID of external access point
 *
 * @param:
 * 1. char * ssid : Pointer to NULL terminated SSID string.
 *
 * @return:
 * nothing
 */
void set_sta_ssid(char * ssid)
{
	memset(sta_ssid, 0, 32);
	for( uint8_t i=0; i<strlen(ssid); i++)
	{
		sta_ssid[i] = *(ssid+i);
	}
}

/*
 * @brief: This function is used to set password of external access point
 *
 * @param:
 * 1. char * password : Pointer to NULL terminated password string.
 *
 * @return: nothing
 */
void set_sta_password(char * password)
{
	memset(sta_password, 0, 64);
	for( uint8_t i=0; i<strlen(password); i++)
	{
		sta_password[i] = *(password+i);
	}
}

/*
 * @brief: This function is used to set SSID of softAP
 *
 * @param:
 * 1. char * ssid : Pointer to NULL terminated SSID string.
 *
 * @return: nothing
 */
void set_ap_ssid(char * ssid)
{
	memset(ap_ssid, 0, 32);
	for( uint8_t i=0; i<strlen(ssid); i++)
	{
		ap_ssid[i] = *(ssid+i);
	}
}

/*
 * @brief: This function is used to set password of softAP
 *
 * @param:
 * 1. char * password : Pointer to NULL terminated password string.
 *
 * @return: nothing
 */
void set_ap_password(char * password)
{
	memset(ap_password, 0, 64);
	for( uint8_t i=0; i<strlen(password); i++)
	{
		ap_password[i] = *(password+i);
	}
}

/*
 * @brief: Event handler for WiFi and Network events
 *
 * @param:
 * 1. void * arg
 * 2. esp_event_base_t event_base
 * 3. int32_t event_id
 * 4. void * event_data
 *
 * @return:
 * nothing
 */
static void wifi_network_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
	if(event_base == WIFI_EVENT)
	{
		switch(event_id)
		{
		case WIFI_EVENT_STA_START:
			esp_wifi_connect();
			break;

		case WIFI_EVENT_STA_CONNECTED:
			wifi_sta_connected = 1;
			break;

		case WIFI_EVENT_STA_DISCONNECTED:
			wifi_sta_connected = 0;
			esp_wifi_connect();
			break;

		case WIFI_EVENT_AP_STACONNECTED:
			break;

		case WIFI_EVENT_AP_STADISCONNECTED:
			break;

		}
	}
	else if(event_base == IP_EVENT)
	{
		ip_event_got_ip_t* event;
		switch(event_id)
		{
		case IP_EVENT_STA_GOT_IP:
	        event = (ip_event_got_ip_t*) event_data;
	        sta_ip_info = event->ip_info;
			break;
		}
	}
}

/*
 * @brief: This function is used to initialize WiFi in station mode
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK if the WiFi has initialized in station mode
 * ESP_FAIL if WiFi was already initialized
 */
esp_err_t begin_wifi_sta()
{
	// If the WiFi is already in AP or STA mode then, exit the function
	if( wifi_current_mode != WIFI_MODE_NULL)
		return ESP_FAIL;

	// initialize the TCP/IP stack
	initialize_tcpip();

	// create the default event loop
	create_default_event_loop();

	// register handlers
	register_sta_handlers();


	// WiFi configuration object with default settings
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	// initialize the WiFi driver
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// register more handlers
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_network_event_handler,NULL,&wifi_event_handler));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,ESP_EVENT_ANY_ID,&wifi_network_event_handler,NULL,&wifi_event_handler));

	wifi_config_t wifi_config =
    {
        .sta = {
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,

            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    memset( &(wifi_config.sta.ssid[0]), 0, 32);
    memset( &(wifi_config.sta.password[0]), 0, 64);

    for(int i=0;i<strlen(sta_ssid);i++)
    {
    	wifi_config.sta.ssid[i]=sta_ssid[i];
    }
    for(int i=0;i<strlen(sta_password);i++)
    {
    	wifi_config.sta.password[i]=sta_password[i];
    }

	// setting the WiFi mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

	// start WiFi as per current settings
	ESP_ERROR_CHECK(esp_wifi_start());

	// set the current mode variable to STA
	wifi_current_mode = WIFI_MODE_STA;

	return ESP_OK;
}

/*
 * @brief: This function is used to initialize WiFi in access point mode
 *
 * @param:
 * none
 *
 * @return: esp_err_t
 * ESP_OK if the WiFi has initialized in station mode
 * ESP_FAIL if WiFi was already initialized
 */
esp_err_t begin_wifi_ap()
{
	// If the WiFi is already in AP or STA mode then, exit the function
	if( wifi_current_mode != WIFI_MODE_NULL)
		return ESP_FAIL;

	// initialize the TCP/IP stack
	initialize_tcpip();

	// create the default event loop
	create_default_event_loop();

	// register handlers
	register_ap_handlers();


	// WiFi configuration object with default settings
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	// initialize the WiFi driver
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// register more handlers
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,ESP_EVENT_ANY_ID,&wifi_network_event_handler,NULL,&wifi_event_handler));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,ESP_EVENT_ANY_ID,&wifi_network_event_handler,NULL,&wifi_event_handler));

	wifi_config_t wifi_config = {
		            .ap = {
		                .ssid_len = strlen(ap_ssid),
		                .channel = ESP_AP_WIFI_CHANNEL,
		                .max_connection = ESP_AP_MAX_STA_CONN,
		                .authmode = WIFI_AUTH_WPA_WPA2_PSK
		            },
	};

	memset( &(wifi_config.ap.ssid[0]), 0, 32);
	memset( &(wifi_config.ap.password[0]), 0, 64);
	for(int i=0;i<strlen(ap_ssid);i++)
	{
		wifi_config.ap.ssid[i]=ap_ssid[i];
	}
	for(int i=0;i<strlen(ap_password);i++)
	{
		wifi_config.ap.password[i]=ap_password[i];
	}

	// setting the WiFi mode
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

	// start WiFi as per current settings
	ESP_ERROR_CHECK(esp_wifi_start());

	// set the current mode variable to AP
	wifi_current_mode = WIFI_MODE_AP;

	return ESP_OK;
}

/*
 * @brief: This function is used to stop WiFi
 *
 * @param:
 * none
 *
 * @return:
 * nothing
 */
void stop_wifi()
{
	if( wifi_current_mode == WIFI_MODE_STA )
	{
		esp_wifi_disconnect();
		esp_wifi_stop();
		esp_wifi_deinit();

		esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler);
		esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, network_event_handler);

		unregister_sta_handlers();

		wifi_sta_connected = 0;

		wifi_current_mode = WIFI_MODE_NULL;

	}
	else if( wifi_current_mode == WIFI_MODE_AP )
	{
		esp_wifi_stop();
		esp_wifi_deinit();

		esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler);
		esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, network_event_handler);

		unregister_ap_handlers();

		wifi_current_mode = WIFI_MODE_NULL;
	}
}

/*
 * @brief: This function is used to get the connection status when in station mode
 *
 * @param:
 * none
 *
 * @return: uint8_t
 * 1 connected
 * 0 unconnected
 */
uint8_t isStationConnected()
{
	return wifi_sta_connected;
}
