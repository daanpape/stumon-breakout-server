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
 * File:   config.c
 * Created on March 14, 2015, 11:29 PM
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <json-c/json.h>

#include "config.h"
#include "logger.h"

/** The application wide configuration structure */
config cfg = { 0 };
config* conf = &cfg;

/** The underlying JSON object which holds the configuration */
json_object *j_config;

/**
 * @brief This function parses a the JSON configuration file.
 * 
 * This function tries to parse a JSON configuration file with the following
 * structure:
 * 
 * @code
 * {
 *     "daemonize" : true/false,
 *     "listen_port": <port>,
 *     "keep_alive_time" : <keep alive time>,
 *     "network_timeout" : <network timeout>,
 * 
 *     "index_file" : "index.html",
 *     "document_root" : "/www",
 *     "api_prefix" : "/apiv1",
 * 
 *     "database_path" : "/path/to/db",
 * 
 *     "stumon_post_heartbeat" : "https://stumon.dptechnics.com/devapi/v1/heartbeat",
 *     "stumon_post_tag" : "https://stumon.dptechnics.com/devapi/v1/tag",
 *     "stumon_post_score" : "https://stumon.dptechnics.com/devapi/v1/score",
 * 
 *     "stumon_reader_id" : "BRU001",
 *     "stumon_reader_key" : "r8jjNVfNYsLReXxrAY3ah6H7"
 * }
 * @endcode
 * 
 * @return True when the configuration parsed successfully.
 */
bool config_parse(const char *configfile) {
    json_object *j_config = json_object_from_file(configfile);
    if(j_config == NULL) {
        fprintf(stderr, "Could not read configuration file: %s\n", configfile);
        return false;
    }
    
    /* Put in default configuration */
    conf->ubus_timeout = UBUS_TIMEOUT;
    conf->stumon_heartbeat_interval = STUMON_HEARTBEAT_INTERVAL;
    
    json_object *j_daemon;
    json_object *j_listen_port;
    json_object *j_database;
    json_object *j_keep_alive_time;
    json_object *j_network_timeout;
    json_object *j_index_file;
    json_object *j_document_root;
    json_object *j_api_prefix;
    json_object *j_stumon_post_tag;
    json_object *j_stumon_post_heartbeat;
    json_object *j_stumon_get_score;
    json_object *j_reader_id;
    json_object *j_reader_key;
    
    if(!json_object_object_get_ex(j_config, "daemonize", &j_daemon) ||
       !json_object_object_get_ex(j_config, "listen_port", &j_listen_port) ||
       !json_object_object_get_ex(j_config, "database_path", &j_database) ||
       !json_object_object_get_ex(j_config, "keep_alive_time", &j_keep_alive_time) ||
       !json_object_object_get_ex(j_config, "network_timeout", &j_network_timeout) ||
       !json_object_object_get_ex(j_config, "index_file", &j_index_file) ||
       !json_object_object_get_ex(j_config, "document_root", &j_document_root) ||
       !json_object_object_get_ex(j_config, "api_prefix", &j_api_prefix) ||
       !json_object_object_get_ex(j_config, "stumon_post_tag", &j_stumon_post_tag) ||
       !json_object_object_get_ex(j_config, "stumon_post_heartbeat", &j_stumon_post_heartbeat) ||
       !json_object_object_get_ex(j_config, "stumon_post_score", &j_stumon_get_score) ||
       !json_object_object_get_ex(j_config, "stumon_reader_id", &j_reader_id) ||
       !json_object_object_get_ex(j_config, "stumon_reader_key", &j_reader_key))
    {
        
        json_object_put(j_config);
    }
    
    conf->daemon = json_object_get_boolean(j_daemon);
    
    conf->listen_port = json_object_get_string(j_listen_port);
    conf->database = json_object_get_string(j_database);
    conf->keep_alive_time = json_object_get_int(j_keep_alive_time);
    conf->network_timeout = json_object_get_int(j_network_timeout);
    
    conf->index_file = json_object_get_string(j_index_file);
    conf->document_root = json_object_get_string(j_document_root);
    conf->api_prefix = json_object_get_string(j_api_prefix);
    conf->api_str_len = strlen(conf->api_prefix) + 1;
    
    conf->stumon_post_tag = json_object_get_string(j_stumon_post_tag);
    conf->stumon_post_heartbeat = json_object_get_string(j_stumon_post_heartbeat);
    conf->stumon_post_score = json_object_get_string(j_stumon_get_score);
    
    conf->stumon_reader_id = json_object_get_string(j_reader_id);
    conf->stumon_reader_key = json_object_get_string(j_reader_key);
    
    return true;
}

/**
 * @brief Free the configuration data
 * 
 * This function releases the configuration memory.
 * 
 * @return None.
 */
void config_free() {
    json_object_put(j_config);
}
