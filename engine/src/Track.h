#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>

#include "Defines.h"

using namespace std;

enum TrackState
{
    STOPPED = 0,
    PLAYING
};

class Track
{
public:
    Track(int trackIndex, int inputChannelLeft, int inputChannelRight) :
        trackIndex(trackIndex),
        // inputChannelLeft(inputChannelLeft),
        // inputChannelRight(inputChannelRight),
        trackState(STOPPED),
        frequency(500.0f),
        phase(0.0f),
        currentPhaseStep(0.0f)
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

    void setFrequency(float freq)
    {
        frequency = freq * 0.4f + frequency * 0.6f;
    }

    void process(
        const float * inputBuffer,
        float *       outputBuffer,
        frame_t       framesPerBuffer,
        int           numInputChannels,
        int           numOutputChannels,
        frame_t       currentFrame)
    {
        float targetPhaseStep = frequency / AUDIO_SAMPLE_RATE;

        for (frame_t i = 0; i < framesPerBuffer; ++i)
        {
            float sine = sin(phase * 2.0f * M_PI) * 0.5f;

            outputBuffer[i * numOutputChannels + LEFT]  += sine * ((trackIndex + 0) % 2);
            outputBuffer[i * numOutputChannels + RIGHT] += sine * ((trackIndex + 1) % 2);

            float slope     = (float) i / framesPerBuffer;
            float phaseStep = (1.0f - slope) * currentPhaseStep + slope * targetPhaseStep;

            phase = fmodf(phase + phaseStep, 1.0f);
        }

        currentPhaseStep = targetPhaseStep;

        // if (trackState == PLAYING)
        // {
        //     frame_t p;
        //
        //     for (frame_t i = 0; i < framesPerBuffer; ++i)
        //     {
        //         p = (currentFrame + i) % trackLengthFrames;
        //
        //         outputBuffer[i * numOutputChannels + LEFT] +=
        //           trackBuffer[p * TRACK_NUM_CHANNELS + LEFT];
        //         outputBuffer[i * numOutputChannels + RIGHT] +=
        //           trackBuffer[p * TRACK_NUM_CHANNELS + RIGHT];
        //     }
        // }
    }

private:
    const int trackIndex;
    // const int inputChannelLeft;
    // const int inputChannelRight;

    TrackState trackState;

    frame_t trackLengthFrames;
    frame_t trackBufferSize;
    float * trackBuffer;

    float frequency;
    float phase;
    float currentPhaseStep;
};

#endif // __TRACK_H__
