#include "openhaldex.h"

openhaldex_state state;
byte haldex_state;
byte haldex_engagement;
byte vehicle_speed;
auto timer = timer_create_default();

void setup()
{
    Serial.begin(115200);
    Serial.println("\nOpenHaldexTeensy init");

#if defined(__IMXRT1062__)
    Serial.printf("Running at %dMHz\r\n", F_CPU_ACTUAL / (1000 * 1000));
#endif

#if BT_CONF_EN
    pinMode(4, INPUT_PULLUP);
    delay(10);
    if (!digitalRead(4))
    {
        uint8_t at_buf[32] = {0};
        int at_len = 0;
        Serial.println("Enter BT setup mode");
        Serial2.begin(38400);

        Serial2.write("AT\r\n");
        Serial.println("AT");
        while (!Serial2.available()){}
        at_len = Serial2.readBytesUntil('\r', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        //Serial.write(at_buf, at_len);
        delay(1000);

        Serial2.write("AT+UART=115200,0,0\r\n");
        Serial.println("AT+UART=115200,0,0");
        while (!Serial2.available()){}
        at_len = Serial2.readBytesUntil('\n', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);
        
        Serial2.write("AT+NAME=OpenHaldexT4\r\n");
        Serial.println("AT+NAME=OpenHaldexT4");
        while (!Serial2.available()){}
        at_len = Serial2.readBytesUntil('\n', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);

        Serial2.write("AT+RESET\r\n");
        Serial.println("AT+RESET");
        while (!Serial2.available()){}
        at_len = Serial2.readBytesUntil('\n', at_buf, ARRAY_SIZE(at_buf));
        Serial.printf("%s\r\n", at_buf);
        delay(1000);

        Serial2.end();
    }
#endif /* BT_CONF_EN */

    Serial2.begin(115200);
    Serial.println("BT initialised");

    can_init();

    timer.every(100, bt_send_status);

#if CAN_TEST_DATA
    timer.every(50, send_can_test);
    timer.every(1000, print_mb_status);
#endif

}

void loop()
{
    timer.tick();
    //service_can_events();

    if (Serial2.available())
    {
        bt_packet rx_packet = {0};
        rx_packet.len = Serial2.readBytesUntil(SERIAL_PACKET_END, rx_packet.data, ARRAY_SIZE(rx_packet.data));
        bt_process(&rx_packet);
    }
}