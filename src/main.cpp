#include "openhaldex.h"

openhaldex_state state;
byte haldex_state;
byte haldex_engagement;
byte vehicle_speed;
auto timer = timer_create_default();

can_s body_can = {
    0,                          // CAN status
    false,                      // Inited flag
    "Body"                      // Name
};

can_s haldex_can = {
    0,                          // CAN status
    false,                      // Inited flag
    "Haldex"                    // Name
};

void setup()
{
    Serial.begin(115200);
    Serial.println("\nOpenHaldexTeensy init");

    pinMode(2, INPUT_PULLUP);
    if (!digitalRead(2))
    {
        Serial.println("Enter BT setup mode");
        Serial2.begin(38400);
        Serial2.write("AT+UART=115200,0,0\r\n");
        Serial2.write("AT+NAME=OpenHaldex32\r\n");
        Serial2.write("AT+RESET\r\n");
        Serial2.end();
    }

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
    service_can_events();

    if (Serial2.available())
    {
        bt_packet rx_packet = {0};
        rx_packet.len = Serial2.readBytesUntil(SERIAL_PACKET_END, rx_packet.data, ARRAY_SIZE(rx_packet.data));
        bt_process(&rx_packet);
    }
}