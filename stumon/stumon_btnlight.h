/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   stumon_btnlight.h
 * Author: dpape
 *
 * Created on May 31, 2016, 8:29 AM
 */

#ifndef STUMON_BTNLIGHT_H
#define STUMON_BTNLIGHT_H

#include <stdbool.h>

#define LIGHT_WIFI          6
#define LIGHT_STATUS        8
#define LIGHT_SCORE         0

#define BTN_SCORE           24
#define BTN_WIFI            21
#define BTN1                14
#define BTN2                16
#define BTN3                19
#define BTN4                18
#define BTN5                22

void set_light_wifi(bool status);

void set_light_status(bool status);

void set_light_score(bool status);

bool get_status_wifi();

bool get_status_status();

bool get_status_score();

int get_latest_score();

void light_status_blink();

void setup_button_inputs();

void stumon_btnlight_longrunner_init(void);

void stumon_btnlight_longrunner_entrypoint(void);

#endif

