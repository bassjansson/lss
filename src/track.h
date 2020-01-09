#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>

#include "defines.h"

using namespace std;

enum TrackState
{
    STOPPED = 0,
    PLAYING
};

class Track
{
public:
    Track(int trackIndex) :
        trackIndex(trackIndex),
        trackState(STOPPED),
        inputChannelLeft(0),
        inputChannelRight(1)
    {
        trackLengthFrames = TRACK_BUFFER_LENGTH * AUDIO_SAMPLE_RATE;
        trackBufferSize   = trackLengthFrames * TRACK_NUM_CHANNELS;
        trackBuffer       = new float[trackBufferSize];

        for (frame_t i = 0; i < trackBufferSize; ++i)
            trackBuffer[i] = 0.0f;
    }

    ~Track()
    {
        delete[] trackBuffer;
    }

    void startPlayback()
    {
        trackState = PLAYING;
    }

    void stopPlayback()
    {
        trackState = STOPPED;
    }

    void process(
        const float * inputBuffer,
        float *       outputBuffer,
        frame_t       framesPerBuffer,
        int           numInputChannels,
        int           numOutputChannels,
        frame_t       currentFrame)
    {
        if (trackState == PLAYING)
        {
            frame_t p;

            for (frame_t i = 0; i < framesPerBuffer; ++i)
            {
                p = (currentFrame + i) % trackLengthFrames;

                outputBuffer[i * numOutputChannels + LEFT] +=
                  trackBuffer[p * TRACK_NUM_CHANNELS + LEFT];
                outputBuffer[i * numOutputChannels + RIGHT] +=
                  trackBuffer[p * TRACK_NUM_CHANNELS + RIGHT];
            }
        }
    }

private:
    const int trackIndex;
    TrackState trackState;

    frame_t trackLengthFrames;
    frame_t trackBufferSize;
    float * trackBuffer;

    int inputChannelLeft;
    int inputChannelRight;
};

#endif // __TRACK_H__
