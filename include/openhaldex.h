/*
 * MIT License
 * 
 * Copyright (c) 2022 ABangingDonk
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Arduino.h>
#include <SPI.h>
#include "openhaldex_defs.h"
#include <stdio.h>
#include <arduino-timer.h>
#include <FlexCAN_T4.h>

/* Defines */
#define CAN1_DEBUG 0
#define CAN2_DEBUG 0
#define CAN_TEST_DATA 0
#define STATE_DEBUG 0

//#define BOARD_v0p1
//#define BOARD_v0p2
#define BOARD_v0p3

#define PIN_BT_CONF     4
#define PIN_LED_R       10
#define PIN_LED_G       11
#define PIN_LED_B       12
#define PIN_BT_LED2     14
#define PIN_BT_LED1     15
#if defined(BOARD_v0p1)
#define PIN_FWD_MODE    7
#define PIN_5050_MODE   8
#elif defined(BOARD_v0p2) || defined(BOARD_v0p3)
#define PIN_5050_MODE   16
#define PIN_FWD_MODE    17
#else
#error "Unknown board"
#endif


/* Globals */
extern openhaldex_state state;
extern byte vehicle_speed;
extern byte dummy_vehicle_speed;
extern byte haldex_engagement;
extern byte haldex_state;
extern float lock_target;
extern float ped_value;

/* Functions */
extern bool bt_send_status(void *params);
extern void bt_process(bt_packet *rx_packet);
extern bool send_can_test(void* params);
extern void haldex_can_rx_callback(const CAN_message_t &frame);
extern void body_can_rx_callback(const CAN_message_t &frame);
extern void get_lock_data(CAN_message_t *frame);
extern void can_init(void);
extern void service_can_events(void);
extern bool print_mb_status(void *params);