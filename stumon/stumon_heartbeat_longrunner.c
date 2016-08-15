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
 * File:   stumon_heartbeat_longrunner.c
 * Created on December 14, 2015, 4:27 AM
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
#include "stumon_heartbeat_longrunner.h"

/**
 * The StuMON heartbeat longrunner initializer.
 */
void stumon_heartbeat_longrunner_init(void)
{
    /* Add the stumon_heartbeat_longrunner entrypoint */
    longrunner_add(stumon_heartbeat_longrunner_entrypoint, conf->stumon_heartbeat_interval * 1000);
    log_message(LOG_INFO, "StuMON heartbeat longrunner initialised\r\n");
}

/**
 * The StuMON heartbeat longrunner entry point. 
 */
void stumon_heartbeat_longrunner_entrypoint(void)
{
    CURL *curl;                             /* cURL handle */
    CURLcode res;                           /* POST result code */
    long http_resp;                         /* HTTP response code */
    char errbuff[CURL_ERROR_SIZE] = {0};    /* Buffer for cURL error messages */

    /* Initialize cURL */
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        /* Prepare post data buffer */
        json_object *post_body = json_object_new_object();
        json_object_object_add(post_body, "reader_id", json_object_new_string(conf->stumon_reader_id));
        json_object_object_add(post_body, "reader_key", json_object_new_string(conf->stumon_reader_key));
        
        log_message(LOG_DEBUG, "Posting heartbeat to '%s'\r\n", conf->stumon_post_heartbeat);
        
        /* Set correct post options */
        curl_easy_setopt(curl, CURLOPT_URL, conf->stumon_post_heartbeat);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_object_to_json_string(post_body));
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuff);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, CURL_USER_AGENT);
        
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Content-Type: application/json;charset=UTF-8");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        /* Perform login */
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            log_message(LOG_ERROR, "Could not post heartbeat to StuMON platform: (err: #%d) %s\r\n", res, errbuff);
            goto end;
        }

        curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &http_resp);

end:
        /* Cleanup cURL */
        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}