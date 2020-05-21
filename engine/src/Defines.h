#ifndef __DEFINES_H__
#define __DEFINES_H__

#define SERIAL_PORT_NAME    "/dev/serial0"
#define SERIAL_BAUD_RATE    115200
#define SERIAL_TIMER_INT    1 // ms

#define AUDIO_SAMPLE_RATE   44100 // Hz
#define AUDIO_BUFFER_SIZE   1024  // frames

#define MIDI_TIMER_INTERVAL 1   // ms
#define MIDI_IN_BUFFER_SIZE 100 // messages

#define TRACK_BUFFER_LENGTH 60 // seconds
#define TRACK_NUM_CHANNELS  2

#define NUMBER_OF_TRACKS    2

#define LEFT                0
#define RIGHT               1

typedef unsigned long long frame_t;

#endif // __DEFINES_H__
