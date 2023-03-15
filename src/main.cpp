#include "openhaldex.h"

openhaldex_state state;
byte haldex_state;
byte haldex_engagement;
byte vehicle_speed;
auto timer = timer_create_default();

#if BUTTONS_HEADLESS
bool print_mode(void *params)
{
    Serial.printf("OpenHaldex mode=%d\n", state.mode);
    return true;
}
#endif

void setup()
{
    Serial.begin(115200);
    Serial.println("\nOpenHaldexTeensy init");

#if defined(__IMXRT1062__)
    Serial.printf("Running at %dMHz\r\n", F_CPU_ACTUAL / (1000 * 1000));
#endif

#ifdef BOARD_v0p2
    pinMode(15, INPUT);
    pinMode(14, INPUT);

    pinMode(12, OUTPUT);
    digitalWriteFast(12, 1);
    pinMode(11, OUTPUT);
    digitalWriteFast(11, 1);
    pinMode(10, OUTPUT);
    digitalWriteFast(10, 1);

    pinMode(4, INPUT_PULLDOWN);
    delay(10);
    if (digitalRead(4))
    {
        uint8_t at_buf[32] = {0};
        Serial.println("Enter BT setup mode");
        Serial2.begin(38400);

        Serial2.write("AT\r\n");
        Serial.println("AT");
        while (!Serial2.available()){}
        Serial2.readBytesUntil('\r', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);

        Serial2.write("AT+UART=115200,0,0\r\n");
        Serial.println("AT+UART=115200,0,0");
        while (!Serial2.available()){}
        Serial2.readBytesUntil('\n', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);
        
        Serial2.write("AT+NAME=OpenHaldexT4\r\n");
        Serial.println("AT+NAME=OpenHaldexT4");
        while (!Serial2.available()){}
        Serial2.readBytesUntil('\n', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);

        Serial2.write("AT+RESET\r\n");
        Serial.println("AT+RESET");
        while (!Serial2.available()){}
        Serial2.readBytesUntil('\n', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);

        Serial2.end();
    }
#endif /* BT_CONF_EN */

#if BUTTONS_HEADLESS
    pinMode(BUTTON_FWD, INPUT_PULLUP);
    pinMode(BUTTON_5050, INPUT_PULLUP);
    delay(10);
    Serial.printf("GPIOs configured for headless mode (%d, %d) (FWD, 50/50)\n", BUTTON_FWD, BUTTON_5050);

#if STATE_DEBUG
    timer.every(1000, print_mode);
#endif /* STATE_DEBUG */

#else /* BUTTONS_HEADLESS */
    Serial2.begin(115200);
    Serial.println("BT initialised");

    timer.every(100, bt_send_status);
#endif /* BUTTONS_HEADLESS */


    can_init();
#if CAN_TEST_DATA
    timer.every(50, send_can_test);
    timer.every(1000, print_mb_status);
#endif

}

void loop()
{
    timer.tick();

    if (Serial2.available())
    {
        bt_packet rx_packet = {0};
        rx_packet.len = Serial2.readBytesUntil(SERIAL_PACKET_END, rx_packet.data, ARRAY_SIZE(rx_packet.data));
        bt_process(&rx_packet);
    }

#if BUTTONS_HEADLESS
    bool button_fwd = !digitalRead(BUTTON_FWD);
    bool button_5050 = !digitalRead(BUTTON_5050);
    analogWrite(10, 240);

    if (button_fwd)
    {
        state.mode = MODE_FWD;
    }
    else if (button_5050)
    {
        state.mode = MODE_5050;
    }
    else
    {
        state.mode = MODE_STOCK;
    }
#else
    static bool bt_led1 = false;
    static bool bt_led2 = false;
    static bool led_switch_state = false;

    if (digitalRead(15) && !bt_led1)
    {
        analogWrite(10, 240);
        led_switch_state = true;
    }
    else if (!digitalRead(15) && bt_led1)
    {
        analogWrite(10, 256);
        led_switch_state = true;
    }

    if (led_switch_state)
    {
        bt_led1 = !bt_led1;
    }

    if (digitalRead(14) && !bt_led2)
    {
        analogWrite(12, 240);
        led_switch_state = true;
    }
    else if (!digitalRead(14) && bt_led2)
    {
        analogWrite(12, 256);
        led_switch_state = true;
    }

    if (led_switch_state)
    {
        bt_led2 = !bt_led2;
    }
#endif
}