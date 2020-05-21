#ifndef __TRACK_H__
#define __TRACK_H__

#include <iostream>
#include <cmath>

#include "Defines.h"

using namespace std;

class Track
{
public:
    Track(int trackIndex, int inputChannelLeft, int inputChannelRight) :
        // trackIndex(trackIndex),
        // inputChannelLeft(inputChannelLeft),
        // inputChannelRight(inputChannelRight),
        frequency(500.0f),
        phase(0.0f),
        currentPhaseStep(0.0f)
    { }

    ~Track()
    { }

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

            outputBuffer[i * numOutputChannels + LEFT]  += sine;
            outputBuffer[i * numOutputChannels + RIGHT] += sine;

            float slope     = (float) i / framesPerBuffer;
            float phaseStep = (1.0f - slope) * currentPhaseStep + slope * targetPhaseStep;

            phase = fmodf(phase + phaseStep, 1.0f);
        }

        currentPhaseStep = targetPhaseStep;
    }

private:
    // const int trackIndex;
    // const int inputChannelLeft;
    // const int inputChannelRight;

    float frequency;
    float phase;
    float currentPhaseStep;
};

#endif // __TRACK_H__
