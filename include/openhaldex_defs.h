#define HALDEX_ID                 0x2C0
#define BRAKES1_ID                0x1A0
#define BRAKES2_ID                0x2A0
#define BRAKES3_ID                0x4A0
#define BRAKES5_ID                0x5A0
#define MOTOR1_ID                 0x280
#define MOTOR2_ID                 0x288
#define MOTOR3_ID                 0x380
#define MOTOR5_ID                 0x480
#define MOTOR6_ID                 0x488
#define MOTOR7_ID                 0x588
#define MOTOR_FLEX_ID             0x580
#define GRA_ID                    0x38A

#define NUM_LOCK_POINTS 10

#define DATA_CTRL_CHECK_LOCKPOINTS  0
#define DATA_CTRL_CLEAR             1
#define DATA_CTRL_CHECK_MODE        2

#define APP_MSG_MODE        0
#define APP_MSG_STATUS      1
#define APP_MSG_CUSTOM_DATA 2
#define APP_MSG_CUSTOM_CTRL 3

#define SERIAL_PACKET_END   0xff

#define ARRAY_SIZE(array) (sizeof((array)) / sizeof((array)[0]))

typedef struct can_s{
    byte status;
    bool inited;
    const char* name;
}can_s;

typedef enum openhaldex_mode_id{
    MODE_STOCK,
    MODE_FWD,
    MODE_5050,
    MODE_CUSTOM
}openhaldex_mode_id;

typedef struct lockpoint{
    byte speed;
    byte lock;
    byte intensity;
}lockpoint;

typedef struct openhaldex_custom_mode{
    lockpoint lockpoints[NUM_LOCK_POINTS];
    byte lockpoint_rx_h;
    byte lockpoint_rx_l;
    byte lockpoint_count;
}openhaldex_custom_mode;

typedef struct openhaldex_state {
    openhaldex_mode_id mode;
    openhaldex_custom_mode custom_mode;
    float ped_threshold;
}openhaldex_state;

typedef struct bt_packet {
    byte len;
    byte data[6];
}bt_packet;