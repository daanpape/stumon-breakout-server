/* 
 * Copyright (c) 2015, Daan Pape
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
 * File:   stumon_longrunner.c
 * Created on December 11, 2015, 6:58 AM
 */

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <json-c/json.h>
#include <curl/curl.h>

#include "../longrunner.h"
#include "../logger.h"
#include "../config.h"
#include "../rfid/pn532/rfid_pn532.h"
#include "stumon_longrunner.h"
#include "stumon_btnlight.h"

/**
 * The StuMON longrunner initializer.
 */
void stumon_longrunner_init(void)
{
    /* Initialize the PN532 RFID reader */
    if(!rfid_pn532_init_i2c(STUMON_RFID_SDA, STUMON_RFID_SCL)) {
        log_message(LOG_INFO, "Could not initialize RFID reader for STuMON longrunner\r\n");
        return;
    }
    
    /* Check if the RFID reader can be found */
    uint32_t versiondata = rfid_pn532_get_firmware_version();
    if(!versiondata) {
        log_message(LOG_ERROR, "I2C bus initialized but could not find PN53x RFID reader\r\n");
        return;
    }
    log_message(LOG_DEBUG, "Found chip PN5%02x, firmware version: %d.%d\r\n", 
            (versiondata>>24) & 0xFF, 
            (int) (versiondata>>16) & 0xFF,
            (int) (versiondata>>8) & 0xFF);
    
    /* Add the stumon_longrunner entrypoint */
    longrunner_add(stumon_longrunner_entrypoint, 10);
    log_message(LOG_INFO, "StuMON longrunner initialised\r\n");
}

/**
 * @brief Post a tag to the Stumon server.
 * 
 * This function composes the tag post body and transmits it to the 
 * StuMON server.
 * 
 * @param tag The tag ID to post.
 * 
 * @return True on success, false on error. 
 */
static bool _stumon_longrunner_post_tag(const char* tag)
{
    CURL *curl;                             /* cURL handle */
    CURLcode res;                           /* POST result code */
    long http_resp;                         /* HTTP response code */
    bool retvalue = false;                  /* Function return value */
    char errbuff[CURL_ERROR_SIZE] = {0};    /* Buffer for cURL error messages */

    /* Initialize cURL */
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        
        /* Prepare post data buffer */
        json_object *post_body = json_object_new_object();
        json_object_object_add(post_body, "tag", json_object_new_string(tag));
        json_object_object_add(post_body, "reader_id", json_object_new_string(conf->stumon_reader_id));
        json_object_object_add(post_body, "reader_key", json_object_new_string(conf->stumon_reader_key));
        
        
        /* Set correct post options */
        curl_easy_setopt(curl, CURLOPT_URL, conf->stumon_post_tag);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_object_to_json_string(post_body));
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuff);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, CURL_USER_AGENT);
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Content-Type: application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        /* Perform the request. */
        log_message(LOG_DEBUG, "Posting tag '%s' to '%s'\r\n", tag, conf->stumon_post_tag);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            log_message(LOG_ERROR, "Could not post tag to %s: (err: #%d) %s\r\n", conf->stumon_post_tag, res, errbuff);
            goto end;
        }

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_resp);
        retvalue = http_resp == 200;

end:
        /* Cleanup additional headers */
        curl_slist_free_all(chunk);

        /* Cleanup the JSON post body */
        json_object_put(post_body);

        /* Cleanup cURL */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return retvalue;    
}

/**
 * @brief Post a score to the Stumon server.
 * 
 * This function composes the score post body and transmits it to the 
 * StuMON server.
 * 
 * @param tag The tag ID to post.
 * 
 * @return True on success, false on error. 
 */
static bool _stumon_longrunner_post_score(const char* tag, int score)
{
    CURL *curl;                             /* cURL handle */
    CURLcode res;                           /* POST result code */
    long http_resp;                         /* HTTP response code */
    bool retvalue = false;                  /* Function return value */
    char errbuff[CURL_ERROR_SIZE] = {0};    /* Buffer for cURL error messages */

    /* Initialize cURL */
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        /* Prepare post data buffer */
        json_object *post_body = json_object_new_object();
        json_object_object_add(post_body, "tag", json_object_new_string(tag));
        json_object_object_add(post_body, "score", json_object_new_int(score));
        json_object_object_add(post_body, "reader_id", json_object_new_string(conf->stumon_reader_id));
        json_object_object_add(post_body, "reader_key", json_object_new_string(conf->stumon_reader_key));
        
        /* Set correct post options */
        curl_easy_setopt(curl, CURLOPT_URL, conf->stumon_post_score);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_object_to_json_string(post_body));
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuff);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, CURL_USER_AGENT);
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Content-Type: application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        /* Perform the request. */
        log_message(LOG_DEBUG, "Posting tag '%s' to '%s'\r\n", tag, conf->stumon_post_tag);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            log_message(LOG_ERROR, "Could not post tag to %s: (err: #%d) %s\r\n", conf->stumon_post_tag, res, errbuff);
            goto end;
        }

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_resp);
        retvalue = http_resp == 200;

end:
        /* Cleanup additional headers */
        curl_slist_free_all(chunk);

        /* Cleanup the JSON post body */
        json_object_put(post_body);

        /* Cleanup cURL */
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return retvalue;    
}

/**
 * The StuMON longrunner entry point. 
 */
void stumon_longrunner_entrypoint(void)
{
    bool result = false;
    char uidstrbuf[16];
    uidstrbuf[0] = '\0';
    
    // Configure board to read RFID tags
    if(!rfid_pn532_config_SAM()) {
        log_message(LOG_ERROR, "Could not configure PN532 to read RFID tags\r\n");
    } else {
        log_message(LOG_DEBUG, "Configured SAM\r\n");
        
        uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
        uint8_t uid_len;				// Length of the UID (4 or 7 bytes depending on ISO14443A card type)

        result = rfid_pn532_read_passive_target_id(RFID_PN532_MIFARE_ISO14443A, &uid[0], &uid_len, 5000);

        if(result) {
            if(uid_len == 4) {
                sprintf(uidstrbuf, "%02x%02x%02x%02x", uid[3], uid[2], uid[1], uid[0]);
            } else {
                sprintf(uidstrbuf, "%02x%02x%02x%02x%02x%02x%02x", uid[6], uid[5], uid[4], uid[3], uid[2], uid[1], uid[0]);
            }
            uidstrbuf[uid_len*2] = '\0';   
            
            log_message(LOG_INFO, "NFC-tag found with UID: %s\r\n", uidstrbuf);
            light_status_blink();
            
            if(get_status_score()) {
                /* Post the tag and score */
                _stumon_longrunner_post_score(uidstrbuf, get_latest_score());
            } else {
                /* Just post the tag */
                _stumon_longrunner_post_tag(uidstrbuf);
            }
            
            // Wait before next read
            usleep(NFC_READ_DELAY * 1000);
        } else {
            log_message(LOG_ERROR, "Could not read NFC tag or timeout\r\n");
        }
    }
    
}