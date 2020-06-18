#ifndef __DEFINES_H__
#define __DEFINES_H__

#define GPIO_TIMER_INTERVAL 10 // ms

#define I2C_BUS_NUMBER      1 // 1 = Hardware I2C, 3 = Software I2C
#define I2C_SLAVE_ADDRESS   0x68
#define I2C_BUFFER_SIZE     6

#define SERIAL_PORT_NAME    "/dev/serial0"
#define SERIAL_BAUD_RATE    115200

#define AUDIO_SAMPLE_RATE   44100 // Hz
#define AUDIO_BUFFER_SIZE   1024  // frames
#define AUDIO_CHANNELS_IN   0
#define AUDIO_CHANNELS_OUT  2

#define TRACK_BUFFER_LENGTH 60 // seconds
#define TRACK_NUM_CHANNELS  2

#define NUMBER_OF_TRACKS    2

#define LEFT                0
#define RIGHT               1

typedef unsigned long long frame_t;
typedef unsigned char byte;

#endif // __DEFINES_H__
