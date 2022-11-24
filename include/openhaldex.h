#include <Arduino.h>
#include <SPI.h>
#include "openhaldex_defs.h"
#include <stdio.h>
#include <arduino-timer.h>
#include <FlexCAN_T4.h>

/* Defines */
#define CAN1_DEBUG 1
#define CAN2_DEBUG 1
#define CAN_TEST_DATA 1
#define STATE_DEBUG 1

/* Globals */
extern openhaldex_state state;
extern byte vehicle_speed;
extern byte dummy_vehicle_speed;
extern byte haldex_engagement;
extern byte haldex_state;
extern float lock_target;
extern float ped_value;
extern can_s body_can;
extern can_s haldex_can;

/* Functions */
extern bool bt_send_status(void *params);
extern void bt_process(bt_packet *rx_packet);
#ifdef CAN_TEST_DATA
extern bool send_can_test(void* params);
#endif
extern void haldex_can_rx_callback(const CAN_message_t &frame);
extern void body_can_rx_callback(const CAN_message_t &frame);
extern void get_lock_data(CAN_message_t *frame);
extern void can_init(void);
extern void service_can_events(void);
extern bool print_mb_status(void *params);