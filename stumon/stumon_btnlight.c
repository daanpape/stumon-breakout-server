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
 * File:   stumon_btnlight.c
 * Created on December 14, 2015, 4:24 AM
 */

#include <stdio.h>

#include "stumon_btnlight.h"
#include "../gpio/gpio.h"
#include "../logger.h"
#include "../longrunner.h"

/**
 * @brief True when the WiFi light is on.
 */
bool status_wifi = false;

/**
 * @brief True when the status light is on.
 */
bool status_status = false;

/**
 * @brief True when the score light is on.
 */
bool status_score = false;
int _status_score_last = GPIO_LOW;

/**
 * @brief The latest selected score.
 */
int last_score = 1;

/**
 * @brief Set the WiFi status light.
 * 
 * This function set's the state of the WiFi light.
 * 
 * @param status True to turn the light on.
 * 
 * @return None.
 */
void set_light_wifi(bool status)
{
    status_wifi = status;
    
    if(status) {
        gpio_write_and_close(LIGHT_WIFI, GPIO_HIGH);
    } else {
        gpio_write_and_close(LIGHT_WIFI, GPIO_LOW);
    }
}

/**
 * @brief Set the status light.
 * 
 * This function set's the state of the status light.
 * 
 * @param status True to turn the light on.
 * 
 * @return None.
 */
void set_light_status(bool status)
{
    status_status = status;
    
    if(status) {
        gpio_write_and_close(LIGHT_STATUS, GPIO_HIGH);
    } else {
        gpio_write_and_close(LIGHT_STATUS, GPIO_LOW);
    }
}

/**
 * @brief Set the score status light.
 * 
 * This function set's the state of the score light.
 * 
 * @param status True to turn the light on.
 * 
 * @return None.
 */
void set_light_score(bool status)
{
    status_score = status;
    
    if(status) {
        gpio_write_and_close(LIGHT_SCORE, GPIO_HIGH);
    } else {
        gpio_write_and_close(LIGHT_SCORE, GPIO_LOW);
    }
}

/**
 * @brief Get the WiFi status.
 * 
 * This function returns the WiFi status.
 * 
 * @return True when the WiFi is connected, false else.
 */
bool get_status_wifi()
{
    return status_wifi;
}

/**
 * @brief Get the scan status.
 * 
 * This function returns the scan status.
 * 
 * @return True when the scan light is on, false else.
 */
bool get_status_status()
{
    return status_status;
}

/**
 * @brief Get the score status.
 * 
 * This function returns the score status.
 * 
 * @return True when the score modus is selected, false else.
 */
bool get_status_score()
{
    return status_score;
}

/**
 * @brief Get the last score.
 * 
 * This function returns the last selected score.
 * 
 * @return The last score.
 */
int get_latest_score()
{
    return last_score;
}

/**
 * @brief Blink the status light for 250ms.
 * 
 * This function blinks the status light for 250ms.
 * 
 * @return None.
 */
void light_status_blink()
{
    gpio_pulse(LIGHT_STATUS, 250000, GPIO_ACT_HIGH);
}

/**
 * @brief Initialize the buttons.
 * 
 * This function initializes the input buttons.
 * 
 * @return None.
 */
void setup_button_inputs()
{
    gpio_release(BTN1);
    gpio_release(BTN2);
    gpio_release(BTN3);
    gpio_release(BTN4);
    gpio_release(BTN5);
    gpio_release(BTN_WIFI);
    gpio_release(BTN_SCORE);
    
    gpio_reserve(BTN1);
    gpio_reserve(BTN2);
    gpio_reserve(BTN3);
    gpio_reserve(BTN4);
    gpio_reserve(BTN5);
    gpio_reserve(BTN_WIFI);
    gpio_reserve(BTN_SCORE);
    
    gpio_set_direction(BTN1, GPIO_IN);
    gpio_set_direction(BTN2, GPIO_IN);
    gpio_set_direction(BTN3, GPIO_IN);
    gpio_set_direction(BTN4, GPIO_IN);
    gpio_set_direction(BTN5, GPIO_IN);
    gpio_set_direction(BTN_WIFI, GPIO_IN);
    gpio_set_direction(BTN_SCORE, GPIO_IN);
}

/**
 * @brief Initialize the btn_light longrunner.
 * 
 * This function initializes the buttons and lights and adds their control
 * loop to the program.
 * 
 * @return None.
 */
void stumon_btnlight_longrunner_init(void)
{
    setup_button_inputs();
    
    /* Add the stumon button and light longrunner entrypoint */
    longrunner_add(stumon_btnlight_longrunner_entrypoint, 50);
    log_message(LOG_INFO, "StuMON button and light longrunner initialised\r\n");
}

/**
 * @brief The button and lights longrunner entrypoint.
 * 
 * This function processes buttons presses and switches lights on and off
 * based on the boolean statuses.
 * 
 * @return None.
 */
void stumon_btnlight_longrunner_entrypoint(void)
{
    /* Save the score */
    if(gpio_get_state(BTN1) == GPIO_HIGH) {
        last_score = 1;
    } else if (gpio_get_state(BTN2) == GPIO_HIGH) {
        last_score = 2;
    } else if (gpio_get_state(BTN3) == GPIO_HIGH) {
        last_score = 3;
    } else if (gpio_get_state(BTN4) == GPIO_HIGH) {
        last_score = 4;
    } else if (gpio_get_state(BTN5) == GPIO_HIGH) {
        last_score = 5;
    }
    
    if(gpio_get_state(BTN_SCORE) == GPIO_HIGH) {
        if(_status_score_last == GPIO_LOW) {
            status_score = !status_score;
            set_light_score(status_score);
        }
        _status_score_last = GPIO_HIGH;
    } else {
        _status_score_last = GPIO_LOW;
    }
}