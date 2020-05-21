#ifndef __DEFINES_H__
#define __DEFINES_H__

#define AUDIO_SAMPLE_RATE   44100 // Hz
#define AUDIO_BUFFER_SIZE   1024  // frames

#define MIDI_TIMER_INTERVAL 1   // ms
#define MIDI_IN_BUFFER_SIZE 100 // messages

#define NUMBER_OF_TRACKS    1

#define LEFT                0
#define RIGHT               1

typedef unsigned long long frame_t;

#endif // __DEFINES_H__
