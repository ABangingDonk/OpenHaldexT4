#include "openhaldex.h"

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;

#if !CAN_FIFO
#define NUM_RX_MB_HALDEX 8
#define NUM_TX_MB_HALDEX 8
#define NUM_RX_MB_BODY 8
#define NUM_TX_MB_BODY 8
#endif

void can_init(void)
{
    Can1.begin();
    Can1.setClock(CLK_60MHz);
    Can1.setBaudRate(500000);
    Can1.setMaxMB(16);
    Can1.onReceive(haldex_can_rx_callback);
#if CAN_FIFO
    Can1.enableFIFO();
    Can1.setFIFOFilter(REJECT_ALL);
    Can1.setFIFOFilter(0, HALDEX_ID, STD);
    Can1.enableFIFOInterrupt();
#else
    for (int i = 0; i<NUM_RX_MB_HALDEX; i++)
    {
        Can1.setMB((FLEXCAN_MAILBOX)i,RX);
    }
    for (int i = NUM_RX_MB_HALDEX; i<(NUM_TX_MB_HALDEX + NUM_RX_MB_HALDEX); i++)
    {
        Can1.setMB((FLEXCAN_MAILBOX)i,TX);
    }
    Can1.enableMBInterrupts();
#endif
    Can1.mailboxStatus();


    Can2.begin();
    Can2.setClock(CLK_60MHz);
    Can2.setBaudRate(500000);
    Can2.setMaxMB(16);
    Can2.onReceive(body_can_rx_callback);
#if CAN_FIFO
    Can2.enableFIFO();
    Can2.enableFIFOInterrupt();
#else
    for (int i = 0; i<NUM_RX_MB_BODY; i++)
    {
        Can2.setMB((FLEXCAN_MAILBOX)i,RX);
    }
    for (int i = NUM_RX_MB_BODY; i<(NUM_TX_MB_BODY + NUM_RX_MB_BODY); i++)
    {
        Can2.setMB((FLEXCAN_MAILBOX)i,TX);
    }
    Can2.enableMBInterrupts();
#endif
    Can2.mailboxStatus();

#if 0
    Can1.setRXFilter(0, HALDEX_ID, 0x7ff, false);
    Can1.setCallback(0, haldex_can_rx_callback);
    Can1.mailbox_set_mode(1,3);
    Can1.mailbox_set_mode(2,3);
    Can1.mailbox_set_mode(3,3);
    Can1.mailbox_set_mode(4,3);
    Can1.mailbox_set_mode(5,3);
    Can1.mailbox_set_mode(6,3);
    Can1.mailbox_set_mode(7,3);
    
    Can2.setRXFilter(0, 0, 0, false);
    Can2.setCallback(0, body_can_rx_callback);
    Can2.setRXFilter(1, 0, 0, false);
    Can2.setCallback(1, body_can_rx_callback);
    Can2.setRXFilter(2, 0, 0, false);
    Can2.setCallback(2, body_can_rx_callback);
    Can2.setRXFilter(3, 0, 0, false);
    Can2.setCallback(3, body_can_rx_callback);
    Can2.setRXFilter(4, 0, 0, false);
    Can2.setCallback(4, body_can_rx_callback);
    Can2.setRXFilter(5, 0, 0, false);
    Can2.setCallback(5, body_can_rx_callback);
    Can2.setRXFilter(6, 0, 0, false);
    Can2.setCallback(6, body_can_rx_callback);
    Can2.mailbox_set_mode(7,3);
    
    Can1.disable_tx_repeat();
    Can2.disable_tx_repeat();
#endif
}

void service_can_events(void)
{
    Can1.events();
    Can2.events();
}

bool print_mb_status(void *params)
{

#if CAN1_DEBUG
    Can1.mailboxStatus();
#endif
#if CAN2_DEBUG
    Can2.mailboxStatus();
#endif

    return true;
}

void haldex_can_rx_callback(const CAN_message_t &frame)
{
#if CAN1_DEBUG
    if(1)//frame.buf[0])
    {
        Serial.printf("[HALDEX RX]ID: 0x%x DATA: ", frame.id);
        for(int i = 0; i < frame.len; i++)
        {
            Serial.printf("%02x ", frame.buf[i]);
        }
        Serial.println();
    }
#endif

    haldex_state = frame.buf[0];
    haldex_engagement = frame.buf[1];

#if !CAN_TEST_DATA
    CAN_message_t frame_out;
    frame_out.id = frame.id;
    frame_out.flags = frame.flags;
    frame_out.len = frame.len;
    if (frame.len <= ARRAY_SIZE(frame_out.buf))
    {
        memcpy(frame_out.buf, frame.buf, frame.len);
    }

    // From haldex to car, just forward whatever we receive.
    if (!Can2.write(frame_out))
    {
        Serial.println("Body CAN TX fail");
        Can2.mailboxStatus();
    }
#endif
}

void body_can_rx_callback(const CAN_message_t &frame)
{
    CAN_message_t frame_out;
    frame_out.id = frame.id;
    frame_out.flags = frame.flags;
    frame_out.len = frame.len;
    if (frame.len <= ARRAY_SIZE(frame_out.buf))
    {
        memcpy(frame_out.buf, frame.buf, frame.len);
    }

#if CAN2_DEBUG
    if(1)//frame->id == MOTOR1_ID)
    {
        Serial.printf("[BODY RX]ID: 0x%x DATA: ", frame.id);
        for(int i = 0; i < frame.len; i++)
        {
            Serial.printf("%02x ", frame.buf[i]);
        }
        Serial.println();
    }
#endif

    // Anything which has come from the car.. i.e. everything except Haldex
    switch(frame.id)
    {
        case MOTOR1_ID:
            ped_value = frame.buf[5] * 0.4;
            
            if (state.mode == MODE_FWD)
            {
                memset(frame_out.buf, 0, frame_out.len);
            }
            break;
        case MOTOR2_ID:
            int calc_speed = (frame.buf[3] * 100 * 128) / 10000;
            vehicle_speed = (byte)(calc_speed >= 255 ? 255 : calc_speed);
            break;
    }

    if(state.mode == MODE_5050 || state.mode == MODE_CUSTOM)
    {
        get_lock_data(&frame_out);
    }

#if !CAN_TEST_DATA
    if (!Can1.write(frame_out))
    {
        Serial.println("Haldex CAN TX fail");
        Can1.mailboxStatus();
    }
#endif
}

byte dummy_vehicle_speed = 0x00;
#if CAN_TEST_DATA
static byte dummy_haldex_engagement = 0x00;
bool send_can_test(void* params)
{
    CAN_message_t frame;

    frame.id = MOTOR1_ID;
    frame.len = 8;
    frame.buf[0] = 0;
    frame.buf[1] = 0xf0;
    frame.buf[2] = 0x20;
    frame.buf[3] = 0x4e;
    frame.buf[4] = 0xf0;
    frame.buf[5] = 0xf0;
    frame.buf[6] = 0x20;
    frame.buf[7] = 0xf0;
    Can1.write(frame);

    CAN_message_t motor2;
    motor2.id = MOTOR2_ID;
    motor2.len = 4;
    motor2.buf[3] = dummy_vehicle_speed++;
    Can1.write(motor2);
#if 0
    
    frame.id = MOTOR3_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);

    frame.id = MOTOR5_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);

    frame.id = MOTOR6_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);

    frame.id = MOTOR7_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);

    frame.id = MOTOR_FLEX_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);
    
    frame.id = BRAKES1_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);
    
    frame.id = BRAKES2_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);
    
    frame.id = BRAKES3_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);
    
    frame.id = BRAKES5_ID;
    frame.length = 1;
    frame.data.bytes[0] = 0xff;
    Can1.sendFrame(frame);
#endif

    CAN_message_t haldex;
    haldex.id = HALDEX_ID;
    haldex.len = 2;
    haldex.buf[0] = 0x00;
    haldex.buf[1] = dummy_haldex_engagement++;
    Can2.write(haldex);

    return true;
}
#endif