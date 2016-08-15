/* 
 * Copyright (c) 2014, Daan Pape
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 *     1. Redistributions of source code must retain the above copyright 
 *        notice, this list of conditions and the following disclaimer.
 *
 *     2. Redistributions in binary form must reproduce the above copyright 
 *        notice, this list of conditions and the following disclaimer in the 
 *        documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * File:   config.h
 * Created on May 9, 2014, 5:28 PM
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

/* Compiled configuration */
#define API_CALL_MAX_LEN                50                                      /* Maximum length of an API uri */
#define CONFIG_BUFF_SIZE                1024                                    /* Maximum length of a configuration line */
#define LOCAL_FIRMWARE_FILE             "/etc/dpt-firmware-version"             /* Location of the DPT-Firmware version file */ 
#define CURL_USER_AGENT                 "dptboard-agent/1.0"                    /* User agent fo the DPT-Board when accessing external services */
#define UBUS_NETWORK                    "network"                               /* ubus network daemon name */
#define UBUS_WIRELLESS                  "network.wireless"                      /* ubus wireless daemon name */ 

/* Configuration default fallback */
#define FORK_ON_START 			false                                   /* True if the server should fork on startup */
#define DB_LOCATION                     "/etc/dptechnics.db"                    /* The breakout server database file */
#define WORKING_BUFF_SIZE		4096                                    /* Size of the working buffer, should be smaller than PAGE_MAX */
#define KEEP_ALIVE_TIME			20                                      /* Time in seconds for Keep-Alive connections */
#define NETWORK_TIMEOUT			30                                      /* The number of seconds before timeout is detected */
#define INDEX_FILE                      "index.html"                            /* The default index page */
#define DOCUMENT_ROOT			"/www"                                  /* The document root */
#define API_PATH			"/api"                                  /* The API uri */
#define LISTEN_PORT			"80"                                    /* Port to listen to for incoming requests */

/* Hardware SPI settings */
#define SPI_DEVICE			"/dev/spidev0.1"                        /* The SPI device the server should use */
#define SPI_DEFAULT_BITS		8                                       /* Default number of bits per word */
#define SPI_DEFAULT_SPEED		250000                                  /* Default bus speed of 250kHz */

/* Alfa IO module port settings */
#define ALFA_STROBE_PORT		24                                      /* The AlfaSprint IO module strobe port */
#define ALFA_ENABLE_PORT		20                                      /* The AlfaSprint IO module enable port */
#define ALFA_SH_LD_PORT			22                                      /* The AlfaSprint IO module shift/load port */
#define ALFA_CLK_INH_PORT		18                                      /* The AlfaSprint IO module clock inhibit port */

/* SSD1306 Screen settings */
#define SCREEN_SSD1306_DC		19                                      /* SSD1306 screen controller Data Command (DC) line */
#define SCREEN_SSD1303_RES		0                                       /* SSD1306 screen controller Reset (RES) line */

/* Wireless device settings */
#define WLAN_DEVICE                     "wlan0"                                 /* Use wlan0 as wireless device */
#define WLAN_DEVICE_ALT                 "wlan0-1"                               /* Use wlan0-1 as alternative device */

/* UBUS configuration */
#define UBUS_TIMEOUT                    1000                                    /* U-bus timeout in milliseconds */                      

/* BlueCherry configuration */
#define BLUECHERRY_CONFIG_FILE          "/etc/config/dpt-connector"             /* The dpt-connector configuration file */

/* StuMON configuration */
#define STUMON_HEARTBEAT_INTERVAL       10                                      /* The number of seconds between heartbeat intervals */
#define STUMON_RFID_SDA                 23                                      /* The StuMON RFID reader SDA line */
#define STUMON_RFID_SCL                 20                                      /* The StuMON RFID reader SCL line */ 
#define STUMON_RFID_IRQ                 19                                      /* The StuMON RFID reader IRQ line */
#define STUMON_RFID_RST                 6                                       /* The StuMON RFID reader RST line */

/* Dynamic configuration structure */
typedef struct{
    bool daemon;                            /* When true the breakout server will run as a daemon */
    
    const char* listen_port;                /* Port to listen to for incoming requests */
    const char* database;                         /* The database file to use */
    int keep_alive_time;                    /* Time in seconds for Keep-Alive connections */
    int network_timeout;                    /* The number of seconds before timeout is detected */
    int max_connections;                    /* The maximum number of connections to this server */
    
    const char* index_file;                 /* The file that is served by default */
    const char* document_root;              /* The document root */
    const char* api_prefix;                 /* The API URI prefix, must start with slash */
    ssize_t api_str_len;                    /* The length of the API URI prefix */
    
    int ubus_timeout;                       /* Timeout in msecs for ubus communication */   
    bool no_symlinks;                       /* True if symlinks should not be followed */
    
    const char* stumon_post_heartbeat;      /* The API URI to post the heartbeats to */
    const char* stumon_post_tag;            /* The API URI to post the tags to */
    const char* stumon_post_score;          /* The API URI to post a score to */
    int stumon_heartbeat_interval;          /* Send a heartbeat frame every interval seconds */
    
    const char* stumon_reader_id;           /* The ID of the key */
    const char* stumon_reader_key;          /* The API key of the reader */
} config;

/* Application wide configuration */
extern config* conf;

bool config_parse(const char *configfile);

void config_free();
#endif
