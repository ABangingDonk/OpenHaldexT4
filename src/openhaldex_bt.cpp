#include "openhaldex.h"

bool bt_send_status(void *params)
{
    bt_packet packet;
    packet.data[0] = APP_MSG_STATUS;
    packet.data[1] = haldex_state;
    packet.data[2] = haldex_engagement;
    packet.data[3] = lock_target;
    packet.data[4] = vehicle_speed;
    packet.data[5] = SERIAL_PACKET_END;
    packet.len = 6;

    for (int i = 0; i < packet.len - 1; i++)
    {
        if (packet.data[i] == SERIAL_PACKET_END)
        {
            packet.data[i] = SERIAL_PACKET_END - 1;
        }
    }

    Serial2.write(packet.data, packet.len);

    return true;
}

void bt_process(bt_packet *rx_packet)
{
    byte lockpoint_index;
    bt_packet tx_packet;
    switch (rx_packet->data[0])
    {
        case APP_MSG_MODE:
            state.mode = rx_packet->data[1] <= MODE_CUSTOM ? (openhaldex_mode_id)rx_packet->data[1]
                                                           : MODE_STOCK;
            state.ped_threshold = rx_packet->data[2];
            if (state.mode == MODE_FWD)
            {
                lock_target = 0;
            }
#if STATE_DEBUG
            Serial.printf("APP_MSG_MODE: mode=%d ped_threshold=%d%%\n", rx_packet->data[1], rx_packet->data[2]);
#endif
            break;
        case APP_MSG_CUSTOM_DATA:
            lockpoint_index = rx_packet->data[1];
            if (lockpoint_index < NUM_LOCK_POINTS)
            {
                state.custom_mode.lockpoints[lockpoint_index].speed = rx_packet->data[2];
                state.custom_mode.lockpoints[lockpoint_index].lock = rx_packet->data[3];
                state.custom_mode.lockpoints[lockpoint_index].intensity = rx_packet->data[4];

                if (lockpoint_index > 6)
                {
                    state.custom_mode.lockpoint_rx_h |= (1 << (lockpoint_index - 7));
                }
                else
                {
                    state.custom_mode.lockpoint_rx_l |= (1 << lockpoint_index);
                }
                state.custom_mode.lockpoint_count++;
#if STATE_DEBUG
                Serial.printf("lockpoint[%d] low 0x%x high 0x%x (count %d)\n",
                                    lockpoint_index,
                                    state.custom_mode.lockpoint_rx_l,
                                    state.custom_mode.lockpoint_rx_h,
                                    state.custom_mode.lockpoint_count);
#endif
            }
            break;
        case APP_MSG_CUSTOM_CTRL:
            switch (rx_packet->data[1])
            {
                case DATA_CTRL_CHECK_LOCKPOINTS:
                    tx_packet.data[0] = APP_MSG_CUSTOM_CTRL;
                    tx_packet.data[1] = DATA_CTRL_CHECK_LOCKPOINTS;
                    tx_packet.data[2] = state.custom_mode.lockpoint_rx_l;
                    tx_packet.data[3] = state.custom_mode.lockpoint_rx_h;
                    tx_packet.data[4] = SERIAL_PACKET_END;
                    tx_packet.len = 5;

                    Serial2.write(tx_packet.data, tx_packet.len);
                    break;
                case DATA_CTRL_CLEAR:
                    state.custom_mode.lockpoint_rx_l = 0;
                    state.custom_mode.lockpoint_rx_h = 0;
                    state.custom_mode.lockpoint_count = 0;
                    memset(state.custom_mode.lockpoints, 0, sizeof(state.custom_mode.lockpoints));
                    break;
                case DATA_CTRL_CHECK_MODE:
                    tx_packet.data[0] = APP_MSG_CUSTOM_CTRL;
                    tx_packet.data[1] = DATA_CTRL_CHECK_MODE;
                    tx_packet.data[2] = state.mode;
                    tx_packet.data[3] = state.ped_threshold;
                    tx_packet.data[4] = SERIAL_PACKET_END;
                    tx_packet.len = 5;

                    Serial2.write(tx_packet.data, tx_packet.len);
                    break;
            }
            break;
    }
}
