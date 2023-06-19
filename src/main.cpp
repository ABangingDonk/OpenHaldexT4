#include "openhaldex.h"

openhaldex_state state;
byte haldex_state;
byte haldex_engagement;
byte vehicle_speed;
auto timer = timer_create_default();

bool print_mode(void *params)
{
    Serial.printf("OpenHaldex mode=%d\n", state.mode);
    return true;
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nOpenHaldexTeensy init");
    Serial.printf("Running at %dMHz\r\n", F_CPU_ACTUAL / (1000 * 1000));

    // Turn off the power LED for power saving
    pinMode(LED_BUILTIN, OUTPUT);

#ifndef BOARD_v0p1
    pinMode(PIN_BT_LED1, INPUT);
    pinMode(PIN_BT_LED2, INPUT);

    pinMode(PIN_LED_R, OUTPUT);
    digitalWriteFast(PIN_LED_R, 1);
    pinMode(PIN_LED_G, OUTPUT);
    digitalWriteFast(PIN_LED_G, 1);
    pinMode(PIN_LED_B, OUTPUT);
    digitalWriteFast(PIN_LED_B, 1);

    pinMode(PIN_BT_CONF, INPUT_PULLDOWN);
    delay(10); // Wait for pull to be applied
    if (digitalRead(PIN_BT_CONF))
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
#endif /* !BOARD_v0p1 */

    can_init();

    pinMode(PIN_FWD_MODE, INPUT_PULLUP);
    pinMode(PIN_5050_MODE, INPUT_PULLUP);
    delay(10);
    Serial.printf("GPIOs configured for mode override (%d, %d) (FWD, 50/50)\n", PIN_FWD_MODE, PIN_5050_MODE);

    Serial2.begin(115200);
    Serial.println("BT initialised");

    timer.every(100, bt_send_status);

#if STATE_DEBUG
    timer.every(1000, print_mode);
#endif /* STATE_DEBUG */

#if CAN_TEST_DATA
    timer.every(50, send_can_test);
    timer.every(1000, print_mb_status);
#endif

    CCM_ANALOG_PLL_AUDIO |= CCM_ANALOG_PLL_AUDIO_POWERDOWN;
    CCM_ANALOG_PLL_VIDEO |= CCM_ANALOG_PLL_VIDEO_POWERDOWN;
    CCM_ANALOG_PLL_ENET  |= CCM_ANALOG_PLL_ENET_POWERDOWN;
}

void loop()
{
    timer.tick();

    bool button_fwd = !digitalRead(PIN_FWD_MODE);
    bool button_5050 = !digitalRead(PIN_5050_MODE);

    if (button_fwd)
    {
        state.mode_override = true;
        state.mode = MODE_FWD;
    }
    else if (button_5050)
    {
        state.mode_override = true;
        state.mode = MODE_5050;
    }
    else if (state.mode_override)
    {
        state.mode = MODE_STOCK;
    }

    if (Serial2.available())
    {
        bt_packet rx_packet = {0};
        rx_packet.len = Serial2.readBytesUntil(SERIAL_PACKET_END, rx_packet.data, ARRAY_SIZE(rx_packet.data));
        bt_process(&rx_packet);
    }

#ifndef BOARD_v0p1
//#if 0
    static bool bt_led1 = false;
    static bool bt_led2 = false;
    static bool led_switch_state = false;

    if (digitalRead(PIN_BT_LED1) && !bt_led1)
    {
        analogWrite(PIN_LED_R, 240);
        led_switch_state = true;
    }
    else if (!digitalRead(PIN_BT_LED1) && bt_led1)
    {
        analogWrite(PIN_LED_R, 256);
        led_switch_state = true;
    }

    if (led_switch_state)
    {
        bt_led1 = !bt_led1;
    }

    if (digitalRead(PIN_BT_LED2) && !bt_led2)
    {
        analogWrite(PIN_LED_B, 240);
        led_switch_state = true;
    }
    else if (!digitalRead(PIN_BT_LED2) && bt_led2)
    {
        analogWrite(PIN_LED_B, 256);
        led_switch_state = true;
    }

    if (led_switch_state)
    {
        bt_led2 = !bt_led2;
    }
#endif /* !BOARD_v0p1 */
}